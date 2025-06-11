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
      currentPhaseProgress_(0.0f), continentGenerator_(0) {  // Initialize with temporary seed 0
    
    std::cout << "[GeologicalSimulator] Initialized for " << worldSizeKm_ << "km world with " 
              << (config.preset == GeologicalPreset::BALANCED ? "BALANCED" : "OTHER") << " quality" << std::endl;
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
    
    // Create continuous fields
    mantleStress_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    crustStress_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    elevationField_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    rockHardness_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    waterFlow_ = std::make_unique<ContinuousField<float>>(resolution, resolution, spacing);
    rockTypes_ = std::make_unique<ContinuousField<RockType>>(resolution, resolution, spacing);
    
    // Initialize with base values
    mantleStress_->fill(0.0f);
    crustStress_->fill(0.0f);
    elevationField_->fill(0.0f); // Sea level
    rockHardness_->fill(5.0f);   // Medium hardness
    waterFlow_->fill(0.0f);
    rockTypes_->fill(RockType::IGNEOUS_GRANITE);
    
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
        updateProgress(progress, "Water Erosion & River Formation");
        
        // Simulate erosion processes
        simulateChemicalWeathering(timeStep);
        simulatePhysicalErosion(timeStep);
        simulateRiverSystems(timeStep);
        
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
        
        // Calculate derived properties
        sample.temperature = 15.0f - (sample.elevation * 0.006f); // Temperature lapse rate
        sample.rainfall = 500.0f + sample.elevation * 0.1f; // Orographic precipitation
        
        // Geological features based on rock type and conditions
    sample.hasJointSystems = (sample.rockType == RockType::IGNEOUS_GRANITE) && config_.custom.enableJointSystems;
    sample.hasCaveSystems = (sample.rockType == RockType::SEDIMENTARY_LIMESTONE) && config_.custom.enableCaveSystems;
    sample.hasQuartzVeins = (sample.rockType == RockType::IGNEOUS_GRANITE) && (sample.stressField > 2.0f);
        sample.weatheringIntensity = std::max(0.1f, sample.rainfall / 1000.0f);
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
        case GeologicalPhase::EROSION:
            simulationTimeMyears = 100.0f + (currentPhaseProgress_ * 50.0f); // 100-150 million years
            break;
        case GeologicalPhase::DETAIL:
            simulationTimeMyears = 150.0f + (currentPhaseProgress_ * 10.0f); // 150-160 million years
            break;
    }
    
    snapshotManager_->SetGenerating(true);
    
    // Create snapshot from current geological state
    snapshotManager_->AddSnapshot(
        *elevationField_,
        *rockTypes_,
        *mantleStress_,
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
    // Wave-based convection that naturally works with toroidal topology
    // This creates seamless, realistic geological patterns without convergence artifacts
    
    // Seed-based wave parameters for variety
    int numWavesX = 2 + (seed_ % 3);       // 2-4 waves horizontally
    int numWavesZ = 2 + ((seed_ >> 8) % 3); // 2-4 waves vertically
    float phaseX = (seed_ % 1000) * 0.001f * 2.0f * M_PI;
    float phaseZ = ((seed_ >> 16) % 1000) * 0.001f * 2.0f * M_PI;
    
    // Process all points in parallel for efficiency
    std::vector<int> indices(mantleStress_->getHeight() * mantleStress_->getWidth());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / mantleStress_->getWidth();
        int x = idx % mantleStress_->getWidth();
        
        float worldX = x * mantleStress_->getSampleSpacing();
        float worldZ = z * mantleStress_->getSampleSpacing();
        
        // Normalize coordinates to 0-1 for wave calculations
        float normX = worldX / (worldSizeKm_ * 1000.0f);
        float normZ = worldZ / (worldSizeKm_ * 1000.0f);
        
        // Create layered geological patterns with noise-based organic variation
        float stress = 0.0f;
        
        // Add seed-based noise for organic base variation
        float seedX = normX + (seed_ % 10000) * 0.0001f;
        float seedZ = normZ + ((seed_ >> 16) % 10000) * 0.0001f;
        
        // Organic noise-based base stress (primary geological variation)
        float noiseBase = VoxelEngine::Util::smoothValueNoise(seedX * 8.0f, 0.0f, seedZ * 8.0f);
        float noiseDetail = VoxelEngine::Util::smoothValueNoise(seedX * 16.0f, 42.0f, seedZ * 16.0f) * 0.5f;
        float noiseFine = VoxelEngine::Util::smoothValueNoise(seedX * 32.0f, 123.0f, seedZ * 32.0f) * 0.25f;
        stress += (noiseBase + noiseDetail + noiseFine) * 0.6f;
        
        // Gentle large-scale waves (much more organic than pure sine/cosine)
        float waveX = normX * numWavesX + VoxelEngine::Util::smoothValueNoise(seedX * 4.0f, 456.0f, seedZ * 4.0f) * 0.3f;
        float waveZ = normZ * numWavesZ + VoxelEngine::Util::smoothValueNoise(seedX * 3.0f, 789.0f, seedZ * 3.0f) * 0.3f;
        
        float primaryWave = std::sin(waveX * 2.0f * M_PI + phaseX) *
                           std::cos(waveZ * 2.0f * M_PI + phaseZ);
        stress += primaryWave * 0.3f; // Reduced from 0.8f to reduce artificial patterns
        
        // Medium-scale variation with noise modulation
        float mediumNoise = VoxelEngine::Util::smoothValueNoise(seedX * 12.0f, 321.0f, seedZ * 12.0f);
        stress += mediumNoise * 0.2f;
        
        // Apply realistic mantle convection strength
        stress *= timeStep * 0.25f; // Reduced from 0.3f for more gradual, organic changes
        
        mantleStress_->addToSample(x, z, stress);
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
    // Convert crust stress to elevation changes with bounds checking
    int height = crustStress_->getHeight();
    int width = crustStress_->getWidth();
    
    // Simple sequential processing for now to avoid segfaults
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float stress = crustStress_->getSample(x, z);
            float rockHard = rockHardness_->getSample(x, z);
                 if (stress > 0.5f) { // Compression threshold
            float uplift = stress * timeStep * 8.0f / rockHard; // Increased from 2.5f to 8.0f for more visible elevation changes
            
            // Clamp uplift to prevent unrealistic mountain growth per step
            uplift = std::max(-30.0f, std::min(30.0f, uplift)); // Increased from ±15.0f to ±30.0f
            elevationField_->addToSample(x, z, uplift);
            
            // Determine new rock type based on pressure and temperature
            float currentElevation = elevationField_->getSample(x, z);
            
            // Clamp elevation to realistic mountain/ocean range (-1800m to +1800m)
            currentElevation = std::max(-1800.0f, std::min(1800.0f, currentElevation));
            elevationField_->setSample(x, z, currentElevation);
            
            // Update rock types based on realistic elevation thresholds
            if (currentElevation > 1000.0f && stress > 3.0f) {
                rockTypes_->setSample(x, z, RockType::METAMORPHIC_SLATE);
                rockHardness_->setSample(x, z, 9.0f);
            } else if (currentElevation > 500.0f) {
                rockTypes_->setSample(x, z, RockType::IGNEOUS_GRANITE);
                rockHardness_->setSample(x, z, 8.0f);
            }
        }
        }
    }
}

void GeologicalSimulator::simulateVolcanicActivity(float timeStep) {
    // Wave-based volcanic activity that creates linear volcanic chains
    // This avoids circular patterns and works beautifully with toroidal topology
    
    // Seed-based parameters for volcanic distribution
    int numChains = 1 + (seed_ % 3);  // 1-3 volcanic chains
    float chainAngle = (seed_ % 1000) * 0.001f * 2.0f * M_PI; // Base angle for first chain
    
    for (int chain = 0; chain < numChains; ++chain) {
        // Each chain has a different angle based on seed
        float angle = chainAngle + (chain * M_PI / numChains);
        
        // Create linear volcanic activity using sine waves
        int height = elevationField_->getHeight();
        int width = elevationField_->getWidth();
        
        std::vector<int> indices(height * width);
        std::iota(indices.begin(), indices.end(), 0);
        
        std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
            int z = idx / width;
            int x = idx % width;
            
            float worldX = x * elevationField_->getSampleSpacing();
            float worldZ = z * elevationField_->getSampleSpacing();
            
            // Normalize coordinates
            float normX = worldX / (worldSizeKm_ * 1000.0f);
            float normZ = worldZ / (worldSizeKm_ * 1000.0f);
            
            // Create linear volcanic activity along specific directions
            float distanceFromChain = std::abs(
                normX * std::cos(angle + M_PI/2) + 
                normZ * std::sin(angle + M_PI/2)
            );
            
            // Volcanic activity intensity based on distance from chain
            float volcanicIntensity = std::exp(-distanceFromChain * distanceFromChain * 50.0f);
            
            // Add variation along the chain using waves
            float chainPosition = normX * std::cos(angle) + normZ * std::sin(angle);
            float chainVariation = std::sin(chainPosition * 4.0f * M_PI + (seed_ % 100) * 0.01f);
            volcanicIntensity *= (0.5f + 0.5f * chainVariation);
            
            // Only create volcanic features where intensity is significant
            if (volcanicIntensity > 0.1f) {
                float volcanoHeight = volcanicIntensity * randomRange(50.0f, 200.0f) * timeStep; // Reduced max volcanic height
                float currentElevation = elevationField_->getSample(x, z);
                float newElevation = currentElevation + volcanoHeight;
                
                // Clamp to elevation bounds
                newElevation = std::max(-1800.0f, std::min(1800.0f, newElevation));
                elevationField_->setSample(x, z, newElevation);
                
                // Set volcanic rock type for strong volcanic areas
                if (volcanicIntensity > 0.3f) {
                    rockTypes_->setSample(x, z, RockType::IGNEOUS_BASALT);
                    rockHardness_->setSample(x, z, 7.0f + volcanicIntensity * 2.0f);
                }
            }
        });
    }
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
        }
    });
}

void GeologicalSimulator::simulateRiverSystems(float timeStep) {
    int height = elevationField_->getHeight();
    int width = elevationField_->getWidth();
    
    // Simple river flow simulation - water flows from high to low elevation
    // Process only interior points (excluding boundaries) in parallel
    std::vector<int> indices;
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            indices.push_back(z * width + x);
        }
    }
    
    // Parallel processing of river flow
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float centerElev = elevationField_->getSample(x, z);
        
        // Check all 8 neighbors for flow direction
        float minElev = centerElev;
        int flowX = x, flowZ = z;
        
        for (int dz = -1; dz <= 1; ++dz) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dz == 0) continue;
                
                float neighborElev = elevationField_->getSample(x + dx, z + dz);
                if (neighborElev < minElev) {
                    minElev = neighborElev;
                    flowX = x + dx;
                    flowZ = z + dz;
                }
            }
        }
        
        // If water can flow, increase flow in that direction
        if (flowX != x || flowZ != z) {
            float flow = (centerElev - minElev) * timeStep * 0.1f; // Reduced flow rate
            
            // Clamp flow to prevent extreme values
            flow = std::max(0.0f, std::min(2.0f, flow));
            waterFlow_->addToSample(flowX, flowZ, flow);
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

void GeologicalSimulator::updateProgress(float phaseProgress, const std::string& processName) {
    currentPhaseProgress_ = phaseProgress;
    
    // Calculate total progress across all phases
    float totalProgress = 0.0f;
    switch (currentPhase_) {
        case GeologicalPhase::TECTONICS:
            totalProgress = phaseProgress * 0.4f; // 40% of total time
            break;
        case GeologicalPhase::EROSION:
            totalProgress = 0.4f + (phaseProgress * 0.4f); // 40% + current erosion progress
            break;
        case GeologicalPhase::DETAIL:
            totalProgress = 0.8f + (phaseProgress * 0.2f); // 80% + current detail progress
            break;
    }
    
    // Calculate time remaining
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime_).count() / 1000.0f;
    float timeRemaining = (totalProgress > 0.01f) ? (elapsed / totalProgress) * (1.0f - totalProgress) : totalSimulationTime_;
    
    if (progressCallback_) {
        PhaseInfo info;
        info.currentPhase = currentPhase_;
        info.phaseProgress = phaseProgress;
        info.totalProgress = totalProgress;
        info.currentProcess = processName;
        info.timeRemaining = timeRemaining;
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

std::unique_ptr<ContinuousField<GeologicalSample>> GeologicalSimulator::exportGeologicalData() const {
    // This would create a field of GeologicalSample objects for chunk generation
    // For now, return nullptr - this will be implemented with the HybridDetailGenerator
    return nullptr;
}

} // namespace World
} // namespace VoxelCastle
