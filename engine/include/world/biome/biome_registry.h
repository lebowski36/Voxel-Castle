#pragma once

#include "biome_data.h"
#include <vector>
#include <memory>

namespace VoxelCastle {
namespace World {

/**
 * @brief Central registry for all biome data and biome-related utilities
 * 
 * The BiomeRegistry is a singleton that manages all biome definitions and provides
 * convenient access methods for biome data lookup and climate-based biome selection.
 */
class BiomeRegistry {
public:
    /**
     * @brief Initialize the biome registry with default biome data
     * Must be called once before using any other methods
     */
    static void initialize();
    
    /**
     * @brief Check if the registry has been initialized
     * @return true if initialize() has been called successfully
     */
    static bool isInitialized();
    
    /**
     * @brief Get biome data for a specific biome type
     * @param type The biome type to look up
     * @return BiomeData for the specified type
     * @throws std::runtime_error if registry not initialized or invalid type
     */
    static const BiomeData& getBiomeData(BiomeType type);
    
    /**
     * @brief Find the best matching biome for given climate conditions
     * @param temperature Temperature value (-1.0 to 1.0)
     * @param humidity Humidity value (0.0 to 1.0)
     * @return BiomeType that best matches the climate conditions
     */
    static BiomeType getBiomeFromTemperatureHumidity(float temperature, float humidity);
    
    /**
     * @brief Get all available biome data
     * @return Vector containing all biome data entries
     */
    static const std::vector<BiomeData>& getAllBiomes();
    
    /**
     * @brief Get count of available biomes
     * @return Number of biomes registered
     */
    static size_t getBiomeCount();
    
    /**
     * @brief Cleanup and reset the registry (for testing)
     */
    static void cleanup();

private:
    // Static registry of all biome data
    static std::vector<BiomeData> biomeRegistry_;
    static bool initialized_;
    
    // Default biome creation methods (one for each biome)
    static BiomeData createPlainsData();
    static BiomeData createForestData();
    static BiomeData createDesertData();
    static BiomeData createMountainsData();
    static BiomeData createOceanData();
    
    // Helper methods
    static void validateBiomeData(const BiomeData& data);
    static float calculateClimateMatch(const BiomeData& biome, float temperature, float humidity);
    
    // Private constructor (singleton pattern)
    BiomeRegistry() = delete;
    BiomeRegistry(const BiomeRegistry&) = delete;
    BiomeRegistry& operator=(const BiomeRegistry&) = delete;
};

} // namespace World
} // namespace VoxelCastle
