#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>

// Static member definitions
std::unique_ptr<std::ofstream> Logger::s_logFile = nullptr;
LogLevel Logger::s_minLevel = LogLevel::INFO;
std::mutex Logger::s_mutex;
bool Logger::s_initialized = false;

void Logger::initialize(const std::string& logFile, LogLevel minLevel) {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (s_initialized) {
        return; // Already initialized
    }
    
    // Create logs directory if it doesn't exist
    std::filesystem::path logPath(logFile);
    if (logPath.has_parent_path()) {
        std::filesystem::create_directories(logPath.parent_path());
    }
    
    // Open log file in append mode
    s_logFile = std::make_unique<std::ofstream>(logFile, std::ios::app);
    if (!s_logFile->is_open()) {
        std::cerr << "[ERROR] Failed to open log file: " << logFile << std::endl;
        return;
    }
    
    s_minLevel = minLevel;
    s_initialized = true;
    
    info("Logger initialized - log file: " + logFile);
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (s_logFile && s_logFile->is_open()) {
        info("Logger shutting down");
        s_logFile->close();
    }
    
    s_logFile.reset();
    s_initialized = false;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_minLevel = level;
}

LogLevel Logger::getMinLevel() {
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_minLevel;
}

void Logger::log(LogLevel level, const std::string& message) {
    // Check if message should be logged based on minimum level
    if (level < s_minLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (!s_initialized) {
        // Fallback to console if logger not initialized
        std::cout << "[FALLBACK] " << message << std::endl;
        return;
    }
    
    // Format log entry: [LEVEL][HH:MM:SS.mmm] message
    std::string timestamp = formatTimestamp();
    std::string levelStr = levelToString(level);
    std::string logEntry = "[" + levelStr + "][" + timestamp + "] " + message;
    
    // Output to console
    if (level >= LogLevel::WARN) {
        std::cerr << logEntry << std::endl;
    } else {
        std::cout << logEntry << std::endl;
    }
    
    // Output to file
    if (s_logFile && s_logFile->is_open()) {
        *s_logFile << logEntry << std::endl;
        s_logFile->flush(); // Ensure immediate write
    }
}

std::string Logger::formatTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ) % 1000;
    
    std::tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &time);
#else
    localtime_r(&time, &timeInfo);
#endif
    
    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

