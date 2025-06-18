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
    // Calculate hierarchical drainage area using multi-scale approach
    float continentalDrainage = CalculateContinentalDrainage(worldX, worldZ, seed);
    float regionalDrainage = CalculateRegionalDrainage(worldX, worldZ, elevation, seed);
    float localDrainage = CalculateLocalDrainage(worldX, worldZ, elevation, precipitation, seed);
    
    // Combine drainage contributions - continental rivers collect from vast areas
    float totalDrainage = continentalDrainage * 0.6f + regionalDrainage * 0.3f + localDrainage * 0.1f;
    
    // Add distance-to-ocean factor - rivers get larger as they approach the ocean
    float distanceToOcean = CalculateDistanceToOcean(worldX, worldZ, elevation, seed);
    float oceanProximityFactor = 1.0f - std::min(1.0f, distanceToOcean / 50000.0f); // 50km max distance
    
    // Rivers accumulate flow downstream
    float flowAccumulation = totalDrainage * (1.0f + oceanProximityFactor * 3.0f);
    
    return std::clamp(flowAccumulation, 0.0f, 1.0f);
}

float RiverNetworks::CalculateRiverWidth(float flowAccumulation, float elevation, uint64_t seed) {
    // Only create rivers above the flow threshold
    if (flowAccumulation < FLOW_THRESHOLD) {
        return 0.0f;
    }
    
    // Calculate stream order based on flow accumulation
    int streamOrder = CalculateStreamOrder(flowAccumulation);
    
    // Base width scales exponentially with stream order (Strahler's Law)
    // Order 1: 1-3m, Order 4: 30-100m, Order 7+: 200-1000m+
    float baseWidth = std::pow(3.5f, streamOrder - 1) * 1.5f;
    
    // Scale by actual flow accumulation within the order
    float flowFactor = (flowAccumulation - FLOW_THRESHOLD) / (1.0f - FLOW_THRESHOLD);
    baseWidth *= (0.5f + flowFactor * 1.5f); // 0.5x to 2x variation within order
    
    // Distance to ocean factor - rivers get much wider near the ocean
    float distanceToOcean = CalculateDistanceToOcean(elevation * 10.0f, elevation * 5.0f, elevation, seed);
    float deltaFactor = 1.0f + (1.0f - std::min(1.0f, distanceToOcean / 10000.0f)) * 2.0f; // Up to 3x wider near ocean
    
    // Elevation factor - rivers are wider at lower elevations
    float elevationFactor = std::max(0.3f, 1.0f - (elevation / 1500.0f));
    
    // Natural variation
    float variation = MultiScaleNoise::GenerateNoise(elevation, flowAccumulation, TerrainScale::LOCAL, seed + 6000);
    float variationFactor = 0.7f + (variation * 0.6f); // 0.7 to 1.3 multiplier
    
    return baseWidth * deltaFactor * elevationFactor * variationFactor;
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

float RiverNetworks::CalculateContinentalDrainage(float worldX, float worldZ, uint64_t seed) {
    // Continental-scale drainage basins using large-scale noise
    float continentalNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::CONTINENTAL, seed + 8000);
    
    // Create major river corridors - areas with high continental drainage
    float riverCorridorNoise = MultiScaleNoise::GenerateNoise(worldX * 0.5f, worldZ * 0.5f, TerrainScale::CONTINENTAL, seed + 9000);
    
    // Combine to create major river basins
    float drainageBasin = (continentalNoise + riverCorridorNoise) * 0.5f;
    
    // Convert to positive drainage contribution
    return std::max(0.0f, (drainageBasin + 1.0f) * 0.5f);
}

float RiverNetworks::CalculateRegionalDrainage(float worldX, float worldZ, float elevation, uint64_t seed) {
    // Regional drainage patterns following terrain
    float regionalNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::REGIONAL, seed + 10000);
    
    // Higher elevation areas contribute more drainage downstream
    float elevationContribution = std::max(0.0f, elevation / 1000.0f);
    
    // Create tributary networks
    float tributaryNoise = MultiScaleNoise::GenerateNoise(worldX * 2.0f, worldZ * 2.0f, TerrainScale::REGIONAL, seed + 11000);
    
    return std::max(0.0f, (regionalNoise + elevationContribution + tributaryNoise) / 3.0f);
}

float RiverNetworks::CalculateLocalDrainage(float worldX, float worldZ, float elevation, float precipitation, uint64_t seed) {
    // Local drainage from immediate area
    float localNoise = MultiScaleNoise::GenerateNoise(worldX, worldZ, TerrainScale::LOCAL, seed + 12000);
    
    // Precipitation factor
    float precipitationFactor = precipitation / 2000.0f; // Normalize
    
    // Local terrain contribution
    float terrainFactor = std::max(0.0f, -elevation / 500.0f + 1.0f); // Lower areas collect more
    
    return std::max(0.0f, localNoise * precipitationFactor * terrainFactor);
}

float RiverNetworks::CalculateDistanceToOcean(float worldX, float worldZ, float elevation, uint64_t seed) {
    // Use continental noise to determine ocean locations
    float oceanNoise = MultiScaleNoise::GenerateNoise(worldX * 0.1f, worldZ * 0.1f, TerrainScale::CONTINENTAL, seed + 13000);
    
    // Oceans are areas with very low continental elevation
    float seaLevel = -200.0f;
    
    // Simulate distance to nearest ocean using noise patterns
    float distanceNoise = MultiScaleNoise::GenerateNoise(worldX * 0.3f, worldZ * 0.3f, TerrainScale::CONTINENTAL, seed + 14000);
    
    // Base distance on elevation above sea level and noise patterns
    float baseDistance = std::max(0.0f, elevation - seaLevel);
    float noiseDistance = (distanceNoise + 1.0f) * 25000.0f; // 0-50km range
    