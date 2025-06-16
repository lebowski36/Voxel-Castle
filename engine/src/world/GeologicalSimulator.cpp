#include "world/GeologicalSimulator.h"
#include "util/noise.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <thread>

using namespace VoxelCastle::World;

namespace VoxelCastle {
namespace World {

GeologicalSimulator::GeologicalSimulator(int worldSizeKm, const GeologicalConfig& config)
    : config_(config), worldSizeKm_(static_cast<float>(worldSizeKm)), seed_(0), currentPhase_(GeologicalPhase::TECTONICS),
      currentPhaseProgress_(0.0f), continentGenerator_(0),
      simulationInitialized_(false), simulationComplete_(false), simulationPaused_(false),
      hasResumedSincePause_(false),
      currentStep_(0), totalSteps_(0), phaseStep_(0), totalPhaseSteps_(0), phaseTimeStep_(0.0f) {
    
    // Time scales are now centralized in geological_constants.h
    // ProcessTimeScales::TECTONIC, EROSION, WATER, DETAIL, VOLCANIC
    
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
        backgroundEngine_->StopBackgroundSimulation();
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
    erosionRateField_->fill(0.001f);        // Very low baseline erosion rate (1mm/1000years)
    
    // Generate fractal continental foundation
    std::cout << "[GeologicalSimulator] Generating fractal continental foundation..." << std::endl;
    continentGenerator_.GenerateContinentalFoundation(
        *elevationField_,
        *rockTypes_,
        *mantleStress_,
        worldSizeKm_,
        config_
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
    
    // ===== DEBUG: Track elevation changes by process =====
    static int debugStepCount = 0;
    bool shouldDebugStep = (debugStepCount < 5); // Debug first 5 steps
    
    auto getElevationRange = [this]() -> std::pair<float, float> {
        float minElev = std::numeric_limits<float>::max();
        float maxElev = std::numeric_limits<float>::lowest();
        int width = elevationField_->getWidth();
        int height = elevationField_->getHeight();
        
        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
                float elev = elevationField_->getSample(x, z);
                minElev = std::min(minElev, elev);
                maxElev = std::max(maxElev, elev);
            }
        }
        return {minElev, maxElev};
    };
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] Step " << debugStepCount << " START: " 
                  << minElev << "m to " << maxElev << "m (range: " << (maxElev - minElev) << "m)" << std::endl;
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
    
    // === TECTONIC PROCESSES ===
    // Run at their own time scale (usually slower)
    tectonicEngine_->simulateMantleConvection(tectonicFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::TECTONIC);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After MantleConvection: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    tectonicEngine_->simulatePlateMovement(tectonicFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::TECTONIC);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After PlateMovement: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    tectonicEngine_->simulateMountainBuilding(tectonicFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::TECTONIC);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After MountainBuilding: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    // === VOLCANIC PROCESSES ===
    // Run at their own time scale (episodic, can be intense)
    tectonicEngine_->simulateVolcanicActivity(tectonicFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::VOLCANIC);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After VolcanicActivity: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    // === EROSION PROCESSES ===
    // Run at their own time scale (typically faster than tectonics)
    erosionEngine_->simulateChemicalWeathering(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::EROSION);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After ChemicalWeathering: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    erosionEngine_->simulatePhysicalErosion(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::EROSION);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After PhysicalErosion: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    erosionEngine_->simulateWaterDrivenErosion(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::EROSION);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After WaterDrivenErosion: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    erosionEngine_->simulateSedimentTransport(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::EROSION);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After SedimentTransport: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
    }
    
    erosionEngine_->simulateErosionUpliftBalance(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::EROSION);
    
    if (shouldDebugStep) {
        auto [minElev, maxElev] = getElevationRange();
        std::cout << "[ELEVATION_TRACK] After ErosionUpliftBalance: " 
                  << minElev << "m to " << maxElev << "m" << std::endl;
        
        // Check spatial distribution - sample different areas to see if we get realistic variation
        int width = elevationField_->getWidth();
        int height = elevationField_->getHeight();
        
        std::cout << "[SPATIAL_VARIATION] Step " << debugStepCount << " elevation samples:" << std::endl;
        std::cout << "  NW corner: " << elevationField_->getSample(width/4, height/4) << "m" << std::endl;
        std::cout << "  NE corner: " << elevationField_->getSample(3*width/4, height/4) << "m" << std::endl;
        std::cout << "  Center: " << elevationField_->getSample(width/2, height/2) << "m" << std::endl;
        std::cout << "  SW corner: " << elevationField_->getSample(width/4, 3*height/4) << "m" << std::endl;
        std::cout << "  SE corner: " << elevationField_->getSample(3*width/4, 3*height/4) << "m" << std::endl;
        
        debugStepCount++;
    }

    // Glacial processes (occasional but intense)
    if (currentStep_ % 5 == 0) {
        erosionEngine_->simulateGlacialCarving(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::EROSION * 5.0f);
    }
    
    // === WATER PROCESSES ===
    // Run at their own time scale (typically fast)
    waterSystem_->SimulatePrecipitationPatterns(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    waterSystem_->SimulateSurfaceWaterAccumulation(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    waterSystem_->SimulateRiverFormation(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    erosionEngine_->simulateRiverSystems(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    
    // Groundwater systems
    waterSystem_->SimulateGroundwaterTable(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    waterSystem_->SimulateAquiferRecharge(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    waterSystem_->SimulateGroundwaterFlow(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    waterSystem_->SimulateSpringFormation(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
    
    // Less frequent water processes
    if (currentStep_ % 3 == 0) {
        waterSystem_->SimulateFloodPlains(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER * 3.0f);
        waterSystem_->SimulateLakeFormation(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER * 3.0f);
    }
    
    if (currentStep_ % 2 == 0) {
        waterSystem_->SimulateWetlandFormation(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER * 2.0f);
    }
    
    // === CAVE SYSTEMS ===
    if (config_.custom.enableCaveSystems) {
        waterSystem_->SimulateCaveWaterInteractions(baseTimeStep * GeologicalConstants::ProcessTimeScales::WATER);
        // TODO: Implement specific cave methods when needed
        // simulateCaveNetworkGrowth, simulateUndergroundRivers, etc.
    }
    
    // === DETAIL PROCESSES ===
    erosionEngine_->simulateMicroWeathering(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::DETAIL);
    erosionEngine_->simulateSedimentDeposition(erosionFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::DETAIL);
    
    // === ISOSTATIC ADJUSTMENT ===
    if (currentStep_ % 10 == 0) {
        tectonicEngine_->simulateIsostasyAdjustment(tectonicFields, baseTimeStep * GeologicalConstants::ProcessTimeScales::TECTONIC * 10.0f);
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
        phaseInfo.totalProgress = totalProgress;
        phaseInfo.phaseProgress = totalProgress;
        phaseInfo.currentProcess = "Modular Interleaved Simulation";
        phaseInfo.metrics = metrics_;
        progressCallback_(phaseInfo);
    }
    
    debugStepCount++;
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
        backgroundEngine_->StartBackgroundSimulation();
    }
}

void GeologicalSimulator::stopBackgroundSimulation() {
    if (backgroundEngine_) {
        backgroundEngine_->StopBackgroundSimulation();
    }
}

bool GeologicalSimulator::isBackgroundSimulationRunning() const {
    return backgroundEngine_ ? backgroundEngine_->IsRunning() : false;
}

void GeologicalSimulator::enableBackgroundExecution(bool enable) {
    useBackgroundExecution_ = enable;
}

std::shared_ptr<GeologicalSnapshot> GeologicalSimulator::getLatestSnapshot() {
    return backgroundEngine_ ? backgroundEngine_->GetNextSnapshot() : nullptr;
}

float GeologicalSimulator::getBackgroundProgress() const {
    return backgroundEngine_ ? backgroundEngine_->GetProgress() : 0.0f;
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
    info.totalProgress = static_cast<float>(currentStep_) / static_cast<float>(totalSteps_);
    info.phaseProgress = info.totalProgress;
    info.currentProcess = "Modular Interleaved Simulation";
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

void GeologicalSimulator::runFullSimulation(std::function<void(const PhaseInfo&)> progressCallback) {
    std::cout << "[GeologicalSimulator] Starting full modular geological simulation" << std::endl;
    
    progressCallback_ = progressCallback;
    
    // Initialize the step-based simulation
    if (!initializeSimulation()) {
        std::cout << "[GeologicalSimulator] Failed to initialize simulation" << std::endl;
        return;
    }
    
    // Run the simulation to completion
    while (!simulationComplete_ && !simulationPaused_) {
        if (!stepSimulation()) {
            break;
        }
        
        // Small delay to prevent overwhelming the system
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "[GeologicalSimulator] Full modular simulation completed" << std::endl;
}

// Individual phase simulation methods for testing and API compatibility
void GeologicalSimulator::simulateTectonicPhase(float durationMillionYears) {
    if (!simulationInitialized_) {
        std::cerr << "[GeologicalSimulator] Cannot simulate tectonic phase - simulation not initialized" << std::endl;
        return;
    }
    
    // Calculate time steps for duration
    int steps = static_cast<int>(durationMillionYears * 1000); // Convert million years to thousand year steps
    
    std::cout << "[GeologicalSimulator] Simulating tectonic phase for " << durationMillionYears 
              << " million years (" << steps << " steps)" << std::endl;
    
    // Prepare field structures for tectonic engine
    TectonicEngine::TectonicFields tectonicFields;
    tectonicFields.mantleStress = mantleStress_.get();
    tectonicFields.crustStress = crustStress_.get();
    tectonicFields.elevationField = elevationField_.get();
    tectonicFields.crustalThickness = crustalThickness_.get();
    tectonicFields.mantleTemperature = mantleTemperature_.get();
    tectonicFields.isostasyAdjustment = isostasyAdjustment_.get();
    tectonicFields.rockTypes = rockTypes_.get();
    tectonicFields.rockHardness = rockHardness_.get();
    
    float timeStepThousandYears = 1.0f; // 1000 years per step
    
    for (int step = 0; step < steps; ++step) {
        // Run tectonic processes
        tectonicEngine_->simulateMantleConvection(tectonicFields, timeStepThousandYears);
        tectonicEngine_->simulatePlateMovement(tectonicFields, timeStepThousandYears);
        tectonicEngine_->simulateMountainBuilding(tectonicFields, timeStepThousandYears);
        tectonicEngine_->simulateVolcanicActivity(tectonicFields, timeStepThousandYears);
        
        // Isostatic adjustment
        tectonicEngine_->simulateIsostasyAdjustment(tectonicFields, timeStepThousandYears);
    }
    
    std::cout << "[GeologicalSimulator] Tectonic phase simulation completed" << std::endl;
}

void GeologicalSimulator::simulateErosionPhase(float durationThousandYears) {
    if (!simulationInitialized_) {
        std::cerr << "[GeologicalSimulator] Cannot simulate erosion phase - simulation not initialized" << std::endl;
        return;
    }
    
    // Calculate time steps for duration
    int steps = static_cast<int>(durationThousandYears); // Already in thousand year units
    
    std::cout << "[GeologicalSimulator] Simulating erosion phase for " << durationThousandYears 
              << " thousand years (" << steps << " steps)" << std::endl;
    
    // Prepare field structures for erosion engine
    ErosionEngine::ErosionFields erosionFields;
    erosionFields.elevationField = elevationField_.get();
    erosionFields.rockHardness = rockHardness_.get();
    erosionFields.waterFlow = waterFlow_.get();
    erosionFields.precipitationField = precipitationField_.get();
    erosionFields.surfaceWaterDepth = surfaceWaterDepth_.get();
    erosionFields.sedimentLoad = sedimentLoad_.get();
    erosionFields.erosionRateField = erosionRateField_.get();
    erosionFields.rockTypes = rockTypes_.get();
    
    float timeStepThousandYears = 1.0f; // 1000 years per step
    
    for (int step = 0; step < steps; ++step) {
        // Run erosion processes
        erosionEngine_->simulateChemicalWeathering(erosionFields, timeStepThousandYears);
        erosionEngine_->simulatePhysicalErosion(erosionFields, timeStepThousandYears);
        erosionEngine_->simulateWaterDrivenErosion(erosionFields, timeStepThousandYears);
        erosionEngine_->simulateSedimentTransport(erosionFields, timeStepThousandYears);
        erosionEngine_->simulateErosionUpliftBalance(erosionFields, timeStepThousandYears);
        
        // Occasional glacial erosion
        if (step % 5 == 0) {
            erosionEngine_->simulateGlacialCarving(erosionFields, timeStepThousandYears * 5.0f);
        }
    }
    
    std::cout << "[GeologicalSimulator] Erosion phase simulation completed" << std::endl;
}

std::unique_ptr<ContinuousField<GeologicalSample>> GeologicalSimulator::exportGeologicalData() const {
    // For now, return nullptr as noted in the test - this is a placeholder for Phase 3
    // In the future, this will create a comprehensive geological data export
    std::cout << "[GeologicalSimulator] exportGeologicalData called - not yet implemented (Phase 3)" << std::endl;
    return nullptr;
}

void GeologicalSimulator::pauseBackgroundSimulation() {
    if (backgroundEngine_) {
        backgroundEngine_->SetPaused(true);
    }
}

void GeologicalSimulator::resumeBackgroundSimulation() {
    if (backgroundEngine_) {
        backgroundEngine_->SetPaused(false);
    }
}

bool GeologicalSimulator::isBackgroundSimulationPaused() const {
    return backgroundEngine_ ? backgroundEngine_->IsPaused() : false;
}

} // namespace World
} // namespace VoxelCastle
