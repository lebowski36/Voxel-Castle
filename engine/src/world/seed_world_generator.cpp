#include "world/seed_world_generator.h"
#include "world/biome/biome_types.h"
#include "world/tectonic_simulator.h"
#include "util/noise.h"
#include "world/voxel_types.h"
#include "world/voxel.h"
#include "world/world_seed.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>

namespace VoxelCastle {
namespace World {

SeedWorldGenerator::SeedWorldGenerator(std::shared_ptr<WorldSeed> seed, std::shared_ptr<WorldParameters> parameters)
    : worldSeed_(seed), worldParameters_(parameters), regionalDatabase_(nullptr), tectonicSimulator_(nullptr), legacyCompatible_(false) {
    
    if (!worldSeed_) {
        throw std::invalid_argument("WorldSeed cannot be null");
    }
    if (!worldParameters_) {
        throw std::invalid_argument("WorldParameters cannot be null");
    }
    
    // Initialize RNG with the world seed
    rng_.seed(worldSeed_->getMasterSeed());
    
    std::cout << "[SeedWorldGenerator] Initialized with seed: " << worldSeed_->getMasterSeed() << std::endl;
}

SeedWorldGenerator::SeedWorldGenerator(std::shared_ptr<WorldSeed> seed, std::shared_ptr<WorldParameters> parameters, bool legacyCompatible)
    : worldSeed_(seed), worldParameters_(parameters), regionalDatabase_(nullptr), tectonicSimulator_(nullptr), legacyCompatible_(legacyCompatible) {
    
    if (!worldSeed_) {
        throw std::invalid_argument("WorldSeed cannot be null");
    }
    if (!worldParameters_) {
        throw std::invalid_argument("WorldParameters cannot be null");
    }
    
    // Initialize RNG with the world seed
    rng_.seed(worldSeed_->getMasterSeed());
    
    std::cout << "[SeedWorldGenerator] Initialized with seed: " << worldSeed_->getMasterSeed() 
              << " (legacy compatible: " << (legacyCompatible ? "true" : "false") << ")" << std::endl;
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
                ::VoxelEngine::World::VoxelType type = getVoxelType(globalY, columnHeight);
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
    // Replicate exact legacy algorithm for feature parity
    
    // Legacy parameters - these MUST match WorldGenerator::generateChunkSegment exactly
    constexpr float noiseInputScale = 0.02f;         // Horizontal frequency
    constexpr float terrainAmplitude = ChunkSegment::CHUNK_HEIGHT * 1.5f;  // Vertical scale (48 for 32-height)
    constexpr float baseTerrainOffset = static_cast<float>(ChunkSegment::CHUNK_HEIGHT) / 8.0f; // Base height offset (4)

    // Calculate noise coordinates (exact legacy algorithm)
    float nx = globalX * noiseInputScale;
    float nz = globalZ * noiseInputScale;
    
    // Apply seed-based coordinate offsets only if NOT in legacy compatibility mode
    if (!legacyCompatible_) {
        // Use terrain-specific seed from WorldSeed for variation
        uint64_t terrainSeed = worldSeed_->getFeatureSeed(0, 0, 0, VoxelCastle::World::FeatureType::TERRAIN);
        
        // Create deterministic but varied coordinate offsets based on terrain seed
        float seedOffsetX = static_cast<float>((terrainSeed & 0xFFFF) % 10000) * 0.001f; // Range [0, 10)
        float seedOffsetZ = static_cast<float>(((terrainSeed >> 16) & 0xFFFF) % 10000) * 0.001f; // Range [0, 10)
        
        // Apply seed-based offsets to noise coordinates
        nx += seedOffsetX;
        nz += seedOffsetZ;
    }
    
    // Use the same noise function as legacy system
    float noise_val = VoxelEngine::Util::smoothValueNoise(nx, 0.0f, nz);
    
    // Convert to height using exact legacy formula
    int columnHeight = static_cast<int>(noise_val * terrainAmplitude) + static_cast<int>(baseTerrainOffset);
    
    return columnHeight;
}

::VoxelEngine::World::VoxelType SeedWorldGenerator::getVoxelType(int globalY, int terrainHeight) const {
    // Replicate exact legacy block selection logic
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

uint64_t SeedWorldGenerator::getCoordinateSeed(int64_t x, int64_t y, int64_t z) const {
    // Use the WorldSeed to generate a coordinate-specific seed
    return worldSeed_->getBlockSeed(x, y, z);
}

void SeedWorldGenerator::setRegionalDatabase(std::unique_ptr<RegionalDatabase> database) {
    regionalDatabase_ = std::move(database);
    std::cout << "[SeedWorldGenerator] Regional database " << (regionalDatabase_ ? "enabled" : "disabled") << std::endl;
}

void SeedWorldGenerator::initializeTectonicSimulation(float worldSizeKm) {
    // Create tectonic simulator
    tectonicSimulator_ = std::make_unique<TectonicSimulator>();
    
    // Use the region-level seed with a specific feature type for tectonics
    uint64_t tectonicSeed = worldSeed_->getRegionFeatureSeed(0, 0, 0, FeatureType::TERRAIN);
    
    // Initialize plates with world-specific parameters
    uint32_t plateCount = 0; // Auto-determine plate count based on world size
    tectonicSimulator_->InitializePlates(static_cast<uint32_t>(tectonicSeed), worldSizeKm, plateCount);
    
    // Run the simulation
    float simulationTime = 100.0f; // Million years
    uint32_t timeSteps = 10;
    tectonicSimulator_->SimulatePlateMovement(simulationTime, timeSteps);
    
    std::cout << "[SeedWorldGenerator] Tectonic simulation initialized with " 
              << tectonicSimulator_->GetPlates().size() << " plates over " 
              << worldSizeKm << "x" << worldSizeKm << " km world" << std::endl;
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

} // namespace World
} // namespace VoxelCastle
