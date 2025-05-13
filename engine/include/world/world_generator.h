// engine/include/world/world_generator.h
#pragma once
#include "world/chunk_segment.h"

namespace VoxelCastle {
namespace World {

class WorldGenerator {
public:
    // Simple heightmap-based terrain using value noise
    static void generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ);
};

} // namespace World
} // namespace VoxelCastle
