#pragma once

#include <string>
#include <chrono>
#include <cstdint>

namespace VoxelCastle {
namespace World {

/**
 * @brief World metadata structure for level.dat files
 * 
 * Represents the metadata stored in each world's level.dat file,
 * following Minecraft-style world persistence patterns.
 */
struct WorldMetadata {
    // Core world information
    int32_t version = 1;
    std::string worldName;
    uint64_t seed;
    std::string gameMode = "creative";  // "creative", "survival", "adventure"
    std::string worldType = "normal";   // "normal", "flat", "amplified"
    
    // Spawn point
    float spawnX = 0.0f;
    float spawnY = 64.0f;
    float spawnZ = 0.0f;
    
    // Generation settings
    bool generateStructures = true;
    
    // World generation statistics (optional, for UI display)
    int32_t worldSize = 1024;           // World size used during generation
    int32_t mountainRanges = 0;         // Number of mountain ranges generated
    int32_t majorRivers = 0;            // Number of major rivers
    int32_t biomesIdentified = 0;       // Number of biomes identified
    int32_t simulationYears = 0;        // Years simulated during generation
    float highestPeak = 0.0f;           // Highest peak elevation in meters
    std::string generatorName = "SeedWorldGenerator";  // Generator used
    std::string generatorVersion = "1.0";              // Generator version
    
    // Timestamps
    std::chrono::system_clock::time_point createdDate;
    std::chrono::system_clock::time_point lastPlayed;
    uint64_t playTime = 0;  // Total play time in seconds
    
    // Constructor
    WorldMetadata() = default;
    WorldMetadata(const std::string& name, uint64_t worldSeed);
    
    // Serialization
    bool SaveToFile(const std::string& worldPath) const;
    bool LoadFromFile(const std::string& worldPath);
    
    // Utility methods
    std::string GetCreatedDateString() const;
    std::string GetLastPlayedString() const;
    std::string GetPlayTimeString() const;
    
private:
    std::string FormatTimePoint(const std::chrono::system_clock::time_point& tp) const;
};

} // namespace World
} // namespace VoxelCastle
