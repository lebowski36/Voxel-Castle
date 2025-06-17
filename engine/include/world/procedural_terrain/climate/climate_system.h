#pragma once

#include <cstdint>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

/**
 * Climate data structure containing all environmental parameters
 * Used for biome selection and feature generation
 */
struct ClimateData {
    float temperature;    // °C (-30 to +50)
    float humidity;       // 0.0 to 1.0 (relative humidity)
    float precipitation;  // mm/year (0 to 4000+)
    float windExposure;   // 0.0 to 1.0 (exposure to prevailing winds)
    float seasonality;    // 0.0 to 1.0 (temperature variation between seasons)
};

/**
 * Advanced climate calculation system for procedural terrain generation
 * All functions are deterministic and derive climate from world coordinates and seed
 */
class ClimateSystem {
public:
    /**
     * Calculate comprehensive climate data for any world position
     * @param worldX World X coordinate (in voxel scale units)
     * @param worldZ World Z coordinate (in voxel scale units)
     * @param elevation Terrain elevation at this position (meters)
     * @param seed World generation seed
     * @return Complete climate data for biome selection
     */
    static ClimateData CalculateClimate(float worldX, float worldZ, float elevation, uint64_t seed);

    /**
     * Calculate base temperature from latitude and continental effects
     * @param worldX World X coordinate
     * @param worldZ World Z coordinate  
     * @param seed World generation seed
     * @return Base temperature in °C before elevation adjustment
     */
    static float CalculateBaseTemperature(float worldX, float worldZ, uint64_t seed);

    /**
     * Calculate humidity from distance to virtual oceans and rain shadow effects
     * @param worldX World X coordinate
     * @param worldZ World Z coordinate
     * @param elevation Terrain elevation (affects moisture retention)
     * @param seed World generation seed
     * @return Humidity value from 0.0 to 1.0
     */
    static float CalculateHumidity(float worldX, float worldZ, float elevation, uint64_t seed);

    /**
     * Calculate annual precipitation from temperature, humidity, and topographic effects
     * @param worldX World X coordinate
     * @param worldZ World Z coordinate
     * @param temperature Base temperature at this location
     * @param humidity Humidity at this location
     * @param elevation Terrain elevation (orographic effects)
     * @param seed World generation seed
     * @return Annual precipitation in mm/year
     */
    static float CalculatePrecipitation(float worldX, float worldZ, float temperature, 
                                       float humidity, float elevation, uint64_t seed);

    /**
     * Calculate wind exposure from topographic position and prevailing wind patterns
     * @param worldX World X coordinate
     * @param worldZ World Z coordinate
     * @param elevation Terrain elevation
     * @param seed World generation seed
     * @return Wind exposure from 0.0 (sheltered) to 1.0 (very exposed)
     */
    static float CalculateWindExposure(float worldX, float worldZ, float elevation, uint64_t seed);

    /**
     * Calculate seasonal temperature variation
     * @param worldX World X coordinate (latitude effect)
     * @param worldZ World Z coordinate
     * @param seed World generation seed
     * @return Seasonality factor from 0.0 (stable) to 1.0 (extreme seasons)
     */
    static float CalculateSeasonality(float worldX, float worldZ, uint64_t seed);

private:
    // Climate constants
    static constexpr float TEMPERATURE_LAPSE_RATE = 0.0065f;  // °C per meter elevation
    static constexpr float MOISTURE_DECAY_DISTANCE = 50000.0f; // meters
    static constexpr float RAIN_SHADOW_STRENGTH = 0.7f;       // reduction factor
    static constexpr float OROGRAPHIC_LIFT_FACTOR = 0.5f;     // precipitation enhancement
    
    // Helper functions
    static float GetDistanceToVirtualOcean(float worldX, float worldZ, uint64_t seed);
    static float CalculateRainShadowEffect(float worldX, float worldZ, uint64_t seed);
    static float CalculateOrographicPrecipitation(float elevation, float baseTemperature);
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
