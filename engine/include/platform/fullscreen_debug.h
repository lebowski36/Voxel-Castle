\
#ifndef FULLSCREEN_DEBUG_H
#define FULLSCREEN_DEBUG_H

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip> // For std::put_time

namespace FullscreenDebug {

    void openLog(const std::string& projectRoot);
    void closeLog();
    void log(const std::string& message);

    // Helper to get timestamp
    inline std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
        return ss.str();
    }

} // namespace FullscreenDebug

#define FS_LOG(msg) FullscreenDebug::log(std::string("[") + FullscreenDebug::getTimestamp() + "] " + msg)

#endif // FULLSCREEN_DEBUG_H
