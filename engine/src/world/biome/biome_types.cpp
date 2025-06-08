#include "world/biome/biome_types.h"
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace VoxelCastle {
namespace World {

const char* BiomeTypeToString(BiomeType type) {
    switch (type) {
        case BiomeType::PLAINS:     return "Plains";
        case BiomeType::FOREST:     return "Forest";
        case BiomeType::DESERT:     return "Desert";
        case BiomeType::MOUNTAINS:  return "Mountains";
        case BiomeType::OCEAN:      return "Ocean";
        case BiomeType::COUNT:      return "Invalid";
        default:                    return "Unknown";
    }
}

BiomeType StringToBiomeType(const std::string& str) {
    // Create uppercase version for case-insensitive comparison
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    
    // Static map for efficient lookup
    static const std::unordered_map<std::string, BiomeType> stringToBiome = {
        {"PLAINS", BiomeType::PLAINS},
        {"FOREST", BiomeType::FOREST},
        {"DESERT", BiomeType::DESERT},
        {"MOUNTAINS", BiomeType::MOUNTAINS},
        {"MOUNTAIN", BiomeType::MOUNTAINS},  // Allow singular form
        {"OCEAN", BiomeType::OCEAN}
    };
    
    auto it = stringToBiome.find(upperStr);
    return (it != stringToBiome.end()) ? it->second : BiomeType::COUNT;
}

bool IsValidBiomeType(BiomeType type) {
    return type >= BiomeType::PLAINS && type < BiomeType::COUNT;
}

} // namespace World
} // namespace VoxelCastle
