#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace VoxelCastle {
namespace World {

/**
 * @brief Scale levels for world generation parameters
 */
enum class WorldSize : uint32_t {
    STARTER = 0,        // 10km² - 100km²
    REGIONAL = 1,       // 1,000km² - 10,000km²
    CONTINENTAL = 2,    // 100,000km² - 1,000,000km²
    MASSIVE = 3         // 5,000,000km² - 25,000,000km² (quarter Earth-size)
};

/**
 * @brief Historical simulation depth options
 */
enum class HistoricalDepth : uint32_t {
    NONE = 0,       // Pure geological/biome generation only
    BASIC = 1,      // Major geological events, basic climate history
    STANDARD = 2,   // Add civilization placement, basic territorial history
    DETAILED = 3,   // Full historical simulation with events, wars, cultural development
    EPIC = 4        // Deep simulation with linguistic evolution, technological progression
};

/**
 * @brief World generation preset types
 */
enum class WorldPreset : uint32_t {
    NORMAL = 0,     // Balanced terrain with moderate variation
    FLAT = 1,       // Minimal height variation, good for building
    AMPLIFIED = 2,  // Exaggerated terrain features, high mountains
    ISLANDS = 3,    // Archipelago-style terrain with water between landmasses
    CUSTOM = 4      // User-defined parameter set
};

/**
 * @brief Terrain generation parameters
 */
struct TerrainParameters {
    float baseHeight = 64.0f;           // Base terrain height
    float heightVariation = 24.0f;      // Maximum height variation from base
    float noiseScale = 0.01f;           // Horizontal frequency of terrain noise
    float erosionStrength = 0.5f;       // Erosion simulation intensity
    float waterLevel = 32.0f;           // Sea level
    
    // Multi-octave noise parameters
    int noiseOctaves = 4;               // Number of noise octaves
    float noisePersistence = 0.5f;      // Amplitude falloff between octaves
    float noiseLacunarity = 2.0f;       // Frequency scaling between octaves
};

/**
 * @brief Biome generation parameters
 */
struct BiomeParameters {
    float temperatureScale = 0.001f;    // Continental temperature gradient scale
    float precipitationScale = 0.001f;  // Continental precipitation gradient scale
    float biomeTransitionSize = 0.1f;   // Size of transition zones between biomes
    float altitudeEffect = 0.5f;        // How much altitude affects biome selection
    bool enableSeasons = true;          // Enable seasonal biome changes
};

/**
 * @brief Cave generation parameters
 */
struct CaveParameters {
    float caveFrequency = 0.03f;        // Overall cave density
    float tunnelWidth = 3.0f;           // Average tunnel width
    float cavernFrequency = 0.01f;      // Large cavern frequency
    float cavernSize = 15.0f;           // Average cavern size
    int minCaveDepth = 8;               // Minimum depth for caves
    int maxCaveDepth = 120;             // Maximum depth for caves
};

/**
 * @brief Resource generation parameters
 */
struct ResourceParameters {
    float oreAbundance = 1.0f;          // Overall ore frequency multiplier
    float coalFrequency = 0.05f;        // Coal vein frequency
    float ironFrequency = 0.03f;        // Iron ore frequency
    float goldFrequency = 0.01f;        // Gold ore frequency
    float diamondFrequency = 0.002f;    // Diamond frequency
    int minOreDepth = 5;                // Minimum depth for ore generation
    int maxOreDepth = 100;              // Maximum depth for ore generation
};

/**
 * @brief Structure generation parameters
 */
struct StructureParameters {
    float ruinFrequency = 0.001f;       // Ancient ruin frequency
    float dungeonFrequency = 0.0005f;   // Dungeon frequency
    float villageFrequency = 0.002f;    // Village frequency
    bool enableLandmarks = true;        // Enable natural landmarks
    bool enableRuins = true;            // Enable ancient ruins
};

/**
 * @brief Historical simulation parameters
 */
struct HistoricalParameters {
    HistoricalDepth depth = HistoricalDepth::STANDARD;
    int simulationYears = 1000;         // Years of history to simulate
    int civilizationCount = 5;          // Number of civilizations
    float eventFrequency = 0.1f;        // Historical event frequency per year
    bool enableParallelProcessing = true; // Use multiple cores for large worlds
};

/**
 * @brief Complete world generation parameters
 */
class WorldParameters {
public:
    WorldParameters();
    explicit WorldParameters(WorldPreset preset, WorldSize size = WorldSize::REGIONAL);
    
    // Parameter categories
    TerrainParameters terrain;
    BiomeParameters biomes;
    CaveParameters caves;
    ResourceParameters resources;
    StructureParameters structures;
    HistoricalParameters history;
    
    // World-level settings
    WorldSize worldSize = WorldSize::REGIONAL;
    WorldPreset preset = WorldPreset::NORMAL;
    
    // Apply a preset configuration
    void applyPreset(WorldPreset preset, WorldSize size = WorldSize::REGIONAL);
    
    // Scale parameters based on world size
    void scaleForWorldSize(WorldSize size);
    
    // Validate parameters and clamp to safe ranges
    bool validateParameters();
    
    // Serialization
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Parameter access and modification
    void setTerrainParameter(const std::string& name, float value);
    float getTerrainParameter(const std::string& name) const;
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    static std::string getPresetDescription(WorldPreset preset);
    static std::string getWorldSizeDescription(WorldSize size);
    
private:
    void initializeDefaults();
    void clampToValidRanges();
};

// Utility functions for parameter conversion
const char* getWorldSizeName(WorldSize size);
const char* getHistoricalDepthName(HistoricalDepth depth);
const char* getWorldPresetName(WorldPreset preset);

} // namespace World
} // namespace VoxelCastle
