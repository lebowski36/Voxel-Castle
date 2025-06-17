#pragma once

#include <cstdint>
#include <string>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

/**
 * @brief Seed management utilities for deterministic world generation
 * 
 * Provides utilities for deriving unique seeds for different generation
 * components while maintaining determinism across the entire world.
 */
class SeedUtils {
public:
    /**
     * @brief Generate a deterministic seed offset for a specific component
     * @param baseSeed The world's base seed
     * @param component Component identifier (e.g., "terrain", "caves", "climate")
     * @return Unique seed for the component
     */
    static uint64_t GetComponentSeed(uint64_t baseSeed, const std::string& component);
    
    /**
     * @brief Generate a chunk-specific seed
     * @param baseSeed The world's base seed
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @return Unique seed for the chunk
     */
    static uint64_t GetChunkSeed(uint64_t baseSeed, int chunkX, int chunkZ);
    
    /**
     * @brief Generate a seed for a specific feature at chunk coordinates
     * @param baseSeed The world's base seed
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param feature Feature identifier (e.g., "rivers", "caves", "ores")
     * @return Unique seed for the feature at chunk coordinates
     */
    static uint64_t GetFeatureSeed(uint64_t baseSeed, int chunkX, int chunkZ, const std::string& feature);
    
    /**
     * @brief Convert a string seed to a 64-bit integer
     * @param seedString String representation of seed
     * @return 64-bit integer seed
     */
    static uint64_t StringToSeed(const std::string& seedString);
    
    /**
     * @brief Generate a random seed (for when user doesn't specify one)
     * @return Random 64-bit seed
     */
    static uint64_t GenerateRandomSeed();

private:
    /**
     * @brief Simple hash function for consistent seed derivation
     * @param input Input data to hash
     * @param length Length of input data
     * @return Hash value
     */
    static uint64_t SimpleHash(const void* input, size_t length);
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
