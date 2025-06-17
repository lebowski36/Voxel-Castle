#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>

// Header for VoxelCastle::Utils logging to support engine files
// This provides file-based timestamped logging like the game implementation

namespace VoxelCastle {
namespace Utils {

class DebugLogger {
public:
    enum class LogLevel {
        DEBUG,
        INFO, 
        WARNING,
        ERROR,
        CRITICAL
    };

    static DebugLogger& getInstance();
    
    // Initialize with a specific subsystem name (creates timestamped file)
    void initialize(const std::string& subsystem);
    
    // Log methods
    void log(LogLevel level, const std::string& subsystem, const std::string& message);
    void debug(const std::string& subsystem, const std::string& message);
    void info(const std::string& subsystem, const std::string& message);
    void warning(const std::string& subsystem, const std::string& message);
    void error(const std::string& subsystem, const std::string& message);
    void critical(const std::string& subsystem, const std::string& message);

    // Convenience method to start a new session (creates new timestamped files)
    void startNewSession();

private:
    DebugLogger() = default;
    ~DebugLogger();
    
    std::string getCurrentTimestamp();
    std::string getLogLevelString(LogLevel level);
    std::ofstream& getLogFile(const std::string& subsystem);
    
    std::string sessionTimestamp_;
    std::map<std::string, std::unique_ptr<std::ofstream>> logFiles_;
    bool initialized_ = false;
};

// Convenience macros for easy logging
#define DEBUG_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().debug(subsystem, message)
#define INFO_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().info(subsystem, message)
#define WARN_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().warning(subsystem, message)
#define ERROR_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().error(subsystem, message)
#define CRITICAL_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().critical(subsystem, message)

// logToFile function
extern void logToFile(const std::string& message);

} // namespace Utils
} // namespace VoxelCastle
