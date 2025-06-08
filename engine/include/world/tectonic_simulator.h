#pragma once

#include "world/tectonic_plate.h"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <memory>

namespace VoxelCastle {

/**
 * @brief Plate boundary interaction types
 */
enum class BoundaryType : uint8_t {
    CONVERGENT,     // Plates moving toward each other (mountains/subduction)
    DIVERGENT,      // Plates moving apart (rifts/seafloor spreading)
    TRANSFORM,      // Plates sliding past each other (fault lines)
    PASSIVE         // No significant interaction
};

/**
 * @brief Information about plate boundary interactions
 */
struct PlateBoundary {
    uint32_t plate1Id;          // First plate ID
    uint32_t plate2Id;          // Second plate ID
    BoundaryType type;          // Type of boundary interaction
    std::vector<glm::vec2> contactPoints; // Points where plates interact
    float stress;               // Accumulated stress at boundary
    float interactionStrength;  // Strength of plate interaction (0.0-1.0)
    
    PlateBoundary(uint32_t p1, uint32_t p2, BoundaryType t)
        : plate1Id(p1), plate2Id(p2), type(t), stress(0.0f), interactionStrength(0.0f) {}
};

/**
 * @brief Tectonic simulation system for generating realistic geological features
 * 
 * This class simulates tectonic plate movement and interactions to generate
 * mountain ranges, rift valleys, fault lines, and other geological features
 * that serve as the foundation for erosion and hydrology systems.
 */
class TectonicSimulator {
public:
    /**
     * @brief Constructor
     */
    TectonicSimulator();
    
    /**
     * @brief Destructor
     */
    ~TectonicSimulator();
    
    /**
     * @brief Initialize plates for a given world
     * @param seed Random seed for deterministic generation
     * @param worldSize World size in kilometers (square world)
     * @param plateCount Desired number of plates (0 = auto-determine from world size)
     */
    void InitializePlates(uint32_t seed, float worldSize, uint32_t plateCount = 0);
    
    /**
     * @brief Execute plate movement simulation
     * @param simulationTime Time to simulate in million years (default: 100)
     * @param timeSteps Number of simulation steps (default: 10)
     */
    void SimulatePlateMovement(float simulationTime = 100.0f, uint32_t timeSteps = 10);
    
    /**
     * @brief Generate terrain features from plate boundary interactions
     */
    void GenerateBoundaryFeatures();
    
    /**
     * @brief Get all plates in the simulation
     * @return Reference to vector of all plates
     */
    const std::vector<TectonicPlate>& GetPlates() const { return plates_; }
    
    /**
     * @brief Get all plate boundaries
     * @return Reference to vector of all boundaries
     */
    const std::vector<PlateBoundary>& GetBoundaries() const { return boundaries_; }
    
    /**
     * @brief Get terrain type at a specific world position
     * @param worldPos Position in world coordinates (km)
     * @return Terrain type based on tectonic activity
     */
    TerrainType GetTerrainTypeAtPosition(glm::vec2 worldPos) const;
    
    /**
     * @brief Get tectonic stress level at a position
     * @param worldPos Position in world coordinates (km)
     * @return Stress level (0.0 = stable, 1.0 = maximum stress)
     */
    float GetTectonicStress(glm::vec2 worldPos) const;
    
    /**
     * @brief Get elevation modifier from tectonic activity
     * @param worldPos Position in world coordinates (km)
     * @return Elevation modifier in meters (can be negative for rifts)
     */
    float GetElevationModifier(glm::vec2 worldPos) const;
    
    /**
     * @brief Get the dominant plate at a position
     * @param worldPos Position in world coordinates (km)
     * @return Pointer to plate containing the position, nullptr if none
     */
    const TectonicPlate* GetDominantPlate(glm::vec2 worldPos) const;
    
    /**
     * @brief Check if simulation has been completed
     * @return True if simulation is complete and results are available
     */
    bool IsSimulationComplete() const { return simulationComplete_; }
    
    /**
     * @brief Get world size
     * @return World size in kilometers
     */
    float GetWorldSize() const { return worldSize_; }
    
    /**
     * @brief Reset simulation to initial state
     */
    void Reset();

private:
    // Core simulation data
    std::vector<TectonicPlate> plates_;
    std::vector<PlateBoundary> boundaries_;
    uint32_t seed_;
    float worldSize_;
    bool simulationComplete_;
    
    // Simulation parameters
    float timeStep_;              // Current simulation time step
    float totalSimulationTime_;   // Total time simulated so far
    
    // Spatial optimization data structures
    struct SpatialGrid {
        std::vector<std::vector<uint32_t>> cells; // Grid cells containing plate IDs
        float cellSize;
        uint32_t gridWidth, gridHeight;
    };
    std::unique_ptr<SpatialGrid> spatialGrid_;
    
    // Stress and terrain maps
    std::vector<std::vector<float>> stressMap_;
    std::vector<std::vector<TerrainType>> terrainMap_;
    std::vector<std::vector<float>> elevationMap_;
    uint32_t mapResolution_;
    
    // Internal simulation methods
    
    /**
     * @brief Generate initial plate configuration
     */
    void GeneratePlates();
    
    /**
     * @brief Create Voronoi-based plate boundaries
     */
    void GenerateVoronoiBoundaries();
    
    /**
     * @brief Build spatial grid for fast plate queries
     */
    void BuildSpatialGrid();
    
    /**
     * @brief Detect collisions between plates
     */
    void DetectPlateCollisions();
    
    /**
     * @brief Handle convergent boundary (plates moving toward each other)
     */
    void HandleConvergentBoundary(PlateBoundary& boundary);
    
    /**
     * @brief Handle divergent boundary (plates moving apart)
     */
    void HandleDivergentBoundary(PlateBoundary& boundary);
    
    /**
     * @brief Handle transform boundary (plates sliding past each other)
     */
    void HandleTransformBoundary(PlateBoundary& boundary);
    
    /**
     * @brief Calculate stress accumulation at boundaries
     */
    void CalculateStressAccumulation(float deltaTime);
    
    /**
     * @brief Update plate positions based on movement vectors
     */
    void UpdatePlatePositions(float deltaTime);
    
    /**
     * @brief Generate terrain maps from plate interactions
     */
    void GenerateTerrainMaps();
    
    /**
     * @brief Classify boundary type based on plate movement
     */
    BoundaryType ClassifyBoundaryType(const TectonicPlate& plate1, const TectonicPlate& plate2) const;
    
    /**
     * @brief Calculate interaction strength between two plates
     */
    float CalculateInteractionStrength(const TectonicPlate& plate1, const TectonicPlate& plate2) const;
    
    /**
     * @brief Get optimal number of plates for world size
     */
    uint32_t GetOptimalPlateCount(float worldSize) const;
    
    /**
     * @brief Interpolate stress at position using spatial grid
     */
    float InterpolateStress(glm::vec2 worldPos) const;
    
    /**
     * @brief Interpolate terrain type at position using spatial grid
     */
    TerrainType InterpolateTerrainType(glm::vec2 worldPos) const;
    
    /**
     * @brief Interpolate elevation modifier at position using spatial grid
     */
    float InterpolateElevationModifier(glm::vec2 worldPos) const;
};

} // namespace VoxelCastle
