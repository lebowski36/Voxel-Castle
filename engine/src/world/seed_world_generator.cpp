#include "world/seed_world_generator.h"
#include "world/chunk_segment.h"
#include "world/voxel_types.h"
#include "world/voxel.h"

#include <iostream>

SeedWorldGenerator::SeedWorldGenerator(uint64_t seed) : worldSeed(seed) {
    std::cout << "SeedWorldGenerator created with seed: " << worldSeed << std::endl;
}

void SeedWorldGenerator::generateChunkSegment(VoxelCastle::World::ChunkSegment& segment, int segmentX, int segmentY, int segmentZ) {
    for (int y = 0; y < VoxelCastle::World::SEGMENT_HEIGHT; ++y) {
        for (int z = 0; z < VoxelCastle::World::SEGMENT_DEPTH; ++z) {
            for (int x = 0; x < VoxelCastle::World::SEGMENT_WIDTH; ++x) {
                if (segmentY * VoxelCastle::World::SEGMENT_HEIGHT + y < 32) {
                    segment.setVoxel(x, y, z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS)));
                } else {
                    segment.setVoxel(x, y, z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)));
                }
            }
        }
    }
}
