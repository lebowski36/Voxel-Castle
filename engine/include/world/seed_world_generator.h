#pragma once

#include "world/world_seed.h"
#include "world/world_parameters.h"
#include "world/chunk_segment.h"
#include "world/regional_data.h"
#include "world/tectonic_simulator.h"
#include "world/GeologicalSimulator.h"  // New geological simulation system
#include "world/biome/biome_types.h"  // For BiomeType enum
#include "world/biome/biome_data.h"   // For BiomeData struct
#include <memory>
#include <random>
#include <functional>
#include <iostream>

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
     * @brief Constructor with geological realism
     * @param seed The world seed
     * @param parameters The world parameters
     * @param useGeologicalRealism If true, uses new geological simulation system; if false, uses legacy mode
     */
    SeedWorldGenerator(std::shared_ptr<WorldSeed> seed, std::shared_ptr<WorldParameters> parameters, bool useGeologicalRealism);

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
     * @brief Generate world preview data for UI visualization
     * 
     * Creates a lightweight heightmap sample for world configuration preview.
     * This method samples terrain heights at regular intervals across the specified
     * world region to create a preview visualization.
     * 
     * @param centerX World center X coordinate for preview
     * @param centerZ World center Z coordinate for preview  
     * @param sampleRadius Radius in world coordinates to sample around center
     * @param resolution Number of samples per axis (creates resolution x resolution grid)
     * @param heightData Output array to fill with height values (must be resolution*resolution size)
     */
    void generatePreviewHeightmap(int centerX, int centerZ, int sampleRadius, int resolution, float* heightData) const;

    /**
     * @brief Get terrain height at a specific world coordinate
     * 
     * Public accessor for terrain height calculation to support preview generation
     * and other systems that need height information without full chunk generation.
     * 
     * @param globalX Global world X coordinate
     * @param globalZ Global world Z coordinate
     * @return Terrain height at the specified coordinate
     */
    int getTerrainHeightAt(int globalX, int globalZ) const;

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
     * @brief Initialize the geological simulation system (replaces tectonic simulation)
     * @param worldSize World size parameters for geological simulation
     * @param config Geological quality configuration
     * @param progressCallback Optional callback for generation progress updates
     */
    void initializeGeologicalSimulation(float worldSizeKm = 2048.0f, const GeologicalConfig& config = GeologicalConfig{}, 
                                       std::function<void(const PhaseInfo&)> progressCallback = nullptr);

    /**
     * @brief Run the complete geological simulation (3-phase system)
     * @return True if simulation completed successfully
     */
    bool runGeologicalSimulation();

    /**
     * @brief Get the geological simulator instance
     * @return Pointer to GeologicalSimulator, nullptr if not initialized
     */
    const GeologicalSimulator* getGeologicalSimulator() const { 
        std::cout << "[SeedWorldGenerator] getGeologicalSimulator() called - useGeologicalRealism_: " << useGeologicalRealism_ 
                  << ", geologicalSimulator_ exists: " << (geologicalSimulator_ ? "YES" : "NO") << std::endl;
        return geologicalSimulator_.get(); 
    }

    /**
     * @brief Check if geological realism is enabled
     */
    bool isGeologicalRealismEnabled() const { return useGeologicalRealism_; }

    /**
     * @brief Initialize the tectonic simulation system (legacy)
     * @param worldSize World size parameters for tectonic simulation
     */
    void initializeTectonicSimulation(float worldSizeKm = 2048.0f);

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

    /**
     * @brief Get the tectonic simulator instance
     * @return Pointer to TectonicSimulator, nullptr if not initialized
     */
    const TectonicSimulator* getTectonicSimulator() const { return tectonicSimulator_.get(); }

private:
    // Core systems
    std::shared_ptr<WorldSeed> worldSeed_;
    std::shared_ptr<WorldParameters> worldParameters_;
    std::mt19937_64 rng_;
    
    // Regional database for advanced world generation
    std::unique_ptr<RegionalDatabase> regionalDatabase_;
    
    // Geological simulation system (NEW)
    std::unique_ptr<GeologicalSimulator> geologicalSimulator_;
    bool useGeologicalRealism_;
    std::function<void(const PhaseInfo&)> geologicalProgressCallback_;
    
    // Tectonic simulation system (LEGACY)
    std::unique_ptr<TectonicSimulator> tectonicSimulator_;
    
    // Compatibility mode flag
    bool legacyCompatible_;

    /**
     * @brief Generate terrain height using geological simulation or legacy algorithm
     * 
     * This method chooses between:
     * - Geological simulation results (if enabled and available)
     * - Legacy noise-based calculation (for compatibility/fallback)
     */
    int generateTerrainHeight(int globalX, int globalZ) const;

    /**
     * @brief Generate terrain height using geological simulation results
     * 
     * Samples the geological elevation field at the specified coordinates
     * and converts to block-level terrain height.
     */
    int generateTerrainHeightGeological(int globalX, int globalZ) const;

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
     * @brief Determine voxel type for a given position, terrain height, and biome
     * 
     * Enhanced version that uses biome-specific surface and subsurface materials
     * to create visually distinct biomes while maintaining realistic layering.
     */
    ::VoxelEngine::World::VoxelType getVoxelType(int globalY, int terrainHeight, BiomeType biome) const;

    /**
     * @brief Get seeded random number for specific coordinate (for future features)
     */
    uint64_t getCoordinateSeed(int64_t x, int64_t y, int64_t z) const;
};

} // namespace World
} // namespace VoxelCastle
