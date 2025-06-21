// engine/include/world/world_generator.h
#pragma once
#include "world/chunk_segment.h"
#include "world/world_seed.h"
#include <random>
#include <string>
#include <memory>

namespace VoxelCastle {
namespace World {

class WorldGenerator {
public:
    virtual ~WorldGenerator() = default; // Add virtual destructor
    // Constructor initializes with a default random seed
    WorldGenerator();
    
    // Set world seed from a numeric value
    void setSeed(uint64_t seed);
    
    // Set world seed from a string (converts to numeric seed)
    void setSeedFromString(const std::string& seedString);
    
    // Get unique seed for a specific block position (legacy method)
    uint64_t getBlockSeed(int64_t x, int64_t y, int64_t z) const;
    
    // Static version of getBlockSeed for use in static methods - uses default seed
    static uint64_t staticGetBlockSeed(int64_t x, int64_t y, int64_t z);
    
    // Enhanced seed system access
    const WorldSeed& getWorldSeed() const;
    WorldSeed& getWorldSeed();
    
    // Get enhanced seeds for specific features and scales
    uint64_t getFeatureSeed(int64_t x, int64_t y, int64_t z, FeatureType feature) const;
    uint64_t getScaleSeed(int64_t x, int64_t y, int64_t z, ScaleLevel scale, FeatureType feature = FeatureType::TERRAIN) const;
    
    // Simple heightmap-based terrain using value noise
    virtual void generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    
    // Enhanced terrain generation (new system)
    void generateChunkSegmentEnhanced(ChunkSegment& segment, int worldX, int worldY, int worldZ);

private:
    uint64_t legacySeed_;  // Legacy seed storage for compatibility
    std::mt19937_64 rng_; // Legacy RNG for compatibility
    std::unique_ptr<WorldSeed> worldSeed_; // Enhanced seed system
    
    // Helper method to ensure worldSeed_ is initialized
    void ensureWorldSeedInitialized() const;
};

} // namespace World
} // namespace VoxelCastle
