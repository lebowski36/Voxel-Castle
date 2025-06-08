#include "utils/debug_logger_stub.h"
#include <iostream>

// Stub implementations to satisfy engine linking
// These will be replaced when the dual logging systems are merged

namespace VoxelCastle {
namespace Utils {

// Stub DebugLogger class implementation
DebugLogger& DebugLogger::getInstance() {
    static DebugLogger instance;
    return instance;
}

void DebugLogger::initialize(const std::string& subsystem) {
    // Stub implementation - do nothing
}

void DebugLogger::log(LogLevel level, const std::string& subsystem, const std::string& message) {
    // Stub implementation - just print to stderr for now
    std::cerr << "[LOG][" << subsystem << "] " << message << std::endl;
}

void DebugLogger::debug(const std::string& subsystem, const std::string& message) {
    // Stub implementation - just print to stderr for now
    std::cerr << "[DEBUG][" << subsystem << "] " << message << std::endl;
}

void DebugLogger::info(const std::string& subsystem, const std::string& message) {
    // Stub implementation - just print to stderr for now
    std::cerr << "[INFO][" << subsystem << "] " << message << std::endl;
}

void DebugLogger::warning(const std::string& subsystem, const std::string& message) {
    // Stub implementation - just print to stderr for now
    std::cerr << "[WARNING][" << subsystem << "] " << message << std::endl;
}

void DebugLogger::error(const std::string& subsystem, const std::string& message) {
    // Stub implementation - just print to stderr for now
    std::cerr << "[ERROR][" << subsystem << "] " << message << std::endl;
}

void DebugLogger::critical(const std::string& subsystem, const std::string& message) {
    // Stub implementation - just print to stderr for now
    std::cerr << "[CRITICAL][" << subsystem << "] " << message << std::endl;
}

void DebugLogger::startNewSession() {
    // Stub implementation - do nothing
}

// Stub logToFile function
void logToFile(const std::string& message) {
    // Stub implementation - just print to stderr for now
    std::cerr << "[LOG] " << message << std::endl;
}

} // namespace Utils
} // namespace VoxelCastle
