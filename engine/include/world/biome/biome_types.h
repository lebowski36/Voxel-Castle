#pragma once

#include <string>

namespace VoxelCastle {
namespace World {

/**
 * @brief Enumeration of all biome types available in the world generation system
 * 
 * This enum defines the core biome types that can be generated in the world.
 * Each biome has unique characteristics for block placement, climate, and features.
 * 
 * Initial implementation includes 5 core biomes with room for future expansion.
 */
enum class BiomeType {
    PLAINS,      ///< Grassland with occasional trees and flowers
    FOREST,      ///< Dense tree coverage with varied heights
    DESERT,      ///< Sandy terrain with cacti and arid features
    MOUNTAINS,   ///< High elevation with stone outcrops and peaks
    OCEAN,       ///< Water bodies with underwater features and depth zones
    
    // Reserved for future biomes (commented out initially)
    // TUNDRA,      ///< Cold, frozen terrain with sparse vegetation
    // JUNGLE,      ///< Dense tropical vegetation with high humidity
    // SAVANNA,     ///< Grassland with scattered trees, transition biome
    // WETLANDS,    ///< Swampy areas with water and marsh plants
    // VOLCANIC,    ///< Active volcanic regions with lava and ash
    
    COUNT        ///< Keep this last for iteration/validation
};

/**
 * @brief Convert BiomeType to human-readable string
 * @param type The biome type to convert
 * @return String representation of the biome type
 */
const char* BiomeTypeToString(BiomeType type);

/**
 * @brief Convert string to BiomeType
 * @param str String representation of biome type
 * @return BiomeType enum value, or BiomeType::COUNT if invalid
 */
BiomeType StringToBiomeType(const std::string& str);

/**
 * @brief Check if a BiomeType value is valid
 * @param type The biome type to validate
 * @return true if the biome type is valid, false otherwise
 */
bool IsValidBiomeType(BiomeType type);

} // namespace World
} // namespace VoxelCastle
