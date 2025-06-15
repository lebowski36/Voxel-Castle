#pragma once

#include "world/geological_data.h"  // Use existing RockType enum
#include <cstdint>

namespace VoxelCastle {
namespace World {

// Use existing RockType from geological_data.h
// No need to redefine it here

/**
 * @brief Geological accuracy presets
 */
enum class GeologicalPreset {
    PERFORMANCE,      // Fast generation, basic visual quality
    BALANCED,         // Good realism with reasonable performance (DEFAULT)
    QUALITY,          // High realism, slower generation
    ULTRA_REALISM,    // Maximum accuracy, very slow
    CUSTOM           // User-defined settings
};

/**
 * @brief Geological phase during simulation
 */
enum class GeologicalPhase {
    TECTONICS = 0,        // Continental drift and plate movement  
    MOUNTAIN_BUILDING = 1, // Mountain formation and uplift
    EROSION = 2,          // Water and weather erosion
    DETAIL = 3            // Micro-scale geological features
};

/**
 * @brief Custom geological settings (for CUSTOM preset)
 */
struct CustomGeologicalSettings {
    bool enableChemicalWeathering = true;
    bool enableJointSystems = true;
    bool enableCaveSystems = true;
    bool enableGlacialFeatures = false;
    float timeScaleMultiplier = 1.0f;
    int simulationDetailLevel = 1000;
    
    // Continental and Ocean Parameters (from World Configuration UI)
    int numContinents = 4;           // Number of continents (3-7)
    float maxContinentSize = 8.0f;   // Maximum continent size as % of world (6-12%)
    float minOceanCoverage = 65.0f;  // Minimum ocean coverage as % of world (60-80%)
    bool forceOceanGeneration = true; // Ensure oceans are always visible
};

/**
 * @brief Complete geological configuration
 */
struct GeologicalConfig {
    GeologicalPreset preset = GeologicalPreset::BALANCED;
    CustomGeologicalSettings custom;
    
    // Performance characteristics based on preset
    int getChunkGenerationTimeMs() const {
        switch (preset) {
            case GeologicalPreset::PERFORMANCE: return 5;
            case GeologicalPreset::BALANCED: return 15;
            case GeologicalPreset::QUALITY: return 50;
            case GeologicalPreset::ULTRA_REALISM: return 200;
            case GeologicalPreset::CUSTOM: return custom.simulationDetailLevel / 5;
        }
        return 15;
    }
    
    int getSimulationSteps() const {
        switch (preset) {
            case GeologicalPreset::PERFORMANCE: return 100;
            case GeologicalPreset::BALANCED: return 1000;
            case GeologicalPreset::QUALITY: return 5000;
            case GeologicalPreset::ULTRA_REALISM: return 10000;
            case GeologicalPreset::CUSTOM: return custom.simulationDetailLevel;
        }
        return 1000;
    }
};

/**
 * @brief Geological sample data at a specific location
 */
struct GeologicalSample {
    float elevation = 0.0f;           // Terrain height in meters
    RockType rockType = RockType::IGNEOUS_GRANITE; // Use existing RockType values
    float rockHardness = 1.0f;        // Resistance to erosion (0-10)
    float waterFlow = 0.0f;           // Water flow intensity (0-10)
    float stressField = 0.0f;         // Tectonic stress (-10 to 10)
    float temperature = 15.0f;        // Climate temperature (Celsius)
    float rainfall = 500.0f;          // Annual rainfall (mm)
    
    // Micro-geological features
    bool hasJointSystems = false;
    bool hasCaveSystems = false;
    bool hasQuartzVeins = false;
    float weatheringIntensity = 1.0f;
    
    // Step 3: Water & Cave System Framework - New water system features
    bool hasWetlands = false;         // Wetland areas with shallow groundwater
    bool hasNaturalSprings = false;   // Areas with spring discharge
    bool hasLakes = false;            // Areas with standing surface water
    bool hasRivers = false;           // Areas with flowing surface water
};

/**
 * @brief Performance metrics for geological simulation
 */
struct PerformanceMetrics {
    float memoryUsage = 0.0f;         // Current memory usage (GB)
    float totalMemory = 8.0f;         // Total available memory (GB)
    float processingRate = 0.0f;      // Samples processed per second
    float chunkGenerationRate = 0.0f; // Chunks generated per second
    float averageChunkTime = 0.0f;    // Average time per chunk (ms)
};

/**
 * @brief Progress information during geological simulation
 */
struct PhaseInfo {
    GeologicalPhase currentPhase = GeologicalPhase::TECTONICS;
    float phaseProgress = 0.0f;       // Progress within current phase (0-1)
    float totalProgress = 0.0f;       // Overall simulation progress (0-1)
    std::string currentProcess = "";  // Description of current process
    float timeRemaining = 0.0f;       // Estimated seconds remaining
    PerformanceMetrics metrics;
};

/**
 * @brief Quality preview information for UI
 */
struct QualityPreview {
    GeologicalPreset preset;
    std::string name;
    std::string description;
    std::string expectedTime;
    std::string memoryUsage;
    std::string visualFeatures;
};

// Helper functions
inline const char* rockTypeToString(RockType type) {
    switch (type) {
        case RockType::SEDIMENTARY_LIMESTONE: return "Limestone";
        case RockType::SEDIMENTARY_SANDSTONE: return "Sandstone";
        case RockType::SEDIMENTARY_SHALE: return "Shale";
        case RockType::IGNEOUS_GRANITE: return "Granite";
        case RockType::IGNEOUS_BASALT: return "Basalt";
        case RockType::IGNEOUS_OBSIDIAN: return "Obsidian";
        case RockType::METAMORPHIC_MARBLE: return "Marble";
        case RockType::METAMORPHIC_SLATE: return "Slate";
        case RockType::METAMORPHIC_QUARTZITE: return "Quartzite";
        case RockType::SOIL_CLAY: return "Clay";
        case RockType::SOIL_SAND: return "Sand";
        case RockType::SOIL_LOAM: return "Loam";
        default: return "Unknown";
    }
}

inline float getRockHardness(RockType type) {
    switch (type) {
        case RockType::SEDIMENTARY_LIMESTONE: return 3.0f;
        case RockType::SEDIMENTARY_SANDSTONE: return 4.0f;
        case RockType::SEDIMENTARY_SHALE: return 2.0f;
        case RockType::IGNEOUS_GRANITE: return 8.0f;
        case RockType::IGNEOUS_BASALT: return 6.0f;
        case RockType::IGNEOUS_OBSIDIAN: return 5.0f;
        case RockType::METAMORPHIC_MARBLE: return 3.5f;
        case RockType::METAMORPHIC_SLATE: return 4.5f;
        case RockType::METAMORPHIC_QUARTZITE: return 9.0f;
        case RockType::SOIL_CLAY: return 1.0f;
        case RockType::SOIL_SAND: return 0.5f;
        case RockType::SOIL_LOAM: return 1.5f;
        default: return 5.0f;
    }
}

inline QualityPreview getQualityPreview(GeologicalPreset preset) {
    switch (preset) {
        case GeologicalPreset::PERFORMANCE:
            return {preset, "Performance", "Fast generation, good visuals", "1-2 minutes", "1-2GB", "Basic erosion, simplified geology"};
        case GeologicalPreset::BALANCED:
            return {preset, "Balanced", "Recommended for most users", "3-5 minutes", "2-3GB", "Realistic erosion, detailed geology"};
        case GeologicalPreset::QUALITY:
            return {preset, "Quality", "High detail, longer generation", "8-12 minutes", "4-6GB", "Advanced weathering, complex cave systems"};
        case GeologicalPreset::ULTRA_REALISM:
            return {preset, "Ultra Realism", "Maximum realism, very slow", "20-30 minutes", "6-8GB", "Research-grade geological accuracy"};
        case GeologicalPreset::CUSTOM:
            return {preset, "Custom", "User-defined settings", "Variable", "Variable", "Configurable features"};
        default:
            return {preset, "Unknown", "", "", "", ""};
    }
}

} // namespace World
} // namespace VoxelCastle
