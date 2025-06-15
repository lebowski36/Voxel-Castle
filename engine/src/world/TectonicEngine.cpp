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
        
        // Combine patterns for realistic mantle flow
        float finalIntensity = (convectionIntensity * 0.7f + warpedConvection * 0.3f) * timeStepMyears * 0.1f;
        
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
            
            // Mountain building occurs where crust stress is high
            if (crustStress > 1.5f) {
                float compressionForce = (crustStress - 1.5f) * timeStepMyears * 0.8f;
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
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float crustalThickness = fields.crustalThickness->getSample(x, z);
            float targetThickness = 35000.0f; // Standard continental crust thickness
            
            // Calculate isostatic adjustment based on thickness difference
            float thicknessDiff = crustalThickness - targetThickness;
            float adjustment = thicknessDiff * 0.0001f * timeStepMyears; // Small adjustment per time step
            
            applyIsostasyAdjustment(fields, x, z, adjustment, timeStepMyears);
            
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
void TectonicEngine::applyMantleConvectionCell(TectonicFields& fields, int x, int z, float intensity, float timeStep) {
    // Update mantle stress
    float currentStress = fields.mantleStress->getSample(x, z);
    fields.mantleStress->setSample(x, z, currentStress + intensity);
    
    // Apply elevation change based on convection
    float elevationChange = intensity * 10.0f; // Scale to meters
    float currentElevation = fields.elevationField->getSample(x, z);
    fields.elevationField->setSample(x, z, currentElevation + elevationChange);
    
    // Update crustal thickness if available
    if (fields.crustalThickness) {
        float thicknessChange = intensity * 100.0f; // Scale to thickness change
        float currentThickness = fields.crustalThickness->getSample(x, z);
        fields.crustalThickness->setSample(x, z, currentThickness + thicknessChange);
    }
}

void TectonicEngine::applyPlateMotion(TectonicFields& fields, int x, int z, float motionX, float motionZ, float timeStep) {
    // Apply lateral stress from plate motion
    float currentCrustStress = fields.crustStress->getSample(x, z);
    float stressIncrease = std::sqrt(motionX * motionX + motionZ * motionZ) * 0.5f;
    fields.crustStress->setSample(x, z, currentCrustStress + stressIncrease);
}

void TectonicEngine::applyMountainBuilding(TectonicFields& fields, int x, int z, float compressionForce, float timeStep) {
    // Apply uplift from mountain building
    float currentElevation = fields.elevationField->getSample(x, z);
    float uplift = compressionForce * 50.0f; // Scale to meters
    fields.elevationField->setSample(x, z, currentElevation + uplift);
    
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
    float noise1 = VoxelEngine::Util::smoothValueNoise(x * scale1, seed_, z * scale1) * 0.5f;
    float noise2 = VoxelEngine::Util::smoothValueNoise(x * scale2, seed_ + 1000, z * scale2) * 0.3f;
    float noise3 = VoxelEngine::Util::smoothValueNoise(x * scale3, seed_ + 2000, z * scale3) * 0.2f;
    
    return noise1 + noise2 + noise3;
}

float TectonicEngine::generateDomainWarpedNoise(float x, float z, float scale, float warpIntensity) const {
    float warpX = VoxelEngine::Util::smoothValueNoise(x * scale * 2.0f, 1000 + seed_, z * scale * 2.0f) * warpIntensity;
    float warpZ = VoxelEngine::Util::smoothValueNoise(x * scale * 2.0f, 2000 + seed_, z * scale * 2.0f) * warpIntensity;
    
    return VoxelEngine::Util::smoothValueNoise(x + warpX, seed_ + 3000, z + warpZ);
}

}}
