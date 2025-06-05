// engine/include/world/world_generator.h
#pragma once
#include "world/chunk_segment.h"
#include <random>
#include <string>

namespace VoxelCastle {
namespace World {

class WorldGenerator {
public:
    // Constructor initializes with a default random seed
    WorldGenerator();
    
    // Set world seed from a numeric value
    void setSeed(uint64_t seed);
    
    // Set world seed from a string (converts to numeric seed)
    void setSeedFromString(const std::string& seedString);
    
    // Get unique seed for a specific block position
    uint64_t getBlockSeed(int64_t x, int64_t y, int64_t z) const;
    
    // Static version of getBlockSeed for use in static methods - uses default seed
    static uint64_t staticGetBlockSeed(int64_t x, int64_t y, int64_t z);
    
    // Simple heightmap-based terrain using value noise
    void generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ);

private:
    uint64_t worldSeed_;
    std::mt19937_64 rng_;
};

} // namespace World
} // namespace VoxelCastle
