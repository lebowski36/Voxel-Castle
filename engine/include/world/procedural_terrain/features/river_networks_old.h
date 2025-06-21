#pragma once

#include <cstdint>
#include <vector>
#include <glm/vec2.hpp>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

// Forward declarations
struct ClimateData;

/**
 * River network generation constants
 */
namespace RiverConstants {
    // Region scales for hierarchical river generation
    constexpr int CONTINENTAL_REGION_SIZE = 500000;    // 500km regions for major river basins
    constexpr int REGIONAL_REGION_SIZE = 100000;       // 100km regions for river networks
    constexpr int LOCAL_REGION_SIZE = 20000;           // 20km regions for local streams
    constexpr int MICRO_REGION_SIZE = 4000;            // 4km regions for creek networks
    
    // River density parameters (realistic Earth-based)
    constexpr float MAJOR_RIVER_DENSITY = 0.8f;       // 0-3 major rivers per 100km² region
    constexpr float REGIONAL_RIVER_DENSITY = 2.5f;    // 1-5 regional rivers per 100km²
    constexpr float STREAM_DENSITY = 8.0f;            // 3-15 streams per 100km²
    
    // River size thresholds
    constexpr float MAJOR_RIVER_WIDTH = 100.0f;       // 100m+ wide
    constexpr float REGIONAL_RIVER_WIDTH = 30.0f;     // 30-100m wide
    constexpr float STREAM_WIDTH = 5.0f;              // 5-30m wide
    constexpr float CREEK_WIDTH = 1.0f;               // 1-5m wide
    
    // Terrain carving parameters
    constexpr float CARVING_DEPTH_FACTOR = 0.1f;      // River depth = width * factor
    constexpr float VALLEY_WIDTH_FACTOR = 3.0f;       // Valley width = river width * factor
    constexpr float WATERFALL_MIN_GRADIENT = 0.15f;   // 15% gradient for waterfalls
    constexpr float RAPIDS_MIN_GRADIENT = 0.08f;      // 8% gradient for rapids
    
    // Lake generation parameters
    constexpr float LAKE_FORMATION_THRESHOLD = 0.3f;  // Probability threshold for lake formation
    constexpr float MIN_LAKE_SIZE = 500.0f;           // Minimum lake diameter (meters)
    constexpr float MAX_LAKE_SIZE = 50000.0f;         // Maximum lake diameter (meters)
}

/**
 * River network data structures
 */

// River point along a path
struct RiverPoint {
    float x, z;              // World coordinates
    float elevation;         // Water surface elevation
    float width;             // River width at this point
    float depth;             // River depth
    bool has_waterfall;      // Waterfall at this point
    bool has_rapids;         // Rapids at this point
    
    RiverPoint() : x(0), z(0), elevation(0), width(0), depth(0), 
                   has_waterfall(false), has_rapids(false) {}
};

// Complete river path from source to destination
struct RiverPath {
    std::vector<RiverPoint> points;    // Path points in order from source
    int river_id;                      // Unique river identifier
    int parent_river_id;               // ID of river this joins (-1 if flows to ocean)
    float total_length;                // Total river length in meters
    float max_width;                   // Maximum width along path
    bool flows_to_ocean;               // True if reaches ocean, false if internal drainage
    bool is_seasonal;                  // True if dries up in dry seasons
    
    RiverPath() : river_id(-1), parent_river_id(-1), total_length(0), 
                  max_width(0), flows_to_ocean(false), is_seasonal(false) {}
};

// Lake data structure
struct Lake {
    float center_x, center_z;         // Lake center coordinates
    float surface_elevation;          // Water surface elevation
    float radius;                     // Lake radius (approximate)
    int connected_river_id;           // River ID if connected (-1 if standalone)
    bool is_seasonal;                 // True if dries up seasonally
    bool has_outflow;                 // True if lake has river outlet
    
    Lake() : center_x(0), center_z(0), surface_elevation(0), radius(0),
             connected_river_id(-1), is_seasonal(false), has_outflow(false) {}
};

// River network for a region
struct RiverNetwork {
    std::vector<RiverPath> rivers;     // All rivers in this region
    std::vector<Lake> lakes;           // All lakes in this region
    int region_x, region_z;            // Region coordinates
    int region_size;                   // Region size in meters
    
    RiverNetwork() : region_x(0), region_z(0), region_size(0) {}
};

// Result of querying rivers at a specific point
struct RiverQueryResult {
    bool has_river;                    // True if river passes through this point
    float river_width;                 // River width at this point (0 if no river)
    float river_depth;                 // River depth at this point
    float water_elevation;             // Water surface elevation
    bool has_lake;                     // True if lake covers this point
    float lake_depth;                  // Lake depth at this point (0 if no lake)
    bool has_waterfall;                // True if waterfall at this point
    bool has_rapids;                   // True if rapids at this point
    
    RiverQueryResult() : has_river(false), river_width(0), river_depth(0), 
                        water_elevation(0), has_lake(false), lake_depth(0),
                        has_waterfall(false), has_rapids(false) {}
};

/**
 * Waterfall and rapids data
 */
struct WaterfallData {
    bool has_waterfall;
    float waterfall_height;
    bool has_rapids;
    float gradient;
    
    WaterfallData() : has_waterfall(false), waterfall_height(0.0f), 
                     has_rapids(false), gradient(0.0f) {}
};

/**
 * Groundwater and spring data
 */
struct GroundwaterData {
    float water_table_depth;    // Depth to groundwater (meters)
    bool is_spring_source;      // Natural spring location
    float spring_flow_rate;     // Water volume from spring
    bool aquifer_connection;    // Connected to underground water
    
    GroundwaterData() : water_table_depth(0.0f), is_spring_source(false), 
                       spring_flow_rate(0.0f), aquifer_connection(false) {}
};

/**
 * River termination types
 */
enum class RiverTerminationType {
    OCEAN,              // Flows to ocean/sea
    INLAND_LAKE,        // Ends in endorheic lake
    DESERT_SINK,        // Disappears in arid region
    UNDERGROUND_CAPTURE, // Goes underground in karst
    WETLAND_DISPERSION, // Spreads into marshland
    GLACIER_SOURCE      // Starts/ends at glacier
};

/**
 * Riparian biome types
 */
enum class RiparianBiomeType {
    RIPARIAN_FOREST,      // Forested river corridors
    WETLAND_MARSH,        // Marshy transition zones
    FLOODPLAIN_GRASSLAND, // Seasonal flood zones
    DESERT_OASIS,         // Water sources in arid regions
    ALPINE_MEADOW,        // High elevation river valleys
    MANGROVE_SWAMP        // Tropical coastal wetlands
};

/**
 * Simple flow data for local chunk simulation
 */
struct SimpleFlowData {
    glm::vec2 flow_direction;    // Which way water flows (normalized)
    float flow_speed_ms;         // Flow speed in meters/second (0.1-5.0)
    float terrain_slope;         // Local terrain gradient (0.0-1.0)
    bool has_water_now;          // Whether water is present (climate-based)
    float visual_flow_rate;      // For visual effects (particle systems, etc.)
    
    SimpleFlowData() : flow_direction(0.0f), flow_speed_ms(0.0f), terrain_slope(0.0f), 
                      has_water_now(false), visual_flow_rate(0.0f) {}
};

/**
 * Underground river connections
 */
struct UndergroundConnection {
    bool connects_to_caves;
    float underground_flow_rate;
    int cave_entrance_count;
    bool creates_aquifer;
    float aquifer_extent;
    
    UndergroundConnection() : connects_to_caves(false), underground_flow_rate(0.0f), 
                             cave_entrance_count(0), creates_aquifer(false), aquifer_extent(0.0f) {}
};

/**
 * Complete river data structure using hierarchical fractal generation
 */
struct EnhancedRiverData {
    // Basic flow characteristics
    float fractal_river_strength;   // Raw fractal strength calculation (0-1000+)
    float river_width_m;            // Channel width (0.5m to 1000m+)
    float river_depth_m;            // Channel depth (0.2m to 50m+)
    glm::vec2 flow_direction;       // Normalized flow direction vector
    float flow_velocity_ms;         // Water velocity (0.1-5.0 m/s)
    
    // Hierarchical classification
    int stream_order;               // Strahler stream order (1-12)
    bool is_headwater;              // Source/spring location
    bool is_main_stem;              // Primary river channel
    bool is_tributary;              // Branch/tributary channel
    
    // Confluence data
    ConfluenceData confluence;      // Junction information
    
    // Terrain modification
    float carving_depth_m;          // How deep river carves terrain
    float valley_width_m;           // Width of carved valley
    bool creates_floodplain;        // Has seasonal flood zones
    float floodplain_width_m;       // Width of flood zone
    
    // Special features
    WaterfallData waterfall;        // Waterfall/rapids information
    float meander_intensity;        // How curvy the river is (0-1)
    bool is_braided;               // Multiple interweaving channels
    
    // Termination and sources
    GroundwaterData groundwater;    // Spring/aquifer connections
    RiverTerminationType termination; // How/where river ends
    
    // Environmental integration
    RiparianBiomeType riparian_biome; // Associated vegetation type
    UndergroundConnection underground; // Cave/aquifer connections
    
    // Climate interaction
    bool is_seasonal;              // Varies with seasons (not implemented yet)
    float base_flow_rate;          // Baseline water volume
    float drought_resistance;      // How well river survives dry periods
    
    EnhancedRiverData() : fractal_river_strength(0.0f), river_width_m(0.0f), river_depth_m(0.0f),
                         flow_direction(0.0f), flow_velocity_ms(0.0f), stream_order(0),
                         is_headwater(false), is_main_stem(false), is_tributary(false),
                         carving_depth_m(0.0f), valley_width_m(0.0f), creates_floodplain(false),
                         floodplain_width_m(0.0f), meander_intensity(0.0f), is_braided(false),
                         termination(RiverTerminationType::WETLAND_DISPERSION),
                         riparian_biome(RiparianBiomeType::RIPARIAN_FOREST),
                         is_seasonal(false), base_flow_rate(0.0f), drought_resistance(0.0f) {}
};

/**
 * Deterministic Fractal River Networks System
 * Generates realistic river systems using hierarchical fractals while maintaining perfect chunk independence
 */
class RiverNetworks {
public:
    // === Core Hierarchical Fractal Functions ===
    
    /**
     * Calculate hierarchical flow accumulation using fractal watershed levels
     * This is the core function that replaces traditional watershed analysis
     */
    static float CalculateFlowAccumulation(float worldX, float worldZ, uint64_t seed);
    
    /**
     * Enhanced river strength calculation (alias for flow accumulation)
     */
    static float CalculateFractalRiverStrength(float worldX, float worldZ, uint64_t seed);
    
    /**
     * Ensure river continuity by connecting strong neighboring segments
     */
    static float EnsureRiverContinuity(float baseStrength, float worldX, float worldZ, uint64_t seed);
    
    /**
     * Calculate natural confluence points where fractal scales converge
     */
    static ConfluenceData CalculateConfluence(float worldX, float worldZ, uint64_t seed);
    
    // === Flow Direction & Terrain Integration ===
    
    /**
     * Calculate deterministic flow direction using elevation gradients
     */
    static glm::vec2 CalculateFlowDirection(float worldX, float worldZ, uint64_t seed);
    
    /**
     * Apply river carving to terrain elevation
     */
    static float ApplyRiverCarving(float baseElevation, float worldX, float worldZ, uint64_t seed);
    
    /**
     * Calculate terrain gradient magnitude
     */
    static float CalculateTerrainGradient(float worldX, float worldZ, uint64_t seed);
    
    // === River Feature Generation ===
    
    /**
     * Calculate waterfall and rapids features
     */
    static WaterfallData CalculateWaterfallFeatures(float worldX, float worldZ, float riverStrength, uint64_t seed);
    
    /**
     * Calculate meandering intensity based on terrain and river size
     */
    static float CalculateMeanderIntensity(float riverStrength, float terrainGradient, uint64_t seed);
    
    /**
     * Apply meandering to flow direction
     */
    static glm::vec2 ApplyMeandering(glm::vec2 baseFlowDirection, float worldX, float worldZ, 
                                    float meanderIntensity, uint64_t seed);
    
    /**
     * Calculate groundwater and spring data
     */
    static GroundwaterData CalculateGroundwater(float worldX, float worldZ, float surfaceElevation, uint64_t seed);
    
    // === River Dimensions & Classification ===
    
    /**
     * Calculate river width based on flow accumulation
     */
    static float CalculateRiverWidth(float riverStrength);
    
    /**
     * Calculate river depth based on flow accumulation
     */
    static float CalculateRiverDepth(float riverStrength);
    
    /**
     * Calculate flow velocity based on strength and terrain gradient
     */
    static float CalculateFlowVelocity(float riverStrength, float terrainGradient);
    
    /**
     * Calculate Strahler stream order from river strength
     */
    static int CalculateStreamOrder(float riverStrength);
    
    /**
     * Determine if location is a headwater (river source)
     */
    static bool DetermineIfHeadwater(float worldX, float worldZ, float riverStrength, uint64_t seed);
    
    /**
     * Determine if river has braided pattern (multiple channels)
     */
    static bool DetermineBraidedPattern(float riverStrength, const ClimateData& climate, uint64_t seed);
    
    // === Termination & Biome Integration ===
    
    /**
     * Determine how/where river terminates
     */
    static RiverTerminationType DetermineRiverTermination(float worldX, float worldZ, float riverStrength, 
                                                         const ClimateData& climate, uint64_t seed);
    
    /**
     * Determine riparian biome type
     */
    static RiparianBiomeType DetermineRiparianBiome(const ClimateData& climate, float elevation, 
                                                   float distanceFromRiver, float riverStrength);
    
    /**
     * Calculate underground connections to caves and aquifers
     */
    static UndergroundConnection CalculateUndergroundConnection(float worldX, float worldZ, 
                                                               float riverStrength, uint64_t seed);
    
    // === Local Flow Simulation ===
    
    /**
     * Calculate simple local flow data for chunk-level simulation
     */
    static SimpleFlowData CalculateLocalFlow(float worldX, float worldZ, uint64_t seed, 
                                           const ClimateData& climate);
    
    /**
     * Calculate terrain slope for flow physics
     */
    static float CalculateTerrainSlope(float worldX, float worldZ, uint64_t seed);
    
    // === Main Generation Function ===
    
    /**
     * Generate comprehensive river data for a specific world position
     * This is the main entry point that combines all fractal river calculations
     */
    static EnhancedRiverData GenerateComprehensiveRiverData(float worldX, float worldZ, 
                                                          const ClimateData& climate, 
                                                          float surfaceElevation, 
                                                          uint64_t seed);
    
    // === Helper Functions ===
    
    /**
     * Calculate distance from river centerline
     */
    static float CalculateDistanceFromRiverCenter(float worldX, float worldZ, glm::vec2 flowDirection, uint64_t seed);
    
    /**
     * Calculate drought resistance based on groundwater and river size
     */
    static float CalculateDroughtResistance(const GroundwaterData& groundwater, float riverStrength);
    
    /**
     * Calculate distance to nearest river (for biome influence)
     */
    static float CalculateDistanceToNearestRiver(float worldX, float worldZ, uint64_t seed);
    
    /**
     * Helper function for rotating vectors (used in delta generation)
     */
    static glm::vec2 RotateVector(glm::vec2 vec, float angleRadians);

private:
    // Prevent instantiation - this is a static utility class
    RiverNetworks() = delete;
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
