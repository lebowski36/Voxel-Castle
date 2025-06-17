#pragma once

#include "world/voxel_types.h"
#include "world/procedural_terrain/core/climate_system.h"
#include <cstdint>
#include <memory>

// Forward declarations
namespace VoxelEngine {
namespace World {
class Chunk;
}
}

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

/**
 * @brief Main entry point for procedural terrain generation
 * 
 * This class orchestrates all terrain generation systems to produce
 * complete chunks with heightmap, biomes, features, and block placement.
 */
class ChunkGenerator {
public:
    /**
     * @brief Constructor
     * @param worldSeed Seed for deterministic generation
     */
    explicit ChunkGenerator(uint64_t worldSeed);
    
    /**
     * @brief Generate a complete chunk with all features
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @return Generated chunk with complete terrain
     */
    std::shared_ptr<VoxelEngine::World::Chunk> GenerateChunk(int chunkX, int chunkZ);
    
    /**
     * @brief Generate heightmap for a chunk
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param heightmap Output heightmap array (32x32)
     */
    void GenerateHeightmap(int chunkX, int chunkZ, float heightmap[32][32]);
    
    /**
     * @brief Get climate data for a chunk
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @return Climate data for the chunk center
     */
    ClimateData GetChunkClimate(int chunkX, int chunkZ);
    
    /**
     * @brief Get the world seed
     * @return The world generation seed
     */
    uint64_t GetWorldSeed() const { return worldSeed_; }

private:
    /**
     * @brief Generate basic terrain heightmap
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param heightmap Output heightmap array
     */
    void GenerateBaseTerrain(int chunkX, int chunkZ, float heightmap[32][32]);
    
    /**
     * @brief Apply biome-specific terrain modifications
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param heightmap Heightmap to modify
     * @param climate Climate data for the chunk
     */
    void ApplyBiomeModifications(int chunkX, int chunkZ, float heightmap[32][32], const ClimateData& climate);
    
    /**
     * @brief Place blocks based on heightmap and biome
     * @param chunk Chunk to fill with blocks
     * @param heightmap Terrain heightmap
     * @param climate Climate data for block selection
     */
    void PlaceBlocks(std::shared_ptr<VoxelEngine::World::Chunk> chunk, 
                     const float heightmap[32][32], 
                     const ClimateData& climate);
    
    /**
     * @brief Generate cave systems in the chunk
     * @param chunk Chunk to carve caves in
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     */
    void GenerateCaves(std::shared_ptr<VoxelEngine::World::Chunk> chunk, int chunkX, int chunkZ);
    
    /**
     * @brief Generate ore veins in the chunk
     * @param chunk Chunk to place ores in
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     */
    void GenerateOres(std::shared_ptr<VoxelEngine::World::Chunk> chunk, int chunkX, int chunkZ);

    uint64_t worldSeed_;
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
