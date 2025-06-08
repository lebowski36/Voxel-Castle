#pragma once

#include <string>
#include "biome_types.h"
#include "world/voxel_types.h"

// Use VoxelType from VoxelEngine namespace
using VoxelEngine::World::VoxelType;

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
    std::string name;              ///< Human-readable name of the biome
    std::string description;       ///< Description of the biome
    
    // Climate parameters
    float baseTemperature;         ///< Base temperature for this biome (-1.0 to 1.0)
    float baseHumidity;           ///< Base humidity for this biome (0.0 to 1.0)
    
    // Terrain generation parameters
    float terrainRoughness;       ///< How rough/varied the terrain should be (0.0 to 1.0)
    float terrainScale;           ///< Scale factor for terrain noise (0.1 to 5.0)
    int baseHeight;               ///< Base elevation for this biome (0 to 255)
    int heightVariation;          ///< Maximum variation from base height (0 to 100)
    
    // Block type assignments
    VoxelType surfaceBlock;       ///< Primary surface block (grass, sand, etc.)
    VoxelType subsurfaceBlock;    ///< Block beneath surface (dirt, sandstone, etc.)
    VoxelType deepBlock;          ///< Deep underground block (stone, etc.)
    
    // Special feature blocks (up to 4 per biome initially)
    VoxelType featureBlocks[4];   ///< Blocks for vegetation, decorations, etc.
    
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
     * @brief Constructor with biome type, name, and description only
     */
    BiomeData(BiomeType biomeType, const std::string& biomeName, const std::string& biomeDescription);
    
    /**
     * @brief Constructor with full parameters
     */
    BiomeData(BiomeType biomeType, 
              float temp, float humidity,
              float roughness, float scale,
              int height, int heightVar,
              VoxelType surface, VoxelType subsurface, VoxelType deep,
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
    
    // Accessors
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    BiomeType getType() const { return type; }
    
    // Block assignment methods
    void setSurfaceBlock(VoxelType block) { surfaceBlock = block; }
    void setSubSurfaceBlock(VoxelType block) { subsurfaceBlock = block; }
    void setDeepBlock(VoxelType block) { deepBlock = block; }
    void setFillerBlock(VoxelType block) { 
        // Use the first feature block slot for filler block
        featureBlocks[0] = block; 
    }
    
    // Climate range setters
    void setTemperatureRange(float minTemp, float maxTemp) {
        minTemperature = minTemp;
        maxTemperature = maxTemp;
    }
    
    void setHumidityRange(float minHum, float maxHum) {
        minHumidity = minHum;
        maxHumidity = maxHum;
    }
    
    // Terrain parameter setters
    void setElevationRange(int minElev, int maxElev) {
        baseHeight = (minElev + maxElev) / 2;
        heightVariation = (maxElev - minElev) / 2;
    }
    
    void setSlope(float slopeValue) { terrainRoughness = slopeValue; }
    void setRoughness(float roughnessValue) { terrainScale = roughnessValue; }
    
    // Getters for biome properties
    float getMinTemperature() const { return minTemperature; }
    float getMaxTemperature() const { return maxTemperature; }
    float getMinHumidity() const { return minHumidity; }
    float getMaxHumidity() const { return maxHumidity; }
};

} // namespace World
} // namespace VoxelCastle
