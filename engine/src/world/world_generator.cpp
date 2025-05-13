// engine/src/world/world_generator.cpp
#include "world/world_generator.h"
#include "util/noise.h"
#include "world/voxel_types.h"
#include "world/voxel.h"
#include <cmath>

namespace VoxelCastle {
namespace World {

// Simple heightmap world generation using value noise
void WorldGenerator::generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ) {
    constexpr int width = ChunkSegment::CHUNK_WIDTH;
    constexpr int height = ChunkSegment::CHUNK_HEIGHT;
    constexpr int depth = ChunkSegment::CHUNK_DEPTH;

    // World Y coordinate of this segment's base
    int baseY = worldY * height;

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            // Use noise to get height in [0, height*4)
            float nx = (worldX * width + x) * 0.05f;
            float nz = (worldZ * depth + z) * 0.05f;
            float noise = VoxelEngine::Util::smoothValueNoise(nx, 0.0f, nz);
            int columnHeight = static_cast<int>(noise * (height * 0.75f)) + (height / 4);

            for (int y = 0; y < height; ++y) {
                int globalY = baseY + y;
                ::VoxelEngine::World::VoxelType type = ::VoxelEngine::World::VoxelType::AIR;
                if (globalY <= columnHeight) {
                    if (globalY == columnHeight) {
                        type = ::VoxelEngine::World::VoxelType::GRASS;
                    } else if (globalY > columnHeight - 3) {
                        type = ::VoxelEngine::World::VoxelType::DIRT;
                    } else {
                        type = ::VoxelEngine::World::VoxelType::STONE;
                    }
                }
                segment.setVoxel(x, y, z, ::VoxelEngine::World::Voxel(static_cast<uint8_t>(type)));
            }
        }
    }
}

} // namespace World
} // namespace VoxelCastle
