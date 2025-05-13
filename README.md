# Keylogger

Beta Advanced Keylogger made in C++ with enhanced monitoring capabilities

## Requirements

Visual Studio 2017 or newer is required to load the solution, though the project may be compiled by any C++ 11 compiler.
CURL library is required for remote logging functionality.

## Features

- Keyboard keypress logging with extensive key support
- Mouse click and movement tracking
- Timestamps for all recorded activities
- Active window context tracking
- Automated startup via Windows registry
- Remote logging capability (send logs to server)
- Memory management for extended runtime
- Log file rotation to prevent excessive file growth
- Crash recovery and resilience
- Complete stealth mode operation
- Clipboard monitoring
- Multi-language keyboard layout support
- Special characters and numeric keypad support

## Configuration

The keylogger is configurable through the `configuration.h` file, current updates include:

- Toggle timestamp logging
- Toggle window title tracking
- Configure remote logging server URL
- Set log rotation thresholds
- Enable/disable mouse tracking
- Configure stealth mode
- Adjust memory management parameters

## Usage

1. Build the project in Release mode
2. Run the executable 
3. Monitor logs in `logged.txt` or via your remote logging server


