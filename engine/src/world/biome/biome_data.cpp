#include "../../include/world/biome/biome_data.h"
#include <algorithm>

namespace VoxelCastle {
namespace World {

BiomeData::BiomeData() 
    : type(BiomeType::COUNT)
    , baseTemperature(0.0f)
    , baseHumidity(0.0f)
    , terrainRoughness(0.5f)
    , terrainScale(1.0f)
    , baseHeight(64)
    , heightVariation(32)
    , surfaceBlock(BlockType::AIR)
    , subsurfaceBlock(BlockType::AIR)
    , deepBlock(BlockType::AIR)
    , featureBlocks{BlockType::AIR, BlockType::AIR, BlockType::AIR, BlockType::AIR}
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
                     BlockType surface, BlockType subsurface, BlockType deep,
                     float minTemp, float maxTemp, float minHum, float maxHum)
    : type(biomeType)
    , baseTemperature(temp)
    , baseHumidity(humidity)
    , terrainRoughness(roughness)
    , terrainScale(scale)
    , baseHeight(height)
    , heightVariation(heightVar)
    , surfaceBlock(surface)
    , subsurfaceBlock(subsurface)
    , deepBlock(deep)
    , featureBlocks{BlockType::AIR, BlockType::AIR, BlockType::AIR, BlockType::AIR}
    , minTemperature(minTemp)
    , maxTemperature(maxTemp)
    , minHumidity(minHum)
    , maxHumidity(maxHum)
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
    if (surfaceBlock == BlockType::AIR || 
        subsurfaceBlock == BlockType::AIR || 
        deepBlock == BlockType::AIR) {
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
