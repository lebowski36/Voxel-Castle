#include "world/HybridGeologicalSimulator.h"
#include "world/ContinuousField.h"
#include "world/geological_data.h"
#include "utils/debug_logger_stub.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <chrono>

namespace VoxelCastle {
namespace World {

HybridGeologicalSimulator::HybridGeologicalSimulator(float worldSizeKm, uint64_t seed)
    : currentTime_(0.0f)
    , targetTime_(SIMULATION_DURATION)
    , isInitialized_(false)
    , particleWeight_(DEFAULT_PARTICLE_WEIGHT)
    , fractalWeight_(DEFAULT_FRACTAL_WEIGHT) {
    
    std::cout << "[HybridGeologicalSimulator] Initializing hybrid system with worldSize: " 
              << worldSizeKm << "km, seed: " << seed << std::endl;
    
    // Create particle simulation engine
    particleEngine_ = std::make_unique<ParticleSimulationEngine>(worldSizeKm, seed);
    
    // Create fractal detail engine
    fractalEngine_ = std::make_unique<FractalDetailEngine>(worldSizeKm, seed);
    
    // Create snapshot manager for UI integration
    // Use reasonable simulation resolution for performance vs detail tradeoff
    // 512x512 = 262k samples, 256x256 = 65k samples 
    const int GEOLOGICAL_SIMULATION_RESOLUTION = 256; // Start with 256x256 for performance
    int simulationWidth = GEOLOGICAL_SIMULATION_RESOLUTION;
    int simulationHeight = GEOLOGICAL_SIMULATION_RESOLUTION;
    float spacing = worldSizeKm * 1000.0f / simulationWidth; // spacing in meters
    snapshotManager_ = std::make_unique<GeologicalSnapshotManager>(
        worldSizeKm, simulationWidth, simulationHeight, spacing);
    
    DEBUG_LOG("GeologicalSimulator", "Snapshot manager initialized with resolution: " + 
              std::to_string(simulationWidth) + "x" + std::to_string(simulationHeight) + 
              ", spacing: " + std::to_string(spacing) + "m, total samples: " + std::to_string(simulationWidth * simulationHeight));
    
    std::cout << "[HybridGeologicalSimulator] Hybrid system initialized" << std::endl;
}

HybridGeologicalSimulator::~HybridGeologicalSimulator() = default;

void HybridGeologicalSimulator::Initialize(int continentCount, float oceanRatio) {
    std::cout << "[HybridGeologicalSimulator] Initializing with " << continentCount 
              << " continents, " << (oceanRatio * 100.0f) << "% ocean" << std::endl;
    
    // Initialize particle simulation
    particleEngine_->InitializeParticles(continentCount, oceanRatio);
    
    // Clear any cached fractal data
    fractalEngine_->ClearCache();
    
    // Reset simulation state
    currentTime_ = 0.0f;
    isInitialized_ = true;
    
    std::cout << "[HybridGeologicalSimulator] Initialization complete. "
              << particleEngine_->GetParticleCount() << " particles created." << std::endl;
    
    // Create initial snapshot for immediate UI display
    std::cout << "[HybridGeologicalSimulator] Creating initial snapshot (this may take a moment)..." << std::endl;
    CreateSnapshot("Initial State", 0, 0.0f);
    std::cout << "[HybridGeologicalSimulator] Initial snapshot created successfully" << std::endl;
}

bool HybridGeologicalSimulator::InitializeSimulation() {
    // Initialize is already called, so just return success
    return isInitialized_;
}

void HybridGeologicalSimulator::RunSimulationStep(float timeStepYears) {
    if (!isInitialized_) {
        std::cout << "[HybridGeologicalSimulator] Warning: RunSimulationStep called before initialization" << std::endl;
        return;
    }
    
    if (IsComplete()) {
        return; // Simulation already complete
    }
    
    // Use provided time step or default
    float actualTimeStep = (timeStepYears > 0.0f) ? timeStepYears : DEFAULT_TIME_STEP;
    
    // Only log every 10 steps to reduce spam
    static int logCount = 0;
    if (logCount < 10) {
        std::cout << "[HybridGeologicalSimulator] Running simulation step: " 
                  << actualTimeStep << " years (current: " << currentTime_ << ")" << std::endl;
        logCount++;
    }
    
    // Update particle physics
    particleEngine_->UpdateParticlePhysics(actualTimeStep);
    
    // Clear fractal cache to reflect particle updates
    fractalEngine_->ClearCache();
    
    // Update simulation time
    currentTime_ += actualTimeStep;
    
    // Cap at target time
    if (currentTime_ > targetTime_) {
        currentTime_ = targetTime_;
    }
    
    // Create snapshots at regular intervals to capture tectonic changes
    static int lastSnapshotStep = 0;
    int currentStep = static_cast<int>(currentTime_ / DEFAULT_TIME_STEP);
    
    // Create snapshots at step 1 and then every 5 simulation steps to show evolution
    if (currentStep > 0 && (currentStep == 1 || (currentStep % 5 == 0)) && currentStep != lastSnapshotStep) {
        std::string phaseDescription = "Tectonic Evolution - " + std::to_string(currentStep * static_cast<int>(DEFAULT_TIME_STEP)) + " years";
        
        DEBUG_LOG("GeologicalSimulator", "Triggering snapshot creation at step " + std::to_string(currentStep) + 
                  " (" + phaseDescription + ")");
        
        CreateSnapshot(phaseDescription, currentStep, GetProgress());
        lastSnapshotStep = currentStep;
        
        DEBUG_LOG("GeologicalSimulator", "Created snapshot: " + phaseDescription + 
                  " (Step: " + std::to_string(currentStep) + ", Progress: " + std::to_string(GetProgress() * 100.0f) + "%)");
    }
    
    DEBUG_LOG("GeologicalSimulator", "Simulation step complete. Time: " + std::to_string(currentTime_) + 
              " years, Progress: " + std::to_string(GetProgress() * 100.0f) + "%");
}

bool HybridGeologicalSimulator::StepSimulation() {
    DEBUG_LOG("GeologicalSimulator", "StepSimulation called - initialized: " + std::to_string(isInitialized_) + ", complete: " + std::to_string(IsComplete()));
    
    if (!isInitialized_ || IsComplete()) {
        DEBUG_LOG("GeologicalSimulator", "StepSimulation early return - not initialized or complete");
        return false;
    }
    
    DEBUG_LOG("GeologicalSimulator", "StepSimulation calling RunSimulationStep with " + std::to_string(DEFAULT_TIME_STEP) + " years");
    
    // Run one simulation step
    RunSimulationStep(DEFAULT_TIME_STEP);
    
    DEBUG_LOG("GeologicalSimulator", "StepSimulation complete");
    return true;
}

float HybridGeologicalSimulator::GetElevationAt(float worldX, float worldZ) {
    return GetElevationAtResolution(worldX, worldZ, 100.0f); // Default fine resolution
}

float HybridGeologicalSimulator::GetElevationAtResolution(float worldX, float worldZ, float resolution) {
    if (!isInitialized_) {
        std::cout << "[HybridGeologicalSimulator] Warning: GetElevationAt called before initialization" << std::endl;
        return 0.0f;
    }
    
    return CombineParticleAndFractalData(worldX, worldZ, resolution);
}

ParticleSimulationEngine::GeologicalData HybridGeologicalSimulator::GetGeologicalDataAt(float worldX, float worldZ) const {
    if (!isInitialized_) {
        return ParticleSimulationEngine::GeologicalData{};
    }
    
    // Get base geological data from particle simulation
    auto particleData = particleEngine_->SampleAt(worldX, worldZ);
    
    // Enhance elevation with fractal detail
    GeologicalContext context = BuildGeologicalContext(particleData, worldX, worldZ);
    float detailedElevation = fractalEngine_->GenerateDetailAt(worldX, worldZ, particleData.elevation, context);
    
    // Update elevation in the result
    particleData.elevation = detailedElevation;
    
    return particleData;
}

float HybridGeologicalSimulator::GetProgress() const {
    if (targetTime_ <= 0.0f) {
        return 1.0f;
    }
    return std::min(1.0f, currentTime_ / targetTime_);
}

bool HybridGeologicalSimulator::IsComplete() const {
    return currentTime_ >= targetTime_;
}

void HybridGeologicalSimulator::Reset() {
    std::cout << "[HybridGeologicalSimulator] Resetting simulation" << std::endl;
    
    particleEngine_->Reset();
    fractalEngine_->ClearCache();
    
    currentTime_ = 0.0f;
    isInitialized_ = false;
    
    std::cout << "[HybridGeologicalSimulator] Reset complete" << std::endl;
}

void HybridGeologicalSimulator::GetSimulationStats(size_t& particleCount, float& cacheHitRatio) const {
    particleCount = particleEngine_->GetParticleCount();
    
    size_t cacheSize;
    fractalEngine_->GetCacheStats(cacheSize, cacheHitRatio);
}

void HybridGeologicalSimulator::SetDetailBalance(float particleWeight, float fractalWeight) {
    particleWeight_ = std::max(0.0f, std::min(1.0f, particleWeight));
    fractalWeight_ = std::max(0.0f, std::min(1.0f, fractalWeight));
    
    // Normalize weights
    float totalWeight = particleWeight_ + fractalWeight_;
    if (totalWeight > 0.0f) {
        particleWeight_ /= totalWeight;
        fractalWeight_ /= totalWeight;
    }
    
    std::cout << "[HybridGeologicalSimulator] Detail balance updated: " 
              << (particleWeight_ * 100.0f) << "% particle, " 
              << (fractalWeight_ * 100.0f) << "% fractal" << std::endl;
}

void HybridGeologicalSimulator::PauseSimulation() {
    // For now, just mark as paused (could extend for thread-safe pausing)
    std::cout << "[HybridGeologicalSimulator] Simulation paused" << std::endl;
}

void HybridGeologicalSimulator::ResumeSimulation() {
    // For now, just resume (could extend for thread-safe resuming)
    std::cout << "[HybridGeologicalSimulator] Simulation resumed" << std::endl;
}

bool HybridGeologicalSimulator::IsSimulationPaused() const {
    // For now, always return false (could extend for pause state tracking)
    return false;
}

GeologicalSample HybridGeologicalSimulator::GetSampleAt(float worldX, float worldZ) const {
    if (!isInitialized_) {
        return GeologicalSample{};
    }
    
    // Get geological data from hybrid system
    auto geoData = GetGeologicalDataAt(worldX, worldZ);
    
    // Convert to GeologicalSample format
    GeologicalSample sample;
    sample.elevation = geoData.elevation;
    sample.rockType = geoData.rockType;
    sample.stressField = geoData.stress / 1000000.0f; // Convert Pa to more reasonable scale
    sample.temperature = geoData.temperature - 273.15f; // Convert K to Celsius
    sample.rockHardness = 5.0f; // Default hardness
    sample.waterFlow = 0.0f; // Default water flow
    sample.rainfall = 500.0f; // Default rainfall
    
    // Set geological features based on rock type and elevation
    if (geoData.rockType == RockType::SEDIMENTARY_LIMESTONE) {
        sample.hasCaveSystems = true;
    }
    if (geoData.stress > 5000000.0f) { // High stress areas
        sample.hasJointSystems = true;
    }
    if (geoData.elevation < 0.0f) { // Below sea level
        sample.hasWetlands = (geoData.elevation > -100.0f); // Shallow areas
    }
    
    return sample;
}

float HybridGeologicalSimulator::CombineParticleAndFractalData(float worldX, float worldZ, float resolution) {
    static int debugCallCount = 0;
    debugCallCount++;
    
    // Get base geological data from particle simulation
    auto particleData = particleEngine_->SampleAt(worldX, worldZ);
    float baseElevation = particleData.elevation;
    
    // Debug first few calls to understand the data flow
    if (debugCallCount <= 5) {
        std::cout << "[HybridGeologicalSimulator] DEBUG " << debugCallCount 
                  << " - Sampling at (" << worldX << "," << worldZ << ")" << std::endl;
        std::cout << "  particleData.elevation: " << particleData.elevation << "m" << std::endl;
        std::cout << "  particleData.crustalThickness: " << particleData.crustalThickness << "m" << std::endl;
        std::cout << "  particleData.stress: " << particleData.stress << std::endl;
    }
    
    // Build geological context for fractal generation
    GeologicalContext context = BuildGeologicalContext(particleData, worldX, worldZ);
    
    // Generate fractal detail
    float fractalDetail = fractalEngine_->GenerateDetailAtResolution(
        worldX, worldZ, baseElevation, context, resolution);
    
    // Combine with weighted approach
    // Note: fractalDetail already includes baseElevation, so we need to extract the detail component
    float detailComponent = fractalDetail - baseElevation;
    
    // Apply detail balance
    float finalElevation = baseElevation + (detailComponent * fractalWeight_);
    
    if (debugCallCount <= 5) {
        std::cout << "  baseElevation: " << baseElevation << "m" << std::endl;
        std::cout << "  fractalDetail: " << fractalDetail << "m" << std::endl;
        std::cout << "  detailComponent: " << detailComponent << "m" << std::endl;
        std::cout << "  fractalWeight: " << fractalWeight_ << std::endl;
        std::cout << "  finalElevation: " << finalElevation << "m" << std::endl;
    }
    
    return finalElevation;
}

GeologicalContext HybridGeologicalSimulator::BuildGeologicalContext(
    const ParticleSimulationEngine::GeologicalData& particleData,
    float worldX, float worldZ) const {
    
    GeologicalContext context;
    
    // Copy particle data to context
    context.baseElevation = particleData.elevation;
    context.stress = particleData.stress;
    context.crustalThickness = particleData.crustalThickness;
    context.rockType = particleData.rockType;
    context.plateVelocity = particleData.velocity;
    
    // Calculate additional context properties
    context.distanceToCoast = CalculateDistanceToCoast(worldX, worldZ);
    context.continentalProximity = CalculateContinentalProximity(
        particleData.elevation, particleData.crustalThickness);
    
    return context;
}

float HybridGeologicalSimulator::CalculateDistanceToCoast(float worldX, float worldZ) const {
    // Sample elevation at multiple points around the query location to find coast
    float sampleRadius = 5000.0f; // 5km search radius
    int sampleCount = 8;
    float minDistanceToCoast = sampleRadius;
    
    float queryElevation = particleEngine_->SampleElevationAt(worldX, worldZ);
    bool queryIsLand = (queryElevation > 0.0f);
    
    for (int i = 0; i < sampleCount; ++i) {
        float angle = (2.0f * M_PI * i) / sampleCount;
        float sampleX = worldX + std::cos(angle) * sampleRadius;
        float sampleZ = worldZ + std::sin(angle) * sampleRadius;
        
        float sampleElevation = particleEngine_->SampleElevationAt(sampleX, sampleZ);
        bool sampleIsLand = (sampleElevation > 0.0f);
        
        // If we found a land/water transition, we're near the coast
        if (queryIsLand != sampleIsLand) {
            // Binary search for more precise coast location
            float nearDist = 0.0f;
            float farDist = sampleRadius;
            
            for (int j = 0; j < 5; ++j) { // 5 iterations for reasonable precision
                float midDist = (nearDist + farDist) * 0.5f;
                float midX = worldX + std::cos(angle) * midDist;
                float midZ = worldZ + std::sin(angle) * midDist;
                float midElevation = particleEngine_->SampleElevationAt(midX, midZ);
                bool midIsLand = (midElevation > 0.0f);
                
                if (midIsLand == queryIsLand) {
                    nearDist = midDist;
                } else {
                    farDist = midDist;
                }
            }
            
            minDistanceToCoast = std::min(minDistanceToCoast, (nearDist + farDist) * 0.5f);
        }
    }
    
    return minDistanceToCoast;
}

float HybridGeologicalSimulator::CalculateContinentalProximity(float elevation, float crustalThickness) const {
    // Continental proximity based on elevation and crustal thickness
    float elevationFactor = 0.0f;
    if (elevation > 0.0f) {
        elevationFactor = std::min(1.0f, elevation / 1000.0f); // 1km elevation = full continental
    } else {
        elevationFactor = std::max(0.0f, (elevation + 2000.0f) / 2000.0f); // Ocean depth influence
    }
    
    // Crustal thickness factor (continental crust is thicker)
    float thicknessFactor = 0.0f;
    if (crustalThickness > 5000.0f) { // Above minimum oceanic thickness
        thicknessFactor = std::min(1.0f, (crustalThickness - 5000.0f) / 30000.0f);
    }
    
    // Combine factors with weights
    return (elevationFactor * 0.7f + thicknessFactor * 0.3f);
}

// Snapshot management and UI compatibility methods
GeologicalSnapshotManager* HybridGeologicalSimulator::getSnapshotManager() {
    return snapshotManager_.get();
}

const GeologicalSnapshotManager* HybridGeologicalSimulator::getSnapshotManager() const {
    return snapshotManager_.get();
}

GeologicalSample HybridGeologicalSimulator::getSampleAt(float worldX, float worldZ) const {
    GeologicalSample sample;
    
    if (!isInitialized_) {
        return sample; // Return default values
    }
    
    // Get detailed geological data from hybrid system
    auto geologicalData = GetGeologicalDataAt(worldX, worldZ);
    
    // Convert to GeologicalSample format
    sample.elevation = geologicalData.elevation;
    sample.rockType = geologicalData.rockType;
    sample.stressField = geologicalData.stress;
    sample.temperature = geologicalData.temperature;
    
    // Set default values for now - can be enhanced later
    sample.rockHardness = 1.0f;
    sample.waterFlow = 0.0f;
    sample.rainfall = 500.0f;
    
    return sample;
}

void HybridGeologicalSimulator::runFullSimulation(std::function<void(const PhaseInfo&)> progressCallback) {
    if (!progressCallback) {
        // Run without progress callback
        while (!IsComplete()) {
            RunSimulationStep(1000.0f);
        }
        return;
    }
    
    // Run with progress reporting
    while (!IsComplete()) {
        RunSimulationStep(1000.0f);
        
        // Create PhaseInfo for progress callback
        PhaseInfo info;
        info.currentPhase = GeologicalPhase::TECTONICS; // Simplified for now
        info.phaseProgress = GetProgress();
        info.totalProgress = GetProgress();
        info.currentProcess = "Hybrid Geological Simulation";
        info.timeRemaining = 0.0f; // Not calculated for now
        
        progressCallback(info);
    }
}

// Step simulation methods for compatibility
void HybridGeologicalSimulator::stepGeologicalSimulation() {
    RunSimulationStep(1000.0f);
}

bool HybridGeologicalSimulator::isGeologicalSimulationComplete() const {
    return IsComplete();
}

// Background simulation control methods (simplified implementation)
void HybridGeologicalSimulator::startBackgroundSimulation() {
    // For Phase 2A, we don't have full background simulation yet
    // This is a placeholder for compatibility
}

bool HybridGeologicalSimulator::isBackgroundSimulationRunning() const {
    return !IsComplete();
}

void HybridGeologicalSimulator::pauseBackgroundSimulation() {
    // Placeholder for compatibility
}

void HybridGeologicalSimulator::resumeBackgroundSimulation() {
    // Placeholder for compatibility
}

void HybridGeologicalSimulator::stopBackgroundSimulation() {
    // Placeholder for compatibility
}

float HybridGeologicalSimulator::getBackgroundProgress() const {
    return GetProgress();
}

std::shared_ptr<class GeologicalSnapshot> HybridGeologicalSimulator::getLatestSnapshot() {
    // For Phase 2A, return nullptr - snapshots will be implemented later
    return nullptr;
}

const FractalContinentGenerator* HybridGeologicalSimulator::getFractalContinentGenerator() const {
    // For Phase 2A, the hybrid system doesn't use the old FractalContinentGenerator
    // Return nullptr - UI should handle this gracefully
    return nullptr;
}

void HybridGeologicalSimulator::CreateSnapshot(const std::string& phaseDescription, int stepNumber, float completionPercentage) {
    if (!snapshotManager_) {
        ERROR_LOG("GeologicalSimulator", "Cannot create snapshot - no snapshot manager");
        return;
    }
    
    DEBUG_LOG("GeologicalSimulator", "Creating snapshot: " + phaseDescription + 
              " (Step: " + std::to_string(stepNumber) + ", Completion: " + std::to_string(completionPercentage * 100.0f) + "%)");
    
    // Get snapshot resolution from snapshot manager - use fixed 256x256 for performance
    const int SNAPSHOT_RESOLUTION = 256;
    int width = SNAPSHOT_RESOLUTION;
    int height = SNAPSHOT_RESOLUTION;
    float worldSizeMeters = 1024.0f * 1000.0f; // 1024km world size in meters (TODO: get from snapshot manager)
    float spacing = worldSizeMeters / width;
    
    DEBUG_LOG("GeologicalSimulator", "Snapshot resolution: " + std::to_string(width) + "x" + std::to_string(height) + 
              ", spacing: " + std::to_string(spacing) + "m, total pixels: " + std::to_string(width * height));
    
    // Create ContinuousField objects for snapshot data
    ContinuousField<float> elevationField(width, height, spacing);
    ContinuousField<RockType> rockTypeField(width, height, spacing);  
    ContinuousField<float> mantleStressField(width, height, spacing);
    
    // Water system fields (simplified for now)
    ContinuousField<float> surfaceWaterField(width, height, spacing);
    ContinuousField<float> precipitationField(width, height, spacing);
    ContinuousField<float> groundwaterField(width, height, spacing);
    ContinuousField<float> waterFlowField(width, height, spacing);
    ContinuousField<float> sedimentLoadField(width, height, spacing);
    
    // Populate the fields by sampling the hybrid system
    float minElevation = 9999.0f;
    float maxElevation = -9999.0f;
    int sampleCount = 0;
    int totalSamples = width * height;
    int progressReportInterval = totalSamples / 20; // Report progress every 5%
    
    DEBUG_LOG("GeologicalSimulator", "Starting snapshot sampling: " + std::to_string(totalSamples) + " total samples");
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            // Convert grid coordinates to world coordinates
            float worldX = x * spacing;
            float worldZ = z * spacing;
            
            // Sample elevation from hybrid system
            float elevation = CombineParticleAndFractalData(worldX, worldZ);
            elevationField.setSample(x, z, elevation);
            
            // Track elevation range
            minElevation = std::min(minElevation, elevation);
            maxElevation = std::max(maxElevation, elevation);
            sampleCount++;
            
            // Progress reporting
            if (sampleCount % progressReportInterval == 0) {
                float progress = (float)sampleCount / totalSamples * 100.0f;
                DEBUG_LOG("GeologicalSimulator", "Snapshot sampling progress: " + std::to_string(progress) + 
                          "% (" + std::to_string(sampleCount) + "/" + std::to_string(totalSamples) + ")");
            }
            
            // Debug first few samples
            if (sampleCount <= 5) {
                DEBUG_LOG("GeologicalSimulator", "Sample " + std::to_string(sampleCount) + 
                          " at grid(" + std::to_string(x) + "," + std::to_string(z) + ") world(" + 
                          std::to_string(worldX) + "," + std::to_string(worldZ) + ") elevation: " + 
                          std::to_string(elevation) + "m");
            }
            
            // For now, use simple rock type assignment based on elevation
            RockType rockType = RockType::IGNEOUS_GRANITE; // Default continental
            if (elevation < -200.0f) {
                rockType = RockType::IGNEOUS_BASALT; // Oceanic crust
            } else if (elevation > 1000.0f) {
                rockType = RockType::IGNEOUS_GRANITE; // Mountains
            } else if (elevation > 0.0f) {
                rockType = RockType::SEDIMENTARY_SANDSTONE; // Continental sediments
            }
            rockTypeField.setSample(x, z, rockType);
            
            // Simple mantle stress (could be improved later)
            float mantleStress = std::abs(elevation) * 0.001f; // Simple relationship
            mantleStressField.setSample(x, z, mantleStress);
            
            // Water system data (simplified for Phase 2A)
            float surfaceWater = std::max(0.0f, -elevation * 0.1f); // Water depth below sea level
            surfaceWaterField.setSample(x, z, surfaceWater);
            
            precipitationField.setSample(x, z, 1000.0f); // Default precipitation
            groundwaterField.setSample(x, z, std::max(0.0f, 50.0f - elevation * 0.01f));
            waterFlowField.setSample(x, z, 0.0f); // No flow for now
            sedimentLoadField.setSample(x, z, 0.0f); // No sediment for now
        }
    }
    
    // Add the snapshot to the manager
    snapshotManager_->AddSnapshot(
        elevationField, rockTypeField, mantleStressField,
        surfaceWaterField, precipitationField, groundwaterField,
        waterFlowField, sedimentLoadField,
        currentTime_, phaseDescription, stepNumber, completionPercentage
    );
    
    INFO_LOG("GeologicalSimulator", "Snapshot created successfully: " + phaseDescription);
    INFO_LOG("GeologicalSimulator", "Elevation range in snapshot: " + std::to_string(minElevation) + 
             "m to " + std::to_string(maxElevation) + "m (from " + std::to_string(sampleCount) + " samples)");
}

} // namespace World
} // namespace VoxelCastle
