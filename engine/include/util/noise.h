// Simple value noise implementation for world generation
// engine/include/util/noise.h
#pragma once
#include <cstdint>
#include <cmath>

namespace VoxelEngine {
namespace Util {

// Hash function for pseudo-randomness
inline uint32_t hash(int x, int y, int z) {
    uint32_t h = static_cast<uint32_t>(x) * 374761393u + static_cast<uint32_t>(y) * 668265263u + static_cast<uint32_t>(z) * 2147483647u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

// Value noise in [0,1]
inline float valueNoise(int x, int y, int z) {
    return static_cast<float>(hash(x, y, z) & 0xFFFFFF) / static_cast<float>(0xFFFFFF);
}

// Smoothed value noise (trilinear interpolation)
inline float smoothValueNoise(float x, float y, float z) {
    int ix = static_cast<int>(std::floor(x));
    int iy = static_cast<int>(std::floor(y));
    int iz = static_cast<int>(std::floor(z));
    float fx = x - ix;
    float fy = y - iy;
    float fz = z - iz;

    float v000 = valueNoise(ix,   iy,   iz);
    float v100 = valueNoise(ix+1, iy,   iz);
    float v010 = valueNoise(ix,   iy+1, iz);
    float v110 = valueNoise(ix+1, iy+1, iz);
    float v001 = valueNoise(ix,   iy,   iz+1);
    float v101 = valueNoise(ix+1, iy,   iz+1);
    float v011 = valueNoise(ix,   iy+1, iz+1);
    float v111 = valueNoise(ix+1, iy+1, iz+1);

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
