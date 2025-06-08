#include "world/biome/biome_registry.h"
#include "world/voxel_types.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

// Use VoxelType from VoxelEngine namespace
using VoxelEngine::World::VoxelType;

namespace VoxelCastle {
namespace World {

// Static member definitions
std::vector<BiomeData> BiomeRegistry::biomeRegistry_;
bool BiomeRegistry::initialized_ = false;

void BiomeRegistry::initialize() {
    if (initialized_) {
        return; // Already initialized
    }
    
    // Clear any existing data
    biomeRegistry_.clear();
    
    // Create and register all default biomes
    biomeRegistry_.reserve(5); // We have 5 biomes currently
    
    BiomeData plains = createPlainsData();
    validateBiomeData(plains);
    biomeRegistry_.push_back(std::move(plains));
    
    BiomeData forest = createForestData();
    validateBiomeData(forest);
    biomeRegistry_.push_back(std::move(forest));
    
    BiomeData desert = createDesertData();
    validateBiomeData(desert);
    biomeRegistry_.push_back(std::move(desert));
    
    BiomeData mountains = createMountainsData();
    validateBiomeData(mountains);
    biomeRegistry_.push_back(std::move(mountains));
    
    BiomeData ocean = createOceanData();
    validateBiomeData(ocean);
    biomeRegistry_.push_back(std::move(ocean));
    
    initialized_ = true;
}

bool BiomeRegistry::isInitialized() {
    return initialized_;
}

const BiomeData& BiomeRegistry::getBiomeData(BiomeType type) {
    if (!initialized_) {
        throw std::runtime_error("BiomeRegistry not initialized. Call initialize() first.");
    }
    
    if (!isValidBiomeType(type)) {
        throw std::runtime_error("Invalid biome type provided to getBiomeData");
    }
    
    // Find the biome data with matching type
    auto it = std::find_if(biomeRegistry_.begin(), biomeRegistry_.end(),
        [type](const BiomeData& data) {
            return data.getType() == type;
        });
    
    if (it == biomeRegistry_.end()) {
        throw std::runtime_error("Biome data not found for type: " + biomeTypeToString(type));
    }
    
    return *it;
}

BiomeType BiomeRegistry::getBiomeFromTemperatureHumidity(float temperature, float humidity) {
    if (!initialized_) {
        throw std::runtime_error("BiomeRegistry not initialized. Call initialize() first.");
    }
    
    BiomeType bestMatch = BiomeType::PLAINS; // Default fallback
    float bestScore = -1.0f;
    
    // Find the biome with the best climate match
    for (const auto& biome : biomeRegistry_) {
        float score = calculateClimateMatch(biome, temperature, humidity);
        if (score > bestScore) {
            bestScore = score;
            bestMatch = biome.getType();
        }
    }
    
    return bestMatch;
}

const std::vector<BiomeData>& BiomeRegistry::getAllBiomes() {
    if (!initialized_) {
        throw std::runtime_error("BiomeRegistry not initialized. Call initialize() first.");
    }
    
    return biomeRegistry_;
}

size_t BiomeRegistry::getBiomeCount() {
    if (!initialized_) {
        return 0;
    }
    
    return biomeRegistry_.size();
}

void BiomeRegistry::cleanup() {
    biomeRegistry_.clear();
    initialized_ = false;
}

// Private biome creation methods

BiomeData BiomeRegistry::createPlainsData() {
    using namespace VoxelEngine::World;
    
    BiomeData plains(BiomeType::PLAINS, "Plains", "Open grasslands with rolling hills");
    
    // Climate parameters
    plains.setTemperatureRange(0.1f, 0.6f);   // Mild to warm temperatures
    plains.setHumidityRange(0.3f, 0.8f);      // Moderate to high humidity
    
    // Terrain parameters
    plains.setElevationRange(60, 80);          // Sea level to gentle hills
    plains.setSlope(0.2f);                     // Gentle slopes
    plains.setRoughness(0.1f);                 // Smooth terrain
    
    // Primary block types for plains
    plains.setSurfaceBlock(VoxelType::GRASS);
    plains.setSubSurfaceBlock(VoxelType::DIRT);
    plains.setDeepBlock(VoxelType::STONE);
    plains.setFillerBlock(VoxelType::TOPSOIL);
    
    return plains;
}

BiomeData BiomeRegistry::createForestData() {
    using namespace VoxelEngine::World;
    
    BiomeData forest(BiomeType::FOREST, "Forest", "Dense woodlands with diverse tree species");
    
    // Climate parameters
    forest.setTemperatureRange(-0.2f, 0.5f);  // Cool to mild temperatures
    forest.setHumidityRange(0.5f, 1.0f);      // High humidity
    
    // Terrain parameters
    forest.setElevationRange(70, 120);         // Slightly elevated
    forest.setSlope(0.4f);                     // Moderate slopes
    forest.setRoughness(0.3f);                 // Somewhat rough terrain
    
    // Primary block types for forest
    forest.setSurfaceBlock(VoxelType::GRASS);
    forest.setSubSurfaceBlock(VoxelType::DIRT);
    forest.setDeepBlock(VoxelType::STONE);
    forest.setFillerBlock(VoxelType::TOPSOIL);
    
    return forest;
}

BiomeData BiomeRegistry::createDesertData() {
    using namespace VoxelEngine::World;
    
    BiomeData desert(BiomeType::DESERT, "Desert", "Arid wasteland with sand dunes and sparse vegetation");
    
    // Climate parameters
    desert.setTemperatureRange(0.4f, 1.0f);   // Hot temperatures
    desert.setHumidityRange(0.0f, 0.3f);      // Very low humidity
    
    // Terrain parameters
    desert.setElevationRange(55, 90);          // Variable elevation
    desert.setSlope(0.3f);                     // Moderate slopes (dunes)
    desert.setRoughness(0.2f);                 // Somewhat smooth (sand)
    
    // Primary block types for desert
    desert.setSurfaceBlock(VoxelType::SAND);
    desert.setSubSurfaceBlock(VoxelType::SAND);
    desert.setDeepBlock(VoxelType::SANDSTONE);
    desert.setFillerBlock(VoxelType::GRAVEL);
    
    return desert;
}

BiomeData BiomeRegistry::createMountainsData() {
    using namespace VoxelEngine::World;
    
    BiomeData mountains(BiomeType::MOUNTAINS, "Mountains", "High elevation rocky terrain with steep slopes");
    
    // Climate parameters
    mountains.setTemperatureRange(-0.5f, 0.2f); // Cold to cool temperatures
    mountains.setHumidityRange(0.2f, 0.7f);     // Variable humidity
    
    // Terrain parameters
    mountains.setElevationRange(120, 200);       // High elevation
    mountains.setSlope(0.8f);                    // Steep slopes
    mountains.setRoughness(0.7f);                // Very rough terrain
    
    // Primary block types for mountains
    mountains.setSurfaceBlock(VoxelType::STONE);
    mountains.setSubSurfaceBlock(VoxelType::STONE);
    mountains.setDeepBlock(VoxelType::STONE);
    mountains.setFillerBlock(VoxelType::GRAVEL);
    
    return mountains;
}

BiomeData BiomeRegistry::createOceanData() {
    using namespace VoxelEngine::World;
    
    BiomeData ocean(BiomeType::OCEAN, "Ocean", "Deep water bodies with marine life");
    
    // Climate parameters
    ocean.setTemperatureRange(-0.1f, 0.4f);   // Cool to mild water temperatures
    ocean.setHumidityRange(0.8f, 1.0f);       // Very high humidity
    
    // Terrain parameters
    ocean.setElevationRange(20, 50);           // Below sea level
    ocean.setSlope(0.1f);                      // Gentle underwater slopes
    ocean.setRoughness(0.1f);                  // Smooth ocean floor
    
    // Primary block types for ocean
    ocean.setSurfaceBlock(VoxelType::WATER);
    ocean.setSubSurfaceBlock(VoxelType::SAND);
    ocean.setDeepBlock(VoxelType::STONE);
    ocean.setFillerBlock(VoxelType::GRAVEL);
    
    return ocean;
}

// Helper methods

void BiomeRegistry::validateBiomeData(const BiomeData& data) {
    if (!data.isValid()) {
        throw std::runtime_error("Invalid biome data for biome: " + data.getName());
    }
    
    // Additional validation could be added here
    // For example, checking that temperature and humidity ranges are within valid bounds
    if (data.getMinTemperature() < -1.0f || data.getMaxTemperature() > 1.0f) {
        throw std::runtime_error("Temperature range out of bounds for biome: " + data.getName());
    }
    
    if (data.getMinHumidity() < 0.0f || data.getMaxHumidity() > 1.0f) {
        throw std::runtime_error("Humidity range out of bounds for biome: " + data.getName());
    }
}

float BiomeRegistry::calculateClimateMatch(const BiomeData& biome, float temperature, float humidity) {
    // Calculate how well the given climate conditions match this biome
    // Returns a score from 0.0 (no match) to 1.0 (perfect match)
    
    float tempScore = 0.0f;
    float humidityScore = 0.0f;
    
    // Temperature scoring
    if (temperature >= biome.getMinTemperature() && temperature <= biome.getMaxTemperature()) {
        // Within range - calculate how close to optimal
        float tempRange = biome.getMaxTemperature() - biome.getMinTemperature();
        float tempCenter = (biome.getMinTemperature() + biome.getMaxTemperature()) * 0.5f;
        float distFromCenter = std::abs(temperature - tempCenter);
        tempScore = 1.0f - (distFromCenter / (tempRange * 0.5f));
    } else {
        // Outside range - calculate penalty based on distance
        float distanceFromRange = 0.0f;
        if (temperature < biome.getMinTemperature()) {
            distanceFromRange = biome.getMinTemperature() - temperature;
        } else {
            distanceFromRange = temperature - biome.getMaxTemperature();
        }
        tempScore = std::max(0.0f, 1.0f - distanceFromRange);
    }
    
    // Humidity scoring (similar to temperature)
    if (humidity >= biome.getMinHumidity() && humidity <= biome.getMaxHumidity()) {
        float humidityRange = biome.getMaxHumidity() - biome.getMinHumidity();
        float humidityCenter = (biome.getMinHumidity() + biome.getMaxHumidity()) * 0.5f;
        float distFromCenter = std::abs(humidity - humidityCenter);
        humidityScore = 1.0f - (distFromCenter / (humidityRange * 0.5f));
    } else {
        float distanceFromRange = 0.0f;
        if (humidity < biome.getMinHumidity()) {
            distanceFromRange = biome.getMinHumidity() - humidity;
        } else {
            distanceFromRange = humidity - biome.getMaxHumidity();
        }
        humidityScore = std::max(0.0f, 1.0f - distanceFromRange);
    }
    
    // Combine scores (weighted equally for now)
    return (tempScore + humidityScore) * 0.5f;
}

} // namespace World
} // namespace VoxelCastle
