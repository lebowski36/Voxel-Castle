#include "world/TectonicEngine.h"
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
        
        // Add spatial tectonic variation: simulate convergent/divergent zones
        float tectonicZone = std::sin(worldX * 0.0005f) + std::cos(worldZ * 0.0005f); // -2 to +2
        float zoneMultiplier = 1.0f + tectonicZone * 0.3f; // 0.7 to 1.3, moderate effect
        
        // EARTH-LIKE SCALING: For 1M year timesteps, mantle convection should produce 1-10m total change
        // Real mantle convection: ~0.001-0.01m/year = 1-10m per 1000 years = 1000-10000m per million years
        // BALANCE FORCES: Need stronger downward forces to counteract tectonic uplift
        float earthLikeIntensity = (convectionIntensity * 0.7f + warpedConvection * 0.3f) * mantleTimeScale;
        float finalIntensity = earthLikeIntensity * 2.0f * zoneMultiplier; // Increased from 0.002 to 2.0 to provide proper balance
        applyMantleConvectionCell(fields, x, z, finalIntensity, timeStepMyears);
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
            }                // EARTH-LIKE MOUNTAIN BUILDING: Real rates are 1-10mm/year = 1-10m per 1000 years
                // For 1M year timesteps: 1000-10000m per million years, but most areas much less
                // Lower threshold but use realistic scaling for Earth-like mountain building
                if (crustStress > 0.5f) {
                    float currentElevation = fields.elevationField->getSample(x, z);
                    float elevationResistance = 1.0f + (currentElevation / 1000.0f) * 0.5f; // More resistance at height
                    
                    // REALISTIC SCALING: 1-5m per million years for typical mountain building
                    float compressionForce = (crustStress - 0.5f) / elevationResistance * timeStepMyears * 0.000005f; // Earth-like rate
                    float maxUpliftPerStep = SCALE_FOR_MYEARS(GeologicalConstants::TYPICAL_TECTONIC_UPLIFT_RATE); // Use global constant
                    compressionForce = std::min(compressionForce, maxUpliftPerStep);
                if (compressionForce > 1.0f) {
                    std::cout << "[COMPRESSION_DEBUG] High compressionForce: " << compressionForce 
                              << " = (" << crustStress << " - 0.5) / " << elevationResistance 
                              << " * " << timeStepMyears << " * 0.02 at (" << x << "," << z << ")" << std::endl;
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
            
            // EARTH-LIKE VOLCANIC ACTIVITY: Real rates vary widely but average ~0.1-1m per 1000 years
            // For 1M year timesteps: 100-1000m per million years during active periods
            if (mantleStress > 3.0f) {
                // Realistic volcanic scaling for 1M year timesteps
                float intensity = (mantleStress - 3.0f) * timeStepMyears * 0.0001f; // Much more conservative scaling
                createVolcanicActivity(fields, x, z, intensity, timeStepMyears);
                activeVolcanoes++;
                
                validateAndClampElevation(fields, x, z, "VolcanicActivity");
            }
        }
    }
    
    metrics_.activeVolcanoes = activeVolcanoes;
    metrics_.volcanicActivity = static_cast<float>(activeVolcanoes) / (width * height);
}

void TectonicEngine::simulateRiftingActivity(TectonicFields& fields, float timeStepMyears) {
    if (!fields.riftingStress || !fields.elevationField) return;

    // Initialize rift zones if this is the first call
    if (riftZones_.empty()) {
        generateRiftingStress(fields, timeStepMyears);
    }

    int width = fields.riftingStress->getWidth();
    int height = fields.riftingStress->getHeight();
    int activeRifts = 0;
    
    std::cout << "[TectonicEngine] Simulating rifting activity with " << riftZones_.size() << " rift zones" << std::endl;
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float worldX = x * fields.riftingStress->getSampleSpacing();
            float worldZ = z * fields.riftingStress->getSampleSpacing();
            
            if (isInRiftZone(worldX, worldZ)) {
                float riftingStress = fields.riftingStress->getSample(x, z);
                
                // EARTH-LIKE RIFTING: Real rates are 0.1-10mm/year = 0.0001-0.01m/year
                // For 1M year timesteps: 100-10000m per million years in active rifts
                if (riftingStress > 2.0f) { // Threshold for active rifting
                    // FIXED: Realistic rifting intensity calculation
                    // Real rifting rates: 0.1-10mm/year = 0.0001-0.01m/year
                    // For 1M year timesteps: 100-10000m per million years in active rifts
                    // Use reasonable scaling to achieve 100-1000m subsidence in active zones
                    float intensity = (riftingStress - 2.0f) * timeStepMyears * 0.01f; // 100,000x stronger than before
                    
                    std::cout << "[RIFTING_CALC] At (" << x << "," << z << ") - riftingStress: " 
                              << riftingStress << ", timeStep: " << timeStepMyears 
                              << ", calculated intensity: " << intensity << std::endl;
                    
                    applyRiftingForces(fields, x, z, intensity, timeStepMyears);
                    activeRifts++;
                    
                    validateAndClampElevation(fields, x, z, "RiftingActivity");
                }
            }
        }
    }
    
    metrics_.activeRiftZones = activeRifts;
    metrics_.totalRiftingActivity = static_cast<float>(activeRifts) / (width * height);
    
    std::cout << "[TectonicEngine] Rifting simulation complete - " << activeRifts << " active rift points" << std::endl;
}

void TectonicEngine::applyRiftingForces(TectonicFields& fields, int x, int z, float intensity, float timeStep) {
    // Rifting creates downward subsidence and crustal thinning
    float currentElevation = fields.elevationField->getSample(x, z);
    
    // DEBUG: Track what we're doing
    std::cout << "[RIFTING_DETAILED] At (" << x << "," << z << ") - Current elevation: " 
              << currentElevation << "m, intensity: " << intensity << std::endl;
    
    // Rifting creates graben (down-dropped valleys) - DOWNWARD force to balance mountain building
    // Use reasonable direct scaling - intensity is already in a good range (0-10000)
    float subsidenceForce = intensity * 0.1f; // Scale intensity to meters: 1000 intensity = 100m subsidence
    float maxSubsidencePerStep = 200.0f; // Maximum 200m subsidence per step (realistic for rift valleys)
    subsidenceForce = std::min(subsidenceForce, maxSubsidencePerStep);
    
    // CRITICAL: Apply downward force (MINUS for subsidence)
    float newElevation = currentElevation - subsidenceForce;
    
    // Only clamp to prevent extreme values, but allow reasonable rifting
    if (newElevation < GeologicalConstants::MIN_ELEVATION_BOUND) {
        newElevation = GeologicalConstants::MIN_ELEVATION_BOUND;
    }
    // Don't clamp upward - allow rifting to lower ANY elevation
    
    std::cout << "[RIFTING_DETAILED] Applying " << subsidenceForce << "m subsidence: " 
              << currentElevation << "m -> " << newElevation << "m" << std::endl;
    
    fields.elevationField->setSample(x, z, newElevation);
    
    // Verify the change actually happened
    float verifyElevation = fields.elevationField->getSample(x, z);
    if (std::abs(verifyElevation - newElevation) > 0.1f) {
        std::cout << "[RIFTING_ERROR] Elevation not set correctly! Expected: " << newElevation 
                  << "m, Got: " << verifyElevation << "m" << std::endl;
    }
    
    // Check for extreme elevations and warn (this should warn about the RESULT)
    WARN_EXTREME_ELEVATION(newElevation, "RiftingActivity", x, z);
    
    // Rifting causes crustal thinning
    if (fields.crustalThickness) {
        float currentThickness = fields.crustalThickness->getSample(x, z);
        float thinning = intensity * 100.0f; // Crustal thinning in meters
        fields.crustalThickness->setSample(x, z, std::max(15000.0f, currentThickness - thinning));
    }
    
    // Rifting can trigger volcanic activity (basaltic upwelling) - only occasionally
    if (fields.rockTypes && intensity > 8.0f) { // Higher threshold
        fields.rockTypes->setSample(x, z, RockType::IGNEOUS_BASALT);
    }
}

bool TectonicEngine::isInRiftZone(float worldX, float worldZ) const {
    for (const auto& rift : riftZones_) {
        if (!rift.active) continue;
        
        // Calculate distance to rift center
        float dx = worldX - rift.centerX;
        float dz = worldZ - rift.centerZ;
        
        // Rotate coordinates to rift orientation
        float cos_angle = std::cos(rift.orientation);
        float sin_angle = std::sin(rift.orientation);
        float rotated_x = dx * cos_angle + dz * sin_angle;
        float rotated_z = -dx * sin_angle + dz * cos_angle;
        
        // Check if point is within rift zone ellipse
        float length_factor = (rotated_x * rotated_x) / (rift.length * rift.length * 0.25f);
        float width_factor = (rotated_z * rotated_z) / (rift.width * rift.width * 0.25f);
        
        if (length_factor + width_factor <= 1.0f) {
            return true;
        }
    }
    return false;
}

void TectonicEngine::createRiftValley(TectonicFields& fields, int x, int z, float intensity) {
    // Create a rift valley depression
    float currentElevation = fields.elevationField->getSample(x, z);
    float valleyDepth = intensity * 2.0f; // 2m depth per intensity unit
    
    fields.elevationField->setSample(x, z, currentElevation - valleyDepth);
}

void TectonicEngine::generateRiftingStress(TectonicFields& fields, float timeStepMyears) {
    int width = fields.riftingStress->getWidth();
    int height = fields.riftingStress->getHeight();
    float worldSizeMeters = worldSizeKm_ * 1000.0f;
    
    std::cout << "[TectonicEngine] Generating " << config_.custom.numContinents/2 << " rift zones for world" << std::endl;
    
    // Generate realistic rift zones based on continental configuration
    int numRifts = std::max(1, config_.custom.numContinents / 2); // 1-3 major rift systems
    std::uniform_real_distribution<float> posDist(0.2f, 0.8f); // Avoid edges
    std::uniform_real_distribution<float> orientDist(0.0f, 3.14159f); // Random orientation
    std::uniform_real_distribution<float> lengthDist(0.15f, 0.3f); // 15-30% of world size
    
    for (int i = 0; i < numRifts; ++i) {
        RiftZone rift;
        rift.centerX = posDist(rng_) * worldSizeMeters;
        rift.centerZ = posDist(rng_) * worldSizeMeters;
        rift.orientation = orientDist(rng_);
        rift.length = lengthDist(rng_) * worldSizeMeters;
        rift.width = rift.length * 0.2f; // Width is 20% of length
        rift.extensionRate = 1.0f + (i * 0.5f); // Varying extension rates
        rift.depth = -200.0f - (i * 100.0f); // Varying depths
        rift.active = true;
        
        riftZones_.push_back(rift);
        
        std::cout << "[TectonicEngine] Created rift zone " << i << " at (" << rift.centerX/1000.0f 
                  << "km, " << rift.centerZ/1000.0f << "km) length=" << rift.length/1000.0f 
                  << "km orientation=" << (rift.orientation * 180.0f / 3.14159f) << "°" << std::endl;
    }
    
    // Initialize rifting stress field based on rift zones
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float worldX = x * fields.riftingStress->getSampleSpacing();
            float worldZ = z * fields.riftingStress->getSampleSpacing();
            
            float maxStress = 0.0f;
            for (const auto& rift : riftZones_) {
                // Calculate distance-based stress
                float dx = worldX - rift.centerX;
                float dz = worldZ - rift.centerZ;
                float distance = std::sqrt(dx*dx + dz*dz);
                float maxRiftDistance = std::max(rift.length, rift.width) * 0.5f;
                
                if (distance < maxRiftDistance) {
                    // FIXED: More realistic stress distribution - peak stress around 3-4, not 25
                    float stress = (1.0f - distance / maxRiftDistance) * rift.extensionRate * 2.0f; // Reduced from 5.0f
                    maxStress = std::max(maxStress, stress);
                }
            }
            
            fields.riftingStress->setSample(x, z, maxStress);
        }
    }
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
    
    // FIXED: Apply realistic geological elevation bounds: -2048m to +2048m (world height limits)
    // Expected terrain range: -1800m to +1800m with ±2048m as absolute physical limits
    float clampedElevation = std::max(-2048.0f, std::min(2048.0f, elevation));
    
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
    float maxMantleStress = GeologicalConstants::MAX_MANTLE_STRESS; // Use global geological constant
    
    // Apply stress change but approach equilibrium rather than accumulate infinitely
    float stressTarget = currentStress + intensity;
    float equilibriumFactor = 0.95f; // 95% approach to target each step
    float newStress = currentStress + (stressTarget - currentStress) * equilibriumFactor;
    
    // Apply hard limits to prevent runaway values
    newStress = std::max(-maxMantleStress, std::min(newStress, maxMantleStress));
    fields.mantleStress->setSample(x, z, newStress);
    
    // Apply elevation change based on convection (increased scale for balance)
    float elevationChange = intensity * 100.0f; // Increased from 0.1 to 100.0 to provide proper geological balance
    float currentElevation = fields.elevationField->getSample(x, z);
    
    // Add rifting/downward bias in ocean areas to maintain ocean basins
    if (currentElevation < -500.0f) { // Ocean areas
        elevationChange *= 0.5f; // Reduce uplift in oceans
        elevationChange -= std::abs(intensity) * 50.0f; // Add downward rift force
    }
    
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
        newThickness = std::max(20000.0f, std::min(newThickness, GeologicalConstants::MAX_CRUSTAL_THICKNESS));
        fields.crustalThickness->setSample(x, z, newThickness);
    }
}

void TectonicEngine::applyPlateMotion(TectonicFields& fields, int x, int z, float motionX, float motionZ, float timeStep) {
    // Apply lateral stress from plate motion with realistic limits
    float currentCrustStress = fields.crustStress->getSample(x, z);
    float stressIncrease = std::sqrt(motionX * motionX + motionZ * motionZ) * 0.01f; // Much smaller factor
    
    // Apply equilibrium approach rather than infinite accumulation
    float maxCrustStress = GeologicalConstants::MAX_CRUSTAL_STRESS; // Use global geological constant
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
    
    // Apply uplift with realistic Earth-like limits using global constants
    // Real mountain building: 1-10mm/year = 1000-10000m per million years max
    // But most areas see much less - maybe 10-100m per million years typically
    float maxUpliftPerStep = SCALE_FOR_MYEARS(GeologicalConstants::TYPICAL_TECTONIC_UPLIFT_RATE * 0.002f); // Conservative for stable simulation
    float uplift = std::min(compressionForce * 1.0f, maxUpliftPerStep); // Remove excessive 50x multiplier
    
    float newElevation = CLAMP_GEOLOGICAL_ELEVATION(currentElevation + uplift);        fields.elevationField->setSample(x, z, newElevation);
        
        // Check for extreme elevations and warn
        WARN_EXTREME_ELEVATION(newElevation, "MountainBuilding", x, z);
    
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
    // Create volcanic elevation with realistic scaling
    float currentElevation = fields.elevationField->getSample(x, z);
    
    // Much more conservative volcanic uplift - 1-10m max per event
    float volcanicUplift = std::min(intensity * 0.01f, 10.0f); // Clamp to max 10m uplift
    
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
