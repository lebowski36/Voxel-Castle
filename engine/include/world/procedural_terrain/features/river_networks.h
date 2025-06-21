#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <cstdint>

// Forward declarations
namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {
    struct ClimateData;
}
}
}

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

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

// Legacy structure for backward compatibility with existing code
struct EnhancedRiverData {
    float fractal_river_strength;
    float river_width_m;
    float river_depth_m;
    glm::vec2 flow_direction;
    float flow_velocity_ms;
    int stream_order;
    bool is_headwater;
    bool is_main_stem;
    bool is_tributary;
    float carving_depth_m;
    float valley_width_m;
    bool creates_floodplain;
    float floodplain_width_m;
    float meander_intensity;
    bool is_braided;
    
    EnhancedRiverData() : fractal_river_strength(0), river_width_m(0), river_depth_m(0),
                         flow_direction(0, 0), flow_velocity_ms(0), stream_order(0),
                         is_headwater(false), is_main_stem(false), is_tributary(false),
                         carving_depth_m(0), valley_width_m(0), creates_floodplain(false),
                         floodplain_width_m(0), meander_intensity(0), is_braided(false) {}
};

/**
 * River Network Generation System
 * Generates realistic connected river networks with proper flow paths, lakes, and terrain carving
 */
class RiverNetworks {
public:
    // === Main River Network Generation ===
    
    /**
     * Generate complete river network for a region
     * @param region_x Region X coordinate (in region units)
     * @param region_z Region Z coordinate (in region units)  
     * @param region_size Size of region in meters
     * @param seed World seed for deterministic generation
     * @return Complete river network for the region
     */
    static RiverNetwork GenerateRiverNetwork(int region_x, int region_z, 
                                            int region_size, uint64_t seed);
    
    /**
     * Query river data at a specific world coordinate
     * @param world_x X coordinate in world space
     * @param world_z Z coordinate in world space
     * @param seed World seed
     * @return River query result with all water feature data
     */
    static RiverQueryResult QueryRiverAtPoint(float world_x, float world_z, uint64_t seed);
    
    /**
     * Apply river carving to terrain elevation
     * @param base_elevation Original terrain elevation
     * @param world_x X coordinate in world space
     * @param world_z Z coordinate in world space
     * @param seed World seed
     * @return Modified elevation with river carving applied
     */
    static float ApplyRiverCarving(float base_elevation, float world_x, 
                                 float world_z, uint64_t seed);
    
    // === Legacy Compatibility Functions ===
    
    /**
     * Generate comprehensive river data (legacy compatibility)
     */
    static EnhancedRiverData GenerateComprehensiveRiverData(float worldX, float worldZ, 
                                                           const ClimateData& climate, 
                                                           float elevation, uint64_t seed);
    
    /**
     * Calculate flow accumulation (legacy compatibility)
     */
    static float CalculateFlowAccumulation(float worldX, float worldZ, uint64_t seed);
    
    /**
     * Calculate flow direction (legacy compatibility)
     */
    static glm::vec2 CalculateFlowDirection(float world_x, float world_z, uint64_t seed);
    
    // === River Path Generation ===
    
    /**
     * Trace a complete river path from source to destination
     * @param source_x Starting X coordinate
     * @param source_z Starting Z coordinate
     * @param source_elevation Starting elevation
     * @param seed World seed
     * @return Complete river path with all points
     */
    static RiverPath TraceRiverPath(float source_x, float source_z, 
                                   float source_elevation, uint64_t seed);
    
    /**
     * Generate detailed river points along a path segment
     * @param start_x Start X coordinate
     * @param start_z Start Z coordinate
     * @param end_x End X coordinate
     * @param end_z End Z coordinate
     * @param start_elevation Start elevation
     * @param end_elevation End elevation
     * @param river_width River width for this segment
     * @param seed World seed
     * @return Vector of detailed river points
     */
    static std::vector<RiverPoint> GenerateRiverPoints(float start_x, float start_z,
                                                     float end_x, float end_z,
                                                     float start_elevation, float end_elevation,
                                                     float river_width, uint64_t seed);
    
    // === Lake Generation ===
    
    /**
     * Generate lakes for a region
     * @param region_x Region X coordinate
     * @param region_z Region Z coordinate
     * @param region_size Region size in meters
     * @param seed World seed
     * @return Vector of lakes in the region
     */
    static std::vector<Lake> GenerateLakes(int region_x, int region_z, 
                                         int region_size, uint64_t seed);
    
    // === River Source Detection ===
    
    /**
     * Determine if a location should be a river source
     * @param world_x X coordinate in world space
     * @param world_z Z coordinate in world space
     * @param climate Climate data for the location
     * @param elevation Terrain elevation
     * @param seed World seed
     * @return True if this should be a river source
     */
    static bool IsRiverSource(float world_x, float world_z, 
                            const ClimateData& climate, float elevation, uint64_t seed);
    
    // === River Width and Flow Calculations ===
    
    /**
     * Calculate river width based on accumulated flow
     * @param accumulated_flow Total flow accumulation
     * @param base_width Base width for the river type
     * @return Calculated river width in meters
     */
    static float CalculateRiverWidth(float accumulated_flow, float base_width);

private:
    // === Internal Helper Functions ===
    
    /**
     * Get terrain elevation at a point (without river carving)
     */
    static float GetTerrainElevation(float world_x, float world_z, uint64_t seed);
    
    /**
     * Calculate flow accumulation along a river path
     */
    static float CalculateFlowAccumulation(const std::vector<RiverPoint>& upstream_points);
    
    /**
     * Determine if elevation change should create a waterfall
     */
    static bool ShouldCreateWaterfall(float elevation_change, float distance);
    
    /**
     * Determine if elevation change should create rapids
     */
    static bool ShouldCreateRapids(float elevation_change, float distance);
    
    /**
     * Find potential river sources in a region
     */
    static std::vector<std::pair<float, float>> FindRiverSources(int region_x, int region_z, 
                                                               int region_size, uint64_t seed);
    
    /**
     * Determine river density based on climate and terrain
     */
    static float GetRiverDensityFactor(const ClimateData& climate, float elevation);
    
    // === Region-based Caching System ===
    
    /**
     * Get cached river network or generate new one
     */
    static const RiverNetwork& GetCachedRiverNetwork(int region_x, int region_z, 
                                                   int region_size, uint64_t seed);
    
    /**
     * Generate unique hash for a region
     */
    static uint64_t GetRegionHash(int region_x, int region_z, int region_size);
    
    // Cache for river networks (static to persist across calls)
    static std::unordered_map<uint64_t, RiverNetwork> cached_networks;
    
    // Prevent instantiation - this is a static utility class
    RiverNetworks() = delete;
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
