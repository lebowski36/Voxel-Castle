#include "ui/UILogger.h"
#include <iostream>
#include <filesystem>

namespace VoxelEngine {
namespace UI {

UILogger& UILogger::getInstance() {
    static UILogger instance;
    return instance;
}

void UILogger::initialize() {
    if (initialized_) {
        return;
    }
    
    // Create debug_logs directory if it doesn't exist
    std::filesystem::create_directories("/home/system-x1/Projects/Voxel Castle/debug_logs");
    
    // Create timestamped log file
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << "/home/system-x1/Projects/Voxel Castle/debug_logs/UISystem_"
       << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S_")
       << std::setfill('0') << std::setw(3) << ms.count() << ".log";
    
    logFile_ = std::make_unique<std::ofstream>(ss.str(), std::ios::app);
    
    if (logFile_->is_open()) {
        initialized_ = true;
        info("UILogger", "UI logging system initialized: " + ss.str());
    } else {
        std::cerr << "Failed to open UI log file: " << ss.str() << std::endl;
    }
}

UILogger::~UILogger() {
    if (logFile_ && logFile_->is_open()) {
        logFile_->close();
    }
}

std::string UILogger::getCurrentTimestamp() {
    auto now = std::chrono::high_resolution_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string UILogger::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void UILogger::log(LogLevel level, const std::string& component, const std::string& message) {
    if (!initialized_) {
        initialize();
    }
    
    if (logFile_ && logFile_->is_open()) {
        *logFile_ << "[" << getCurrentTimestamp() << "] "
                  << "[" << getLogLevelString(level) << "] "
                  << "[" << component << "] "
                  << message << std::endl;
        logFile_->flush();
    }
}

void UILogger::debug(const std::string& component, const std::string& message) {
    log(LogLevel::DEBUG, component, message);
}

void UILogger::info(const std::string& component, const std::string& message) {
    log(LogLevel::INFO, component, message);
}

void UILogger::warning(const std::string& component, const std::string& message) {
    log(LogLevel::WARNING, component, message);
}

void UILogger::error(const std::string& component, const std::string& message) {
    log(LogLevel::ERROR, component, message);
}

} // namespace UI
} // namespace VoxelEngine
