// engine/src/world/world_seed.cpp
#include "world/world_seed.h"
#include <random>
#include <functional>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace VoxelCastle {
namespace World {

// Feature type names for debugging
const char* getFeatureTypeName(FeatureType type) {
    switch (type) {
        case FeatureType::TERRAIN: return "TERRAIN";
        case FeatureType::CAVES: return "CAVES";
        case FeatureType::ORES: return "ORES";
        case FeatureType::STRUCTURES: return "STRUCTURES";
        case FeatureType::BIOMES: return "BIOMES";
        case FeatureType::WEATHER: return "WEATHER";
        case FeatureType::WATER: return "WATER";
        case FeatureType::VEGETATION: return "VEGETATION";
        default: return "UNKNOWN";
    }
}

const char* getScaleLevelName(ScaleLevel scale) {
    switch (scale) {
        case ScaleLevel::BLOCK: return "BLOCK";
        case ScaleLevel::CHUNK: return "CHUNK";
        case ScaleLevel::REGION: return "REGION";
        case ScaleLevel::CONTINENTAL: return "CONTINENTAL";
        default: return "UNKNOWN";
    }
}

// Constructor with random seed
WorldSeed::WorldSeed() {
    std::random_device rd;
    uint64_t seed = (static_cast<uint64_t>(rd()) << 32) | rd();
    setSeed(seed);
}

// Constructor with numeric seed
WorldSeed::WorldSeed(uint64_t seed) {
    setSeed(seed);
}

// Constructor with string seed
WorldSeed::WorldSeed(const std::string& seedString) {
    setSeedFromString(seedString);
}

// Set new master seed
void WorldSeed::setSeed(uint64_t seed) {
    masterSeed_ = avalancheHash(seed); // Apply high-quality bit mixing
    originalSeedString_.clear();
    featureDomainSeeds_.clear();
    initializeFeatureDomains();
    
    // Simple console logging for now
    std::cout << "[WorldSeed] Master seed set to: " << masterSeed_ << 
                 " (input: " << seed << ")" << std::endl;
}

void WorldSeed::setSeedFromString(const std::string& seedString) {
    originalSeedString_ = seedString;
    uint64_t seed = hashString(seedString);
    masterSeed_ = avalancheHash(seed);
    featureDomainSeeds_.clear();
    initializeFeatureDomains();
    
    std::cout << "[WorldSeed] Seed from string '" << seedString << "' -> " << 
                 seed << " -> " << masterSeed_ << std::endl;
}

// Get string representation
std::string WorldSeed::getSeedString() const {
    if (!originalSeedString_.empty()) {
        return originalSeedString_;
    }
    return std::to_string(masterSeed_);
}

// Initialize feature domain seeds
void WorldSeed::initializeFeatureDomains() {
    // Create unique seeds for each feature domain using prime multiplication
    static const uint64_t FEATURE_PRIMES[] = {
        2654435761ULL,  // TERRAIN
        4294967291ULL,  // CAVES  
        6700417233ULL,  // ORES
        9576890767ULL,  // STRUCTURES
        12884901888ULL, // BIOMES
        16106127360ULL, // WEATHER
        19327352832ULL, // WATER
        22548578304ULL  // VEGETATION
    };
    
    for (uint32_t i = 0; i < static_cast<uint32_t>(FeatureType::COUNT); ++i) {
        FeatureType feature = static_cast<FeatureType>(i);
        uint64_t domainSeed = avalancheHash(masterSeed_ ^ FEATURE_PRIMES[i]);
        featureDomainSeeds_[feature] = domainSeed;
        
        std::cout << "[WorldSeed] Feature domain " << getFeatureTypeName(feature) << 
                     " seed: " << domainSeed << std::endl;
    }
}

// Get feature domain seed
uint64_t WorldSeed::getFeatureDomainSeed(FeatureType feature) const {
    auto it = featureDomainSeeds_.find(feature);
    if (it != featureDomainSeeds_.end()) {
        return it->second;
    }
    
    // Fallback - shouldn't happen if properly initialized
    std::cerr << "[WorldSeed] WARNING: Feature domain not found for " << 
                 getFeatureTypeName(feature) << std::endl;
    return masterSeed_;
}

// Get block-level seed
uint64_t WorldSeed::getBlockSeed(int64_t x, int64_t y, int64_t z) const {
    return deriveSeed(masterSeed_, x, y, z);
}

// Get feature-specific block seed
uint64_t WorldSeed::getFeatureSeed(int64_t x, int64_t y, int64_t z, FeatureType feature) const {
    uint64_t domainSeed = getFeatureDomainSeed(feature);
    return deriveSeed(domainSeed, x, y, z);
}

// Get chunk-level seed
uint64_t WorldSeed::getChunkSeed(int64_t chunkX, int64_t chunkY, int64_t chunkZ) const {
    return deriveSeed(masterSeed_, chunkX, chunkY, chunkZ);
}

// Get feature-specific chunk seed
uint64_t WorldSeed::getChunkFeatureSeed(int64_t chunkX, int64_t chunkY, int64_t chunkZ, FeatureType feature) const {
    uint64_t domainSeed = getFeatureDomainSeed(feature);
    return deriveSeed(domainSeed, chunkX, chunkY, chunkZ);
}

// Get region-level seed  
uint64_t WorldSeed::getRegionSeed(int64_t regionX, int64_t regionY, int64_t regionZ) const {
    return deriveSeed(masterSeed_, regionX, regionY, regionZ);
}

// Get feature-specific region seed
uint64_t WorldSeed::getRegionFeatureSeed(int64_t regionX, int64_t regionY, int64_t regionZ, FeatureType feature) const {
    uint64_t domainSeed = getFeatureDomainSeed(feature);
    return deriveSeed(domainSeed, regionX, regionY, regionZ);
}

// Get scale-appropriate seed
uint64_t WorldSeed::getScaleSeed(int64_t x, int64_t y, int64_t z, ScaleLevel scale, FeatureType feature) const {
    switch (scale) {
        case ScaleLevel::BLOCK:
            return getFeatureSeed(x, y, z, feature);
            
        case ScaleLevel::CHUNK: {
            // Convert block coordinates to chunk coordinates
            int64_t chunkX = x >> 4;  // Divide by 16
            int64_t chunkY = y >> 5;  // Divide by 32 (chunk height)
            int64_t chunkZ = z >> 4;  // Divide by 16
            return getChunkFeatureSeed(chunkX, chunkY, chunkZ, feature);
        }
        
        case ScaleLevel::REGION: {
            // Convert block coordinates to region coordinates (16x16 chunks = 256x256 blocks)
            int64_t regionX = x >> 8;  // Divide by 256
            int64_t regionY = y >> 8;  // Divide by 256  
            int64_t regionZ = z >> 8;  // Divide by 256
            return getRegionFeatureSeed(regionX, regionY, regionZ, feature);
        }
        
        case ScaleLevel::CONTINENTAL: {
            // Very large scale - divide by 4096
            int64_t contX = x >> 12;  // Divide by 4096
            int64_t contY = y >> 12;  // Divide by 4096
            int64_t contZ = z >> 12;  // Divide by 4096
            return getRegionFeatureSeed(contX, contY, contZ, feature);
        }
        
        default:
            return getFeatureSeed(x, y, z, feature);
    }
}

// Utility: Convert seed to float [0.0, 1.0]
float WorldSeed::seedToFloat(uint64_t seed) {
    // Use upper 32 bits for better distribution
    uint32_t upper = static_cast<uint32_t>(seed >> 32);
    return static_cast<float>(upper) / static_cast<float>(UINT32_MAX);
}

// Utility: Convert seed to range [min, max]
int32_t WorldSeed::seedToRange(uint64_t seed, int32_t min, int32_t max) {
    if (min >= max) return min;
    
    uint32_t range = static_cast<uint32_t>(max - min + 1);
    uint32_t value = static_cast<uint32_t>(seed >> 32) % range;
    return min + static_cast<int32_t>(value);
}

// Debug info
WorldSeed::SeedInfo WorldSeed::getDebugInfo(int64_t x, int64_t y, int64_t z, ScaleLevel scale, FeatureType feature) const {
    SeedInfo info;
    info.baseSeed = masterSeed_;
    info.featureSeed = getFeatureDomainSeed(feature);
    info.finalSeed = getScaleSeed(x, y, z, scale, feature);
    info.featureName = getFeatureTypeName(feature);
    info.scaleName = getScaleLevelName(scale);
    return info;
}

// Core seed derivation with high-quality mixing
uint64_t WorldSeed::deriveSeed(uint64_t baseSeed, int64_t x, int64_t y, int64_t z) {
    // Use large prime numbers for good distribution
    // These primes are chosen to avoid patterns in common coordinate sequences
    uint64_t h = baseSeed;
    h ^= avalancheHash(static_cast<uint64_t>(x) * 73856093ULL);
    h ^= avalancheHash(static_cast<uint64_t>(y) * 19349663ULL);  
    h ^= avalancheHash(static_cast<uint64_t>(z) * 83492791ULL);
    
    return avalancheHash(h);
}

// High-quality string hash
uint64_t WorldSeed::hashString(const std::string& str) {
    // Use std::hash as base, then improve distribution
    std::hash<std::string> hasher;
    uint64_t h = hasher(str);
    
    // Apply additional mixing for better distribution
    return avalancheHash(h);
}

// Avalanche hash for excellent bit distribution
uint64_t WorldSeed::avalancheHash(uint64_t x) {
    // MurmurHash3 finalizer - provides excellent avalanche properties
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

} // namespace World
} // namespace VoxelCastle
