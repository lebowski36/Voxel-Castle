#pragma once

#include "world/world_seed.h"
#include "world/world_parameters.h"
#include "world/chunk_segment.h"
#include "world/regional_data.h"
#include <memory>
#include <random>

namespace VoxelCastle {
namespace World {

/**
 * @brief New seed-based world generator that achieves feature parity with legacy system
 * 
 * This generator implements the two-tier generation strategy:
 * - Tier 1: Regional template generation (for large worlds)
 * - Tier 2: Chunk-level voxelization (runtime generation)
 * 
 * The goal is to reproduce the exact output of the legacy WorldGenerator while
 * being extensible for future features like biomes, structures, etc.
 */
class SeedWorldGenerator {
public:
    /**
     * @brief Constructor with seed and parameters
     */
    SeedWorldGenerator(std::shared_ptr<WorldSeed> seed, std::shared_ptr<WorldParameters> parameters);

    /**
     * @brief Constructor with legacy compatibility mode
     * @param seed The world seed
     * @param parameters The world parameters
     * @param legacyCompatible If true, generates exactly like legacy system (no seed variation)
     */
    SeedWorldGenerator(std::shared_ptr<WorldSeed> seed, std::shared_ptr<WorldParameters> parameters, bool legacyCompatible);

    /**
     * @brief Generate a chunk segment with feature parity to legacy system
     * 
     * This is the main generation method that should produce identical results
     * to WorldGenerator::generateChunkSegment() for the same world coordinates.
     * 
     * @param segment The chunk segment to fill with voxel data
     * @param worldX World X coordinate (segment index)
     * @param worldY World Y coordinate (segment index) 
     * @param worldZ World Z coordinate (segment index)
     */
    void generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ);

    /**
     * @brief Generate chunk segment with enhanced features (future use)
     * 
     * This method will add new features beyond legacy parity while maintaining
     * backwards compatibility.
     */
    void generateChunkSegmentEnhanced(ChunkSegment& segment, int worldX, int worldY, int worldZ);

    /**
     * @brief Get the world seed used by this generator
     */
    std::shared_ptr<WorldSeed> getWorldSeed() const { return worldSeed_; }

    /**
     * @brief Get the world parameters used by this generator
     */
    std::shared_ptr<WorldParameters> getWorldParameters() const { return worldParameters_; }

    /**
     * @brief Set new world parameters (for runtime parameter tuning)
     */
    void setWorldParameters(std::shared_ptr<WorldParameters> parameters);

    /**
     * @brief Set the regional database for advanced world generation
     * @param database The regional database instance
     */
    void setRegionalDatabase(std::unique_ptr<RegionalDatabase> database);

    /**
     * @brief Get regional data for a specific coordinate
     * @param globalX Global X coordinate
     * @param globalZ Global Z coordinate
     * @return RegionalData for the region containing this coordinate, or default if no database
     */
    RegionalData getRegionalData(int globalX, int globalZ) const;

    /**
     * @brief Generate and store regional data for a region (used in world creation)
     * @param regionX Region X coordinate
     * @param regionZ Region Z coordinate
     * @return Generated RegionalData
     */
    RegionalData generateRegionalData(int regionX, int regionZ);

private:
    // Core systems
    std::shared_ptr<WorldSeed> worldSeed_;
    std::shared_ptr<WorldParameters> worldParameters_;
    std::mt19937_64 rng_;
    
    // Regional database for advanced world generation
    std::unique_ptr<RegionalDatabase> regionalDatabase_;
    
    // Compatibility mode flag
    bool legacyCompatible_;

    /**
     * @brief Generate terrain height for a global coordinate using legacy-compatible algorithm
     * 
     * This method replicates the exact noise calculation from the legacy system:
     * - Same noise input scale (0.02f)
     * - Same terrain amplitude (height * 1.5f)
     * - Same base terrain offset (height / 8.0f)
     */
    int generateTerrainHeight(int globalX, int globalZ) const;

    /**
     * @brief Determine voxel type based on height and position (legacy compatible)
     * 
     * Replicates the legacy block selection logic:
     * - Surface: GRASS
     * - 1-3 blocks below surface: DIRT  
     * - Deeper: STONE
     * - Above terrain: AIR
     */
    ::VoxelEngine::World::VoxelType getVoxelType(int globalY, int terrainHeight) const;

    /**
     * @brief Get seeded random number for specific coordinate (for future features)
     */
    uint64_t getCoordinateSeed(int64_t x, int64_t y, int64_t z) const;
};

} // namespace World
} // namespace VoxelCastle
