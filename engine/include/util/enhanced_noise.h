// engine/include/util/enhanced_noise.h
#pragma once
#include "world/world_seed.h"
#include <cstdint>
#include <cmath>

namespace VoxelEngine {
namespace Util {

/**
 * Enhanced noise system that integrates with the WorldSeed system
 * for consistent, feature-separated noise generation.
 */
class EnhancedNoise {
public:
    // Constructor with world seed reference
    explicit EnhancedNoise(const VoxelCastle::World::WorldSeed& worldSeed);
    
    // Seeded hash function for pseudo-randomness
    uint32_t hash(int x, int y, int z, VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN) const;
    
    // Value noise in [0,1] with feature separation
    float valueNoise(int x, int y, int z, VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN) const;
    
    // Smoothed value noise (trilinear interpolation) with feature separation
    float smoothValueNoise(float x, float y, float z, VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN) const;
    
    // Multi-octave noise for more complex patterns
    float octaveNoise(float x, float y, float z, int octaves, float persistence = 0.5f, 
                     VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN) const;
    
    // Ridged noise for mountain-like features
    float ridgedNoise(float x, float y, float z, int octaves, float persistence = 0.5f,
                     VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN) const;
    
    // Domain warping for more natural-looking terrain
    float warpedNoise(float x, float y, float z, float warpStrength = 0.1f,
                     VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN) const;
    
    // Scale-aware noise that automatically adjusts based on coordinate scale
    float scaleAwareNoise(float x, float y, float z, VoxelCastle::World::ScaleLevel scale,
                         VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN) const;
    
    // Legacy compatibility function (maps to existing smoothValueNoise)
    static float legacySmoothValueNoise(float x, float y, float z);

private:
    const VoxelCastle::World::WorldSeed& worldSeed_;
    
    // Internal smoothing functions
    float lerp(float a, float b, float t) const;
    float smoothstep(float t) const;
};

// Legacy noise functions for backward compatibility
// These use the original noise.h implementation
inline uint32_t legacyHash(int x, int y, int z) {
    uint32_t h = static_cast<uint32_t>(x) * 374761393u + static_cast<uint32_t>(y) * 668265263u + static_cast<uint32_t>(z) * 2147483647u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

inline float legacyValueNoise(int x, int y, int z) {
    return static_cast<float>(legacyHash(x, y, z) & 0xFFFFFF) / static_cast<float>(0xFFFFFF);
}

inline float legacySmoothValueNoise(float x, float y, float z) {
    int ix = static_cast<int>(std::floor(x));
    int iy = static_cast<int>(std::floor(y));
    int iz = static_cast<int>(std::floor(z));
    float fx = x - ix;
    float fy = y - iy;
    float fz = z - iz;

    float v000 = legacyValueNoise(ix,   iy,   iz);
    float v100 = legacyValueNoise(ix+1, iy,   iz);
    float v010 = legacyValueNoise(ix,   iy+1, iz);
    float v110 = legacyValueNoise(ix+1, iy+1, iz);
    float v001 = legacyValueNoise(ix,   iy,   iz+1);
    float v101 = legacyValueNoise(ix+1, iy,   iz+1);
    float v011 = legacyValueNoise(ix,   iy+1, iz+1);
    float v111 = legacyValueNoise(ix+1, iy+1, iz+1);

    float v00 = v000 * (1-fx) + v100 * fx;
    float v01 = v001 * (1-fx) + v101 * fx;
    float v10 = v010 * (1-fx) + v110 * fx;
    float v11 = v011 * (1-fx) + v111 * fx;

    float v0 = v00 * (1-fy) + v10 * fy;
    float v1 = v01 * (1-fy) + v11 * fy;

    return v0 * (1-fz) + v1 * fz;
}

} // namespace Util
} // namespace VoxelEngine
