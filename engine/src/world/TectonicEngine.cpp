#include "world/TectonicEngine.h"
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

TectonicEngine::TectonicEngine(float worldSizeKm, const GeologicalConfig& config, uint64_t seed)
    : worldSizeKm_(worldSizeKm), config_(config), seed_(seed), rng_(seed) {
    std::cout << "[TectonicEngine] Initialized for " << worldSizeKm_ << "km world" << std::endl;
}

void TectonicEngine::simulateMantleConvection(TectonicFields& fields, float timeStepMyears) {
    if (!fields.mantleStress || !fields.elevationField) return;

    // Realistic geological scaling - mantle convection operates on much longer timescales
    float mantleTimeScale = std::min(1.0f, timeStepMyears / 10000.0f); // Normalize to 10,000 year cycles
    
    // Process all points in parallel for efficiency
    std::vector<int> indices(fields.mantleStress->getHeight() * fields.mantleStress->getWidth());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](int idx) {
        int z = idx / fields.mantleStress->getWidth();
        int x = idx % fields.mantleStress->getWidth();
        
        float worldX = x * fields.mantleStress->getSampleSpacing();
        float worldZ = z * fields.mantleStress->getSampleSpacing();
        
        // Generate organic mantle convection patterns using fractal noise
        float convectionIntensity = generateFractalNoise(worldX, worldZ, 0.0002f, 0.0008f, 0.003f);
        
        // Apply domain warping for more organic patterns
        float warpedConvection = generateDomainWarpedNoise(worldX, worldZ, 0.0001f, 2000.0f);
        
        // Combine patterns for realistic mantle flow with proper geological scaling
        float finalIntensity = (convectionIntensity * 0.7f + warpedConvection * 0.3f) * mantleTimeScale * 0.01f;
        
        applyMantleConvectionCell(fields, x, z, finalIntensity, timeStepMyears);
        
        // Validate elevation after modification
        validateAndClampElevation(fields, x, z, "MantleConvection");
    });
    
    updateMetrics(fields);
}

void TectonicEngine::simulatePlateMovement(TectonicFields& fields, float timeStepMyears) {
    if (!fields.mantleStress || !fields.elevationField) return;

    // Simulate plate boundaries and movement based on mantle stress
    int width = fields.mantleStress->getWidth();
    int height = fields.mantleStress->getHeight();
    
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            float stress = fields.mantleStress->getSample(x, z);
            
            // Calculate plate motion direction based on stress gradients
            float stressGradX = fields.mantleStress->getSample(x + 1, z) - fields.mantleStress->getSample(x - 1, z);
            float stressGradZ = fields.mantleStress->getSample(x, z + 1) - fields.mantleStress->getSample(x, z - 1);
            
            // Apply plate motion
            float motionScale = timeStepMyears * 0.05f; // Scale factor for plate movement
            applyPlateMotion(fields, x, z, stressGradX * motionScale, stressGradZ * motionScale, timeStepMyears);
            
            validateAndClampElevation(fields, x, z, "PlateMovement");
        }
    }
}

void TectonicEngine::simulateMountainBuilding(TectonicFields& fields, float timeStepMyears) {
    if (!fields.crustStress || !fields.elevationField) return;

    int width = fields.crustStress->getWidth();
    int height = fields.crustStress->getHeight();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float crustStress = fields.crustStress->getSample(x, z);
            
            // Apply stress dissipation - stress naturally decays over time
            float stressDissipation = crustStress * 0.001f * timeStepMyears; // 0.1% per thousand years
            crustStress = std::max(0.0f, crustStress - stressDissipation);
            
            // Apply realistic physical limits to prevent runaway values
            float maxRealisticStress = 100.0f; // Maximum realistic crustal stress (MPa equivalent)
            crustStress = std::min(crustStress, maxRealisticStress);
            
            // Update the stress field with dissipated/limited values
            fields.crustStress->setSample(x, z, crustStress);
            
            // DEBUG: Log extreme stress values
            if (crustStress > 25.0f) {
                std::cout << "[STRESS_DEBUG] High crustStress: " << crustStress 
                          << " at (" << x << "," << z << ") after dissipation" << std::endl;
            }
            
            // Mountain building occurs where crust stress is high, with elevation-dependent resistance
            if (crustStress > 1.5f) {
                float currentElevation = fields.elevationField->getSample(x, z);
                
                // Higher elevations resist further uplift (isostatic equilibrium effect)
                float elevationResistance = 1.0f + (currentElevation / 1000.0f) * 0.5f; // Resistance increases with height
                
                // Realistic compression force calculation with physical limits
                float compressionForce = (crustStress - 1.5f) / elevationResistance * timeStepMyears * 0.001f; // Much smaller scaling
                
                // Apply maximum realistic uplift rate (a few mm per year in extreme cases)
                float maxUpliftPerStep = 0.01f * timeStepMyears; // 10mm per thousand years maximum
                compressionForce = std::min(compressionForce, maxUpliftPerStep);
                
                // DEBUG: Log compression force calculation for high values
                if (compressionForce > 1.0f) {
                    std::cout << "[COMPRESSION_DEBUG] High compressionForce: " << compressionForce 
                              << " = (" << crustStress << " - 1.5) / " << elevationResistance 
                              << " * " << timeStepMyears << " * 0.001 at (" << x << "," << z << ")" << std::endl;
                }
                
                applyMountainBuilding(fields, x, z, compressionForce, timeStepMyears);
                
                validateAndClampElevation(fields, x, z, "MountainBuilding");
            }
        }
    }
}

void TectonicEngine::simulateVolcanicActivity(TectonicFields& fields, float timeStepMyears) {
    if (!fields.mantleStress || !fields.elevationField) return;

    int width = fields.mantleStress->getWidth();
    int height = fields.mantleStress->getHeight();
    int activeVolcanoes = 0;
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float mantleStress = fields.mantleStress->getSample(x, z);
            
            // Volcanic activity occurs where mantle stress is very high
            if (mantleStress > 3.0f) {
                float intensity = (mantleStress - 3.0f) * timeStepMyears * 0.2f;
                createVolcanicActivity(fields, x, z, intensity, timeStepMyears);
                activeVolcanoes++;
                
                validateAndClampElevation(fields, x, z, "VolcanicActivity");
            }
        }
    }
    
    metrics_.activeVolcanoes = activeVolcanoes;
    metrics_.volcanicActivity = static_cast<float>(activeVolcanoes) / (width * height);
}

void TectonicEngine::simulateIsostasyAdjustment(TectonicFields& fields, float timeStepMyears) {
    if (!fields.crustalThickness || !fields.isostasyAdjustment || !fields.elevationField) return;

    int width = fields.crustalThickness->getWidth();
    int height = fields.crustalThickness->getHeight();
    
    // Scale down the time step to prevent runaway calculations
    float safeTimeStep = std::min(timeStepMyears, 100.0f); // Cap at 100 years to prevent instability
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float crustalThickness = fields.crustalThickness->getSample(x, z);
            float targetThickness = 35000.0f; // Standard continental crust thickness (35km)
            
            // Calculate isostatic adjustment based on thickness difference
            float thicknessDiff = crustalThickness - targetThickness;
            
            // Much smaller scaling factor to prevent exponential growth
            // Using proper geological isostatic response rate (~1mm/year per km of load)
            float adjustmentRate = thicknessDiff * 0.000001f; // 1 meter per 1000 years per km difference
            float adjustment = adjustmentRate * safeTimeStep;
            
            // Clamp adjustment to prevent runaway calculations
            adjustment = std::max(-10.0f, std::min(10.0f, adjustment));
            
            applyIsostasyAdjustment(fields, x, z, adjustment, safeTimeStep);
            
            validateAndClampElevation(fields, x, z, "IsostasyAdjustment");
        }
    }
}

void TectonicEngine::updateMetrics(const TectonicFields& fields) {
    if (!fields.elevationField || !fields.crustalThickness) return;
    
    float totalUplift = 0.0f;
    float totalSubsidence = 0.0f;
    float totalThickness = 0.0f;
    int count = 0;
    
    int width = fields.elevationField->getWidth();
    int height = fields.elevationField->getHeight();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float elevation = fields.elevationField->getSample(x, z);
            float thickness = fields.crustalThickness->getSample(x, z);
            
            if (elevation > 0.0f) totalUplift += elevation;
            if (elevation < 0.0f) totalSubsidence += std::abs(elevation);
            totalThickness += thickness;
            count++;
        }
    }
    
    metrics_.totalUplift = totalUplift;
    metrics_.totalSubsidence = totalSubsidence;
    metrics_.averageCrustalThickness = count > 0 ? totalThickness / count : 35000.0f;
}

float TectonicEngine::getRockHardness(RockType rockType) const {
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

void TectonicEngine::validateAndClampElevation(TectonicFields& fields, int x, int z, const std::string& processName) {
    if (!fields.elevationField) return;
    
    float elevation = fields.elevationField->getSample(x, z);
    
    // Check for extreme values that indicate a bug - but limit spam
    static int errorCount = 0;
    if (std::abs(elevation) > 10000.0f && errorCount < 10) {
        std::cout << "[ELEVATION_BUG] " << processName << " created extreme elevation: " 
                  << elevation << "m at (" << x << "," << z << ") - CLAMPING to bounds" << std::endl;
        errorCount++;
        if (errorCount == 10) {
            std::cout << "[ELEVATION_BUG] Further elevation clamping messages suppressed to reduce spam" << std::endl;
        }
    }
    
    // Clamp to realistic geological bounds
    float clampedElevation = std::max(-1800.0f, std::min(1200.0f, elevation));
    
    if (elevation != clampedElevation) {
        fields.elevationField->setSample(x, z, clampedElevation);
    }
}

// Private helper methods
void TectonicEngine::applyMantleConvectionCell(TectonicFields& fields, int x, int z, float intensity, float timeStep) {
    // DEBUG: Track elevation changes for the first few cells in first few steps
    static int debugCellCount = 0;
    static int debugStepCount = 0;
    bool shouldDebug = (debugStepCount < 3) && (debugCellCount < 20) && (x % 50 == 0) && (z % 50 == 0);
    
    if (shouldDebug) {
        float beforeElevation = fields.elevationField->getSample(x, z);
        std::cout << "[MANTLE_DEBUG] Step " << debugStepCount << " Cell(" << x << "," << z 
                  << ") Before: " << beforeElevation << "m, Intensity: " << intensity 
                  << ", TimeStep: " << timeStep << std::endl;
    }
    
    // Update mantle stress with equilibrium limits
    float currentStress = fields.mantleStress->getSample(x, z);
    float maxMantleStress = 20.0f; // Realistic maximum mantle stress
    
    // Apply stress change but approach equilibrium rather than accumulate infinitely
    float stressTarget = currentStress + intensity;
    float equilibriumFactor = 0.95f; // 95% approach to target each step
    float newStress = currentStress + (stressTarget - currentStress) * equilibriumFactor;
    
    // Apply hard limits to prevent runaway values
    newStress = std::max(-maxMantleStress, std::min(newStress, maxMantleStress));
    fields.mantleStress->setSample(x, z, newStress);
    
    // Apply elevation change based on convection (much smaller scale)
    float elevationChange = intensity * 0.1f; // Reduced scale - 0.1m per unit intensity
    float currentElevation = fields.elevationField->getSample(x, z);
    fields.elevationField->setSample(x, z, currentElevation + elevationChange);
    
    if (shouldDebug) {
        float afterElevation = fields.elevationField->getSample(x, z);
        std::cout << "[MANTLE_DEBUG] After: " << afterElevation << "m, Change: " 
                  << elevationChange << "m" << std::endl;
        debugCellCount++;
        if (debugCellCount >= 20) {
            debugStepCount++;
            debugCellCount = 0;
        }
    }
    
    // Update crustal thickness if available (with limits)
    if (fields.crustalThickness) {
        float thicknessChange = intensity * 1.0f; // Much smaller thickness change
        float currentThickness = fields.crustalThickness->getSample(x, z);
        float newThickness = currentThickness + thicknessChange;
        
        // Apply realistic crustal thickness limits (20-80km)
        newThickness = std::max(20000.0f, std::min(newThickness, 80000.0f));
        fields.crustalThickness->setSample(x, z, newThickness);
    }
}

void TectonicEngine::applyPlateMotion(TectonicFields& fields, int x, int z, float motionX, float motionZ, float timeStep) {
    // Apply lateral stress from plate motion with realistic limits
    float currentCrustStress = fields.crustStress->getSample(x, z);
    float stressIncrease = std::sqrt(motionX * motionX + motionZ * motionZ) * 0.01f; // Much smaller factor
    
    // Apply equilibrium approach rather than infinite accumulation
    float maxCrustStress = 50.0f; // Realistic maximum crustal stress
    float newStress = std::min(currentCrustStress + stressIncrease, maxCrustStress);
    
    // Apply stress dissipation (crustal stress naturally decays)
    float dissipationRate = 0.001f * timeStep; // 0.1% per thousand years
    newStress = std::max(0.0f, newStress - newStress * dissipationRate);
    
    fields.crustStress->setSample(x, z, newStress);
}

void TectonicEngine::applyMountainBuilding(TectonicFields& fields, int x, int z, float compressionForce, float timeStep) {
    // Apply uplift from mountain building
    float currentElevation = fields.elevationField->getSample(x, z);
    
    // DEBUG: Log extreme compression forces
    if (compressionForce > 100.0f) {
        std::cout << "[MOUNTAIN_DEBUG] Extreme compressionForce: " << compressionForce 
                  << " at (" << x << "," << z << ") - current elevation: " << currentElevation << "m" << std::endl;
    }
    
    // Apply uplift with physical limits to prevent runaway values
    float maxUpliftPerStep = 10.0f; // Maximum 10m uplift per timestep
    float uplift = std::min(compressionForce * 50.0f, maxUpliftPerStep);
    
    float newElevation = currentElevation + uplift;
    fields.elevationField->setSample(x, z, newElevation);
    
    // DEBUG: Log if we applied significant uplift
    if (uplift > 5.0f) {
        std::cout << "[MOUNTAIN_DEBUG] Applied " << uplift << "m uplift at (" << x << "," << z 
                  << ") - elevation: " << currentElevation << "m -> " << newElevation << "m" << std::endl;
    }
    
    // Update rock type to metamorphic under high stress
    if (compressionForce > 2.0f && fields.rockTypes) {
        RockType currentType = fields.rockTypes->getSample(x, z);
        if (currentType == RockType::SEDIMENTARY_SANDSTONE) {
            fields.rockTypes->setSample(x, z, RockType::METAMORPHIC_SLATE);
        } else if (currentType == RockType::IGNEOUS_GRANITE) {
            fields.rockTypes->setSample(x, z, RockType::METAMORPHIC_MARBLE);
        }
    }
}

void TectonicEngine::createVolcanicActivity(TectonicFields& fields, int x, int z, float intensity, float timeStep) {
    // Create volcanic elevation
    float currentElevation = fields.elevationField->getSample(x, z);
    float volcanicUplift = intensity * 80.0f; // Scale to meters
    fields.elevationField->setSample(x, z, currentElevation + volcanicUplift);
    
    // Set rock type to igneous
    if (fields.rockTypes) {
        fields.rockTypes->setSample(x, z, RockType::IGNEOUS_BASALT);
    }
    
    // Update rock hardness
    if (fields.rockHardness) {
        fields.rockHardness->setSample(x, z, getRockHardness(RockType::IGNEOUS_BASALT));
    }
}

void TectonicEngine::applyIsostasyAdjustment(TectonicFields& fields, int x, int z, float adjustment, float timeStep) {
    // Apply isostatic elevation adjustment
    float currentElevation = fields.elevationField->getSample(x, z);
    fields.elevationField->setSample(x, z, currentElevation + adjustment);
    
    // Update isostasy adjustment field
    fields.isostasyAdjustment->setSample(x, z, adjustment);
}

float TectonicEngine::generateFractalNoise(float x, float z, float scale1, float scale2, float scale3) const {
    // Convert the large seed to smaller, stable seed values for noise layers
    int seed1 = static_cast<int>(seed_ & 0xFFFF);           // Bottom 16 bits
    int seed2 = static_cast<int>((seed_ >> 16) & 0xFFFF);   // Next 16 bits
    int seed3 = static_cast<int>((seed_ >> 32) & 0xFFFF);   // Next 16 bits
    
    float noise1 = VoxelEngine::Util::smoothValueNoise(x * scale1, 0.0f, z * scale1 + seed1) * 0.5f;
    float noise2 = VoxelEngine::Util::smoothValueNoise(x * scale2, 0.0f, z * scale2 + seed2) * 0.3f;
    float noise3 = VoxelEngine::Util::smoothValueNoise(x * scale3, 0.0f, z * scale3 + seed3) * 0.2f;
    
    return noise1 + noise2 + noise3;
}

float TectonicEngine::generateDomainWarpedNoise(float x, float z, float scale, float warpIntensity) const {
    // Convert the large seed to smaller, stable seed values
    int seed1 = static_cast<int>((seed_ + 1000) & 0xFFFF);
    int seed2 = static_cast<int>((seed_ + 2000) & 0xFFFF);
    int seed3 = static_cast<int>((seed_ + 3000) & 0xFFFF);
    
    float warpX = VoxelEngine::Util::smoothValueNoise(x * scale * 2.0f, 0.0f, z * scale * 2.0f + seed1) * warpIntensity;
    float warpZ = VoxelEngine::Util::smoothValueNoise(x * scale * 2.0f, 0.0f, z * scale * 2.0f + seed2) * warpIntensity;
    
    return VoxelEngine::Util::smoothValueNoise(x + warpX, 0.0f, z + warpZ + seed3);
}

}}
