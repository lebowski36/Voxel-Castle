#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
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
        
        RiverData river = RiverNetworks::CalculateRiverData(world_x, world_z, elev_ptr[i], precip_ptr[i], seed);
        result_ptr[i] = river.flow_accumulation;
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
        
        // Get elevation and precipitation for this point to calculate flow
        float elevation = generate_single_heightmap(x_ptr[i], z_ptr[i], seed);
        ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, elevation, seed);
        
        // Calculate flow accumulation first
        float flow = RiverNetworks::CalculateFlowAccumulation(world_x, world_z, elevation, climate.precipitation, seed);
        
        // Then calculate river width from flow
        result_ptr[i] = RiverNetworks::CalculateRiverWidth(flow, elevation, seed);
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
        
        // Calculate river data
        RiverData river = RiverNetworks::CalculateRiverData(world_x, world_z, terrain_elevation, climate.precipitation, seed);
        
        // Apply river carving to terrain
        float final_elevation = terrain_elevation;
        if (river.river_width > 0.0f) {
            // Rivers carve channels into the terrain
            float carving_depth = river.river_depth + (river.river_width * 0.1f); // Additional carving based on width
            final_elevation -= carving_depth;
            
            // Ensure rivers don't carve below reasonable depths
            float min_river_elevation = terrain_elevation - 15.0f; // Max 15m carving depth
            final_elevation = std::max(final_elevation, min_river_elevation);
        }
        
        if (river.is_lake) {
            // Lakes create flat water surfaces
            float lake_depth = river.river_depth * 1.5f; // Lakes are deeper than rivers
            final_elevation = terrain_elevation - lake_depth;
        }
        
        // Final hard clamp to ±2048m range
        result_ptr[i] = std::max(MIN_ELEVATION, std::min(MAX_ELEVATION, final_elevation));
    }
    
    return result;
}

// Complete river data generation for comprehensive visualization
py::dict generate_river_data(float x, float z, uint64_t seed) {
    float world_x = x * VOXEL_SCALE;
    float world_z = z * VOXEL_SCALE;
    
    // Get elevation and climate data
    float elevation = generate_single_heightmap(x, z, seed);
    ClimateData climate = ClimateSystem::CalculateClimate(world_x, world_z, elevation, seed);
    
    // Calculate complete river data
    RiverData river = RiverNetworks::CalculateRiverData(world_x, world_z, elevation, climate.precipitation, seed);
    
    py::dict river_dict;
    river_dict["flow_accumulation"] = river.flow_accumulation;
    river_dict["river_width"] = river.river_width;
    river_dict["river_depth"] = river.river_depth;
    river_dict["is_lake"] = river.is_lake;
    
    return river_dict;
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
    
    // Expose RiverData structure
    py::class_<RiverData>(m, "RiverData")
        .def_readwrite("flow_accumulation", &RiverData::flow_accumulation)
        .def_readwrite("river_width", &RiverData::river_width)
        .def_readwrite("river_depth", &RiverData::river_depth)
        .def_readwrite("is_lake", &RiverData::is_lake);
    
    // Constants matching ProceduralTerrain design
    m.attr("VOXEL_SIZE") = VOXEL_SCALE;
    m.attr("MAX_ELEVATION") = MAX_ELEVATION;
    m.attr("MIN_ELEVATION") = MIN_ELEVATION;
}
