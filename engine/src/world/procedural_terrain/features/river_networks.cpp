#include "world/procedural_terrain/features/river_networks.h"
#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include <cmath>
#include <algorithm>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

RiverData RiverNetworks::CalculateRiverData(float worldX, float worldZ, float elevation, 
                                           float precipitation, uint64_t seed) {
    RiverData riverData;
    
    // Calculate flow accumulation using watershed analysis
    riverData.flow_accumulation = CalculateFlowAccumulation(worldX, worldZ, elevation, precipitation, seed);
    
    // Determine river width based on flow accumulation
    riverData.river_width = CalculateRiverWidth(riverData.flow_accumulation, elevation, seed);
    
    // Calculate river depth based on width
    riverData.river_depth = CalculateRiverDepth(riverData.river_width, riverData.flow_accumulation);
    
    // Check if this should be a lake
    riverData.is_lake = IsLakeLocation(worldX, worldZ, elevation, riverData.flow_accumulation, seed);
    
    return riverData;
}

float RiverNetworks::CalculateFlowAccumulation(float worldX, float worldZ, float elevation,
                                              float precipitation, uint64_t seed) {
    float totalFlow = 0.0f;
    
    // Sample points in a radius around the current location to simulate watershed
    int sampleCount = 16; // Sample 16 points around the location
    float angleStep = 2.0f * M_PI / sampleCount;
    
    for (int i = 0; i < sampleCount; i++) {
        float angle = i * angleStep;
        float sampleX = worldX + std::cos(angle) * SAMPLING_RADIUS;
        float sampleZ = worldZ + std::sin(angle) * SAMPLING_RADIUS;
        
        // Get elevation at sample point using the same noise system
        float sampleElevation = 
            MultiScaleNoise::GenerateNoise(sampleX, sampleZ, TerrainScale::CONTINENTAL, seed) * 1400.0f +
            MultiScaleNoise::GenerateNoise(sampleX, sampleZ, TerrainScale::REGIONAL, seed + 1000) * 500.0f +
            MultiScaleNoise::GenerateNoise(sampleX, sampleZ, TerrainScale::LOCAL, seed + 2000) * 120.0f +
            MultiScaleNoise::GenerateNoise(sampleX, sampleZ, TerrainScale::MICRO, seed + 3000) * 30.0f;
        
        // If sample point is higher than current point, water flows from there to here
        if (sampleElevation > elevation) {
            float elevationDiff = sampleElevation - elevation;
            float flowContribution = elevationDiff / 100.0f; // Normalize elevation difference
            
            // Weight by precipitation at the sample point (more rain = more flow)
            float precipitationWeight = precipitation / 2000.0f; // Normalize precipitation
            
            totalFlow += flowContribution * precipitationWeight;
        }
    }
    
    // Add local precipitation contribution
    float localContribution = precipitation / 4000.0f; // Convert mm/year to flow factor
    totalFlow += localContribution;
    
    // Add watershed contribution using noise for natural variation
    float watershedNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::REGIONAL, seed + 5000);
    float watershedContribution = CalculateWatershedContribution(worldX, worldZ, precipitation, seed);
    totalFlow += watershedContribution * (0.5f + watershedNoise * 0.5f);
    
    return std::clamp(totalFlow, 0.0f, 1.0f);
}

float RiverNetworks::CalculateRiverWidth(float flowAccumulation, float elevation, uint64_t seed) {
    // Only create rivers above the flow threshold
    if (flowAccumulation < FLOW_THRESHOLD) {
        return 0.0f;
    }
    
    // Calculate base width from flow accumulation
    float baseWidth = (flowAccumulation - FLOW_THRESHOLD) * RIVER_WIDTH_SCALE;
    
    // Adjust for elevation - rivers are wider at lower elevations
    float elevationFactor = std::max(0.5f, 1.0f - (elevation / 1000.0f));
    
    // Add some natural variation using noise
    float variation = MultiScaleNoise::GenerateNoise(elevation, flowAccumulation, TerrainScale::LOCAL, seed + 6000);
    float variationFactor = 0.8f + (variation * 0.4f); // 0.8 to 1.2 multiplier
    
    return baseWidth * elevationFactor * variationFactor;
}

float RiverNetworks::CalculateRiverDepth(float riverWidth, float flowAccumulation) {
    if (riverWidth <= 0.0f) {
        return 0.0f;
    }
    
    // River depth is proportional to width, but not linear
    float baseDepth = riverWidth * DEPTH_WIDTH_RATIO;
    
    // Additional depth from flow accumulation
    float flowDepth = flowAccumulation * 2.0f; // Up to 2m additional depth
    
    return std::min(baseDepth + flowDepth, 10.0f); // Cap at 10m depth
}

bool RiverNetworks::IsLakeLocation(float worldX, float worldZ, float elevation,
                                  float flowAccumulation, uint64_t seed) {
    // Lakes form where there's high flow accumulation but low gradient
    if (flowAccumulation < LAKE_THRESHOLD) {
        return false;
    }
    
    // Check if we're in a depression (lower than surrounding area)
    float gradient = SampleElevationGradient(worldX, worldZ, seed);
    
    // Lakes form in areas with very low gradient and high flow
    bool isDepression = gradient < 0.1f; // Very flat area
    bool hasHighFlow = flowAccumulation > 0.8f;
    
    // Add some randomness for natural lake distribution
    float lakeNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::LOCAL, seed + 7000);
    bool randomLake = lakeNoise > 0.7f;
    
    return isDepression && (hasHighFlow || randomLake);
}

// Private helper functions
float RiverNetworks::SampleElevationGradient(float worldX, float worldZ, uint64_t seed) {
    float centerElevation = 
        MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::CONTINENTAL, seed) * 1400.0f +
        MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::REGIONAL, seed + 1000) * 500.0f +
        MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::LOCAL, seed + 2000) * 120.0f +
        MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::MICRO, seed + 3000) * 30.0f;
    
    // Sample elevation in 4 directions
    float sampleDistance = 100.0f; // 100m sampling distance
    float northElevation = 
        MultiScaleNoise::GenerateNoise(worldX, worldZ + sampleDistance, TerrainScale::CONTINENTAL, seed) * 1400.0f +
        MultiScaleNoise::GenerateNoise(worldX, worldZ + sampleDistance, TerrainScale::REGIONAL, seed + 1000) * 500.0f +
        MultiScaleNoise::GenerateNoise(worldX, worldZ + sampleDistance, TerrainScale::LOCAL, seed + 2000) * 120.0f +
        MultiScaleNoise::GenerateNoise(worldX, worldZ + sampleDistance, TerrainScale::MICRO, seed + 3000) * 30.0f;
    
    float eastElevation = 
        MultiScaleNoise::GenerateNoise(worldX + sampleDistance, worldZ, TerrainScale::CONTINENTAL, seed) * 1400.0f +
        MultiScaleNoise::GenerateNoise(worldX + sampleDistance, worldZ, TerrainScale::REGIONAL, seed + 1000) * 500.0f +
        MultiScaleNoise::GenerateNoise(worldX + sampleDistance, worldZ, TerrainScale::LOCAL, seed + 2000) * 120.0f +
        MultiScaleNoise::GenerateNoise(worldX + sampleDistance, worldZ, TerrainScale::MICRO, seed + 3000) * 30.0f;
    
    // Calculate maximum elevation difference
    float maxDiff = std::max(std::abs(northElevation - centerElevation),
                            std::abs(eastElevation - centerElevation));
    
    return maxDiff / sampleDistance; // Gradient in m/m
}

float RiverNetworks::CalculateWatershedContribution(float worldX, float worldZ, 
                                                   float precipitation, uint64_t seed) {
    // Use continental-scale noise to determine watershed boundaries
    float watershedNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::CONTINENTAL, seed + 8000);
    
    // Areas with high watershed noise collect more water from surrounding regions
    float watershedFactor = (watershedNoise + 1.0f) * 0.5f; // Convert from [-1,1] to [0,1]
    
    // Scale by precipitation - more rain means more watershed contribution
    float precipitationFactor = precipitation / 4000.0f; // Normalize to [0,1]
    
    return watershedFactor * precipitationFactor * 0.3f; // Up to 30% contribution
}

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
