#include "world/ErosionEngine.h"
#include "util/noise.h"
#include "world/geological_data.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <execution>
#include <vector>
#include <numeric>

using namespace VoxelCastle::World;

namespace VoxelCastle {
namespace World {

ErosionEngine::ErosionEngine(float worldSizeKm, const GeologicalConfig& config, uint64_t seed)
    : worldSizeKm_(worldSizeKm), config_(config), seed_(seed), rng_(seed) {
    std::cout << "[ErosionEngine] Initialized for " << worldSizeKm_ << "km world" << std::endl;
}

void ErosionEngine::simulateChemicalWeathering(ErosionFields& fields, float timeStepKyears) {
    if (!fields.elevationField || !fields.precipitationField || !fields.rockTypes) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    // Process all points in parallel
    std::vector<int> indices(height * width);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / width;
        int x = idx % width;
        
        float precipitation = fields.precipitationField->getSample(x, z);
        RockType rockType = fields.rockTypes->getSample(x, z);
        float elevation = fields.elevationField->getSample(x, z);
        
        // Calculate weathering intensity based on climate and rock type
        float weatheringIntensity = calculateErosionRate(rockType, precipitation, 0.0f) * timeStepKyears * 0.1f;
        
        // Apply chemical weathering - reduces elevation slightly
        applyChemicalWeathering(fields, x, z, weatheringIntensity, timeStepKyears);
        
        validateAndClampElevation(fields, x, z, "ChemicalWeathering");
    });
    
    updateMetrics(fields);
}

void ErosionEngine::simulatePhysicalErosion(ErosionFields& fields, float timeStepKyears) {
    if (!fields.elevationField || !fields.rockHardness) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float slope = calculateSlope(*fields.elevationField, x, z);
            float hardness = fields.rockHardness->getSample(x, z);
            
            // Physical erosion is stronger on steep slopes and softer rocks
            float erosionIntensity = (slope * 0.1f) / (hardness * 0.1f) * timeStepKyears;
            
            applyPhysicalErosion(fields, x, z, erosionIntensity, timeStepKyears);
            
            validateAndClampElevation(fields, x, z, "PhysicalErosion");
        }
    }
}

void ErosionEngine::simulateWaterDrivenErosion(ErosionFields& fields, float timeStepKyears) {
    if (!fields.elevationField || !fields.waterFlow || !fields.surfaceWaterDepth) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float waterFlow = fields.waterFlow->getSample(x, z);
            float waterDepth = fields.surfaceWaterDepth->getSample(x, z);
            
            if (waterFlow > 0.01f || waterDepth > 0.01f) {
                float slope = calculateSlope(*fields.elevationField, x, z);
                float velocity = calculateWaterFlowVelocity(waterDepth, slope);
                
                // Water erosion power is proportional to velocity squared
                float erosionPower = velocity * velocity * waterFlow * timeStepKyears * 0.01f;
                
                applyWaterErosion(fields, x, z, erosionPower, timeStepKyears);
                
                validateAndClampElevation(fields, x, z, "WaterDrivenErosion");
            }
        }
    }
}

void ErosionEngine::simulateSedimentTransport(ErosionFields& fields, float timeStepKyears) {
    if (!fields.sedimentLoad || !fields.waterFlow || !fields.elevationField) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float waterFlow = fields.waterFlow->getSample(x, z);
            float slope = calculateSlope(*fields.elevationField, x, z);
            
            if (waterFlow > 0.01f) {
                float sedimentCapacity = calculateSedimentCapacity(waterFlow, slope);
                transportSediment(fields, x, z, sedimentCapacity, timeStepKyears);
            }
        }
    }
}

void ErosionEngine::simulateSedimentDeposition(ErosionFields& fields, float timeStepKyears) {
    if (!fields.sedimentLoad || !fields.elevationField) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float sediment = fields.sedimentLoad->getSample(x, z);
            
            if (sediment > 0.01f) {
                // Deposit sediment where water flow is low
                float waterFlow = fields.waterFlow ? fields.waterFlow->getSample(x, z) : 0.0f;
                float depositionRate = sediment * (1.0f - std::min(1.0f, waterFlow)) * timeStepKyears * 0.1f;
                
                depositSediment(fields, x, z, depositionRate, timeStepKyears);
                
                validateAndClampElevation(fields, x, z, "SedimentDeposition");
            }
        }
    }
}

void ErosionEngine::simulateRiverSystems(ErosionFields& fields, float timeStepKyears) {
    if (!fields.elevationField || !fields.waterFlow) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    int activeRivers = 0;
    
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float flow = fields.waterFlow->getSample(x, z);
            
            if (flow > 0.5f) { // Significant river flow
                carveRiverChannel(fields, x, z, flow, timeStepKyears);
                activeRivers++;
                
                validateAndClampElevation(fields, x, z, "RiverSystems");
            }
        }
    }
    
    metrics_.activeRivers = activeRivers;
}

void ErosionEngine::simulateGlacialCarving(ErosionFields& fields, float timeStepKyears) {
    if (!fields.elevationField) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    // Glacial erosion occurs at high elevations
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float elevation = fields.elevationField->getSample(x, z);
            
            if (elevation > 800.0f) { // Above treeline
                float glacialIntensity = (elevation - 800.0f) * 0.001f * timeStepKyears;
                applyGlacialErosion(fields, x, z, glacialIntensity, timeStepKyears);
                
                validateAndClampElevation(fields, x, z, "GlacialCarving");
            }
        }
    }
}

void ErosionEngine::simulateMicroWeathering(ErosionFields& fields, float timeStepYears) {
    if (!fields.elevationField || !fields.rockTypes) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    // Very small-scale weathering processes
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            RockType rockType = fields.rockTypes->getSample(x, z);
            float resistance = getWeatheringResistance(rockType);
            
            float microErosion = (1.0f / resistance) * timeStepYears * 0.001f;
            
            float currentElevation = fields.elevationField->getSample(x, z);
            fields.elevationField->setSample(x, z, currentElevation - microErosion);
            
            validateAndClampElevation(fields, x, z, "MicroWeathering");
        }
    }
}

void ErosionEngine::simulateErosionUpliftBalance(ErosionFields& fields, float timeStepKyears) {
    if (!fields.elevationField || !fields.erosionRateField) return;

    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float erosionRate = fields.erosionRateField->getSample(x, z);
            float elevation = fields.elevationField->getSample(x, z);
            
            // Balance erosion with tectonic uplift
            float upliftRate = 0.01f; // Background uplift rate
            float netChange = (upliftRate - erosionRate) * timeStepKyears;
            
            fields.elevationField->setSample(x, z, elevation + netChange);
            
            validateAndClampElevation(fields, x, z, "ErosionUpliftBalance");
        }
    }
}

void ErosionEngine::updateMetrics(const ErosionFields& fields) {
    if (!fields.elevationField || !fields.erosionRateField) return;
    
    float totalErosion = 0.0f;
    float totalDeposition = 0.0f;
    float totalErosionRate = 0.0f;
    float totalSediment = 0.0f;
    int count = 0;
    
    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float erosionRate = fields.erosionRateField->getSample(x, z);
            totalErosionRate += erosionRate;
            
            if (fields.sedimentLoad) {
                totalSediment += fields.sedimentLoad->getSample(x, z);
            }
            
            count++;
        }
    }
    
    metrics_.averageErosionRate = count > 0 ? totalErosionRate / count : 0.0f;
    metrics_.sedimentTransported = totalSediment;
    metrics_.totalErosion = totalErosion;
    metrics_.totalDeposition = totalDeposition;
}

float ErosionEngine::calculateErosionRate(RockType rockType, float precipitation, float slope) const {
    float baseRate = 0.1f;
    float rockResistance = getErosionResistance(rockType);
    float climateEffect = precipitation / 1000.0f; // Normalize precipitation
    float slopeEffect = 1.0f + slope * 2.0f;
    
    return baseRate * climateEffect * slopeEffect / rockResistance;
}

float ErosionEngine::calculateSedimentCapacity(float waterFlow, float slope) const {
    return waterFlow * (1.0f + slope * 5.0f) * 0.1f;
}

void ErosionEngine::validateAndClampElevation(ErosionFields& fields, int x, int z, const std::string& processName) {
    if (!fields.elevationField) return;
    
    float elevation = fields.elevationField->getSample(x, z);
    
    // Check for extreme values that indicate a bug
    if (std::abs(elevation) > 10000.0f) {
        std::cout << "[ELEVATION_BUG] " << processName << " created extreme elevation: " 
                  << elevation << "m at (" << x << "," << z << ") - CLAMPING to bounds" << std::endl;
    }
    
    // Clamp to realistic geological bounds
    float clampedElevation = std::max(-1800.0f, std::min(1200.0f, elevation));
    
    if (elevation != clampedElevation) {
        fields.elevationField->setSample(x, z, clampedElevation);
    }
}

// Private helper methods
void ErosionEngine::applyChemicalWeathering(ErosionFields& fields, int x, int z, float intensity, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    float erosion = intensity * 0.5f; // Scale erosion amount
    fields.elevationField->setSample(x, z, currentElevation - erosion);
    
    // Add to sediment load if available
    if (fields.sedimentLoad) {
        float currentSediment = fields.sedimentLoad->getSample(x, z);
        fields.sedimentLoad->setSample(x, z, currentSediment + erosion * 0.3f);
    }
}

void ErosionEngine::applyPhysicalErosion(ErosionFields& fields, int x, int z, float intensity, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    float erosion = intensity * 0.8f; // Scale erosion amount
    fields.elevationField->setSample(x, z, currentElevation - erosion);
    
    // Add to sediment load if available
    if (fields.sedimentLoad) {
        float currentSediment = fields.sedimentLoad->getSample(x, z);
        fields.sedimentLoad->setSample(x, z, currentSediment + erosion * 0.5f);
    }
}

void ErosionEngine::applyWaterErosion(ErosionFields& fields, int x, int z, float power, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    float erosion = power * 0.2f; // Scale erosion amount
    fields.elevationField->setSample(x, z, currentElevation - erosion);
    
    // Add to sediment load
    if (fields.sedimentLoad) {
        float currentSediment = fields.sedimentLoad->getSample(x, z);
        fields.sedimentLoad->setSample(x, z, currentSediment + erosion * 0.7f);
    }
}

void ErosionEngine::transportSediment(ErosionFields& fields, int x, int z, float capacity, float timeStep) {
    if (!fields.sedimentLoad) return;
    
    float currentSediment = fields.sedimentLoad->getSample(x, z);
    
    if (currentSediment > capacity) {
        // Deposit excess sediment
        float excess = currentSediment - capacity;
        fields.sedimentLoad->setSample(x, z, capacity);
        
        // Add to elevation
        float currentElevation = fields.elevationField->getSample(x, z);
        fields.elevationField->setSample(x, z, currentElevation + excess * 0.1f);
    }
}

void ErosionEngine::depositSediment(ErosionFields& fields, int x, int z, float amount, float timeStep) {
    // Add sediment to elevation
    float currentElevation = fields.elevationField->getSample(x, z);
    fields.elevationField->setSample(x, z, currentElevation + amount);
    
    // Reduce sediment load
    if (fields.sedimentLoad) {
        float currentSediment = fields.sedimentLoad->getSample(x, z);
        fields.sedimentLoad->setSample(x, z, std::max(0.0f, currentSediment - amount));
    }
}

void ErosionEngine::carveRiverChannel(ErosionFields& fields, int x, int z, float flow, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    float carving = flow * 0.05f * timeStep; // Scale carving amount
    fields.elevationField->setSample(x, z, currentElevation - carving);
    
    // Increase sediment load from carving
    if (fields.sedimentLoad) {
        float currentSediment = fields.sedimentLoad->getSample(x, z);
        fields.sedimentLoad->setSample(x, z, currentSediment + carving * 0.8f);
    }
}

void ErosionEngine::applyGlacialErosion(ErosionFields& fields, int x, int z, float intensity, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    float erosion = intensity * 2.0f; // Glacial erosion is powerful
    fields.elevationField->setSample(x, z, currentElevation - erosion);
}

float ErosionEngine::calculateSlope(const ContinuousField<float>& elevationField, int x, int z) const {
    int width = elevationField.getWidth();
    int height = elevationField.getHeight();
    
    if (x <= 0 || x >= width - 1 || z <= 0 || z >= height - 1) return 0.0f;
    
    float dx = elevationField.getSample(x + 1, z) - elevationField.getSample(x - 1, z);
    float dz = elevationField.getSample(x, z + 1) - elevationField.getSample(x, z - 1);
    
    return std::sqrt(dx * dx + dz * dz) / (2.0f * elevationField.getSampleSpacing());
}

float ErosionEngine::calculateWaterFlowVelocity(float waterDepth, float slope) const {
    // Simplified Manning equation for water flow velocity
    float roughness = 0.03f; // Typical for natural channels
    return (1.0f / roughness) * std::pow(waterDepth, 2.0f / 3.0f) * std::sqrt(slope);
}

float ErosionEngine::getWeatheringResistance(RockType rockType) const {
    switch (rockType) {
        case RockType::IGNEOUS_GRANITE: return 8.0f;
        case RockType::IGNEOUS_BASALT: return 7.0f;
        case RockType::SEDIMENTARY_SANDSTONE: return 3.0f;
        case RockType::SEDIMENTARY_LIMESTONE: return 2.0f;
        case RockType::METAMORPHIC_MARBLE: return 9.0f;
        case RockType::METAMORPHIC_SLATE: return 6.0f;
        default: return 5.0f;
    }
}

float ErosionEngine::getErosionResistance(RockType rockType) const {
    switch (rockType) {
        case RockType::IGNEOUS_GRANITE: return 8.0f;
        case RockType::IGNEOUS_BASALT: return 7.0f;
        case RockType::SEDIMENTARY_SANDSTONE: return 4.0f;
        case RockType::SEDIMENTARY_LIMESTONE: return 3.0f;
        case RockType::METAMORPHIC_MARBLE: return 8.5f;
        case RockType::METAMORPHIC_SLATE: return 6.0f;
        default: return 5.0f;
    }
}

}}
