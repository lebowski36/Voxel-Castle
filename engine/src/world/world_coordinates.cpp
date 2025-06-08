#include "world/world_coordinates.h"
#include <sstream>

namespace VoxelCastle {
namespace World {
namespace WorldCoordinates {

const char* GetCoordinateSystemInfo() {
    static std::string info;
    if (info.empty()) {
        std::ostringstream oss;
        oss << "Voxel Castle World Coordinate System:\n";
        oss << "  Voxel Size: " << VOXEL_SIZE_METERS << "m³ (" 
            << (VOXEL_SIZE_METERS * 100) << "cm cubes)\n";
        oss << "  Chunk Size: " << CHUNK_SIZE_VOXELS << "×" << CHUNK_SIZE_VOXELS 
            << "×" << CHUNK_SIZE_VOXELS << " voxels = " 
            << CHUNK_SIZE_METERS << "m³\n";
        oss << "  World Height: " << TOTAL_CHUNK_LAYERS << " chunks ("
            << TOTAL_WORLD_HEIGHT_METERS << "m = " << TOTAL_WORLD_HEIGHT_BLOCKS << " blocks)\n";
        oss << "  Vertical Range: " << MIN_CHUNK_Y << " to " << MAX_CHUNK_Y << " chunks\n";
        oss << "  Depth Range: " << MIN_WORLD_Y_METERS << "m to " << MAX_WORLD_Y_METERS << "m\n";
        oss << "  Block Range: " << MIN_WORLD_Y_BLOCKS << " to " << MAX_WORLD_Y_BLOCKS << " blocks\n";
        oss << "  Sea Level: Chunk Y=" << SEA_LEVEL_CHUNK_Y << ", Block Y=" << SEA_LEVEL_BLOCKS << "\n";
        info = oss.str();
    }
    return info.c_str();
}

} // namespace WorldCoordinates
} // namespace World
} // namespace VoxelCastle
