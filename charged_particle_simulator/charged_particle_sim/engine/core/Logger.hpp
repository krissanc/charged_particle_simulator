#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>

/**
 * Logger System for Charged Particle Simulator
 * 
 * Provides hierarchical logging with levels: DEBUG, INFO, WARN, ERROR
 * Outputs to both console and file with timestamps
 * Thread-safe logging with mutex protection
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Logger {
public:
    // Initialize logger with log file path and minimum log level
    static void initialize(const std::string& logFile = "logs/simulation.log", 
                          LogLevel minLevel = LogLevel::INFO);
    
    // Shutdown logger and close file
    static void shutdown();
    
    // Log functions for each level
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    
    // Set minimum log level (messages below this level are ignored)
    static void setMinLevel(LogLevel level);
    
    // Get current minimum log level
    static LogLevel getMinLevel();

private:
    Logger() = default;
    ~Logger() = default;
    
    // Core logging function
    static void log(LogLevel level, const std::string& message);
    
    // Format timestamp as [HH:MM:SS.mmm]
    static std::string formatTimestamp();
    
    // Convert log level to string
    static std::string levelToString(LogLevel level);
    
    static std::unique_ptr<std::ofstream> s_logFile;
    static LogLevel s_minLevel;
    static std::mutex s_mutex;
    static bool s_initialized;
};

// Convenience macros for logging
#define LOG_DEBUG(msg) Logger::debug(msg)
#define LOG_INFO(msg) Logger::info(msg)
#define LOG_WARN(msg) Logger::warn(msg)
#define LOG_ERROR(msg) Logger::error(msg)

