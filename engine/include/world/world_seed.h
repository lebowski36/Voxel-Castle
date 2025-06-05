// engine/include/world/world_seed.h
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace VoxelCastle {
namespace World {

// Feature types for seed domain separation
enum class FeatureType : uint32_t {
    TERRAIN = 0,
    CAVES = 1,
    ORES = 2,
    STRUCTURES = 3,
    BIOMES = 4,
    WEATHER = 5,
    WATER = 6,
    VEGETATION = 7,
    // Add more as needed
    COUNT // Keep at end for iteration
};

// Scale levels for multi-scale generation
enum class ScaleLevel : uint32_t {
    BLOCK = 0,     // Individual block level
    CHUNK = 1,     // 16x16x32 chunk level
    REGION = 2,    // Multiple chunks (e.g., 16x16 chunks)
    CONTINENTAL = 3 // Very large scale features
};

/**
 * Enhanced seed system for deterministic world generation.
 * Provides feature-separated, multi-scale seed derivation with good distribution.
 */
class WorldSeed {
public:
    // Constructor with random seed
    WorldSeed();
    
    // Constructor with numeric seed
    explicit WorldSeed(uint64_t seed);
    
    // Constructor with string seed
    explicit WorldSeed(const std::string& seedString);
    
    // Get the master world seed
    uint64_t getMasterSeed() const { return masterSeed_; }
    
    // Get a seed for a specific world position
    uint64_t getBlockSeed(int64_t x, int64_t y, int64_t z) const;
    
    // Get a seed for a specific world position and feature type
    uint64_t getFeatureSeed(int64_t x, int64_t y, int64_t z, FeatureType feature) const;
    
    // Get a seed for a chunk (16x16x32 block region)
    uint64_t getChunkSeed(int64_t chunkX, int64_t chunkY, int64_t chunkZ) const;
    
    // Get a seed for a chunk with feature type
    uint64_t getChunkFeatureSeed(int64_t chunkX, int64_t chunkY, int64_t chunkZ, FeatureType feature) const;
    
    // Get a seed for a region (multiple chunks)
    uint64_t getRegionSeed(int64_t regionX, int64_t regionY, int64_t regionZ) const;
    
    // Get a seed for a region with feature type
    uint64_t getRegionFeatureSeed(int64_t regionX, int64_t regionY, int64_t regionZ, FeatureType feature) const;
    
    // Get a scale-appropriate seed (automatically chooses block/chunk/region based on scale)
    uint64_t getScaleSeed(int64_t x, int64_t y, int64_t z, ScaleLevel scale, FeatureType feature = FeatureType::TERRAIN) const;
    
    // Convenient helper methods for specific feature generation
    // Regional Seed Helpers (Tier 1 - Pre-Generation)
    uint64_t getRegionSeed(int64_t regionX, int64_t regionZ) const;
    uint64_t getClimateSeed(int64_t regionX, int64_t regionZ) const;
    uint64_t getBiomeSeed(int64_t regionX, int64_t regionZ) const;
    uint64_t getCivilizationSeed(int64_t regionX, int64_t regionZ) const;
    uint64_t getHistorySeed(int64_t regionX, int64_t regionZ, int32_t year) const;
    
    // Chunk Seed Helpers (Tier 2 - Runtime)
    uint64_t getStructureSeed(int64_t chunkX, int64_t chunkZ) const;
    uint64_t getCaveSeed(int64_t chunkX, int64_t chunkY, int64_t chunkZ) const;
    uint64_t getOreSeed(int64_t chunkX, int64_t chunkY, int64_t chunkZ) const;
    uint64_t getVegetationSeed(int64_t chunkX, int64_t chunkZ) const;
    uint64_t getWaterSeed(int64_t chunkX, int64_t chunkZ) const;
    
    // Set new master seed
    void setSeed(uint64_t seed);
    void setSeedFromString(const std::string& seedString);
    
    // Get string representation of the seed (for display/saving)
    std::string getSeedString() const;
    
    // Utility: Generate random 0.0-1.0 float from seed
    static float seedToFloat(uint64_t seed);
    
    // Utility: Generate random integer in range [min, max] from seed
    static int32_t seedToRange(uint64_t seed, int32_t min, int32_t max);
    
    // Debug: Get derived seed info for debugging
    struct SeedInfo {
        uint64_t baseSeed;
        uint64_t featureSeed;
        uint64_t finalSeed;
        std::string featureName;
        std::string scaleName;
    };
    SeedInfo getDebugInfo(int64_t x, int64_t y, int64_t z, ScaleLevel scale, FeatureType feature) const;

private:
    uint64_t masterSeed_;
    std::string originalSeedString_; // Store original string if provided
    
    // Feature domain seeds (derived from master seed)
    mutable std::unordered_map<FeatureType, uint64_t> featureDomainSeeds_;
    
    // Initialize feature domain seeds
    void initializeFeatureDomains();
    
    // Get the domain seed for a specific feature type
    uint64_t getFeatureDomainSeed(FeatureType feature) const;
    
    // Core seed derivation with high-quality bit mixing
    static uint64_t deriveSeed(uint64_t baseSeed, int64_t x, int64_t y, int64_t z);
    
    // High-quality string-to-seed hash
    static uint64_t hashString(const std::string& str);
    
    // Avalanche hash function for good bit distribution
    static uint64_t avalancheHash(uint64_t x);
};

// Feature type name conversion for debugging
const char* getFeatureTypeName(FeatureType type);
const char* getScaleLevelName(ScaleLevel scale);

} // namespace World
} // namespace VoxelCastle
