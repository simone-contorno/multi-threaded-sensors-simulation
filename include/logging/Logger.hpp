#pragma once
#include <string>
#include <iostream>
#include <mutex>
#include <fstream>

class Logger {
public:
    // Log levels
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    // Constructor: create the log file
    static void init();

    // Log a message with a specific level
    static void log(Level level, const std::string& message);

private:
    static std::ofstream logfile_; // Log file stream
    static std::string filename_; // Log file name
    static std::mutex log_mutex_; // Mutex for thread-safe logging
};