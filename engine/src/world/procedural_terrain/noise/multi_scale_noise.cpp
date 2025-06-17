#include "world/procedural_terrain/noise/multi_scale_noise.h"
#include <cmath>
#include <algorithm>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

// Scale-specific frequency settings for realistic continental-scale terrain
// Reduced frequencies by 5-10x for proper continental-scale features
static constexpr float CONTINENTAL_FREQUENCY = 0.000002f;  // Ultra-low frequency for continents (~500km features)
static constexpr float REGIONAL_FREQUENCY = 0.00002f;     // Very low frequency for major mountain ranges (~50km features)  
static constexpr float LOCAL_FREQUENCY = 0.0002f;         // Low frequency for individual mountains (~5km features)
static constexpr float MICRO_FREQUENCY = 0.002f;          // Medium frequency for surface detail (~500m features)

float MultiScaleNoise::GenerateNoise(float x, float z, TerrainScale scale, uint64_t seed) {
    float frequency;
    
    switch (scale) {
        case TerrainScale::CONTINENTAL:
            frequency = CONTINENTAL_FREQUENCY;
            break;
        case TerrainScale::REGIONAL:
            frequency = REGIONAL_FREQUENCY;
            break;
        case TerrainScale::LOCAL:
            frequency = LOCAL_FREQUENCY;
            break;
        case TerrainScale::MICRO:
            frequency = MICRO_FREQUENCY;
            break;
        default:
            frequency = LOCAL_FREQUENCY;
            break;
    }
    
    return PerlinNoise(x, z, seed, frequency);
}

float MultiScaleNoise::GenerateCombinedNoise(float x, float z, uint64_t seed,
                                             float continentalWeight,
                                             float regionalWeight,
                                             float localWeight,
                                             float microWeight) {
    // Generate noise at each scale with different seed offsets
    float continental = GenerateNoise(x, z, TerrainScale::CONTINENTAL, seed + 1000);
    float regional = GenerateNoise(x, z, TerrainScale::REGIONAL, seed + 2000);
    float local = GenerateNoise(x, z, TerrainScale::LOCAL, seed + 3000);
    float micro = GenerateNoise(x, z, TerrainScale::MICRO, seed + 4000);
    
    // Combine with weights
    float combined = continental * continentalWeight +
                     regional * regionalWeight +
                     local * localWeight +
                     micro * microWeight;
    
    // Normalize to ensure output stays in reasonable range
    float totalWeight = continentalWeight + regionalWeight + localWeight + microWeight;
    if (totalWeight > 0.0f) {
        combined /= totalWeight;
    }
    
    return std::clamp(combined, -1.0f, 1.0f);
}

float MultiScaleNoise::GenerateHeightmapNoise(float x, float z, uint64_t seed) {
    // Generate combined noise with terrain-appropriate weights
    float noise = GenerateCombinedNoise(x, z, seed, 0.5f, 0.3f, 0.15f, 0.05f);
    
    // Convert from (-1,1) to (0,1) range for heightmap
    return (noise + 1.0f) * 0.5f;
}

float MultiScaleNoise::GenerateRidgeNoise(float x, float z, uint64_t seed) {
    // Use absolute value of noise to create ridge-like patterns
    float noise = PerlinNoise(x, z, seed, REGIONAL_FREQUENCY);
    return 1.0f - std::abs(noise);
}

float MultiScaleNoise::GenerateVoronoiNoise(float x, float z, uint64_t seed, float frequency) {
    // Scale coordinates by frequency
    float fx = x * frequency;
    float fz = z * frequency;
    
    // Find integer grid coordinates
    int ix = static_cast<int>(std::floor(fx));
    int iz = static_cast<int>(std::floor(fz));
    
    float minDist = 10000.0f;
    
    // Check surrounding cells for closest point
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            int cellX = ix + dx;
            int cellZ = iz + dz;
            
            // Generate pseudo-random point in cell
            uint32_t hash = Hash(cellX, cellZ, seed);
            float pointX = cellX + (hash & 0xFF) / 255.0f;
            float pointZ = cellZ + ((hash >> 8) & 0xFF) / 255.0f;
            
            // Calculate distance to point
            float distX = fx - pointX;
            float distZ = fz - pointZ;
            float dist = std::sqrt(distX * distX + distZ * distZ);
            
            minDist = std::min(minDist, dist);
        }
    }
    
    return std::clamp(minDist, 0.0f, 1.0f);
}

float MultiScaleNoise::PerlinNoise(float x, float z, uint64_t seed, float frequency) {
    // Scale coordinates by frequency
    x *= frequency;
    z *= frequency;
    
    // Find integer grid coordinates
    int x0 = static_cast<int>(std::floor(x));
    int z0 = static_cast<int>(std::floor(z));
    int x1 = x0 + 1;
    int z1 = z0 + 1;
    
    // Calculate fractional parts
    float fx = x - x0;
    float fz = z - z0;
    
    // Generate gradients at grid corners
    float n00 = Gradient(Hash(x0, z0, seed), fx, fz);
    float n10 = Gradient(Hash(x1, z0, seed), fx - 1, fz);
    float n01 = Gradient(Hash(x0, z1, seed), fx, fz - 1);
    float n11 = Gradient(Hash(x1, z1, seed), fx - 1, fz - 1);
    
    // Apply smooth interpolation
    float u = SmoothStep(0, 1, fx);
    float v = SmoothStep(0, 1, fz);
    
    // Bilinear interpolation
    float nx0 = n00 * (1 - u) + n10 * u;
    float nx1 = n01 * (1 - u) + n11 * u;
    float nxy = nx0 * (1 - v) + nx1 * v;
    
    return std::clamp(nxy, -1.0f, 1.0f);
}

float MultiScaleNoise::SimplexNoise(float x, float z, uint64_t seed, float frequency) {
    // Simplified 2D simplex noise implementation
    // For now, use Perlin noise as a placeholder
    // TODO: Implement proper simplex noise if needed for performance
    return PerlinNoise(x, z, seed, frequency);
}

float MultiScaleNoise::SmoothStep(float a, float b, float t) {
    // Hermite interpolation
    t = std::clamp(t, 0.0f, 1.0f);
    t = t * t * (3.0f - 2.0f * t);
    return a + t * (b - a);
}

uint32_t MultiScaleNoise::Hash(int x, int z, uint64_t seed) {
    // Simple hash function for noise generation
    uint32_t h = static_cast<uint32_t>(seed);
    h ^= static_cast<uint32_t>(x) * 374761393U;
    h ^= static_cast<uint32_t>(z) * 668265263U;
    h = (h ^ (h >> 13)) * 1274126177U;
    return h ^ (h >> 16);
}

float MultiScaleNoise::Gradient(uint32_t hash, float x, float z) {
    // Generate pseudo-random gradient vector and compute dot product
    int h = hash & 7; // Use lower 3 bits for 8 gradient directions
    float u = h < 4 ? x : z;
    float v = h < 4 ? z : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
