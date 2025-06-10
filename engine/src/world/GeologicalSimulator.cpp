#include "world/GeologicalSimulator.h"
#include "util/noise.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

namespace VoxelCastle {
namespace World {

GeologicalSimulator::GeologicalSimulator(int worldSize, const GeologicalConfig& config)
    : config_(config), worldSize_(worldSize), seed_(0), currentPhase_(GeologicalPhase::TECTONICS),
      currentPhaseProgress_(0.0f) {
    
    // Calculate world size in kilometers (assuming 32m chunks)
    worldSizeKm_ = worldSize * 0.032f; // 32m per chunk
    
    std::cout << "[GeologicalSimulator] Initialized for " << worldSizeKm_ << "km world with " 
              << (config.preset == GeologicalPreset::BALANCED ? "BALANCED" : "OTHER") << " quality" << std::endl;
}

void GeologicalSimulator::initialize(uint64_t seed) {
    seed_ = seed;
    rng_.seed(seed);
    
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
            break;
        case GeologicalPreset::BALANCED:
            totalSimulationTime_ = 300.0f; // 5 minutes
            break;
        case GeologicalPreset::QUALITY:
            totalSimulationTime_ = 720.0f; // 12 minutes
            break;
        case GeologicalPreset::ULTRA_REALISM:
            totalSimulationTime_ = 1800.0f; // 30 minutes
            break;
        case GeologicalPreset::CUSTOM:
            totalSimulationTime_ = config_.custom.simulationDetailLevel * 0.3f;
            break;
    }
    
    std::cout << "[GeologicalSimulator] Estimated simulation time: " << totalSimulationTime_ << " seconds" << std::endl;
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
    
    // Add some initial variation
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            float worldX = x * spacing;
            float worldZ = z * spacing;
            
            // Initial mantle convection pattern
            float mantleValue = VoxelEngine::Util::smoothValueNoise(worldX * 0.0001f, 0.0f, worldZ * 0.0001f) * 2.0f;
            mantleStress_->setSample(x, z, mantleValue);
            
            // Initial elevation variation
            float elevationValue = VoxelEngine::Util::smoothValueNoise(worldX * 0.0005f, 0.0f, worldZ * 0.0005f) * 100.0f;
            elevationField_->setSample(x, z, elevationValue);
            
            // Initial rock type distribution
            RockType initialRock = (random01() < 0.7f) ? RockType::IGNEOUS_GRANITE : RockType::IGNEOUS_BASALT;
            rockTypes_->setSample(x, z, initialRock);
        }
    }
    
    std::cout << "[GeologicalSimulator] Fields initialized with base variation" << std::endl;
}

void GeologicalSimulator::runFullSimulation(std::function<void(const PhaseInfo&)> progressCallback) {
    progressCallback_ = progressCallback;
    
    std::cout << "[GeologicalSimulator] Starting full geological simulation" << std::endl;
    
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
    std::cout << "[GeologicalSimulator] Full simulation completed" << std::endl;
}

void GeologicalSimulator::simulateTectonicPhase(float durationMillionYears) {
    std::cout << "[GeologicalSimulator] Tectonic Phase: Simulating " << durationMillionYears << " million years" << std::endl;
    
    int steps = config_.getSimulationSteps() / 3; // 1/3 of total steps for tectonic phase
    float timeStep = durationMillionYears / steps;
    
    for (int step = 0; step < steps; ++step) {
        // Update progress
        float progress = static_cast<float>(step) / steps;
        updateProgress(progress, "Mantle Convection & Plate Movement");
        
        // Simulate geological processes
        simulateMantleConvection(timeStep);
        simulatePlateMovement(timeStep);
        simulateMountainBuilding(timeStep);
        
        if (step % (steps / 4) == 0) {
            simulateVolcanicActivity(timeStep * 4); // Less frequent volcanic activity
        }
        
        updatePerformanceMetrics();
    }
}

void GeologicalSimulator::simulateErosionPhase(float durationThousandYears) {
    std::cout << "[GeologicalSimulator] Erosion Phase: Simulating " << durationThousandYears << " thousand years" << std::endl;
    
    int steps = config_.getSimulationSteps() / 3; // 1/3 of total steps for erosion phase
    float timeStep = durationThousandYears / steps;
    
    for (int step = 0; step < steps; ++step) {
        // Update progress
        float progress = static_cast<float>(step) / steps;
        updateProgress(progress, "Water Erosion & River Formation");
        
        // Simulate erosion processes
        simulateChemicalWeathering(timeStep);
        simulatePhysicalErosion(timeStep);
        simulateRiverSystems(timeStep);
        
        if (config_.custom.enableGlacialFeatures && step % (steps / 8) == 0) {
            simulateGlacialCarving(timeStep * 8); // Occasional glacial periods
        }
        
        updatePerformanceMetrics();
    }
}

void GeologicalSimulator::simulateDetailPhase(float durationHundredYears) {
    std::cout << "[GeologicalSimulator] Detail Phase: Simulating " << durationHundredYears << " hundred years" << std::endl;
    
    int steps = config_.getSimulationSteps() / 3; // 1/3 of total steps for detail phase
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
        
        if (config_.custom.enableCaveSystems && step % (steps / 4) == 0) {
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

// Implementation of simulation methods
void GeologicalSimulator::simulateMantleConvection(float timeStep) {
    auto resistance = [this](float x, float z) -> float {
        return 1.0f; // Uniform resistance for mantle convection
    };
    
    // Create convection cells
    int numCells = 8 + static_cast<int>(random01() * 4); // 8-12 convection cells
    for (int i = 0; i < numCells; ++i) {
        float x = randomRange(0, worldSizeKm_ * 1000.0f);
        float z = randomRange(0, worldSizeKm_ * 1000.0f);
        float strength = randomRange(0.5f, 2.0f) * timeStep;
        float range = randomRange(100000.0f, 500000.0f); // 100-500km range
        
        mantleStress_->propagateValue(strength, x, z, range, resistance);
    }
}

void GeologicalSimulator::simulatePlateMovement(float timeStep) {
    // Transfer mantle stress to crust stress
    for (int z = 0; z < mantleStress_->getHeight(); ++z) {
        for (int x = 0; x < mantleStress_->getWidth(); ++x) {
            float mantleValue = mantleStress_->getSample(x, z);
            float transferredStress = mantleValue * timeStep * 0.1f;
            crustStress_->addToSample(x, z, transferredStress);
        }
    }
}

void GeologicalSimulator::simulateMountainBuilding(float timeStep) {
    // Convert crust stress to elevation changes
    for (int z = 0; z < crustStress_->getHeight(); ++z) {
        for (int x = 0; x < crustStress_->getWidth(); ++x) {
            float stress = crustStress_->getSample(x, z);
            float rockHard = rockHardness_->getSample(x, z);
            
            if (stress > 0.5f) { // Compression threshold
                float uplift = stress * timeStep * 10.0f / rockHard; // Harder rock uplifts less
                elevationField_->addToSample(x, z, uplift);
                
                // Determine new rock type based on pressure and temperature
                float currentElevation = elevationField_->getSample(x, z);
                if (currentElevation > 2000.0f && stress > 3.0f) {
                    rockTypes_->setSample(x, z, RockType::METAMORPHIC_SLATE);
                    rockHardness_->setSample(x, z, 9.0f);
                }
            }
        }
    }
}

void GeologicalSimulator::simulateVolcanicActivity(float timeStep) {
    // Create volcanic hotspots
    int numVolcanoes = 2 + static_cast<int>(random01() * 3); // 2-5 volcanic events
    for (int i = 0; i < numVolcanoes; ++i) {
        float x = randomRange(0, worldSizeKm_ * 1000.0f);
        float z = randomRange(0, worldSizeKm_ * 1000.0f);
        
        // Convert to grid coordinates
        int gridX = static_cast<int>(x / elevationField_->getSampleSpacing());
        int gridZ = static_cast<int>(z / elevationField_->getSampleSpacing());
        
        // Create volcanic cone
        float volcanoHeight = randomRange(500.0f, 2000.0f);
        elevationField_->addToSample(gridX, gridZ, volcanoHeight);            rockTypes_->setSample(gridX, gridZ, RockType::IGNEOUS_BASALT);
        rockHardness_->setSample(gridX, gridZ, 7.0f);
        
        // Volcanic influence radius
        int radius = 5 + static_cast<int>(random01() * 10);
        for (int dz = -radius; dz <= radius; ++dz) {
            for (int dx = -radius; dx <= radius; ++dx) {
                float distance = std::sqrt(dx*dx + dz*dz);
                if (distance <= radius) {
                    float heightMod = volcanoHeight * (1.0f - distance / radius) * 0.3f;
                    elevationField_->addToSample(gridX + dx, gridZ + dz, heightMod);
                }
            }
        }
    }
}

void GeologicalSimulator::simulateChemicalWeathering(float timeStep) {
    if (!config_.custom.enableChemicalWeathering) return;
    
    for (int z = 0; z < elevationField_->getHeight(); ++z) {
        for (int x = 0; x < elevationField_->getWidth(); ++x) {
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
                float erosion = weatheringRate * timeStep * climateMultiplier;
                elevationField_->addToSample(x, z, -erosion);
            }
        }
    }
}

void GeologicalSimulator::simulatePhysicalErosion(float timeStep) {
    for (int z = 0; z < elevationField_->getHeight(); ++z) {
        for (int x = 0; x < elevationField_->getWidth(); ++x) {
            float elevation = elevationField_->getSample(x, z);
            float waterFlowValue = waterFlow_->getSample(x, z);
            
            // Physical erosion increases with water flow and slope
            if (waterFlowValue > 0.1f) {
                float erosion = waterFlowValue * timeStep * 0.1f;
                elevationField_->addToSample(x, z, -erosion);
                waterFlow_->addToSample(x, z, erosion * 0.1f); // Increase flow downstream
            }
        }
    }
}

void GeologicalSimulator::simulateRiverSystems(float timeStep) {
    // Simple river flow simulation - water flows from high to low elevation
    for (int z = 1; z < elevationField_->getHeight() - 1; ++z) {
        for (int x = 1; x < elevationField_->getWidth() - 1; ++x) {
            float centerElev = elevationField_->getSample(x, z);
            
            // Check all 8 neighbors
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
                float flow = (centerElev - minElev) * timeStep;
                waterFlow_->addToSample(flowX, flowZ, flow);
            }
        }
    }
}

void GeologicalSimulator::simulateGlacialCarving(float timeStep) {
    // Glacial erosion occurs at high elevations in cold conditions
    for (int z = 0; z < elevationField_->getHeight(); ++z) {
        for (int x = 0; x < elevationField_->getWidth(); ++x) {
            float elevation = elevationField_->getSample(x, z);
            float temperature = 15.0f - (elevation * 0.006f);
            
            if (elevation > 1500.0f && temperature < -5.0f) {
                // Glacial erosion creates U-shaped valleys
                float erosion = timeStep * 2.0f;
                elevationField_->addToSample(x, z, -erosion);
            }
        }
    }
}

void GeologicalSimulator::simulateMicroWeathering(float timeStep) {
    // Fine-scale weathering processes
    // This would be expanded in the HybridDetailGenerator
}

void GeologicalSimulator::simulateSedimentDeposition(float timeStep) {
    // Sediment accumulation in low-energy environments
    for (int z = 0; z < waterFlow_->getHeight(); ++z) {
        for (int x = 0; x < waterFlow_->getWidth(); ++x) {
            float flow = waterFlow_->getSample(x, z);
            float elevation = elevationField_->getSample(x, z);
            
            // Sediment deposits where water flow is low and elevation is low
            if (flow < 0.5f && elevation < 100.0f) {
                float deposition = (0.5f - flow) * timeStep * 0.1f;
                elevationField_->addToSample(x, z, deposition);                    rockTypes_->setSample(x, z, RockType::SEDIMENTARY_SHALE);
                rockHardness_->setSample(x, z, 2.0f);
            }
        }
    }
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
