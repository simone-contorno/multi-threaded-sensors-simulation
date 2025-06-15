#include "Logger.hpp"
#include <filesystem>

std::string Logger::filename_;
std::ofstream Logger::logfile_;
std::mutex Logger::log_mutex_;

void Logger::init() {
    // Retrieve file name
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << "../log/log_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".log";
    filename_ = oss.str();

    // Ensure the log directory exists
    std::filesystem::create_directories("../log");

    // Open the log file in append mode
    logfile_.open(filename_, std::ios::out | std::ios::app); // Append mode
}

void Logger::log(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);

    // Open the log file if it is not already open
    if (!logfile_.is_open()) {
        logfile_.open(filename_, std::ios::out | std::ios::app); // Append mode
    }

    switch (level) {
        case Level::Debug:
            logfile_ << "[DEBUG] - " << message << std::endl;
            logfile_.flush(); // Ensure the debug message is written immediately
            std::cout << "[DEBUG] - " << message << std::endl;
            break;
        case Level::Info:
            logfile_ << "[INFO] - " << message << std::endl;
            logfile_.flush(); // Ensure the info is written immediately
            std::cout << "[INFO] - " << message << std::endl;
            break;
        case Level::Warning:
            logfile_ << "[WARNING] - " << message << std::endl;
            logfile_.flush(); // Ensure the warning is written immediately
            std::cout << "[WARNING] - " << message << std::endl;
            break;
        case Level::Error:
            logfile_ << "[ERROR] - " << message << std::endl;
            logfile_.flush(); // Ensure the error is written immediately
            std::cerr << "[ERROR] - " << message << std::endl;
            break;
    }
}