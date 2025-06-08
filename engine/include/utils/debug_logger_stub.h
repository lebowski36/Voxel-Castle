#pragma once

#include <string>

// Stub header for VoxelCastle::Utils logging to satisfy engine linking
// This is a temporary solution until dual logging systems are merged

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
    ~DebugLogger() = default;
};

// Convenience macros for easy logging
#define DEBUG_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().debug(subsystem, message)
#define INFO_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().info(subsystem, message)
#define WARN_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().warning(subsystem, message)
#define ERROR_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().error(subsystem, message)
#define CRITICAL_LOG(subsystem, message) VoxelCastle::Utils::DebugLogger::getInstance().critical(subsystem, message)

// Stub logToFile function
extern void logToFile(const std::string& message);

} // namespace Utils
} // namespace VoxelCastle
