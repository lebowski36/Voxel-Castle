#include "world/geological_constants.h"

namespace VoxelCastle {
namespace World {

// Initialize warning counters
std::atomic<int> extreme_elevation_warning_count{0};
std::atomic<bool> elevation_warnings_exceeded{false};

} // namespace World
} // namespace VoxelCastle
