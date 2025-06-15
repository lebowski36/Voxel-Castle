#include "world/ErosionEngine.h"
#include "world/geological_constants.h"
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
        
        // EARTH-LIKE CHEMICAL WEATHERING for 1M year timesteps  
        // Chemical weathering: 0.01-0.1mm/year = 10-100m per million years
        // Calculate weathering intensity based on climate and rock type
        float weatheringIntensity = calculateErosionRate(rockType, precipitation, 0.0f) * timeStepKyears * 10.0f; // 100x increase for million-year steps
        
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
            
            // EARTH-LIKE PHYSICAL EROSION RATES
            // For 1M year timesteps: Physical erosion 0.1-1mm/year = 100-1000m per million years
            // Most areas much less: 10-100m per million years typical
            
            if (slope > 0.001f && hardness > 0.01f) {
                // REALISTIC BASE RATE: 50m per million years (0.05mm/year average)
                float baseErosionRate = 50.0f; // Scaled for 1M year timesteps
                
                // Slope effect: gentle scaling, not exponential
                float slopeMultiplier = 1.0f + std::min(slope * 5.0f, 4.0f); // Max 5x for very steep slopes
                
                // Hardness resistance: prevent division by tiny numbers
                float hardnessResistance = std::max(hardness, 0.1f);
                
                // Calculate realistic erosion for 1M year timesteps
                float erosionRate = (baseErosionRate * slopeMultiplier) / hardnessResistance;
                float erosionIntensity = erosionRate * (timeStepKyears / 1000000.0f); // Correct scaling for million-year steps
                
                // Geological reality check: even in extreme conditions, erosion is limited
                erosionIntensity = std::min(erosionIntensity, 0.5f); // Maximum 0.5m per step regardless of timestep
                
                applyPhysicalErosion(fields, x, z, erosionIntensity, timeStepKyears);
                
                validateAndClampElevation(fields, x, z, "PhysicalErosion");
            }
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
                
                // EARTH-LIKE WATER EROSION for 1M year timesteps
                // River erosion: 1-10mm/year = 1000-10000m per million years in active channels
                // Most rivers: 100-1000m per million years typical
                
                // Base water erosion rate: scaled for million-year timesteps  
                float baseWaterErosionRate = 500.0f; // 500m per million years for active water flow
                
                // Scale by water power (velocity^2 * flow)
                float waterPower = velocity * velocity * std::min(waterFlow, 10.0f); // Limit flow effect
                float flowMultiplier = 1.0f + std::min(waterPower * 0.5f, 3.0f); // Max 4x for strong flow
                
                // Calculate realistic water erosion for 1M year timesteps
                float erosionRate = baseWaterErosionRate * flowMultiplier;
                float erosionPower = erosionRate * (timeStepKyears / 1000000.0f); // Correct million-year scaling
                
                // Geological limit: even the most powerful rivers are limited over million year periods
                erosionPower = std::min(erosionPower, 200.0f); // Max 200m per million-year step
                
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
                
                // CRITICAL: Clamp sediment deposition to prevent geological impossibilities
                // Maximum realistic deposition: ~20m per thousand years in extreme conditions
                depositionRate = std::min(depositionRate, 20.0f * timeStepKyears);
                
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
                // EARTH-LIKE GLACIAL EROSION for 1M year timesteps
                // Glacial erosion: 1-100mm/year = 1000-100000m per million years (much faster than other erosion)
                // But glaciers are rare and episodic - average over long periods much less
                
                float elevationAboveTreeline = elevation - 800.0f;
                
                // Base glacial erosion rate: 5000m per million years (realistic for active glaciation)
                float baseGlacialRate = 5000.0f;
                
                // Scale gently with elevation (not linearly - would be extreme)
                float elevationFactor = 1.0f + std::min(elevationAboveTreeline / 1000.0f, 2.0f); // Max 3x at 2800m+
                
                // Calculate realistic glacial intensity for 1M year timesteps
                float glacialRate = baseGlacialRate * elevationFactor;
                float glacialIntensity = glacialRate * (timeStepKyears / 1000000.0f); // Correct million-year scaling
                
                // Geological limit: even the most active glaciers have limits over million-year periods
                glacialIntensity = std::min(glacialIntensity, 1000.0f); // Max 1000m per million-year step
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
    
    // DEBUG: Sample a few cells to see what's happening
    static int debugCount = 0;
    bool shouldDebug = debugCount < 3;
    
    float minElev = std::numeric_limits<float>::max();
    float maxElev = std::numeric_limits<float>::lowest();
    float minErosionRate = std::numeric_limits<float>::max();
    float maxErosionRate = std::numeric_limits<float>::lowest();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float erosionRate = fields.erosionRateField->getSample(x, z);
            float elevation = fields.elevationField->getSample(x, z);
            
            minErosionRate = std::min(minErosionRate, erosionRate);
            maxErosionRate = std::max(maxErosionRate, erosionRate);
            
            // DISABLE erosion-uplift balance entirely to prevent it from overwhelming other processes
            // This process is supposed to balance erosion and uplift over geological time scales,
            // but it's currently causing catastrophic elevation drops
            // Instead, let individual tectonic and erosion processes handle their own balance
            
            // Apply minimal isostatic adjustment only
            float isostaticAdjustment = 0.0f;
            if (elevation < -1000.0f) {
                // Slight uplift for very deep areas (isostatic rebound)
                isostaticAdjustment = 0.1f * timeStepKyears * 0.001f; // 0.1mm/1000years
            } else if (elevation > 500.0f) {
                // Slight subsidence for very high areas (isostatic subsidence)
                isostaticAdjustment = -0.05f * timeStepKyears * 0.001f; // 0.05mm/1000years
            }
            
            float newElevation = elevation + isostaticAdjustment;
            
            // Only clamp extreme outliers, preserve the main elevation distribution
            if (newElevation < -3000.0f) newElevation = -3000.0f;
            if (newElevation > 3000.0f) newElevation = 3000.0f;
            
            fields.elevationField->setSample(x, z, newElevation);
            minElev = std::min(minElev, newElevation);
            maxElev = std::max(maxElev, newElevation);
        }
    }
    if (shouldDebug) {
        std::cout << "[EROSION_UPLIFT_DEBUG] TimeStep: " << timeStepKyears << " kyears" << std::endl;
        std::cout << "[EROSION_UPLIFT_DEBUG] Erosion rate range: " << minErosionRate << " to " << maxErosionRate << std::endl;
        std::cout << "[EROSION_UPLIFT_DEBUG] Elevation range after ErosionUpliftBalance: " << minElev << "m to " << maxElev << "m" << std::endl;
        debugCount++;
        if (debugCount == 3) {
            std::cout << "[EROSION_UPLIFT_DEBUG] Debug output complete - further messages suppressed" << std::endl;
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
    
    // Check for extreme values that indicate a bug - but limit spam  
    static int errorCount = 0;
    if (std::abs(elevation) > 10000.0f && errorCount < 5) {
        std::cout << "[ELEVATION_BUG] " << processName << " created extreme elevation: " 
                  << elevation << "m at (" << x << "," << z << ") - CLAMPING to bounds" << std::endl;
        errorCount++;
        if (errorCount == 5) {
            std::cout << "[ELEVATION_BUG] Further erosion elevation clamping messages suppressed to reduce spam" << std::endl;
        }
    }
    
    // FIXED: Apply realistic geological elevation bounds using global constants
    // Expected terrain range: -1800m to +1800m with ±2048m as absolute physical limits
    float clampedElevation = CLAMP_GEOLOGICAL_ELEVATION(elevation);
    
    if (elevation != clampedElevation) {
        fields.elevationField->setSample(x, z, clampedElevation);
        WARN_EXTREME_ELEVATION(elevation, "ErosionClamping", x, z);
    }
}

// Private helper methods
void ErosionEngine::applyChemicalWeathering(ErosionFields& fields, int x, int z, float intensity, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    float erosion = intensity * 0.5f; // Scale erosion amount
    float newElevation = currentElevation - erosion;
    
    // CRITICAL: Clamp immediately to prevent geological impossibilities
    newElevation = CLAMP_GEOLOGICAL_ELEVATION(newElevation);
    fields.elevationField->setSample(x, z, newElevation);
    
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
        
        // CRITICAL FIX: Clamp excessive deposition to prevent geological impossibilities
        // Realistic deposition rate: ~1-5m per thousand years maximum
        float maxDeposition = 5.0f * (timeStep / 1000.0f);
        float deposition = std::min(excess * 0.1f, maxDeposition);
        
        // Add to elevation with proper bounds checking
        float currentElevation = fields.elevationField->getSample(x, z);
        float newElevation = currentElevation + deposition;
        newElevation = CLAMP_GEOLOGICAL_ELEVATION(newElevation);
        fields.elevationField->setSample(x, z, newElevation);
        
        validateAndClampElevation(fields, x, z, "SedimentTransport");
    }
}

void ErosionEngine::depositSediment(ErosionFields& fields, int x, int z, float amount, float timeStep) {
    // Add sediment to elevation
    float currentElevation = fields.elevationField->getSample(x, z);
    float newElevation = currentElevation + amount;
    
    // CRITICAL: Clamp immediately to prevent geological impossibilities
    newElevation = CLAMP_GEOLOGICAL_ELEVATION(newElevation);
    fields.elevationField->setSample(x, z, newElevation);
    
    // Reduce sediment load
    if (fields.sedimentLoad) {
        float currentSediment = fields.sedimentLoad->getSample(x, z);
        fields.sedimentLoad->setSample(x, z, std::max(0.0f, currentSediment - amount));
    }
}

void ErosionEngine::carveRiverChannel(ErosionFields& fields, int x, int z, float flow, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    
    // REALISTIC RIVER CARVING RATES
    // Active rivers: 0.1-10 m/kyear carving rate
    // Large rivers in soft rock: up to 20 m/kyear in extreme cases
    
    float baseRiverCarvingRate = 3.0f; // 3 m/kyear for moderate flow
    float flowEffect = std::min(flow * 0.5f, 5.0f); // Flow increases carving, but limited
    
    float carvingRate = baseRiverCarvingRate * (1.0f + flowEffect);
    float carving = carvingRate * (timeStep / 1000.0f); // Scale for timestep
    
    // Geological limit: even the most powerful rivers are constrained
    carving = std::min(carving, 0.3f); // Max 0.3m per step
    
    fields.elevationField->setSample(x, z, currentElevation - carving);
    
    // Increase sediment load from carving
    if (fields.sedimentLoad) {
        float currentSediment = fields.sedimentLoad->getSample(x, z);
        fields.sedimentLoad->setSample(x, z, currentSediment + carving * 0.8f);
    }
}

void ErosionEngine::applyGlacialErosion(ErosionFields& fields, int x, int z, float intensity, float timeStep) {
    float currentElevation = fields.elevationField->getSample(x, z);
    
    // REALISTIC GLACIAL EROSION APPLICATION
    // The intensity has already been calculated with realistic rates
    // Just apply it directly without additional scaling
    float erosion = intensity; // Use calculated realistic intensity
    
    // Final safety check: geological maximum
    erosion = std::min(erosion, 1.0f); // Absolute max 1m per step
    
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
