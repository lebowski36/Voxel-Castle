#include "world/procedural_terrain/climate/climate_system.h"
#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include <cmath>
#include <algorithm>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

ClimateData ClimateSystem::CalculateClimate(float worldX, float worldZ, float elevation, uint64_t seed) {
    ClimateData climate;
    
    // Calculate base temperature from latitude and continental effects
    float baseTemperature = CalculateBaseTemperature(worldX, worldZ, seed);
    
    // Apply elevation temperature lapse rate (-6.5°C per 1000m)
    climate.temperature = baseTemperature - (elevation * TEMPERATURE_LAPSE_RATE);
    
    // Calculate humidity from ocean distance and rain shadow
    climate.humidity = CalculateHumidity(worldX, worldZ, elevation, seed);
    
    // Calculate precipitation from temperature, humidity, and orographic effects
    climate.precipitation = CalculatePrecipitation(worldX, worldZ, climate.temperature, 
                                                  climate.humidity, elevation, seed);
    
    // Calculate wind exposure and seasonality
    climate.windExposure = CalculateWindExposure(worldX, worldZ, elevation, seed);
    climate.seasonality = CalculateSeasonality(worldX, worldZ, seed);
    
    return climate;
}

float ClimateSystem::CalculateBaseTemperature(float worldX, float worldZ, uint64_t seed) {
    // Create virtual latitude from world coordinates
    // Use large-scale noise to create temperature bands similar to Earth's climate zones
    float latitude = std::abs(worldZ) / 100000.0f; // Rough latitude equivalent
    
    // Base temperature from latitude (tropical to polar)
    float latitudeTemp = 30.0f - (latitude * 35.0f); // 30°C at equator, -5°C at poles
    
    // Add continental climate variation (large-scale temperature noise)
    float continentalNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, 
                                                           TerrainScale::CONTINENTAL, 
                                                           seed + 5000);
    float continentalVariation = continentalNoise * 15.0f; // ±15°C variation
    
    // Add regional temperature variation (e.g., local climate zones)
    float regionalNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, 
                                                        TerrainScale::REGIONAL, 
                                                        seed + 6000);
    float regionalVariation = regionalNoise * 5.0f; // ±5°C variation
    
    return latitudeTemp + continentalVariation + regionalVariation;
}

float ClimateSystem::CalculateHumidity(float worldX, float worldZ, float elevation, uint64_t seed) {
    // Distance to virtual ocean (using continental-scale noise to define ocean locations)
    float oceanDistance = GetDistanceToVirtualOcean(worldX, worldZ, seed);
    
    // Base humidity decreases exponentially with distance from ocean
    float baseHumidity = std::exp(-oceanDistance / MOISTURE_DECAY_DISTANCE);
    
    // Rain shadow effects from mountains
    float rainShadow = CalculateRainShadowEffect(worldX, worldZ, seed);
    float shadowedHumidity = baseHumidity * (1.0f - rainShadow * RAIN_SHADOW_STRENGTH);
    
    // Elevation effects - higher elevations tend to be drier
    float elevationFactor = std::max(0.5f, 1.0f - (elevation / 3000.0f));
    
    // Local humidity variation
    float localNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::LOCAL, seed + 7000);
    float localVariation = 1.0f + (localNoise * 0.3f); // ±30% variation
    
    float finalHumidity = shadowedHumidity * elevationFactor * localVariation;
    return std::clamp(finalHumidity, 0.0f, 1.0f);
}

float ClimateSystem::CalculatePrecipitation(float worldX, float worldZ, float temperature, 
                                           float humidity, float elevation, uint64_t seed) {
    // Base precipitation from temperature and humidity
    // Warmer air holds more moisture, higher humidity means more precipitation potential
    float temperatureFactor = std::max(0.1f, (temperature + 20.0f) / 70.0f); // Scale 0.1-1.0
    float basePrecipitation = humidity * temperatureFactor * 2000.0f; // 0-2000mm base
    
    // Orographic precipitation enhancement
    float orographicBonus = CalculateOrographicPrecipitation(elevation, temperature);
    
    // Seasonal precipitation variation
    float seasonalNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::REGIONAL, seed + 8000);
    float seasonalVariation = 1.0f + (seasonalNoise * 0.5f); // ±50% seasonal variation
    
    // Regional precipitation patterns (monsoons, trade winds, etc.)
    float regionalNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::CONTINENTAL, seed + 9000);
    float regionalFactor = 0.5f + (regionalNoise * 0.5f); // 0.5-1.5x multiplier
    
    float totalPrecipitation = (basePrecipitation + orographicBonus) * seasonalVariation * regionalFactor;
    return std::max(0.0f, std::min(4000.0f, totalPrecipitation)); // Clamp to realistic range
}

float ClimateSystem::CalculateWindExposure(float worldX, float worldZ, float elevation, uint64_t seed) {
    // Base exposure from elevation (higher = more exposed)
    float elevationExposure = std::min(1.0f, elevation / 1500.0f);
    
    // Topographic shelter calculation
    // Check if position is in a valley or on a ridge
    float localSlope = std::abs(MultiScaleNoise::GenerateNoise(worldX + 100, worldZ, TerrainScale::LOCAL, seed) -
                               MultiScaleNoise::GenerateNoise(worldX - 100, worldZ, TerrainScale::LOCAL, seed));
    float slopeExposure = std::min(1.0f, localSlope * 2.0f);
    
    // Regional wind pattern variation
    float windNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::REGIONAL, seed + 10000);
    float windVariation = 0.7f + (windNoise * 0.3f); // 0.7-1.0 multiplier
    
    return std::clamp((elevationExposure + slopeExposure) * 0.5f * windVariation, 0.0f, 1.0f);
}

float ClimateSystem::CalculateSeasonality(float worldX, float worldZ, uint64_t seed) {
    // Distance from virtual equator affects seasonal variation
    float latitude = std::abs(worldZ) / 100000.0f;
    float latitudeSeasonality = std::min(1.0f, latitude * 2.0f); // More seasonal away from equator
    
    // Continental vs maritime climate (continental = more seasonal)
    float oceanDistance = GetDistanceToVirtualOcean(worldX, worldZ, seed);
    float continentalEffect = std::min(1.0f, oceanDistance / MOISTURE_DECAY_DISTANCE);
    
    // Regional climate variation
    float regionalNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::CONTINENTAL, seed + 11000);
    float regionalVariation = 0.5f + (regionalNoise * 0.5f); // 0.5-1.0 multiplier
    
    return std::clamp((latitudeSeasonality + continentalEffect) * 0.5f * regionalVariation, 0.0f, 1.0f);
}

// Private helper functions
float ClimateSystem::GetDistanceToVirtualOcean(float worldX, float worldZ, uint64_t seed) {
    // Use continental-scale noise to determine if we're near a virtual ocean
    // This creates realistic moisture gradients from coast to interior
    float oceanNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::CONTINENTAL, seed + 12000);
    
    // Convert noise to distance (negative noise = near ocean, positive = inland)
    float virtualDistance = (oceanNoise + 0.5f) * MOISTURE_DECAY_DISTANCE;
    return std::max(0.0f, virtualDistance);
}

float ClimateSystem::CalculateRainShadowEffect(float worldX, float worldZ, uint64_t seed) {
    // Calculate if position is in the rain shadow of mountains
    // This is a simplified model - real rain shadows depend on wind direction
    
    // Get elevation gradient to detect mountain barriers
    float elevationHere = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::REGIONAL, seed);
    float elevationUpwind = MultiScaleNoise::GenerateNoise(worldX - 5000, worldZ, TerrainScale::REGIONAL, seed);
    
    // If there's a significant mountain barrier upwind, create rain shadow
    float mountainBarrier = std::max(0.0f, elevationUpwind - elevationHere);
    float rainShadowStrength = std::min(1.0f, mountainBarrier * 2.0f);
    
    return rainShadowStrength;
}

float ClimateSystem::CalculateOrographicPrecipitation(float elevation, float baseTemperature) {
    // Orographic lift increases precipitation on mountain slopes
    // Only applies if temperature is above freezing (otherwise snow)
    if (baseTemperature < 0.0f || elevation < 200.0f) {
        return 0.0f; // No orographic effect for cold or low areas
    }
    
    // Precipitation increases with elevation up to a maximum
    float elevationBonus = std::min(800.0f, elevation * OROGRAPHIC_LIFT_FACTOR);
    return elevationBonus;
}

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
