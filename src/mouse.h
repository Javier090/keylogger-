#pragma once

#include <Windows.h>

namespace mouse {
    // Initialize mouse hooks
    void set_hook();
    
    std::wstring get_position();

    bool should_log_movement();
} 
