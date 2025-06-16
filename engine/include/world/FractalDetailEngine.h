#pragma once

#include "world/GeologicalTypes.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

namespace VoxelCastle {
namespace World {

/**
 * @brief Multi-octave noise generator for geological detail
 */
class MultiOctaveNoise {
public:
    MultiOctaveNoise(uint64_t seed, float frequency, float amplitude, int octaves, float persistence = 0.5f);
    
    float Sample(float x, float z) const;
    float Sample(glm::vec2 position) const;
    
private:
    uint64_t seed_;
    float baseFrequency_;
    float baseAmplitude_;
    int octaves_;
    float persistence_;
    
    float SimplexNoise(float x, float z, uint64_t noiseSeed) const;
    uint64_t Hash(int x, int z, uint64_t seed) const;
};

/**
 * @brief Geological context for fractal detail generation
 */
struct GeologicalContext {
    float baseElevation;        // Base elevation from particle simulation
    float stress;               // Tectonic stress level
    float crustalThickness;     // Crustal thickness
    RockType rockType;          // Dominant rock type
    float distanceToCoast;      // Distance to nearest coastline (m)
    float continentalProximity; // How continental vs oceanic (0.0-1.0)
    glm::vec2 plateVelocity;    // Local tectonic movement
    
    GeologicalContext() : 
        baseElevation(0.0f), stress(0.0f), crustalThickness(35000.0f),
        rockType(RockType::SEDIMENTARY_SANDSTONE), distanceToCoast(0.0f),
        continentalProximity(0.5f), plateVelocity(0.0f) {}
};

/**
 * @brief Detail cache for performance optimization
 */
template<typename T>
class DetailCache {
public:
    DetailCache(size_t maxSize = 10000) : maxSize_(maxSize) {}
    
    bool TryGet(float x, float z, float resolution, T& result) const;
    void Store(float x, float z, float resolution, const T& result);
    void Clear();
    size_t Size() const { return cache_.size(); }
    
private:
    struct CacheKey {
        int64_t x, z;
        int32_t resolution;
        
        bool operator==(const CacheKey& other) const {
            return x == other.x && z == other.z && resolution == other.resolution;
        }
    };
    
    struct CacheKeyHash {
        size_t operator()(const CacheKey& key) const {
            return std::hash<int64_t>()(key.x) ^ 
                   (std::hash<int64_t>()(key.z) << 1) ^
                   (std::hash<int32_t>()(key.resolution) << 2);
        }
    };
    
    mutable std::unordered_map<CacheKey, T, CacheKeyHash> cache_;
    size_t maxSize_;
    
    CacheKey MakeKey(float x, float z, float resolution) const;
};

/**
 * @brief Fractal detail overlay system for geological terrain
 * 
 * Generates seamless geological detail that respects particle simulation results.
 * Provides multi-scale detail from continental features down to meter-scale variation.
 */
class FractalDetailEngine {
public:
    /**
     * @brief Constructor
     * @param worldSizeKm World size in kilometers
     * @param seed Random seed for reproducible results
     */
    FractalDetailEngine(float worldSizeKm, uint64_t seed);
    
    /**
     * @brief Destructor
     */
    ~FractalDetailEngine();
    
    /**
     * @brief Generate fractal detail at specific location
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param baseElevation Base elevation from particle simulation
     * @param context Geological context for detail generation
     * @return Detailed elevation incorporating fractal features
     */
    float GenerateDetailAt(float worldX, float worldZ, float baseElevation, 
                          const GeologicalContext& context);
    
    /**
     * @brief Generate detail with specific resolution/LOD
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param baseElevation Base elevation from particle simulation
     * @param context Geological context for detail generation
     * @param resolution Detail resolution in meters (larger = coarser detail)
     * @return Detailed elevation at specified resolution
     */
    float GenerateDetailAtResolution(float worldX, float worldZ, float baseElevation,
                                   const GeologicalContext& context, float resolution);
    
    /**
     * @brief Generate continental-scale features
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param context Geological context
     * @return Continental feature contribution to elevation
     */
    float GenerateContinentalFeatures(float worldX, float worldZ, const GeologicalContext& context);
    
    /**
     * @brief Generate coastline detail
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param context Geological context
     * @return Coastline detail contribution to elevation
     */
    float GenerateCoastlineDetail(float worldX, float worldZ, const GeologicalContext& context);
    
    /**
     * @brief Generate mountain and ridge detail
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param context Geological context
     * @return Mountain detail contribution to elevation
     */
    float GenerateMountainDetail(float worldX, float worldZ, const GeologicalContext& context);
    
    /**
     * @brief Generate fine-scale terrain detail
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param context Geological context
     * @return Fine detail contribution to elevation
     */
    float GenerateFineDetail(float worldX, float worldZ, const GeologicalContext& context);
    
    /**
     * @brief Clear detail cache
     */
    void ClearCache();
    
    /**
     * @brief Get cache statistics
     */
    void GetCacheStats(size_t& cacheSize, float& hitRatio) const;

private:
    // World configuration
    float worldSizeKm_;             // World size in kilometers
    float worldSizeM_;              // World size in meters
    uint64_t seed_;                 // Random seed
    
    // Multi-scale noise generators
    std::unique_ptr<MultiOctaveNoise> continentalNoise_;    // Large-scale continental shape
    std::unique_ptr<MultiOctaveNoise> coastlineNoise_;      // Coastline complexity
    std::unique_ptr<MultiOctaveNoise> mountainNoise_;       // Mountain ridge detail
    std::unique_ptr<MultiOctaveNoise> hillNoise_;           // Hill and valley detail
    std::unique_ptr<MultiOctaveNoise> fineNoise_;           // Fine-scale terrain variation
    
    // Performance caching
    mutable DetailCache<float> elevationCache_;
    mutable size_t cacheHits_;
    mutable size_t cacheRequests_;
    
    // Geological feature parameters
    static constexpr float CONTINENTAL_SCALE = 100000.0f;   // 100km continental features
    static constexpr float COASTLINE_SCALE = 10000.0f;      // 10km coastline features
    static constexpr float MOUNTAIN_SCALE = 5000.0f;        // 5km mountain features
    static constexpr float HILL_SCALE = 1000.0f;            // 1km hill features
    static constexpr float FINE_SCALE = 100.0f;             // 100m fine detail
    
    // Feature strength parameters
    static constexpr float CONTINENTAL_STRENGTH = 500.0f;    // Up to 500m continental variation
    static constexpr float COASTLINE_STRENGTH = 200.0f;      // Up to 200m coastline variation
    static constexpr float MOUNTAIN_STRENGTH = 1000.0f;      // Up to 1000m mountain variation
    static constexpr float HILL_STRENGTH = 100.0f;           // Up to 100m hill variation
    static constexpr float FINE_STRENGTH = 10.0f;            // Up to 10m fine variation
    
    // Helper functions
    float CalculateGeologicalWeight(const GeologicalContext& context, float featureScale);
    float ApplyRockTypeModification(float baseDetail, RockType rockType);
    float ApplyStressModification(float baseDetail, float stress);
    float ApplyCrustalThicknessModification(float baseDetail, float crustalThickness);
    glm::vec2 WorldToNormalizedCoords(float worldX, float worldZ) const;
};

} // namespace World
} // namespace VoxelCastle
