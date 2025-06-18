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
 * River generation constants and thresholds
 */
namespace RiverConstants {
    constexpr float RIVER_THRESHOLD = 30.0f;           // Minimum strength for river presence
    constexpr float CONFLUENCE_THRESHOLD = 100.0f;     // Minimum strength for confluence
    constexpr float MAJOR_RIVER_THRESHOLD = 500.0f;    // Threshold for major rivers
    constexpr float WATERFALL_GRADIENT = 0.15f;        // Minimum gradient for waterfalls
    constexpr float RAPIDS_GRADIENT = 0.08f;           // Minimum gradient for rapids

    // Scaling factors
    constexpr float WIDTH_SCALE_FACTOR = 0.1f;         // River width = strength * factor
    constexpr float DEPTH_SCALE_FACTOR = 0.05f;        // River depth = strength * factor
    constexpr float CARVING_SCALE_FACTOR = 1.5f;       // Carving depth = river depth * factor
    constexpr float VALLEY_WIDTH_FACTOR = 3.0f;        // Valley width = river width * factor

    // Fractal noise frequencies (more realistic spacing)
    constexpr float CONTINENTAL_FREQ = 0.00002f;       // 1 cycle per 50km (major rivers)
    constexpr float REGIONAL_FREQ = 0.0001f;           // 1 cycle per 10km (regional rivers)
    constexpr float LOCAL_FREQ = 0.0005f;              // 1 cycle per 2km (small rivers)
    constexpr float STREAM_FREQ = 0.002f;              // 1 cycle per 500m (streams)
    constexpr float CREEK_FREQ = 0.01f;                // 1 cycle per 100m (creeks)
}

/**
 * Confluence detection data
 */
struct ConfluenceData {
    bool is_confluence;
    float confluence_strength;
    int tributary_count;
    float main_river_direction;
    std::vector<float> tributary_strengths;
    
    ConfluenceData() : is_confluence(false), confluence_strength(0.0f), 
                      tributary_count(0), main_river_direction(0.0f) {}
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
