#include "world/procedural_terrain/noise/fractal_patterns.h"
#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include <cmath>
#include <algorithm>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

float FractalPatterns::GenerateMountainRidge(float x, float z, uint64_t seed,
                                             int octaves, float persistence, float lacunarity) {
    // Use ridged fractal noise for sharp mountain ridges
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 0.001f; // Base frequency for mountain scale
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        float ridged = RidgedNoise(x, z, seed + i * 100, frequency);
        value += ridged * amplitude;
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    // Normalize and ensure positive values for ridges
    if (maxValue > 0.0f) {
        value /= maxValue;
    }
    
    return std::clamp(value, 0.0f, 1.0f);
}

float FractalPatterns::GenerateRiverNetwork(float x, float z, uint64_t seed, float branchingFactor) {
    // Generate base flow direction using gradient of large-scale noise
    float epsilon = 10.0f; // Small offset for gradient calculation
    float height = MultiScaleNoise::GenerateHeightmapNoise(x, z, seed);
    float heightX = MultiScaleNoise::GenerateHeightmapNoise(x + epsilon, z, seed);
    float heightZ = MultiScaleNoise::GenerateHeightmapNoise(x, z + epsilon, seed);
    
    // Calculate gradient (flow direction)
    float gradientX = (heightX - height) / epsilon;
    float gradientZ = (heightZ - height) / epsilon;
    float gradientMagnitude = std::sqrt(gradientX * gradientX + gradientZ * gradientZ);
    
    // Generate noise for river network branching
    float branchNoise = FractalBrownianMotion(x, z, seed + 5000, 4, 0.5f, 2.0f, 0.01f);
    
    // River presence based on gradient and branching noise
    float riverStrength = 0.0f;
    if (gradientMagnitude > 0.01f) { // Only where there's slope
        riverStrength = (1.0f - gradientMagnitude) * (0.5f + branchNoise * branchingFactor);
    }
    
    return std::clamp(riverStrength, 0.0f, 1.0f);
}

float FractalPatterns::GenerateCaveSystem(float x, float y, float z, uint64_t seed, float density) {
    // Generate 3D fractal noise for cave networks
    float caveNoise = FractalBrownianMotion3D(x, y, z, seed + 6000, 5, 0.6f, 2.0f, 0.02f);
    
    // Apply depth-based scaling (more caves deeper underground)
    float depthFactor = std::max(0.0f, (y - 50.0f) / 100.0f); // Assuming y > 50 is underground
    depthFactor = std::clamp(depthFactor, 0.0f, 1.0f);
    
    // Cave threshold based on density and depth
    float threshold = 1.0f - (density * (0.5f + depthFactor * 0.5f));
    
    return (caveNoise > threshold) ? 1.0f : 0.0f;
}

float FractalPatterns::GenerateOreVein(float x, float y, float z, uint64_t seed, float veinThickness) {
    // Generate 3D ridged noise for ore veins
    float veinNoise = RidgedNoise3D(x, y, z, seed + 7000, 0.05f);
    
    // Apply depth-based ore distribution
    float depthFactor = std::clamp(y / 200.0f, 0.0f, 1.0f); // More ore deeper
    
    // Vein presence based on ridged noise and thickness
    float threshold = 1.0f - (veinThickness * (0.5f + depthFactor * 0.5f));
    
    return (veinNoise > threshold) ? 1.0f : 0.0f;
}

float FractalPatterns::GenerateCoastline(float x, float z, uint64_t seed, float roughness) {
    // Generate base distance field
    float baseDistance = FractalBrownianMotion(x, z, seed + 8000, 6, 0.7f, 2.0f, 0.001f);
    
    // Add fractal detail for irregular coastline
    float detail = FractalBrownianMotion(x, z, seed + 8500, 4, 0.5f, 3.0f, 0.01f);
    
    // Combine base shape with fractal detail
    float coastline = baseDistance + detail * roughness * 0.3f;
    
    return coastline;
}

float FractalPatterns::GenerateErosionPattern(float x, float z, uint64_t seed, float intensity) {
    // Generate turbulent pattern for erosion
    float erosion = Turbulence(x, z, seed + 9000, 5, 0.005f);
    
    // Scale by intensity
    erosion *= intensity;
    
    return std::clamp(erosion, 0.0f, 1.0f);
}

// Private helper methods

float FractalPatterns::FractalBrownianMotion(float x, float z, uint64_t seed,
                                             int octaves, float persistence,
                                             float lacunarity, float frequency) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += MultiScaleNoise::PerlinNoise(x, z, seed + i * 100, frequency) * amplitude;
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return maxValue > 0.0f ? value / maxValue : 0.0f;
}

float FractalPatterns::FractalBrownianMotion3D(float x, float y, float z, uint64_t seed,
                                               int octaves, float persistence,
                                               float lacunarity, float frequency) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        // Combine 2D noise at different Y offsets for 3D effect
        float noise1 = MultiScaleNoise::PerlinNoise(x, z + y * 0.1f, seed + i * 100, frequency);
        float noise2 = MultiScaleNoise::PerlinNoise(x + y * 0.1f, z, seed + i * 100 + 50, frequency);
        float noise3D = (noise1 + noise2) * 0.5f;
        
        value += noise3D * amplitude;
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return maxValue > 0.0f ? value / maxValue : 0.0f;
}

float FractalPatterns::RidgedNoise(float x, float z, uint64_t seed, float frequency) {
    float noise = MultiScaleNoise::PerlinNoise(x, z, seed, frequency);
    return 1.0f - std::abs(noise);
}

float FractalPatterns::RidgedNoise3D(float x, float y, float z, uint64_t seed, float frequency) {
    // Combine 2D noise with Y offset for 3D ridged effect
    float noise1 = MultiScaleNoise::PerlinNoise(x, z + y * 0.1f, seed, frequency);
    float noise2 = MultiScaleNoise::PerlinNoise(x + y * 0.1f, z, seed + 100, frequency);
    float noise3D = (noise1 + noise2) * 0.5f;
    
    return 1.0f - std::abs(noise3D);
}

float FractalPatterns::Turbulence(float x, float z, uint64_t seed, int octaves, float frequency) {
    float value = 0.0f;
    float amplitude = 1.0f;
    
    for (int i = 0; i < octaves; i++) {
        value += std::abs(MultiScaleNoise::PerlinNoise(x, z, seed + i * 100, frequency)) * amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    
    return value;
}

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
