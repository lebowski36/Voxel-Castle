#include "world/biome/biome_data.h"
#include <algorithm>

// Use VoxelType from VoxelEngine namespace
using VoxelEngine::World::VoxelType;

namespace VoxelCastle {
namespace World {

BiomeData::BiomeData() 
    : type(BiomeType::COUNT)
    , name("")
    , description("")
    , baseTemperature(0.0f)
    , baseHumidity(0.0f)
    , terrainRoughness(0.5f)
    , terrainScale(1.0f)
    , baseHeight(64)
    , heightVariation(32)
    , surfaceBlock(VoxelType::AIR)
    , subsurfaceBlock(VoxelType::AIR)
    , deepBlock(VoxelType::AIR)
    , featureBlocks{VoxelType::AIR, VoxelType::AIR, VoxelType::AIR, VoxelType::AIR}
    , minTemperature(-1.0f)
    , maxTemperature(1.0f)
    , minHumidity(0.0f)
    , maxHumidity(1.0f)
{
}

BiomeData::BiomeData(BiomeType biomeType, 
                     float temp, float humidity,
                     float roughness, float scale,
                     int height, int heightVar,
                     VoxelType surface, VoxelType subsurface, VoxelType deep,
                     float minTemp, float maxTemp, float minHum, float maxHum)
    : type(biomeType)
    , name("") // Default empty name for full constructor
    , description("") // Default empty description for full constructor
    , baseTemperature(temp)
    , baseHumidity(humidity)
    , terrainRoughness(roughness)
    , terrainScale(scale)
    , baseHeight(height)
    , heightVariation(heightVar)
    , surfaceBlock(surface)
    , subsurfaceBlock(subsurface)
    , deepBlock(deep)
    , featureBlocks{VoxelType::AIR, VoxelType::AIR, VoxelType::AIR, VoxelType::AIR}
    , minTemperature(minTemp)
    , maxTemperature(maxTemp)
    , minHumidity(minHum)
    , maxHumidity(maxHum)
{
}

BiomeData::BiomeData(BiomeType biomeType, const std::string& biomeName, const std::string& biomeDescription)
    : type(biomeType)
    , name(biomeName)
    , description(biomeDescription)
    , baseTemperature(0.0f)
    , baseHumidity(0.0f)
    , terrainRoughness(0.5f)
    , terrainScale(1.0f)
    , baseHeight(64)
    , heightVariation(32)
    , surfaceBlock(VoxelType::AIR)
    , subsurfaceBlock(VoxelType::AIR)
    , deepBlock(VoxelType::AIR)
    , featureBlocks{VoxelType::AIR, VoxelType::AIR, VoxelType::AIR, VoxelType::AIR}
    , minTemperature(-1.0f)
    , maxTemperature(1.0f)
    , minHumidity(0.0f)
    , maxHumidity(1.0f)
{
}

bool BiomeData::isValid() const {
    // Check biome type is valid
    if (!IsValidBiomeType(type)) {
        return false;
    }
    
    // Check temperature ranges
    if (baseTemperature < -1.0f || baseTemperature > 1.0f ||
        minTemperature < -1.0f || minTemperature > 1.0f ||
        maxTemperature < -1.0f || maxTemperature > 1.0f ||
        minTemperature > maxTemperature) {
        return false;
    }
    
    // Check humidity ranges
    if (baseHumidity < 0.0f || baseHumidity > 1.0f ||
        minHumidity < 0.0f || minHumidity > 1.0f ||
        maxHumidity < 0.0f || maxHumidity > 1.0f ||
        minHumidity > maxHumidity) {
        return false;
    }
    
    // Check terrain parameters
    if (terrainRoughness < 0.0f || terrainRoughness > 1.0f ||
        terrainScale < 0.1f || terrainScale > 5.0f ||
        baseHeight < 0 || baseHeight > 255 ||
        heightVariation < 0 || heightVariation > 100) {
        return false;
    }
    
    // Check that required blocks are not AIR
    if (surfaceBlock == VoxelType::AIR ||
        subsurfaceBlock == VoxelType::AIR ||
        deepBlock == VoxelType::AIR) {
        return false;
    }
    
    return true;
}

bool BiomeData::matchesClimate(float temperature, float humidity) const {
    return temperature >= minTemperature && temperature <= maxTemperature &&
           humidity >= minHumidity && humidity <= maxHumidity;
}

} // namespace World
} // namespace VoxelCastle
