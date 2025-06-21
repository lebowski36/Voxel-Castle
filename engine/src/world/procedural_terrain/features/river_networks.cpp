#include "world/procedural_terrain/features/river_networks.h"
#include "world/procedural_terrain/climate/climate_system.h"
#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include "world/procedural_terrain/utils/seed_utils.h"
#include "util/enhanced_noise.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <limits>
#include <iostream>
#include <iomanip>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

// Static cache for river networks
std::unordered_map<uint64_t, RiverNetwork> RiverNetworks::cached_networks;

// === Utility Functions ===

static float SimpleNoise(float x, float z, uint64_t seed) {
    // Simple hash-based noise function
    uint64_t hash = seed + static_cast<uint64_t>(x * 1000000) + static_cast<uint64_t>(z * 1000);
    hash = (hash ^ (hash >> 16)) * 0x45d9f3b;
    hash = (hash ^ (hash >> 16)) * 0x45d9f3b;
    hash = hash ^ (hash >> 16);
    return (hash % 1000000) / 1000000.0f;
}

float RiverNetworks::GetTerrainElevation(float world_x, float world_z, uint64_t seed) {
    // Generate terrain elevation using multi-scale noise (same as main terrain system)
    float continental = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::CONTINENTAL, seed) * 1400.0f;
    float regional = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::REGIONAL, seed + 1000) * 500.0f;
    float local = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::LOCAL, seed + 2000) * 120.0f;
    float micro = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::MICRO, seed + 3000) * 30.0f;
    
    return continental + regional + local + micro;
}

glm::vec2 RiverNetworks::CalculateFlowDirection(float world_x, float world_z, uint64_t seed) {
    // Use multi-directional sampling to find the steepest downhill direction
    const float sample_distance = 50.0f; // Match the step_size used in river tracing
    const int num_directions = 8; // Sample 8 directions around current point
    
    float current_elevation = GetTerrainElevation(world_x, world_z, seed);
    float steepest_gradient = 0.0f;
    glm::vec2 best_direction(1.0f, 0.0f); // Default eastward flow
    
    // Sample all 8 directions and find steepest downhill
    for (int i = 0; i < num_directions; i++) {
        float angle = (float)i * 2.0f * M_PI / (float)num_directions;
        float test_x = world_x + cos(angle) * sample_distance;
        float test_z = world_z + sin(angle) * sample_distance;
        float test_elevation = GetTerrainElevation(test_x, test_z, seed);
        
        // Calculate gradient (positive means downhill)
        float gradient = (current_elevation - test_elevation) / sample_distance;
        
        // Keep track of steepest downhill direction
        if (gradient > steepest_gradient) {
            steepest_gradient = gradient;
            best_direction = glm::vec2(cos(angle), sin(angle));
        }
    }
    
    // If no downhill direction found, use traditional 4-direction gradient
    if (steepest_gradient <= 0.0f) {
        float north = GetTerrainElevation(world_x, world_z + sample_distance, seed);
        float south = GetTerrainElevation(world_x, world_z - sample_distance, seed);
        float east = GetTerrainElevation(world_x + sample_distance, world_z, seed);
        float west = GetTerrainElevation(world_x - sample_distance, world_z, seed);
        
        // Calculate gradients (water flows downhill)
        float gradient_x = (west - east) / (2.0f * sample_distance);
        float gradient_z = (south - north) / (2.0f * sample_distance);
        
        best_direction = glm::vec2(gradient_x, gradient_z);
    }
    
    // Normalize the direction vector
    float length = std::sqrt(best_direction.x * best_direction.x + best_direction.y * best_direction.y);
    if (length > 0.0f) {
        best_direction.x /= length;
        best_direction.y /= length;
    }
    
    return best_direction;
}

// === River Source Detection ===

bool RiverNetworks::IsRiverSource(float world_x, float world_z, 
                                 const ClimateData& climate, float elevation, uint64_t seed) {
    // More permissive river source detection as mentioned in handoff document
    
    // Allow rivers at moderate elevations (was 200m, now 100m)
    if (elevation < 100.0f) return false; // Must be above 100m elevation
    
    // Require moderate precipitation (was 100.0f, now more permissive)
    if (climate.precipitation < 50.0f) return false; // Need moderate rainfall
    
    // Use sparse noise for selective river placement
    float source_noise = SimpleNoise(world_x * 0.0001f, world_z * 0.0001f, seed + 9999);
    
    // More permissive threshold as mentioned in handoff - was 0.95, now 0.25
    return source_noise > 0.25f;
}

// === River Path Generation ===

RiverPath RiverNetworks::TraceRiverPath(float source_x, float source_z, 
                                       float source_elevation, uint64_t seed) {
    RiverPath path;
    path.river_id = static_cast<int>(SimpleNoise(source_x, source_z, seed) * 1000000);
    
    // DEBUG: Log river tracing start
    std::cout << "    Tracing river from (" << source_x << ", " << source_z 
              << ") elevation: " << source_elevation << "m" << std::endl;
    
    // Start with source point
    RiverPoint current_point;
    current_point.x = source_x;
    current_point.z = source_z;
    current_point.elevation = source_elevation;
    current_point.width = RiverConstants::CREEK_WIDTH; // Start as creek
    current_point.depth = current_point.width * RiverConstants::CARVING_DEPTH_FACTOR;
    
    path.points.push_back(current_point);
    
    // Trace path downhill
    const float step_size = 50.0f; // 50m steps
    const int max_steps = 2000; // Maximum 100km river length
    
    for (int step = 0; step < max_steps; ++step) {
        // Calculate flow direction
        glm::vec2 flow_dir = CalculateFlowDirection(current_point.x, current_point.z, seed);
        
        // Move to next point
        float next_x = current_point.x + flow_dir.x * step_size;
        float next_z = current_point.z + flow_dir.y * step_size;
        float next_elevation = GetTerrainElevation(next_x, next_z, seed);
        
        // Check if we've reached the ocean (elevation below sea level)
        if (next_elevation < 0.0f) {
            path.flows_to_ocean = true;
            break;
        }
        
        // Check if we're flowing uphill (shouldn't happen, but safety check)
        if (next_elevation > current_point.elevation) {
            // End the river - this indicates a problem with flow direction calculation
            break;
        }
        
        // Create next river point
        RiverPoint next_point;
        next_point.x = next_x;
        next_point.z = next_z;
        next_point.elevation = next_elevation;
        
        // Calculate accumulated flow (simplified - more points = more flow)
        float flow_accumulation = static_cast<float>(path.points.size());
        next_point.width = std::min(RiverConstants::MAJOR_RIVER_WIDTH, 
                                   RiverConstants::CREEK_WIDTH + flow_accumulation * 0.5f);
        next_point.depth = next_point.width * RiverConstants::CARVING_DEPTH_FACTOR;
        
        // Check for waterfalls and rapids
        float elevation_change = current_point.elevation - next_elevation;
        next_point.has_waterfall = ShouldCreateWaterfall(elevation_change, step_size);
        next_point.has_rapids = !next_point.has_waterfall && ShouldCreateRapids(elevation_change, step_size);
        
        path.points.push_back(next_point);
        current_point = next_point;
        
        // Update path statistics
        path.total_length += step_size;
        path.max_width = std::max(path.max_width, next_point.width);
    }
    
    // DEBUG: Log final river path results    // Debug: Output only summary info without verbose tracing
    return path;
}

bool RiverNetworks::ShouldCreateWaterfall(float elevation_change, float distance) {
    float gradient = elevation_change / distance;
    return gradient >= RiverConstants::WATERFALL_MIN_GRADIENT;
}

bool RiverNetworks::ShouldCreateRapids(float elevation_change, float distance) {
    float gradient = elevation_change / distance;
    return gradient >= RiverConstants::RAPIDS_MIN_GRADIENT;
}

// === Lake Generation ===

std::vector<Lake> RiverNetworks::GenerateLakes(int region_x, int region_z, 
                                              int region_size, uint64_t seed) {
    std::vector<Lake> lakes;
    
    // Generate lakes based on terrain depressions and climate
    std::mt19937 rng(seed + region_x * 1000 + region_z);
    
    // Number of lakes in this region (0-5 typically)
    int num_lakes = rng() % 6;
    
    for (int i = 0; i < num_lakes; ++i) {
        Lake lake;
        
        // Random position within region
        float region_world_x = region_x * region_size + (region_size * 0.5f);
        float region_world_z = region_z * region_size + (region_size * 0.5f);
        
        lake.center_x = region_world_x + (rng() % region_size) - (region_size * 0.5f);
        lake.center_z = region_world_z + (rng() % region_size) - (region_size * 0.5f);
        
        // Lake elevation based on terrain
        lake.surface_elevation = GetTerrainElevation(lake.center_x, lake.center_z, seed);
        
        // Random lake size
        lake.radius = RiverConstants::MIN_LAKE_SIZE + 
                     (rng() % static_cast<int>(RiverConstants::MAX_LAKE_SIZE - RiverConstants::MIN_LAKE_SIZE));
        
        // Determine if seasonal based on climate
        ClimateData climate = ClimateSystem::CalculateClimate(lake.center_x, lake.center_z, 
                                                            lake.surface_elevation, seed);
        lake.is_seasonal = climate.precipitation < 50.0f; // Dry areas have seasonal lakes
        
        lakes.push_back(lake);
    }
    
    return lakes;
}

// === Main River Network Generation ===

RiverNetwork RiverNetworks::GenerateRiverNetwork(int region_x, int region_z, 
                                                int region_size, uint64_t seed) {
    RiverNetwork network;
    network.region_x = region_x;
    network.region_z = region_z;
    network.region_size = region_size;
    
    // Generate lakes first
    network.lakes = GenerateLakes(region_x, region_z, region_size, seed);
    
    // Find river sources in this region
    std::vector<std::pair<float, float>> sources = FindRiverSources(region_x, region_z, region_size, seed);
    
    // Generate rivers from sources
    size_t total_sources = sources.size();
    for (size_t i = 0; i < sources.size(); i++) {
        const auto& source = sources[i];
        
        // Progress reporting for river generation (force flush for Python interop)
        if (total_sources > 10) {
            float progress = (float)(i + 1) / (float)total_sources * 100.0f;
            printf("\rTracing rivers: %.1f%% (%zu/%zu)", 
                   progress, (i + 1), total_sources);
            fflush(stdout);  // Force immediate output for Python interop
        }
        
        float source_elevation = GetTerrainElevation(source.first, source.second, seed);
        RiverPath river_path = TraceRiverPath(source.first, source.second, source_elevation, seed);
        
        // DEBUG: Log river path tracing results (but less verbose during progress)
        if (total_sources <= 10) {
            printf("  River from (%.1f, %.1f) traced %zu points, length: %.1fm\n", 
                   source.first, source.second, river_path.points.size(), river_path.total_length);
            fflush(stdout);  // Force immediate output for Python interop
        }
        
        // REDUCED REQUIREMENT: Accept rivers with 3+ points (was 10+) for realistic coverage
        if (river_path.points.size() >= 3) {
            network.rivers.push_back(river_path);
            if (total_sources <= 10) {
                printf("    -> Added to network\n");
                fflush(stdout);  // Force immediate output for Python interop
            }
        } else {
            if (total_sources <= 10) {
                printf("    -> Rejected (too short: %zu points)\n", river_path.points.size());
                fflush(stdout);  // Force immediate output for Python interop
            }
        }
    }
    
    // Final progress update (force flush for Python interop)
    if (total_sources > 10) {
        printf("\nTracing rivers: Complete! Generated %zu rivers from %zu sources.\n", 
               network.rivers.size(), total_sources);
        fflush(stdout);  // Force immediate output for Python interop
    }
    
    return network;
}

std::vector<std::pair<float, float>> RiverNetworks::FindRiverSources(int region_x, int region_z, 
                                                                    int region_size, uint64_t seed) {
    std::vector<std::pair<float, float>> sources;
    
    // Calculate region bounds
    float region_world_x = region_x * region_size;
    float region_world_z = region_z * region_size;
    
    // SPARSE SAMPLING: Use much larger spacing for river placement to avoid performance issues
    const float SAMPLE_SPACING = 500.0f;  // 500m spacing (was 100m - too dense!)
    const int samples_per_side = static_cast<int>(region_size / SAMPLE_SPACING);
    const int total_samples = samples_per_side * samples_per_side;
    
    printf("Generating river network for region (%d,%d) - %dx%d samples (%d total, %.1fm spacing)\n", 
           region_x, region_z, samples_per_side, samples_per_side, total_samples, SAMPLE_SPACING);
    
    // Sample at regular intervals for river source detection
    int samples_processed = 0;
    for (int i = 0; i < samples_per_side; ++i) {
        for (int j = 0; j < samples_per_side; ++j) {
            // Calculate sample coordinates
            float world_x = region_world_x + (i + 0.5f) * SAMPLE_SPACING;
            float world_z = region_world_z + (j + 0.5f) * SAMPLE_SPACING;
            
            float elevation = GetTerrainElevation(world_x, world_z, seed);
            ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, elevation, seed);
            
            if (IsRiverSource(world_x, world_z, climate, elevation, seed)) {
                sources.push_back(std::make_pair(world_x, world_z));
            }
            
            samples_processed++;
        }
    }
    
    printf("  Found %zu river sources in region (%d,%d)\n", sources.size(), region_x, region_z);
    return sources;
}

// === River Queries ===

RiverQueryResult RiverNetworks::QueryRiverAtPoint(float world_x, float world_z, uint64_t seed) {
    RiverQueryResult result;
    
    // Determine which region this point belongs to
    int region_x = static_cast<int>(std::floor(world_x / RiverConstants::REGIONAL_REGION_SIZE));
    int region_z = static_cast<int>(std::floor(world_z / RiverConstants::REGIONAL_REGION_SIZE));
    
    // Get cached river network for this region
    const RiverNetwork& network = GetCachedRiverNetwork(region_x, region_z, 
                                                       RiverConstants::REGIONAL_REGION_SIZE, seed);
    
    // Check if point is in any river - use more flexible search
    float closest_distance = std::numeric_limits<float>::max();
    const RiverPoint* closest_river_point = nullptr;
    
    for (const RiverPath& river : network.rivers) {
        for (const RiverPoint& point : river.points) {
            float distance = std::sqrt((world_x - point.x) * (world_x - point.x) + 
                                     (world_z - point.z) * (world_z - point.z));
            
            // Primary check: exact river width match
            if (distance < point.width * 0.5f) {
                result.has_river = true;
                result.river_width = point.width;
                result.river_depth = point.depth;
                result.water_elevation = point.elevation;
                result.has_waterfall = point.has_waterfall;
                result.has_rapids = point.has_rapids;
                break;
            }
            
            // Track closest river point for fallback
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_river_point = &point;
            }
        }
        if (result.has_river) break;
    }
    
    // Fallback: if no exact match found, check if we're close to any river
    // Use a larger search radius (100m) to account for grid spacing
    if (!result.has_river && closest_river_point && closest_distance < 100.0f) {
        result.has_river = true;
        result.river_width = closest_river_point->width;
        result.river_depth = closest_river_point->depth;
        result.water_elevation = closest_river_point->elevation;
        result.has_waterfall = closest_river_point->has_waterfall;
        result.has_rapids = closest_river_point->has_rapids;
    }
    
    // Check if point is in any lake
    if (!result.has_river) {
        for (const Lake& lake : network.lakes) {
            float distance = std::sqrt((world_x - lake.center_x) * (world_x - lake.center_x) + 
                                     (world_z - lake.center_z) * (world_z - lake.center_z));
            
            if (distance < lake.radius) {
                result.has_lake = true;
                result.water_elevation = lake.surface_elevation;
                result.lake_depth = 10.0f; // Simplified lake depth
                break;
            }
        }
    }
    
    return result;
}

const RiverNetwork& RiverNetworks::GetCachedRiverNetwork(int region_x, int region_z, 
                                                       int region_size, uint64_t seed) {
    uint64_t hash = GetRegionHash(region_x, region_z, region_size);
    
    auto it = cached_networks.find(hash);
    if (it != cached_networks.end()) {
        return it->second;
    }
    
    // Generate new network and cache it
    RiverNetwork network = GenerateRiverNetwork(region_x, region_z, region_size, seed);
    cached_networks[hash] = network;
    return cached_networks[hash];
}

uint64_t RiverNetworks::GetRegionHash(int region_x, int region_z, int region_size) {
    return static_cast<uint64_t>(region_x) * 1000000 + 
           static_cast<uint64_t>(region_z) * 1000 + 
           static_cast<uint64_t>(region_size);
}

// === River Carving ===

float RiverNetworks::ApplyRiverCarving(float base_elevation, float world_x, 
                                      float world_z, uint64_t seed) {
    // FIX: Use actual river networks instead of flow accumulation noise
    RiverQueryResult river_data = QueryRiverAtPoint(world_x, world_z, seed);
    
    if (river_data.has_river) {
        // Use actual river data for carving
        float carving_depth = river_data.river_depth * 1.5f; // Deeper carving for river channel
        return base_elevation - carving_depth;
    } else if (river_data.has_lake) {
        // Lake carving - flatten to lake surface elevation
        return river_data.water_elevation;
    }
    
    // No river or lake - return original elevation
    return base_elevation;
}

// === Legacy Compatibility Functions ===

float RiverNetworks::CalculateFlowAccumulation(float worldX, float worldZ, uint64_t seed) {
    // Generate flow accumulation based on terrain and climate independent of existing rivers
    // This is used to determine where rivers SHOULD be, not where they already are
    
    float elevation = GetTerrainElevation(worldX, worldZ, seed);
    ClimateData climate = ClimateSystem::CalculateClimate(worldX, worldZ, elevation, seed);
    
    // Base flow from precipitation (more rain = more potential flow)
    float precipitation_flow = climate.precipitation * 0.1f; // Scale precipitation to flow units
    
    // Elevation contribution (higher areas contribute to downstream flow)
    float elevation_contribution = std::max(0.0f, elevation * 0.01f);
    
    // Terrain roughness affects flow accumulation
    float terrain_roughness = SimpleNoise(worldX * 0.001f, worldZ * 0.001f, seed + 1111);
    float roughness_factor = 0.5f + terrain_roughness * 0.5f; // 0.5 to 1.0
    
    // Combine factors
    float base_flow = (precipitation_flow + elevation_contribution) * roughness_factor;
    
    // Add some noise for variation
    float flow_noise = SimpleNoise(worldX * 0.0001f, worldZ * 0.0001f, seed + 2222);
    float final_flow = base_flow * (0.8f + flow_noise * 0.4f); // Vary by ±20%
    
    return std::max(0.0f, final_flow);
}

EnhancedRiverData RiverNetworks::GenerateComprehensiveRiverData(float worldX, float worldZ, 
                                                               const ClimateData& climate, 
                                                               float elevation, uint64_t seed) {
    EnhancedRiverData data;
    
    // FIX: Use actual river network data instead of flow accumulation noise
    RiverQueryResult river_result = QueryRiverAtPoint(worldX, worldZ, seed);
    
    if (river_result.has_river) {
        // Use actual river data
        data.fractal_river_strength = river_result.river_width; // Use width as strength indicator
        data.river_width_m = river_result.river_width;
        data.river_depth_m = river_result.river_depth;
        data.flow_direction = CalculateFlowDirection(worldX, worldZ, seed);
        data.flow_velocity_ms = std::min(5.0f, river_result.river_width * 0.1f); // Scale velocity with width
        data.carving_depth_m = data.river_depth_m * 1.5f;
        data.valley_width_m = data.river_width_m * RiverConstants::VALLEY_WIDTH_FACTOR;
        
        // Classify river type based on actual width
        data.is_main_stem = data.river_width_m > RiverConstants::REGIONAL_RIVER_WIDTH;
        data.is_tributary = data.river_width_m > RiverConstants::CREEK_WIDTH && 
                           data.river_width_m <= RiverConstants::REGIONAL_RIVER_WIDTH;
        data.is_headwater = data.river_width_m <= RiverConstants::STREAM_WIDTH;
        
        // Set stream order based on width
        if (data.river_width_m >= RiverConstants::MAJOR_RIVER_WIDTH) {
            data.stream_order = 6;
        } else if (data.river_width_m >= RiverConstants::REGIONAL_RIVER_WIDTH) {
            data.stream_order = 4;
        } else if (data.river_width_m >= RiverConstants::STREAM_WIDTH) {
            data.stream_order = 2;
        } else {
            data.stream_order = 1;
        }
        
        // Additional features
        data.creates_floodplain = data.river_width_m > RiverConstants::REGIONAL_RIVER_WIDTH;
        data.floodplain_width_m = data.creates_floodplain ? data.river_width_m * 5.0f : 0.0f;
        
        // Meandering based on terrain gradient
        glm::vec2 flow_dir = data.flow_direction;
        float gradient = std::sqrt(flow_dir.x * flow_dir.x + flow_dir.y * flow_dir.y);
        data.meander_intensity = std::max(0.0f, 1.0f - gradient * 10.0f); // Less meandering on steep terrain
        
        // Features from river data
        if (river_result.has_waterfall || river_result.has_rapids) {
            data.meander_intensity = 0.0f; // No meandering in rapids/waterfalls
        }
    } else {
        // No river at this point - set all values to zero/false
        data.fractal_river_strength = 0.0f;
        data.river_width_m = 0.0f;
        data.river_depth_m = 0.0f;
        data.flow_direction = glm::vec2(0.0f, 0.0f);
        data.flow_velocity_ms = 0.0f;
        data.stream_order = 0;
        data.is_headwater = false;
        data.is_main_stem = false;
        data.is_tributary = false;
        data.carving_depth_m = 0.0f;
        data.valley_width_m = 0.0f;
        data.creates_floodplain = false;
        data.floodplain_width_m = 0.0f;
        data.meander_intensity = 0.0f;
        data.is_braided = false;
    }
    
    return data;
}

float RiverNetworks::CalculateRiverWidth(float accumulated_flow, float base_width) {
    return base_width + accumulated_flow * 0.1f;
}

std::vector<RiverPoint> RiverNetworks::GenerateRiverPoints(float start_x, float start_z,
                                                         float end_x, float end_z,
                                                         float start_elevation, float end_elevation,
                                                         float river_width, uint64_t seed) {
    std::vector<RiverPoint> points;
    
    // Simple linear interpolation between start and end points
    const int num_points = 10;
    for (int i = 0; i <= num_points; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(num_points);
        
        RiverPoint point;
        point.x = start_x + t * (end_x - start_x);
        point.z = start_z + t * (end_z - start_z);
        point.elevation = start_elevation + t * (end_elevation - start_elevation);
        point.width = river_width;
        point.depth = river_width * RiverConstants::CARVING_DEPTH_FACTOR;
        
        points.push_back(point);
    }
    
    return points;
}

// === Dynamic Resolution System ===

float RiverNetworks::GetOptimalSamplingResolution(float area_size, const std::string& feature_type) {
    // Dynamic resolution based on area size and feature requirements
    
    // Base resolution targets (samples per meter)
    float base_resolution;
    
    if (feature_type == "rivers") {
        // Rivers need higher resolution to capture flow paths
        base_resolution = 2.0f;  // 0.5m per sample
    } else if (feature_type == "terrain") {
        // Terrain can use moderate resolution
        base_resolution = 1.0f;  // 1m per sample  
    } else {
        // Default case
        base_resolution = 1.0f;
    }
    
    // Scale resolution based on area size
    if (area_size <= 500.0f) {
        // Very small areas (≤500m) - high detail
        return base_resolution * 2.0f;
    } else if (area_size <= 2000.0f) {
        // Small areas (≤2km) - normal detail
        return base_resolution;
    } else if (area_size <= 10000.0f) {
        // Medium areas (≤10km) - reduced detail
        return base_resolution * 0.5f;
    } else if (area_size <= 50000.0f) {
        // Large areas (≤50km) - low detail
        return base_resolution * 0.2f;
    } else {
        // Very large areas (>50km) - minimal detail
        return base_resolution * 0.1f;
    }
}

int RiverNetworks::GetOptimalSampleCount(float area_size, const std::string& feature_type) {
    float resolution = GetOptimalSamplingResolution(area_size, feature_type);
    int sample_count = static_cast<int>(area_size * resolution);
    
    // Enforce reasonable limits
    const int MIN_SAMPLES = 50;    // Minimum detail
    const int MAX_SAMPLES = 2000;  // Maximum for performance
    
    sample_count = std::max(MIN_SAMPLES, std::min(MAX_SAMPLES, sample_count));
    
    return sample_count;
}

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
