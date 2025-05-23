// engine/src/world/world_generator.cpp
#include "world/world_generator.h"
#include "util/noise.h"
#include "world/voxel_types.h"
#include "world/voxel.h"
#include <cmath>
#include <iostream> // Added for logging

namespace VoxelCastle {
namespace World {

// Simple heightmap world generation using value noise
void WorldGenerator::generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ) {
    // Added for logging task 3.1
    if (worldX == 0 && worldY == 0 && worldZ == 0) { // Log only for a specific segment to avoid spam
        std::cout << "[WorldGenerator] Generating segment at indices: (" << worldX << ", " << worldY << ", " << worldZ << ")" << std::endl;
    }

    constexpr int width = ChunkSegment::CHUNK_WIDTH;
    constexpr int height = ChunkSegment::CHUNK_HEIGHT; // e.g., 32
    constexpr int depth = ChunkSegment::CHUNK_DEPTH;

    // --- TUNABLE PARAMETERS ---
    const float noiseInputScale = 0.08f;         // Increased for potentially steeper features
    const float terrainAmplitude = height * 1.5f;  // Increased amplitude (e.g., 32 * 1.5 = 48)
    const float baseTerrainOffset = static_cast<float>(height) / 8.0f; // Adjusted base offset (e.g., 32 / 8 = 4)
    // --- END TUNABLE PARAMETERS ---

    // World Y coordinate of this segment's base
    int baseY = worldY * height;

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            // Use noise to get height
            // FIX: Use true world coordinates for noise input
            int globalX = worldX + x;
            int globalZ = worldZ + z;
            float nx = globalX * noiseInputScale;
            float nz = globalZ * noiseInputScale;
            float noise_val = VoxelEngine::Util::smoothValueNoise(nx, 0.0f, nz); // Renamed to noise_val to avoid conflict

            // Assuming noise_val is in [0, 1]. If not, it might need clamping/remapping:
            // noise_val = std::max(0.0f, std::min(1.0f, noise_val));

            int columnHeight = static_cast<int>(noise_val * terrainAmplitude) + static_cast<int>(baseTerrainOffset);

            // Added for logging task 3.1 - log for local (0,0) and (15,15)
            if (worldX == 0 && worldY == 0 && worldZ == 0 && ((x == 0 && z == 0) || (x == 15 && z == 15))) {
                std::cout << "  [Debug] Local (" << x << "," << z << "): nx=" << nx << ", nz=" << nz << ", noise_val=" << noise_val << ", columnHeight=" << columnHeight << std::endl;
            }

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

                // Added for logging task 3.1 - log for local (0,y,0) and (15,y,15)
                if (worldX == 0 && worldY == 0 && worldZ == 0 && ((x == 0 && z == 0) || (x == 15 && z == 15))) {
                    if (y == 0 || y == 15 || y == columnHeight || y == columnHeight + 1) { // Log specific y values
                         std::cout << "    [Debug] Local (" << x << "," << y << "," << z << "): globalY=" << globalY << ", type=" << static_cast<int>(type) << std::endl;
                    }
                }
            }
        }
    }
}

} // namespace World
} // namespace VoxelCastle
