#pragma once

#include <cstdint>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

/**
 * @brief Climate data for a specific location
 * 
 * Contains all environmental parameters that affect biome selection
 * and terrain generation at a specific location.
 */
struct ClimateData {
    float temperature;    ///< Temperature in Celsius (-30°C to +50°C)
    float humidity;       ///< Humidity ratio (0.0 to 1.0)
    float precipitation;  ///< Annual precipitation in mm/year equivalent
    float windExposure;   ///< Wind exposure factor (0.0 to 1.0)
    float seasonality;    ///< Seasonal variation intensity (0.0 to 1.0)
    
    ClimateData() 
        : temperature(20.0f)
        , humidity(0.5f)
        , precipitation(800.0f)
        , windExposure(0.5f)
        , seasonality(0.3f) {}
};

/**
 * @brief Core climate calculation system
 * 
 * Provides deterministic climate calculation for any world position
 * using only the world seed and coordinates.
 */
class ClimateSystem {
public:
    /**
     * @brief Calculate climate data for a specific location
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param worldSeed World generation seed
     * @return Complete climate data for the location
     */
    static ClimateData CalculateClimate(int chunkX, int chunkZ, uint64_t worldSeed);
    
    /**
     * @brief Get base temperature from world-scale patterns
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param worldSeed World generation seed
     * @return Base temperature before elevation adjustments
     */
    static float GetBaseTemperature(int chunkX, int chunkZ, uint64_t worldSeed);
    
    /**
     * @brief Get base humidity from world-scale patterns
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param worldSeed World generation seed
     * @return Base humidity before local modifications
     */
    static float GetBaseHumidity(int chunkX, int chunkZ, uint64_t worldSeed);
    
    /**
     * @brief Calculate distance to nearest virtual ocean
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param worldSeed World generation seed
     * @return Distance to ocean in chunk units
     */
    static float GetDistanceToOcean(int chunkX, int chunkZ, uint64_t worldSeed);
    
    /**
     * @brief Calculate rain shadow effect from virtual mountains
     * @param chunkX Chunk X coordinate
     * @param chunkZ Chunk Z coordinate
     * @param worldSeed World generation seed
     * @return Rain shadow multiplier (0.0 to 1.0)
     */
    static float GetRainShadowEffect(int chunkX, int chunkZ, uint64_t worldSeed);
    
    /**
     * @brief Apply elevation adjustment to temperature
     * @param baseTemperature Base temperature
     * @param elevation Elevation in meters
     * @return Temperature adjusted for elevation
     */
    static float ApplyElevationLapse(float baseTemperature, float elevation);

private:
    // Climate generation constants
    static constexpr float TEMPERATURE_LAPSE_RATE = 0.0067f;  // °C per meter
    static constexpr float MOISTURE_DECAY_DISTANCE = 50.0f;   // chunks
    static constexpr float RAIN_SHADOW_STRENGTH = 0.7f;       // effect multiplier
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
