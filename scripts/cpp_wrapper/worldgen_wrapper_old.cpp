#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include "world/procedural_terrain/noise/fractal_patterns.h"
#include "world/procedural_terrain/utils/seed_utils.h"

using namespace VoxelCastle::World::ProceduralTerrain;
namespace py = pybind11;

// Efficient batch terrain generation for visualization
py::array_t<float> generate_terrain_heightmap(
    py::array_t<float> x_coords,
    py::array_t<float> z_coords,
    uint64_t seed,
    float voxel_scale = 0.25f
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
        float world_x = x_ptr[i] * voxel_scale;
        float world_z = z_ptr[i] * voxel_scale;
        
        // Generate heightmap using C++ terrain system
        float noise_value = MultiScaleNoise::GenerateHeightmapNoise(world_x, world_z, seed);
        
        // Convert to elevation in meters
        // Enhanced terrain generation with realistic distribution
        float base_elevation = noise_value * 1600.0f; // Base range: -1600m to +1600m
        
        // Add ridge noise for sharp mountain peaks
        float ridge_value = MultiScaleNoise::GenerateRidgeNoise(world_x, world_z, seed + 1000);
        float mountain_boost = ridge_value * ridge_value * 600.0f; // Sharp peaks up to +600m
        
        // Combine with soft clamping for realistic distribution
        float total_elevation = base_elevation + mountain_boost;
        
        // Soft clamping to avoid flat areas at extremes
        if (total_elevation > 1800.0f) {
            float excess = total_elevation - 1800.0f;
            total_elevation = 1800.0f + excess * 0.3f; // Compress extreme heights
        } else if (total_elevation < -1800.0f) {
            float excess = total_elevation + 1800.0f;
            total_elevation = -1800.0f + excess * 0.3f; // Compress extreme depths
        }
        
        // Final hard clamp (should rarely be needed)
        result_ptr[i] = std::max(-2048.0f, std::min(2048.0f, total_elevation));
    }
    
    return result;
}

// Single point terrain generation
float generate_single_heightmap(float x, float z, uint64_t seed, float voxel_scale = 0.25f) {
    float world_x = x * voxel_scale;
    float world_z = z * voxel_scale;
    
    float noise_value = MultiScaleNoise::GenerateHeightmapNoise(world_x, world_z, seed);
    float base_elevation = noise_value * 1600.0f;
    
    float ridge_value = MultiScaleNoise::GenerateRidgeNoise(world_x, world_z, seed + 1000);
    float mountain_boost = ridge_value * ridge_value * 600.0f;
    
    float total_elevation = base_elevation + mountain_boost;
    
    if (total_elevation > 1800.0f) {
        float excess = total_elevation - 1800.0f;
        total_elevation = 1800.0f + excess * 0.3f;
    } else if (total_elevation < -1800.0f) {
        float excess = total_elevation + 1800.0f;
        total_elevation = -1800.0f + excess * 0.3f;
    }
    
    return std::max(-2048.0f, std::min(2048.0f, total_elevation));
}
    return MultiScaleNoise::GenerateVoronoiNoise(x, z, seed, frequency);
}

uint64_t derive_seed(uint64_t base_seed, const std::string& feature_name) {
    SeedUtils seed_utils(base_seed);
    return seed_utils.GetNoiseSeed(feature_name);
}

pybind11::array_t<float> generate_heightmap_array(int width, int height, 
                                                  float min_x, float min_z,
                                                  float max_x, float max_z,
                                                  uint64_t seed) {
    auto result = pybind11::array_t<float>(width * height);
    pybind11::buffer_info buf = result.request();
    float* ptr = static_cast<float*>(buf.ptr);
    
    float dx = (max_x - min_x) / (width - 1);
    float dz = (max_z - min_z) / (height - 1);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float world_x = min_x + x * dx;
            float world_z = min_z + y * dz;
            ptr[y * width + x] = MultiScaleNoise::GenerateHeightmapNoise(world_x, world_z, seed);
        }
    }
    
    result.resize({height, width});
    return result;
}

PYBIND11_MODULE(worldgen_cpp, m) {
    m.doc() = "C++ World Generation Python Wrapper";
    
    // Noise generation functions
    m.def("generate_heightmap_noise", &generate_heightmap_noise, 
          "Generate heightmap noise at specific coordinates");
    m.def("generate_multi_scale_noise", &generate_multi_scale_noise,
          "Generate combined multi-scale noise",
          pybind11::arg("x"), pybind11::arg("z"), pybind11::arg("seed"),
          pybind11::arg("continental_weight") = 0.4f,
          pybind11::arg("regional_weight") = 0.3f,
          pybind11::arg("local_weight") = 0.2f,
          pybind11::arg("micro_weight") = 0.1f);
    m.def("generate_ridge_noise", &generate_ridge_noise,
          "Generate ridge noise for mountain ranges");
    m.def("generate_voronoi_noise", &generate_voronoi_noise,
          "Generate Voronoi noise for cellular patterns",
          pybind11::arg("x"), pybind11::arg("z"), pybind11::arg("seed"),
          pybind11::arg("frequency") = 0.01f);
    
    // Utility functions
    m.def("derive_seed", &derive_seed, "Derive feature-specific seed from base seed");
    
    // Array generation for efficient batch processing
    m.def("generate_heightmap_array", &generate_heightmap_array,
          "Generate heightmap array for visualization");
}
