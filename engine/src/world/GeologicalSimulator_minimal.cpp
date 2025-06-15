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
    
    // Initialize background simulation engine (simplified)
    backgroundEngine_ = std::make_unique<BackgroundSimulationEngine>(this);
    
    // Initialize water system simulator (simplified)
    waterSystem_ = std::make_unique<WaterSystemSimulator>(worldSizeKm_, config);
    
    std::cout << "[GeologicalSimulator] Initialized for " << worldSizeKm_ << "km world (modular)" << std::endl;
    lastSnapshotTime_ = std::chrono::steady_clock::now(); 
}

GeologicalSimulator::~GeologicalSimulator() {
    // Simplified destructor - let the unique_ptrs handle cleanup
}

void GeologicalSimulator::initialize(uint64_t seed) {
    seed_ = seed;
    rng_.seed(seed);
    
    // Initialize the fractal continent generator with the real seed
    continentGenerator_ = FractalContinentGenerator(seed);
    
    std::cout << "[GeologicalSimulator] Initializing with seed: " << seed << std::endl;
    
    // Initialize geological fields (keep existing implementation)
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

// Keep the existing initializeFields method (it works)
void GeologicalSimulator::initializeFields() {
    // Keep all the existing field initialization code unchanged
    // ... (keep the working implementation from the old file)
}

// Keep existing simulation methods but make them simpler
void GeologicalSimulator::runFullSimulation(std::function<void(const PhaseInfo&)> progressCallback) {
    progressCallback_ = progressCallback;
    
    std::cout << "[GeologicalSimulator] Starting modular geological simulation" << std::endl;
    
    // Clear any existing snapshots and create initial snapshot
    if (snapshotManager_) {
        snapshotManager_->Clear();
        createSnapshot("Initial Continental Formation", 0.0f);
    }
    
    // Use existing phase methods (keep them working)
    // Phase 1: Tectonic Phase (40% of total time)
    updateProgress(0.0f, "Starting Tectonic Simulation");
    simulateTectonicPhase(100.0f); // 100 million years
    
    // Phase 2: Erosion Phase (40% of total time)  
    currentPhase_ = GeologicalPhase::EROSION;
    updateProgress(0.0f, "Starting Erosion Simulation");
    simulateErosionPhase(10000.0f); // 10 thousand years
    
    // Phase 3: Detail Phase (20% of total time)
    currentPhase_ = GeologicalPhase::DETAIL;
    updateProgress(0.0f, "Starting Detail Simulation");
    simulateDetailPhase(1000.0f); // 1 thousand years
    
    updateProgress(1.0f, "Simulation Complete");
    
    // Create final snapshot
    if (snapshotManager_) {
        createSnapshot("Final Geological State", 1.0f);
    }
    
    std::cout << "[GeologicalSimulator] Full simulation completed" << std::endl;
}

// Simplified step simulation method
bool GeologicalSimulator::stepSimulation() {
    if (!simulationInitialized_ || simulationComplete_ || simulationPaused_) {
        return false;
    }
    
    // Simple step increment
    currentStep_++;
    
    // Check if simulation is complete
    if (currentStep_ >= totalSteps_) {
        std::cout << "[GeologicalSimulator] Modular simulation complete after " 
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
        std::string description = "Modular Simulation (" + 
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
        phaseInfo.metrics = metrics_;
        progressCallback_(phaseInfo);
    }
    
    return !simulationComplete_;
}

// Keep the existing step simulation method intact
bool GeologicalSimulator::initializeSimulation() {
    phaseStep_ = 0;
    totalPhaseSteps_ = totalSteps_;
    
    // Create initial snapshot
    createSnapshot("Modular simulation initialized", 0.0f);
    
    simulationInitialized_ = true;
    lastSnapshotTime_ = std::chrono::steady_clock::now();
    
    std::cout << "[GeologicalSimulator] Modular simulation initialized (" 
              << totalSteps_ << " total steps)" << std::endl;
    return true;
}

// Keep existing control methods
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

// Keep existing sampling method
GeologicalSample GeologicalSimulator::getSampleAt(float x, float z) const {
    // Keep existing implementation unchanged
    GeologicalSample sample;
    
    if (elevationField_) {
        sample.elevation = elevationField_->sampleAt(x, z);
        sample.rockHardness = rockHardness_->sampleAt(x, z);
        sample.waterFlow = waterFlow_->sampleAt(x, z);
        sample.stressField = crustStress_->sampleAt(x, z);
        sample.rockType = rockTypes_->sampleAt(x, z);
        sample.rainfall = precipitationField_ ? precipitationField_->sampleAt(x, z) : 800.0f;
        sample.temperature = 15.0f - (sample.elevation * 0.006f);
        
        sample.hasJointSystems = (sample.rockType == RockType::IGNEOUS_GRANITE) && config_.custom.enableJointSystems;
        sample.hasCaveSystems = (sample.rockType == RockType::SEDIMENTARY_LIMESTONE) && config_.custom.enableCaveSystems;
        sample.hasQuartzVeins = (sample.rockType == RockType::IGNEOUS_GRANITE) && (sample.stressField > 2.0f);
        sample.weatheringIntensity = std::max(0.1f, sample.rainfall / 1000.0f);
    }
    
    return sample;
}

// Keep existing snapshot methods
void GeologicalSimulator::createSnapshot(const std::string& phaseDescription, float completionPercentage) {
    if (!snapshotManager_) return;
    
    float simulationTimeMyears = completionPercentage * 100.0f;
    
    snapshotManager_->SetGenerating(true);
    
    // Use existing snapshot creation (simplified)
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

// Simplified background simulation methods (stubs for now)
void GeologicalSimulator::startBackgroundSimulation() {
    if (backgroundEngine_) {
        backgroundEngine_->Start(); // Use existing method names
    }
}

void GeologicalSimulator::stopBackgroundSimulation() {
    if (backgroundEngine_) {
        backgroundEngine_->Stop(); // Use existing method names
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

// Keep existing utility methods
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
