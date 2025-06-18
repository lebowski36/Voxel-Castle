#pragma once

#include <cstdint>
#include <vector>

namespace VoxelCastle {
namespace World {
namespace ProceduralTerrain {

/**
 * River network data structure
 */
struct RiverData {
    float flow_accumulation;  // Amount of water flowing through this point (0.0 to 1.0)
    float river_width;        // Width of river in meters (0 = no river)
    float river_depth;        // Depth of river channel in meters
    bool is_lake;            // True if this point is part of a lake/pond
};

/**
 * Deterministic river network generation system
 * Rivers flow downhill based on elevation and accumulate flow from precipitation
 */
class RiverNetworks {
public:
    /**
     * Calculate river data for a specific world position
     * @param worldX World X coordinate (in voxel scale units)
     * @param worldZ World Z coordinate (in voxel scale units)
     * @param elevation Terrain elevation at this position
     * @param precipitation Annual precipitation at this position
     * @param seed World generation seed
     * @return River data for this position
     */
    static RiverData CalculateRiverData(float worldX, float worldZ, float elevation, 
                                       float precipitation, uint64_t seed);

    /**
     * Calculate flow accumulation using watershed analysis
     * @param worldX World X coordinate
     * @param worldZ World Z coordinate
     * @param elevation Terrain elevation
     * @param precipitation Precipitation input
     * @param seed World generation seed
     * @return Flow accumulation value (0.0 to 1.0)
     */
    static float CalculateFlowAccumulation(float worldX, float worldZ, float elevation,
                                          float precipitation, uint64_t seed);

    /**
     * Determine if a location should have a river based on flow accumulation
     * @param flowAccumulation Flow accumulation value
     * @param elevation Current elevation
     * @param seed World generation seed
     * @return River width in meters (0 = no river)
     */
    static float CalculateRiverWidth(float flowAccumulation, float elevation, uint64_t seed);

    /**
     * Calculate river depth based on width and flow
     * @param riverWidth River width in meters
     * @param flowAccumulation Flow accumulation
     * @return River depth in meters
     */
    static float CalculateRiverDepth(float riverWidth, float flowAccumulation);

    /**
     * Determine if a location should be a lake
     * @param worldX World X coordinate
     * @param worldZ World Z coordinate
     * @param elevation Terrain elevation
     * @param flowAccumulation Flow accumulation
     * @param seed World generation seed
     * @return True if this should be a lake
     */
    static bool IsLakeLocation(float worldX, float worldZ, float elevation,
                              float flowAccumulation, uint64_t seed);

private:
    // River generation constants
    static constexpr float FLOW_THRESHOLD = 0.3f;        // Minimum flow to create a river
    static constexpr float RIVER_WIDTH_SCALE = 20.0f;    // Scaling factor for river width
    static constexpr float DEPTH_WIDTH_RATIO = 0.1f;     // River depth as fraction of width
    static constexpr float LAKE_THRESHOLD = 0.7f;        // Flow threshold for lake formation
    static constexpr float SAMPLING_RADIUS = 500.0f;     // Radius for watershed sampling

    // Helper functions
    static float SampleElevationGradient(float worldX, float worldZ, uint64_t seed);
    static float CalculateWatershedContribution(float worldX, float worldZ, 
                                               float precipitation, uint64_t seed);
};

} // namespace ProceduralTerrain
} // namespace World
} // namespace VoxelCastle
