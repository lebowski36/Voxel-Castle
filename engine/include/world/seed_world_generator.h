#ifndef SEED_WORLD_GENERATOR_H
#define SEED_WORLD_GENERATOR_H

#include "world/world_generator.h"
#include <cstdint>

// Forward declaration
namespace VoxelCastle {
    namespace World {
        class ChunkSegment;
    }
}

class SeedWorldGenerator : public VoxelCastle::World::WorldGenerator {
public:
    explicit SeedWorldGenerator(uint64_t seed);
    ~SeedWorldGenerator() override = default;

    void generateChunkSegment(VoxelCastle::World::ChunkSegment& segment, int worldX, int worldY, int worldZ) override;


private:
    uint64_t worldSeed;

    // Noise generators and other components will be added here
};

#endif // SEED_WORLD_GENERATOR_H
