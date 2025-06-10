#include "world/ContinuousField.h"
#include "world/geological_data.h"  // For RockType enum
#include "util/noise.h"
#include <algorithm>
#include <cstring>

namespace VoxelCastle {
namespace World {

template<typename T>
ContinuousField<T>::ContinuousField(int width, int height, float spacing)
    : sampleSpacing_(spacing), width_(width), height_(height), noiseCacheValid_(false) {
    
    worldWidth_ = width * spacing;
    worldHeight_ = height * spacing;
    
    // Initialize sample grid
    samples_.resize(height_);
    for (int z = 0; z < height_; ++z) {
        samples_[z].resize(width_);
    }
    
    // Initialize noise cache
    noiseCache_.resize(width_ * height_);
}

template<typename T>
T ContinuousField<T>::sampleAt(float x, float z) const {
    // Wrap coordinates for toroidal topology
    Point wrapped = wrapCoordinates(x, z);
    x = wrapped.x;
    z = wrapped.z;
    
    // Convert to grid coordinates
    int gridX, gridZ;
    float fx, fz;
    worldToGrid(x, z, gridX, gridZ, fx, fz);
    
    // Get 4x4 sample grid for bicubic interpolation
    T samples[4][4];
    for (int dz = -1; dz <= 2; ++dz) {
        for (int dx = -1; dx <= 2; ++dx) {
            samples[dz + 1][dx + 1] = getSampleWrapped(gridX + dx, gridZ + dz);
        }
    }
    
    // Perform bicubic interpolation
    T baseValue = bicubicInterpolate(fx, fz, samples);
    
    // Add Perlin noise enhancement for organic variation
    float noiseValue = generatePerlinNoise(x, z, 4);
    
    // For scalar types, add noise directly
    if constexpr (std::is_arithmetic_v<T>) {
        return static_cast<T>(baseValue + noiseValue * 0.1f * baseValue);
    } else {
        // For complex types, return base value (noise can be added by derived classes)
        return baseValue;
    }
}

template<typename T>
void ContinuousField<T>::propagateValue(const T& value, float x, float z, float range,
                                       std::function<float(float, float)> resistanceFunc) {
    
    // Convert range to grid units
    int gridRange = static_cast<int>(std::ceil(range / sampleSpacing_));
    
    // Convert source position to grid coordinates
    int sourceX, sourceZ;
    float fx, fz;
    worldToGrid(x, z, sourceX, sourceZ, fx, fz);
    
    // Propagate in a square around the source
    for (int dz = -gridRange; dz <= gridRange; ++dz) {
        for (int dx = -gridRange; dx <= gridRange; ++dx) {
            int targetX = sourceX + dx;
            int targetZ = sourceZ + dz;
            
            // Calculate world position of target
            float targetWorldX = targetX * sampleSpacing_;
            float targetWorldZ = targetZ * sampleSpacing_;
            
            // Calculate distance with toroidal wrapping
            float distance = getToroidalDistance(x, z, targetWorldX, targetWorldZ);
            
            if (distance <= range) {
                // Calculate influence based on distance and resistance
                float resistance = resistanceFunc(targetWorldX, targetWorldZ);
                float distanceFactor = std::exp(-distance / (range * 0.3f)); // Exponential falloff
                float influence = distanceFactor * (1.0f / resistance);
                
                // Apply influence to target sample
                if constexpr (std::is_arithmetic_v<T>) {
                    T influenceValue = static_cast<T>(value * influence);
                    addToSample(targetX, targetZ, influenceValue);
                } else {
                    // For complex types, implement custom propagation
                    // This would need to be specialized for specific types
                    setSample(targetX, targetZ, value);
                }
            }
        }
    }
    
    invalidateNoiseCache();
}

template<typename T>
Point ContinuousField<T>::wrapCoordinates(float x, float z) const {
    Point result;
    
    // Wrap X coordinate
    result.x = std::fmod(x + worldWidth_, worldWidth_);
    if (result.x < 0) result.x += worldWidth_;
    
    // Wrap Z coordinate  
    result.z = std::fmod(z + worldHeight_, worldHeight_);
    if (result.z < 0) result.z += worldHeight_;
    
    return result;
}

template<typename T>
float ContinuousField<T>::getToroidalDistance(float x1, float z1, float x2, float z2) const {
    // Calculate distance in X direction (considering wrapping)
    float dx = std::abs(x2 - x1);
    dx = std::min(dx, worldWidth_ - dx);
    
    // Calculate distance in Z direction (considering wrapping)
    float dz = std::abs(z2 - z1);
    dz = std::min(dz, worldHeight_ - dz);
    
    return std::sqrt(dx * dx + dz * dz);
}

template<typename T>
void ContinuousField<T>::setSample(int x, int z, const T& value) {
    // Wrap indices
    x = ((x % width_) + width_) % width_;
    z = ((z % height_) + height_) % height_;
    
    samples_[z][x] = value;
    invalidateNoiseCache();
}

template<typename T>
T ContinuousField<T>::getSample(int x, int z) const {
    // Wrap indices
    x = ((x % width_) + width_) % width_;
    z = ((z % height_) + height_) % height_;
    
    return samples_[z][x];
}

template<typename T>
void ContinuousField<T>::addToSample(int x, int z, const T& value) {
    // Wrap indices
    x = ((x % width_) + width_) % width_;
    z = ((z % height_) + height_) % height_;
    
    if constexpr (std::is_arithmetic_v<T>) {
        samples_[z][x] += value;
    } else {
        // For complex types, this needs to be specialized
        samples_[z][x] = value;
    }
    
    invalidateNoiseCache();
}

template<typename T>
void ContinuousField<T>::clear() {
    for (auto& row : samples_) {
        for (auto& sample : row) {
            sample = T{};
        }
    }
    invalidateNoiseCache();
}

template<typename T>
void ContinuousField<T>::fill(const T& value) {
    for (auto& row : samples_) {
        for (auto& sample : row) {
            sample = value;
        }
    }
    invalidateNoiseCache();
}

template<typename T>
T ContinuousField<T>::bicubicInterpolate(float fx, float fz, const T samples[4][4]) const {
    // Hermite basis functions for smooth interpolation
    auto hermite = [](float t) -> std::array<float, 4> {
        float t2 = t * t;
        float t3 = t2 * t;
        return {{
            2*t3 - 3*t2 + 1,   // h00
            t3 - 2*t2 + t,     // h10  
            -2*t3 + 3*t2,      // h01
            t3 - t2            // h11
        }};
    };
    
    auto hx = hermite(fx);
    auto hz = hermite(fz);
    
    T result{};
    
    // Bicubic interpolation using tensor product of Hermite polynomials
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if constexpr (std::is_arithmetic_v<T>) {
                result += static_cast<T>(samples[i][j] * hx[j] * hz[i]);
            } else {
                // For complex types, use bilinear fallback or specialized implementation
                if (i == 1 && j == 1) {
                    result = samples[i][j];
                }
            }
        }
    }
    
    return result;
}

template<typename T>
float ContinuousField<T>::generatePerlinNoise(float x, float z, int octaves) const {
    // Use existing noise utility with multiple octaves
    float result = 0.0f;
    float amplitude = 1.0f;
    float frequency = 0.01f; // Base frequency
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        result += VoxelEngine::Util::smoothValueNoise(x * frequency, 0.0f, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    
    return result / maxValue; // Normalize to [-1, 1]
}

template<typename T>
void ContinuousField<T>::worldToGrid(float x, float z, int& gridX, int& gridZ, float& fx, float& fz) const {
    // Convert to grid coordinates
    float gridXf = x / sampleSpacing_;
    float gridZf = z / sampleSpacing_;
    
    gridX = static_cast<int>(std::floor(gridXf));
    gridZ = static_cast<int>(std::floor(gridZf));
    
    fx = gridXf - gridX;
    fz = gridZf - gridZ;
}

template<typename T>
T ContinuousField<T>::getSampleWrapped(int x, int z) const {
    return getSample(x, z); // getSample already handles wrapping
}

template<typename T>
void ContinuousField<T>::invalidateNoiseCache() const {
    noiseCacheValid_ = false;
}

// Explicit template instantiations for commonly used types
template class ContinuousField<float>;
template class ContinuousField<double>;
template class ContinuousField<int>;
template class ContinuousField<RockType>;  // Add RockType instantiation

} // namespace World
} // namespace VoxelCastle
