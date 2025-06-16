#include "world/WaterSystemSimulator.h"
#include "util/noise.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <execution>
#include <vector>
#include <numeric>

namespace VoxelCastle {
namespace World {

WaterSystemSimulator::WaterSystemSimulator(float worldSizeKm, const GeologicalConfig& config)
    : worldSizeKm_(worldSizeKm), config_(config), seed_(0) {
    
    std::cout << "[WaterSystemSimulator] Initialized for " << worldSizeKm_ 
              << "km world" << std::endl;
}

void WaterSystemSimulator::Initialize(
    std::shared_ptr<ContinuousField<float>> elevationField,
    std::shared_ptr<ContinuousField<RockType>> rockTypes,
    uint64_t seed) {
    
    elevationField_ = elevationField;
    rockTypes_ = rockTypes;
    seed_ = seed;
    rng_.seed(seed);
    
    if (!elevationField_) {
        throw std::runtime_error("WaterSystemSimulator: elevationField is required");
    }
    
    int resolution = elevationField_->getWidth();
    
    // Initialize water system fields
    precipitationField_ = std::make_unique<ContinuousField<float>>(resolution, resolution, worldSizeKm_);
    groundwaterTable_ = std::make_unique<ContinuousField<float>>(resolution, resolution, worldSizeKm_);
    permeabilityField_ = std::make_unique<ContinuousField<float>>(resolution, resolution, worldSizeKm_);
    surfaceWaterDepth_ = std::make_unique<ContinuousField<float>>(resolution, resolution, worldSizeKm_);
    sedimentLoad_ = std::make_unique<ContinuousField<float>>(resolution, resolution, worldSizeKm_);
    springFlow_ = std::make_unique<ContinuousField<float>>(resolution, resolution, worldSizeKm_);
    waterFlow_ = std::make_unique<ContinuousField<float>>(resolution, resolution, worldSizeKm_);
    
    // Initialize with default values
    precipitationField_->fill(500.0f);      // 500mm annual precipitation
    groundwaterTable_->fill(10.0f);         // 10m depth to groundwater
    permeabilityField_->fill(0.5f);         // Medium permeability
    surfaceWaterDepth_->fill(0.0f);         // No surface water initially
    sedimentLoad_->fill(0.0f);              // No sediment initially
    springFlow_->fill(0.0f);                // No springs initially
    waterFlow_->fill(0.0f);                 // No flow initially
    
    std::cout << "[WaterSystemSimulator] Initialized with seed: " << seed_ << std::endl;
}

void WaterSystemSimulator::SimulatePrecipitationPatterns(float timeStep) {
    if (!precipitationField_ || !elevationField_) return;
    
    int height = precipitationField_->getHeight();
    int width = precipitationField_->getWidth();
    
    // Create index range for parallel processing
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int y = idx / width;
        int x = idx % width;
        
        // Convert grid coordinates to world coordinates
        float worldX = (static_cast<float>(x) / width) * worldSizeKm_;
        float worldZ = (static_cast<float>(y) / height) * worldSizeKm_;
        
        float elevation = elevationField_->sampleAt(worldX, worldZ);
        float newPrecipitation = CalculatePrecipitation(worldX, worldZ, elevation);
        
        precipitationField_->setSample(x, y, newPrecipitation);
    });
}

void WaterSystemSimulator::SimulateSurfaceWaterAccumulation(float timeStep) {
    if (!surfaceWaterDepth_ || !precipitationField_ || !elevationField_) return;
    
    int height = surfaceWaterDepth_->getHeight();
    int width = surfaceWaterDepth_->getWidth();
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float worldX = (static_cast<float>(x) / width) * worldSizeKm_;
            float worldZ = (static_cast<float>(y) / height) * worldSizeKm_;
            
            float precipitation = precipitationField_->sampleAt(worldX, worldZ);
            float elevation = elevationField_->sampleAt(worldX, worldZ);
            
            // Water accumulates more in low-elevation areas
            float accumulationFactor = std::max(0.1f, 1.0f - (elevation / 1000.0f));
            float waterInput = (precipitation / 1000.0f) * timeStep * accumulationFactor;
            
            float currentWater = surfaceWaterDepth_->getSample(x, y);
            float newWater = currentWater + waterInput;
            
            // Evaporation and drainage
            float evaporation = currentWater * 0.01f * timeStep;
            float drainage = currentWater * 0.05f * timeStep;
            newWater = std::max(0.0f, newWater - evaporation - drainage);
            
            surfaceWaterDepth_->setSample(x, y, newWater);
        }
    }
}

void WaterSystemSimulator::SimulateRiverFormation(float timeStep) {
    if (!waterFlow_ || !elevationField_ || !surfaceWaterDepth_) return;
    
    // Implement simple flow accumulation for river formation
    // This is a simplified version - full implementation would use proper flow accumulation algorithms
    
    int height = waterFlow_->getHeight();
    int width = waterFlow_->getWidth();
    
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            float worldX = (static_cast<float>(x) / width) * worldSizeKm_;
            float worldZ = (static_cast<float>(y) / height) * worldSizeKm_;
            
            float elevation = elevationField_->sampleAt(worldX, worldZ);
            float surfaceWater = surfaceWaterDepth_->getSample(x, y);
            
            // Calculate slope and flow direction
            float slope = 0.0f;
            float maxSlope = 0.0f;
            
            // Check 8 neighbors for steepest descent
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = x + dx;
                    int ny = y + dy;
                    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        float neighborWorldX = (static_cast<float>(nx) / width) * worldSizeKm_;
                        float neighborWorldZ = (static_cast<float>(ny) / height) * worldSizeKm_;
                        float neighborElevation = elevationField_->sampleAt(neighborWorldX, neighborWorldZ);
                        
                        float currentSlope = elevation - neighborElevation;
                        if (currentSlope > maxSlope) {
                            maxSlope = currentSlope;
                        }
                    }
                }
            }
            
            // Calculate flow based on slope and surface water
            float flow = maxSlope * surfaceWater * timeStep * 0.1f;
            waterFlow_->setSample(x, y, flow);
        }
    }
}

void WaterSystemSimulator::SimulateRiverSystems(float timeStep) {
    // Update existing river channels and flow
    UpdateWaterFlow(timeStep);
    ProcessSedimentTransport(timeStep);
}

void WaterSystemSimulator::SimulateGroundwaterTable(float timeStep) {
    if (!groundwaterTable_ || !precipitationField_ || !permeabilityField_) return;
    
    int height = groundwaterTable_->getHeight();
    int width = groundwaterTable_->getWidth();
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float worldX = (static_cast<float>(x) / width) * worldSizeKm_;
            float worldZ = (static_cast<float>(y) / height) * worldSizeKm_;
            
            float precipitation = precipitationField_->sampleAt(worldX, worldZ);        float permeability = permeabilityField_->getSample(x, y);
        float currentDepth = groundwaterTable_->getSample(x, y);
            
            // Recharge from precipitation
            float recharge = (precipitation / 1000.0f) * permeability * timeStep * 0.1f;
            float newDepth = std::max(1.0f, currentDepth - recharge);
            
            groundwaterTable_->setSample(x, y, newDepth);
        }
    }
}

void WaterSystemSimulator::SimulateAquiferRecharge(float timeStep) {
    // Additional aquifer recharge logic
    // For now, this is handled in SimulateGroundwaterTable
}

void WaterSystemSimulator::SimulateGroundwaterFlow(float timeStep) {
    // Implement groundwater flow based on hydraulic gradients
    // This would be a complex 3D flow simulation - simplified for now
}

void WaterSystemSimulator::SimulateSpringFormation(float timeStep) {
    if (!springFlow_ || !groundwaterTable_ || !elevationField_) return;
    
    int height = springFlow_->getHeight();
    int width = springFlow_->getWidth();
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float worldX = (static_cast<float>(x) / width) * worldSizeKm_;
            float worldZ = (static_cast<float>(y) / height) * worldSizeKm_;
            
            float groundwaterDepth = groundwaterTable_->getSample(x, y);
            float elevation = elevationField_->sampleAt(worldX, worldZ);
            
            // Springs form where groundwater is close to surface and elevation is moderate
            if (groundwaterDepth < 5.0f && elevation > 100.0f && elevation < 800.0f) {
                float springStrength = (5.0f - groundwaterDepth) / 5.0f;
                springFlow_->setSample(x, y, springStrength * 0.1f);
            } else {
                springFlow_->setSample(x, y, 0.0f);
            }
        }
    }
}

void WaterSystemSimulator::SimulateFloodPlains(float timeStep) {
    // Implement flood plain formation based on river overflow
    // Simplified implementation for now
}

void WaterSystemSimulator::SimulateLakeFormation(float timeStep) {
    // Implement lake formation in natural basins
    // This would analyze topography to find closed basins
}

void WaterSystemSimulator::SimulateWetlandFormation(float timeStep) {
    // Implement wetland formation where groundwater is near surface
    // Already partially handled in spring formation
}

void WaterSystemSimulator::SimulateCaveWaterInteractions(float timeStep) {
    // Implement cave-water system interactions
    // This is a basic implementation that will be expanded when cave networks are fully implemented
    
    if (!elevationField_ || !groundwaterTable_) {
        // Don't spam the logs - only show this warning once per 10 calls
        static int warningCount = 0;
        if (warningCount++ < 3) {
            std::cout << "[WaterSystemSimulator] SimulateCaveWaterInteractions: Missing required fields (water system not yet initialized)" << std::endl;
        }
        return;
    }
    
    int width = elevationField_->getWidth();
    int height = elevationField_->getHeight();
    int caveInteractions = 0;
    
    // Process cave-water interactions where limestone is present and groundwater is shallow
    for (int z = 0; z < height; z += 4) { // Sample every 4th point for performance
        for (int x = 0; x < width; x += 4) {
            float groundwaterDepth = groundwaterTable_->getSample(x, z);
            float elevation = elevationField_->getSample(x, z);
            
            // Cave formation likely where:
            // 1. Groundwater is shallow (< 50m below surface)
            // 2. There's sufficient water flow for dissolution
            if (groundwaterDepth < 50.0f && rockTypes_) {
                RockType rockType = rockTypes_->getSample(x, z);
                
                // Enhanced cave formation in limestone and similar rocks
                if (rockType == RockType::SEDIMENTARY_LIMESTONE || 
                    rockType == RockType::SEDIMENTARY_SANDSTONE) {
                    
                    // Simulate basic cave water flow effects
                    float caveFlowRate = std::min(1.0f, (50.0f - groundwaterDepth) / 50.0f);
                    caveFlowRate *= timeStep * 0.1f; // Scale by timestep
                    
                    // Slightly increase groundwater flow in cave areas
                    if (waterFlow_) {
                        float currentFlow = waterFlow_->getSample(x, z);
                        waterFlow_->setSample(x, z, currentFlow + caveFlowRate);
                    }
                    
                    caveInteractions++;
                }
            }
        }
    }
    
    if (caveInteractions > 0) {
        std::cout << "[WaterSystemSimulator] Processed " << caveInteractions 
                  << " cave-water interactions this timestep" << std::endl;
    }
}

// Private helper methods

float WaterSystemSimulator::CalculatePrecipitation(float x, float z, float elevation) const {
    // Base precipitation with elevation effects
    float basePrecipitation = 500.0f; // 500mm annual
    
    // Orographic precipitation - more rain on windward slopes
    float elevationEffect = std::min(2.0f, 1.0f + elevation / 1000.0f);
    
    // Add some noise for variation
    float noise = VoxelEngine::Util::smoothValueNoise(x * 0.001f, 1000, z * 0.001f) * 200.0f;
    
    return basePrecipitation * elevationEffect + noise;
}

float WaterSystemSimulator::CalculatePermeability(RockType rockType) const {
    switch (rockType) {
        case RockType::SEDIMENTARY_SANDSTONE:
            return 0.8f; // High permeability
        case RockType::SEDIMENTARY_LIMESTONE:
            return 0.9f; // Very high (due to caves)
        case RockType::SEDIMENTARY_SHALE:
            return 0.2f; // Low permeability
        case RockType::IGNEOUS_GRANITE:
            return 0.3f; // Low-medium
        case RockType::IGNEOUS_BASALT:
            return 0.4f; // Medium
        case RockType::METAMORPHIC_QUARTZITE:
            return 0.3f; // Low-medium
        case RockType::METAMORPHIC_SLATE:
            return 0.2f; // Low
        default:
            return 0.5f; // Default medium
    }
}

void WaterSystemSimulator::UpdateWaterFlow(float timeStep) {
    // Update water flow based on elevation gradients
    // This is a simplified implementation
}

void WaterSystemSimulator::ProcessSedimentTransport(float timeStep) {
    // Process sediment pickup and deposition
    // This would be a complex erosion-deposition model
}

float WaterSystemSimulator::Random01() const {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng_);
}

float WaterSystemSimulator::RandomRange(float min, float max) const {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng_);
}

} // namespace World
} // namespace VoxelCastle
