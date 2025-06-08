#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace VoxelCastle {
namespace World {

/**
 * River size classification
 */
enum class RiverSize : uint8_t {
    STREAM = 0,     // Small streams
    CREEK = 1,      // Medium creeks
    RIVER = 2,      // Large rivers
    MAJOR_RIVER = 3 // Major rivers
};

/**
 * Water body types
 */
enum class WaterBodyType : uint8_t {
    NONE = 0,
    RIVER = 1,
    LAKE = 2,
    POND = 3,
    MARSH = 4,
    WETLAND = 5,
    SPRING = 6,
    WATERFALL = 7
};

/**
 * Single river segment data
 */
struct RiverSegment {
    float startX, startZ;       // Start coordinates (local to region)
    float endX, endZ;           // End coordinates (local to region)
    float width;                // River width in meters
    float depth;                // River depth in meters
    float flow;                 // Flow rate (cubic meters per second)
    float elevation;            // Water surface elevation
    RiverSize size;             // River size classification
    uint8_t connectsTo[4];      // Indices of connected segments (255 = none)
    
    RiverSegment();
    RiverSegment(float sx, float sz, float ex, float ez, float w, float d, float f);
    
    // Serialization
    bool SerializeToBinary(std::vector<uint8_t>& buffer) const;
    bool DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset);
    
    // Validation
    bool IsValid() const;
    void SetDefaults();
    
    // Utility
    float GetLength() const;
    std::string ToString() const;
};

/**
 * Water body information
 */
struct WaterBody {
    WaterBodyType type;         // Type of water body
    float centerX, centerZ;     // Center coordinates (local to region)
    float area;                 // Surface area (square meters)
    float volume;               // Water volume (cubic meters)
    float depth;                // Maximum depth (meters)
    float elevation;            // Water surface elevation
    float temperature;          // Water temperature (°C)
    uint8_t connectedRivers[8]; // Indices of connected rivers (255 = none)
    
    WaterBody();
    WaterBody(WaterBodyType t, float x, float z, float a, float v, float d);
    
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
 * Comprehensive hydrological data for a region
 * Includes rivers, lakes, groundwater, and watersheds
 */
struct HydrologicalData {
    static constexpr uint32_t MAX_RIVER_SEGMENTS = 32;
    static constexpr uint32_t MAX_WATER_BODIES = 16;
    static constexpr uint32_t CURRENT_VERSION = 1;
    
    // Watershed and drainage information
    float drainageArea;         // Total drainage area (square km)
    float precipitationCapture; // Precipitation capture efficiency (0.0-1.0)
    float runoffCoefficient;    // Surface runoff coefficient (0.0-1.0)
    float infiltrationRate;     // Water infiltration rate (mm/hour)
    
    // River network
    uint8_t riverSegmentCount;  // Number of active river segments
    RiverSegment riverSegments[MAX_RIVER_SEGMENTS];
    
    // Water bodies
    uint8_t waterBodyCount;     // Number of active water bodies  
    WaterBody waterBodies[MAX_WATER_BODIES];
    
    // Groundwater data
    float groundwaterLevel;     // Groundwater table elevation (meters)
    float aquiferThickness;     // Aquifer thickness (meters)
    float porosity;             // Aquifer porosity (0.0-1.0)
    float permeability;         // Aquifer permeability (darcy units)
    float rechargeRate;         // Groundwater recharge rate (mm/year)
    
    // Flow and connectivity
    float totalFlow;            // Total water flow through region (m³/s)
    float flowDirection;        // Primary flow direction (radians)
    uint8_t watershedID;        // ID of watershed this region belongs to
    
    // Seasonal variations
    float springFlow;           // Flow multiplier for spring (0.0-2.0)
    float summerFlow;           // Flow multiplier for summer (0.0-2.0)
    float autumnFlow;           // Flow multiplier for autumn (0.0-2.0)
    float winterFlow;           // Flow multiplier for winter (0.0-2.0)
    
    // Special features
    bool hasWaterfall;          // Region contains waterfalls
    bool hasSpring;             // Region contains natural springs
    bool hasFloodPlain;         // Region is flood-prone
    bool hasWetlands;           // Region contains wetlands
    
    // Constructors
    HydrologicalData();
    
    // Serialization methods
    bool SerializeToBinary(std::vector<uint8_t>& buffer) const;
    bool DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset);
    
    // Validation methods
    bool IsValid() const;
    void SetDefaults();
    
    // River management
    bool AddRiverSegment(const RiverSegment& segment);
    bool RemoveRiverSegment(size_t index);
    const RiverSegment* GetRiverSegment(size_t index) const;
    
    // Water body management
    bool AddWaterBody(const WaterBody& body);
    bool RemoveWaterBody(size_t index);
    const WaterBody* GetWaterBody(size_t index) const;
    
    // Flow calculations
    float GetTotalRiverLength() const;
    float GetTotalWaterSurface() const;
    float GetSeasonalFlowMultiplier(int month) const; // month 0-11
    
    // Utility methods
    std::string ToString() const;
    size_t GetSerializedSize() const;
};

// Utility functions
const char* RiverSizeToString(RiverSize size);
const char* WaterBodyTypeToString(WaterBodyType type);
RiverSize StringToRiverSize(const std::string& str);
WaterBodyType StringToWaterBodyType(const std::string& str);

} // namespace World
} // namespace VoxelCastle
