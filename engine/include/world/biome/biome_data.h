#pragma once

#include "biome_types.h"
#include "../voxel_types.h"

namespace VoxelCastle {
namespace World {

/**
 * @brief Complete data structure containing all parameters for a biome type
 * 
 * This structure defines all the characteristics that make each biome unique,
 * including climate parameters, terrain properties, and block assignments.
 */
struct BiomeData {
    BiomeType type;                ///< The biome type this data represents
    
    // Climate parameters
    float baseTemperature;         ///< Base temperature for this biome (-1.0 to 1.0)
    float baseHumidity;           ///< Base humidity for this biome (0.0 to 1.0)
    
    // Terrain generation parameters
    float terrainRoughness;       ///< How rough/varied the terrain should be (0.0 to 1.0)
    float terrainScale;           ///< Scale factor for terrain noise (0.1 to 5.0)
    int baseHeight;               ///< Base elevation for this biome (0 to 255)
    int heightVariation;          ///< Maximum variation from base height (0 to 100)
    
    // Block type assignments
    BlockType surfaceBlock;       ///< Primary surface block (grass, sand, etc.)
    BlockType subsurfaceBlock;    ///< Block beneath surface (dirt, sandstone, etc.)
    BlockType deepBlock;          ///< Deep underground block (stone, etc.)
    
    // Special feature blocks (up to 4 per biome initially)
    BlockType featureBlocks[4];   ///< Blocks for vegetation, decorations, etc.
    
    // Climate thresholds for biome selection
    float minTemperature;         ///< Minimum temperature for this biome
    float maxTemperature;         ///< Maximum temperature for this biome
    float minHumidity;            ///< Minimum humidity for this biome
    float maxHumidity;            ///< Maximum humidity for this biome
    
    /**
     * @brief Default constructor - initializes to invalid/empty state
     */
    BiomeData();
    
    /**
     * @brief Constructor with full parameters
     */
    BiomeData(BiomeType biomeType, 
              float temp, float humidity,
              float roughness, float scale,
              int height, int heightVar,
              BlockType surface, BlockType subsurface, BlockType deep,
              float minTemp, float maxTemp, float minHum, float maxHum);
    
    /**
     * @brief Check if this biome data is valid
     * @return true if all parameters are within valid ranges
     */
    bool isValid() const;
    
    /**
     * @brief Check if given temperature and humidity fall within this biome's range
     * @param temperature Temperature value to check
     * @param humidity Humidity value to check
     * @return true if the climate values match this biome
     */
    bool matchesClimate(float temperature, float humidity) const;
};

} // namespace World
} // namespace VoxelCastle
