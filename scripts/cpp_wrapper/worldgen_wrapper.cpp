#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include "world/procedural_terrain/noise/fractal_patterns.h"
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

// Climate system functions (placeholder - matches design)
py::dict generate_climate_data(float x, float z, uint64_t seed, float elevation) {
    // TODO: Implement full climate system from PROCEDURAL_TERRAIN_DESIGN.md
    py::dict climate;
    
    // Placeholder climate calculation
    float world_x = x * VOXEL_SCALE;
    float world_z = z * VOXEL_SCALE;
    
    // Base temperature from latitude effect
    float latitude_factor = abs(world_z) / 10000.0f;
    float base_temp = 25.0f - (latitude_factor * 30.0f);
    
    // Elevation effect (-6.5°C per 1000m)
    float elevation_effect = -elevation * 0.0065f;
    
    climate["temperature"] = base_temp + elevation_effect;
    climate["humidity"] = 0.5f; // Placeholder
    climate["precipitation"] = 800.0f; // Placeholder
    
    return climate;
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

PYBIND11_MODULE(worldgen_cpp, m) {
    m.doc() = "C++ Terrain Generation for Voxel Castle ProceduralTerrain System";
    
    // Main terrain generation functions
    m.def("generate_terrain_heightmap", &generate_terrain_heightmap,
          "Generate heightmap using C++ MultiScaleNoise system (batch mode)",
          py::arg("x_coords"), py::arg("z_coords"), py::arg("seed"));
    
    m.def("generate_single_heightmap", &generate_single_heightmap,
          "Generate single heightmap point",
          py::arg("x"), py::arg("z"), py::arg("seed"));
    
    // Climate system
    m.def("generate_climate_data", &generate_climate_data,
          "Generate climate data for a point",
          py::arg("x"), py::arg("z"), py::arg("seed"), py::arg("elevation"));
    
    // Direct noise access
    m.def("generate_continental_noise", &generate_continental_noise,
          "Generate continental-scale noise");
    m.def("generate_regional_noise", &generate_regional_noise,
          "Generate regional-scale noise");
    
    // Utility functions
    m.def("create_subsystem_seed", &create_subsystem_seed,
          "Create subsystem-specific seed from base seed");
    
    // Expose TerrainScale enum
    py::enum_<TerrainScale>(m, "TerrainScale")
        .value("CONTINENTAL", TerrainScale::CONTINENTAL)
        .value("REGIONAL", TerrainScale::REGIONAL)
        .value("LOCAL", TerrainScale::LOCAL)
        .value("MICRO", TerrainScale::MICRO);
    
    // Constants matching ProceduralTerrain design
    m.attr("VOXEL_SIZE") = VOXEL_SCALE;
    m.attr("MAX_ELEVATION") = MAX_ELEVATION;
    m.attr("MIN_ELEVATION") = MIN_ELEVATION;
}
