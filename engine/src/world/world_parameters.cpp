#include "world/world_parameters.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

namespace VoxelCastle {
namespace World {

// Utility function implementations
const char* getWorldSizeName(WorldSize size) {
    switch (size) {
        case WorldSize::STARTER: return "STARTER";
        case WorldSize::REGIONAL: return "REGIONAL";
        case WorldSize::CONTINENTAL: return "CONTINENTAL";
        case WorldSize::MASSIVE: return "MASSIVE";
        default: return "UNKNOWN";
    }
}

const char* getHistoricalDepthName(HistoricalDepth depth) {
    switch (depth) {
        case HistoricalDepth::NONE: return "NONE";
        case HistoricalDepth::BASIC: return "BASIC";
        case HistoricalDepth::STANDARD: return "STANDARD";
        case HistoricalDepth::DETAILED: return "DETAILED";
        case HistoricalDepth::EPIC: return "EPIC";
        default: return "UNKNOWN";
    }
}

const char* getWorldPresetName(WorldPreset preset) {
    switch (preset) {
        case WorldPreset::NORMAL: return "NORMAL";
        case WorldPreset::FLAT: return "FLAT";
        case WorldPreset::AMPLIFIED: return "AMPLIFIED";
        case WorldPreset::ISLANDS: return "ISLANDS";
        case WorldPreset::CUSTOM: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

// WorldParameters implementation
WorldParameters::WorldParameters() {
    initializeDefaults();
}

WorldParameters::WorldParameters(WorldPreset preset, WorldSize size) {
    initializeDefaults();
    applyPreset(preset, size);
}

void WorldParameters::initializeDefaults() {
    // Terrain defaults (already set in struct definition)
    terrain.baseHeight = 64.0f;
    terrain.heightVariation = 24.0f;
    terrain.noiseScale = 0.01f;
    terrain.erosionStrength = 0.5f;
    terrain.waterLevel = 32.0f;
    terrain.noiseOctaves = 4;
    terrain.noisePersistence = 0.5f;
    terrain.noiseLacunarity = 2.0f;
    
    // Biome defaults
    biomes.temperatureScale = 0.001f;
    biomes.precipitationScale = 0.001f;
    biomes.biomeTransitionSize = 0.1f;
    biomes.altitudeEffect = 0.5f;
    biomes.enableSeasons = true;
    
    // Cave defaults (updated for ±256 chunk range)
    caves.caveFrequency = 0.03f;
    caves.tunnelWidth = 3.0f;
    caves.cavernFrequency = 0.01f;
    caves.cavernSize = 15.0f;
    caves.minCaveDepth = 32;        // Start caves 8m below surface (32 blocks)
    caves.maxCaveDepth = 3200;      // Extend caves to -800m depth (3200 blocks from surface)
    
    // Resource defaults (updated for ±256 chunk range)
    resources.oreAbundance = 1.0f;
    resources.coalFrequency = 0.05f;
    resources.ironFrequency = 0.03f;
    resources.goldFrequency = 0.01f;
    resources.diamondFrequency = 0.002f;
    resources.minOreDepth = 16;      // Start ores 4m below surface (16 blocks)
    resources.maxOreDepth = 3840;    // Extend ores to -960m depth (3840 blocks from surface)
    
    // Structure defaults
    structures.ruinFrequency = 0.001f;
    structures.dungeonFrequency = 0.0005f;
    structures.villageFrequency = 0.002f;
    structures.enableLandmarks = true;
    structures.enableRuins = true;
    
    // Historical defaults
    history.depth = HistoricalDepth::STANDARD;
    history.simulationYears = 1000;
    history.civilizationCount = 5;
    history.eventFrequency = 0.1f;
    history.enableParallelProcessing = true;
    
    std::cout << "[WorldParameters] Initialized default parameters" << std::endl;
}

void WorldParameters::applyPreset(WorldPreset preset, WorldSize size) {
    this->preset = preset;
    this->worldSize = size;
    
    switch (preset) {
        case WorldPreset::NORMAL:
            // Keep default values - balanced terrain
            break;
            
        case WorldPreset::FLAT:
            terrain.heightVariation = 4.0f;     // Very flat terrain
            terrain.noiseScale = 0.005f;        // Large, gentle features
            terrain.erosionStrength = 0.1f;     // Minimal erosion
            caves.caveFrequency = 0.01f;        // Fewer caves
            break;
            
        case WorldPreset::AMPLIFIED:
            terrain.heightVariation = 60.0f;    // Extreme height variation
            terrain.noiseScale = 0.02f;         // More detailed features
            terrain.erosionStrength = 0.8f;     // Strong erosion
            terrain.noiseOctaves = 6;           // More detail layers
            caves.caveFrequency = 0.05f;        // More caves
            structures.ruinFrequency = 0.002f;  // More ruins in dramatic terrain
            break;
            
        case WorldPreset::ISLANDS:
            terrain.baseHeight = 24.0f;         // Lower base height
            terrain.waterLevel = 48.0f;         // Higher water level
            terrain.heightVariation = 32.0f;    // Moderate variation
            terrain.noiseScale = 0.015f;        // Island-sized features
            biomes.temperatureScale = 0.002f;   // More climate variation
            break;
            
        case WorldPreset::CUSTOM:
            // Keep current values - user will modify
            break;
    }
    
    // Scale parameters based on world size
    scaleForWorldSize(size);
    
    std::cout << "[WorldParameters] Applied preset: " << getWorldPresetName(preset) 
              << " for world size: " << getWorldSizeName(size) << std::endl;
}

void WorldParameters::scaleForWorldSize(WorldSize size) {
    worldSize = size;
    
    switch (size) {
        case WorldSize::STARTER:
            // Smaller worlds can afford more detail
            terrain.noiseOctaves = std::min(terrain.noiseOctaves + 1, 6);
            structures.villageFrequency *= 1.5f;  // More dense features
            history.simulationYears = std::max(history.simulationYears / 2, 100);
            history.civilizationCount = std::max(history.civilizationCount / 2, 1);
            break;
            
        case WorldSize::REGIONAL:
            // Default scaling - no changes needed
            break;
            
        case WorldSize::CONTINENTAL:
            // Larger scale features for bigger worlds
            biomes.temperatureScale *= 0.5f;      // Larger climate zones
            biomes.precipitationScale *= 0.5f;
            structures.villageFrequency *= 0.7f;  // Less dense features
            history.simulationYears *= 2;         // More history for large worlds
            history.civilizationCount *= 2;
            break;
            
        case WorldSize::MASSIVE:
            // Continental-scale features
            biomes.temperatureScale *= 0.25f;     // Very large climate zones
            biomes.precipitationScale *= 0.25f;
            structures.villageFrequency *= 0.5f;  // Sparse features
            history.simulationYears *= 5;         // Deep history
            history.civilizationCount *= 3;
            history.enableParallelProcessing = true; // Required for massive worlds
            break;
    }
    
    std::cout << "[WorldParameters] Scaled parameters for world size: " << getWorldSizeName(size) << std::endl;
}

bool WorldParameters::validateParameters() {
    bool isValid = true;
    
    // Validate terrain parameters
    if (terrain.baseHeight < 1.0f || terrain.baseHeight > 200.0f) {
        std::cout << "[WorldParameters] Warning: baseHeight out of range, clamping" << std::endl;
        terrain.baseHeight = std::clamp(terrain.baseHeight, 1.0f, 200.0f);
        isValid = false;
    }
    
    if (terrain.heightVariation < 0.0f || terrain.heightVariation > 100.0f) {
        std::cout << "[WorldParameters] Warning: heightVariation out of range, clamping" << std::endl;
        terrain.heightVariation = std::clamp(terrain.heightVariation, 0.0f, 100.0f);
        isValid = false;
    }
    
    if (terrain.noiseScale < 0.001f || terrain.noiseScale > 0.1f) {
        std::cout << "[WorldParameters] Warning: noiseScale out of range, clamping" << std::endl;
        terrain.noiseScale = std::clamp(terrain.noiseScale, 0.001f, 0.1f);
        isValid = false;
    }
    
    // Validate cave parameters
    if (caves.caveFrequency < 0.0f || caves.caveFrequency > 0.2f) {
        std::cout << "[WorldParameters] Warning: caveFrequency out of range, clamping" << std::endl;
        caves.caveFrequency = std::clamp(caves.caveFrequency, 0.0f, 0.2f);
        isValid = false;
    }
    
    // Validate resource parameters
    if (resources.oreAbundance < 0.1f || resources.oreAbundance > 5.0f) {
        std::cout << "[WorldParameters] Warning: oreAbundance out of range, clamping" << std::endl;
        resources.oreAbundance = std::clamp(resources.oreAbundance, 0.1f, 5.0f);
        isValid = false;
    }
    
    // Validate historical parameters
    if (history.simulationYears < 0 || history.simulationYears > 50000) {
        std::cout << "[WorldParameters] Warning: simulationYears out of range, clamping" << std::endl;
        history.simulationYears = std::clamp(history.simulationYears, 0, 50000);
        isValid = false;
    }
    
    return isValid;
}

std::string WorldParameters::serialize() const {
    std::ostringstream oss;
    
    // Simple serialization format for now (could be JSON later)
    oss << "WorldParameters{\n";
    oss << "  worldSize=" << static_cast<int>(worldSize) << "\n";
    oss << "  preset=" << static_cast<int>(preset) << "\n";
    
    oss << "  terrain.baseHeight=" << terrain.baseHeight << "\n";
    oss << "  terrain.heightVariation=" << terrain.heightVariation << "\n";
    oss << "  terrain.noiseScale=" << terrain.noiseScale << "\n";
    oss << "  terrain.erosionStrength=" << terrain.erosionStrength << "\n";
    oss << "  terrain.waterLevel=" << terrain.waterLevel << "\n";
    oss << "  terrain.noiseOctaves=" << terrain.noiseOctaves << "\n";
    oss << "  terrain.noisePersistence=" << terrain.noisePersistence << "\n";
    oss << "  terrain.noiseLacunarity=" << terrain.noiseLacunarity << "\n";
    
    oss << "  caves.caveFrequency=" << caves.caveFrequency << "\n";
    oss << "  caves.tunnelWidth=" << caves.tunnelWidth << "\n";
    oss << "  caves.cavernFrequency=" << caves.cavernFrequency << "\n";
    oss << "  caves.cavernSize=" << caves.cavernSize << "\n";
    
    oss << "  resources.oreAbundance=" << resources.oreAbundance << "\n";
    oss << "  resources.coalFrequency=" << resources.coalFrequency << "\n";
    oss << "  resources.ironFrequency=" << resources.ironFrequency << "\n";
    oss << "  resources.goldFrequency=" << resources.goldFrequency << "\n";
    
    oss << "  history.depth=" << static_cast<int>(history.depth) << "\n";
    oss << "  history.simulationYears=" << history.simulationYears << "\n";
    oss << "  history.civilizationCount=" << history.civilizationCount << "\n";
    
    oss << "}";
    return oss.str();
}

bool WorldParameters::deserialize(const std::string& data) {
    // Simple deserialization for now
    // In a full implementation, this would parse JSON or a more robust format
    std::cout << "[WorldParameters] Deserializing parameters (simplified implementation)" << std::endl;
    return true;
}

void WorldParameters::setTerrainParameter(const std::string& name, float value) {
    if (name == "baseHeight") {
        terrain.baseHeight = value;
    } else if (name == "heightVariation") {
        terrain.heightVariation = value;
    } else if (name == "noiseScale") {
        terrain.noiseScale = value;
    } else if (name == "erosionStrength") {
        terrain.erosionStrength = value;
    } else if (name == "waterLevel") {
        terrain.waterLevel = value;
    } else if (name == "noisePersistence") {
        terrain.noisePersistence = value;
    } else if (name == "noiseLacunarity") {
        terrain.noiseLacunarity = value;
    } else {
        std::cout << "[WorldParameters] Warning: Unknown terrain parameter: " << name << std::endl;
    }
}

float WorldParameters::getTerrainParameter(const std::string& name) const {
    if (name == "baseHeight") {
        return terrain.baseHeight;
    } else if (name == "heightVariation") {
        return terrain.heightVariation;
    } else if (name == "noiseScale") {
        return terrain.noiseScale;
    } else if (name == "erosionStrength") {
        return terrain.erosionStrength;
    } else if (name == "waterLevel") {
        return terrain.waterLevel;
    } else if (name == "noisePersistence") {
        return terrain.noisePersistence;
    } else if (name == "noiseLacunarity") {
        return terrain.noiseLacunarity;
    } else {
        std::cout << "[WorldParameters] Warning: Unknown terrain parameter: " << name << std::endl;
        return 0.0f;
    }
}

std::vector<std::string> WorldParameters::getAvailablePresets() {
    return {"Normal", "Flat", "Amplified", "Islands", "Custom"};
}

std::string WorldParameters::getPresetDescription(WorldPreset preset) {
    switch (preset) {
        case WorldPreset::NORMAL:
            return "Balanced terrain with moderate variation - good for general gameplay";
        case WorldPreset::FLAT:
            return "Minimal height variation - ideal for building and construction";
        case WorldPreset::AMPLIFIED:
            return "Exaggerated terrain features with high mountains - challenging and dramatic";
        case WorldPreset::ISLANDS:
            return "Archipelago-style terrain with water between landmasses - exploration focused";
        case WorldPreset::CUSTOM:
            return "User-defined parameter set - fully customizable";
        default:
            return "Unknown preset";
    }
}

std::string WorldParameters::getWorldSizeDescription(WorldSize size) {
    switch (size) {
        case WorldSize::STARTER:
            return "10km² - 100km² - Quick generation, perfect for learning";
        case WorldSize::REGIONAL:
            return "1,000km² - 10,000km² - Standard gameplay area with good variety";
        case WorldSize::CONTINENTAL:
            return "100,000km² - 1,000,000km² - Epic campaigns with vast exploration";
        case WorldSize::MASSIVE:
            return "5,000,000km² - 25,000,000km² - Quarter Earth-size (requires optimization)";
        default:
            return "Unknown size";
    }
}

} // namespace World
} // namespace VoxelCastle
