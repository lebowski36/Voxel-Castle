#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstdint>

namespace VoxelCastle {
    
/**
 * @brief Types of tectonic plates based on crustal composition and behavior
 */
enum class PlateType : uint8_t {
    OCEANIC,        // Dense oceanic crust, subducts under continental plates
    CONTINENTAL,    // Less dense continental crust, resists subduction
    MICROPLATE,     // Small plates, often more volatile movement
    
    COUNT           // For iteration and validation
};

/**
 * @brief Terrain types resulting from tectonic plate interactions
 */
enum class TerrainType : uint8_t {
    STABLE,         // Stable continental interior
    MOUNTAIN,       // Convergent boundary mountain ranges
    RIFT,           // Divergent boundary rift valleys
    OCEANIC,        // Ocean floor areas
    FAULT,          // Transform boundary fault zones
    
    COUNT           // For iteration and validation
};

/**
 * @brief Represents a single tectonic plate with movement and geological properties
 */
struct TectonicPlate {
    uint32_t plateId;                    // Unique plate identifier
    glm::vec2 centerPosition;            // Plate center in world coordinates (km)
    glm::vec2 movementVector;            // Direction and speed of movement (km/million years)
    float area;                          // Plate size in km²
    PlateType type;                      // Oceanic, continental, or microplate
    float density;                       // Crustal density (affects collision behavior)
    std::vector<glm::vec2> boundary;     // Plate boundary vertices in world coordinates
    
    // Age and stability factors
    float age;                           // Plate age in million years (affects density)
    float stability;                     // Stability factor (0.0-1.0, affects movement changes)
    
    /**
     * @brief Default constructor initializes to stable continental plate
     */
    TectonicPlate();
    
    /**
     * @brief Constructor with basic parameters
     */
    TectonicPlate(uint32_t id, glm::vec2 center, PlateType plateType);
    
    /**
     * @brief Check if a world position is within this plate's boundary
     * @param worldPos Position in world coordinates (km)
     * @return True if position is within plate boundary
     */
    bool ContainsPosition(glm::vec2 worldPos) const;
    
    /**
     * @brief Get distance from position to nearest plate boundary
     * @param worldPos Position in world coordinates (km)
     * @return Distance to boundary in km (negative if outside plate)
     */
    float DistanceToBoundary(glm::vec2 worldPos) const;
    
    /**
     * @brief Serialization methods for persistence
     */
    void Serialize(std::vector<uint8_t>& buffer) const;
    void Deserialize(const uint8_t* data, size_t& offset);
    static size_t GetSerializedSize();
};

/**
 * @brief Utility functions for plate type operations
 */
const char* PlateTypeToString(PlateType type);
PlateType StringToPlateType(const std::string& str);

const char* TerrainTypeToString(TerrainType type);
TerrainType StringToTerrainType(const std::string& str);

/**
 * @brief Calculate relative density for plate interactions
 * @param type Plate type
 * @param age Plate age in million years
 * @return Effective density for collision calculations
 */
float CalculatePlateDensity(PlateType type, float age);

/**
 * @brief Generate typical movement vector for plate type
 * @param type Plate type
 * @param seed Random seed for deterministic generation
 * @return Movement vector in km/million years
 */
glm::vec2 GenerateTypicalMovement(PlateType type, uint32_t seed);

} // namespace VoxelCastle
