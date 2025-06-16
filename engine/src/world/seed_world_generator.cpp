#include "world/seed_world_generator.h"
#include "world/biome/biome_types.h"
#include "world/biome/biome_registry.h"  // Add BiomeRegistry for biome-aware generation
#include "world/tectonic_simulator.h"
#include "world/HybridGeologicalSimulator.h"   // Hybrid geological simulation system
#include "util/noise.h"
#include "world/voxel_types.h"
#include "world/voxel.h"
#include "world/world_seed.h"
#include "world/world_coordinates.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>

namespace VoxelCastle {
namespace World {

SeedWorldGenerator::SeedWorldGenerator(std::shared_ptr<WorldSeed> seed, std::shared_ptr<WorldParameters> parameters)
    : worldSeed_(seed), worldParameters_(parameters), regionalDatabase_(nullptr), 
      geologicalSimulator_(nullptr), useGeologicalRealism_(false), tectonicSimulator_(nullptr), legacyCompatible_(false) {
    
    if (!worldSeed_) {
        throw std::invalid_argument("WorldSeed cannot be null");
    }
    if (!worldParameters_) {
        throw std::invalid_argument("WorldParameters cannot be null");
    }
    
    // Initialize RNG with the world seed
    rng_.seed(worldSeed_->getMasterSeed());
    
    std::cout << "[SeedWorldGenerator] Initialized with seed: " << worldSeed_->getMasterSeed() << " (standard mode)" << std::endl;
}

SeedWorldGenerator::SeedWorldGenerator(std::shared_ptr<WorldSeed> seed, std::shared_ptr<WorldParameters> parameters, bool useGeologicalRealism)
    : worldSeed_(seed), worldParameters_(parameters), regionalDatabase_(nullptr), 
      geologicalSimulator_(nullptr), useGeologicalRealism_(useGeologicalRealism), tectonicSimulator_(nullptr), legacyCompatible_(!useGeologicalRealism) {
    
    if (!worldSeed_) {
        throw std::invalid_argument("WorldSeed cannot be null");
    }
    if (!worldParameters_) {
        throw std::invalid_argument("WorldParameters cannot be null");
    }
    
    // Initialize RNG with the world seed
    rng_.seed(worldSeed_->getMasterSeed());
    
    std::cout << "[SeedWorldGenerator] Initialized with seed: " << worldSeed_->getMasterSeed() 
              << " (geological realism: " << (useGeologicalRealism ? "ENABLED" : "DISABLED") << ")" << std::endl;
}

void SeedWorldGenerator::generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ) {
    // Add debugging to track which chunks are being generated with timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    std::cout << "[" << ss.str() << "] [SEED_WORLD_GEN] Generating chunk segment at world coordinates (" 
        << std::to_string(worldX) << ", " << std::to_string(worldY) << ", " << std::to_string(worldZ) << ")" << std::endl;
        
    // Show world loading progress only for the first segment
    if (worldX == 0 && worldY == 0 && worldZ == 0) {
        std::cout << "[INFO] Seed-based world generation started" << std::endl;
    }
    
    std::cout << "[SeedWorldGenerator] Generating segment at indices: (" << std::to_string(worldX) 
              << ", " << std::to_string(worldY) << ", " << std::to_string(worldZ) << ")" << std::endl;

    constexpr int width = ChunkSegment::CHUNK_WIDTH;
    constexpr int height = ChunkSegment::CHUNK_HEIGHT; // e.g., 32
    constexpr int depth = ChunkSegment::CHUNK_DEPTH;

    // World Y coordinate of this segment's base
    int baseY = worldY * height;
    
    // Initialize BiomeRegistry if not already done
    if (!BiomeRegistry::isInitialized()) {
        BiomeRegistry::initialize();
    }

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            // Use true world coordinates for terrain height calculation
            int globalX = worldX + x;
            int globalZ = worldZ + z;
            
            // Generate terrain height using legacy-compatible algorithm
            int columnHeight = generateTerrainHeight(globalX, globalZ);

            // Debug: Log first few terrain heights to detect patterns with timestamp
            static int heightLogCount = 0;
            if (heightLogCount < 20) {
                auto now = std::chrono::system_clock::now();
                auto time_t = std::chrono::system_clock::to_time_t(now);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                std::stringstream ss;
                ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
                ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
                
                std::cout << "[" << ss.str() << "] [SEED_TERRAIN] Chunk(" << std::to_string(worldX) << "," 
                    << std::to_string(worldY) << "," << std::to_string(worldZ) << ") local(" 
                    << std::to_string(x) << "," << std::to_string(z) << ") global(" 
                    << std::to_string(globalX) << "," << std::to_string(globalZ) << ") height=" 
                    << std::to_string(columnHeight) << std::endl;
                heightLogCount++;
            }

            // Added for logging task 3.1 - log for local (0,0) and (15,15)
            if (worldX == 0 && worldY == 0 && worldZ == 0 && ((x == 0 && z == 0) || (x == 15 && z == 15))) {
                std::cout << "[SeedWorldGenerator] Local (" << std::to_string(x) << "," << std::to_string(z) << "): globalX=" << std::to_string(globalX) << ", globalZ=" << std::to_string(globalZ) << ", columnHeight=" << std::to_string(columnHeight) << std::endl;
            }

            // Fill the vertical column with appropriate voxel types
            for (int y = 0; y < height; ++y) {
                int globalY = baseY + y;
                
                // Get regional biome data for this location
                RegionalData regionalData = getRegionalData(globalX, globalZ);
                
                ::VoxelEngine::World::VoxelType type = getVoxelType(globalY, columnHeight, regionalData.primaryBiome);
                segment.setVoxel(x, y, z, ::VoxelEngine::World::Voxel(static_cast<uint8_t>(type)));

                // Added for logging task 3.1 - log for local (0,y,0) and (15,y,15)
                if (worldX == 0 && worldY == 0 && worldZ == 0 && ((x == 0 && z == 0) || (x == 15 && z == 15))) {
                    if (y == 0 || y == 15 || y == columnHeight || y == columnHeight + 1) { // Log specific y values
                         std::cout << "[SeedWorldGenerator] Local (" << std::to_string(x) << "," << std::to_string(y) << "," << std::to_string(z) << "): globalY=" << std::to_string(globalY) << ", type=" << std::to_string(static_cast<int>(type)) << std::endl;
                    }
                }
            }
        }
    }
}

void SeedWorldGenerator::generateChunkSegmentEnhanced(ChunkSegment& segment, int worldX, int worldY, int worldZ) {
    // For now, enhanced generation is the same as basic generation
    // Future enhancements will be added here while maintaining backwards compatibility
    generateChunkSegment(segment, worldX, worldY, worldZ);
    
    // TODO: Add biome-based generation
    // TODO: Add structure placement
    // TODO: Add ore distribution
    // TODO: Add cave generation
}

void SeedWorldGenerator::setWorldParameters(std::shared_ptr<WorldParameters> parameters) {
    if (!parameters) {
        throw std::invalid_argument("WorldParameters cannot be null");
    }
    worldParameters_ = parameters;
    std::cout << "[SeedWorldGenerator] Updated world parameters" << std::endl;
}

int SeedWorldGenerator::generateTerrainHeight(int globalX, int globalZ) const {
    // Use geological simulation if available and enabled
    if (useGeologicalRealism_ && geologicalSimulator_) {
        return generateTerrainHeightGeological(globalX, globalZ);
    }
    
    // Legacy compatibility mode
    if (legacyCompatible_) {
        // Legacy algorithm for compatibility testing
        constexpr float noiseInputScale = 0.02f;
        constexpr float terrainAmplitude = ChunkSegment::CHUNK_HEIGHT * 1.5f;
        constexpr float baseTerrainOffset = static_cast<float>(ChunkSegment::CHUNK_HEIGHT) / 8.0f;
        
        float nx = globalX * noiseInputScale;
        float nz = globalZ * noiseInputScale;
        float noise_val = VoxelEngine::Util::smoothValueNoise(nx, 0.0f, nz);
        int columnHeight = static_cast<int>(noise_val * terrainAmplitude) + static_cast<int>(baseTerrainOffset);
        return columnHeight;
    }
    
    // Enhanced terrain generation using full ±2048m vertical range
    // Use world coordinate constants for proper scaling
    using namespace VoxelCastle::World::WorldCoordinates;
    
    // Enhanced terrain parameters for full vertical range
    constexpr float baseNoiseScale = 0.005f;        // Large-scale continental features
    constexpr float detailNoiseScale = 0.02f;       // Medium-scale terrain detail
    constexpr float fineNoiseScale = 0.08f;         // Fine-scale surface variation
    
    // Terrain generation uses world coordinate system
    // Sea level is at Y=0, terrain can range from deep ocean floors to high mountains
    const float seaLevel = 0.0f;                    // World Y = 0 (sea level)
    const float maxMountainHeight = 500.0f;         // 500m above sea level
    const float maxOceanDepth = -300.0f;            // 300m below sea level
    const float terrainRange = maxMountainHeight - maxOceanDepth; // 800m total range
    
    // Get terrain-specific seed for consistent generation
    uint64_t terrainSeed = worldSeed_->getFeatureSeed(globalX / 1000, 0, globalZ / 1000, VoxelCastle::World::FeatureType::TERRAIN);
    
    // Create deterministic coordinate offsets for terrain variation using world seed
    uint64_t masterSeed = worldSeed_->getMasterSeed();
    float seedOffsetX = static_cast<float>((masterSeed & 0xFFFFFFFF) % 1000000) * 0.00001f;
    float seedOffsetZ = static_cast<float>(((masterSeed >> 32) & 0xFFFFFFFF) % 1000000) * 0.00001f;
    
    // Additional region-based variation using terrain seed
    float regionOffsetX = static_cast<float>((terrainSeed & 0xFFFF) % 10000) * 0.0001f;
    float regionOffsetZ = static_cast<float>(((terrainSeed >> 16) & 0xFFFF) % 10000) * 0.0001f;
    
    // Multi-scale noise generation for realistic terrain
    float nx = globalX;
    float nz = globalZ;
    
    // Large-scale continental structure (mountains, valleys, ocean basins)
    float continentalNoise = VoxelEngine::Util::smoothValueNoise(
        (nx + seedOffsetX + regionOffsetX) * baseNoiseScale, 
        0.0f, 
        (nz + seedOffsetZ + regionOffsetZ) * baseNoiseScale
    );
    
    // Medium-scale terrain features (hills, ridges, local topography)
    float terrainNoise = VoxelEngine::Util::smoothValueNoise(
        (nx + seedOffsetX + regionOffsetX) * detailNoiseScale, 
        1000.0f, // Different Y offset for different noise pattern
        (nz + seedOffsetZ + regionOffsetZ) * detailNoiseScale
    );
    
    // Fine-scale surface variation (small bumps, local detail)
    float surfaceNoise = VoxelEngine::Util::smoothValueNoise(
        (nx + seedOffsetX + regionOffsetX) * fineNoiseScale, 
        2000.0f, // Different Y offset for different noise pattern
        (nz + seedOffsetZ + regionOffsetZ) * fineNoiseScale
    );
    
    // Combine noise layers with different weights
    float combinedNoise = 
        continentalNoise * 0.6f +      // 60% continental structure
        terrainNoise * 0.3f +          // 30% terrain features  
        surfaceNoise * 0.1f;           // 10% surface detail
    
    // Convert noise (-1 to 1) to terrain height in world coordinates
    float terrainHeight = seaLevel + (combinedNoise * terrainRange * 0.5f);
    
    // Convert world coordinates to block coordinates (25cm blocks)
    int columnHeightBlocks = static_cast<int>(terrainHeight / VoxelCastle::World::WorldCoordinates::VOXEL_SIZE_METERS);
    
    return columnHeightBlocks;
}

::VoxelEngine::World::VoxelType SeedWorldGenerator::getVoxelType(int globalY, int terrainHeight) const {
    if (legacyCompatible_) {
        // Legacy voxel assignment for compatibility
        if (globalY <= terrainHeight) {
            if (globalY == terrainHeight) {
                return ::VoxelEngine::World::VoxelType::GRASS;
            } else if (globalY > terrainHeight - 3) {
                return ::VoxelEngine::World::VoxelType::DIRT;
            } else {
                return ::VoxelEngine::World::VoxelType::STONE;
            }
        } else {
            return ::VoxelEngine::World::VoxelType::AIR;
        }
    }
    
    // Enhanced voxel type assignment for full vertical range
    // Convert block coordinates to world coordinates for logic
    const float worldY = globalY * VoxelCastle::World::WorldCoordinates::VOXEL_SIZE_METERS;
    const float terrainWorldY = terrainHeight * VoxelCastle::World::WorldCoordinates::VOXEL_SIZE_METERS;
    
    // Above terrain: Air
    if (worldY > terrainWorldY) {
        return ::VoxelEngine::World::VoxelType::AIR;
    }
    
    // At or below terrain: Determine material based on depth and elevation
    const float depthBelowSurface = terrainWorldY - worldY;
    const float seaLevel = 0.0f;
    
    // Surface layer (top block)
    if (globalY == terrainHeight) {
        if (terrainWorldY > 200.0f) {
            // High altitude: Snow
            return ::VoxelEngine::World::VoxelType::STONE; // TODO: Add SNOW when available
        } else if (terrainWorldY > 100.0f) {
            // Mountains: Stone exposure
            return ::VoxelEngine::World::VoxelType::STONE;
        } else if (terrainWorldY > seaLevel) {
            // Above sea level: Grass
            return ::VoxelEngine::World::VoxelType::GRASS;
        } else {
            // Below sea level: Sand/underwater
            return ::VoxelEngine::World::VoxelType::SAND;
        }
    }
    
    // Subsurface layers based on depth
    if (depthBelowSurface <= 1.0f) { // 0-1m: Topsoil
        if (terrainWorldY > seaLevel) {
            return ::VoxelEngine::World::VoxelType::DIRT;
        } else {
            return ::VoxelEngine::World::VoxelType::SAND;
        }
    } else if (depthBelowSurface <= 4.0f) { // 1-4m: Subsoil
        return ::VoxelEngine::World::VoxelType::DIRT;
    } else if (depthBelowSurface <= 20.0f) { // 4-20m: Weathered rock
        return ::VoxelEngine::World::VoxelType::STONE;
    } else if (depthBelowSurface <= 100.0f) { // 20-100m: Bedrock
        return ::VoxelEngine::World::VoxelType::STONE;
    } else {
        // Deep underground: Different rock types based on depth
        // For now, use stone - future: implement geological layers
        return ::VoxelEngine::World::VoxelType::STONE;
    }
}

::VoxelEngine::World::VoxelType SeedWorldGenerator::getVoxelType(int globalY, int terrainHeight, BiomeType biome) const {
    if (legacyCompatible_) {
        // Use original method for legacy compatibility
        return getVoxelType(globalY, terrainHeight);
    }
    
    // Enhanced biome-aware voxel type assignment
    const float worldY = globalY * VoxelCastle::World::WorldCoordinates::VOXEL_SIZE_METERS;
    const float terrainWorldY = terrainHeight * VoxelCastle::World::WorldCoordinates::VOXEL_SIZE_METERS;
    
    // Debug output for testing
    if (globalY == terrainHeight || globalY == terrainHeight - 1) {
        std::cout << "[DEBUG_BIOME] globalY=" << globalY << ", terrainHeight=" << terrainHeight 
                  << ", worldY=" << worldY << ", terrainWorldY=" << terrainWorldY 
                  << ", biome=" << static_cast<int>(biome) << std::endl;
    }
    
    // Above terrain: Air
    if (worldY > terrainWorldY) {
        return ::VoxelEngine::World::VoxelType::AIR;
    }
    
    // Get biome data for surface material selection
    const BiomeData& biomeData = BiomeRegistry::getBiomeData(biome);
    
    const float depthBelowSurface = terrainWorldY - worldY;
    const float seaLevel = 0.0f;
    
    // Surface layer (top block) - use biome-specific surface material
    if (globalY == terrainHeight) {
        ::VoxelEngine::World::VoxelType surfaceType;
        if (terrainWorldY > 200.0f) {
            // High altitude: Snow or stone based on biome
            if (biome == BiomeType::MOUNTAINS) {
                surfaceType = ::VoxelEngine::World::VoxelType::STONE;
            } else {
                surfaceType = ::VoxelEngine::World::VoxelType::STONE; // TODO: Add SNOW when available
            }
        } else if (terrainWorldY > 100.0f) {
            // Mid-high altitude: Stone exposure for mountains, biome surface for others
            if (biome == BiomeType::MOUNTAINS) {
                surfaceType = ::VoxelEngine::World::VoxelType::STONE;
            } else {
                surfaceType = biomeData.surfaceBlock;
            }
        } else if (terrainWorldY > seaLevel) {
            // Above sea level: Use biome-specific surface material
            surfaceType = biomeData.surfaceBlock;
        } else {
            // Below sea level: Sand/underwater or biome-specific underwater material
            if (biome == BiomeType::OCEAN) {
                surfaceType = ::VoxelEngine::World::VoxelType::SAND; // Ocean floor
            } else {
                surfaceType = ::VoxelEngine::World::VoxelType::SAND; // Flooded areas
            }
        }
        
        // Debug output for surface block selection
        std::cout << "[DEBUG_SURFACE] Selected surface type: " << static_cast<int>(surfaceType) 
                  << ", biome surface: " << static_cast<int>(biomeData.surfaceBlock) << std::endl;
        return surfaceType;
    }
    
    // Subsurface layers - use biome-specific subsurface material for shallow depths
    if (depthBelowSurface <= 1.0f) { // 0-1m: Topsoil
        ::VoxelEngine::World::VoxelType subsurfaceType;
        if (terrainWorldY > seaLevel) {
            subsurfaceType = biomeData.subsurfaceBlock;
        } else {
            subsurfaceType = ::VoxelEngine::World::VoxelType::SAND;
        }
        
        // Debug output for subsurface block selection
        if (globalY == terrainHeight - 1) {
            std::cout << "[DEBUG_SUBSURFACE] Selected subsurface type: " << static_cast<int>(subsurfaceType) 
                      << ", biome subsurface: " << static_cast<int>(biomeData.subsurfaceBlock) << std::endl;
        }
        return subsurfaceType;
    } else if (depthBelowSurface <= 4.0f) { // 1-4m: Subsoil
        return biomeData.subsurfaceBlock;
    } else if (depthBelowSurface <= 20.0f) { // 4-20m: Weathered rock
        return ::VoxelEngine::World::VoxelType::STONE;
    } else if (depthBelowSurface <= 100.0f) { // 20-100m: Bedrock
        return ::VoxelEngine::World::VoxelType::STONE;
    } else {
        // Deep underground: Different rock types based on depth
        // For now, use stone - future: implement geological layers
        return ::VoxelEngine::World::VoxelType::STONE;
    }
}

uint64_t SeedWorldGenerator::getCoordinateSeed(int64_t x, int64_t y, int64_t z) const {
    // Use the WorldSeed to generate a coordinate-specific seed
    return worldSeed_->getBlockSeed(x, y, z);
}

void SeedWorldGenerator::setRegionalDatabase(std::unique_ptr<RegionalDatabase> database) {
    regionalDatabase_ = std::move(database);
    std::cout << "[SeedWorldGenerator] Regional database " << (regionalDatabase_ ? "enabled" : "disabled") << std::endl;
}

void SeedWorldGenerator::initializeTectonicSimulation(float worldSizeKm) {
    if (useGeologicalRealism_) {
        std::cout << "[SeedWorldGenerator] Using geological realism, skipping legacy tectonic simulation" << std::endl;
        return;
    }
    
    std::cout << "[SeedWorldGenerator] Initializing legacy tectonic simulation..." << std::endl;
    
    tectonicSimulator_ = std::make_unique<TectonicSimulator>();
    
    // Calculate number of plates based on world size (heuristic: ~1 plate per 100km)
    uint32_t plateCount = std::max(3u, static_cast<uint32_t>(worldSizeKm / 100.0f));
    
    tectonicSimulator_->InitializePlates(
        static_cast<uint32_t>(worldSeed_->getMasterSeed()),
        worldSizeKm,
        plateCount
    );
    
    std::cout << "[SeedWorldGenerator] Legacy tectonic simulation initialized with " << plateCount << " plates" << std::endl;
}

RegionalData SeedWorldGenerator::getRegionalData(int globalX, int globalZ) const {
    if (!regionalDatabase_) {
        // Return default regional data when no database is available
        RegionalData defaultData;
        defaultData.primaryBiome = BiomeType::PLAINS;
        defaultData.temperature = 20.0f;
        defaultData.humidity = 50.0f;
        defaultData.elevation = 64.0f;
        defaultData.precipitation = 800.0f; // mm per year
        return defaultData;
    }
    
    // Convert world coordinates to region coordinates
    int regionX = globalX / RegionalData::REGION_SIZE;
    int regionZ = globalZ / RegionalData::REGION_SIZE;
    
    // Try to load existing regional data
    RegionalData data;
    if (regionalDatabase_->GetRegionalData(regionX, regionZ, data)) {
        return data;
    }
    
    // Generate new regional data if not found
    return const_cast<SeedWorldGenerator*>(this)->generateRegionalData(regionX, regionZ);
}

RegionalData SeedWorldGenerator::generateRegionalData(int regionX, int regionZ) {
    RegionalData data;
    
    // Initialize header information
    data.magicNumber = RegionalData::MAGIC_NUMBER;
    data.version = RegionalData::CURRENT_VERSION;
    data.flags = 0;
    data.regionX = regionX;
    data.regionZ = regionZ;
    
    // Use coordinate-based seeding for consistent generation
    uint64_t regionSeed = getCoordinateSeed(regionX, 0, regionZ);
    std::mt19937_64 regionRng(regionSeed);
    
    // Convert region coordinates to world coordinates (center of region in km)
    float worldX = regionX * RegionalData::REGION_SIZE * 0.001f; // Convert to km
    float worldZ = regionZ * RegionalData::REGION_SIZE * 0.001f; // Convert to km
    
    // Generate tectonic data if simulator is available
    if (tectonicSimulator_) {
        // Get tectonic properties for this region
        glm::vec2 worldPos(worldX, worldZ);
        
        // Query tectonic simulator for geological properties
        const TectonicPlate* dominantPlate = tectonicSimulator_->GetDominantPlate(worldPos);
        uint32_t dominantPlateId = dominantPlate ? dominantPlate->plateId : 0;
        float tectonicStress = tectonicSimulator_->GetTectonicStress(worldPos);
        TerrainType terrainType = tectonicSimulator_->GetTerrainTypeAtPosition(worldPos);
        
        // Populate GeologicalData with tectonic simulation results
        data.geological.dominantPlateId = dominantPlateId;
        data.geological.tectonicStress = tectonicStress;
        data.geological.terrainType = terrainType;
        
        // Set crustal thickness based on terrain type and tectonic stress
        switch (terrainType) {
            case TerrainType::MOUNTAIN:
                data.geological.crustalThickness = 45.0f + tectonicStress * 20.0f; // Thick continental crust
                break;
            case TerrainType::OCEANIC:
                data.geological.crustalThickness = 7.0f + tectonicStress * 3.0f; // Thin oceanic crust
                break;
            case TerrainType::RIFT:
                data.geological.crustalThickness = 20.0f - tectonicStress * 10.0f; // Thinned crust
                break;
            case TerrainType::STABLE:
            default:
                data.geological.crustalThickness = 35.0f + tectonicStress * 5.0f; // Normal continental crust
                break;
        }
        
        // Get plate movement vector for this position
        if (dominantPlate) {
            data.geological.plateMovementVector = dominantPlate->movementVector;
        }
        
        // Set elevation based on terrain type and crustal thickness
        float baseElevation = 64.0f; // Sea level
        switch (terrainType) {
            case TerrainType::MOUNTAIN:
                data.elevation = baseElevation + 50.0f + tectonicStress * 100.0f;
                break;
            case TerrainType::OCEANIC:
                data.elevation = baseElevation - 20.0f - tectonicStress * 30.0f;
                break;
            case TerrainType::RIFT:
                data.elevation = baseElevation - 10.0f + tectonicStress * 20.0f;
                break;
            case TerrainType::STABLE:
            default:
                data.elevation = baseElevation + tectonicStress * 10.0f;
                break;
        }
        
        // Determine biome based on elevation and terrain type
        if (data.elevation > 100.0f) {
            data.primaryBiome = BiomeType::MOUNTAINS;
        } else if (data.elevation < 50.0f) {
            data.primaryBiome = BiomeType::OCEAN;
        } else if (terrainType == TerrainType::RIFT) {
            data.primaryBiome = BiomeType::DESERT; // Rift valleys tend to be dry
        } else {
            data.primaryBiome = BiomeType::PLAINS; // Default for stable terrain
        }
        
    } else {
        // Fallback to simple random biome assignment if no tectonic simulation
        std::uniform_int_distribution<int> biomeDist(0, static_cast<int>(BiomeType::COUNT) - 1);
        data.primaryBiome = static_cast<BiomeType>(biomeDist(regionRng));
        
        // Generate basic elevation
        std::uniform_real_distribution<float> elevationDist(32.0f, 128.0f);
        data.elevation = elevationDist(regionRng);
    }
    
    // Generate basic environmental parameters (can be refined with climate simulation later)
    std::uniform_real_distribution<float> tempDist(-10.0f, 35.0f);
    std::uniform_real_distribution<float> humidityDist(0.0f, 100.0f);
    std::uniform_real_distribution<float> precipitationDist(200.0f, 2000.0f);
    
    data.temperature = tempDist(regionRng);
    data.humidity = humidityDist(regionRng);
    data.precipitation = precipitationDist(regionRng);
    
    // Clear reserved space
    std::memset(data.reserved, 0, sizeof(data.reserved));
    
    // Save to database if available
    if (regionalDatabase_) {
        if (!regionalDatabase_->SetRegionalData(regionX, regionZ, data)) {
            std::cerr << "[SeedWorldGenerator] Warning: Failed to save regional data for region (" 
                      << regionX << ", " << regionZ << ")" << std::endl;
        } else {
            std::cout << "[SeedWorldGenerator] Generated and saved regional data for region (" 
                      << regionX << ", " << regionZ << ") - Biome: " 
                      << static_cast<int>(data.primaryBiome) 
                      << ", PlateId: " << data.geological.dominantPlateId
                      << ", TerrainType: " << static_cast<int>(data.geological.terrainType) << std::endl;
        }
    }
    
    return data;
}

void SeedWorldGenerator::generatePreviewHeightmap(int centerX, int centerZ, int sampleRadius, int resolution, float* heightData) const {
    if (!heightData) {
        throw std::invalid_argument("heightData cannot be null");
    }
    
    if (resolution <= 0) {
        throw std::invalid_argument("resolution must be positive");
    }
    
    // Calculate sampling parameters
    int minX = centerX - sampleRadius;
    int maxX = centerX + sampleRadius;
    int minZ = centerZ - sampleRadius;
    int maxZ = centerZ + sampleRadius;
    
    int worldRangeX = maxX - minX;
    int worldRangeZ = maxZ - minZ;
    
    // Sample terrain heights at regular intervals
    for (int sampleZ = 0; sampleZ < resolution; sampleZ++) {
        for (int sampleX = 0; sampleX < resolution; sampleX++) {
            // Map sample coordinates to world coordinates
            int worldX = minX + (sampleX * worldRangeX) / (resolution - 1);
            int worldZ = minZ + (sampleZ * worldRangeZ) / (resolution - 1);
            
            // Get terrain height at this world coordinate
            int terrainHeight = generateTerrainHeight(worldX, worldZ);
            
            // Convert block height to world meters for consistent scaling
            float heightInMeters = terrainHeight * VoxelCastle::World::WorldCoordinates::VOXEL_SIZE_METERS;
            
            // Store in output array (row-major order)
            heightData[sampleZ * resolution + sampleX] = heightInMeters;
        }
    }
}

int SeedWorldGenerator::getTerrainHeightAt(int globalX, int globalZ) const {
    return generateTerrainHeight(globalX, globalZ);
}

void SeedWorldGenerator::initializeGeologicalSimulation(float worldSizeKm, const GeologicalConfig& config, 
                                                        std::function<void(const PhaseInfo&)> progressCallback) {
    if (!useGeologicalRealism_) {
        std::cout << "[SeedWorldGenerator] Geological realism disabled, skipping geological simulation initialization" << std::endl;
        return;
    }
    
    std::cout << "[SeedWorldGenerator] Initializing geological simulation system..." << std::endl;
    
    // Pass the world size in km directly to the geological simulator
    // The geological simulator will handle converting this to appropriate resolution
    int worldSizeKmInt = static_cast<int>(worldSizeKm);
    
    // Create hybrid geological simulator
    geologicalSimulator_ = std::make_unique<HybridGeologicalSimulator>(worldSizeKmInt, worldSeed_->getMasterSeed());
    
    // Store progress callback
    geologicalProgressCallback_ = progressCallback;
    
    // Initialize with continental configuration
    geologicalSimulator_->Initialize(4, 0.71f); // 4 continents, 71% ocean
    
    std::cout << "[SeedWorldGenerator] Geological simulation initialized for " << worldSizeKm << "km world" << std::endl;
}

bool SeedWorldGenerator::runGeologicalSimulation() {
    if (!useGeologicalRealism_ || !geologicalSimulator_) {
        std::cout << "[SeedWorldGenerator] No geological simulator available" << std::endl;
        return false;
    }
    
    std::cout << "[SeedWorldGenerator] Starting geological simulation..." << std::endl;
    
    try {
        // Run simulation steps until complete
        while (!geologicalSimulator_->IsComplete()) {
            geologicalSimulator_->RunSimulationStep(1000.0f); // 1000 year steps
            
            // Report progress if callback available
            if (geologicalProgressCallback_) {
                float progress = geologicalSimulator_->GetProgress();
                PhaseInfo phaseInfo;
                phaseInfo.currentPhase = GeologicalPhase::TECTONICS; // Simplified for now
                phaseInfo.phaseProgress = progress;
                phaseInfo.totalProgress = progress;
                phaseInfo.currentProcess = "Hybrid Geological Simulation";
                phaseInfo.timeRemaining = 0.0f;
                geologicalProgressCallback_(phaseInfo);
            }
        }
        
        std::cout << "[SeedWorldGenerator] Geological simulation completed successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SeedWorldGenerator] ERROR: Geological simulation failed: " << e.what() << std::endl;
        return false;
    }
}

// Step-based geological simulation methods
bool SeedWorldGenerator::initializeStepBasedGeologicalSimulation() {
    if (!useGeologicalRealism_ || !geologicalSimulator_) {
        std::cout << "[SeedWorldGenerator] No geological simulator available for step-based simulation" << std::endl;
        return false;
    }
    
    std::cout << "[SeedWorldGenerator] Initializing step-based geological simulation..." << std::endl;
    return geologicalSimulator_->InitializeSimulation();
}

bool SeedWorldGenerator::stepGeologicalSimulation() {
    if (!useGeologicalRealism_ || !geologicalSimulator_) {
        return false;
    }
    
    return geologicalSimulator_->StepSimulation();
}

bool SeedWorldGenerator::isGeologicalSimulationComplete() const {
    if (!useGeologicalRealism_ || !geologicalSimulator_) {
        return true; // Consider complete if not using geological simulation
    }
    
    return geologicalSimulator_->IsSimulationComplete();
}

void SeedWorldGenerator::pauseGeologicalSimulation() {
    if (useGeologicalRealism_ && geologicalSimulator_) {
        geologicalSimulator_->PauseSimulation();
    }
}

void SeedWorldGenerator::resumeGeologicalSimulation() {
    if (useGeologicalRealism_ && geologicalSimulator_) {
        geologicalSimulator_->ResumeSimulation();
    }
}

bool SeedWorldGenerator::isGeologicalSimulationPaused() const {
    if (!useGeologicalRealism_ || !geologicalSimulator_) {
        return false;
    }
    
    return geologicalSimulator_->IsSimulationPaused();
}

int SeedWorldGenerator::generateTerrainHeightGeological(int globalX, int globalZ) const {
    if (!geologicalSimulator_) {
        std::cerr << "[SeedWorldGenerator] ERROR: Geological simulator not available, falling back to noise" << std::endl;
        // Fall back to legacy noise-based generation
        return generateTerrainHeight(globalX, globalZ);
    }
    
    // Convert block coordinates to world meters
    using namespace VoxelCastle::World::WorldCoordinates;
    float worldX = globalX * VOXEL_SIZE_METERS;
    float worldZ = globalZ * VOXEL_SIZE_METERS;
    
    // Sample geological data at this position
    GeologicalSample sample = geologicalSimulator_->GetSampleAt(worldX, worldZ);
    
    // Convert geological elevation (meters) to block height
    int blockHeight = static_cast<int>(sample.elevation / VOXEL_SIZE_METERS);
    
    return blockHeight;
}

} // namespace World
} // namespace VoxelCastle
