#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace VoxelCastle {
namespace World {

/**
 * Rock types for geological simulation
 */
enum class RockType : uint8_t {
    SEDIMENTARY_LIMESTONE = 0,
    SEDIMENTARY_SANDSTONE = 1,
    SEDIMENTARY_SHALE = 2,
    IGNEOUS_GRANITE = 3,
    IGNEOUS_BASALT = 4,
    IGNEOUS_OBSIDIAN = 5,
    METAMORPHIC_MARBLE = 6,
    METAMORPHIC_SLATE = 7,
    METAMORPHIC_QUARTZITE = 8,
    SOIL_CLAY = 9,
    SOIL_SAND = 10,
    SOIL_LOAM = 11,
    COUNT = 12
};

/**
 * Mineral types for ore distribution
 */
enum class MineralType : uint8_t {
    IRON = 0,
    COPPER = 1,
    GOLD = 2,
    SILVER = 3,
    COAL = 4,
    DIAMOND = 5,
    EMERALD = 6,
    QUARTZ = 7,
    SALT = 8,
    OIL = 9,
    COUNT = 10
};

/**
 * Geological layer information
 */
struct GeologicalLayer {
    RockType rockType;          // Type of rock/soil in this layer
    float thickness;            // Thickness in meters
    float hardness;             // Relative hardness (0.0-1.0)
    float porosity;             // Water absorption capacity (0.0-1.0)
    uint8_t mineralContent[static_cast<size_t>(MineralType::COUNT)]; // Mineral percentages (0-100)
    
    GeologicalLayer();
    GeologicalLayer(RockType type, float thick, float hard, float porous);
    
    // Serialization
    bool SerializeToBinary(std::vector<uint8_t>& buffer) const;
    bool DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset);
    
    // Validation
    bool IsValid() const;
    void SetDefaults();
    
    // Utility
    std::string ToString() const;
};

/**
 * Comprehensive geological data for a region
 * Includes rock layers, erosion history, and mineral distribution
 */
struct GeologicalData {
    static constexpr uint32_t MAX_LAYERS = 16; // Maximum geological layers
    static constexpr uint32_t CURRENT_VERSION = 1;
    
    // Basic geological properties
    float bedrockDepth;         // Depth to bedrock in meters
    float soilDepth;            // Depth of soil layer in meters
    float tectonicStability;    // Tectonic stability index (0.0-1.0)
    float erosionResistance;    // Resistance to erosion (0.0-1.0)
    
    // Geological layers (surface to bedrock)
    uint8_t layerCount;         // Number of active layers
    GeologicalLayer layers[MAX_LAYERS];
    
    // Erosion and weathering data
    float hydraulicErosion;     // Amount of water erosion applied
    float thermalErosion;       // Amount of thermal erosion applied
    float chemicalWeathering;   // Chemical weathering intensity
    float massWasting;          // Landslide/rockfall susceptibility
    
    // Resource distribution
    float mineralRichness;      // Overall mineral richness (0.0-1.0)
    float oilReserves;          // Oil/petroleum reserves (cubic meters)
    float groundwaterDepth;     // Depth to groundwater table (meters)
    
    // Constructors
    GeologicalData();
    
    // Serialization methods
    bool SerializeToBinary(std::vector<uint8_t>& buffer) const;
    bool DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset);
    
    // Validation methods
    bool IsValid() const;
    void SetDefaults();
    
    // Layer management
    bool AddLayer(const GeologicalLayer& layer);
    bool RemoveLayer(size_t index);
    const GeologicalLayer* GetLayer(size_t index) const;
    
    // Resource queries
    float GetMineralDensity(MineralType mineral) const;
    RockType GetSurfaceRockType() const;
    float GetHardnessAtDepth(float depth) const;
    
    // Utility methods
    std::string ToString() const;
    size_t GetSerializedSize() const;
};

// Utility functions
const char* RockTypeToString(RockType type);
const char* MineralTypeToString(MineralType type);
RockType StringToRockType(const std::string& str);
MineralType StringToMineralType(const std::string& str);

} // namespace World
} // namespace VoxelCastle
