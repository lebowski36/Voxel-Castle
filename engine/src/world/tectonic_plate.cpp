#include "world/tectonic_plate.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <cassert>
#include <cstring>
#include <iostream>

namespace VoxelCastle {

// Default constructor
TectonicPlate::TectonicPlate() 
    : plateId(0)
    , centerPosition(0.0f, 0.0f)
    , movementVector(0.0f, 0.0f)
    , area(0.0f)
    , type(PlateType::CONTINENTAL)
    , density(2.7f)  // Typical continental crust density (g/cm³)
    , age(500.0f)    // 500 million years (stable continental)
    , stability(0.8f) // High stability
{
}

// Constructor with basic parameters
TectonicPlate::TectonicPlate(uint32_t id, glm::vec2 center, PlateType plateType)
    : plateId(id)
    , centerPosition(center)
    , movementVector(0.0f, 0.0f)
    , area(0.0f)
    , type(plateType)
    , age(500.0f)
    , stability(0.8f)
{
    density = CalculatePlateDensity(plateType, age);
}

// Check if a world position is within this plate's boundary
bool TectonicPlate::ContainsPosition(glm::vec2 worldPos) const {
    if (boundary.size() < 3) {
        // Fallback to distance from center if no boundary is defined
        float distance = glm::length(worldPos - centerPosition);
        float estimatedRadius = std::sqrt(area / 3.14159f);
        return distance <= estimatedRadius;
    }
    
    // Ray casting algorithm for polygon containment
    bool inside = false;
    size_t j = boundary.size() - 1;
    
    for (size_t i = 0; i < boundary.size(); i++) {
        float xi = boundary[i].x, yi = boundary[i].y;
        float xj = boundary[j].x, yj = boundary[j].y;
        
        if (((yi > worldPos.y) != (yj > worldPos.y)) &&
            (worldPos.x < (xj - xi) * (worldPos.y - yi) / (yj - yi) + xi)) {
            inside = !inside;
        }
        j = i;
    }
    
    // If ray casting fails to find the point inside but we're very close to center,
    // use the distance-based fallback as a safety net
    if (!inside) {
        float distance = glm::length(worldPos - centerPosition);
        float estimatedRadius = std::sqrt(area / 3.14159f);
        if (distance <= estimatedRadius * 0.8f) { // Use 80% of radius for safety
            inside = true;
        }
    }
    
    return inside;
}

// Get distance from position to nearest plate boundary
float TectonicPlate::DistanceToBoundary(glm::vec2 worldPos) const {
    if (boundary.size() < 2) {
        // Fallback to distance from center if no boundary is defined
        float distance = glm::length(worldPos - centerPosition);
        float estimatedRadius = std::sqrt(area / 3.14159f);
        return estimatedRadius - distance;
    }
    
    float minDistance = std::numeric_limits<float>::max();
    
    // Calculate distance to each boundary edge
    for (size_t i = 0; i < boundary.size(); i++) {
        size_t j = (i + 1) % boundary.size();
        glm::vec2 edgeStart = boundary[i];
        glm::vec2 edgeEnd = boundary[j];
        
        // Distance from point to line segment
        glm::vec2 edgeVec = edgeEnd - edgeStart;
        glm::vec2 pointVec = worldPos - edgeStart;
        
        float edgeLength = glm::length(edgeVec);
        if (edgeLength > 0.0f) {
            float t = glm::clamp(glm::dot(pointVec, edgeVec) / (edgeLength * edgeLength), 0.0f, 1.0f);
            glm::vec2 projection = edgeStart + t * edgeVec;
            float distance = glm::length(worldPos - projection);
            minDistance = std::min(minDistance, distance);
        }
    }
    
    // Return negative distance if outside the plate
    bool inside = ContainsPosition(worldPos);
    return inside ? minDistance : -minDistance;
}

// Serialization methods
void TectonicPlate::Serialize(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    
    // Reserve space for the data
    buffer.resize(startSize + GetSerializedSize() + boundary.size() * 8);
    uint8_t* data = buffer.data() + startSize;
    size_t offset = 0;
    
    // Serialize basic data
    memcpy(data + offset, &plateId, sizeof(plateId)); offset += sizeof(plateId);
    memcpy(data + offset, &centerPosition, sizeof(centerPosition)); offset += sizeof(centerPosition);
    memcpy(data + offset, &movementVector, sizeof(movementVector)); offset += sizeof(movementVector);
    memcpy(data + offset, &area, sizeof(area)); offset += sizeof(area);
    memcpy(data + offset, &type, sizeof(type)); offset += sizeof(type);
    memcpy(data + offset, &density, sizeof(density)); offset += sizeof(density);
    memcpy(data + offset, &age, sizeof(age)); offset += sizeof(age);
    memcpy(data + offset, &stability, sizeof(stability)); offset += sizeof(stability);
    
    // Serialize boundary
    uint32_t boundarySize = static_cast<uint32_t>(boundary.size());
    memcpy(data + offset, &boundarySize, sizeof(boundarySize)); offset += sizeof(boundarySize);
    
    for (const auto& point : boundary) {
        memcpy(data + offset, &point, sizeof(point)); offset += sizeof(point);
    }
    
    // Adjust buffer size to actual used size
    buffer.resize(startSize + offset);
}

void TectonicPlate::Deserialize(const uint8_t* data, size_t& offset) {
    // Deserialize basic data
    memcpy(&plateId, data + offset, sizeof(plateId)); offset += sizeof(plateId);
    memcpy(&centerPosition, data + offset, sizeof(centerPosition)); offset += sizeof(centerPosition);
    memcpy(&movementVector, data + offset, sizeof(movementVector)); offset += sizeof(movementVector);
    memcpy(&area, data + offset, sizeof(area)); offset += sizeof(area);
    memcpy(&type, data + offset, sizeof(type)); offset += sizeof(type);
    memcpy(&density, data + offset, sizeof(density)); offset += sizeof(density);
    memcpy(&age, data + offset, sizeof(age)); offset += sizeof(age);
    memcpy(&stability, data + offset, sizeof(stability)); offset += sizeof(stability);
    
    // Deserialize boundary
    uint32_t boundarySize;
    memcpy(&boundarySize, data + offset, sizeof(boundarySize)); offset += sizeof(boundarySize);
    
    boundary.clear();
    boundary.reserve(boundarySize);
    
    for (uint32_t i = 0; i < boundarySize; i++) {
        glm::vec2 point;
        memcpy(&point, data + offset, sizeof(point)); offset += sizeof(point);
        boundary.push_back(point);
    }
}

size_t TectonicPlate::GetSerializedSize() {
    return sizeof(uint32_t) +      // plateId
           sizeof(glm::vec2) +     // centerPosition
           sizeof(glm::vec2) +     // movementVector
           sizeof(float) +         // area
           sizeof(PlateType) +     // type
           sizeof(float) +         // density
           sizeof(float) +         // age
           sizeof(float) +         // stability
           sizeof(uint32_t);       // boundary size (dynamic boundary data added separately)
}

// Utility functions
const char* PlateTypeToString(PlateType type) {
    switch (type) {
        case PlateType::OCEANIC:     return "Oceanic";
        case PlateType::CONTINENTAL: return "Continental";
        case PlateType::MICROPLATE:  return "Microplate";
        default:                     return "Unknown";
    }
}

PlateType StringToPlateType(const std::string& str) {
    if (str == "Oceanic") return PlateType::OCEANIC;
    if (str == "Continental") return PlateType::CONTINENTAL;
    if (str == "Microplate") return PlateType::MICROPLATE;
    return PlateType::CONTINENTAL; // Default fallback
}

const char* TerrainTypeToString(TerrainType type) {
    switch (type) {
        case TerrainType::STABLE:   return "Stable";
        case TerrainType::MOUNTAIN: return "Mountain";
        case TerrainType::RIFT:     return "Rift";
        case TerrainType::OCEANIC:  return "Oceanic";
        case TerrainType::FAULT:    return "Fault";
        default:                    return "Unknown";
    }
}

TerrainType StringToTerrainType(const std::string& str) {
    if (str == "Stable") return TerrainType::STABLE;
    if (str == "Mountain") return TerrainType::MOUNTAIN;
    if (str == "Rift") return TerrainType::RIFT;
    if (str == "Oceanic") return TerrainType::OCEANIC;
    if (str == "Fault") return TerrainType::FAULT;
    return TerrainType::STABLE; // Default fallback
}

// Calculate relative density for plate interactions
float CalculatePlateDensity(PlateType type, float age) {
    float baseDensity;
    
    switch (type) {
        case PlateType::OCEANIC:
            baseDensity = 3.0f;  // Oceanic crust is denser
            break;
        case PlateType::CONTINENTAL:
            baseDensity = 2.7f;  // Continental crust is lighter
            break;
        case PlateType::MICROPLATE:
            baseDensity = 2.85f; // Intermediate density
            break;
        default:
            baseDensity = 2.7f;
            break;
    }
    
    // Older oceanic crust becomes denser due to cooling
    if (type == PlateType::OCEANIC) {
        float ageFactor = std::min(age / 200.0f, 1.0f); // Max age factor at 200 million years
        baseDensity += ageFactor * 0.2f; // Up to 0.2 g/cm³ increase
    }
    
    return baseDensity;
}

// Generate typical movement vector for plate type
glm::vec2 GenerateTypicalMovement(PlateType type, uint32_t seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    
    // Create 16 preferred global movement directions for maximum diversity
    float globalDirections[16] = {
        0.0f,           // East
        3.14159f / 8,   // ENE  
        3.14159f / 4,   // Northeast  
        3 * 3.14159f / 8, // NNE
        3.14159f / 2,   // North
        5 * 3.14159f / 8, // NNW
        3 * 3.14159f / 4, // Northwest
        7 * 3.14159f / 8, // WNW
        3.14159f,       // West
        9 * 3.14159f / 8, // WSW
        5 * 3.14159f / 4, // Southwest
        11 * 3.14159f / 8, // SSW
        3 * 3.14159f / 2, // South
        13 * 3.14159f / 8, // SSE
        7 * 3.14159f / 4, // Southeast
        15 * 3.14159f / 8  // ESE
    };
    
    // Assign plate to one of 16 global directions based on seed
    int directionIndex = seed % 16;
    float baseAngle = globalDirections[directionIndex];
    
    // Add significant random variation around the base direction
    std::uniform_real_distribution<float> variationDist(-1.2f, 1.2f); // ±70 degrees for major variation
    float angle = baseAngle + variationDist(rng);
    
    // Create opposing plate movements for dramatic boundaries
    if (seed % 2 == 0) {
        angle += 3.14159f; // Reverse direction for 50% of plates
    }
    
    // Add additional random rotation for maximum chaos
    std::uniform_real_distribution<float> chaosAngleDist(0.0f, 2.0f * 3.14159f);
    if (seed % 4 == 1) {
        angle = chaosAngleDist(rng); // Completely random direction for 25% of plates
    }
    
    float speed;
    std::uniform_real_distribution<float> speedDist;
    
    switch (type) {
        case PlateType::OCEANIC:
            speedDist = std::uniform_real_distribution<float>(8.0f, 25.0f); // Much faster, more variable
            break;
        case PlateType::CONTINENTAL:
            speedDist = std::uniform_real_distribution<float>(2.0f, 15.0f);  // More variable speed range
            break;
        case PlateType::MICROPLATE:
            speedDist = std::uniform_real_distribution<float>(1.0f, 35.0f); // Extremely variable speed
            break;
        default:
            speedDist = std::uniform_real_distribution<float>(3.0f, 12.0f);
            break;
    }
    
    speed = speedDist(rng);
    
    // Add speed multiplier based on seed for extreme variations
    if (seed % 7 == 0) {
        speed *= 2.5f; // Very fast plates for 1/7 of plates
    } else if (seed % 11 == 0) {
        speed *= 0.3f; // Very slow plates for some boundaries
    }
    
    // Convert from cm/year to km/million years
    speed *= 10.0f; // 1 cm/year = 10 km/million years
    
    glm::vec2 movement = glm::vec2(std::cos(angle) * speed, std::sin(angle) * speed);
    
    // Debug output for first few plates to verify movement diversity
    if (seed % 100 < 12) {
        std::cout << "[GenerateTypicalMovement] Plate " << seed 
                  << " type=" << static_cast<int>(type) 
                  << " direction=" << directionIndex
                  << " baseAngle=" << baseAngle 
                  << " finalAngle=" << angle 
                  << " speed=" << speed 
                  << " movement=(" << movement.x << "," << movement.y << ")" << std::endl;
    }
    
    return movement;
}

} // namespace VoxelCastle
