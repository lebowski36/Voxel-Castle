#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include <stack>
#include <limits>
#include <glm/glm.hpp>
#include "world/geological_data.h"
#include "world/ContinuousField.h"
#include "world/GeologicalTypes.h"

namespace VoxelCastle {
namespace World {

/**
 * Continental Plate structure for fractal generation
 */
struct ContinentalPlate {
    glm::vec2 center;           // Center position in world coordinates (meters)
    float radius;               // Approximate radius in meters
    float elevation;            // Base elevation for this continent
    RockType dominantRockType;  // Primary rock type for this continent
    uint32_t plateId;           // Unique identifier
    float tectonicActivity;     // How active this plate is (0.0 to 1.0)
};

/**
 * Ocean Basin structure
 */
struct OceanBasin {
    std::vector<glm::vec2> boundary; // Boundary points
    float depth;                     // Ocean depth (negative elevation)
    float tectonicActivity;          // Activity level at this basin
};

/**
 * River Network Template structure
 */
struct RiverTemplate {
    std::vector<glm::vec2> mainStem;     // Main river path
    std::vector<std::vector<glm::vec2>> tributaries; // Tributary branches
    glm::vec2 source;                    // River source (mountain/high elevation)
    glm::vec2 mouth;                     // River mouth (coastline)
    uint32_t continentId;                // Which continent this river belongs to
};

/**
 * Mountain Ridge Template structure
 */
struct MountainRidge {
    std::vector<glm::vec2> ridgeLine;    // Main ridge spine
    float baseElevation;                 // Base elevation for this ridge
    float maxElevation;                  // Peak elevation
    uint32_t continentId;                // Which continent this ridge belongs to
    bool isVolcanic;                     // Whether this is a volcanic ridge
};

/**
 * Generates continental plates using Voronoi diagrams with fractal distribution
 * Replaces noise-based initialization with realistic continental shapes
 */
class FractalContinentGenerator {
public:
    explicit FractalContinentGenerator(uint64_t seed);
    
    /**
     * Generate complete continental foundation replacing noise-based initialization
     */
    void GenerateContinentalFoundation(
        ContinuousField<float>& elevationField,
        ContinuousField<RockType>& rockTypes,
        ContinuousField<float>& mantleStress,
        float worldSizeKm,
        const struct GeologicalConfig& config
    );
    
    /**
     * Get generated continental plates for later use
     */
    const std::vector<ContinentalPlate>& GetContinentalPlates() const { return continentalPlates_; }
    
    /**
     * Get generated ocean basins
     */
    const std::vector<OceanBasin>& GetOceanBasins() const { return oceanBasins_; }
    
    /**
     * Get generated river templates
     */
    const std::vector<RiverTemplate>& GetRiverTemplates() const { return riverTemplates_; }
    
    /**
     * Get generated mountain ridges
     */
    const std::vector<MountainRidge>& GetMountainRidges() const { return mountainRidges_; }

private:
    uint64_t seed_;
    std::vector<ContinentalPlate> continentalPlates_;
    std::vector<OceanBasin> oceanBasins_;
    std::vector<RiverTemplate> riverTemplates_;
    std::vector<MountainRidge> mountainRidges_;
    
    // Internal generation methods
    void generateContinentalPlates(float worldSizeKm, const GeologicalConfig& config);
    void generateOceanBasins(float worldSizeKm);
    void generateCoastlines(ContinuousField<float>& elevationField, float worldSizeKm);
    void generateRiverTemplates(float worldSizeKm);
    void generateMountainRidges(ContinuousField<float>& elevationField, float worldSizeKm);
    void assignRockTypes(ContinuousField<RockType>& rockTypes, float worldSizeKm);
    void initializeMantleStress(ContinuousField<float>& mantleStress, float worldSizeKm);
    float generateOrganicContinentalElevation(const glm::vec2& worldPos, float worldSizeKm);
    
    // Helper methods
    std::vector<glm::vec2> generateContinentalSeeds(float worldSizeKm, int numContinents);
    float calculateDistanceToNearestContinent(const glm::vec2& point) const;
    ContinentalPlate* findNearestContinent(const glm::vec2& point);
    bool isInOcean(const glm::vec2& point) const;
    float generateOceanDepth(const glm::vec2& point, float worldSizeKm) const;
    
    // L-System generation methods
    std::vector<glm::vec2> generateLSystemRiver(const glm::vec2& source, const glm::vec2& target, 
                                                float worldSizeKm, int iterations) const;
    std::vector<glm::vec2> generateLSystemMountainRidge(const glm::vec2& start, const glm::vec2& end,
                                                        float worldSizeKm, int iterations) const;
    std::string applyLSystemRules(const std::string& axiom, int iterations, 
                                  const std::map<char, std::string>& rules) const;
    std::vector<glm::vec2> interpretLSystemString(const std::string& lsystem, const glm::vec2& start,
                                                  const glm::vec2& direction, float stepSize) const;
    
    // Fractal algorithms
    float perlinNoise(float x, float y, float frequency) const;
    float fractionalBrownianMotion(float x, float y, int octaves, float persistence) const;
    glm::vec2 generateCoastlinePoint(const glm::vec2& basePoint, float detail) const;
    float generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const;
};

} // namespace World
} // namespace VoxelCastle
