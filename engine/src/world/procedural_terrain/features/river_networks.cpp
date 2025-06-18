#include "world/procedural_terrain/features/river_networks.h"
#include "world/procedural_terrain/climate/climate_system.h"
#include "world/procedural_terrain/noise/fractal_patterns.h"
#include "world/procedural_terrain/utils/seed_utils.h"
#include "util/enhanced_noise.h"
#include <algorithm>
#include <cmath>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

// Helper function for fractal noise generation
static float FractalNoise(float x, float z, uint64_t seed) {
    // Use seed-based noise generation - simplified for now
    // In production, this would use a proper noise function with the seed
    uint64_t hash = seed + static_cast<uint64_t>(x * 1000000) + static_cast<uint64_t>(z * 1000);
    hash = (hash ^ (hash >> 16)) * 0x45d9f3b;
    hash = (hash ^ (hash >> 16)) * 0x45d9f3b;
    hash = hash ^ (hash >> 16);
    return (hash % 1000000) / 1000000.0f;
}

// Helper function for geological noise
static float GeologicalNoise(float x, float z, uint64_t seed) {
    return FractalNoise(x * 0.001f, z * 0.001f, seed);
}

// Helper function to get terrain elevation (placeholder - would integrate with actual terrain system)
static float GetElevation(float worldX, float worldZ, uint64_t seed) {
    // Simplified elevation calculation for river carving
    // In production, this would call the actual terrain generation system
    float continental = FractalNoise(worldX * 0.00001f, worldZ * 0.00001f, seed + 100000);
    float regional = FractalNoise(worldX * 0.0001f, worldZ * 0.0001f, seed + 200000);
    float local = FractalNoise(worldX * 0.001f, worldZ * 0.001f, seed + 300000);
    
    // Combine scales for realistic elevation (sea level to mountain peaks)
    float elevation = continental * 2000.0f + regional * 500.0f + local * 100.0f;
    return std::max(0.0f, elevation); // Ensure no negative elevations
}

// === Core Hierarchical Fractal Functions ===

float RiverNetworks::CalculateFlowAccumulation(float worldX, float worldZ, uint64_t seed) {
    // Each scale represents a different watershed hierarchy level
    float continental = FractalNoise(worldX * RiverConstants::CONTINENTAL_FREQ, worldZ * RiverConstants::CONTINENTAL_FREQ, seed + 1000);
    float regional = FractalNoise(worldX * RiverConstants::REGIONAL_FREQ, worldZ * RiverConstants::REGIONAL_FREQ, seed + 2000);
    float local = FractalNoise(worldX * RiverConstants::LOCAL_FREQ, worldZ * RiverConstants::LOCAL_FREQ, seed + 3000);
    float streams = FractalNoise(worldX * RiverConstants::STREAM_FREQ, worldZ * RiverConstants::STREAM_FREQ, seed + 4000);
    float creeks = FractalNoise(worldX * RiverConstants::CREEK_FREQ, worldZ * RiverConstants::CREEK_FREQ, seed + 5000);
    
    float flow_accumulation = 0.0f;
    
    // More realistic thresholds - each level becomes progressively rarer
    // Continental drainage (major rivers) - very rare (5% of land)
    if (continental > 0.95f) {
        flow_accumulation += 1000.0f; // Base flow for major rivers
        
        // Major rivers collect from all smaller systems in their watershed
        if (regional > 0.3f) flow_accumulation += 200.0f;
        if (local > 0.2f) flow_accumulation += 40.0f;
        if (streams > 0.1f) flow_accumulation += 8.0f;
        if (creeks > 0.1f) flow_accumulation += 1.0f;
    }
    // Regional drainage (tributaries) - uncommon (10% of remaining land)
    else if (regional > 0.9f) {
        flow_accumulation += 200.0f; // Base flow for tributaries
        
        // Regional rivers collect from smaller systems
        if (local > 0.3f) flow_accumulation += 40.0f;
        if (streams > 0.2f) flow_accumulation += 8.0f;
        if (creeks > 0.1f) flow_accumulation += 1.0f;
    }
    // Local drainage (small rivers) - moderate (15% of remaining land)
    else if (local > 0.85f) {
        flow_accumulation += 40.0f; // Base flow for small rivers
        
        if (streams > 0.4f) flow_accumulation += 8.0f;
        if (creeks > 0.2f) flow_accumulation += 1.0f;
    }
    // Stream drainage - common (20% of remaining land)
    else if (streams > 0.8f) {
        flow_accumulation += 8.0f; // Base flow for streams
        
        if (creeks > 0.3f) flow_accumulation += 1.0f;
    }
    // Creek drainage - most common (25% of remaining land)
    else if (creeks > 0.75f) {
        flow_accumulation += 1.0f; // Base flow for creeks
    }
    
    return flow_accumulation;
}

float RiverNetworks::CalculateFractalRiverStrength(float worldX, float worldZ, uint64_t seed) {
    return CalculateFlowAccumulation(worldX, worldZ, seed);
}

float RiverNetworks::EnsureRiverContinuity(float baseStrength, float worldX, float worldZ, uint64_t seed) {
    // Sample neighboring points for river presence
    float north = CalculateFlowAccumulation(worldX, worldZ + 1.0f, seed);
    float south = CalculateFlowAccumulation(worldX, worldZ - 1.0f, seed);
    float east = CalculateFlowAccumulation(worldX + 1.0f, worldZ, seed);
    float west = CalculateFlowAccumulation(worldX - 1.0f, worldZ, seed);
    
    float maxNeighbor = std::max({north, south, east, west});
    
    // If strong neighbors exist but this point is weak, connect them
    if (maxNeighbor > RiverConstants::RIVER_THRESHOLD && baseStrength > RiverConstants::RIVER_THRESHOLD * 0.6f) {
        // Connect with reduced flow (80% of strongest neighbor)
        return std::max(baseStrength, maxNeighbor * 0.8f);
    }
    
    return baseStrength;
}

ConfluenceData RiverNetworks::CalculateConfluence(float worldX, float worldZ, uint64_t seed) {
    // Get individual scale contributions
    float continental = FractalNoise(worldX * RiverConstants::CONTINENTAL_FREQ, worldZ * RiverConstants::CONTINENTAL_FREQ, seed + 1000);
    float regional = FractalNoise(worldX * RiverConstants::REGIONAL_FREQ, worldZ * RiverConstants::REGIONAL_FREQ, seed + 2000);
    float local = FractalNoise(worldX * RiverConstants::LOCAL_FREQ, worldZ * RiverConstants::LOCAL_FREQ, seed + 3000);
    float streams = FractalNoise(worldX * RiverConstants::STREAM_FREQ, worldZ * RiverConstants::STREAM_FREQ, seed + 4000);
    
    ConfluenceData confluence;
    confluence.is_confluence = false;
    confluence.tributary_count = 0;
    confluence.tributary_strengths.clear();
    
    // Natural confluences occur where fractal tree branches converge
    // Check for sudden flow increase indicating tributary joining
    float hereFlow = CalculateFlowAccumulation(worldX, worldZ, seed);
    float upstreamFlow = CalculateFlowAccumulation(worldX - 4.0f, worldZ - 4.0f, seed);
    
    // Confluence detected when flow significantly increases
    if (hereFlow > upstreamFlow * 1.4f && hereFlow > 50.0f) {
        confluence.is_confluence = true;
        confluence.confluence_strength = hereFlow;
        
        // Count contributing scales as tributaries
        if (continental > 0.7f) {
            confluence.tributary_count++;
            confluence.tributary_strengths.push_back(continental * 1000.0f);
        }
        if (regional > 0.7f) {
            confluence.tributary_count++;
            confluence.tributary_strengths.push_back(regional * 200.0f);
        }
        if (local > 0.7f) {
            confluence.tributary_count++;
            confluence.tributary_strengths.push_back(local * 40.0f);
        }
        if (streams > 0.8f) {
            confluence.tributary_count++;
            confluence.tributary_strengths.push_back(streams * 8.0f);
        }
        
        // Main flow direction from dominant contributing scale
        confluence.main_river_direction = atan2(
            CalculateFlowDirection(worldX, worldZ, seed).y,
            CalculateFlowDirection(worldX, worldZ, seed).x
        );
    }
    
    return confluence;
}

// === Flow Direction & Terrain Integration ===

glm::vec2 RiverNetworks::CalculateFlowDirection(float worldX, float worldZ, uint64_t seed) {
    // Sample elevation in 4 cardinal directions
    float north = GetElevation(worldX, worldZ + 2.0f, seed);      // 2m sampling distance
    float south = GetElevation(worldX, worldZ - 2.0f, seed);
    float east = GetElevation(worldX + 2.0f, worldZ, seed);
    float west = GetElevation(worldX - 2.0f, worldZ, seed);
    
    // Calculate gradients (positive = uphill, negative = downhill)
    float gradientX = (east - west) * 0.25f;  // Gradient in X direction
    float gradientZ = (north - south) * 0.25f; // Gradient in Z direction
    
    // Water flows in direction of steepest descent
    glm::vec2 flowDirection(-gradientX, -gradientZ);
    
    // Normalize and add small random perturbation to prevent flow loops
    float flowLength = std::sqrt(flowDirection.x * flowDirection.x + flowDirection.y * flowDirection.y);
    if (flowLength > 0.0f) {
        flowDirection.x /= flowLength;
        flowDirection.y /= flowLength;
    } else {
        // Default flow direction if terrain is perfectly flat
        flowDirection = glm::vec2(1.0f, 0.0f);
    }
    
    // Add fractal perturbation for natural meandering
    float meanderX = FractalNoise(worldX * 0.1f, worldZ * 0.1f, seed + 6000) * 0.3f;
    float meanderZ = FractalNoise(worldX * 0.1f, worldZ * 0.1f, seed + 7000) * 0.3f;
    
    flowDirection.x += meanderX;
    flowDirection.y += meanderZ;
    
    // Re-normalize
    flowLength = std::sqrt(flowDirection.x * flowDirection.x + flowDirection.y * flowDirection.y);
    if (flowLength > 0.0f) {
        flowDirection.x /= flowLength;
        flowDirection.y /= flowLength;
    }
    
    return flowDirection;
}

float RiverNetworks::ApplyRiverCarving(float baseElevation, float worldX, float worldZ, uint64_t seed) {
    float riverStrength = CalculateFlowAccumulation(worldX, worldZ, seed);
    
    // Only carve where river strength exceeds threshold
    if (riverStrength < RiverConstants::RIVER_THRESHOLD) {
        return baseElevation; // No river, no carving
    }
    
    // Calculate river dimensions
    float riverWidth = CalculateRiverWidth(riverStrength);
    float riverDepth = CalculateRiverDepth(riverStrength);
    
    // Valley carving extends beyond river channel
    float valleyWidth = riverWidth * RiverConstants::VALLEY_WIDTH_FACTOR;
    float valleyDepth = riverDepth * 0.7f; // Valley is 70% as deep as river
    
    // Calculate distance from river centerline
    glm::vec2 flowDirection = CalculateFlowDirection(worldX, worldZ, seed);
    float distanceFromCenter = CalculateDistanceFromRiverCenter(worldX, worldZ, flowDirection, seed);
    
    float carvingDepth = 0.0f;
    
    if (distanceFromCenter < riverWidth * 0.5f) {
        // Inside river channel - full depth carving
        carvingDepth = riverDepth * RiverConstants::CARVING_SCALE_FACTOR;
    } else if (distanceFromCenter < valleyWidth * 0.5f) {
        // Inside valley - gradual carving
        float valleyFactor = 1.0f - (distanceFromCenter / (valleyWidth * 0.5f));
        carvingDepth = valleyDepth * valleyFactor;
    }
    
    // Apply carving with smooth falloff
    return baseElevation - carvingDepth;
}

float RiverNetworks::CalculateTerrainGradient(float worldX, float worldZ, uint64_t seed) {
    // Sample elevation in 4 directions to calculate gradient magnitude
    float north = GetElevation(worldX, worldZ + 1.0f, seed);
    float south = GetElevation(worldX, worldZ - 1.0f, seed);
    float east = GetElevation(worldX + 1.0f, worldZ, seed);
    float west = GetElevation(worldX - 1.0f, worldZ, seed);
    
    // Calculate partial derivatives
    float dzdx = (east - west) * 0.5f;
    float dzdy = (north - south) * 0.5f;
    
    // Gradient magnitude
    return std::sqrt(dzdx * dzdx + dzdy * dzdy);
}

// === River Feature Generation ===

WaterfallData RiverNetworks::CalculateWaterfallFeatures(float worldX, float worldZ, float riverStrength, uint64_t seed) {
    WaterfallData waterfall;
    waterfall.has_waterfall = false;
    waterfall.has_rapids = false;
    
    // Calculate terrain gradient
    waterfall.gradient = CalculateTerrainGradient(worldX, worldZ, seed);
    
    // Get geological hardness (simulates rock layers)
    float rockHardness = GeologicalNoise(worldX, worldZ, seed + 8000);
    
    // Waterfalls where rivers cross hard rock barriers with steep gradients
    if (waterfall.gradient > RiverConstants::WATERFALL_GRADIENT && rockHardness > 0.6f && riverStrength > 50.0f) {
        waterfall.has_waterfall = true;
        waterfall.waterfall_height = waterfall.gradient * 30.0f; // Up to 30m waterfalls
    }
    
    // Rapids in moderately steep, rocky areas
    else if (waterfall.gradient > RiverConstants::RAPIDS_GRADIENT && rockHardness > 0.4f) {
        waterfall.has_rapids = true;
    }
    
    return waterfall;
}

float RiverNetworks::CalculateMeanderIntensity(float riverStrength, float terrainGradient, uint64_t seed) {
    // Base meandering intensity (larger rivers meander more in flat terrain)
    float baseMeander = riverStrength / 200.0f; // Normalize river strength
    
    // Gradient effect (flatter terrain = more meandering)
    float gradientFactor = 1.0f / (1.0f + terrainGradient * 20.0f);
    
    // Fractal variation for natural irregularity
    float meanderNoise = FractalNoise(riverStrength * 0.01f, terrainGradient * 10.0f, seed + 9000);
    float meanderVariation = 0.7f + (meanderNoise * 0.6f); // 0.7 to 1.3 multiplier
    
    return baseMeander * gradientFactor * meanderVariation;
}

glm::vec2 RiverNetworks::ApplyMeandering(glm::vec2 baseFlowDirection, float worldX, float worldZ, 
                                        float meanderIntensity, uint64_t seed) {
    // Generate smooth curves using multiple noise octaves
    float curveX = 0.0f;
    float curveZ = 0.0f;
    
    // Multiple scales of meandering
    for (int octave = 0; octave < 3; octave++) {
        float frequency = 0.02f * std::pow(2.0f, octave);
        float amplitude = meanderIntensity / std::pow(2.0f, octave);
        
        curveX += FractalNoise(worldX * frequency, worldZ * frequency, seed + 10000 + octave) * amplitude;
        curveZ += FractalNoise(worldX * frequency, worldZ * frequency, seed + 11000 + octave) * amplitude;
    }
    
    // Apply curves to base flow direction
    glm::vec2 meanderingDirection = baseFlowDirection;
    meanderingDirection.x += curveX;
    meanderingDirection.y += curveZ;
    
    // Normalize
    float length = std::sqrt(meanderingDirection.x * meanderingDirection.x + meanderingDirection.y * meanderingDirection.y);
    if (length > 0.0f) {
        meanderingDirection.x /= length;
        meanderingDirection.y /= length;
    }
    
    return meanderingDirection;
}

GroundwaterData RiverNetworks::CalculateGroundwater(float worldX, float worldZ, float surfaceElevation, uint64_t seed) {
    GroundwaterData groundwater;
    
    // Calculate regional water table depth
    float regionalWaterTable = FractalNoise(worldX * 0.0002f, worldZ * 0.0002f, seed + 12000) * 50.0f;
    
    // Local geological variations affect water table
    float geologicalPermeability = FractalNoise(worldX * 0.01f, worldZ * 0.01f, seed + 13000);
    float localAdjustment = geologicalPermeability * 20.0f;
    
    // Water table depth relative to surface
    groundwater.water_table_depth = regionalWaterTable + localAdjustment;
    
    // Springs occur where water table intersects surface in permeable areas
    // Use surface elevation in calculation
    if (groundwater.water_table_depth < (surfaceElevation * 0.05f + 5.0f) && geologicalPermeability > 0.6f) {
        groundwater.is_spring_source = true;
        groundwater.spring_flow_rate = (5.0f - groundwater.water_table_depth) * geologicalPermeability * 10.0f;
        groundwater.aquifer_connection = true;
    } else {
        groundwater.is_spring_source = false;
        groundwater.spring_flow_rate = 0.0f;
        groundwater.aquifer_connection = geologicalPermeability > 0.3f;
    }
    
    return groundwater;
}

// === River Dimensions & Classification ===

float RiverNetworks::CalculateRiverWidth(float riverStrength) {
    return riverStrength * RiverConstants::WIDTH_SCALE_FACTOR;
}

float RiverNetworks::CalculateRiverDepth(float riverStrength) {
    return riverStrength * RiverConstants::DEPTH_SCALE_FACTOR;
}

float RiverNetworks::CalculateFlowVelocity(float riverStrength, float terrainGradient) {
    // Base velocity from river strength
    float baseVelocity = std::sqrt(riverStrength) * 0.1f;
    
    // Gradient effect (steeper = faster)
    float gradientEffect = 1.0f + terrainGradient * 10.0f;
    
    // Clamp to realistic range (0.1 to 5.0 m/s)
    return std::clamp(baseVelocity * gradientEffect, 0.1f, 5.0f);
}

int RiverNetworks::CalculateStreamOrder(float riverStrength) {
    // Strahler stream order based on strength thresholds
    if (riverStrength < 2.0f) return 1;        // Tiny creeks
    if (riverStrength < 8.0f) return 2;        // Small streams
    if (riverStrength < 30.0f) return 3;       // Medium streams
    if (riverStrength < 100.0f) return 4;      // Small rivers
    if (riverStrength < 300.0f) return 5;      // Medium rivers
    if (riverStrength < 800.0f) return 6;      // Large rivers
    if (riverStrength < 2000.0f) return 7;     // Major rivers
    return std::min(12, static_cast<int>(8 + std::log2(riverStrength / 2000.0f)));
}

bool RiverNetworks::DetermineIfHeadwater(float worldX, float worldZ, float riverStrength, uint64_t seed) {
    // Check if nearby areas have significantly lower flow
    float surroundingFlow = 0.0f;
    int samples = 0;
    
    // Sample in a small radius around the point
    for (int dx = -2; dx <= 2; dx++) {
        for (int dz = -2; dz <= 2; dz++) {
            if (dx == 0 && dz == 0) continue; // Skip center point
            
            float neighborFlow = CalculateFlowAccumulation(worldX + dx * 2.0f, worldZ + dz * 2.0f, seed);
            surroundingFlow += neighborFlow;
            samples++;
        }
    }
    
    float avgSurroundingFlow = surroundingFlow / samples;
    
    // Headwater if this point is significantly stronger than surroundings
    // and the groundwater indicates spring source
    GroundwaterData groundwater = CalculateGroundwater(worldX, worldZ, GetElevation(worldX, worldZ, seed), seed);
    
    return (riverStrength > avgSurroundingFlow * 1.5f && riverStrength > 1.0f) || groundwater.is_spring_source;
}

bool RiverNetworks::DetermineBraidedPattern(float riverStrength, const ClimateData& climate, uint64_t seed) {
    // Braided rivers occur in specific conditions:
    // 1. Large rivers with high sediment load
    // 2. Areas with high precipitation variability
    // 3. Moderate terrain gradients
    
    if (riverStrength < 200.0f) return false; // Only large rivers can be braided
    
    // High precipitation with seasonal variation favors braiding
    bool climaticConditions = (climate.precipitation > 800.0f && climate.seasonality > 0.6f);
    
    // Moderate gradients favor braiding (not too steep, not too flat)
    // This would need terrain gradient - simplified for now
    bool terrainConditions = true; // Placeholder
    
    // Random factor for natural variation
    float randomFactor = FractalNoise(riverStrength * 0.001f, climate.temperature * 0.1f, seed + 20000);
    
    return climaticConditions && terrainConditions && randomFactor > 0.7f;
}

// === Termination & Biome Integration ===

RiverTerminationType RiverNetworks::DetermineRiverTermination(float worldX, float worldZ, float riverStrength, 
                                                             const ClimateData& climate, uint64_t seed) {
    // Distance to virtual ocean (simplified - would use actual ocean system)
    float oceanDistance = std::sqrt(worldX * worldX + worldZ * worldZ) * 0.001f; // Very simplified
    
    // Most rivers flow to ocean if within reasonable distance
    if (oceanDistance < 50.0f && riverStrength > 30.0f) { // Within 50km of ocean
        return RiverTerminationType::OCEAN;
    }
    
    // Desert sinks in arid climates
    if (climate.precipitation < 300.0f && climate.temperature > 25.0f) {
        return RiverTerminationType::DESERT_SINK;
    }
    
    // Underground capture in karst terrain
    float karstFactor = GeologicalNoise(worldX, worldZ, seed + 14000);
    if (karstFactor > 0.8f && riverStrength < 100.0f) {
        return RiverTerminationType::UNDERGROUND_CAPTURE;
    }
    
    // Inland lakes in mountainous/closed basins
    float elevation = GetElevation(worldX, worldZ, seed);
    if (elevation > 800.0f) {
        return RiverTerminationType::INLAND_LAKE;
    }
    
    // Default to wetland dispersion
    return RiverTerminationType::WETLAND_DISPERSION;
}

RiparianBiomeType RiverNetworks::DetermineRiparianBiome(const ClimateData& climate, float elevation, 
                                                       float distanceFromRiver, float riverStrength) {
    // Distance-based influence (closer to river = stronger effect)
    float influenceFactor = 1.0f / (1.0f + distanceFromRiver / 20.0f);
    
    // Only strong rivers create riparian zones
    if (riverStrength < 50.0f || influenceFactor < 0.3f) {
        return RiparianBiomeType::RIPARIAN_FOREST; // Default minimal influence
    }
    
    // Desert oases
    if (climate.precipitation < 400.0f && climate.temperature > 20.0f) {
        return RiparianBiomeType::DESERT_OASIS;
    }
    
    // Mangrove swamps in tropical coastal areas
    if (climate.temperature > 20.0f && elevation < 10.0f && 
        climate.precipitation > 1000.0f) {
        return RiparianBiomeType::MANGROVE_SWAMP;
    }
    
    // Alpine meadows in high mountains
    if (elevation > 1200.0f) {
        return RiparianBiomeType::ALPINE_MEADOW;
    }
    
    // Wetland marshes for large, slow rivers (gradient would be calculated here)
    if (riverStrength > 300.0f) { // Simplified gradient check
        return RiparianBiomeType::WETLAND_MARSH;
    }
    
    // Floodplains for major rivers
    if (riverStrength > 200.0f) {
        return RiparianBiomeType::FLOODPLAIN_GRASSLAND;
    }
    
    // Default riparian forest
    return RiparianBiomeType::RIPARIAN_FOREST;
}

UndergroundConnection RiverNetworks::CalculateUndergroundConnection(float worldX, float worldZ, 
                                                                   float riverStrength, uint64_t seed) {
    UndergroundConnection connection;
    connection.connects_to_caves = false;
    connection.underground_flow_rate = 0.0f;
    connection.cave_entrance_count = 0;
    connection.creates_aquifer = false;
    
    // Check for cave systems in the area
    float caveDensity = FractalNoise(worldX * 0.001f, worldZ * 0.001f, seed + 17000);
    
    // Rivers connect to caves in karst terrain
    if (caveDensity > 0.6f && riverStrength > 20.0f) {
        connection.connects_to_caves = true;
        
        // Underground flow proportional to river size and cave density
        connection.underground_flow_rate = riverStrength * caveDensity * 0.3f;
        
        // Number of cave entrances
        connection.cave_entrance_count = static_cast<int>(caveDensity * 3.0f) + 1;
    }
    
    // Large rivers create aquifers
    if (riverStrength > 100.0f) {
        connection.creates_aquifer = true;
        connection.aquifer_extent = riverStrength * 2.0f; // Aquifer radius in meters
    }
    
    return connection;
}

// === Local Flow Simulation ===

SimpleFlowData RiverNetworks::CalculateLocalFlow(float worldX, float worldZ, uint64_t seed, 
                                                 const ClimateData& climate) {
    SimpleFlowData flow;
    
    // Basic flow direction and speed
    flow.flow_direction = CalculateFlowDirection(worldX, worldZ, seed);
    flow.terrain_slope = CalculateTerrainGradient(worldX, worldZ, seed);
    
    // Flow speed based on terrain slope and river presence
    float riverStrength = CalculateFlowAccumulation(worldX, worldZ, seed);
    flow.flow_speed_ms = CalculateFlowVelocity(riverStrength, flow.terrain_slope);
    
    // Water presence based on climate and river strength
    flow.has_water_now = (riverStrength > RiverConstants::RIVER_THRESHOLD) && 
                         (climate.precipitation > 200.0f); // Basic climate check
    
    // Visual flow rate for effects
    flow.visual_flow_rate = riverStrength * 0.1f;
    
    return flow;
}

float RiverNetworks::CalculateTerrainSlope(float worldX, float worldZ, uint64_t seed) {
    return CalculateTerrainGradient(worldX, worldZ, seed);
}

// === Main Generation Function ===

EnhancedRiverData RiverNetworks::GenerateComprehensiveRiverData(float worldX, float worldZ, 
                                                               const ClimateData& climate, 
                                                               float surfaceElevation, 
                                                               uint64_t seed) {
    EnhancedRiverData river;
    
    // 1. Calculate base fractal river strength using hierarchical flow accumulation
    river.fractal_river_strength = CalculateFlowAccumulation(worldX, worldZ, seed);
    
    // Early exit if no river at this location
    if (river.fractal_river_strength < RiverConstants::RIVER_THRESHOLD) {
        return EnhancedRiverData{}; // Empty/default struct
    }
    
    // 2. Basic flow characteristics
    river.river_width_m = CalculateRiverWidth(river.fractal_river_strength);
    river.river_depth_m = CalculateRiverDepth(river.fractal_river_strength);
    river.flow_direction = CalculateFlowDirection(worldX, worldZ, seed);
    river.flow_velocity_ms = CalculateFlowVelocity(river.fractal_river_strength, 
                                                   CalculateTerrainGradient(worldX, worldZ, seed));
    
    // 3. Hierarchical classification
    river.stream_order = CalculateStreamOrder(river.fractal_river_strength);
    river.is_headwater = DetermineIfHeadwater(worldX, worldZ, river.fractal_river_strength, seed);
    river.is_main_stem = river.fractal_river_strength > RiverConstants::MAJOR_RIVER_THRESHOLD;
    river.is_tributary = !river.is_main_stem && !river.is_headwater;
    
    // 4. Confluence analysis
    river.confluence = CalculateConfluence(worldX, worldZ, seed);
    
    // 5. Terrain modification
    river.carving_depth_m = CalculateRiverDepth(river.fractal_river_strength) * RiverConstants::CARVING_SCALE_FACTOR;
    river.valley_width_m = river.river_width_m * RiverConstants::VALLEY_WIDTH_FACTOR;
    river.creates_floodplain = river.fractal_river_strength > 200.0f;
    river.floodplain_width_m = river.creates_floodplain ? river.river_width_m * 8.0f : 0.0f;
    
    // 6. Special features
    river.waterfall = CalculateWaterfallFeatures(worldX, worldZ, river.fractal_river_strength, seed);
    river.meander_intensity = CalculateMeanderIntensity(river.fractal_river_strength, 
                                                       CalculateTerrainGradient(worldX, worldZ, seed), seed);
    river.is_braided = DetermineBraidedPattern(river.fractal_river_strength, climate, seed);
    
    // 7. Sources and termination
    river.groundwater = CalculateGroundwater(worldX, worldZ, surfaceElevation, seed);
    river.termination = DetermineRiverTermination(worldX, worldZ, river.fractal_river_strength, climate, seed);
    
    // 8. Environmental integration
    river.riparian_biome = DetermineRiparianBiome(climate, surfaceElevation, 0.0f, river.fractal_river_strength);
    river.underground = CalculateUndergroundConnection(worldX, worldZ, river.fractal_river_strength, seed);
    
    // 9. Climate interaction (basic setup - seasonal not implemented yet)
    river.is_seasonal = false;
    river.base_flow_rate = river.fractal_river_strength;
    river.drought_resistance = CalculateDroughtResistance(river.groundwater, river.fractal_river_strength);
    
    return river;
}

// === Helper Functions ===

float RiverNetworks::CalculateDistanceFromRiverCenter(float worldX, float worldZ, glm::vec2 flowDirection, uint64_t seed) {
    // Simplified distance calculation - in practice would trace along river path
    // For now, assume perpendicular distance from flow direction
    glm::vec2 perpendicular(-flowDirection.y, flowDirection.x);
    
    // Sample noise to create natural river meandering path
    float pathOffset = FractalNoise(worldX * 0.01f, worldZ * 0.01f, seed + 18000) * 10.0f;
    
    // Calculate distance from meandering centerline
    return std::abs(perpendicular.x * worldX + perpendicular.y * worldZ + pathOffset);
}

float RiverNetworks::CalculateDroughtResistance(const GroundwaterData& groundwater, float riverStrength) {
    float baseResistance = riverStrength / 1000.0f; // Larger rivers more resistant
    
    // Groundwater connections increase drought resistance
    if (groundwater.aquifer_connection) {
        baseResistance += 0.3f;
    }
    
    // Springs provide additional drought resistance
    if (groundwater.is_spring_source) {
        baseResistance += groundwater.spring_flow_rate * 0.01f;
    }
    
    return std::clamp(baseResistance, 0.0f, 1.0f);
}

float RiverNetworks::CalculateDistanceToNearestRiver(float worldX, float worldZ, uint64_t seed) {
    // Sample in expanding rings to find nearest river
    float searchRadius = 10.0f;
    float maxRadius = 1000.0f;
    
    while (searchRadius < maxRadius) {
        // Sample points in a circle at current radius
        int numSamples = static_cast<int>(searchRadius * 0.2f); // More samples for larger radii
        
        for (int i = 0; i < numSamples; i++) {
            float angle = (i * 2.0f * M_PI) / numSamples;
            float sampleX = worldX + searchRadius * std::cos(angle);
            float sampleZ = worldZ + searchRadius * std::sin(angle);
            
            float riverStrength = CalculateFlowAccumulation(sampleX, sampleZ, seed);
            if (riverStrength > RiverConstants::RIVER_THRESHOLD) {
                return searchRadius;
            }
        }
        
        searchRadius *= 1.5f; // Expand search radius
    }
    
    return maxRadius; // No river found within search area
}

glm::vec2 RiverNetworks::RotateVector(glm::vec2 vec, float angleRadians) {
    float cosA = std::cos(angleRadians);
    float sinA = std::sin(angleRadians);
    
    return glm::vec2(
        vec.x * cosA - vec.y * sinA,
        vec.x * sinA + vec.y * cosA
    );
}

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
