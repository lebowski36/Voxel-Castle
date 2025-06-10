#pragma once

#include "world/ContinuousField.h"
#include "world/GeologicalTypes.h"
#include <memory>
#include <functional>
#include <chrono>
#include <random>  // For std::mt19937

namespace VoxelCastle {
namespace World {

/**
 * @brief Advanced geological simulator that replaces the discrete plate system
 * 
 * This class implements the three-phase geological simulation:
 * 1. Tectonic Phase: Continental drift, mountain building (millions of years)
 * 2. Erosion Phase: Water systems, weathering (thousands of years)  
 * 3. Detail Phase: Micro-scale features (hundreds of years)
 * 
 * The simulator uses continuous fields to eliminate grid artifacts and create
 * organic, realistic terrain with proper geological processes.
 */
class GeologicalSimulator {
private:
    // Continuous geological fields
    std::unique_ptr<ContinuousField<float>> mantleStress_;
    std::unique_ptr<ContinuousField<float>> crustStress_;
    std::unique_ptr<ContinuousField<float>> elevationField_;
    std::unique_ptr<ContinuousField<float>> rockHardness_;
    std::unique_ptr<ContinuousField<float>> waterFlow_;
    std::unique_ptr<ContinuousField<RockType>> rockTypes_;
    
    // Configuration
    GeologicalConfig config_;
    int worldSize_;
    float worldSizeKm_;
    uint64_t seed_;
    
    // Progress tracking
    std::function<void(const PhaseInfo&)> progressCallback_;
    std::chrono::steady_clock::time_point startTime_;
    float totalSimulationTime_;
    
    // Current simulation state
    GeologicalPhase currentPhase_;
    float currentPhaseProgress_;
    PerformanceMetrics metrics_;
    
public:
    /**
     * @brief Constructor
     * @param worldSize World size in chunks
     * @param config Geological configuration
     */
    GeologicalSimulator(int worldSize, const GeologicalConfig& config);
    
    /**
     * @brief Destructor
     */
    ~GeologicalSimulator() = default;
    
    /**
     * @brief Initialize the simulator with a seed
     * @param seed Random seed for deterministic generation
     */
    void initialize(uint64_t seed);
    
    /**
     * @brief Run the complete three-phase geological simulation
     * @param progressCallback Optional callback for progress updates
     */
    void runFullSimulation(std::function<void(const PhaseInfo&)> progressCallback = nullptr);
    
    /**
     * @brief Run individual simulation phases
     */
    void simulateTectonicPhase(float durationMillionYears);
    void simulateErosionPhase(float durationThousandYears);
    void simulateDetailPhase(float durationHundredYears);
    
    /**
     * @brief Get geological sample at a specific world position
     * @param x World X coordinate in meters
     * @param z World Z coordinate in meters
     * @return Geological sample data
     */
    GeologicalSample getSampleAt(float x, float z) const;
    
    /**
     * @brief Export geological data for chunk generation
     * @return Geological sample field for entire world
     */
    std::unique_ptr<ContinuousField<GeologicalSample>> exportGeologicalData() const;
    
    /**
     * @brief Get current simulation progress
     */
    PhaseInfo getProgressInfo() const;
    
    /**
     * @brief Set progress callback for real-time updates
     */
    void setProgressCallback(std::function<void(const PhaseInfo&)> callback);
    
    /**
     * @brief Get performance metrics
     */
    const PerformanceMetrics& getPerformanceMetrics() const { return metrics_; }
    
private:
    /**
     * @brief Initialize geological fields with base values
     */
    void initializeFields();
    
    /**
     * @brief Tectonic simulation methods
     */
    void simulateMantleConvection(float timeStep);
    void simulatePlateMovement(float timeStep);
    void simulateMountainBuilding(float timeStep);
    void simulateVolcanicActivity(float timeStep);
    
    /**
     * @brief Erosion simulation methods
     */
    void simulateChemicalWeathering(float timeStep);
    void simulatePhysicalErosion(float timeStep);
    void simulateRiverSystems(float timeStep);
    void simulateGlacialCarving(float timeStep);
    
    /**
     * @brief Detail simulation methods
     */
    void simulateMicroWeathering(float timeStep);
    void simulateSedimentDeposition(float timeStep);
    void simulateJointFormation(float timeStep);
    void simulateCaveGeneration(float timeStep);
    
    /**
     * @brief Utility methods
     */
    void updateProgress(float phaseProgress, const std::string& processName);
    void updatePerformanceMetrics();
    float calculateResistance(float x, float z) const;
    RockType determineRockType(float elevation, float stress, float temperature) const;
    
    /**
     * @brief Random number generation
     */
    std::mt19937 rng_;
    float random01();
    float randomRange(float min, float max);
};

} // namespace World
} // namespace VoxelCastle
