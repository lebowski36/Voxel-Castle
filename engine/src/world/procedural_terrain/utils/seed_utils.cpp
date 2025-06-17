#include "world/procedural_terrain/utils/seed_utils.h"
#include <random>
#include <chrono>
#include <functional>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

uint64_t SeedUtils::GetComponentSeed(uint64_t baseSeed, const std::string& component) {
    // Create a deterministic hash by combining base seed with component name
    uint64_t componentHash = SimpleHash(component.c_str(), component.length());
    return baseSeed ^ componentHash;
}

uint64_t SeedUtils::GetChunkSeed(uint64_t baseSeed, int chunkX, int chunkZ) {
    // Combine chunk coordinates deterministically
    uint64_t combined = (static_cast<uint64_t>(chunkX) << 32) | (static_cast<uint64_t>(chunkZ) & 0xFFFFFFFF);
    return baseSeed ^ combined;
}

uint64_t SeedUtils::GetFeatureSeed(uint64_t baseSeed, int chunkX, int chunkZ, const std::string& feature) {
    uint64_t chunkSeed = GetChunkSeed(baseSeed, chunkX, chunkZ);
    uint64_t featureHash = SimpleHash(feature.c_str(), feature.length());
    return chunkSeed ^ featureHash;
}

uint64_t SeedUtils::StringToSeed(const std::string& seedString) {
    if (seedString.empty()) {
        return GenerateRandomSeed();
    }
    
    // Try to parse as number first
    try {
        return static_cast<uint64_t>(std::stoull(seedString));
    } catch (const std::exception&) {
        // If not a number, hash the string
        return SimpleHash(seedString.c_str(), seedString.length());
    }
}

uint64_t SeedUtils::GenerateRandomSeed() {
    // Use current time and random device for seed generation
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    
    std::random_device rd;
    uint64_t randomPart = rd();
    
    return static_cast<uint64_t>(timestamp) ^ (randomPart << 32);
}

uint64_t SeedUtils::SimpleHash(const void* input, size_t length) {
    // Simple FNV-1a hash for deterministic seed derivation
    const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = FNV_OFFSET_BASIS;
    const uint8_t* data = static_cast<const uint8_t*>(input);
    
    for (size_t i = 0; i < length; ++i) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }
    
    return hash;
}

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
