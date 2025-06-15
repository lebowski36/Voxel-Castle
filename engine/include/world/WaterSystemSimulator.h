#pragma once

#include "world/ContinuousField.h"
#include "world/GeologicalTypes.h"
#include <memory>
#include <random>

namespace VoxelCastle {
namespace World {

/**
 * @brief Handles all water-related geological processes
 * 
 * This class encapsulates all water system simulation including:
 * - Precipitation patterns
 * - Surface water accumulation 
 * - River formation and flow
 * - Groundwater table simulation
 * - Aquifer systems
 * - Spring formation
 * - Cave water interactions
 */
class WaterSystemSimulator {
public:
    /**
     * @brief Constructor
     * @param worldSizeKm World size in kilometers
     * @param config Geological configuration
     */
    WaterSystemSimulator(float worldSizeKm, const GeologicalConfig& config);
    
    /**
     * @brief Initialize water system fields
     * @param elevationField Reference to elevation field
     * @param rockTypes Reference to rock types field
     * @param seed Random seed for water generation
     */
    void Initialize(
        std::shared_ptr<ContinuousField<float>> elevationField,
        std::shared_ptr<ContinuousField<RockType>> rockTypes,
        uint64_t seed
    );
    
    // === Water Process Simulation Methods ===
    
    /**
     * @brief Simulate precipitation patterns based on elevation and geography
     */
    void SimulatePrecipitationPatterns(float timeStep);
    
    /**
     * @brief Simulate surface water accumulation and flow
     */
    void SimulateSurfaceWaterAccumulation(float timeStep);
    
    /**
     * @brief Simulate river formation from terrain analysis
     */
    void SimulateRiverFormation(float timeStep);
    
    /**
     * @brief Simulate existing river systems
     */
    void SimulateRiverSystems(float timeStep);
    
    /**
     * @brief Simulate groundwater table dynamics
     */
    void SimulateGroundwaterTable(float timeStep);
    
    /**
     * @brief Simulate aquifer recharge from precipitation
     */
    void SimulateAquiferRecharge(float timeStep);
    
    /**
     * @brief Simulate groundwater flow
     */
    void SimulateGroundwaterFlow(float timeStep);
    
    /**
     * @brief Simulate spring formation where aquifers meet surface
     */
    void SimulateSpringFormation(float timeStep);
    
    /**
     * @brief Simulate flood plain formation
     */
    void SimulateFloodPlains(float timeStep);
    
    /**
     * @brief Simulate lake formation in basins
     */
    void SimulateLakeFormation(float timeStep);
    
    /**
     * @brief Simulate wetland formation
     */
    void SimulateWetlandFormation(float timeStep);
    
    /**
     * @brief Simulate cave water interactions
     */
    void SimulateCaveWaterInteractions(float timeStep);
    
    // === Data Access Methods ===
    
    /**
     * @brief Get precipitation field for visualization
     */
    std::shared_ptr<ContinuousField<float>> GetPrecipitationField() const { 
        return precipitationField_; 
    }
    
    /**
     * @brief Get surface water depth field
     */
    std::shared_ptr<ContinuousField<float>> GetSurfaceWaterField() const { 
        return surfaceWaterDepth_; 
    }
    
    /**
     * @brief Get groundwater table field
     */
    std::shared_ptr<ContinuousField<float>> GetGroundwaterField() const { 
        return groundwaterTable_; 
    }
    
    /**
     * @brief Get spring flow field
     */
    std::shared_ptr<ContinuousField<float>> GetSpringFlowField() const { 
        return springFlow_; 
    }

private:
    // Configuration
    float worldSizeKm_;
    GeologicalConfig config_;
    uint64_t seed_;
    
    // External references
    std::shared_ptr<ContinuousField<float>> elevationField_;
    std::shared_ptr<ContinuousField<RockType>> rockTypes_;
    
    // Water system fields
    std::shared_ptr<ContinuousField<float>> precipitationField_;     // Precipitation patterns
    std::shared_ptr<ContinuousField<float>> groundwaterTable_;       // Groundwater table depth
    std::shared_ptr<ContinuousField<float>> permeabilityField_;      // Rock permeability
    std::shared_ptr<ContinuousField<float>> surfaceWaterDepth_;      // Surface water
    std::shared_ptr<ContinuousField<float>> sedimentLoad_;           // Sediment transport
    std::shared_ptr<ContinuousField<float>> springFlow_;             // Spring discharge
    std::shared_ptr<ContinuousField<float>> waterFlow_;              // Water flow direction/velocity
    
    // Helper methods
    float CalculatePrecipitation(float x, float z, float elevation) const;
    float CalculatePermeability(RockType rockType) const;
    void UpdateWaterFlow(float timeStep);
    void ProcessSedimentTransport(float timeStep);
    
    // Random number generation
    mutable std::mt19937 rng_;
    float Random01() const;
    float RandomRange(float min, float max) const;
};

/**
 * @brief Water data structure for geological samples
 */
struct WaterData {
    float precipitation{0.0f};      // Annual precipitation (mm)
    float surfaceWater{0.0f};       // Surface water depth (m)
    float groundwaterDepth{10.0f};  // Depth to groundwater table (m)
    float springFlow{0.0f};         // Spring discharge rate (L/s)
    float sedimentLoad{0.0f};       // Suspended sediment (kg/m³)
    bool hasRivers{false};          // River presence
    bool hasLakes{false};           // Lake presence
    bool hasWetlands{false};        // Wetland presence
    bool hasNaturalSprings{false};  // Spring presence
};

} // namespace World
} // namespace VoxelCastle
