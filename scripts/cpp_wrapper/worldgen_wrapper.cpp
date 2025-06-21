#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <iostream>
#include <iomanip>
#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include "world/procedural_terrain/noise/fractal_patterns.h"
#include "world/procedural_terrain/climate/climate_system.h"
#include "world/procedural_terrain/features/river_networks.h"
#include "world/procedural_terrain/utils/seed_utils.h"

using namespace VoxelCastle::World::ProceduralTerrain;
namespace py = pybind11;

// Constants from ProceduralTerrain design
constexpr float VOXEL_SCALE = 0.25f;  // 25cm voxels
constexpr float MAX_ELEVATION = 2048.0f;
constexpr float MIN_ELEVATION = -2048.0f;

// Efficient batch terrain generation for visualization
py::array_t<float> generate_terrain_heightmap(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    
    if (x_buf.size != z_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    // Convert world coordinates to actual world positions (accounting for voxel scale)
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        // Multi-scale terrain synthesis (4 scales for realistic terrain)
        float continental = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::CONTINENTAL, seed) * 1400.0f;
        float regional = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::REGIONAL, seed + 1000) * 500.0f;
        float local = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::LOCAL, seed + 2000) * 120.0f;
        float micro = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::MICRO, seed + 3000) * 30.0f;
        
        // Apply ocean/land detail scaling for realistic characteristics
        float base_elevation = continental + regional;
        if (base_elevation < -200.0f) {
            // Deep ocean - reduce fine detail for smoother ocean floors
            local *= 0.5f;
            micro *= 0.5f;
        } else if (base_elevation > 500.0f) {
            // Mountain regions - enhance detail for dramatic peaks and ridges
            local *= 1.25f;
            micro *= 1.25f;
        }
        
        float total_elevation = base_elevation + local + micro;
        
        // Final hard clamp to ±2048m range
        result_ptr[i] = std::max(MIN_ELEVATION, std::min(MAX_ELEVATION, total_elevation));
    }
    
    return result;
}

// Single point terrain generation
float generate_single_heightmap(float x, float z, uint64_t seed) {
    float world_x = x * VOXEL_SCALE;
    float world_z = z * VOXEL_SCALE;
    
    // Multi-scale terrain synthesis (4 scales for realistic terrain)
    float continental = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::CONTINENTAL, seed) * 1400.0f;
    float regional = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::REGIONAL, seed + 1000) * 500.0f;
    float local = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::LOCAL, seed + 2000) * 120.0f;
    float micro = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::MICRO, seed + 3000) * 30.0f;
    
    // Apply ocean/land detail scaling for realistic characteristics
    float base_elevation = continental + regional;
    if (base_elevation < -200.0f) {
        // Deep ocean - reduce fine detail for smoother ocean floors
        local *= 0.5f;
        micro *= 0.5f;
    } else if (base_elevation > 500.0f) {
        // Mountain regions - enhance detail for dramatic peaks and ridges
        local *= 1.25f;
        micro *= 1.25f;
    }
    
    float total_elevation = base_elevation + local + micro;
    
    return std::max(MIN_ELEVATION, std::min(MAX_ELEVATION, total_elevation));
}

// Climate system functions using the new ClimateSystem
py::dict generate_climate_data(float x, float z, uint64_t seed, float elevation) {
    float world_x = x * VOXEL_SCALE;
    float world_z = z * VOXEL_SCALE;
    
    // Use the new climate system
    ClimateData climate_data = ClimateSystem::CalculateClimate(world_x, world_z, elevation, seed);
    
    py::dict climate;
    climate["temperature"] = climate_data.temperature;
    climate["humidity"] = climate_data.humidity;
    climate["precipitation"] = climate_data.precipitation;
    climate["wind_exposure"] = climate_data.windExposure;
    climate["seasonality"] = climate_data.seasonality;
    
    return climate;
}

// Batch climate generation for efficient visualization
py::array_t<float> generate_climate_temperature(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    py::array_t<float> elevations,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    auto elev_buf = elevations.request();
    
    if (x_buf.size != z_buf.size || x_buf.size != elev_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* elev_ptr = static_cast<float*>(elev_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, elev_ptr[i], seed);
        result_ptr[i] = climate.temperature;
    }
    
    return result;
}

py::array_t<float> generate_climate_humidity(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    py::array_t<float> elevations,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    auto elev_buf = elevations.request();
    
    if (x_buf.size != z_buf.size || x_buf.size != elev_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* elev_ptr = static_cast<float*>(elev_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, elev_ptr[i], seed);
        result_ptr[i] = climate.humidity;
    }
    
    return result;
}

py::array_t<float> generate_climate_precipitation(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    py::array_t<float> elevations,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    auto elev_buf = elevations.request();
    
    if (x_buf.size != z_buf.size || x_buf.size != elev_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* elev_ptr = static_cast<float*>(elev_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, elev_ptr[i], seed);
        result_ptr[i] = climate.precipitation;
    }
    
    return result;
}

// Direct access to C++ noise functions
float generate_continental_noise(float x, float z, uint64_t seed) {
    return MultiScaleNoise::GenerateNoise(x * VOXEL_SCALE, z * VOXEL_SCALE, TerrainScale::CONTINENTAL, seed);
}

float generate_regional_noise(float x, float z, uint64_t seed) {
    return MultiScaleNoise::GenerateNoise(x * VOXEL_SCALE, z * VOXEL_SCALE, TerrainScale::REGIONAL, seed);
}

uint64_t create_subsystem_seed(uint64_t base_seed, const std::string& subsystem_name) {
    return SeedUtils::GetComponentSeed(base_seed, subsystem_name);
}

// Batch noise generation functions for efficient visualization  
py::array_t<float> generate_continental_noise_batch(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    
    if (x_buf.size != z_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        result_ptr[i] = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::CONTINENTAL, seed);
    }
    
    return result;
}

py::array_t<float> generate_regional_noise_batch(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    
    if (x_buf.size != z_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        result_ptr[i] = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::REGIONAL, seed);
    }
    
    return result;
}

// River network generation for visualization
py::array_t<float> generate_river_flow(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    py::array_t<float> elevations,
    py::array_t<float> precipitations,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    auto elev_buf = elevations.request();
    auto precip_buf = precipitations.request();
    
    if (x_buf.size != z_buf.size || x_buf.size != elev_buf.size || x_buf.size != precip_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* elev_ptr = static_cast<float*>(elev_buf.ptr);
    float* precip_ptr = static_cast<float*>(precip_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        // Use the new hierarchical fractal river system
        float flow_accumulation = RiverNetworks::CalculateFlowAccumulation(world_x, world_z, seed);
        result_ptr[i] = flow_accumulation;
    }
    
    return result;
}

py::array_t<float> generate_river_width(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    
    if (x_buf.size != z_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        // Calculate flow accumulation using new hierarchical system
        float river_strength = RiverNetworks::CalculateFlowAccumulation(world_x, world_z, seed);
        
        // Calculate river width from strength using new system
        result_ptr[i] = RiverNetworks::CalculateRiverWidth(river_strength, RiverConstants::CREEK_WIDTH);
    }
    
    return result;
}

// Enhanced terrain generation with river carving
py::array_t<float> generate_terrain_heightmap_with_rivers(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    
    if (x_buf.size != z_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    // Convert world coordinates to actual world positions (accounting for voxel scale)
    for (size_t i = 0; i < size; i++) {
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        // Multi-scale terrain synthesis (4 scales for realistic terrain)
        float continental = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::CONTINENTAL, seed) * 1400.0f;
        float regional = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::REGIONAL, seed + 1000) * 500.0f;
        float local = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::LOCAL, seed + 2000) * 120.0f;
        float micro = MultiScaleNoise::GenerateNoise(world_x, world_z, TerrainScale::MICRO, seed + 3000) * 30.0f;
        
        // Apply ocean/land detail scaling for realistic characteristics
        float base_elevation = continental + regional;
        if (base_elevation < -200.0f) {
            // Deep ocean - reduce fine detail for smoother ocean floors
            local *= 0.5f;
            micro *= 0.5f;
        } else if (base_elevation > 500.0f) {
            // Mountain regions - enhance detail for dramatic peaks and ridges
            local *= 1.25f;
            micro *= 1.25f;
        }
        
        float terrain_elevation = base_elevation + local + micro;
        
        // Calculate climate data for river generation
        ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, terrain_elevation, seed);
        
        // Generate comprehensive river data using new system
        EnhancedRiverData river = RiverNetworks::GenerateComprehensiveRiverData(
            world_x, world_z, climate, terrain_elevation, seed);
        
        // Apply river carving to terrain using new system
        float final_elevation = RiverNetworks::ApplyRiverCarving(terrain_elevation, world_x, world_z, seed);
        
        // Final hard clamp to ±2048m range
        result_ptr[i] = std::max(MIN_ELEVATION, std::min(MAX_ELEVATION, final_elevation));
    }
    
    return result;
}

// Comprehensive river data generation for visualization
py::dict generate_comprehensive_river_data(float x, float z, uint64_t seed) {
    float world_x = x * VOXEL_SCALE;
    float world_z = z * VOXEL_SCALE;
    
    // Get terrain elevation
    float elevation = generate_single_heightmap(x, z, seed);
    
    // Calculate climate data
    ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, elevation, seed);
    
    // Generate comprehensive river data using new system
    EnhancedRiverData river = RiverNetworks::GenerateComprehensiveRiverData(
        world_x, world_z, climate, elevation, seed);
    
    // Create Python dictionary with all river data
    py::dict river_dict;
    river_dict["fractal_river_strength"] = river.fractal_river_strength;
    river_dict["river_width_m"] = river.river_width_m;
    river_dict["river_depth_m"] = river.river_depth_m;
    river_dict["flow_direction_x"] = river.flow_direction.x;
    river_dict["flow_direction_y"] = river.flow_direction.y;
    river_dict["flow_velocity_ms"] = river.flow_velocity_ms;
    river_dict["stream_order"] = river.stream_order;
    river_dict["is_headwater"] = river.is_headwater;
    river_dict["is_main_stem"] = river.is_main_stem;
    river_dict["is_tributary"] = river.is_tributary;
    river_dict["carving_depth_m"] = river.carving_depth_m;
    river_dict["valley_width_m"] = river.valley_width_m;
    river_dict["creates_floodplain"] = river.creates_floodplain;
    river_dict["floodplain_width_m"] = river.floodplain_width_m;
    river_dict["meander_intensity"] = river.meander_intensity;
    river_dict["is_braided"] = river.is_braided;
    
    // Simplified waterfall/rapids data (derived from query)
    RiverQueryResult query = RiverNetworks::QueryRiverAtPoint(world_x, world_z, seed);
    river_dict["has_waterfall"] = query.has_waterfall;
    river_dict["has_rapids"] = query.has_rapids;
    
    return river_dict;
}

// Complete river data generation for comprehensive visualization
py::dict generate_river_data(float x, float z, uint64_t seed) {
    // Use the new comprehensive system but return legacy-compatible data
    py::dict comprehensive_data = generate_comprehensive_river_data(x, z, seed);
    
    py::dict river_dict;
    river_dict["flow_accumulation"] = comprehensive_data["fractal_river_strength"];
    river_dict["river_width"] = comprehensive_data["river_width_m"];
    river_dict["river_depth"] = comprehensive_data["river_depth_m"];
    river_dict["is_lake"] = false; // Legacy compatibility
    
    return river_dict;
}

// Batch comprehensive river data generation
py::array_t<float> generate_comprehensive_river_flow(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    uint64_t seed
) {
    auto x_buf = x_coords.request();
    auto z_buf = z_coords.request();
    
    if (x_buf.size != z_buf.size) {
        throw std::runtime_error("Input arrays must have the same size");
    }
    
    size_t size = x_buf.size;
    auto result = py::array_t<float>(size);
    auto result_buf = result.request();
    
    float* x_ptr = static_cast<float*>(x_buf.ptr);
    float* z_ptr = static_cast<float*>(z_buf.ptr);
    float* result_ptr = static_cast<float*>(result_buf.ptr);
    
    // Progress tracking for large datasets - more frequent updates for better feedback
    size_t progress_interval = std::max(size / 100, size_t(10000)); // Progress every 1% or 10k samples
    
    printf("Starting river query for %zu points...\n", size);
    fflush(stdout);
    
    for (size_t i = 0; i < size; i++) {
        // Show progress for large datasets (force flush for Python interop)
        if (i % progress_interval == 0 || i == size - 1) {
            float progress = (float)(i + 1) / (float)size * 100.0f;
            printf("\rProcessing rivers: %.1f%% (%zu/%zu)", 
                   progress, (i + 1), size);
            fflush(stdout);  // Force immediate output for Python interop
        }
        
        float world_x = x_ptr[i] * VOXEL_SCALE;
        float world_z = z_ptr[i] * VOXEL_SCALE;
        
        // Query river data
        RiverQueryResult river_result = RiverNetworks::QueryRiverAtPoint(world_x, world_z, seed);
        
        // Return river width as flow strength (0 if no river)
        result_ptr[i] = river_result.has_river ? river_result.river_width : 0.0f;
    }
    
    printf("\nProcessing rivers: Complete!\n");
    fflush(stdout);  // Force immediate output for Python interop
    return result;
}

// Simple river query for single point (used for river location finding)
float query_river_at_point(float x, float z, uint64_t seed) {
    float world_x = x * VOXEL_SCALE;
    float world_z = z * VOXEL_SCALE;
    
    RiverQueryResult river_result = RiverNetworks::QueryRiverAtPoint(world_x, world_z, seed);
    
    // Return river width as strength (0 if no river)
    return river_result.has_river ? river_result.river_width : 0.0f;
}

// Dynamic resolution calculation functions
int get_optimal_sample_count(float area_size, const std::string& feature_type = "terrain") {
    return RiverNetworks::GetOptimalSampleCount(area_size, feature_type);
}

float get_optimal_sampling_resolution(float area_size, const std::string& feature_type = "terrain") {
    return RiverNetworks::GetOptimalSamplingResolution(area_size, feature_type);
}

PYBIND11_MODULE(worldgen_cpp, m) {
    m.doc() = "C++ Terrain Generation for Voxel Castle ProceduralTerrain System";
    
    // Main terrain generation functions
    m.def("generate_terrain_heightmap", &generate_terrain_heightmap,
          "Generate heightmap using C++ MultiScaleNoise system (batch mode)",
          py::arg("x_coords"), py::arg("z_coords"), py::arg("seed"));
    
    m.def("generate_terrain_heightmap_with_rivers", &generate_terrain_heightmap_with_rivers,
          "Generate heightmap with river carving integrated (batch mode)",
          py::arg("x_coords"), py::arg("z_coords"), py::arg("seed"));
    
    m.def("generate_single_heightmap", &generate_single_heightmap,
          "Generate single heightmap point",
          py::arg("x"), py::arg("z"), py::arg("seed"));
    
    // Climate system
    m.def("generate_climate_data", &generate_climate_data,
          "Generate climate data for a point",
          py::arg("x"), py::arg("z"), py::arg("seed"), py::arg("elevation"));
    
    // Batch climate generation functions
    m.def("generate_climate_temperature", &generate_climate_temperature,
          "Generate temperature data for multiple points");
    m.def("generate_climate_humidity", &generate_climate_humidity,
          "Generate humidity data for multiple points");
    m.def("generate_climate_precipitation", &generate_climate_precipitation,
          "Generate precipitation data for multiple points");
    
    // River network functions
    m.def("generate_river_flow", &generate_river_flow,
          "Generate river flow accumulation data");
    m.def("generate_river_width", &generate_river_width,
          "Generate river width data");
    m.def("generate_river_data", &generate_river_data,
          "Generate complete river data for a point",
          py::arg("x"), py::arg("z"), py::arg("seed"));
    m.def("generate_comprehensive_river_data", &generate_comprehensive_river_data,
          "Generate comprehensive river data using new hierarchical fractal system",
          py::arg("x"), py::arg("z"), py::arg("seed"));
    m.def("generate_comprehensive_river_flow", &generate_comprehensive_river_flow,
          "Generate comprehensive river flow data for multiple points",
          py::arg("x_coords"), py::arg("z_coords"), py::arg("seed"));
    
    // Direct noise access
    m.def("generate_continental_noise", &generate_continental_noise,
          "Generate continental-scale noise");
    m.def("generate_regional_noise", &generate_regional_noise,
          "Generate regional-scale noise");
    
    // Batch noise generation functions
    m.def("generate_continental_noise_batch", &generate_continental_noise_batch,
          "Generate continental-scale noise for multiple points");
    m.def("generate_regional_noise_batch", &generate_regional_noise_batch,
          "Generate regional-scale noise for multiple points");
    
    // Utility functions
    m.def("create_subsystem_seed", &create_subsystem_seed,
          "Create subsystem-specific seed from base seed");
    
    // Expose TerrainScale enum
    py::enum_<TerrainScale>(m, "TerrainScale")
        .value("CONTINENTAL", TerrainScale::CONTINENTAL)
        .value("REGIONAL", TerrainScale::REGIONAL)
        .value("LOCAL", TerrainScale::LOCAL)
        .value("MICRO", TerrainScale::MICRO);
    
    // Expose EnhancedRiverData structure (new hierarchical system)
    py::class_<EnhancedRiverData>(m, "EnhancedRiverData")
        .def_readwrite("fractal_river_strength", &EnhancedRiverData::fractal_river_strength)
        .def_readwrite("river_width_m", &EnhancedRiverData::river_width_m)
        .def_readwrite("river_depth_m", &EnhancedRiverData::river_depth_m)
        .def_readwrite("flow_velocity_ms", &EnhancedRiverData::flow_velocity_ms)
        .def_readwrite("stream_order", &EnhancedRiverData::stream_order)
        .def_readwrite("is_headwater", &EnhancedRiverData::is_headwater)
        .def_readwrite("is_main_stem", &EnhancedRiverData::is_main_stem)
        .def_readwrite("is_tributary", &EnhancedRiverData::is_tributary)
        .def_readwrite("carving_depth_m", &EnhancedRiverData::carving_depth_m)
        .def_readwrite("valley_width_m", &EnhancedRiverData::valley_width_m)
        .def_readwrite("creates_floodplain", &EnhancedRiverData::creates_floodplain)
        .def_readwrite("floodplain_width_m", &EnhancedRiverData::floodplain_width_m)
        .def_readwrite("meander_intensity", &EnhancedRiverData::meander_intensity)
        .def_readwrite("is_braided", &EnhancedRiverData::is_braided);
    
    // Constants matching ProceduralTerrain design
    m.attr("VOXEL_SIZE") = VOXEL_SCALE;
    m.attr("MAX_ELEVATION") = MAX_ELEVATION;
    m.attr("MIN_ELEVATION") = MIN_ELEVATION;
    
    // River query functions
    m.def("query_river_at_point", &query_river_at_point,
          "Query river data at a single point (returns river width or 0)",
          py::arg("x"), py::arg("z"), py::arg("seed"));
    
    // Dynamic resolution functions
    m.def("get_optimal_sample_count", &get_optimal_sample_count,
          "Get optimal sample count for a given area size and feature type",
          py::arg("area_size"), py::arg("feature_type") = "terrain");
    m.def("get_optimal_sampling_resolution", &get_optimal_sampling_resolution,
          "Get optimal sampling resolution for a given area size and feature type",
          py::arg("area_size"), py::arg("feature_type") = "terrain");
}
