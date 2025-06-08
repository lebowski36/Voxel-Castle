#pragma once

#include "biome/biome_types.h"
#include "geological_data.h"
#include "hydrological_data.h"
#include "climate_data.h"
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <iomanip>

namespace VoxelCastle {
namespace World {

/**
 * Regional environmental data structure for world template storage.
 * This structure represents environmental data for a 1km² region and is
 * designed to be efficiently serialized to binary format.
 * 
 * Version 2.0 - Added geological, hydrological, and climate data structures
 * Compatible with advanced world generation simulation systems.
 */
struct RegionalData {
    static constexpr uint32_t CURRENT_VERSION = 2;
    static constexpr uint32_t MAGIC_NUMBER = 0x52454743; // "REGC" 
    static constexpr int32_t REGION_SIZE = 1000; // Region size in world units (1km)
    
    // Header information
    uint32_t magicNumber;       // File format validation
    uint32_t version;           // Format version for compatibility
    uint32_t flags;             // Feature flags and validation
    uint32_t dataSize;          // Size of data section for validation
    
    // Coordinate information
    int32_t regionX;            // Region X coordinate
    int32_t regionZ;            // Region Z coordinate
    
    // Basic environmental data (legacy compatibility)
    BiomeType primaryBiome;     // Primary biome type
    float temperature;          // Average temperature (°C) - superseded by climate data
    float humidity;             // Average humidity percentage (0-100) - superseded by climate data
    float elevation;            // Average elevation (meters above sea level)
    float precipitation;        // Annual precipitation (mm) - superseded by climate data
    
    // Advanced environmental data systems
    GeologicalData geological;  // Comprehensive geological information
    HydrologicalData hydrological; // Water systems and drainage
    ClimateData climate;        // Detailed climate and weather patterns
    
    // Generation metadata
    uint64_t generationSeed;    // Seed used for this region's generation
    uint32_t generationTime;    // Time when this region was generated (unix timestamp)
    uint8_t simulationLevel;    // Level of simulation detail applied (0-100)
    
    // Future expansion space (reduced due to advanced data)
    uint8_t reserved[16];       // Reserved for future features
    
    // Constructors
    RegionalData();
    RegionalData(int32_t x, int32_t z, BiomeType biome, 
                 float temp, float humid, float elev);
    
    // Serialization methods
    bool SerializeToBinary(std::vector<uint8_t>& buffer) const;
    bool DeserializeFromBinary(const std::vector<uint8_t>& buffer);
    
    // File I/O methods
    bool SaveToFile(const std::string& filepath) const;
    bool LoadFromFile(const std::string& filepath);
    
    // Validation methods
    bool IsValid() const;
    void SetDefaults();
    
    // Utility methods
    std::string GetRegionFilename() const;
    static std::string GetRegionFilename(int32_t x, int32_t z);
    
    // Debug methods
    std::string ToString() const;
};

/**
 * Regional database manager for handling multiple region files
 * and providing efficient access to regional environmental data.
 */
class RegionalDatabase {
public:
    RegionalDatabase();
    ~RegionalDatabase();
    
    // Database operations
    bool Initialize(const std::string& worldPath);
    void Shutdown();
    
    // Region data access
    bool GetRegionalData(int32_t x, int32_t z, RegionalData& data) const;
    bool SetRegionalData(int32_t x, int32_t z, const RegionalData& data);
    
    // Batch operations
    bool LoadRegionBatch(const std::vector<std::pair<int32_t, int32_t>>& coordinates);
    void ClearCache();
    
    // File management
    bool CreateWorldDirectory(const std::string& worldPath) const;
    std::string GetRegionFilePath(int32_t x, int32_t z) const;
    
    // Statistics and debugging
    size_t GetCacheSize() const;
    std::vector<std::pair<int32_t, int32_t>> GetLoadedRegions() const;
    
private:
    std::string worldPath_;
    std::string regionsPath_;
    
    // Simple cache for frequently accessed regions
    mutable std::map<std::pair<int32_t, int32_t>, RegionalData> cache_;
    mutable std::map<std::pair<int32_t, int32_t>, uint64_t> cacheTimestamps_;
    
    static constexpr size_t MAX_CACHE_SIZE = 256; // Keep 256 regions in cache
    
    // Internal methods
    void EvictOldestCacheEntry() const;
    uint64_t GetCurrentTimestamp() const;
};

} // namespace World
} // namespace VoxelCastle
