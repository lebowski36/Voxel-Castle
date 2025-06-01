#include "utils/debug_logger.h"
#include <filesystem>
#include <iostream>
#include <map>
#include <fstream>
#include <string>

namespace VoxelCastle {
namespace Utils {

DebugLogger& DebugLogger::getInstance() {
    static DebugLogger instance;
    return instance;
}

void DebugLogger::initialize(const std::string& subsystem) {
    if (!initialized_) {
        startNewSession();
        initialized_ = true;
    }
    
    // Create debug_logs directory if it doesn't exist
    std::filesystem::create_directories("debug_logs");
    
    // Ensure this subsystem has a log file
    getLogFile(subsystem);
}

void DebugLogger::startNewSession() {
    // Close existing files
    logFiles_.clear();
    
    // Generate new session timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    sessionTimestamp_ = ss.str();
    
    // Redirected verbose session start message to file-based logging
    logToFile("[DebugLogger] Started new session: " + sessionTimestamp_);
}

std::string DebugLogger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string DebugLogger::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::ofstream& DebugLogger::getLogFile(const std::string& subsystem) {
    auto it = logFiles_.find(subsystem);
    if (it == logFiles_.end()) {
        std::string filename = "debug_logs/" + subsystem + "_" + sessionTimestamp_ + ".log";
        auto file = std::make_unique<std::ofstream>(filename, std::ios::app);
        if (!file->is_open()) {
            // Preserved critical error message
            std::cerr << "[DebugLogger] Failed to open log file: " << filename << std::endl;
        } else {
            *file << "[" << getCurrentTimestamp() << "] ===== LOG SESSION STARTED =====" << std::endl;
            file->flush();
        }
        it = logFiles_.emplace(subsystem, std::move(file)).first;
    }
    return *(it->second);
}

void DebugLogger::log(LogLevel level, const std::string& subsystem, const std::string& message) {
    if (!initialized_) {
        initialize(subsystem);
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = getLogLevelString(level);
    
    auto& file = getLogFile(subsystem);
    if (file.is_open()) {
        file << "[" << timestamp << "] [" << levelStr << "] " << message << std::endl;
        file.flush(); // Ensure immediate write
    }
    
    // Also output critical and error messages to console
    if (level == LogLevel::CRITICAL || level == LogLevel::ERROR) {
        std::cerr << "[" << subsystem << "] [" << levelStr << "] " << message << std::endl;
    }
}

void DebugLogger::debug(const std::string& subsystem, const std::string& message) {
    log(LogLevel::DEBUG, subsystem, message);
}

void DebugLogger::info(const std::string& subsystem, const std::string& message) {
    log(LogLevel::INFO, subsystem, message);
}

void DebugLogger::warning(const std::string& subsystem, const std::string& message) {
    log(LogLevel::WARNING, subsystem, message);
}

void DebugLogger::error(const std::string& subsystem, const std::string& message) {
    log(LogLevel::ERROR, subsystem, message);
}

void DebugLogger::critical(const std::string& subsystem, const std::string& message) {
    log(LogLevel::CRITICAL, subsystem, message);
}

DebugLogger::~DebugLogger() {
    for (auto& [subsystem, file] : logFiles_) {
        if (file && file->is_open()) {
            *file << "[" << getCurrentTimestamp() << "] ===== LOG SESSION ENDED =====" << std::endl;
            file->close();
        }
    }
}

void logToFile(const std::string& message) {
    std::ofstream logFile("debug_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

} // namespace Utils
} // namespace VoxelCastle
