#include "world/GeologicalSimulator.h"
#include "util/noise.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

using namespace VoxelCastle::World;

namespace VoxelCastle {
namespace World {

GeologicalSimulator::GeologicalSimulator(int worldSizeKm, const GeologicalConfig& config)
    : config_(config), worldSizeKm_(static_cast<float>(worldSizeKm)), seed_(0), currentPhase_(GeologicalPhase::TECTONICS),
      currentPhaseProgress_(0.0f), continentGenerator_(0),
      simulationInitialized_(false), simulationComplete_(false), simulationPaused_(false),
      hasResumedSincePause_(false),
      currentStep_(0), totalSteps_(0), phaseStep_(0), totalPhaseSteps_(0), phaseTimeStep_(0.0f) {
    
    // Initialize interleaved process time scales
    processTimeScales_.tectonicTimeStep = 1000.0f;  // 1000 years per step
    processTimeScales_.erosionTimeStep = 100.0f;    // 100 years per step
    processTimeScales_.waterTimeStep = 10.0f;       // 10 years per step
    processTimeScales_.detailTimeStep = 1.0f;       // 1 year per step
    processTimeScales_.volcanicTimeStep = 5000.0f;  // 5000 years per step
    
    // Initialize modular engines
    backgroundEngine_ = std::make_unique<BackgroundSimulationEngine>(this);
    waterSystem_ = std::make_unique<WaterSystemSimulator>(worldSizeKm_, config);
    tectonicEngine_ = std::make_unique<TectonicEngine>(worldSizeKm_, config, seed_);
    erosionEngine_ = std::make_unique<ErosionEngine>(worldSizeKm_, config, seed_);
    
    std::cout << "[GeologicalSimulator] Initialized for " << worldSizeKm_ << "km world with modular architecture" << std::endl;
    lastSnapshotTime_ = std::chrono::steady_clock::now(); 
}

GeologicalSimulator::~GeologicalSimulator() {
    if (backgroundEngine_) {
        backgroundEngine_->stop();
    }
}

void GeologicalSimulator::initialize(uint64_t seed) {
    seed_ = seed;
    rng_.seed(seed);
    
    // Initialize the fractal continent generator with the real seed
    continentGenerator_ = FractalContinentGenerator(seed);
    
    // Re-initialize modular engines with the new seed
    tectonicEngine_ = std::make_unique<TectonicEngine>(worldSizeKm_, config_, seed);
    erosionEngine_ = std::make_unique<ErosionEngine>(worldSizeKm_, config_, seed);
    
    std::cout << "[GeologicalSimulator] Initializing with seed: " << seed << std::endl;
    
    // Initialize geological fields
    initializeFields();
    
    // Reset progress tracking
    startTime_ = std::chrono::steady_clock::now();
    currentPhase_ = GeologicalPhase::TECTONICS;
    currentPhaseProgress_ = 0.0f;
    
    // Estimate total simulation time based on quality preset
    switch (config_.preset) {
        case GeologicalPreset::PERFORMANCE:
            totalSimulationTime_ = 120.0f; // 2 minutes
            totalSteps_ = 100;
            break;
        case GeologicalPreset::BALANCED:
            totalSimulationTime_ = 300.0f; // 5 minutes
            totalSteps_ = 200;
            break;
        case GeologicalPreset::QUALITY:
            totalSimulationTime_ = 720.0f; // 12 minutes
            totalSteps_ = 500;
            break;
        case GeologicalPreset::ULTRA_REALISM:
            totalSimulationTime_ = 1800.0f; // 30 minutes
            totalSteps_ = 1000;
            break;
        case GeologicalPreset::CUSTOM:
            totalSimulationTime_ = config_.custom.simulationDetailLevel * 0.3f;
            totalSteps_ = config_.custom.simulationDetailLevel;
            break;
    }
    
    std::cout << "[GeologicalSimulator] Estimated simulation time: " << totalSimulationTime_ 
              << " seconds, " << totalSteps_ << " steps" << std::endl;
    
    // Initialize snapshot manager for UI visualization
    snapshotManager_ = std::make_unique<GeologicalSnapshotManager>(
        worldSizeKm_, 
        config_.preset == GeologicalPreset::BALANCED ? 512 : 256,
        config_.preset == GeologicalPreset::BALANCED ? 512 : 256,
        worldSizeKm_ * 1000.0f / (config_.preset == GeologicalPreset::BALANCED ? 512 : 256)
    );
}

void GeologicalSimulator::initializeFields() {
    // Field resolution based on quality preset
    int resolution;
    switch (config_.preset) {
        case GeologicalPreset::PERFORMANCE: resolution = 256; break;
        case GeologicalPreset::BALANCED: resolution = 512; break;
        case GeologicalPreset::QUALITY: resolution = 1024; break;
        case GeologicalPreset::ULTRA_REALISM: resolution = 2048; break;
        case GeologicalPreset::CUSTOM: 
            resolution = std::min(2048, std::max(256, config_.custom.simulationDetailLevel / 4)); break;
    }
    
    float spacing = worldSizeKm_ * 1000.0f / resolution; // Convert to meters
    
    std::cout << "[GeologicalSimulator] Creating fields: " << resolution << "x" << resolution 
              << " with " << spacing << "m spacing" << std::endl;
    
    // Create continuous fields - Original geological fields
    mantleStress_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    crustStress_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    elevationField_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    rockHardness_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    waterFlow_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    rockTypes_ = std::make_unique<ContinuousField<RockType>>(resolution, resolution, spacing);
    
    // Step 3: Water & Cave System Framework - Create new fields
    precipitationField_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    groundwaterTable_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    permeabilityField_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    surfaceWaterDepth_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    sedimentLoad_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    caveNetworkDensity_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    springFlow_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    
    // Step 4: Physics-Based Geological Engine - Create new fields
    crustalThickness_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    mantleTemperature_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    isostasyAdjustment_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    erosionRateField_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    
    // Initialize with base values
    mantleStress_->fill(0.0f);
    crustStress_->fill(0.0f);
    elevationField_->fill(0.0f); // Sea level
    rockHardness_->fill(5.0f);   // Medium hardness
    waterFlow_->fill(0.0f);
    rockTypes_->fill(RockType::IGNEOUS_GRANITE);
    
    precipitationField_->fill(800.0f);      // 800mm annual precipitation baseline
    groundwaterTable_->fill(10.0f);         // 10m groundwater depth
    permeabilityField_->fill(0.01f);        // Low baseline permeability
    surfaceWaterDepth_->fill(0.0f);         // No surface water initially
    sedimentLoad_->fill(0.0f);              // No sediment initially
    caveNetworkDensity_->fill(0.0f);        // No caves initially
    springFlow_->fill(0.0f);                // No springs initially
    
    crustalThickness_->fill(35000.0f);      // 35km baseline crustal thickness
    mantleTemperature_->fill(1300.0f);      // 1300°C mantle temperature
    isostasyAdjustment_->fill(0.0f);        // No isostatic adjustment initially
    erosionRateField_->fill(0.1f);          // Baseline erosion rate
    
    // Generate fractal continental foundation
    std::cout << "[GeologicalSimulator] Generating fractal continental foundation..." << std::endl;
    continentGenerator_.GenerateContinentalFoundation(
        *elevationField_,
        *rockTypes_,
        *mantleStress_,
        worldSizeKm_
    );
    
    // Calculate rock hardness based on rock types
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            RockType rockType = rockTypes_->getSample(x, z);
            float hardness = tectonicEngine_->getRockHardness(rockType);
            rockHardness_->setSample(x, z, hardness);
        }
    }
    
    std::cout << "[GeologicalSimulator] Fields initialized with fractal continental foundation" << std::endl;
}

// Step-based simulation implementation
bool GeologicalSimulator::initializeSimulation() {
    phaseStep_ = 0;
    totalPhaseSteps_ = totalSteps_;
    
    // Create initial snapshot
    createSnapshot("Modular interleaved simulation initialized", 0.0f);
    
    simulationInitialized_ = true;
    lastSnapshotTime_ = std::chrono::steady_clock::now();
    
    std::cout << "[GeologicalSimulator] Modular interleaved simulation initialized (" 
              << totalSteps_ << " total steps)" << std::endl;
    return true;
}

bool GeologicalSimulator::stepSimulation() {
    if (!simulationInitialized_ || simulationComplete_ || simulationPaused_) {
        return false;
    }
    
    // ===== Step 4.1: NEW MODULAR INTERLEAVED ARCHITECTURE =====
    // All geological processes run every step using modular engines
    
    // Calculate base time step (represents geological time per simulation step)
    float baseTimeStep = 1000.0f; // 1000 years per step (adjustable)
    
    // Prepare field structures for modular engines
    TectonicEngine::TectonicFields tectonicFields;
    tectonicFields.mantleStress = mantleStress_.get();
    tectonicFields.crustStress = crustStress_.get();
    tectonicFields.elevationField = elevationField_.get();
    tectonicFields.crustalThickness = crustalThickness_.get();
    tectonicFields.mantleTemperature = mantleTemperature_.get();
    tectonicFields.isostasyAdjustment = isostasyAdjustment_.get();
    tectonicFields.rockTypes = rockTypes_.get();
    tectonicFields.rockHardness = rockHardness_.get();
    
    ErosionEngine::ErosionFields erosionFields;
    erosionFields.elevationField = elevationField_.get();
    erosionFields.rockHardness = rockHardness_.get();
    erosionFields.waterFlow = waterFlow_.get();
    erosionFields.precipitationField = precipitationField_.get();
    erosionFields.surfaceWaterDepth = surfaceWaterDepth_.get();
    erosionFields.sedimentLoad = sedimentLoad_.get();
    erosionFields.erosionRateField = erosionRateField_.get();
    erosionFields.rockTypes = rockTypes_.get();
    
    WaterSystemSimulator::WaterFields waterFields;
    waterFields.elevationField = elevationField_.get();
    waterFields.precipitationField = precipitationField_.get();
    waterFields.groundwaterTable = groundwaterTable_.get();
    waterFields.permeabilityField = permeabilityField_.get();
    waterFields.surfaceWaterDepth = surfaceWaterDepth_.get();
    waterFields.sedimentLoad = sedimentLoad_.get();
    waterFields.caveNetworkDensity = caveNetworkDensity_.get();
    waterFields.springFlow = springFlow_.get();
    waterFields.waterFlow = waterFlow_.get();
    waterFields.rockTypes = rockTypes_.get();
    
    // === TECTONIC PROCESSES ===
    tectonicEngine_->simulateMantleConvection(tectonicFields, baseTimeStep * processTimeScales_.tectonicTimeStep);
    tectonicEngine_->simulatePlateMovement(tectonicFields, baseTimeStep * processTimeScales_.tectonicTimeStep);
    tectonicEngine_->simulateMountainBuilding(tectonicFields, baseTimeStep * processTimeScales_.tectonicTimeStep);
    
    // === VOLCANIC PROCESSES ===
    tectonicEngine_->simulateVolcanicActivity(tectonicFields, baseTimeStep * processTimeScales_.volcanicTimeStep);
    
    // === EROSION PROCESSES ===
    erosionEngine_->simulateChemicalWeathering(erosionFields, baseTimeStep * processTimeScales_.erosionTimeStep);
    erosionEngine_->simulatePhysicalErosion(erosionFields, baseTimeStep * processTimeScales_.erosionTimeStep);
    erosionEngine_->simulateWaterDrivenErosion(erosionFields, baseTimeStep * processTimeScales_.erosionTimeStep);
    erosionEngine_->simulateSedimentTransport(erosionFields, baseTimeStep * processTimeScales_.erosionTimeStep);
    erosionEngine_->simulateErosionUpliftBalance(erosionFields, baseTimeStep * processTimeScales_.erosionTimeStep);
    
    // Glacial processes (occasional but intense)
    if (currentStep_ % 5 == 0) {
        erosionEngine_->simulateGlacialCarving(erosionFields, baseTimeStep * processTimeScales_.erosionTimeStep * 5.0f);
    }
    
    // === WATER PROCESSES ===
    waterSystem_->simulatePrecipitationPatterns(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
    waterSystem_->simulateSurfaceWaterAccumulation(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
    waterSystem_->simulateRiverFormation(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
    erosionEngine_->simulateRiverSystems(erosionFields, baseTimeStep * processTimeScales_.waterTimeStep);
    
    // Groundwater systems
    waterSystem_->simulateGroundwaterTable(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
    waterSystem_->simulateAquiferRecharge(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
    waterSystem_->simulateGroundwaterFlow(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
    waterSystem_->simulateSpringFormation(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
    
    // Less frequent water processes
    if (currentStep_ % 3 == 0) {
        waterSystem_->simulateFloodPlains(waterFields, baseTimeStep * processTimeScales_.waterTimeStep * 3.0f);
        waterSystem_->simulateLakeFormation(waterFields, baseTimeStep * processTimeScales_.waterTimeStep * 3.0f);
    }
    
    if (currentStep_ % 2 == 0) {
        waterSystem_->simulateWetlandFormation(waterFields, baseTimeStep * processTimeScales_.waterTimeStep * 2.0f);
    }
    
    // === CAVE SYSTEMS ===
    if (config_.custom.enableCaveSystems) {
        waterSystem_->simulateCaveNetworkGrowth(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
        waterSystem_->simulateUndergroundRivers(waterFields, baseTimeStep * processTimeScales_.waterTimeStep);
        
        if (currentStep_ % 4 == 0) {
            waterSystem_->simulateKarstWeathering(waterFields, baseTimeStep * processTimeScales_.waterTimeStep * 4.0f);
            waterSystem_->simulateCaveCollapse(waterFields, baseTimeStep * processTimeScales_.waterTimeStep * 4.0f);
        }
    }
    
    // === DETAIL PROCESSES ===
    erosionEngine_->simulateMicroWeathering(erosionFields, baseTimeStep * processTimeScales_.detailTimeStep);
    erosionEngine_->simulateSedimentDeposition(erosionFields, baseTimeStep * processTimeScales_.detailTimeStep);
    
    // === ISOSTATIC ADJUSTMENT ===
    if (currentStep_ % 10 == 0) {
        tectonicEngine_->simulateIsostasyAdjustment(tectonicFields, baseTimeStep * processTimeScales_.tectonicTimeStep * 10.0f);
    }
    
    // === STEP TRACKING ===
    currentStep_++;
    
    // Check if simulation is complete
    if (currentStep_ >= totalSteps_) {
        std::cout << "[GeologicalSimulator] Modular interleaved simulation complete after " 
                  << currentStep_ << " steps" << std::endl;
        createSnapshot("Modular geological simulation complete", 100.0f);
        simulationComplete_ = true;
    }
    
    // Calculate overall progress
    float totalProgress = static_cast<float>(currentStep_) / static_cast<float>(totalSteps_);
    
    // Create periodic snapshots for UI preview
    auto now = std::chrono::steady_clock::now();
    float timeSinceLastSnapshot = std::chrono::duration<float>(now - lastSnapshotTime_).count();
    if (timeSinceLastSnapshot >= SNAPSHOT_INTERVAL_SECONDS) {
        std::string description = "Modular Interleaved Simulation (" + 
                                std::to_string(static_cast<int>(totalProgress * 100.0f)) + "%)";
        createSnapshot(description, totalProgress);
        lastSnapshotTime_ = now;
    }
    
    // Call progress callback if set
    if (progressCallback_) {
        PhaseInfo phaseInfo;
        phaseInfo.currentPhase = currentPhase_;
        phaseInfo.overallProgress = totalProgress;
        phaseInfo.phaseProgress = totalProgress;
        phaseInfo.processName = "Modular Interleaved Simulation";
        phaseInfo.metrics = metrics_;
        progressCallback_(phaseInfo);
    }
    
    return !simulationComplete_;
}

bool GeologicalSimulator::isSimulationComplete() const {
    return simulationComplete_;
}

void GeologicalSimulator::pauseSimulation() {
    if (!simulationPaused_) {
        simulationPaused_ = true;
        std::cout << "[GeologicalSimulator] Simulation paused" << std::endl;
    }
}

void GeologicalSimulator::resumeSimulation() {
    if (simulationPaused_) {
        simulationPaused_ = false;
        hasResumedSincePause_ = true;
        std::cout << "[GeologicalSimulator] Simulation resumed" << std::endl;
    }
}

bool GeologicalSimulator::isSimulationPaused() const {
    return simulationPaused_;
}

GeologicalSample GeologicalSimulator::getSampleAt(float x, float z) const {
    GeologicalSample sample;
    
    if (elevationField_) {
        sample.elevation = elevationField_->sampleAt(x, z);
        sample.rockHardness = rockHardness_->sampleAt(x, z);
        sample.waterFlow = waterFlow_->sampleAt(x, z);
        sample.stressField = crustStress_->sampleAt(x, z);
        sample.rockType = rockTypes_->sampleAt(x, z);
        sample.rainfall = precipitationField_ ? precipitationField_->sampleAt(x, z) : 800.0f;
        sample.temperature = 15.0f - (sample.elevation * 0.006f); // Temperature lapse rate
        
        // Enhanced geological features
        sample.hasJointSystems = (sample.rockType == RockType::IGNEOUS_GRANITE) && config_.custom.enableJointSystems;
        
        // Cave systems based on cave network density
        if (caveNetworkDensity_) {
            float caveDensity = caveNetworkDensity_->sampleAt(x, z);
            sample.hasCaveSystems = (caveDensity > 0.1f) && config_.custom.enableCaveSystems;
        } else {
            sample.hasCaveSystems = (sample.rockType == RockType::SEDIMENTARY_LIMESTONE) && config_.custom.enableCaveSystems;
        }
        
        sample.hasQuartzVeins = (sample.rockType == RockType::IGNEOUS_GRANITE) && (sample.stressField > 2.0f);
        sample.weatheringIntensity = std::max(0.1f, sample.rainfall / 1000.0f);
        
        // Water system properties
        if (groundwaterTable_) {
            float groundwaterDepth = groundwaterTable_->sampleAt(x, z);
            sample.hasWetlands = (groundwaterDepth < 2.0f) && (sample.elevation > 50.0f) && (sample.elevation < 300.0f);
            sample.hasNaturalSprings = springFlow_ ? (springFlow_->sampleAt(x, z) > 0.05f) : false;
        }
        
        // Surface water features
        if (surfaceWaterDepth_) {
            float surfaceWater = surfaceWaterDepth_->sampleAt(x, z);
            sample.hasLakes = (surfaceWater > 0.5f);
            sample.hasRivers = (sample.waterFlow > 0.2f) && (surfaceWater > 0.01f);
        }
    }
    
    return sample;
}

void GeologicalSimulator::createSnapshot(const std::string& phaseDescription, float completionPercentage) {
    if (!snapshotManager_) return;
    
    float simulationTimeMyears = completionPercentage * 100.0f; // Simple time calculation
    
    snapshotManager_->SetGenerating(true);
    
    // Create snapshot with water system data
    snapshotManager_->AddSnapshot(
        *elevationField_,
        *rockTypes_,
        *mantleStress_,
        surfaceWaterDepth_ ? *surfaceWaterDepth_ : *elevationField_,
        precipitationField_ ? *precipitationField_ : *elevationField_,
        groundwaterTable_ ? *groundwaterTable_ : *elevationField_,
        waterFlow_ ? *waterFlow_ : *elevationField_,
        sedimentLoad_ ? *sedimentLoad_ : *elevationField_,
        simulationTimeMyears,
        phaseDescription,
        static_cast<int>(snapshotManager_->GetSnapshotCount()),
        completionPercentage
    );
    
    snapshotManager_->SetGenerating(false);
    
    std::cout << "[GeologicalSimulator] Created snapshot: " << phaseDescription 
              << " (" << (completionPercentage * 100.0f) << "% complete)" << std::endl;
}

float GeologicalSimulator::getSnapshotElevationAt(float x, float z) const {
    if (!snapshotManager_) {
        return getSampleAt(x, z).elevation;
    }
    
    const auto* currentSnapshot = snapshotManager_->GetCurrentSnapshot();
    if (!currentSnapshot) {
        return getSampleAt(x, z).elevation;
    }
    
    return currentSnapshot->GetElevationAt(x, z);
}

// =============================================================================
// Step 4.1.2: Background Thread Architecture for UI Responsiveness
// =============================================================================

void GeologicalSimulator::startBackgroundSimulation() {
    if (backgroundEngine_) {
        backgroundEngine_->start();
    }
}

void GeologicalSimulator::stopBackgroundSimulation() {
    if (backgroundEngine_) {
        backgroundEngine_->stop();
    }
}

bool GeologicalSimulator::isBackgroundSimulationRunning() const {
    return backgroundEngine_ ? backgroundEngine_->isRunning() : false;
}

void GeologicalSimulator::enableBackgroundExecution(bool enable) {
    useBackgroundExecution_ = enable;
}

std::shared_ptr<GeologicalSnapshot> GeologicalSimulator::getLatestSnapshot() {
    return backgroundEngine_ ? backgroundEngine_->getLatestSnapshot() : nullptr;
}

float GeologicalSimulator::getBackgroundProgress() const {
    return backgroundEngine_ ? backgroundEngine_->getProgress() : 0.0f;
}

// Snapshot management methods  
bool GeologicalSimulator::hasSnapshots() const {
    return snapshotManager_ && snapshotManager_->GetSnapshotCount() > 0;
}

std::vector<std::string> GeologicalSimulator::getSnapshotDescriptions() const {
    std::vector<std::string> descriptions;
    if (snapshotManager_) {
        for (size_t i = 0; i < snapshotManager_->GetSnapshotCount(); ++i) {
            descriptions.push_back("Snapshot " + std::to_string(i));
        }
    }
    return descriptions;
}

bool GeologicalSimulator::setCurrentSnapshot(size_t index) {
    if (snapshotManager_) {
        return snapshotManager_->SetCurrentSnapshot(index);
    }
    return false;
}

size_t GeologicalSimulator::getCurrentSnapshotIndex() const {
    if (snapshotManager_) {
        return snapshotManager_->GetCurrentSnapshotIndex();
    }
    return 0;
}

size_t GeologicalSimulator::getSnapshotCount() const {
    if (snapshotManager_) {
        return snapshotManager_->GetSnapshotCount();
    }
    return 0;
}

PhaseInfo GeologicalSimulator::getProgressInfo() const {
    PhaseInfo info;
    info.currentPhase = currentPhase_;
    info.overallProgress = static_cast<float>(currentStep_) / static_cast<float>(totalSteps_);
    info.phaseProgress = info.overallProgress;
    info.processName = "Modular Interleaved Simulation";
    info.metrics = metrics_;
    return info;
}

void GeologicalSimulator::setProgressCallback(std::function<void(const PhaseInfo&)> callback) {
    progressCallback_ = callback;
}

float GeologicalSimulator::random01() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng_);
}

float GeologicalSimulator::randomRange(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng_);
}

} // namespace World
} // namespace VoxelCastle
