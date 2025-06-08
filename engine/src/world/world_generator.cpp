// engine/src/world/world_generator.cpp
#include "world/world_generator.h"
#include "world/world_seed.h"
#include "util/noise.h"
#include "util/enhanced_noise.h"
#include "world/voxel_types.h"
#include "world/voxel.h"
#include "utils/debug_logger_stub.h"
#include <cmath>
#include <iostream> // Added for logging
#include <functional> // For std::hash
#include <chrono>    // For timestamps
#include <iomanip>   // For time formatting
#include <sstream>   // For string streams

namespace VoxelCastle {
namespace World {

// Constructor initializes with a default random seed
WorldGenerator::WorldGenerator() : legacySeed_(0) {
    // Set a default random seed
    std::random_device rd;
    uint64_t seed = rd();
    setSeed(seed);
}

// Set world seed from a numeric value
void WorldGenerator::setSeed(uint64_t seed) {
    legacySeed_ = seed;
    rng_.seed(seed);
    worldSeed_ = std::make_unique<WorldSeed>(seed);
    DEBUG_LOG("WorldGenerator", "World seed set to: " + std::to_string(seed));
}

// Set world seed from a string (converts to numeric seed)
void WorldGenerator::setSeedFromString(const std::string& seedString) {
    // Convert string to numeric seed using hash
    std::hash<std::string> hasher;
    uint64_t seed = hasher(seedString);
    legacySeed_ = seed;
    rng_.seed(seed);
    worldSeed_ = std::make_unique<WorldSeed>(seedString);
    DEBUG_LOG("WorldGenerator", "World seed generated from string: '" + seedString + "' = " + std::to_string(seed));
}

// Static version of getBlockSeed - uses a default seed for consistent results
uint64_t WorldGenerator::staticGetBlockSeed(int64_t x, int64_t y, int64_t z) {
    // Fixed default seed for static contexts - will be used when no instance is available
    constexpr uint64_t DEFAULT_STATIC_SEED = 12345678901234567890ULL;
    
    // Simple but effective position hash combined with default seed
    uint64_t posHash = (x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
    return DEFAULT_STATIC_SEED ^ posHash;
}

// Get unique seed for a specific block position (legacy compatibility)
uint64_t WorldGenerator::getBlockSeed(int64_t x, int64_t y, int64_t z) const {
    // Use legacy method for backward compatibility
    uint64_t posHash = (x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
    return legacySeed_ ^ posHash;
}

// Get enhanced seeds for specific features and scales
uint64_t WorldGenerator::getFeatureSeed(int64_t x, int64_t y, int64_t z, FeatureType feature) const {
    return worldSeed_->getFeatureSeed(x, y, z, feature);
}

uint64_t WorldGenerator::getScaleSeed(int64_t x, int64_t y, int64_t z, ScaleLevel scale, FeatureType feature) const {
    return worldSeed_->getScaleSeed(x, y, z, scale, feature);
}

// Generate chunk segment with the current world seed
void WorldGenerator::generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ) {
    // Add debugging to track which chunks are being generated with timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    VoxelCastle::Utils::logToFile("[" + ss.str() + "] [WORLD_GEN] Generating chunk segment at world coordinates (" + 
        std::to_string(worldX) + ", " + std::to_string(worldY) + ", " + std::to_string(worldZ) + ")");
        
    // Show world loading progress only for the first segment
    if (worldX == 0 && worldY == 0 && worldZ == 0) {
        std::cout << "[INFO] World generation started" << std::endl;
    }
    
    DEBUG_LOG("WorldGenerator", "Generating segment at indices: (" + std::to_string(worldX) + 
              ", " + std::to_string(worldY) + ", " + std::to_string(worldZ) + ")");

    constexpr int width = ChunkSegment::CHUNK_WIDTH;
    constexpr int height = ChunkSegment::CHUNK_HEIGHT; // e.g., 32
    constexpr int depth = ChunkSegment::CHUNK_DEPTH;

    // --- TUNABLE PARAMETERS ---
    const float noiseInputScale = 0.02f;         // Reduced for wider, more stretched terrain features
    const float terrainAmplitude = height * 1.5f;  // Increased amplitude (e.g., 32 * 1.5 = 48)
    const float baseTerrainOffset = static_cast<float>(height) / 8.0f; // Adjusted base offset (e.g., 32 / 8 = 4)
    // --- END TUNABLE PARAMETERS ---

    // World Y coordinate of this segment's base
    int baseY = worldY * height;

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            // Use noise to get height
            // FIX: Use true world coordinates for noise input
            int globalX = worldX + x;
            int globalZ = worldZ + z;
            float nx = globalX * noiseInputScale;
            float nz = globalZ * noiseInputScale;
            float noise_val = VoxelEngine::Util::smoothValueNoise(nx, 0.0f, nz); // Renamed to noise_val to avoid conflict

            // Assuming noise_val is in [0, 1]. If not, it might need clamping/remapping:
            // noise_val = std::max(0.0f, std::min(1.0f, noise_val));

            int columnHeight = static_cast<int>(noise_val * terrainAmplitude) + static_cast<int>(baseTerrainOffset);

            // Debug: Log first few terrain heights to detect patterns with timestamp
            static int heightLogCount = 0;
            if (heightLogCount < 20) {
                auto now = std::chrono::system_clock::now();
                auto time_t = std::chrono::system_clock::to_time_t(now);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                std::stringstream ss;
                ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
                ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
                
                VoxelCastle::Utils::logToFile("[" + ss.str() + "] [TERRAIN] Chunk(" + std::to_string(worldX) + "," + 
                    std::to_string(worldY) + "," + std::to_string(worldZ) + ") local(" + 
                    std::to_string(x) + "," + std::to_string(z) + ") global(" + 
                    std::to_string(globalX) + "," + std::to_string(globalZ) + ") noise=" + 
                    std::to_string(noise_val) + " height=" + std::to_string(columnHeight));
                heightLogCount++;
            }

            // Added for logging task 3.1 - log for local (0,0) and (15,15)
            if (worldX == 0 && worldY == 0 && worldZ == 0 && ((x == 0 && z == 0) || (x == 15 && z == 15))) {
                DEBUG_LOG("WorldGenerator", "Local (" + std::to_string(x) + "," + std::to_string(z) + "): nx=" + std::to_string(nx) + ", nz=" + std::to_string(nz) + ", noise_val=" + std::to_string(noise_val) + ", columnHeight=" + std::to_string(columnHeight));
            }

            for (int y = 0; y < height; ++y) {
                int globalY = baseY + y;
                ::VoxelEngine::World::VoxelType type = ::VoxelEngine::World::VoxelType::AIR;
                if (globalY <= columnHeight) {
                    if (globalY == columnHeight) {
                        type = ::VoxelEngine::World::VoxelType::GRASS;
                    } else if (globalY > columnHeight - 3) {
                        type = ::VoxelEngine::World::VoxelType::DIRT;
                    } else {
                        type = ::VoxelEngine::World::VoxelType::STONE;
                    }
                }
                segment.setVoxel(x, y, z, ::VoxelEngine::World::Voxel(static_cast<uint8_t>(type)));

                // Added for logging task 3.1 - log for local (0,y,0) and (15,y,15)
                if (worldX == 0 && worldY == 0 && worldZ == 0 && ((x == 0 && z == 0) || (x == 15 && z == 15))) {
                    if (y == 0 || y == 15 || y == columnHeight || y == columnHeight + 1) { // Log specific y values
                         DEBUG_LOG("WorldGenerator", "Local (" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "): globalY=" + std::to_string(globalY) + ", type=" + std::to_string(static_cast<int>(type)));
                    }
                }
            }
        }
    }
}

// Enhanced terrain generation using the new seed system
void WorldGenerator::generateChunkSegmentEnhanced(ChunkSegment& segment, int worldX, int worldY, int worldZ) {
    // Add debugging to track which chunks are being generated with timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    VoxelCastle::Utils::logToFile("[" + ss.str() + "] [ENHANCED_GEN] Generating enhanced chunk segment at world coordinates (" + 
        std::to_string(worldX) + ", " + std::to_string(worldY) + ", " + std::to_string(worldZ) + ")");
        
    // Show world loading progress only for the first segment
    if (worldX == 0 && worldY == 0 && worldZ == 0) {
        std::cout << "[INFO] Enhanced world generation started with seed: " << worldSeed_->getSeedString() << std::endl;
    }
    
    DEBUG_LOG("WorldGenerator", "Enhanced generation at indices: (" + std::to_string(worldX) + 
              ", " + std::to_string(worldY) + ", " + std::to_string(worldZ) + ")");

    constexpr int width = ChunkSegment::CHUNK_WIDTH;
    constexpr int height = ChunkSegment::CHUNK_HEIGHT; // e.g., 32
    constexpr int depth = ChunkSegment::CHUNK_DEPTH;

    // --- ENHANCED TUNABLE PARAMETERS ---
    const float mountainScale = 0.005f;        // Large-scale mountain features
    const float hillScale = 0.02f;             // Medium-scale hill features
    const float detailScale = 0.1f;            // Fine detail features
    const float terrainAmplitude = height * 2.0f; // Increased for more dramatic terrain
    const float baseTerrainOffset = static_cast<float>(height) / 4.0f; // Base terrain level
    // --- END ENHANCED PARAMETERS ---

    // World Y coordinate of this segment's base
    int baseY = worldY * height;

    // Create enhanced noise generator
    VoxelEngine::Util::EnhancedNoise noise(*worldSeed_);

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            // Use true world coordinates for noise input
            int globalX = worldX + x;
            int globalZ = worldZ + z;
            
            // Multi-scale terrain generation using different feature types
            float mountainHeight = noise.scaleAwareNoise(globalX * mountainScale, 0.0f, globalZ * mountainScale, 
                                                        ScaleLevel::CONTINENTAL, FeatureType::TERRAIN);
            
            float hillHeight = noise.scaleAwareNoise(globalX * hillScale, 0.0f, globalZ * hillScale, 
                                                    ScaleLevel::REGION, FeatureType::TERRAIN);
            
            float detailHeight = noise.scaleAwareNoise(globalX * detailScale, 0.0f, globalZ * detailScale, 
                                                      ScaleLevel::CHUNK, FeatureType::TERRAIN);
            
            // Combine different scales with varying weights
            float combinedNoise = mountainHeight * 0.6f + hillHeight * 0.3f + detailHeight * 0.1f;
            
            // Apply ridged noise for mountain peaks
            float ridgedMountains = noise.ridgedNoise(globalX * mountainScale * 0.5f, 0.0f, globalZ * mountainScale * 0.5f, 
                                                     4, 0.4f, FeatureType::TERRAIN);
            
            // Blend ridged mountains with base terrain
            float finalHeight = std::max(combinedNoise, ridgedMountains * 0.8f);
            
            int columnHeight = static_cast<int>(finalHeight * terrainAmplitude) + static_cast<int>(baseTerrainOffset);

            // Debug: Log first few terrain heights to detect patterns with timestamp
            static int enhancedHeightLogCount = 0;
            if (enhancedHeightLogCount < 10) {
                auto now = std::chrono::system_clock::now();
                auto time_t = std::chrono::system_clock::to_time_t(now);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                std::stringstream ss;
                ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
                ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
                
                VoxelCastle::Utils::logToFile("[" + ss.str() + "] [ENHANCED_TERRAIN] Chunk(" + std::to_string(worldX) + "," + 
                    std::to_string(worldY) + "," + std::to_string(worldZ) + ") local(" + 
                    std::to_string(x) + "," + std::to_string(z) + ") global(" + 
                    std::to_string(globalX) + "," + std::to_string(globalZ) + ") " +
                    "mountain=" + std::to_string(mountainHeight) + " hill=" + std::to_string(hillHeight) + 
                    " detail=" + std::to_string(detailHeight) + " ridged=" + std::to_string(ridgedMountains) +
                    " final=" + std::to_string(finalHeight) + " height=" + std::to_string(columnHeight));
                enhancedHeightLogCount++;
            }

            // Generate cave systems using separate feature domain (currently just for logging)
            // bool hasCave = false; // Commented out for now to avoid unused variable warning
            if (baseY < columnHeight) { // Only check for caves below surface
                for (int y = 0; y < height; ++y) {
                    int globalY = baseY + y;
                    if (globalY > 10 && globalY < columnHeight - 5) { // Cave generation zone
                        float caveNoise = noise.smoothValueNoise(globalX * 0.05f, globalY * 0.05f, globalZ * 0.05f, FeatureType::CAVES);
                        if (caveNoise > 0.6f) { // Cave threshold
                            // hasCave = true; // Could be used later for biome generation
                            break;
                        }
                    }
                }
            }

            for (int y = 0; y < height; ++y) {
                int globalY = baseY + y;
                ::VoxelEngine::World::VoxelType type = ::VoxelEngine::World::VoxelType::AIR;
                
                // Check for caves first
                if (globalY > 10 && globalY < columnHeight - 5) {
                    float caveNoise = noise.smoothValueNoise(globalX * 0.05f, globalY * 0.05f, globalZ * 0.05f, FeatureType::CAVES);
                    if (caveNoise > 0.6f) {
                        type = ::VoxelEngine::World::VoxelType::AIR; // Cave
                        segment.setVoxel(x, y, z, ::VoxelEngine::World::Voxel(static_cast<uint8_t>(type)));
                        continue;
                    }
                }
                
                // Standard terrain generation
                if (globalY <= columnHeight) {
                    if (globalY == columnHeight) {
                        type = ::VoxelEngine::World::VoxelType::GRASS;
                    } else if (globalY > columnHeight - 3) {
                        type = ::VoxelEngine::World::VoxelType::DIRT;
                    } else {
                        // Add ore generation using separate feature domain
                        float oreNoise = noise.smoothValueNoise(globalX * 0.1f, globalY * 0.1f, globalZ * 0.1f, FeatureType::ORES);
                        if (oreNoise > 0.95f && globalY < 64) { // Very rare ores
                            type = ::VoxelEngine::World::VoxelType::STONE; // Would be ore type in future
                        } else {
                            type = ::VoxelEngine::World::VoxelType::STONE;
                        }
                    }
                }
                
                segment.setVoxel(x, y, z, ::VoxelEngine::World::Voxel(static_cast<uint8_t>(type)));

                // Enhanced logging for key positions
                if (worldX == 0 && worldY == 0 && worldZ == 0 && ((x == 0 && z == 0) || (x == 15 && z == 15))) {
                    if (y == 0 || y == 15 || y == columnHeight || y == columnHeight + 1) {
                         DEBUG_LOG("WorldGenerator", "Enhanced Local (" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "): globalY=" + std::to_string(globalY) + ", type=" + std::to_string(static_cast<int>(type)) + ", seed=" + std::to_string(worldSeed_->getBlockSeed(globalX, globalY, globalZ)));
                    }
                }
            }
        }
    }
}

} // namespace World
} // namespace VoxelCastle
