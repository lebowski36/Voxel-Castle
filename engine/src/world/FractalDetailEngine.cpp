#include "world/FractalDetailEngine.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace VoxelCastle {
namespace World {

// MultiOctaveNoise implementation
MultiOctaveNoise::MultiOctaveNoise(uint64_t seed, float frequency, float amplitude, int octaves, float persistence)
    : seed_(seed), baseFrequency_(frequency), baseAmplitude_(amplitude), octaves_(octaves), persistence_(persistence) {
}

float MultiOctaveNoise::Sample(float x, float z) const {
    float result = 0.0f;
    float frequency = baseFrequency_;
    float amplitude = baseAmplitude_;
    
    for (int i = 0; i < octaves_; ++i) {
        result += SimplexNoise(x * frequency, z * frequency, seed_ + i) * amplitude;
        frequency *= 2.0f;
        amplitude *= persistence_;
    }
    
    return result;
}

float MultiOctaveNoise::Sample(glm::vec2 position) const {
    return Sample(position.x, position.y);
}

float MultiOctaveNoise::SimplexNoise(float x, float z, uint64_t noiseSeed) const {
    // Simple gradient noise implementation
    // This is a simplified version - in production, use a proper simplex noise library
    
    int x0 = static_cast<int>(std::floor(x));
    int z0 = static_cast<int>(std::floor(z));
    int x1 = x0 + 1;
    int z1 = z0 + 1;
    
    float sx = x - x0;
    float sz = z - z0;
    
    // Smooth interpolation
    sx = sx * sx * (3.0f - 2.0f * sx);
    sz = sz * sz * (3.0f - 2.0f * sz);
    
    // Hash-based gradients
    float n00 = static_cast<float>(Hash(x0, z0, noiseSeed) % 1000) / 1000.0f * 2.0f - 1.0f;
    float n10 = static_cast<float>(Hash(x1, z0, noiseSeed) % 1000) / 1000.0f * 2.0f - 1.0f;
    float n01 = static_cast<float>(Hash(x0, z1, noiseSeed) % 1000) / 1000.0f * 2.0f - 1.0f;
    float n11 = static_cast<float>(Hash(x1, z1, noiseSeed) % 1000) / 1000.0f * 2.0f - 1.0f;
    
    // Bilinear interpolation
    float nx0 = n00 * (1.0f - sx) + n10 * sx;
    float nx1 = n01 * (1.0f - sx) + n11 * sx;
    
    return nx0 * (1.0f - sz) + nx1 * sz;
}

uint64_t MultiOctaveNoise::Hash(int x, int z, uint64_t seed) const {
    uint64_t h = seed;
    h ^= static_cast<uint64_t>(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= static_cast<uint64_t>(z) + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
}

// DetailCache implementation
template<typename T>
bool DetailCache<T>::TryGet(float x, float z, float resolution, T& result) const {
    CacheKey key = MakeKey(x, z, resolution);
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        result = it->second;
        return true;
    }
    return false;
}

template<typename T>
void DetailCache<T>::Store(float x, float z, float resolution, const T& result) {
    if (cache_.size() >= maxSize_) {
        // Simple eviction: clear half the cache
        auto it = cache_.begin();
        std::advance(it, cache_.size() / 2);
        cache_.erase(cache_.begin(), it);
    }
    
    CacheKey key = MakeKey(x, z, resolution);
    cache_[key] = result;
}

template<typename T>
void DetailCache<T>::Clear() {
    cache_.clear();
}

template<typename T>
typename DetailCache<T>::CacheKey DetailCache<T>::MakeKey(float x, float z, float resolution) const {
    // Quantize coordinates to cache grid
    int64_t qx = static_cast<int64_t>(std::round(x / resolution));
    int64_t qz = static_cast<int64_t>(std::round(z / resolution));
    int32_t qr = static_cast<int32_t>(std::round(resolution));
    
    return {qx, qz, qr};
}

// Explicit template instantiation
template class DetailCache<float>;

// FractalDetailEngine implementation
FractalDetailEngine::FractalDetailEngine(float worldSizeKm, uint64_t seed)
    : worldSizeKm_(worldSizeKm)
    , worldSizeM_(worldSizeKm * 1000.0f)
    , seed_(seed)
    , elevationCache_(10000)
    , cacheHits_(0)
    , cacheRequests_(0) {
    
    std::cout << "[FractalDetailEngine] Initializing with worldSize: " << worldSizeKm 
              << "km, seed: " << seed << std::endl;
    
    // Initialize multi-scale noise generators
    continentalNoise_ = std::make_unique<MultiOctaveNoise>(
        seed, 1.0f / CONTINENTAL_SCALE, CONTINENTAL_STRENGTH, 4, 0.6f);
    
    coastlineNoise_ = std::make_unique<MultiOctaveNoise>(
        seed + 1, 1.0f / COASTLINE_SCALE, COASTLINE_STRENGTH, 5, 0.5f);
    
    mountainNoise_ = std::make_unique<MultiOctaveNoise>(
        seed + 2, 1.0f / MOUNTAIN_SCALE, MOUNTAIN_STRENGTH, 6, 0.5f);
    
    hillNoise_ = std::make_unique<MultiOctaveNoise>(
        seed + 3, 1.0f / HILL_SCALE, HILL_STRENGTH, 4, 0.6f);
    
    fineNoise_ = std::make_unique<MultiOctaveNoise>(
        seed + 4, 1.0f / FINE_SCALE, FINE_STRENGTH, 3, 0.4f);
    
    std::cout << "[FractalDetailEngine] Initialized with " << worldSizeKm 
              << "km world, seed " << seed << std::endl;
}

FractalDetailEngine::~FractalDetailEngine() = default;

float FractalDetailEngine::GenerateDetailAt(float worldX, float worldZ, float baseElevation, 
                                           const GeologicalContext& context) {
    return GenerateDetailAtResolution(worldX, worldZ, baseElevation, context, FINE_SCALE);
}

float FractalDetailEngine::GenerateDetailAtResolution(float worldX, float worldZ, float baseElevation,
                                                    const GeologicalContext& context, float resolution) {
    cacheRequests_++;
    
    // Try cache first
    float cachedResult;
    if (elevationCache_.TryGet(worldX, worldZ, resolution, cachedResult)) {
        cacheHits_++;
        return cachedResult;
    }
    
    // Generate detailed elevation
    float detailedElevation = baseElevation;
    
    // Add continental-scale features
    if (resolution >= CONTINENTAL_SCALE * 0.1f) {
        detailedElevation += GenerateContinentalFeatures(worldX, worldZ, context);
    }
    
    // Add coastline detail
    if (resolution >= COASTLINE_SCALE * 0.1f) {
        detailedElevation += GenerateCoastlineDetail(worldX, worldZ, context);
    }
    
    // Add mountain detail
    if (resolution >= MOUNTAIN_SCALE * 0.1f) {
        detailedElevation += GenerateMountainDetail(worldX, worldZ, context);
    }
    
    // Add hill detail
    if (resolution >= HILL_SCALE * 0.1f) {
        float hillDetail = hillNoise_->Sample(worldX, worldZ);
        float hillWeight = CalculateGeologicalWeight(context, HILL_SCALE);
        detailedElevation += hillDetail * hillWeight;
    }
    
    // Add fine detail
    if (resolution >= FINE_SCALE * 0.1f) {
        detailedElevation += GenerateFineDetail(worldX, worldZ, context);
    }
    
    // Apply geological modifications
    detailedElevation = ApplyRockTypeModification(detailedElevation, context.rockType);
    detailedElevation = ApplyStressModification(detailedElevation, context.stress);
    detailedElevation = ApplyCrustalThicknessModification(detailedElevation, context.crustalThickness);
    
    // Cache result
    elevationCache_.Store(worldX, worldZ, resolution, detailedElevation);
    
    return detailedElevation;
}

float FractalDetailEngine::GenerateContinentalFeatures(float worldX, float worldZ, const GeologicalContext& context) {
    float continentalDetail = continentalNoise_->Sample(worldX, worldZ);
    
    // Modify based on continental proximity
    float continentalWeight = context.continentalProximity;
    
    // Apply geological weight
    continentalWeight *= CalculateGeologicalWeight(context, CONTINENTAL_SCALE);
    
    return continentalDetail * continentalWeight;
}

float FractalDetailEngine::GenerateCoastlineDetail(float worldX, float worldZ, const GeologicalContext& context) {
    float coastlineDetail = coastlineNoise_->Sample(worldX, worldZ);
    
    // Coastline features are strongest near the transition between land and ocean
    float coastlineWeight = 1.0f;
    if (context.distanceToCoast > 0.0f) {
        // Exponential decay based on distance to coast
        float maxDistance = 50000.0f; // 50km maximum influence
        coastlineWeight = std::exp(-context.distanceToCoast / maxDistance);
    }
    
    // Apply geological weight
    coastlineWeight *= CalculateGeologicalWeight(context, COASTLINE_SCALE);
    
    return coastlineDetail * coastlineWeight;
}

float FractalDetailEngine::GenerateMountainDetail(float worldX, float worldZ, const GeologicalContext& context) {
    float mountainDetail = mountainNoise_->Sample(worldX, worldZ);
    
    // Mountain features are enhanced by tectonic stress
    float stressWeight = std::min(1.0f, context.stress / 1000000.0f); // Normalize stress
    
    // Continental areas are more likely to have mountains
    float continentalWeight = context.continentalProximity;
    
    // Apply geological weight
    float totalWeight = stressWeight * continentalWeight * CalculateGeologicalWeight(context, MOUNTAIN_SCALE);
    
    return mountainDetail * totalWeight;
}

float FractalDetailEngine::GenerateFineDetail(float worldX, float worldZ, const GeologicalContext& context) {
    float fineDetail = fineNoise_->Sample(worldX, worldZ);
    
    // Fine detail is influenced by rock type and local geology
    float fineWeight = 1.0f;
    
    // Different rock types have different fine detail characteristics
    switch (context.rockType) {
        case RockType::IGNEOUS_GRANITE:
            fineWeight = 1.2f; // Granite has more varied fine detail
            break;
        case RockType::SEDIMENTARY_SANDSTONE:
            fineWeight = 0.8f; // Sandstone is more uniform
            break;
        case RockType::METAMORPHIC_SLATE:
            fineWeight = 1.0f; // Slate has moderate variation
            break;
        case RockType::IGNEOUS_BASALT:
            fineWeight = 0.6f; // Basalt is quite uniform
            break;
        default:
            fineWeight = 1.0f;
            break;
    }
    
    // Apply geological weight
    fineWeight *= CalculateGeologicalWeight(context, FINE_SCALE);
    
    return fineDetail * fineWeight;
}

void FractalDetailEngine::ClearCache() {
    elevationCache_.Clear();
    cacheHits_ = 0;
    cacheRequests_ = 0;
}

void FractalDetailEngine::GetCacheStats(size_t& cacheSize, float& hitRatio) const {
    cacheSize = elevationCache_.Size();
    hitRatio = (cacheRequests_ > 0) ? (static_cast<float>(cacheHits_) / cacheRequests_) : 0.0f;
}

float FractalDetailEngine::CalculateGeologicalWeight(const GeologicalContext& context, float /* featureScale */) {
    // Base weight
    float weight = 1.0f;
    
    // Adjust based on crustal thickness (thicker crust = more topographic variation)
    float normalizedThickness = context.crustalThickness / 35000.0f; // Normalize to average continental thickness
    weight *= (0.5f + normalizedThickness * 0.5f);
    
    // Adjust based on plate velocity (faster movement = more dynamic features)
    float velocityMagnitude = glm::length(context.plateVelocity);
    float normalizedVelocity = std::min(1.0f, velocityMagnitude / 0.1f); // 0.1 m/year is fast
    weight *= (0.8f + normalizedVelocity * 0.2f);
    
    return weight;
}

float FractalDetailEngine::ApplyRockTypeModification(float baseDetail, RockType rockType) {
    float modifier = 1.0f;
    
    // Different rock types have different erosion resistance and structural properties
    switch (rockType) {
        case RockType::IGNEOUS_GRANITE:
            modifier = 1.3f; // Granite is resistant and forms prominent features
            break;
        case RockType::SEDIMENTARY_SANDSTONE:
            modifier = 0.9f; // Sandstone erodes more easily
            break;
        case RockType::METAMORPHIC_SLATE:
            modifier = 1.1f; // Slate is moderately resistant
            break;
        case RockType::IGNEOUS_BASALT:
            modifier = 1.0f; // Basalt is baseline
            break;
        case RockType::SEDIMENTARY_LIMESTONE:
            modifier = 0.8f; // Limestone can be dissolved
            break;
        default:
            modifier = 1.0f;
            break;
    }
    
    return baseDetail * modifier;
}

float FractalDetailEngine::ApplyStressModification(float baseDetail, float stress) {
    // High stress areas can create more dramatic topography
    float stressModifier = 1.0f + std::min(0.5f, stress / 10000000.0f); // Cap at 50% increase
    return baseDetail * stressModifier;
}

float FractalDetailEngine::ApplyCrustalThicknessModification(float baseDetail, float crustalThickness) {
    // Thicker crust generally supports higher topography
    float normalizedThickness = crustalThickness / 35000.0f; // Normalize to average continental thickness
    float thicknessModifier = 0.5f + normalizedThickness * 0.5f;
    return baseDetail * thicknessModifier;
}

glm::vec2 FractalDetailEngine::WorldToNormalizedCoords(float worldX, float worldZ) const {
    return glm::vec2(worldX / worldSizeM_, worldZ / worldSizeM_);
}

} // namespace World
} // namespace VoxelCastle
