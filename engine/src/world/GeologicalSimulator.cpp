#include "world/GeologicalSimulator.h"
#include "util/noise.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <execution>  // For parallel algorithms
#include <vector>
#include <numeric>    // For std::iota

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
    
    std::cout << "[GeologicalSimulator] Initialized for " << worldSizeKm_ << "km world with " 
              << (config.preset == GeologicalPreset::BALANCED ? "BALANCED" : "OTHER") << " quality" << std::endl;

    lastSnapshotTime_ = std::chrono::steady_clock::now(); 
}

void GeologicalSimulator::initialize(uint64_t seed) {
    seed_ = seed;
    rng_.seed(seed);
    
    // Initialize the fractal continent generator with the real seed
    continentGenerator_ = FractalContinentGenerator(seed);
    
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
            totalSimulationTime_ = 120.0f; // 2 minutes - fast but watchable
            break;
        case GeologicalPreset::BALANCED:
            totalSimulationTime_ = 300.0f; // 5 minutes - ideal for watching geological evolution
            break;
        case GeologicalPreset::QUALITY:
            totalSimulationTime_ = 720.0f; // 12 minutes - detailed geological processes
            break;
        case GeologicalPreset::ULTRA_REALISM:
            totalSimulationTime_ = 1800.0f; // 30 minutes - maximum realism
            break;
        case GeologicalPreset::CUSTOM:
            totalSimulationTime_ = config_.custom.simulationDetailLevel * 0.3f;
            break;
    }
    
    std::cout << "[GeologicalSimulator] Estimated simulation time: " << totalSimulationTime_ << " seconds" << std::endl;
    
    // Initialize snapshot manager for UI visualization
    snapshotManager_ = std::make_unique<GeologicalSnapshotManager>(
        worldSizeKm_, 
        config_.preset == GeologicalPreset::BALANCED ? 512 : 256,  // Use lower resolution for snapshots
        config_.preset == GeologicalPreset::BALANCED ? 512 : 256,
        worldSizeKm_ * 1000.0f / (config_.preset == GeologicalPreset::BALANCED ? 512 : 256)
    );
}

// CRITICAL: Helper function to enforce elevation bounds throughout simulation
void GeologicalSimulator::clampElevationSafe(int x, int z, const std::string& processName) {
    if (!elevationField_) return;
    
    float elevation = elevationField_->getSample(x, z);
    if (elevation < -1800.0f || elevation > 1200.0f) {
        std::cout << "[ELEVATION SAFETY] " << processName << " at (" << x << "," << z 
                  << ") exceeded bounds: " << elevation << "m - clamping" << std::endl;
        elevation = std::max(-1800.0f, std::min(1200.0f, elevation));
        elevationField_->setSample(x, z, elevation);
    }
}

// =============================================================================
// Elevation Safety System - Prevent Extreme Values
// =============================================================================

void GeologicalSimulator::validateAndClampElevation(int x, int z, const std::string& processName) {
    float elevation = elevationField_->getSample(x, z);
    
    // Check for extreme values that indicate a bug
    if (std::abs(elevation) > 10000.0f) {
        std::cout << "[ELEVATION_BUG] " << processName << " created extreme elevation: " 
                  << elevation << "m at (" << x << "," << z << ") - CLAMPING to bounds" << std::endl;
    }
    
    // Clamp to realistic geological bounds used elsewhere in the code
    float clampedElevation = std::max(-1800.0f, std::min(1200.0f, elevation));
    
    if (elevation != clampedElevation) {
        elevationField_->setSample(x, z, clampedElevation);
    }
}

// =============================================================================

void GeologicalSimulator::initializeFields() {
    // Field resolution based on quality preset
    int resolution;
    switch (config_.preset) {
        case GeologicalPreset::PERFORMANCE:
            resolution = 256;
            break;
        case GeologicalPreset::BALANCED:
            resolution = 512;
            break;
        case GeologicalPreset::QUALITY:
            resolution = 1024;
            break;
        case GeologicalPreset::ULTRA_REALISM:
            resolution = 2048;
            break;
        case GeologicalPreset::CUSTOM:
            resolution = std::min(2048, std::max(256, config_.custom.simulationDetailLevel / 4));
            break;
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
    
    // Initialize with base values - Original fields
    mantleStress_->fill(0.0f);
    crustStress_->fill(0.0f);
    elevationField_->fill(0.0f); // Sea level
    rockHardness_->fill(5.0f);   // Medium hardness
    waterFlow_->fill(0.0f);
    rockTypes_->fill(RockType::IGNEOUS_GRANITE);
    
    // Initialize Step 3: Water & Cave System fields
    precipitationField_->fill(800.0f);      // 800mm annual precipitation baseline
    groundwaterTable_->fill(10.0f);         // 10m groundwater depth
    permeabilityField_->fill(0.01f);        // Low baseline permeability
    surfaceWaterDepth_->fill(0.0f);         // No surface water initially
    sedimentLoad_->fill(0.0f);              // No sediment initially
    caveNetworkDensity_->fill(0.0f);        // No caves initially
    springFlow_->fill(0.0f);                // No springs initially
    
    // Initialize Step 4: Physics-Based Geological Engine fields
    crustalThickness_->fill(35000.0f);      // 35km baseline crustal thickness
    mantleTemperature_->fill(1300.0f);      // 1300°C mantle temperature
    isostasyAdjustment_->fill(0.0f);        // No isostatic adjustment initially
    erosionRateField_->fill(0.1f);          // Baseline erosion rate
    
    // NEW: Replace noise-based initialization with fractal continental generation
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
            float hardness = getRockHardness(rockType);
            rockHardness_->setSample(x, z, hardness);
        }
    }
    
    std::cout << "[GeologicalSimulator] Fields initialized with fractal continental foundation" << std::endl;
}

void GeologicalSimulator::runFullSimulation(std::function<void(const PhaseInfo&)> progressCallback) {
    progressCallback_ = progressCallback;
    
    std::cout << "[GeologicalSimulator] Starting full geological simulation" << std::endl;
    
    // Clear any existing snapshots and create initial snapshot
    if (snapshotManager_) {
        snapshotManager_->Clear();
        createSnapshot("Initial Continental Formation", 0.0f);
    }
    
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

void GeologicalSimulator::simulateTectonicPhase(float durationMillionYears) {
    std::cout << "[GeologicalSimulator] Tectonic Phase: Simulating " << durationMillionYears << " million years" << std::endl;
    
    int steps = std::max(30, config_.getSimulationSteps() / 3); // At least 30 steps for meaningful geological detail
    float timeStep = durationMillionYears / steps;
    
    std::cout << "[GeologicalSimulator] Tectonic simulation: " << steps << " steps, " << timeStep << " million years per step" << std::endl;
    
    for (int step = 0; step < steps; ++step) {
        // Update progress less frequently for better performance
        if (steps <= 5 || step % std::max(1, steps/20) == 0) { // Update 20 times max during phase
            float progress = static_cast<float>(step) / steps;
            updateProgress(progress, "Mantle Convection & Plate Movement");
        }
        
        // Simulate geological processes with timing info
        if (steps <= 5 || step % std::max(1, steps/5) == 0) { // Every 1/5th of steps, print progress
            std::cout << "[GeologicalSimulator] Tectonic step " << step << "/" << steps << " (" << ((step*100)/steps) << "%)" << std::endl;
        }
        
        simulateMantleConvection(timeStep);
        simulatePlateMovement(timeStep);
        simulateMountainBuilding(timeStep);
        
        if (step % std::max(1, steps / 4) == 0) {
            simulateVolcanicActivity(timeStep * 4); // Less frequent volcanic activity
        }
        
        // Update performance metrics less frequently
        if (steps <= 10 || step % std::max(1, steps/10) == 0) {
            updatePerformanceMetrics();
        }
    }
    
    std::cout << "[GeologicalSimulator] Tectonic phase completed" << std::endl;
}

void GeologicalSimulator::simulateErosionPhase(float durationThousandYears) {
    std::cout << "[GeologicalSimulator] Erosion Phase: Simulating " << durationThousandYears << " thousand years" << std::endl;
    
    int steps = std::max(30, config_.getSimulationSteps() / 3); // At least 30 steps for meaningful geological detail
    float timeStep = durationThousandYears / steps;
    
    for (int step = 0; step < steps; ++step) {
        // Update progress
        float progress = static_cast<float>(step) / steps;
        updateProgress(progress, "Water Systems & Erosion Balance");
        
        // Step 3.1: Surface Water Flow System
        simulatePrecipitationPatterns(timeStep);
        simulateSurfaceWaterAccumulation(timeStep);
        simulateRiverFormation(timeStep);
        simulateFloodPlains(timeStep);
        simulateLakeFormation(timeStep);
        
        // Step 3.2: Groundwater Aquifer System
        simulateGroundwaterTable(timeStep);
        simulateAquiferRecharge(timeStep);
        simulateGroundwaterFlow(timeStep);
        simulateSpringFormation(timeStep);
        simulateWetlandFormation(timeStep);
        
        // Step 3.3: Underground Cave Network System
        if (config_.custom.enableCaveSystems) {
            simulateCaveNetworkGrowth(timeStep);
            simulateUndergroundRivers(timeStep);
            simulateKarstWeathering(timeStep);
            simulateCaveCollapse(timeStep);
        }
        
        // Step 3.4 & 3.5: Integrated Water-Erosion System
        simulateSedimentTransport(timeStep);
        simulateWaterDrivenErosion(timeStep);
        simulateErosionUpliftBalance(timeStep);
        
        // Legacy erosion processes (enhanced by new water systems)
        simulateChemicalWeathering(timeStep * 0.5f); // Reduced as water systems now handle this
        simulatePhysicalErosion(timeStep * 0.5f);    // Reduced as water systems now handle this
        simulateRiverSystems(timeStep * 0.3f);       // Reduced as new river formation handles this
        
        if (config_.custom.enableGlacialFeatures && step % std::max(1, steps / 8) == 0) {
            simulateGlacialCarving(timeStep * 8); // Occasional glacial periods
        }
        
        updatePerformanceMetrics();
    }
}

void GeologicalSimulator::simulateDetailPhase(float durationHundredYears) {
    std::cout << "[GeologicalSimulator] Detail Phase: Simulating " << durationHundredYears << " hundred years" << std::endl;
    
    int steps = std::max(10, config_.getSimulationSteps() / 3); // At least 10 steps for meaningful detail
    float timeStep = durationHundredYears / steps;
    
    for (int step = 0; step < steps; ++step) {
        // Update progress
        float progress = static_cast<float>(step) / steps;
        updateProgress(progress, "Micro-Weathering & Surface Features");
        
        // Simulate detail processes
        simulateMicroWeathering(timeStep);
        simulateSedimentDeposition(timeStep);
        
        if (config_.custom.enableJointSystems) {
            simulateJointFormation(timeStep);
        }
        
        if (config_.custom.enableCaveSystems && step % std::max(1, steps / 4) == 0) {
            simulateCaveGeneration(timeStep * 4);
        }
        
        updatePerformanceMetrics();
    }
}

GeologicalSample GeologicalSimulator::getSampleAt(float x, float z) const {
    GeologicalSample sample;
    
    if (elevationField_) {
        sample.elevation = elevationField_->sampleAt(x, z);
        sample.rockHardness = rockHardness_->sampleAt(x, z);
        sample.waterFlow = waterFlow_->sampleAt(x, z);
        sample.stressField = crustStress_->sampleAt(x, z);
        sample.rockType = rockTypes_->sampleAt(x, z);
        
        // Step 3: Water & Cave System Framework - Add new water system data
        sample.rainfall = precipitationField_ ? precipitationField_->sampleAt(x, z) : 800.0f;
        sample.temperature = 15.0f - (sample.elevation * 0.006f); // Temperature lapse rate
        
        // Enhanced geological features based on new water and cave systems
        sample.hasJointSystems = (sample.rockType == RockType::IGNEOUS_GRANITE) && config_.custom.enableJointSystems;
        
        // Cave systems now based on actual cave network density
        if (caveNetworkDensity_) {
            float caveDensity = caveNetworkDensity_->sampleAt(x, z);
            sample.hasCaveSystems = (caveDensity > 0.1f) && config_.custom.enableCaveSystems;
        } else {
            sample.hasCaveSystems = (sample.rockType == RockType::SEDIMENTARY_LIMESTONE) && config_.custom.enableCaveSystems;
        }
        
        sample.hasQuartzVeins = (sample.rockType == RockType::IGNEOUS_GRANITE) && (sample.stressField > 2.0f);
        sample.weatheringIntensity = std::max(0.1f, sample.rainfall / 1000.0f);
        
        // Additional water system properties for advanced chunk generation
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
    
    // Calculate current simulation time in millions of years
    float simulationTimeMyears = 0.0f;
    switch (currentPhase_) {
        case GeologicalPhase::TECTONICS:
            simulationTimeMyears = currentPhaseProgress_ * 100.0f; // 0-100 million years
            break;
        case GeologicalPhase::MOUNTAIN_BUILDING:
            simulationTimeMyears = 100.0f + (currentPhaseProgress_ * 25.0f); // 100-125 million years
            break;
        case GeologicalPhase::EROSION:
            simulationTimeMyears = 125.0f + (currentPhaseProgress_ * 50.0f); // 125-175 million years
            break;
        case GeologicalPhase::DETAIL:
            simulationTimeMyears = 175.0f + (currentPhaseProgress_ * 10.0f); // 175-185 million years
            break;
    }
    
    snapshotManager_->SetGenerating(true);
    
    // ===== Step 4.1.2: Create snapshot with water system data =====
    snapshotManager_->AddSnapshot(
        *elevationField_,
        *rockTypes_,
        *mantleStress_,
        // Water system fields for visualization
        surfaceWaterDepth_ ? *surfaceWaterDepth_ : *elevationField_, // Fallback to elevation if null
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
        // Fallback to direct geological sampling
        return getSampleAt(x, z).elevation;
    }
    
    const auto* currentSnapshot = snapshotManager_->GetCurrentSnapshot();
    if (!currentSnapshot) {
        // No snapshots available, fallback to direct sampling
        return getSampleAt(x, z).elevation;
    }
    
    return currentSnapshot->GetElevationAt(x, z);
}

// Implementation of simulation methods
void GeologicalSimulator::simulateMantleConvection(float timeStep) {
    // Completely organic, fractal-based mantle convection that preserves irregular continental shapes
    // This approach avoids any wave-based or circular patterns
    
    // Process all points in parallel for efficiency
    std::vector<int> indices(mantleStress_->getHeight() * mantleStress_->getWidth());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / mantleStress_->getWidth();
        int x = idx % mantleStress_->getWidth();
        
        float worldX = x * mantleStress_->getSampleSpacing();
        float worldZ = z * mantleStress_->getSampleSpacing();
        
        // Use fractal noise for completely organic mantle convection patterns
        float scale1 = 0.0002f; // Large scale convection cells
        float scale2 = 0.0008f; // Medium scale variation
        float scale3 = 0.003f;  // Fine scale turbulence
        
        // Multi-scale fractal noise with domain warping for organic patterns
        float warpX = VoxelEngine::Util::smoothValueNoise(worldX * scale1 * 2.0f, 1000.0f + seed_, worldZ * scale1 * 2.0f) * 2000.0f;
        float warpZ = VoxelEngine::Util::smoothValueNoise(worldX * scale1 * 2.0f, 2000.0f + seed_, worldZ * scale1 * 2.0f) * 2000.0f;
        
        float warpedX = worldX + warpX;
        float warpedZ = worldZ + warpZ;
        
        // Large scale convection using warped coordinates
        float largeScale = VoxelEngine::Util::smoothValueNoise(warpedX * scale1, seed_, warpedZ * scale1);
        
        // Medium scale variation
        float mediumScale = VoxelEngine::Util::smoothValueNoise(warpedX * scale2, seed_ + 1000, warpedZ * scale2) * 0.6f;
        
        // Fine scale turbulence
        float fineScale = VoxelEngine::Util::smoothValueNoise(warpedX * scale3, seed_ + 2000, warpedZ * scale3) * 0.3f;
        
        // Combine scales for complex, organic convection patterns
        float convectionStress = (largeScale + mediumScale + fineScale) * 0.4f;
        
        // Apply very gradual changes to preserve organic shapes
        float stressChange = convectionStress * timeStep * 0.05f; // Very gentle changes
        
        mantleStress_->addToSample(x, z, stressChange);
    });
}

void GeologicalSimulator::simulatePlateMovement(float timeStep) {
    // Transfer mantle stress to crust stress with bounds checking
    int height = mantleStress_->getHeight();
    int width = mantleStress_->getWidth();
    
    // Simple sequential processing for now to avoid segfaults
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {        float mantleValue = mantleStress_->getSample(x, z);
        float transferredStress = mantleValue * timeStep * 0.1f; // Reduced transfer rate for realistic changes
        
        // Clamp stress values to prevent overflow
        transferredStress = std::max(-10.0f, std::min(10.0f, transferredStress)); // Reduced clamping range
        crustStress_->addToSample(x, z, transferredStress);
        }
    }
}

void GeologicalSimulator::simulateMountainBuilding(float timeStep) {
    // Completely organic mountain building using fractal stress distribution
    // This avoids circular patterns by using multi-scale fractal modulation
    int height = crustStress_->getHeight();
    int width = crustStress_->getWidth();
    
    // Simple sequential processing for now to avoid segfaults
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float stress = crustStress_->getSample(x, z);
            float rockHard = rockHardness_->getSample(x, z);
            
            if (stress > 0.5f) { // Compression threshold
                float worldX = x * crustStress_->getSampleSpacing();
                float worldZ = z * crustStress_->getSampleSpacing();
                
                // Add fractal modulation to stress distribution for organic mountain formation
                float scale1 = 0.0008f; // Large scale mountain ranges  
                float scale2 = 0.003f;  // Medium scale ridges
                float scale3 = 0.012f;  // Fine scale peaks
                
                // Domain warping for organic stress patterns
                float warpX = VoxelEngine::Util::smoothValueNoise(worldX * scale1 * 0.5f, 11000.0f + seed_, worldZ * scale1 * 0.5f) * 1000.0f;
                float warpZ = VoxelEngine::Util::smoothValueNoise(worldX * scale1 * 0.5f, 12000.0f + seed_, worldZ * scale1 * 0.5f) * 1000.0f;
                
                float warpedX = worldX + warpX;
                float warpedZ = worldZ + warpZ;
                
                // Multi-scale fractal stress modulation
                float stressModulator1 = VoxelEngine::Util::smoothValueNoise(warpedX * scale1, seed_ + 13000, warpedZ * scale1);
                float stressModulator2 = VoxelEngine::Util::smoothValueNoise(warpedX * scale2, seed_ + 14000, warpedZ * scale2) * 0.6f;
                float stressModulator3 = VoxelEngine::Util::smoothValueNoise(warpedX * scale3, seed_ + 15000, warpedZ * scale3) * 0.4f;
                
                // Combine modulators for complex, organic stress distribution
                float fractalStressModifier = (stressModulator1 + stressModulator2 + stressModulator3) * 0.5f + 0.5f;
                
                // Apply fractal-modulated stress for organic mountain building
                float modulated_stress = stress * fractalStressModifier;
                float uplift = modulated_stress * timeStep * 2.0f / rockHard;
                
                // Clamp uplift to prevent unrealistic mountain growth per step
                uplift = std::max(-8.0f, std::min(8.0f, uplift));
                elevationField_->addToSample(x, z, uplift);
                
                // Determine new rock type based on pressure and temperature
                float currentElevation = elevationField_->getSample(x, z);
                
                // CRITICAL: Clamp elevation to realistic mountain/ocean range after every modification
                currentElevation = std::max(-1800.0f, std::min(1200.0f, currentElevation));
                elevationField_->setSample(x, z, currentElevation);
                
                // Validate elevation with enhanced debugging
                validateAndClampElevation(x, z, "Mountain Building");
                
                // Additional safety check - verify clamping worked
                float verifyElevation = elevationField_->getSample(x, z);
                if (verifyElevation < -1800.0f || verifyElevation > 1200.0f) {
                    std::cout << "[ELEVATION BUG] Failed to clamp at (" << x << "," << z << ") - Value: " << verifyElevation << "m" << std::endl;
                    elevationField_->setSample(x, z, std::max(-1800.0f, std::min(1200.0f, verifyElevation)));
                }
                
                // Update rock types based on realistic elevation thresholds
                if (currentElevation > 800.0f && modulated_stress > 3.0f) {
                    rockTypes_->setSample(x, z, RockType::METAMORPHIC_SLATE);
                    rockHardness_->setSample(x, z, 9.0f);
                } else if (currentElevation > 400.0f) {
                    rockTypes_->setSample(x, z, RockType::IGNEOUS_GRANITE);
                    rockHardness_->setSample(x, z, 8.0f);
                }
            }
        }
    }
}

void GeologicalSimulator::simulateVolcanicActivity(float timeStep) {
    // Completely organic, fractal-based volcanic activity that avoids any geometric patterns
    // Uses multi-scale fractal noise with domain warping for completely irregular volcanic distribution
    
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float worldX = x * elevationField_->getSampleSpacing();
        float worldZ = z * elevationField_->getSampleSpacing();
        
        // Multiple scales of fractal noise for completely organic volcanic patterns
        float scale1 = 0.0001f; // Very large scale tectonic hotspots
        float scale2 = 0.0005f; // Regional volcanic zones  
        float scale3 = 0.002f;  // Local volcanic activity
        float scale4 = 0.008f;  // Fine volcanic variation
        
        // Domain warping to create completely irregular patterns
        float warpX1 = VoxelEngine::Util::smoothValueNoise(worldX * scale1 * 2.0f, 3000.0f + seed_, worldZ * scale1 * 2.0f) * 5000.0f;
        float warpZ1 = VoxelEngine::Util::smoothValueNoise(worldX * scale1 * 2.0f, 4000.0f + seed_, worldZ * scale1 * 2.0f) * 5000.0f;
        
        float warpX2 = VoxelEngine::Util::smoothValueNoise(worldX * scale2 * 1.5f, 5000.0f + seed_, worldZ * scale2 * 1.5f) * 2000.0f; 
        float warpZ2 = VoxelEngine::Util::smoothValueNoise(worldX * scale2 * 1.5f, 6000.0f + seed_, worldZ * scale2 * 1.5f) * 2000.0f;
        
        float warpedX = worldX + warpX1 + warpX2;
        float warpedZ = worldZ + warpZ1 + warpZ2;
        
        // Multi-layered fractal volcanic activity using warped coordinates
        float volcanicBase = VoxelEngine::Util::smoothValueNoise(warpedX * scale1, seed_ + 7000, warpedZ * scale1);
        float volcanicRegional = VoxelEngine::Util::smoothValueNoise(warpedX * scale2, seed_ + 8000, warpedZ * scale2) * 0.7f;
        float volcanicLocal = VoxelEngine::Util::smoothValueNoise(warpedX * scale3, seed_ + 9000, warpedZ * scale3) * 0.5f;
        float volcanicFine = VoxelEngine::Util::smoothValueNoise(warpedX * scale4, seed_ + 10000, warpedZ * scale4) * 0.3f;
        
        // Combine all scales for completely organic volcanic intensity
        float volcanicIntensity = (volcanicBase + volcanicRegional + volcanicLocal + volcanicFine) * 0.5f;
        
        // Apply threshold to create sparse, realistic volcanic activity
        if (volcanicIntensity > 0.3f) { // Only in high-intensity fractal zones
            float adjustedIntensity = (volcanicIntensity - 0.3f) / 0.7f; // Normalize to 0-1
            
            // Create realistic volcanic elevation changes
            float volcanoHeight = adjustedIntensity * adjustedIntensity * randomRange(20.0f, 80.0f) * timeStep;
            float currentElevation = elevationField_->getSample(x, z);
            float newElevation = currentElevation + volcanoHeight;
            
            // Clamp to realistic elevation bounds
            newElevation = std::max(-1800.0f, std::min(1200.0f, newElevation)); // Lower max for realistic mountains
            elevationField_->setSample(x, z, newElevation);
            
            // Validate volcanic elevation changes
            if (idx % 500 == 0) { // Sample validation to avoid performance impact
                validateAndClampElevation(x, z, "Volcanic Activity");
            }
            
            // Set volcanic rock type for strong volcanic areas
            if (adjustedIntensity > 0.6f) {
                rockTypes_->setSample(x, z, RockType::IGNEOUS_BASALT);
                rockHardness_->setSample(x, z, 7.0f + adjustedIntensity * 2.0f);
            }
        }
    });
}

void GeologicalSimulator::simulateChemicalWeathering(float timeStep) {
    if (!config_.custom.enableChemicalWeathering) return;
    
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    // Parallel processing of chemical weathering
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        RockType rockType = rockTypes_->getSample(x, z);
        
        // Chemical weathering is stronger in warm, wet conditions
        float temperature = 15.0f - (elevation * 0.006f);
        float rainfall = 500.0f + elevation * 0.1f;
        
        float weatheringRate = 0.0f;
        switch (rockType) {
            case RockType::SEDIMENTARY_LIMESTONE:
                weatheringRate = 0.5f; // Limestone dissolves easily
                break;
            case RockType::IGNEOUS_GRANITE:
                weatheringRate = 0.1f; // Granite is resistant
                break;
            case RockType::SEDIMENTARY_SANDSTONE:
                weatheringRate = 0.3f; // Moderate weathering
                break;
            default:
                weatheringRate = 0.2f;
                break;
        }
        
        if (temperature > 0 && rainfall > 100) {
            float climateMultiplier = (temperature / 30.0f) * (rainfall / 1000.0f);
            float erosion = weatheringRate * timeStep * climateMultiplier * 0.1f; // Reduced erosion rate
            
            // Clamp erosion to prevent extreme changes and respect elevation bounds
            erosion = std::max(0.0f, std::min(10.0f, erosion));
            
            // Ensure we don't erode below deep ocean level
            float currentElevation = elevationField_->getSample(x, z);
            if (currentElevation - erosion < -1800.0f) {
                erosion = std::max(0.0f, currentElevation + 1800.0f);
            }
            
            elevationField_->addToSample(x, z, -erosion);
        }
    });
}

void GeologicalSimulator::simulatePhysicalErosion(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    // Parallel processing of physical erosion
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float waterFlowValue = waterFlow_->getSample(x, z);
        
        // Physical erosion increases with water flow and slope
        if (waterFlowValue > 0.1f) {
            float erosion = waterFlowValue * timeStep * 0.01f; // Much reduced erosion rate
            float flowIncrease = erosion * 0.02f; // Much reduced flow increase
            
            // Prevent erosion below reasonable deep ocean level (-1500m)
            float currentElevation = elevationField_->getSample(x, z);
            if (currentElevation <= -1500.0f) {
                erosion = std::max(0.0f, erosion * 0.1f); // Greatly reduce deep ocean erosion
            }
            
            // Clamp values to prevent overflow and respect elevation bounds
            erosion = std::max(0.0f, std::min(2.0f, erosion));
            flowIncrease = std::max(0.0f, std::min(1.0f, flowIncrease));
            
            // Ensure we don't erode below deep ocean level
            if (currentElevation - erosion < -1800.0f) {
                erosion = std::max(0.0f, currentElevation + 1800.0f);
            }
            
            elevationField_->addToSample(x, z, -erosion);
            waterFlow_->addToSample(x, z, flowIncrease);
            
            // Validate elevation after erosion
            if (idx % 1000 == 0) { // Only validate some samples to avoid performance impact
                validateAndClampElevation(x, z, "Physical Erosion");
            }
        }
    });
}

void GeologicalSimulator::simulateRiverSystems(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Phase 1: Calculate water flow from high to low elevation
    // Process only interior points (excluding boundaries) in parallel
    std::vector<int> indices;
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            indices.push_back(z * width + x);
        }
    }
    
    // Reset water flow field for this timestep
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            waterFlow_->setSample(x, z, 0.0f);
        }
    }
    
    // Calculate water flow directions and accumulate downstream
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float centerElev = elevationField_->getSample(x, z);
        
        // Only calculate flow for land areas (above sea level)
        if (centerElev > 0.0f) {
            // Check all 8 neighbors for steepest descent
            float maxGradient = 0.0f;
            int flowX = x, flowZ = z;
            
            for (int dz = -1; dz <= 1; ++dz) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dz == 0) continue;
                    
                    float neighborElev = elevationField_->getSample(x + dx, z + dz);
                    float gradient = (centerElev - neighborElev) / elevationField_->getSampleSpacing();
                    
                    if (gradient > maxGradient) {
                        maxGradient = gradient;
                        flowX = x + dx;
                        flowZ = z + dz;
                    }
                }
            }
            
            // If water can flow downhill, accumulate flow downstream
            if (maxGradient > 0.001f) { // Minimum gradient for flow
                float baseFlow = std::min(1.0f, maxGradient * 0.1f); // Flow proportional to gradient
                float precipitationFlow = 0.1f; // Base precipitation
                float totalFlow = baseFlow + precipitationFlow;
                
                waterFlow_->addToSample(flowX, flowZ, totalFlow * timeStep);
            }
        }
    });
    
    // Phase 2: Apply water-driven erosion based on accumulated flow
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float currentElevation = elevationField_->getSample(x, z);
        float flow = waterFlow_->getSample(x, z);
        
        // Only erode land areas with significant water flow
        if (currentElevation > 0.0f && flow > 0.05f) {
            RockType rockType = rockTypes_->getSample(x, z);
            float rockHard = rockHardness_->getSample(x, z);
            
            // Erosion rate depends on water flow, rock hardness, and slope
            float slope = 0.0f;
            if (x > 0 && x < width-1 && z > 0 && z < height-1) {
                float dxElev = elevationField_->getSample(x+1, z) - elevationField_->getSample(x-1, z);
                float dzElev = elevationField_->getSample(x, z+1) - elevationField_->getSample(x, z-1);
                slope = std::sqrt(dxElev*dxElev + dzElev*dzElev) / (2.0f * elevationField_->getSampleSpacing());
            }
            
            // Calculate erosion strength
            float flowFactor = std::min(1.0f, flow * 2.0f); // Flow intensity
            float slopeFactor = std::min(1.0f, slope * 0.1f); // Slope enhancement
            float hardnessFactor = 1.0f / std::max(1.0f, rockHard); // Softer rocks erode faster
            
            float erosionRate = flowFactor * (0.3f + slopeFactor) * hardnessFactor * timeStep;
            
            // Apply different erosion rates for different rock types
            switch (rockType) {
                case RockType::SEDIMENTARY_LIMESTONE:
                    erosionRate *= 2.0f; // Limestone erodes faster in water
                    break;
                case RockType::IGNEOUS_GRANITE:
                    erosionRate *= 0.5f; // Granite is more resistant
                    break;
                case RockType::SEDIMENTARY_SANDSTONE:
                    erosionRate *= 1.2f; // Moderate erosion
                    break;
                default:
                    break;
            }
            
            // Clamp erosion to prevent unrealistic carving
            float maxErosion = std::min(5.0f, currentElevation * 0.01f); // Max 1% of elevation per step
            erosionRate = std::min(erosionRate, maxErosion);
            
            // Apply erosion, but don't erode below sea level in upland areas
            float newElevation = currentElevation - erosionRate;
            if (newElevation < 0.0f) {
                newElevation = std::max(0.0f, currentElevation * 0.95f); // Gradual approach to sea level
            }
            
            elevationField_->setSample(x, z, newElevation);
        }
    });
}

void GeologicalSimulator::simulateGlacialCarving(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    // Parallel processing of glacial erosion
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float temperature = 15.0f - (elevation * 0.006f);
        
        // More detailed glacial erosion with varying intensity
        if (elevation > 1200.0f && temperature < -2.0f) {
            // Glacial erosion intensity depends on elevation and temperature
            float glacialIntensity = std::min(1.0f, (elevation - 1200.0f) / 2000.0f);
            float temperatureFactor = std::max(0.1f, (-temperature - 2.0f) / 10.0f);
            float erosion = timeStep * glacialIntensity * temperatureFactor * 1.5f;
            
            // Ensure we don't erode below deep ocean level
            if (elevation - erosion < -1800.0f) {
                erosion = std::max(0.0f, elevation + 1800.0f);
            }
            
            elevationField_->addToSample(x, z, -erosion);
        }
    });
}

void GeologicalSimulator::simulateMicroWeathering(float timeStep) {
    // Fine-scale weathering processes
    // This would be expanded in the HybridDetailGenerator
}

void GeologicalSimulator::simulateSedimentDeposition(float timeStep) {
    int height = waterFlow_->getHeight();
    int width = waterFlow_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    // Parallel processing of sediment deposition
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float flow = waterFlow_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        RockType currentRockType = rockTypes_->getSample(x, z);
        
        // More sophisticated sediment deposition based on multiple factors
        bool isDepositionZone = false;
        float depositionRate = 0.0f;
        
        // River deltas and low flow areas
        if (flow > 0.1f && flow < 0.8f && elevation < 200.0f) {
            depositionRate = (0.8f - flow) * timeStep * 0.15f;
            isDepositionZone = true;
        }
        
        // Lake and ocean floor deposition
        if (elevation < 10.0f && flow < 0.3f) {
            depositionRate = std::max(depositionRate, (0.3f - flow) * timeStep * 0.2f);
            isDepositionZone = true;
        }
        
        // Valley floor accumulation
        if (elevation > 10.0f && elevation < 500.0f && flow < 0.4f) {
            depositionRate = std::max(depositionRate, (0.4f - flow) * timeStep * 0.08f);
            isDepositionZone = true;
        }
        
        if (isDepositionZone && depositionRate > 0.001f) {
            elevationField_->addToSample(x, z, depositionRate);
            
            // Set appropriate sedimentary rock type based on environment
            if (elevation < 50.0f) {
                rockTypes_->setSample(x, z, RockType::SEDIMENTARY_SHALE); // Marine environment
                rockHardness_->setSample(x, z, 3.0f);
            } else {
                rockTypes_->setSample(x, z, RockType::SEDIMENTARY_SANDSTONE); // River/lake environment
                rockHardness_->setSample(x, z, 4.0f);
            }
        }
    });
}

void GeologicalSimulator::simulateJointFormation(float timeStep) {
    // Joint systems form due to stress and cooling
    // This would be expanded in the HybridDetailGenerator for chunk-scale detail
}

void GeologicalSimulator::simulateCaveGeneration(float timeStep) {
    // Cave formation in limestone areas with water flow
    // This would be expanded in the HybridDetailGenerator for actual cave geometry
}

void GeologicalSimulator::updateProgress(float overallProgress, const std::string& processName) {
    // ===== Step 4.1: Updated for Interleaved Architecture =====
    // No longer phase-based, but process-based with overall progress
    
    // Calculate total progress based on current step
    float totalProgress = static_cast<float>(currentStep_) / static_cast<float>(totalSteps_);
    
    // Override with provided progress if it's more accurate
    if (overallProgress >= 0.0f && overallProgress <= 1.0f) {
        totalProgress = overallProgress;
    }
    
    // Calculate time remaining
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime_).count() / 1000.0f;
    float timeRemaining = (totalProgress > 0.01f) ? (elapsed / totalProgress) * (1.0f - totalProgress) : totalSimulationTime_;
    
    if (progressCallback_) {
        PhaseInfo info;
        info.currentPhase = GeologicalPhase::TECTONICS; // Placeholder - all processes active
        info.phaseProgress = totalProgress; // Same as total in interleaved mode
        info.totalProgress = totalProgress;
        info.currentProcess = processName.empty() ? "Interleaved Geological Processes" : processName;
        info.timeRemaining = static_cast<int>(timeRemaining);
        info.metrics = metrics_;
        
        progressCallback_(info);
    }
}

void GeologicalSimulator::updatePerformanceMetrics() {
    // Simple performance monitoring
    metrics_.memoryUsage = 0.5f; // Placeholder - would calculate actual memory usage
    metrics_.totalMemory = 8.0f;
    metrics_.processingRate = 1000000.0f; // 1M samples/sec placeholder
    metrics_.chunkGenerationRate = 0.0f; // Not generating chunks yet
    metrics_.averageChunkTime = 0.0f;
}

float GeologicalSimulator::calculateResistance(float x, float z) const {
    if (rockHardness_) {
        return rockHardness_->sampleAt(x, z);
    }
    return 1.0f;
}

RockType GeologicalSimulator::determineRockType(float elevation, float stress, float temperature) const {
    if (stress > 5.0f && temperature > 500.0f) {
        return RockType::METAMORPHIC_SLATE;
    } else if (elevation < -100.0f) {
        return RockType::IGNEOUS_BASALT;
    } else if (temperature > 1000.0f) {
        return RockType::IGNEOUS_BASALT;
    } else if (elevation < 100.0f) {
        return RockType::SEDIMENTARY_SHALE;
    } else {
        return RockType::IGNEOUS_GRANITE;
    }
}

float GeologicalSimulator::getRockHardness(RockType rockType) const {
    switch (rockType) {
        case RockType::IGNEOUS_GRANITE:
            return 9.0f;  // Very hard
        case RockType::IGNEOUS_BASALT:
            return 8.5f;  // Hard
        case RockType::METAMORPHIC_QUARTZITE:
            return 9.5f;  // Extremely hard
        case RockType::METAMORPHIC_SLATE:
            return 7.0f;  // Moderately hard
        case RockType::SEDIMENTARY_LIMESTONE:
            return 4.0f;  // Soft
        case RockType::SEDIMENTARY_SANDSTONE:
            return 6.0f;  // Medium
        case RockType::SEDIMENTARY_SHALE:
            return 3.0f;  // Very soft
        default:
            return 5.0f;  // Default medium hardness
    }
}

float GeologicalSimulator::random01() {
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng_);
}

float GeologicalSimulator::randomRange(float min, float max) {
    return min + (max - min) * random01();
}

PhaseInfo GeologicalSimulator::getProgressInfo() const {
    PhaseInfo info;
    info.currentPhase = currentPhase_;
    info.phaseProgress = currentPhaseProgress_;
    info.totalProgress = 0.0f; // Would calculate based on current phase
    info.currentProcess = "Geological Simulation";
    info.timeRemaining = 0.0f;
    info.metrics = metrics_;
    return info;
}

void GeologicalSimulator::setProgressCallback(std::function<void(const PhaseInfo&)> callback) {
    progressCallback_ = callback;
}

// Step-based simulation implementation
bool GeologicalSimulator::initializeSimulation() {
    if (simulationInitialized_) {
        return true; // Already initialized
    }
    
    std::cout << "[GeologicalSimulator] Initializing interleaved process simulation..." << std::endl;
    
    // Reset simulation state
    currentStep_ = 0;
    currentPhase_ = GeologicalPhase::TECTONICS; // Keep for UI compatibility, but not used for control flow
    currentPhaseProgress_ = 0.0f;
    simulationComplete_ = false;
    simulationPaused_ = false;
    
    // Calculate total steps based on quality preset
    totalSteps_ = std::max(100, config_.getSimulationSteps()); // Minimum 100 steps for smooth progression
    
    // No more phase-based initialization - all processes run every step
    phaseStep_ = 0;
    totalPhaseSteps_ = totalSteps_; // For UI progress calculation
    
    // Create initial snapshot
    createSnapshot("Interleaved simulation initialized", 0.0f);
    
    simulationInitialized_ = true;
    lastSnapshotTime_ = std::chrono::steady_clock::now();
    
    std::cout << "[GeologicalSimulator] Interleaved simulation initialized (" << totalSteps_ << " total steps)" << std::endl;
    std::cout << "[GeologicalSimulator] Process time scales - Tectonic: " << processTimeScales_.tectonicTimeStep 
              << "yr, Erosion: " << processTimeScales_.erosionTimeStep 
              << "yr, Water: " << processTimeScales_.waterTimeStep 
              << "yr, Detail: " << processTimeScales_.detailTimeStep 
              << "yr, Volcanic: " << processTimeScales_.volcanicTimeStep << "yr" << std::endl;
    return true;
}

bool GeologicalSimulator::stepSimulation() {
    if (!simulationInitialized_ || simulationComplete_ || simulationPaused_) {
        return false; // Can't step if not initialized, complete, or paused
    }
    
    // ===== Step 4.1: NEW INTERLEAVED ARCHITECTURE =====
    // All geological processes run every step with appropriate time scaling
    // This provides maximum realism and immediate feedback loops
    
    // Calculate base time step (represents geological time per simulation step)
    float baseTimeStep = 1000.0f; // 1000 years per step (adjustable)
    
    // === TECTONIC PROCESSES ===
    // Run at their own time scale (usually slower)
    simulateMantleConvection(baseTimeStep * processTimeScales_.tectonicTimeStep);
    simulatePlateMovement(baseTimeStep * processTimeScales_.tectonicTimeStep);
    simulateMountainBuilding(baseTimeStep * processTimeScales_.tectonicTimeStep);
    
    // === VOLCANIC PROCESSES ===
    // Run at their own time scale (episodic, can be intense)
    simulateVolcanicActivity(baseTimeStep * processTimeScales_.volcanicTimeStep);
    
    // === EROSION PROCESSES ===
    // Run at their own time scale (typically faster than tectonics)
    simulateChemicalWeathering(baseTimeStep * processTimeScales_.erosionTimeStep);
    simulatePhysicalErosion(baseTimeStep * processTimeScales_.erosionTimeStep);
    simulateWaterDrivenErosion(baseTimeStep * processTimeScales_.erosionTimeStep);
    simulateSedimentTransport(baseTimeStep * processTimeScales_.erosionTimeStep);
    simulateErosionUpliftBalance(baseTimeStep * processTimeScales_.erosionTimeStep);
    
    // Glacial processes (occasional but intense)
    if (currentStep_ % 5 == 0) { // Every 5th step
        simulateGlacialCarving(baseTimeStep * processTimeScales_.erosionTimeStep * 5.0f);
    }
    
    // === WATER PROCESSES ===
    // Run at their own time scale (typically fast)
    simulatePrecipitationPatterns(baseTimeStep * processTimeScales_.waterTimeStep);
    simulateSurfaceWaterAccumulation(baseTimeStep * processTimeScales_.waterTimeStep);
    simulateRiverFormation(baseTimeStep * processTimeScales_.waterTimeStep);
    simulateRiverSystems(baseTimeStep * processTimeScales_.waterTimeStep);
    
    // Groundwater systems
    simulateGroundwaterTable(baseTimeStep * processTimeScales_.waterTimeStep);
    simulateAquiferRecharge(baseTimeStep * processTimeScales_.waterTimeStep);
    simulateGroundwaterFlow(baseTimeStep * processTimeScales_.waterTimeStep);
    simulateSpringFormation(baseTimeStep * processTimeScales_.waterTimeStep);
    
    // Less frequent water processes
    if (currentStep_ % 3 == 0) { // Every 3rd step
        simulateFloodPlains(baseTimeStep * processTimeScales_.waterTimeStep * 3.0f);
        simulateLakeFormation(baseTimeStep * processTimeScales_.waterTimeStep * 3.0f);
    }
    
    if (currentStep_ % 2 == 0) { // Every 2nd step
        simulateWetlandFormation(baseTimeStep * processTimeScales_.waterTimeStep * 2.0f);
    }
    
    // === CAVE SYSTEMS ===
    // Run when enabled, at water time scale
    if (config_.custom.enableCaveSystems) {
        simulateCaveNetworkGrowth(baseTimeStep * processTimeScales_.waterTimeStep);
        simulateUndergroundRivers(baseTimeStep * processTimeScales_.waterTimeStep);
        
        if (currentStep_ % 4 == 0) { // Every 4th step
            simulateKarstWeathering(baseTimeStep * processTimeScales_.waterTimeStep * 4.0f);
            simulateCaveCollapse(baseTimeStep * processTimeScales_.waterTimeStep * 4.0f);
        }
    }
    
    // === DETAIL PROCESSES ===
    // Run at their own time scale (typically finest detail)
    simulateMicroWeathering(baseTimeStep * processTimeScales_.detailTimeStep);
    simulateSedimentDeposition(baseTimeStep * processTimeScales_.detailTimeStep);
    simulateJointFormation(baseTimeStep * processTimeScales_.detailTimeStep);
    simulateCaveGeneration(baseTimeStep * processTimeScales_.detailTimeStep);
    
    // === STEP TRACKING ===
    currentStep_++;
    
    // Check if simulation is complete
    if (currentStep_ >= totalSteps_) {
        std::cout << "[GeologicalSimulator] Interleaved simulation complete after " 
                  << currentStep_ << " steps" << std::endl;
        createSnapshot("Interleaved geological simulation complete", 100.0f);
        simulationComplete_ = true;
    }
    
    // Calculate overall progress
    float totalProgress = static_cast<float>(currentStep_) / static_cast<float>(totalSteps_);
    
    // Create periodic snapshots for UI preview
    auto now = std::chrono::steady_clock::now();
    float timeSinceLastSnapshot = std::chrono::duration<float>(now - lastSnapshotTime_).count();
    if (timeSinceLastSnapshot >= SNAPSHOT_INTERVAL_SECONDS) {
        std::string description = "Interleaved Simulation (" + 
                                std::to_string(static_cast<int>(totalProgress * 100)) + "%)";
        createSnapshot(description, totalProgress * 100.0f);
        lastSnapshotTime_ = now;
    }
    
    // Call progress callback if set
    if (progressCallback_) {
        PhaseInfo info;
        info.currentPhase = GeologicalPhase::TECTONICS; // Placeholder - all processes run
        info.phaseProgress = totalProgress;
        info.totalProgress = totalProgress;
        info.currentProcess = "Interleaved Geological Processes";
        info.timeRemaining = static_cast<int>((totalSteps_ - currentStep_) * 0.1f); // Estimated
        progressCallback_(info);
    }
    
    return !simulationComplete_;
}

bool GeologicalSimulator::isSimulationComplete() const {
    return simulationComplete_;
}

void GeologicalSimulator::pauseSimulation() {
    if (!simulationPaused_) {
        simulationPaused_ = true;
        hasResumedSincePause_ = false; // Reset flag when actually pausing
        std::cout << "[GeologicalSimulator] Simulation paused" << std::endl;
    }
}

void GeologicalSimulator::resumeSimulation() {
    if (simulationPaused_) {
        simulationPaused_ = false;
        if (!hasResumedSincePause_) {
            std::cout << "[GeologicalSimulator] Simulation resumed" << std::endl;
            hasResumedSincePause_ = true;
        }
    }
}

bool GeologicalSimulator::isSimulationPaused() const {
    return simulationPaused_;
}

std::string GeologicalSimulator::getPhaseDisplayName() const {
    switch (currentPhase_) {
        case GeologicalPhase::TECTONICS: return "Tectonic Evolution";
        case GeologicalPhase::MOUNTAIN_BUILDING: return "Mountain Building";
        case GeologicalPhase::EROSION: return "Erosion & Weathering";
        case GeologicalPhase::DETAIL: return "Surface Detail";
        default: return "Unknown Phase";
    }
}

// Snapshot management methods  
void GeologicalSimulator::createSnapshot(const std::string& description) {
    // Ensure currentPhaseProgress_ is a ratio (0.0 to 1.0) if it's used as completionPercentage
    // Or, better, calculate overall progress if this is for step-based simulation.
    // For now, assuming currentPhaseProgress_ is correctly representing phase-specific progress for this overload.
    // The problematic log output suggests the `completionPercentage` in the other overload needs attention.
    float overallCompletion = 0.0f;
    if (totalSteps_ > 0 && simulationInitialized_) { // Check if step-based sim is active
        overallCompletion = static_cast<float>(currentStep_) / static_cast<float>(totalSteps_);
    } else { // Fallback for non-step-based or pre-init
        overallCompletion = currentPhaseProgress_; // This might be phase progress, not overall
    }
    createSnapshot(description, overallCompletion);
}

bool GeologicalSimulator::hasSnapshots() const {
    return snapshotManager_ && snapshotManager_->GetSnapshotCount() > 0;
}

std::vector<std::string> GeologicalSimulator::getSnapshotDescriptions() const {
    std::vector<std::string> descriptions;
    if (snapshotManager_) {
        for (size_t i = 0; i < snapshotManager_->GetSnapshotCount(); ++i) {
            const auto* snapshot = snapshotManager_->GetSnapshot(i);
            if (snapshot) {
                descriptions.push_back(snapshot->phaseDescription);
            }
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

std::unique_ptr<ContinuousField<GeologicalSample>> GeologicalSimulator::exportGeologicalData() const {
    // For now, return nullptr as this method is only used by tests
    // In a full implementation, we would create a new ContinuousField with proper template instantiation
    std::cout << "[GeologicalSimulator] exportGeologicalData() not fully implemented - returning nullptr" << std::endl;
    return nullptr;
}

// =============================================================================
// Step 3: Water & Cave System Framework Implementation
// =============================================================================

void GeologicalSimulator::simulatePrecipitationPatterns(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float worldX = x * elevationField_->getSampleSpacing();
        float worldZ = z * elevationField_->getSampleSpacing();
        float elevation = elevationField_->getSample(x, z);
        
        // Realistic precipitation patterns based on elevation and geography
        float basePrecipitation = 800.0f; // 800mm baseline
        
        // Orographic effect: mountains get more precipitation
        float elevationEffect = 1.0f + (elevation > 0 ? elevation * 0.0008f : 0.0f); // 0.8mm per meter
        
        // Rain shadow effect using fractal noise
        float rainShadowNoise = VoxelEngine::Util::smoothValueNoise(
            worldX * 0.0001f, 3000.0f + seed_, worldZ * 0.0001f
        );
        float rainShadowEffect = 0.7f + 0.6f * rainShadowNoise; // 0.7 to 1.3 multiplier
        
        // Continental effect: interior areas get less precipitation
        float centerX = worldSizeKm_ * 500.0f; // World center
        float centerZ = worldSizeKm_ * 500.0f;
        float distanceFromCenter = std::sqrt((worldX - centerX) * (worldX - centerX) + 
                                           (worldZ - centerZ) * (worldZ - centerZ));
        float continentalEffect = 1.0f - (distanceFromCenter / (worldSizeKm_ * 500.0f)) * 0.3f; // Reduce by up to 30%
        continentalEffect = std::max(0.4f, continentalEffect); // Minimum 40% of coastal precipitation
        
        // Seasonal variation using fractal noise
        float seasonalNoise = VoxelEngine::Util::smoothValueNoise(
            worldX * 0.0002f, 4000.0f + seed_, worldZ * 0.0002f
        );
        float seasonalEffect = 0.8f + 0.4f * seasonalNoise; // 0.8 to 1.2 multiplier
        
        float totalPrecipitation = basePrecipitation * elevationEffect * rainShadowEffect * 
                                 continentalEffect * seasonalEffect;
        
        // Clamp to realistic values (200mm to 3000mm annually)
        totalPrecipitation = std::max(200.0f, std::min(3000.0f, totalPrecipitation));
        
        precipitationField_->setSample(x, z, totalPrecipitation);
    });
}

void GeologicalSimulator::simulateSurfaceWaterAccumulation(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Reset surface water depth for this timestep
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            surfaceWaterDepth_->setSample(x, z, 0.0f);
        }
    }
    
    // Create index range for parallel processing (excluding boundaries)
    std::vector<int> indices;
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            indices.push_back(z * width + x);
        }
    }
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float precipitation = precipitationField_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        float permeability = permeabilityField_->getSample(x, z);
        
        // Convert annual precipitation to water accumulation per timestep
        float waterInput = (precipitation / 365.0f) * timeStep * 0.001f; // mm to meters per day
        
        // Infiltration reduces surface water (higher permeability = more infiltration)
        float infiltrationRate = permeability * 10.0f; // Convert to m/day
        float infiltration = std::min(waterInput, infiltrationRate * timeStep);
        float surfaceWater = waterInput - infiltration;
        
        // Only positive surface water areas
        if (surfaceWater > 0.0001f) {
            surfaceWaterDepth_->setSample(x, z, surfaceWater);
            
            // Add infiltrated water to groundwater table (raises water table)
            float currentGroundwater = groundwaterTable_->getSample(x, z);
            float newGroundwater = currentGroundwater - (infiltration * 0.1f); // Reduces depth to water table
            groundwaterTable_->setSample(x, z, std::max(0.5f, newGroundwater)); // Minimum 0.5m depth
        }
    });
}

void GeologicalSimulator::simulateRiverFormation(float timeStep) {
    int height = elevationField_->getHeight();  
    int width = elevationField_->getWidth();
    
    // Enhanced river formation that creates persistent channels
    // This replaces the basic flow accumulation with realistic river network development
    
    // Step 1: Initialize flow accumulation array
    std::vector<std::vector<float>> flowAccumulation(height, std::vector<float>(width, 0.0f));
    
    // Step 2: Calculate flow directions using D8 algorithm
    std::vector<std::vector<int>> flowDirection(height, std::vector<int>(width, -1));
    
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float centerElev = elevationField_->getSample(x, z);
            float maxGradient = 0.0f;
            int bestDirection = -1;
            
            // Check 8 neighbors
            int dx[] = {-1, -1, -1,  0,  0,  1,  1,  1};
            int dz[] = {-1,  0,  1, -1,  1, -1,  0,  1};
            
            for (int i = 0; i < 8; ++i) {
                int nx = x + dx[i];
                int nz = z + dz[i];
                
                float neighborElev = elevationField_->getSample(nx, nz);
                float gradient = (centerElev - neighborElev) / elevationField_->getSampleSpacing();
                
                if (gradient > maxGradient) {
                    maxGradient = gradient;
                    bestDirection = i;
                }
            }
            
            flowDirection[z][x] = bestDirection;
        }
    }
    
    // Step 3: Calculate flow accumulation using upstream contributing area
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float precipitation = precipitationField_->getSample(x, z);
            float baseFlow = precipitation * 0.0001f; // Convert mm/year to flow units
            flowAccumulation[z][x] = baseFlow;
        }
    }
    
    // Accumulate flow downstream (simplified single-pass approximation)
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            int direction = flowDirection[z][x];
            if (direction >= 0) {
                int dx[] = {-1, -1, -1,  0,  0,  1,  1,  1};
                int dz[] = {-1,  0,  1, -1,  1, -1,  0,  1};
                
                int nx = x + dx[direction];
                int nz = z + dz[direction];
                
                if (nx >= 1 && nx < width - 1 && nz >= 1 && nz < height - 1) {
                    flowAccumulation[nz][nx] += flowAccumulation[z][x] * 0.9f; // 90% flow transfer
                }
            }
        }
    }
    
    // Step 4: Update water flow field and create persistent river channels
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float newFlow = flowAccumulation[z][x];
            float currentFlow = waterFlow_->getSample(x, z);
            
            // Rivers persist and grow over time (hysteresis effect)
            float persistentFlow = std::max(newFlow, currentFlow * 0.8f); // 80% persistence
            
            // River threshold: areas with significant flow become established rivers
            if (persistentFlow > 0.1f) {
                // Strong rivers carve channels that persist
                persistentFlow += 0.05f; // Channel enhancement
            }
            
            waterFlow_->setSample(x, z, persistentFlow);
        }
    }
}

void GeologicalSimulator::simulateFloodPlains(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float waterFlow = waterFlow_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        float surfaceWater = surfaceWaterDepth_->getSample(x, z);
        
        // Flood plains form adjacent to rivers with high flow
        if (waterFlow > 0.3f && elevation > 50.0f && elevation < 300.0f) {
            // Check if this area is a low-lying area next to a river
            bool nearRiver = false;
            float maxNeighborFlow = 0.0f;
            
            // Check 8 neighbors for river presence

            for (int dz = -1; dz <= 1; ++dz) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dz == 0) continue;
                    
                    int nx = x + dx;
                    int nz = z + dz;
                    
                    if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                        float neighborFlow = waterFlow_->getSample(nx, nz);
                        maxNeighborFlow = std::max(maxNeighborFlow, neighborFlow);
                        if (neighborFlow > 0.5f) {
                            nearRiver = true;
                        }
                    }
                }
            }
            
            // Flood plain characteristics
            if (nearRiver && maxNeighborFlow > waterFlow) {
                // This is a flood plain - periodic flooding deposits sediment
                float floodIntensity = std::min(1.0f, maxNeighborFlow - 0.3f);
                
                // Sediment deposition during floods (builds up flood plains)
                float sedimentDeposition = floodIntensity * timeStep * 0.02f;
                elevationField_->addToSample(x, z, sedimentDeposition);
                
                // Set flood plain rock type
                rockTypes_->setSample(x, z, RockType::SEDIMENTARY_SHALE);
                rockHardness_->setSample(x, z, 2.5f); // Soft flood plain sediments
                
                // Enhanced permeability in flood plains
                permeabilityField_->setSample(x, z, 0.05f); // Higher permeability
            }
        }
    });
}

void GeologicalSimulator::simulateLakeFormation(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float waterFlow = waterFlow_->getSample(x, z);
        float surfaceWater = surfaceWaterDepth_->getSample(x, z);
        
        // Lakes form in natural basins with water accumulation
        bool isBasin = true;
        bool hasWaterInput = (waterFlow > 0.05f || surfaceWater > 0.001f);
        
        // Check if this location is lower than all neighbors (basin test)
        for (int dz = -1; dz <= 1 && isBasin; ++dz) {
            for (int dx = -1; dx <= 1 && isBasin; ++dx) {
                if (dx == 0 && dz == 0) continue;
                
                int nx = x + dx;
                int nz = z + dz;
                
                if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                    float neighborElev = elevationField_->getSample(nx, nz);
                    if (neighborElev <= elevation) {
                        isBasin = false; // Not a local minimum
                    }
                }
            }
        }
        
        // Lake formation conditions
        if (isBasin && hasWaterInput && elevation > 10.0f && elevation < 800.0f) {
            // Calculate lake depth based on water input and basin size
            float lakeDepth = std::min(5.0f, waterFlow * 2.0f + surfaceWater * 100.0f);
            
            if (lakeDepth > 0.5f) {
                // Create lake by adjusting surface water depth
                surfaceWaterDepth_->setSample(x, z, lakeDepth);
                
                // Lakes modify local groundwater table
                float currentGroundwater = groundwaterTable_->getSample(x, z);
                float newGroundwater = std::min(currentGroundwater, lakeDepth * 0.5f); // Lake raises water table
                groundwaterTable_->setSample(x, z, newGroundwater);
                
                // Lake sediment accumulation over time
                if (lakeDepth > 1.0f) {
                    float sedimentAccumulation = timeStep * 0.001f; // Slow lake filling
                    elevationField_->addToSample(x, z, sedimentAccumulation);
                    
                    // Set lake sediment rock type
                    rockTypes_->setSample(x, z, RockType::SEDIMENTARY_SHALE);
                    rockHardness_->setSample(x, z, 1.5f); // Very soft lake sediments
                }
            }
        }
    });
}

// =============================================================================
// Step 3.2: Groundwater Aquifer System Implementation
// =============================================================================

void GeologicalSimulator::simulateGroundwaterTable(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float precipitation = precipitationField_->getSample(x, z);
        float permeability = permeabilityField_->getSample(x, z);
        float currentWaterTable = groundwaterTable_->getSample(x, z);
        
        // Groundwater table simulation based on realistic hydrology
        // Water table depth influenced by:
        // 1. Precipitation recharge
        // 2. Rock permeability  
        // 3. Elevation (gravity flow)
        // 4. Local drainage
        
        // Recharge from precipitation
        float rechargeRate = precipitation * 0.0001f * permeability; // mm/year to m/day
        float recharge = rechargeRate * timeStep;
        
        // Gravity-driven flow (water table follows topography but smoother)
        float gravityEffect = elevation * 0.01f; // Water table roughly follows elevation
        float targetDepth = std::max(2.0f, gravityEffect);
        
        // Drainage to nearby lower areas
        float drainageRate = 0.0f;
        for (int dz = -1; dz <= 1; ++dz) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dz == 0) continue;
                
                int nx = x + dx;
                int nz = z + dz;
                
                if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                    float neighborElev = elevationField_->getSample(nx, nz);
                    float neighborWaterTable = groundwaterTable_->getSample(nx, nz);
                    
                    float waterTableElev = elevation - currentWaterTable;
                    float neighborWaterTableElev = neighborElev - neighborWaterTable;
                    
                    if (waterTableElev > neighborWaterTableElev) {
                        float gradient = (waterTableElev - neighborWaterTableElev) / elevationField_->getSampleSpacing();
                        drainageRate += gradient * permeability * 0.01f;
                    }
                }
            }
        }
        
        // Update water table depth
        float waterTableChange = recharge - (drainageRate * timeStep);
        float newWaterTable = currentWaterTable - waterTableChange; // Negative change raises water table
        
        // Realistic bounds for water table depth
        newWaterTable = std::max(0.5f, std::min(50.0f, newWaterTable));
        
        groundwaterTable_->setSample(x, z, newWaterTable);
    });
}

void GeologicalSimulator::simulateAquiferRecharge(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Aquifer recharge is handled primarily in simulateGroundwaterTable
    // This method handles special recharge scenarios
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float surfaceWater = surfaceWaterDepth_->getSample(x, z);
        float permeability = permeabilityField_->getSample(x, z);
        RockType rockType = rockTypes_->getSample(x, z);
        
        // Enhanced recharge in specific geological conditions
        float enhancedRecharge = 0.0f;
        
        // River and lake recharge (losing streams)
        if (surfaceWater > 0.1f) {
            float leakageRate = surfaceWater * permeability * 0.1f; // m/day
            enhancedRecharge += leakageRate * timeStep;
        }
        
        // Fractured rock enhanced recharge
        if (rockType == RockType::IGNEOUS_GRANITE || rockType == RockType::METAMORPHIC_SLATE) {
            // Fractured crystalline rocks can have enhanced recharge
            float fractureRecharge = permeability * 2.0f * timeStep;
            enhancedRecharge += fractureRecharge;
        }
        
        // Karst recharge (limestone areas)
        if (rockType == RockType::SEDIMENTARY_LIMESTONE) {
            // Limestone has rapid recharge through solution features
            float karstRecharge = permeability * 5.0f * timeStep;
            enhancedRecharge += karstRecharge;
        }
        
        // Apply enhanced recharge to water table
        if (enhancedRecharge > 0.001f) {
            float currentWaterTable = groundwaterTable_->getSample(x, z);
            float newWaterTable = currentWaterTable - (enhancedRecharge * 0.1f); // Raises water table
            newWaterTable = std::max(0.5f, newWaterTable);
            groundwaterTable_->setSample(x, z, newWaterTable);
        }
    });
}

void GeologicalSimulator::simulateGroundwaterFlow(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Groundwater flows from high hydraulic head to low hydraulic head
    // Hydraulic head = elevation - water table depth
    
    // Create temporary array for hydraulic head calculations
    std::vector<std::vector<float>> hydraulicHead(height, std::vector<float>(width, 0.0f));
    
    // Calculate hydraulic head at each point
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float elevation = elevationField_->getSample(x, z);
            float waterTableDepth = groundwaterTable_->getSample(x, z);
            hydraulicHead[z][x] = elevation - waterTableDepth;
        }
    }
    
    // Apply groundwater flow based on hydraulic gradients
    std::vector<int> indices;
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            indices.push_back(z * width + x);
        }
    }
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float centerHead = hydraulicHead[z][x];
        float permeability = permeabilityField_->getSample(x, z);
        float currentWaterTable = groundwaterTable_->getSample(x, z);
        
        // Calculate flow to/from neighbors based on hydraulic gradient
        float totalFlow = 0.0f;
        
        for (int dz = -1; dz <= 1; ++dz) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dz == 0) continue;
                
                int nx = x + dx;
                int nz = z + dz;
                
                float neighborHead = hydraulicHead[nz][nx];
                float gradient = (centerHead - neighborHead) / elevationField_->getSampleSpacing();
                
                // Flow rate proportional to gradient and permeability
                float flowRate = gradient * permeability * 0.001f; // m/day
                totalFlow -= flowRate; // Negative because outflow lowers water table
            }
        }
        
        // Apply groundwater flow to water table
        float waterTableChange = totalFlow * timeStep * 0.1f;
        float newWaterTable = currentWaterTable + waterTableChange;
        
        // Realistic bounds
        newWaterTable = std::max(0.5f, std::min(50.0f, newWaterTable));
        
        groundwaterTable_->setSample(x, z, newWaterTable);
    });
}

void GeologicalSimulator::simulateSpringFormation(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float waterTableDepth = groundwaterTable_->getSample(x, z);
        float permeability = permeabilityField_->getSample(x, z);
        
        // Springs form where groundwater table intersects surface
        // or where permeability contrasts force groundwater to surface
        
        float springFlow = 0.0f;
        
        // Direct intersection springs (water table at or near surface)
        if (waterTableDepth < 2.0f) {
            springFlow = (2.0f - waterTableDepth) * permeability * 0.1f;
        }
        
        // Permeability contrast springs
        float maxNeighborPermeability = permeability;
        float minNeighborPermeability = permeability;
        
        for (int dz = -1; dz <= 1; ++dz) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dz == 0) continue;
                
                int nx = x + dx;
                int nz = z + dz;
                
                if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                    float neighborPerm = permeabilityField_->getSample(nx, nz);
                    maxNeighborPermeability = std::max(maxNeighborPermeability, neighborPerm);
                    minNeighborPermeability = std::min(minNeighborPermeability, neighborPerm);
                }
            }
        }
        
        // Spring forms if this location has low permeability adjacent to high permeability
        float permeabilityContrast = maxNeighborPermeability - permeability;
        if (permeabilityContrast > 0.02f && waterTableDepth < 5.0f) {
            springFlow += permeabilityContrast * (5.0f - waterTableDepth) * 0.05f;
        }
        
        // Set spring flow
        springFlow = std::max(0.0f, std::min(1.0f, springFlow));
        springFlow_->setSample(x, z, springFlow);
        
        // Springs add to surface water flow
        if (springFlow > 0.01f) {
            float currentFlow = waterFlow_->getSample(x, z);
            waterFlow_->setSample(x, z, currentFlow + springFlow * 0.5f);
            
            // Springs also create small amounts of surface water
            float currentSurfaceWater = surfaceWaterDepth_->getSample(x, z);
            surfaceWaterDepth_->setSample(x, z, currentSurfaceWater + springFlow * 0.01f);
        }
    });
}

void GeologicalSimulator::simulateWetlandFormation(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float waterTableDepth = groundwaterTable_->getSample(x, z);
        float surfaceWater = surfaceWaterDepth_->getSample(x, z);
        float springFlow = springFlow_->getSample(x, z);
        float permeability = permeabilityField_->getSample(x, z);
        
        // Wetlands form in areas with:
        // 1. Shallow water table
        // 2. Low permeability (poor drainage)
        // 3. Low elevation areas
        // 4. Spring discharge areas
        
        bool isWetlandCondition = false;
        
        // Shallow water table wetlands
        if (waterTableDepth < 1.5f && permeability < 0.02f) {
            isWetlandCondition = true;
        }
        
        // Spring wetlands
        if (springFlow > 0.05f) {
            isWetlandCondition = true;
        }
        
        // Flat, low-lying wetlands
        if (elevation > 50.0f && elevation < 200.0f) {
            // Check if area is relatively flat
            bool isFlat = true;
            for (int dz = -1; dz <= 1 && isFlat; ++dz) {
                for (int dx = -1; dx <= 1 && isFlat; ++dx) {
                    if (dx == 0 && dz == 0) continue;
                    
                    int nx = x + dx;
                    int nz = z + dz;
                    
                    if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                        float neighborElev = elevationField_->getSample(nx, nz);
                        if (std::abs(elevation - neighborElev) > 5.0f) {
                            isFlat = false;
                        }
                    }
                }
            }
            
            if (isFlat && waterTableDepth < 3.0f && permeability < 0.03f) {
                isWetlandCondition = true;
            }
        }
        
        // Create wetland characteristics
        if (isWetlandCondition) {
            // Wetlands have standing water
            float wetlandDepth = std::max(surfaceWater, 0.1f + (1.5f - waterTableDepth) * 0.2f);
            surfaceWaterDepth_->setSample(x, z, wetlandDepth);
            
            // Wetlands accumulate organic sediments
            float organicAccumulation = timeStep * 0.005f; // Slow peat accumulation
            elevationField_->addToSample(x, z, organicAccumulation);
            
            // Wetland sediments are soft and organic
            rockTypes_->setSample(x, z, RockType::SEDIMENTARY_SHALE); // Closest to organic sediments
            rockHardness_->setSample(x, z, 1.0f); // Very soft
            
            // Wetlands have very low permeability (clay and organic matter)
            permeabilityField_->setSample(x, z, 0.005f);
        }
    });
}

// =============================================================================
// Step 3.3: Underground Cave Network Generation Implementation  
// =============================================================================

void GeologicalSimulator::simulateCaveNetworkGrowth(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Cave formation based on realistic speleogenesis processes
    // Caves form primarily in limestone through carbonic acid dissolution
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        RockType rockType = rockTypes_->getSample(x, z);
        float waterTableDepth = groundwaterTable_->getSample(x, z);
        float groundwaterFlow = 0.0f;
        float currentCaveDensity = caveNetworkDensity_->getSample(x, z);
        
        // Calculate groundwater flow magnitude for this location
        for (int dz = -1; dz <= 1; ++dz) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dz == 0) continue;
                
                int nx = x + dx;
                int nz = z + dz;
                
                if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                    float neighborWaterTable = groundwaterTable_->getSample(nx, nz);
                    float neighborElevation = elevationField_->getSample(nx, nz);
                    
                    float waterTableElev = elevation - waterTableDepth;
                    float neighborWaterTableElev = neighborElevation - neighborWaterTable;
                    
                    float gradient = std::abs(waterTableElev - neighborWaterTableElev) / 
                                   elevationField_->getSampleSpacing();
                    groundwaterFlow += gradient;
                }
            }
        }
        groundwaterFlow /= 8.0f; // Average gradient
        
        // Cave formation conditions
        float caveFormationRate = 0.0f;
        
        // Primary cave formation: limestone + groundwater flow
        if (rockType == RockType::SEDIMENTARY_LIMESTONE) {
            // Limestone dissolution rate depends on groundwater flow and chemistry
            float dissolutionRate = groundwaterFlow * 0.1f;
            
            // Enhanced dissolution in the vadose zone (above water table)
            if (waterTableDepth > 5.0f && waterTableDepth < 30.0f) {
                dissolutionRate *= 2.0f; // Vadose zone enhancement
            }
            
            // Phreatic zone dissolution (at water table)
            if (waterTableDepth > 2.0f && waterTableDepth < 8.0f) {
                dissolutionRate *= 1.5f; // Phreatic zone caves
            }
            
            caveFormationRate = dissolutionRate * timeStep * 0.001f;
        }
        
        // Secondary cave formation: lava tubes in volcanic areas
        if (rockType == RockType::IGNEOUS_BASALT && elevation > 200.0f) {
            // Lava tube formation (simplified)
            float volcanicCaveRate = 0.0001f * timeStep;
            caveFormationRate = std::max(caveFormationRate, volcanicCaveRate);
        }
        
        // Tertiary cave formation: solution caves in other soluble rocks
        if (rockType == RockType::SEDIMENTARY_SANDSTONE && groundwaterFlow > 0.01f) {
            float sandstoneDisolution = groundwaterFlow * 0.02f * timeStep * 0.0005f;
            caveFormationRate = std::max(caveFormationRate, sandstoneDisolution);
        }
        
        // Update cave network density
        float newCaveDensity = currentCaveDensity + caveFormationRate;
        newCaveDensity = std::max(0.0f, std::min(1.0f, newCaveDensity)); // 0-1 scale
        
        caveNetworkDensity_->setSample(x, z, newCaveDensity);
        
        // Cave formation affects surface hydrology
        if (newCaveDensity > 0.1f) {
            // Caves increase effective permeability
            float currentPermeability = permeabilityField_->getSample(x, z);
            float enhancedPermeability = currentPermeability + (newCaveDensity * 0.05f);
            permeabilityField_->setSample(x, z, std::min(0.2f, enhancedPermeability));
            
            // Caves can drain surface water (losing streams)
            float surfaceWater = surfaceWaterDepth_->getSample(x, z);
            if (surfaceWater > 0.01f && newCaveDensity > 0.3f) {
                float drainage = surfaceWater * newCaveDensity * 0.1f;
                surfaceWaterDepth_->setSample(x, z, surfaceWater - drainage);
            }
        }
    });
}

void GeologicalSimulator::simulateUndergroundRivers(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Underground rivers form in well-developed cave systems
    // They follow similar flow patterns to surface rivers but underground
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float caveDensity = caveNetworkDensity_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        float waterTableDepth = groundwaterTable_->getSample(x, z);
        
        // Underground rivers require well-developed cave systems
        if (caveDensity > 0.3f) {
            float waterTableElevation = elevation - waterTableDepth;
            
            // Find steepest underground gradient (following cave networks)
            float maxGradient = 0.0f;
            float undergroundFlow = 0.0f;
            
            for (int dz = -1; dz <= 1; ++dz) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dz == 0) continue;
                    
                    int nx = x + dx;
                    int nz = z + dz;
                    
                    if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                        float neighborCaveDensity = caveNetworkDensity_->getSample(nx, nz);
                        
                        // Underground flow only between well-connected cave areas
                        if (neighborCaveDensity > 0.2f) {
                            float neighborElevation = elevationField_->getSample(nx, nz);
                            float neighborWaterTable = groundwaterTable_->getSample(nx, nz);
                            float neighborWaterTableElev = neighborElevation - neighborWaterTable;
                            
                            float gradient = (waterTableElevation - neighborWaterTableElev) / 
                                           elevationField_->getSampleSpacing();
                            
                            if (gradient > 0) {
                                maxGradient = std::max(maxGradient, gradient);
                                undergroundFlow += gradient * std::min(caveDensity, neighborCaveDensity);
                            }
                        }
                    }
                }
            }
            
            // Underground rivers enhance cave development along their paths
            if (undergroundFlow > 0.01f) {
                float flowEnhancement = undergroundFlow * timeStep * 0.002f;
                float newCaveDensity = caveDensity + flowEnhancement;
                newCaveDensity = std::min(1.0f, newCaveDensity);
                caveNetworkDensity_->setSample(x, z, newCaveDensity);
                
                // Underground rivers can create springs where they intersect surface
                if (waterTableDepth < 3.0f) {
                    float springEnhancement = undergroundFlow * 0.2f;
                    float currentSpringFlow = springFlow_->getSample(x, z);
                    springFlow_->setSample(x, z, currentSpringFlow + springEnhancement);
                }
            }
        }
    });
}

void GeologicalSimulator::simulateKarstWeathering(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Karst weathering creates distinctive surface features in limestone terrains
    // including sinkholes, disappearing streams, and solution valleys
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        RockType rockType = rockTypes_->getSample(x, z);
        float caveDensity = caveNetworkDensity_->getSample(x, z);
        float precipitation = precipitationField_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        
        // Karst weathering primarily affects limestone areas
        if (rockType == RockType::SEDIMENTARY_LIMESTONE) {
            float karstIntensity = 0.0f;
            
            // Surface karst weathering rate depends on:
            // 1. Precipitation (carbonic acid formation)
            // 2. Underlying cave development
            // 3. Temperature (chemical reaction rates)
            
            float precipitationEffect = precipitation / 1000.0f; // Normalize
            float caveEffect = caveDensity * 2.0f; // Caves enhance surface weathering
            float temperatureEffect = 1.0f + (elevation > 0 ? -elevation * 0.0001f : 0.0f); // Cooler at altitude
            
            karstIntensity = precipitationEffect * (1.0f + caveEffect) * temperatureEffect;
            
            // Apply karst weathering
            float weatheringRate = karstIntensity * timeStep * 0.05f;
            
            // Create solution features
            if (weatheringRate > 0.001f) {
                // General surface lowering
                elevationField_->addToSample(x, z, -weatheringRate);
                
                // Sinkhole formation in areas with high cave density
                if (caveDensity > 0.5f && weatheringRate > 0.005f) {
                    // Accelerated sinkhole development
                    float sinkholeRate = weatheringRate * caveDensity * 2.0f;
                    elevationField_->addToSample(x, z, -sinkholeRate);
                    
                    // Sinkholes drain surface water
                    float surfaceWater = surfaceWaterDepth_->getSample(x, z);
                    if (surfaceWater > 0.001f) {
                        float drainage = surfaceWater * caveDensity * 0.5f;
                        surfaceWaterDepth_->setSample(x, z, surfaceWater - drainage);
                    }
                }
                
                // Enhanced cave development from surface karst processes
                float caveEnhancement = weatheringRate * 0.1f;
                float newCaveDensity = caveDensity + caveEnhancement;
                newCaveDensity = std::min(1.0f, newCaveDensity);
                caveNetworkDensity_->setSample(x, z, newCaveDensity);
            }
            
            // Karst areas have distinctive hydrology
            if (caveDensity > 0.2f) {
                // Disappearing streams - surface water disappears into caves
                float waterFlow = waterFlow_->getSample(x, z);
                if (waterFlow > 0.1f) {
                    float disappearance = waterFlow * caveDensity * 0.3f;
                    waterFlow_->setSample(x, z, waterFlow - disappearance);
                    
                    // This water reappears as springs elsewhere (simplified)
                    // In reality, it would flow through the cave system
                }
                
                // Enhanced permeability in karst areas
                float currentPermeability = permeabilityField_->getSample(x, z);
                float karstPermeability = currentPermeability + (caveDensity * 0.1f);
                permeabilityField_->setSample(x, z, std::min(0.3f, karstPermeability));
            }
        }
    });
}

void GeologicalSimulator::simulateCaveCollapse(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Cave collapse creates surface depressions and modifies cave networks
    // This is a natural process in mature cave systems
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float caveDensity = caveNetworkDensity_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        float waterTableDepth = groundwaterTable_->getSample(x, z);
        RockType rockType = rockTypes_->getSample(x, z);
        
        // Cave collapse likelihood depends on:
        // 1. Cave development (more caves = higher collapse risk)
        // 2. Rock strength (weaker rocks collapse more easily)  
        // 3. Cave depth (shallow caves more likely to collapse)
        // 4. Groundwater changes (water table drops increase collapse risk)
        
        if (caveDensity > 0.3f) {
            float collapseRisk = 0.0f;
            
            // Cave density effect (more caves = more collapse risk)
            collapseRisk += caveDensity * 0.001f;
            
            // Rock strength effect
            float rockStrength = rockHardness_->getSample(x, z);
            collapseRisk += (10.0f - rockStrength) * 0.0001f; // Weaker rocks more prone to collapse
            
            // Depth effect (shallow caves more unstable)
            if (waterTableDepth > 10.0f) {
                collapseRisk += (waterTableDepth - 10.0f) * 0.00005f;
            }
            
            // Water table changes (dropping water table destabilizes caves)
            // This would require tracking water table changes over time
            // For now, use a simplified random component
            float randomFactor = VoxelEngine::Util::smoothValueNoise(
                elevation * 0.001f, 5000.0f + seed_, waterTableDepth * 0.1f
            );
            if (randomFactor > 0.8f) {
                collapseRisk *= 1.5f; // Occasional higher collapse risk
            }
            
            // Apply cave collapse
            float collapseRate = collapseRisk * timeStep;
            
            if (collapseRate > 0.0001f) {
                // Surface subsidence from cave collapse
                float subsidence = collapseRate * caveDensity * 5.0f; // Amplify surface effect
                elevationField_->addToSample(x, z, -subsidence);
                
                // Collapse reduces cave density but doesn't eliminate it entirely
                float densityReduction = collapseRate * 2.0f;
                float newCaveDensity = caveDensity - densityReduction;
                newCaveDensity = std::max(0.1f, newCaveDensity); // Some caves always remain
                caveNetworkDensity_->setSample(x, z, newCaveDensity);
                
                // Collapse can create new surface depressions that collect water
                if (subsidence > 0.01f) {
                    // Depression becomes a water collection point
                    float waterAccumulation = subsidence * 0.1f;
                    float currentSurfaceWater = surfaceWaterDepth_->getSample(x, z);
                    surfaceWaterDepth_->setSample(x, z, currentSurfaceWater + waterAccumulation);
                    
                    // Collapsed areas often have different rock properties
                    rockTypes_->setSample(x, z, RockType::SEDIMENTARY_SHALE); // Collapsed/fractured rock
                    rockHardness_->setSample(x, z, rockStrength * 0.7f); // Weakened by collapse
                }
                
                // Major collapses can affect groundwater flow
                if (subsidence > 0.05f) {
                    // Create local groundwater depression
                    float waterTableEffect = subsidence * 0.2f;
                    float currentWaterTable = groundwaterTable_->getSample(x, z);
                    groundwaterTable_->setSample(x, z, currentWaterTable + waterTableEffect); // Deeper water table
                }
            }
        }
    });
}

// =============================================================================
// Step 3.4 & 3.5: Integrated Water-Erosion & Sediment Transport System Implementation
// =============================================================================

void GeologicalSimulator::simulateSedimentTransport(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Reset sediment load for this timestep
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            sedimentLoad_->setSample(x, z, 0.0f);
        }
    }
    
    // Phase 1: Sediment pickup from erosion sources
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float waterFlow = waterFlow_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        RockType rockType = rockTypes_->getSample(x, z);
        float rockHardness = rockHardness_->getSample(x, z);
        
        // Sediment pickup capacity depends on water velocity and volume
        if (waterFlow > 0.05f && elevation > 0.0f) {
            // Calculate slope for velocity estimation
            float slope = 0.0f;
            if (x > 0 && x < width-1 && z > 0 && z < height-1) {
                float dxElev = elevationField_->getSample(x+1, z) - elevationField_->getSample(x-1, z);
                float dzElev = elevationField_->getSample(x, z+1) - elevationField_->getSample(x, z-1);
                slope = std::sqrt(dxElev*dxElev + dzElev*dzElev) / (2.0f * elevationField_->getSampleSpacing());
            }
            
            // Sediment pickup capacity (Hjulström-Sundborg curve approximation)
            float velocity = std::sqrt(waterFlow * slope * 9.81f); // Simple velocity estimate
            float pickupCapacity = velocity * velocity * waterFlow * 0.001f; // kg/m³
            
            // Rock erodibility affects pickup rate
            float erodibility = 1.0f / std::max(1.0f, rockHardness);
            switch (rockType) {
                case RockType::SEDIMENTARY_SHALE:
                    erodibility *= 3.0f; // Shale erodes easily
                    break;
                case RockType::SEDIMENTARY_SANDSTONE:
                    erodibility *= 2.0f; // Sandstone moderately erodible
                    break;
                case RockType::SEDIMENTARY_LIMESTONE:
                    erodibility *= 1.5f; // Limestone chemically erodible
                    break;
                case RockType::IGNEOUS_GRANITE:
                    erodibility *= 0.5f; // Granite very resistant
                    break;
                case RockType::IGNEOUS_BASALT:
                    erodibility *= 0.7f; // Basalt moderately resistant
                    break;
                default:
                    break;
            }
            
            float actualPickup = pickupCapacity * erodibility * timeStep;
            
            // Clamp pickup to prevent unrealistic erosion
            actualPickup = std::max(0.0f, std::min(0.1f, actualPickup));
            
            sedimentLoad_->setSample(x, z, actualPickup);
            
            // Erode elevation based on sediment pickup
            float erosionDepth = actualPickup * 0.01f; // Convert to elevation loss
            float currentElevation = elevationField_->getSample(x, z);
            
            // Don't erode below reasonable levels
            if (currentElevation - erosionDepth > -1800.0f) {
                elevationField_->addToSample(x, z, -erosionDepth);
            }
        }
    });
    
    // Phase 2: Sediment transport downstream
    // This is simplified - in reality, sediment would be tracked as it moves
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float currentSediment = sedimentLoad_->getSample(x, z);
            float waterFlow = waterFlow_->getSample(x, z);
            
            if (currentSediment > 0.001f && waterFlow > 0.05f) {
                // Find downstream direction (steepest descent)
                float centerElev = elevationField_->getSample(x, z);
                float maxGradient = 0.0f;
                int bestX = x, bestZ = z;
                
                for (int dz = -1; dz <= 1; ++dz) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dz == 0) continue;
                        
                        int nx = x + dx;
                        int nz = z + dz;
                        
                        float neighborElev = elevationField_->getSample(nx, nz);
                        float gradient = (centerElev - neighborElev) / elevationField_->getSampleSpacing();
                        
                        if (gradient > maxGradient) {
                            maxGradient = gradient;
                            bestX = nx;
                            bestZ = nz;
                        }
                    }
                }
                
                // Transport sediment downstream
                if (maxGradient > 0.001f && bestX != x && bestZ != z) {
                    float transportedSediment = currentSediment * 0.8f; // 80% transport efficiency
                    float downstreamSediment = sedimentLoad_->getSample(bestX, bestZ);
                    sedimentLoad_->setSample(bestX, bestZ, downstreamSediment + transportedSediment);
                    sedimentLoad_->setSample(x, z, currentSediment * 0.2f); // 20% remains
                }
            }
        }
    }
}

void GeologicalSimulator::simulateWaterDrivenErosion(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Enhanced water-driven erosion that provides proper counterbalance to mountain building
    // This is the key system that prevents terrain flattening by creating equilibrium
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float waterFlow = waterFlow_->getSample(x, z);
        float elevation = elevationField_->getSample(x, z);
        float precipitation = precipitationField_->getSample(x, z);
        RockType rockType = rockTypes_->getSample(x, z);
        float rockHardness = rockHardness_->getSample(x, z);
        float caveDensity = caveNetworkDensity_->getSample(x, z);
        
        // Multiple erosion processes work together
        float totalErosion = 0.0f;
        
        // 1. Stream power erosion (dominant in high flow areas)
        if (waterFlow > 0.05f && elevation > 0.0f) {
            // Calculate local slope
            float slope = 0.0f;
            if (x > 0 && x < width-1 && z > 0 && z < height-1) {
                float dxElev = elevationField_->getSample(x+1, z) - elevationField_->getSample(x-1, z);
                float dzElev = elevationField_->getSample(x, z+1) - elevationField_->getSample(x, z-1);
                slope = std::sqrt(dxElev*dxElev + dzElev*dzElev) / (2.0f * elevationField_->getSampleSpacing());
            }
            
            // Stream power law: E = K * A^m * S^n (simplified)
            float drainageArea = waterFlow; // Proxy for drainage area
            float streamPower = drainageArea * slope;
            
            float erodibility = 1.0f / std::max(1.0f, rockHardness);
            float streamErosion = streamPower * erodibility * timeStep * 0.01f;
            
            // Rock type modifiers
            switch (rockType) {
                case RockType::SEDIMENTARY_SHALE:
                    streamErosion *= 2.5f; // Very erodible
                    break;
                case RockType::SEDIMENTARY_SANDSTONE:
                    streamErosion *= 1.8f; // Moderately erodible
                    break;
                case RockType::SEDIMENTARY_LIMESTONE:
                    streamErosion *= 1.3f; // Chemical dissolution
                    break;
                case RockType::IGNEOUS_GRANITE:
                    streamErosion *= 0.4f; // Very resistant
                    break;
                case RockType::IGNEOUS_BASALT:
                    streamErosion *= 0.6f; // Resistant
                    break;
                default:
                    break;
            }
            
            totalErosion += streamErosion;
        }
        
        // 2. Chemical weathering enhanced by water
        if (precipitation > 500.0f) {
            float temperature = 15.0f - (elevation * 0.006f); // Temperature lapse rate
            if (temperature > 0.0f) {
                float chemicalRate = 0.0f;
                
                switch (rockType) {
                    case RockType::SEDIMENTARY_LIMESTONE:
                        chemicalRate = 0.8f; // Carbonic acid dissolution
                        break;
                    case RockType::IGNEOUS_GRANITE:
                        chemicalRate = 0.2f; // Feldspar weathering
                        break;
                    case RockType::IGNEOUS_BASALT:
                        chemicalRate = 0.4f; // Olivine/pyroxene weathering
                        break;
                    case RockType::SEDIMENTARY_SANDSTONE:
                        chemicalRate = 0.3f; // Cement dissolution
                        break;
                    case RockType::SEDIMENTARY_SHALE:
                        chemicalRate = 0.6f; // Clay mineral alteration
                        break;
                    default:
                        chemicalRate = 0.3f;
                        break;
                }
                
                float climateModifier = (temperature / 25.0f) * (precipitation / 1000.0f);
                float chemicalErosion = chemicalRate * climateModifier * timeStep * 0.005f;
                
                totalErosion += chemicalErosion;
            }
        }
        
        // 3. Enhanced erosion in cave areas (subsurface piping)
        if (caveDensity > 0.1f) {
            float subsidenceErosion = caveDensity * timeStep * 0.002f;
            totalErosion += subsidenceErosion;
        }
        
        // 4. Elevation-dependent erosion (higher elevations erode faster)
        if (elevation > 500.0f) {
            float elevationEffect = (elevation - 500.0f) / 1000.0f; // 0-1 for 500-1500m
            float elevationErosion = elevationEffect * timeStep * 0.008f;
            totalErosion += elevationErosion;
        }
        
        // 5. Freeze-thaw weathering at high elevations
        if (elevation > 1000.0f) {
            float temperature = 15.0f - (elevation * 0.006f);
            if (temperature < 5.0f && temperature > -10.0f) { // Freeze-thaw zone
                float freezeThawIntensity = 1.0f - std::abs(temperature) / 5.0f;
                float mechanicalErosion = freezeThawIntensity * timeStep * 0.003f;
                totalErosion += mechanicalErosion;
            }
        }
        
        // Apply total erosion with realistic constraints
        totalErosion = std::max(0.0f, std::min(5.0f, totalErosion)); // Max 5m per timestep
        
        // Critical balance: erosion should balance mountain building
        // This prevents terrain flattening by providing proper counterforce
        if (totalErosion > 0.001f) {
            float currentElevation = elevationField_->getSample(x, z);
            
            // Don't erode below deep ocean floor
            if (currentElevation - totalErosion > -1800.0f) {
                elevationField_->addToSample(x, z, -totalErosion);
                
                // Erosion creates sediment that gets transported
                float sedimentProduced = totalErosion * 0.8f; // 80% becomes transportable sediment
                float currentSediment = sedimentLoad_->getSample(x, z);
                sedimentLoad_->setSample(x, z, currentSediment + sedimentProduced);
            }
        }
    });
}

void GeologicalSimulator::simulateErosionUpliftBalance(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // This method implements the critical erosion-uplift equilibrium that prevents terrain flattening
    // It ensures that erosion rates balance tectonic uplift rates for stable topography
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float elevation = elevationField_->getSample(x, z);
        float waterFlow = waterFlow_->getSample(x, z);
        float tectonicStress = mantleStress_->getSample(x, z) + crustStress_->getSample(x, z);
        float rockHardness = rockHardness_->getSample(x, z);
        
        // Calculate local tectonic uplift rate (from ongoing mountain building)
        float upliftRate = 0.0f;
        if (tectonicStress > 0.5f && elevation > 100.0f) {
            upliftRate = tectonicStress * timeStep * 0.1f / std::max(1.0f, rockHardness);
            upliftRate = std::max(0.0f, std::min(2.0f, upliftRate)); // Max 2m uplift per step
        }
        
        // Calculate local erosion rate (from all erosion processes)
        float erosionRate = 0.0f;
        
        // Base erosion from water flow
        if (waterFlow > 0.01f) {
            erosionRate += waterFlow * timeStep * 0.05f;
        }
        
        // Elevation-dependent erosion (gravitational potential energy)
        if (elevation > 200.0f) {
            float gravitationalErosion = (elevation - 200.0f) * 0.00001f * timeStep;
            erosionRate += gravitationalErosion;
        }
        
        // Climate-dependent erosion
        float precipitation = precipitationField_->getSample(x, z);
        if (precipitation > 600.0f) {
            float climateErosion = (precipitation - 600.0f) * 0.000001f * timeStep;
            erosionRate += climateErosion;
        }
        
        // Calculate equilibrium elevation tendency
        float netChange = upliftRate - erosionRate;
        
        // Apply equilibrium feedback to maintain stable topography
        if (std::abs(netChange) > 0.001f) {
            // If uplift > erosion, enhance erosion processes
            if (netChange > 0.0f && elevation > 300.0f) {
                float erosionEnhancement = netChange * 0.5f; // Negative feedback
                elevationField_->addToSample(x, z, -erosionEnhancement);
                
                // Enhanced erosion increases water flow (creates more erosive channels)
                float flowEnhancement = erosionEnhancement * 0.1f;
                float currentFlow = waterFlow_->getSample(x, z);
                waterFlow_->setSample(x, z, currentFlow + flowEnhancement);
            }
            
            // If erosion > uplift, reduce erosion efficiency in low areas
            else if (netChange < 0.0f && elevation < 100.0f) {
                float protectionFactor = (100.0f - elevation) / 100.0f; // Protect low areas
                float erosionReduction = std::abs(netChange) * protectionFactor * 0.3f;
                elevationField_->addToSample(x, z, erosionReduction);
            }
        }
        
        // Long-term equilibrium: mountains should stabilize at realistic heights
        if (elevation > 1000.0f) {
            // Very high mountains have enhanced erosion (alpine processes)
            float alpineErosion = (elevation - 1000.0f) * 0.00005f * timeStep;
            elevationField_->addToSample(x, z, -alpineErosion);
        }
        
        // Isostatic response preview (will be enhanced in Step 4)
        if (elevation > 800.0f) {
            // Heavy mountains start to sink slightly (isostatic depression)
            float isostaticSinking = (elevation - 800.0f) * 0.00001f * timeStep;
            elevationField_->addToSample(x, z, -isostaticSinking);
        }
    });
}

} // namespace World
} // namespace VoxelCastle
