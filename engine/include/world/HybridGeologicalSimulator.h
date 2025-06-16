#pragma once

#include "world/GeologicalSimulator.h"
#include "world/ParticleSimulationEngine.h"
#include "world/FractalDetailEngine.h"
#include "world/GeologicalTypes.h"  // For PhaseInfo and GeologicalSample
#include <memory>
#include <functional>

namespace VoxelCastle {
namespace World {

// Forward declarations
class FractalContinentGenerator;

/**
 * @brief Hybrid geological simulator combining particle-based tectonics and fractal detail
 * 
 * This class orchestrates both the ParticleSimulationEngine for realistic geological
 * forces and the FractalDetailEngine for seamless terrain detail generation.
 * Replaces the old grid-based system with a continuous, artifact-free approach.
 */
class HybridGeologicalSimulator {
public:
    /**
     * @brief Constructor
     * @param worldSizeKm World size in kilometers
     * @param seed Random seed for reproducible results
     */
    HybridGeologicalSimulator(float worldSizeKm, uint64_t seed);
    
    /**
     * @brief Destructor
     */
    ~HybridGeologicalSimulator();
    
    /**
     * @brief Initialize the hybrid simulation system
     * @param continentCount Number of continental masses
     * @param oceanRatio Fraction of world that should be ocean (0.0-1.0)
     */
    void Initialize(int continentCount = 4, float oceanRatio = 0.71f);
    
    /**
     * @brief Run one simulation step
     * @param timeStepYears Time step in years
     */
    void RunSimulationStep(float timeStepYears);
    
    /**
     * @brief Get elevation at any world coordinate (seamless sampling)
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @return Terrain elevation with particle + fractal detail
     */
    float GetElevationAt(float worldX, float worldZ);
    
    /**
     * @brief Get elevation at specific resolution/LOD
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param resolution Detail resolution in meters (larger = coarser detail)
     * @return Terrain elevation at specified resolution
     */
    float GetElevationAtResolution(float worldX, float worldZ, float resolution);
    
    /**
     * @brief Get complete geological data at any coordinate
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @return Complete geological information
     */
    ParticleSimulationEngine::GeologicalData GetGeologicalDataAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get simulation progress (0.0 to 1.0)
     */
    float GetProgress() const;
    
    /**
     * @brief Check if simulation is complete
     */
    bool IsComplete() const;
    
    /**
     * @brief Check if simulation is complete (alias for IsComplete)
     */
    bool IsSimulationComplete() const { return IsComplete(); }
    
    /**
     * @brief Reset simulation to initial state
     */
    void Reset();
    
    /**
     * @brief Get simulation statistics for debugging
     */
    void GetSimulationStats(size_t& particleCount, float& cacheHitRatio) const;
    
    /**
     * @brief Snapshot system compatibility methods
     */
    GeologicalSnapshotManager* getSnapshotManager();
    const GeologicalSnapshotManager* getSnapshotManager() const;
    
    /**
     * @brief Step-based simulation methods for UI compatibility
     */
    void stepSimulation(float timeStepYears);
    void stepSimulation(float timeStepYears, const PhaseInfo& phaseInfo);
    bool InitializeSimulation();
    bool StepSimulation(); 
    void PauseSimulation();
    void ResumeSimulation(); 
    bool IsSimulationPaused() const;
    
    /**
     * @brief Get geological sample at world coordinates
     */
    GeologicalSample getSampleAt(float worldX, float worldZ) const;
    GeologicalSample GetSampleAt(float worldX, float worldZ) const;
    
    /**
     * @brief Run full simulation with progress callback
     */
    void runFullSimulation(std::function<void(const PhaseInfo&)> progressCallback);
    
    /**
     * @brief Set particle/fractal balance for terrain generation
     * @param particleWeight Weight of particle simulation (0.0-1.0)
     * @param fractalWeight Weight of fractal detail (0.0-1.0)
     */
    void SetDetailBalance(float particleWeight, float fractalWeight);
    
    /**
     * @brief Step simulation methods for compatibility
     */
    void stepGeologicalSimulation();
    bool isGeologicalSimulationComplete() const;
    
    /**
     * @brief Background simulation control methods
     */
    void startBackgroundSimulation();
    bool isBackgroundSimulationRunning() const;
    void pauseBackgroundSimulation();
    void resumeBackgroundSimulation();
    void stopBackgroundSimulation();
    float getBackgroundProgress() const;
    std::shared_ptr<class GeologicalSnapshot> getLatestSnapshot();

    /**
     * @brief Get fractal continent generator (for UI compatibility)
     * @return Fractal continent generator or nullptr
     */
    const FractalContinentGenerator* getFractalContinentGenerator() const;

private:
    // Core simulation engines
    std::unique_ptr<ParticleSimulationEngine> particleEngine_;
    std::unique_ptr<FractalDetailEngine> fractalEngine_;
    
    // Simulation state
    float currentTime_;             // Current simulation time (years)
    float targetTime_;              // Target simulation time (years)
    bool isInitialized_;
    
    // Detail balance parameters
    float particleWeight_;          // Weight of particle-based elevation
    float fractalWeight_;           // Weight of fractal detail
    
    // Performance parameters
    static constexpr float DEFAULT_TIME_STEP = 1000.0f;    // 1000 years per step
    static constexpr float SIMULATION_DURATION = 100000.0f; // 100,000 years total
    static constexpr float DEFAULT_PARTICLE_WEIGHT = 0.7f;  // 70% particle influence
    static constexpr float DEFAULT_FRACTAL_WEIGHT = 0.3f;   // 30% fractal detail
    
    /**
     * @brief Combine particle and fractal data for seamless terrain
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param resolution Detail resolution (optional)
     * @return Combined elevation value
     */
    float CombineParticleAndFractalData(float worldX, float worldZ, float resolution = 100.0f);
    
    /**
     * @brief Build geological context from particle data
     * @param particleData Geological data from particle simulation
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @return Geological context for fractal generation
     */
    GeologicalContext BuildGeologicalContext(const ParticleSimulationEngine::GeologicalData& particleData,
                                            float worldX, float worldZ) const;
    
    /**
     * @brief Calculate distance to nearest coastline
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @return Distance to coast in meters
     */
    float CalculateDistanceToCoast(float worldX, float worldZ) const;
    
    /**
     * @brief Calculate continental proximity (0.0 = oceanic, 1.0 = continental)
     * @param elevation Current elevation
     * @param crustalThickness Local crustal thickness
     * @return Continental proximity value
     */
    float CalculateContinentalProximity(float elevation, float crustalThickness) const;
};

} // namespace World
} // namespace VoxelCastle
