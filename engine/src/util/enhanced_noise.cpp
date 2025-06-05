// engine/src/util/enhanced_noise.cpp
#include "util/enhanced_noise.h"
#include <algorithm>

namespace VoxelEngine {
namespace Util {

// Constructor
EnhancedNoise::EnhancedNoise(const VoxelCastle::World::WorldSeed& worldSeed) 
    : worldSeed_(worldSeed) {
}

// Seeded hash function for pseudo-randomness
uint32_t EnhancedNoise::hash(int x, int y, int z, VoxelCastle::World::FeatureType feature) const {
    // Get feature-specific seed for this location
    uint64_t seed = worldSeed_.getFeatureSeed(x, y, z, feature);
    
    // Use the seed to create deterministic hash
    uint32_t h = static_cast<uint32_t>(seed & 0xFFFFFFFF);
    h ^= static_cast<uint32_t>(seed >> 32);
    
    // Additional mixing with coordinates
    h ^= static_cast<uint32_t>(x) * 374761393u;
    h ^= static_cast<uint32_t>(y) * 668265263u;
    h ^= static_cast<uint32_t>(z) * 2147483647u;
    
    // Final mixing
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

// Value noise in [0,1] with feature separation
float EnhancedNoise::valueNoise(int x, int y, int z, VoxelCastle::World::FeatureType feature) const {
    return static_cast<float>(hash(x, y, z, feature) & 0xFFFFFF) / static_cast<float>(0xFFFFFF);
}

// Smoothed value noise (trilinear interpolation) with feature separation
float EnhancedNoise::smoothValueNoise(float x, float y, float z, VoxelCastle::World::FeatureType feature) const {
    int ix = static_cast<int>(std::floor(x));
    int iy = static_cast<int>(std::floor(y));
    int iz = static_cast<int>(std::floor(z));
    float fx = x - ix;
    float fy = y - iy;
    float fz = z - iz;

    // Apply smoothstep for better interpolation
    fx = smoothstep(fx);
    fy = smoothstep(fy);
    fz = smoothstep(fz);

    float v000 = valueNoise(ix,   iy,   iz,   feature);
    float v100 = valueNoise(ix+1, iy,   iz,   feature);
    float v010 = valueNoise(ix,   iy+1, iz,   feature);
    float v110 = valueNoise(ix+1, iy+1, iz,   feature);
    float v001 = valueNoise(ix,   iy,   iz+1, feature);
    float v101 = valueNoise(ix+1, iy,   iz+1, feature);
    float v011 = valueNoise(ix,   iy+1, iz+1, feature);
    float v111 = valueNoise(ix+1, iy+1, iz+1, feature);

    float v00 = lerp(v000, v100, fx);
    float v01 = lerp(v001, v101, fx);
    float v10 = lerp(v010, v110, fx);
    float v11 = lerp(v011, v111, fx);

    float v0 = lerp(v00, v10, fy);
    float v1 = lerp(v01, v11, fy);

    return lerp(v0, v1, fz);
}

// Multi-octave noise for more complex patterns
float EnhancedNoise::octaveNoise(float x, float y, float z, int octaves, float persistence, 
                                VoxelCastle::World::FeatureType feature) const {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        value += smoothValueNoise(x * frequency, y * frequency, z * frequency, feature) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return value / maxValue; // Normalize to [0, 1]
}

// Ridged noise for mountain-like features
float EnhancedNoise::ridgedNoise(float x, float y, float z, int octaves, float persistence,
                                VoxelCastle::World::FeatureType feature) const {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        float sample = smoothValueNoise(x * frequency, y * frequency, z * frequency, feature);
        // Invert and square for ridged effect
        sample = 1.0f - std::abs(sample * 2.0f - 1.0f);
        sample = sample * sample;
        
        value += sample * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return value / maxValue;
}

// Domain warping for more natural-looking terrain
float EnhancedNoise::warpedNoise(float x, float y, float z, float warpStrength,
                                VoxelCastle::World::FeatureType feature) const {
    // Generate warping offsets using different feature domains
    float warpX = smoothValueNoise(x * 0.1f, y * 0.1f, z * 0.1f, feature) * 2.0f - 1.0f;
    float warpY = smoothValueNoise((x + 100.0f) * 0.1f, (y + 100.0f) * 0.1f, (z + 100.0f) * 0.1f, feature) * 2.0f - 1.0f;
    float warpZ = smoothValueNoise((x + 200.0f) * 0.1f, (y + 200.0f) * 0.1f, (z + 200.0f) * 0.1f, feature) * 2.0f - 1.0f;
    
    // Apply warping
    float warpedX = x + warpX * warpStrength;
    float warpedY = y + warpY * warpStrength;
    float warpedZ = z + warpZ * warpStrength;
    
    // Sample noise at warped coordinates
    return smoothValueNoise(warpedX, warpedY, warpedZ, feature);
}

// Scale-aware noise that automatically adjusts based on coordinate scale
float EnhancedNoise::scaleAwareNoise(float x, float y, float z, VoxelCastle::World::ScaleLevel scale,
                                    VoxelCastle::World::FeatureType feature) const {
    // Adjust frequency and octaves based on scale
    float frequency = 1.0f;
    int octaves = 1;
    float persistence = 0.5f;
    
    switch (scale) {
        case VoxelCastle::World::ScaleLevel::BLOCK:
            frequency = 1.0f;
            octaves = 3;
            persistence = 0.6f;
            break;
            
        case VoxelCastle::World::ScaleLevel::CHUNK:
            frequency = 0.1f;
            octaves = 4;
            persistence = 0.5f;
            break;
            
        case VoxelCastle::World::ScaleLevel::REGION:
            frequency = 0.01f;
            octaves = 5;
            persistence = 0.4f;
            break;
            
        case VoxelCastle::World::ScaleLevel::CONTINENTAL:
            frequency = 0.001f;
            octaves = 6;
            persistence = 0.3f;
            break;
    }
    
    return octaveNoise(x * frequency, y * frequency, z * frequency, octaves, persistence, feature);
}

// Legacy compatibility function
float EnhancedNoise::legacySmoothValueNoise(float x, float y, float z) {
    return ::VoxelEngine::Util::legacySmoothValueNoise(x, y, z);
}

// Helper functions
float EnhancedNoise::lerp(float a, float b, float t) const {
    return a + t * (b - a);
}

float EnhancedNoise::smoothstep(float t) const {
    return t * t * (3.0f - 2.0f * t);
}

} // namespace Util
} // namespace VoxelEngine
