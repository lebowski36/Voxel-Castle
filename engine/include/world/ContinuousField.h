#pragma once

#include <vector>
#include <functional>
#include <cmath>

namespace VoxelCastle {
namespace World {

/**
 * @brief Point structure for 2D coordinates
 */
struct Point {
    float x, z;
    Point(float x = 0.0f, float z = 0.0f) : x(x), z(z) {}
};

/**
 * @brief Continuous field template class for smooth geological data interpolation
 * 
 * This class provides the mathematical foundation for all geological simulations,
 * offering smooth, organic interpolation between data points with no grid artifacts.
 * 
 * Key Features:
 * - Perlin-enhanced bicubic interpolation for organic results
 * - Geological resistance model for realistic force propagation
 * - Toroidal wrapping for seamless world boundaries
 * - High-performance sampling for real-time chunk generation
 */
template<typename T>
class ContinuousField {
private:
    std::vector<std::vector<T>> samples_;
    float sampleSpacing_;
    float worldWidth_;
    float worldHeight_;
    int width_;
    int height_;
    
    // Noise generation for organic enhancement
    mutable std::vector<float> noiseCache_;
    mutable bool noiseCacheValid_;
    
public:
    /**
     * @brief Constructor
     * @param width Number of samples in X direction
     * @param height Number of samples in Z direction  
     * @param spacing Distance between samples in world units
     */
    ContinuousField(int width, int height, float spacing);
    
    /**
     * @brief Destructor
     */
    ~ContinuousField() = default;
    
    // Copy and move semantics
    ContinuousField(const ContinuousField& other) = default;
    ContinuousField& operator=(const ContinuousField& other) = default;
    ContinuousField(ContinuousField&& other) noexcept = default;
    ContinuousField& operator=(ContinuousField&& other) noexcept = default;
    
    /**
     * @brief Sample the field at any world position with Perlin-enhanced interpolation
     * @param x World X coordinate
     * @param z World Z coordinate
     * @return Interpolated value at the given position
     */
    T sampleAt(float x, float z) const;
    
    /**
     * @brief Propagate a value through the field using geological resistance model
     * @param value The value to propagate
     * @param x World X coordinate of source
     * @param z World Z coordinate of source
     * @param range Maximum propagation range
     * @param resistanceFunc Function that returns resistance factor for each position
     */
    void propagateValue(const T& value, float x, float z, float range, 
                       std::function<float(float, float)> resistanceFunc);
    
    /**
     * @brief Wrap coordinates for toroidal world topology
     * @param x World X coordinate
     * @param z World Z coordinate
     * @return Wrapped coordinates
     */
    Point wrapCoordinates(float x, float z) const;
    
    /**
     * @brief Calculate distance accounting for toroidal wrapping
     * @param x1 First point X coordinate
     * @param z1 First point Z coordinate
     * @param x2 Second point X coordinate
     * @param z2 Second point Z coordinate
     * @return Minimum distance considering wrapping
     */
    float getToroidalDistance(float x1, float z1, float x2, float z2) const;
    
    /**
     * @brief Set a sample value at grid coordinates
     * @param x Grid X index
     * @param z Grid Z index
     * @param value Value to set
     */
    void setSample(int x, int z, const T& value);
    
    /**
     * @brief Get a sample value at grid coordinates
     * @param x Grid X index
     * @param z Grid Z index
     * @return Sample value
     */
    T getSample(int x, int z) const;
    
    /**
     * @brief Add a value to an existing sample
     * @param x Grid X index
     * @param z Grid Z index
     * @param value Value to add
     */
    void addToSample(int x, int z, const T& value);
    
    /**
     * @brief Get field dimensions
     */
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    float getSampleSpacing() const { return sampleSpacing_; }
    float getWorldWidth() const { return worldWidth_; }
    float getWorldHeight() const { return worldHeight_; }
    
    /**
     * @brief Clear all sample data
     */
    void clear();
    
    /**
     * @brief Fill entire field with a value
     * @param value Value to fill with
     */
    void fill(const T& value);
    
private:
    /**
     * @brief Bicubic interpolation between 16 surrounding samples
     * @param fx Fractional X position (0-1)
     * @param fz Fractional Z position (0-1)
     * @param samples 4x4 array of surrounding sample values
     * @return Interpolated value
     */
    T bicubicInterpolate(float fx, float fz, const T samples[4][4]) const;
    
    /**
     * @brief Generate Perlin noise for organic enhancement
     * @param x World X coordinate
     * @param z World Z coordinate
     * @param octaves Number of noise octaves
     * @return Noise value (-1 to 1)
     */
    float generatePerlinNoise(float x, float z, int octaves = 4) const;
    
    /**
     * @brief Convert world coordinates to grid indices
     * @param x World X coordinate
     * @param z World Z coordinate
     * @param gridX Output grid X index
     * @param gridZ Output grid Z index
     * @param fx Output fractional X (0-1)
     * @param fz Output fractional Z (0-1)
     */
    void worldToGrid(float x, float z, int& gridX, int& gridZ, float& fx, float& fz) const;
    
    /**
     * @brief Get sample with bounds checking and wrapping
     * @param x Grid X index (can be negative or >= width)
     * @param z Grid Z index (can be negative or >= height)
     * @return Sample value
     */
    T getSampleWrapped(int x, int z) const;
    
    /**
     * @brief Invalidate noise cache
     */
    void invalidateNoiseCache() const;
};

} // namespace World
} // namespace VoxelCastle
