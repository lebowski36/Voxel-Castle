#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace VoxelEngine {
namespace UI {

class UILogger {
public:
    enum class LogLevel {
        DEBUG,
        INFO, 
        WARNING,
        ERROR
    };

    static UILogger& getInstance();
    
    void initialize();
    void log(LogLevel level, const std::string& component, const std::string& message);
    void debug(const std::string& component, const std::string& message);
    void info(const std::string& component, const std::string& message);
    void warning(const std::string& component, const std::string& message);
    void error(const std::string& component, const std::string& message);

private:
    UILogger() = default;
    ~UILogger();
    
    std::string getCurrentTimestamp();
    std::string getLogLevelString(LogLevel level);
    
    std::unique_ptr<std::ofstream> logFile_;
    bool initialized_ = false;
};

} // namespace UI
} // namespace VoxelEngine
