#pragma once

#include "world/ContinuousField.h"
#include "world/GeologicalTypes.h"
#include "world/FractalContinentGenerator.h"
#include "world/GeologicalSnapshot.h"
#include "world/BackgroundSimulationEngine.h"
#include "world/WaterSystemSimulator.h"
#include "world/TectonicEngine.h"
#include "world/ErosionEngine.h"
#include <memory>
#include <functional>
#include <chrono>
#include <random>  // For std::mt19937
#include <thread>  // For background thread support
#include <mutex>   // For thread-safe snapshot access
#include <atomic>  // For atomic progress tracking
#include <queue>   // For snapshot buffering
#include <vector>  // For process queue

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
    
    // Step 3: Water & Cave System Framework - New fields
    std::unique_ptr<ContinuousField<float>> precipitationField_;     // Precipitation patterns
    std::unique_ptr<ContinuousField<float>> groundwaterTable_;       // Groundwater table depth
    std::unique_ptr<ContinuousField<float>> permeabilityField_;      // Rock permeability for groundwater flow
    std::unique_ptr<ContinuousField<float>> surfaceWaterDepth_;      // Surface water accumulation (rivers/lakes)
    std::unique_ptr<ContinuousField<float>> sedimentLoad_;           // Sediment transported by water
    std::unique_ptr<ContinuousField<float>> caveNetworkDensity_;     // Underground cave network density
    std::unique_ptr<ContinuousField<float>> springFlow_;             // Spring discharge rates
    
    // Step 4: Physics-Based Geological Engine - New fields  
    std::unique_ptr<ContinuousField<float>> crustalThickness_;       // Crustal thickness for isostasy
    std::unique_ptr<ContinuousField<float>> mantleTemperature_;      // Mantle temperature for convection
    std::unique_ptr<ContinuousField<float>> isostasyAdjustment_;     // Isostatic adjustment forces
    std::unique_ptr<ContinuousField<float>> erosionRateField_;       // Spatially varying erosion rates
    
    // Configuration
    GeologicalConfig config_;
    float worldSizeKm_;  // World size in kilometers
    uint64_t seed_;
    
    // Progress tracking
    std::function<void(const PhaseInfo&)> progressCallback_;
    std::chrono::steady_clock::time_point startTime_;
    float totalSimulationTime_;
    
    // Current simulation state
    GeologicalPhase currentPhase_;
    float currentPhaseProgress_;
    PerformanceMetrics metrics_;
    
    // Fractal continental generation system (initialized after other members)
    FractalContinentGenerator continentGenerator_;
    
    // Snapshot system for UI visualization
    std::unique_ptr<GeologicalSnapshotManager> snapshotManager_;
    
    // Step-based simulation state
    bool simulationInitialized_;
    bool simulationComplete_;
    bool simulationPaused_;
    bool hasResumedSincePause_; // <<< ADDED
    int currentStep_;
    int totalSteps_;
    
    // Current phase state
    int phaseStep_;
    int totalPhaseSteps_;
    float phaseTimeStep_;
    
    // Step 4.1: Interleaved Process Time Scaling
    struct ProcessTimeScales {
        float tectonicTimeStep;    // 1000 years per step
        float erosionTimeStep;     // 100 years per step  
        float waterTimeStep;       // 10 years per step
        float detailTimeStep;      // 1 year per step
        float volcanicTimeStep;    // 5000 years per step
    } processTimeScales_;
    
    // Timing for responsive updates
    std::chrono::steady_clock::time_point lastSnapshotTime_;
    static constexpr float SNAPSHOT_INTERVAL_SECONDS = 2.0f;  // Create snapshot every 2 seconds
    
    // Phase A: Background Thread Infrastructure for UI Responsiveness
    std::unique_ptr<BackgroundSimulationEngine> backgroundEngine_;  // Background simulation engine
    std::unique_ptr<WaterSystemSimulator> waterSystem_;            // Water system simulator
    std::unique_ptr<TectonicEngine> tectonicEngine_;               // Tectonic processes engine
    std::unique_ptr<ErosionEngine> erosionEngine_;                 // Erosion and weathering engine
    
    // Background execution control
    bool useBackgroundExecution_{false};                          // Enable/disable background mode
    void createBackgroundSnapshot();                       // Create snapshot for UI consumption
    std::shared_ptr<GeologicalSnapshot> consumeLatestSnapshot(); // Get latest snapshot from background thread
    
public:
    /**
     * @brief Constructor
     * @param worldSizeKm World size in kilometers
     * @param config Geological configuration
     */
    GeologicalSimulator(int worldSizeKm, const GeologicalConfig& config);
    
    /**
     * @brief Destructor
     */
    ~GeologicalSimulator();
    
    // Phase A: Background Thread Control Methods
    void startBackgroundSimulation();                    // Start simulation in background thread
    void stopBackgroundSimulation();                     // Stop background simulation and join thread
    bool isBackgroundSimulationRunning() const;          // Check if background simulation is active
    void enableBackgroundExecution(bool enable = true);  // Enable/disable background mode
    
    // Thread-safe snapshot access for UI
    std::shared_ptr<GeologicalSnapshot> getLatestSnapshot(); // Get latest snapshot for UI rendering
    float getBackgroundProgress() const;                 // Get current progress (thread-safe)
    
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
    
    // Step-based simulation for responsive UI
    bool initializeSimulation();                    // Initialize simulation state
    bool stepSimulation();                          // Run one simulation step
    bool isSimulationComplete() const;              // Check if simulation is done
    void pauseSimulation();                         // Pause/resume simulation
    void resumeSimulation();
    bool isSimulationPaused() const;
    
    // === Background Execution Methods (Step 4.1.2) ===
    
    /**
     * @brief Enable background simulation for responsive UI
     * @param enable True to enable background mode, false for foreground
     */
    void setBackgroundExecution(bool enable);
    
    /**
     * @brief Get the next snapshot from background simulation
     * @return Shared pointer to snapshot, or nullptr if none available
     */
    std::shared_ptr<GeologicalSnapshot> getNextBackgroundSnapshot();
    
    /**
     * @brief Check if background snapshots are available
     */
    bool hasBackgroundSnapshots() const;
    
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
    
    /**
     * @brief Get fractal continent generator for preview visualization
     */
    const FractalContinentGenerator* getFractalContinentGenerator() const { return &continentGenerator_; }
    
    /**
     * @brief Snapshot system methods for UI visualization
     */
    const GeologicalSnapshotManager* getSnapshotManager() const { return snapshotManager_.get(); }
    GeologicalSnapshotManager* getSnapshotManager() { return snapshotManager_.get(); }
    
    /**
     * @brief Force creation of a new snapshot at current simulation state
     */
    void createSnapshot(const std::string& phaseDescription, float completionPercentage);
    
    /**
     * @brief Get elevation data directly from current snapshot (for UI optimization)
     */
    float getSnapshotElevationAt(float x, float z) const;
    
    // Snapshot management for UI preview
    void createSnapshot(const std::string& description = "");
    bool hasSnapshots() const;
    std::vector<std::string> getSnapshotDescriptions() const;
    bool setCurrentSnapshot(size_t index);
    size_t getCurrentSnapshotIndex() const;
    size_t getSnapshotCount() const;
    
    // Phase display utility
    std::string getPhaseDisplayName() const;
    
private:
    /**
     * @brief Initialize geological fields with base values
     */
    void initializeFields();
    
    /**
     * @brief CRITICAL: Safety function to enforce elevation bounds
     */
    void clampElevationSafe(int x, int z, const std::string& processName = "Unknown");
    
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
     * @brief Step 3: Surface Water Flow System Methods
     */
    void simulatePrecipitationPatterns(float timeStep);
    void simulateSurfaceWaterAccumulation(float timeStep);
    void simulateRiverFormation(float timeStep);
    void simulateFloodPlains(float timeStep);
    void simulateLakeFormation(float timeStep);
    
    /**
     * @brief Step 3: Groundwater Aquifer System Methods
     */
    void simulateGroundwaterTable(float timeStep);
    void simulateAquiferRecharge(float timeStep);
    void simulateGroundwaterFlow(float timeStep);
    void simulateSpringFormation(float timeStep);
    void simulateWetlandFormation(float timeStep);
    
    /**
     * @brief Step 3: Underground Cave Network Methods
     */
    void simulateCaveNetworkGrowth(float timeStep);
    void simulateUndergroundRivers(float timeStep);
    void simulateKarstWeathering(float timeStep);
    void simulateCaveCollapse(float timeStep);
    
    /**
     * @brief Step 3: Integrated Water-Erosion System Methods
     */
    void simulateSedimentTransport(float timeStep);
    void simulateWaterDrivenErosion(float timeStep);
    void simulateSedimentDeposition(float timeStep);
    void simulateErosionUpliftBalance(float timeStep);
    
    /**
     * @brief Step 4: Isostatic Equilibrium System Methods
     */
    void simulateCrustalBuoyancy(float timeStep);
    void simulateMantleResponse(float timeStep);
    void simulateIsostaticRebound(float timeStep);
    void simulateRegionalIsostaticAdjustment(float timeStep);
    void simulateGlacialIsostasy(float timeStep);
    void simulatePostGlacialRebound(float timeStep);
    
    /**
     * @brief Step 4: Steady-State Force System Methods
     */
    void simulateEquilibriumForceFields(float timeStep);
    void simulateTectonicForceDecay(float timeStep);
    void simulateCrustalStrengthLimits(float timeStep);
    void simulateRealisticFaultFormation(float timeStep);
    void simulateStressReleaseEarthquakes(float timeStep);
    
    /**
     * @brief Step 4: Erosion-Uplift Balance Methods
     */
    void simulateElevationDependentErosion(float timeStep);
    void simulateClimateErosionFeedback(float timeStep);
    void simulateUpliftErosionEquilibrium(float timeStep);
    void simulateTerrainStabilization(float timeStep);
    
    /**
     * @brief Detail simulation methods
     */
    void simulateMicroWeathering(float timeStep);
    void simulateJointFormation(float timeStep);
    void simulateCaveGeneration(float timeStep);
    
    /**
     * @brief Utility methods
     */
    void updateProgress(float phaseProgress, const std::string& processName);
    void updatePerformanceMetrics();
    float calculateResistance(float x, float z) const;
    RockType determineRockType(float elevation, float stress, float temperature) const;
    float getRockHardness(RockType rockType) const;
    void validateAndClampElevation(int x, int z, const std::string& processName);
    void validateEntireElevationField(const std::string& processName);
    
    /**
     * @brief Random number generation
     */
    std::mt19937 rng_;
    float random01();
    float randomRange(float min, float max);
};

} // namespace World
} // namespace VoxelCastle
