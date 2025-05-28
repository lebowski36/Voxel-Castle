#ifndef VOXEL_ENGINE_LOGGING_UTILS_H
#define VOXEL_ENGINE_LOGGING_UTILS_H

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <thread>

namespace VoxelEngine {
namespace Utils {

// Helper function for timestamped logging
inline std::string getTimestamp() {
    auto now = std::chrono::high_resolution_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

inline std::string getThreadInfo() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

} // namespace Utils
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_LOGGING_UTILS_H
