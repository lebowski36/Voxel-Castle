#pragma once

#include <cstdint>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

/**
 * @brief Terrain scale levels for multi-scale noise generation
 */
enum class TerrainScale {
    CONTINENTAL,  ///< 10km+ wavelength - Major mountain ranges, continental features
    REGIONAL,     ///< 1-5km wavelength - Individual mountains, hills, large valleys
    LOCAL,        ///< 100-500m wavelength - Ridges, valleys, local terrain detail
    MICRO         ///< 10-50m wavelength - Surface roughness, small features
};

/**
 * @brief Multi-scale noise generation system for realistic terrain
 * 
 * Provides layered noise generation at different scales to create
 * natural-looking terrain with features ranging from continental
 * mountain ranges down to local surface detail.
 */
class MultiScaleNoise {
public:
    /**
     * @brief Generate noise value at specified coordinates and scale
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param scale Terrain scale level
     * @param seed Noise seed for deterministic generation
     * @return Noise value (-1.0 to 1.0)
     */
    static float GenerateNoise(float x, float z, TerrainScale scale, uint64_t seed);
    
    /**
     * @brief Generate combined multi-scale noise
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Base seed for generation
     * @param continentalWeight Weight for continental scale (default: 0.4)
     * @param regionalWeight Weight for regional scale (default: 0.3)
     * @param localWeight Weight for local scale (default: 0.2)
     * @param microWeight Weight for micro scale (default: 0.1)
     * @return Combined noise value (-1.0 to 1.0)
     */
    static float GenerateCombinedNoise(float x, float z, uint64_t seed,
                                       float continentalWeight = 0.4f,
                                       float regionalWeight = 0.3f,
                                       float localWeight = 0.2f,
                                       float microWeight = 0.1f);
    
    /**
     * @brief Generate heightmap noise suitable for terrain generation
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Noise seed
     * @return Height value (0.0 to 1.0, suitable for scaling to terrain height)
     */
    static float GenerateHeightmapNoise(float x, float z, uint64_t seed);
    
    /**
     * @brief Generate ridge noise for mountain ranges
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Noise seed
     * @return Ridge noise value (0.0 to 1.0)
     */
    static float GenerateRidgeNoise(float x, float z, uint64_t seed);
    
    /**
     * @brief Generate Voronoi noise for cellular patterns
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Noise seed
     * @param frequency Frequency of the pattern
     * @return Voronoi noise value (0.0 to 1.0)
     */
    static float GenerateVoronoiNoise(float x, float z, uint64_t seed, float frequency = 0.01f);
    
    /**
     * @brief Basic Perlin noise implementation (public for use by other systems)
     * @param x X coordinate
     * @param z Z coordinate
     * @param seed Noise seed
     * @param frequency Noise frequency
     * @return Noise value (-1.0 to 1.0)
     */
    static float PerlinNoise(float x, float z, uint64_t seed, float frequency);

private:
    
    /**
     * @brief Simplex noise implementation (alternative to Perlin)
     * @param x X coordinate
     * @param z Z coordinate
     * @param seed Noise seed
     * @param frequency Noise frequency
     * @return Noise value (-1.0 to 1.0)
     */
    static float SimplexNoise(float x, float z, uint64_t seed, float frequency);
    
    /**
     * @brief Smooth interpolation function
     * @param a First value
     * @param b Second value
     * @param t Interpolation factor (0.0 to 1.0)
     * @return Interpolated value
     */
    static float SmoothStep(float a, float b, float t);
    
    /**
     * @brief Hash function for noise generation
     * @param x X coordinate
     * @param z Z coordinate
     * @param seed Seed value
     * @return Hash value
     */
    static uint32_t Hash(int x, int z, uint64_t seed);
    
    /**
     * @brief Generate gradient vector for noise
     * @param hash Hash value
     * @param x X offset
     * @param z Z offset
     * @return Dot product of gradient and offset
     */
    static float Gradient(uint32_t hash, float x, float z);
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
