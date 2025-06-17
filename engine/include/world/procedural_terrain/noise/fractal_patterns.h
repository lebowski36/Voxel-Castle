#pragma once

#include <cstdint>
#include <vector>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

/**
 * @brief Fractal pattern generation for natural geological features
 * 
 * Provides algorithms for generating fractal patterns that mimic
 * natural geological formations like mountain ridges, river networks,
 * cave systems, and ore veins.
 */
class FractalPatterns {
public:
    /**
     * @brief Generate fractal mountain ridge pattern
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Seed for deterministic generation
     * @param octaves Number of fractal octaves (default: 6)
     * @param persistence Amplitude decay per octave (default: 0.5)
     * @param lacunarity Frequency increase per octave (default: 2.0)
     * @return Ridge pattern value (0.0 to 1.0)
     */
    static float GenerateMountainRidge(float x, float z, uint64_t seed,
                                       int octaves = 6,
                                       float persistence = 0.5f,
                                       float lacunarity = 2.0f);
    
    /**
     * @brief Generate fractal river network pattern
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Seed for deterministic generation
     * @param branchingFactor Controls river branching (default: 0.3)
     * @return River network value (0.0 = no river, 1.0 = main river)
     */
    static float GenerateRiverNetwork(float x, float z, uint64_t seed,
                                      float branchingFactor = 0.3f);
    
    /**
     * @brief Generate fractal cave system pattern
     * @param x X coordinate in world space
     * @param y Y coordinate in world space (depth)
     * @param z Z coordinate in world space
     * @param seed Seed for deterministic generation
     * @param density Cave density factor (default: 0.1)
     * @return Cave presence value (0.0 = solid, 1.0 = cave)
     */
    static float GenerateCaveSystem(float x, float y, float z, uint64_t seed,
                                    float density = 0.1f);
    
    /**
     * @brief Generate fractal ore vein pattern
     * @param x X coordinate in world space
     * @param y Y coordinate in world space (depth)
     * @param z Z coordinate in world space
     * @param seed Seed for deterministic generation
     * @param veinThickness Controls ore vein thickness (default: 0.05)
     * @return Ore vein presence (0.0 = no ore, 1.0 = ore present)
     */
    static float GenerateOreVein(float x, float y, float z, uint64_t seed,
                                 float veinThickness = 0.05f);
    
    /**
     * @brief Generate fractal coastline pattern
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Seed for deterministic generation
     * @param roughness Coastline roughness factor (default: 0.7)
     * @return Distance to coastline (negative = water, positive = land)
     */
    static float GenerateCoastline(float x, float z, uint64_t seed,
                                   float roughness = 0.7f);
    
    /**
     * @brief Generate fractal erosion pattern
     * @param x X coordinate in world space
     * @param z Z coordinate in world space
     * @param seed Seed for deterministic generation
     * @param intensity Erosion intensity (default: 0.5)
     * @return Erosion factor (0.0 = no erosion, 1.0 = heavy erosion)
     */
    static float GenerateErosionPattern(float x, float z, uint64_t seed,
                                        float intensity = 0.5f);

private:
    /**
     * @brief Generate fractal Brownian motion (fBm)
     * @param x X coordinate
     * @param z Z coordinate
     * @param seed Noise seed
     * @param octaves Number of octaves
     * @param persistence Amplitude decay
     * @param lacunarity Frequency increase
     * @param frequency Base frequency
     * @return fBm value
     */
    static float FractalBrownianMotion(float x, float z, uint64_t seed,
                                       int octaves, float persistence,
                                       float lacunarity, float frequency);
    
    /**
     * @brief Generate 3D fractal Brownian motion
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     * @param seed Noise seed
     * @param octaves Number of octaves
     * @param persistence Amplitude decay
     * @param lacunarity Frequency increase
     * @param frequency Base frequency
     * @return 3D fBm value
     */
    static float FractalBrownianMotion3D(float x, float y, float z, uint64_t seed,
                                         int octaves, float persistence,
                                         float lacunarity, float frequency);
    
    /**
     * @brief Generate ridged noise for sharp geological features
     * @param x X coordinate
     * @param z Z coordinate
     * @param seed Noise seed
     * @param frequency Noise frequency
     * @return Ridged noise value
     */
    static float RidgedNoise(float x, float z, uint64_t seed, float frequency);
    
    /**
     * @brief Generate 3D ridged noise
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     * @param seed Noise seed
     * @param frequency Noise frequency
     * @return 3D ridged noise value
     */
    static float RidgedNoise3D(float x, float y, float z, uint64_t seed, float frequency);
    
    /**
     * @brief Generate turbulence pattern
     * @param x X coordinate
     * @param z Z coordinate
     * @param seed Noise seed
     * @param octaves Number of octaves
     * @param frequency Base frequency
     * @return Turbulence value
     */
    static float Turbulence(float x, float z, uint64_t seed, int octaves, float frequency);
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
