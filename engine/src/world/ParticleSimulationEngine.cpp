#include "world/ParticleSimulationEngine.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace VoxelCastle {
namespace World {

ParticleSimulationEngine::ParticleSimulationEngine(float worldSizeKm, uint64_t seed)
    : worldSizeKm_(worldSizeKm)
    , worldSizeM_(worldSizeKm * 1000.0f)
    , seed_(seed)
    , rng_(seed)
    , continentCount_(0)
    , oceanRatio_(0.71f) {
    
    std::cout << "[ParticleSimulationEngine] Initializing with worldSize: " << worldSizeKm 
              << "km, seed: " << seed << std::endl;
    
    rng_.seed(seed);
    
    // Create spatial hash grid for efficient particle queries
    spatialGrid_ = std::make_unique<SpatialHashGrid>(worldSizeM_, PARTICLE_INTERACTION_RADIUS);
    
    std::cout << "[ParticleSimulationEngine] Initialized for " << worldSizeKm 
              << "km world with seed " << seed << std::endl;
}

ParticleSimulationEngine::~ParticleSimulationEngine() = default;

void ParticleSimulationEngine::InitializeParticles(int continentCount, float oceanRatio) {
    continentCount_ = continentCount;
    oceanRatio_ = oceanRatio;
    
    particles_.clear();
    spatialGrid_->Clear();
    
    std::cout << "[ParticleSimulationEngine] Initializing " << continentCount 
              << " continents with " << (oceanRatio * 100.0f) << "% ocean ratio" << std::endl;
    
    // Calculate continental parameters - IMPROVED for larger, more cohesive continents
    float continentalArea = worldSizeM_ * worldSizeM_ * (1.0f - oceanRatio);
    float avgContinentArea = continentalArea / continentCount;
    
    // IMPROVEMENT 1: Larger continental radius (removed 0.8f reduction factor)
    float avgContinentRadius = std::sqrt(avgContinentArea / M_PI) * 1.1f; // Actually larger than calculated area
    
    // IMPROVEMENT 2: Allow closer continental placement for larger landmasses
    float minContinentDistance = avgContinentRadius * 1.2f; // Reduced from 1.5f to 1.2f
    
    // Generate continental positions using Poisson disk sampling for realistic distribution
    std::vector<glm::vec2> continentCenters;
    std::uniform_real_distribution<float> worldDist(avgContinentRadius, worldSizeM_ - avgContinentRadius);
    
    for (int i = 0; i < continentCount; ++i) {
        glm::vec2 center;
        int attempts = 0;
        bool validPosition = false;
        
        while (!validPosition && attempts < 100) {
            center = glm::vec2(worldDist(rng_), worldDist(rng_));
            
            // Check minimum distance from other continents
            validPosition = true;
            for (const auto& existing : continentCenters) {
                float distance = CalculateDistance(center, existing);
                if (distance < minContinentDistance) {
                    validPosition = false;
                    break;
                }
            }
            attempts++;
        }
        
        if (validPosition) {
            continentCenters.push_back(center);
            CreateContinentalParticles(i, center, avgContinentRadius);
        }
    }
    
    // Fill remaining areas with oceanic particles
    CreateOceanicParticles();
    
    // Rebuild spatial grid
    for (size_t i = 0; i < particles_.size(); ++i) {
        if (particles_[i].isActive) {
            spatialGrid_->Insert(i, particles_[i].position);
        }
    }
    
    std::cout << "[ParticleSimulationEngine] Created " << particles_.size() 
              << " particles (" << continentCenters.size() << " continents)" << std::endl;
}

void ParticleSimulationEngine::CreateContinentalParticles(int continentIndex, glm::vec2 center, float radius) {
    // Use fractal coastline generation for realistic continental shapes
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radiusDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> densityDist(0.8f, 1.0f); // Increased density for more cohesive landmasses
    std::uniform_real_distribution<float> fractalDist(-1.0f, 1.0f);
    
    // Calculate particle density - IMPROVED for larger, more cohesive continents
    float continentArea = M_PI * radius * radius;
    
    // IMPROVEMENT 3: Higher particle density for more solid, cohesive landmasses
    int particleCount = static_cast<int>(continentArea / (PARTICLE_INTERACTION_RADIUS * PARTICLE_INTERACTION_RADIUS * 0.2f)); // Increased density (was 0.3f)
    
    // IMPROVEMENT 4: Higher minimum particle count for substantial landmasses
    particleCount = std::max(300, std::min(1200, particleCount)); // Increased from 200-800 to 300-1200
    
    // IMPROVEMENT 5: More concentrated core for cohesive landmasses, less scattered periphery
    int coreParticles = static_cast<int>(particleCount * 0.85f); // Increased from 80% to 85%
    int peripheryParticles = particleCount - coreParticles;
    
    // Generate seed-based continental shape parameters
    std::uniform_real_distribution<float> shapeDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> elongationDist(0.6f, 2.5f); // How stretched the continent is
    std::uniform_real_distribution<float> orientationDist(0.0f, 2.0f * M_PI); // Continental orientation
    
    // Seed-based shape characteristics for this continent
    uint32_t continentSeed = seed_ + continentIndex * 12345;
    std::mt19937 shapeRng(continentSeed);
    
    float shapeRandomness = shapeDist(shapeRng);
    float elongationFactor = elongationDist(shapeRng); // 1.0 = circular, >1.0 = elongated
    float continentOrientation = orientationDist(shapeRng); // Main axis orientation
    
    // Determine continental shape type based on seed
    enum class ContinentalShape { CIRCULAR, OVAL, ELONGATED, CRESCENT };
    ContinentalShape shapeType;
    
    if (shapeRandomness < 0.3f) {
        shapeType = ContinentalShape::CIRCULAR;
        elongationFactor = 0.8f + shapeDist(shapeRng) * 0.4f; // 0.8-1.2
    } else if (shapeRandomness < 0.6f) {
        shapeType = ContinentalShape::OVAL;
        elongationFactor = 1.2f + shapeDist(shapeRng) * 0.8f; // 1.2-2.0
    } else if (shapeRandomness < 0.85f) {
        shapeType = ContinentalShape::ELONGATED;
        elongationFactor = 1.8f + shapeDist(shapeRng) * 0.7f; // 1.8-2.5
    } else {
        shapeType = ContinentalShape::CRESCENT;
        elongationFactor = 1.4f + shapeDist(shapeRng) * 0.6f; // 1.4-2.0
    }

    // Core continental particles (main landmass)
    for (int i = 0; i < coreParticles; ++i) {
        TectonicParticle particle;
        
        // Generate position using fractal coastline algorithm - concentrated in center
        float angle = angleDist(rng_);
        
        // Create fractal coastline variation using multiple octaves of noise
        float fractalRadius = radius * 0.7f; // Smaller radius for core landmass
        
        // Apply fractal variation to radius (creates irregular coastlines)
        // Reduced variation for more cohesive landmasses
        float noise1 = std::sin(angle * 2.0f + continentIndex * 2.0f) * 0.2f; // Reduced from 0.3f
        float noise2 = std::sin(angle * 6.0f + continentIndex * 3.14f) * 0.1f; // Reduced from 0.15f
        float noise3 = std::sin(angle * 15.0f + continentIndex * 1.57f) * 0.05f; // Reduced from 0.08f
        
        // Combine fractal noise to create realistic coastline variation
        float coastlineVariation = noise1 + noise2 + noise3;
        fractalRadius *= (1.0f + coastlineVariation);
        
        // More concentrated distribution for main landmass
        float r = std::pow(radiusDist(rng_), 0.5f) * fractalRadius * densityDist(rng_); // Square root for more center concentration
        
        // Apply continental shape transformation
        glm::vec2 basePosition = glm::vec2(std::cos(angle) * r, std::sin(angle) * r);
        
        // Transform based on continental shape type
        switch (shapeType) {
            case ContinentalShape::CIRCULAR:
                // Keep roughly circular with minor variation
                basePosition.y *= (0.9f + shapeDist(shapeRng) * 0.2f);
                break;
                
            case ContinentalShape::OVAL:
                // Elongated in one direction
                basePosition.x *= elongationFactor;
                break;
                
            case ContinentalShape::ELONGATED:
                // Very stretched continent (like Chile or Norway)
                basePosition.x *= elongationFactor;
                basePosition.y *= 0.8f;
                break;
                
            case ContinentalShape::CRESCENT:
                // Crescent or arc shape (like Italy or Japan)
                float crescentFactor = std::sin(angle * 1.5f + continentOrientation) * 0.3f + 1.0f;
                basePosition.x *= elongationFactor * crescentFactor;
                break;
        }
        
        // Rotate the entire continent based on orientation
        float cosOrient = std::cos(continentOrientation);
        float sinOrient = std::sin(continentOrientation);
        glm::vec2 rotatedPosition(
            basePosition.x * cosOrient - basePosition.y * sinOrient,
            basePosition.x * sinOrient + basePosition.y * cosOrient
        );
        
        particle.position = center + rotatedPosition;
        particle.position = ClampToWorldBounds(particle.position);
        
        // Initialize continental properties with height variation based on distance from center
        InitializeParticleProperties(particle, true);
        
        // Create realistic elevation profile: higher in center, lower at edges
        float distanceFromCenter = glm::length(particle.position - center) / fractalRadius;
        float elevationMultiplier = 1.2f - distanceFromCenter * 0.4f; // Higher base elevation
        
        // Add fractal elevation variation for mountain ranges and valleys
        float elevationNoise = std::sin(angle * 4.0f + continentIndex * 2.5f) * 0.2f + 
                              std::sin(angle * 10.0f + continentIndex * 1.8f) * 0.1f;
        
        particle.elevation = CONTINENT_ELEVATION + 
                           elevationMultiplier * (400.0f + elevationNoise * 300.0f); // Higher elevation range
        
        // Ensure solid continental elevation
        particle.elevation = std::max(particle.elevation, 200.0f); // Minimum 200m above sea level
        
        particles_.push_back(particle);
    }
    
    // IMPROVEMENT 6: Reduce scattered islands - periphery particles closer to main continent
    for (int i = 0; i < peripheryParticles; ++i) {
        TectonicParticle particle;
        
        float angle = angleDist(rng_);
        
        // IMPROVEMENT: Smaller radius variation to keep islands closer to main continent
        float peripheralRadius = radius * (0.85f + radiusDist(rng_) * 0.25f); // Reduced from 0.8-1.2x to 0.85-1.1x radius
        
        // More dramatic fractal variation for interesting island shapes
        float noise1 = std::sin(angle * 3.0f + continentIndex * 2.0f) * 0.4f;
        float noise2 = std::sin(angle * 8.0f + continentIndex * 3.14f) * 0.2f;
        float noise3 = std::sin(angle * 20.0f + continentIndex * 1.57f) * 0.1f;
        
        float coastlineVariation = noise1 + noise2 + noise3;
        peripheralRadius *= (1.0f + coastlineVariation);
        
        // IMPROVEMENT: Keep peripheral particles closer to core continent
        float r = (0.75f + radiusDist(rng_) * 0.2f) * peripheralRadius; // Changed from 0.7-1.0x to 0.75-0.95x
        
        particle.position = center + glm::vec2(std::cos(angle) * r, std::sin(angle) * r);
        particle.position = ClampToWorldBounds(particle.position);
        
        InitializeParticleProperties(particle, true);
        
        // IMPROVEMENT: Higher elevation for peripheral particles to reduce tiny islands
        particle.elevation = CONTINENT_ELEVATION * 0.7f + radiusDist(rng_) * 250.0f; // Increased from 0.6f and 200.0f
        particle.elevation = std::max(particle.elevation, 80.0f); // Increased minimum from 50m to 80m above sea level
        
        particles_.push_back(particle);
    }
}

void ParticleSimulationEngine::CreateOceanicParticles() {
    // Fill oceanic areas with particles to ensure complete coverage
    float particleSpacing = PARTICLE_INTERACTION_RADIUS * 0.8f; // Slight overlap for good coverage
    int gridSize = static_cast<int>(worldSizeM_ / particleSpacing);
    
    std::uniform_real_distribution<float> elevationDist(OCEAN_ELEVATION - 500.0f, OCEAN_ELEVATION + 200.0f);
    
    for (int x = 0; x < gridSize; ++x) {
        for (int z = 0; z < gridSize; ++z) {
            glm::vec2 position(x * particleSpacing + particleSpacing * 0.5f,
                              z * particleSpacing + particleSpacing * 0.5f);
            
            // Check if this position is far enough from continental particles
            bool isOceanic = true;
            auto nearbyParticles = FindNearbyParticles(position.x, position.y, PARTICLE_INTERACTION_RADIUS);
            
            for (size_t idx : nearbyParticles) {
                if (particles_[idx].elevation > 0.0f) { // Continental particle
                    isOceanic = false;
                    break;
                }
            }
            
            if (isOceanic) {
                TectonicParticle particle;
                particle.position = position;
                InitializeParticleProperties(particle, false);
                particle.elevation = elevationDist(rng_);
                particles_.push_back(particle);
            }
        }
    }
}

void ParticleSimulationEngine::InitializeParticleProperties(TectonicParticle& particle, bool isContinental) {
    std::uniform_real_distribution<float> velocityDist(-CONTINENTAL_DRIFT_SPEED, CONTINENTAL_DRIFT_SPEED);
    std::uniform_real_distribution<float> ageDist(0.0f, 4.5e9f); // 0-4.5 billion years
    
    particle.velocity = glm::vec2(velocityDist(rng_), velocityDist(rng_));
    particle.stress = 0.0f;
    particle.age = ageDist(rng_);
    particle.temperature = 273.15f + 15.0f; // ~15°C average
    particle.influenceRadius = PARTICLE_INTERACTION_RADIUS;
    particle.mass = 1.0f;
    particle.isActive = true;
    
    if (isContinental) {
        particle.crustalThickness = 35000.0f + (ageDist(rng_) / 4.5e9f) * 15000.0f; // 35-50km
        particle.density = 2700.0f; // Continental crust density
        particle.rockType = (ageDist(rng_) > 2.0e9f) ? RockType::IGNEOUS_GRANITE : RockType::SEDIMENTARY_SANDSTONE;
    } else {
        particle.crustalThickness = 5000.0f + (ageDist(rng_) / 4.5e9f) * 3000.0f; // 5-8km
        particle.density = 2900.0f; // Oceanic crust density
        particle.rockType = RockType::IGNEOUS_BASALT; // Oceanic crust is primarily basaltic
    }
}

void ParticleSimulationEngine::UpdateParticlePhysics(float timeStepYears) {
    // Only log first 5 calls to reduce spam (was 10)
    static int logCount = 0;
    if (logCount < 5) {
        std::cout << "[ParticleSimulationEngine] UpdateParticlePhysics called with timeStep: " 
                  << timeStepYears << " years, " << particles_.size() << " particles" << std::endl;
        logCount++;
    } else if (logCount == 5) {
        std::cout << "[ParticleSimulationEngine] Further physics updates will not be logged (spam reduction)" << std::endl;
        logCount++; // Prevent this message from repeating
    }
    
    if (particles_.empty()) {
        return;
    }
    
    // Clear and rebuild spatial grid for this timestep
    spatialGrid_->Clear();
    for (size_t i = 0; i < particles_.size(); ++i) {
        if (particles_[i].isActive) {
            spatialGrid_->Insert(i, particles_[i].position);
        }
    }
    
    // NEW: Apply tectonic forces BEFORE movement
    ApplyTectonicForces(timeStepYears);
    
    // Update particle movement and interactions
    for (auto& particle : particles_) {
        if (!particle.isActive) continue;
        
        UpdateParticleMovement(particle, timeStepYears);
        UpdateParticleStress(particle, timeStepYears);
    }
    
    // Apply tectonic forces (convection, collision, separation)
    ApplyTectonicForces(timeStepYears);
    
    // Process particle interactions and tectonic processes
    ProcessParticleInteractions(timeStepYears);
    ProcessTectonicBoundaries(timeStepYears);
    
    // Cleanup particles that moved outside bounds
    CleanupParticles();
}

void ParticleSimulationEngine::UpdateParticleMovement(TectonicParticle& particle, float timeStepYears) {
    // Simple continental drift simulation
    glm::vec2 newPosition = particle.position + particle.velocity * timeStepYears;
    
    // Apply world boundary constraints
    if (IsInWorldBounds(newPosition)) {
        particle.position = newPosition;
    } else {
        // Reflect velocity at world boundaries (simplified boundary handling)
        if (newPosition.x < 0 || newPosition.x > worldSizeM_) {
            particle.velocity.x *= -0.5f; // Reduce and reverse velocity
        }
        if (newPosition.y < 0 || newPosition.y > worldSizeM_) {
            particle.velocity.y *= -0.5f;
        }
        particle.position = ClampToWorldBounds(newPosition);
    }
}

void ParticleSimulationEngine::UpdateParticleStress(TectonicParticle& particle, float timeStepYears) {
    // Accumulate stress based on local particle density and movement
    auto nearbyParticles = FindNearbyParticles(particle.position.x, particle.position.y, 
                                              particle.influenceRadius);
    
    float localStress = 0.0f;
    for (size_t idx : nearbyParticles) {
        if (idx >= particles_.size() || !particles_[idx].isActive) continue;
        
        const auto& other = particles_[idx];
        float distance = CalculateDistance(particle.position, other.position);
        
        if (distance > 0.1f && distance < particle.influenceRadius) {
            // Calculate stress based on relative motion and proximity
            glm::vec2 relativeVelocity = particle.velocity - other.velocity;
            float velocityMagnitude = glm::length(relativeVelocity);
            float stressContribution = velocityMagnitude / (distance * 0.001f); // Scale factor
            localStress += stressContribution;
        }
    }
    
    // Update stress with decay
    particle.stress = particle.stress * 0.95f + localStress * timeStepYears * 0.05f;
}

void ParticleSimulationEngine::ProcessParticleInteractions(float timeStepYears) {
    // Process collision and interaction forces between nearby particles
    for (size_t i = 0; i < particles_.size(); ++i) {
        if (!particles_[i].isActive) continue;
        
        auto nearbyParticles = FindNearbyParticles(particles_[i].position.x, particles_[i].position.y,
                                                  particles_[i].influenceRadius);
        
        for (size_t j : nearbyParticles) {
            if (j <= i || !particles_[j].isActive) continue; // Avoid double processing
            
            ProcessParticleCollisions(particles_[i], particles_[j]);
        }
    }
}

void ParticleSimulationEngine::ProcessParticleCollisions(TectonicParticle& particle1, TectonicParticle& particle2) {
    float distance = CalculateDistance(particle1.position, particle2.position);
    float minDistance = (particle1.influenceRadius + particle2.influenceRadius) * 0.5f;
    
    if (distance < minDistance && distance > 0.1f) {
        // Calculate collision response
        glm::vec2 direction = (particle2.position - particle1.position) / distance;
        float overlap = minDistance - distance;
        float repulsionForce = overlap * 0.1f; // Adjust strength as needed
        
        // Apply repulsion forces
        glm::vec2 force = direction * repulsionForce;
        particle1.velocity -= force * 0.5f;
        particle2.velocity += force * 0.5f;
        
        // Exchange some properties (simplified geological mixing)
        float stressExchange = (particle1.stress - particle2.stress) * 0.01f;
        particle1.stress -= stressExchange;
        particle2.stress += stressExchange;
    }
}

ParticleSimulationEngine::GeologicalData ParticleSimulationEngine::SampleAt(float worldX, float worldZ) const {
    static int debugCallCount = 0;
    debugCallCount++;
    
    auto nearbyParticles = FindNearbyParticles(worldX, worldZ, PARTICLE_INTERACTION_RADIUS * 2.0f);
    
    if (debugCallCount <= 5) {
        std::cout << "[ParticleSimulationEngine] DEBUG " << debugCallCount 
                  << " - SampleAt(" << worldX << "," << worldZ << ")" << std::endl;
        std::cout << "  Found " << nearbyParticles.size() << " nearby particles within radius " 
                  << (PARTICLE_INTERACTION_RADIUS * 2.0f) << "m" << std::endl;
    }
    
    if (nearbyParticles.empty()) {
        // Return default oceanic values if no particles nearby
        GeologicalData data;
        data.elevation = OCEAN_ELEVATION;
        data.rockType = RockType::IGNEOUS_BASALT;
        data.crustalThickness = 5000.0f;
        
        if (debugCallCount <= 5) {
            std::cout << "  No particles found - returning OCEAN_ELEVATION: " << OCEAN_ELEVATION << "m" << std::endl;
        }
        
        return data;
    }
    
    GeologicalData result;
    result.elevation = InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.elevation; });
    result.stress = InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.stress; });
    result.crustalThickness = InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.crustalThickness; });
    result.geologicalAge = InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.age; });
    result.temperature = InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.temperature; });
    
    result.rockType = InterpolateRockType(worldX, worldZ, nearbyParticles);
    
    if (debugCallCount <= 5) {
        std::cout << "  Interpolated elevation: " << result.elevation << "m" << std::endl;
        std::cout << "  Interpolated stress: " << result.stress << std::endl;
        std::cout << "  Interpolated crustalThickness: " << result.crustalThickness << "m" << std::endl;
    }
    
    // Calculate average velocity
    float vx = InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.velocity.x; });
    float vz = InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.velocity.y; });
    result.velocity = glm::vec2(vx, vz);
    
    return result;
}

float ParticleSimulationEngine::SampleElevationAt(float worldX, float worldZ) const {
    auto nearbyParticles = FindNearbyParticles(worldX, worldZ, PARTICLE_INTERACTION_RADIUS * 2.0f);
    
    if (nearbyParticles.empty()) {
        return OCEAN_ELEVATION; // Default to ocean depth
    }
    
    return InterpolateFloat(worldX, worldZ, nearbyParticles,
        [](const TectonicParticle& p) { return p.elevation; });
}

std::vector<size_t> ParticleSimulationEngine::FindNearbyParticles(float worldX, float worldZ, float maxRadius) const {
    return spatialGrid_->FindInRadius(glm::vec2(worldX, worldZ), maxRadius);
}

float ParticleSimulationEngine::InterpolateFloat(float worldX, float worldZ, 
                                               const std::vector<size_t>& particleIndices,
                                               std::function<float(const TectonicParticle&)> valueExtractor) const {
    if (particleIndices.empty()) {
        return 0.0f;
    }
    
    float totalWeight = 0.0f;
    float weightedSum = 0.0f;
    glm::vec2 queryPos(worldX, worldZ);
    
    for (size_t idx : particleIndices) {
        if (idx >= particles_.size() || !particles_[idx].isActive) continue;
        
        const auto& particle = particles_[idx];
        float distance = CalculateDistance(queryPos, particle.position);
        
        // Inverse distance weighting with smooth falloff
        float weight = 1.0f / (1.0f + distance * distance * 0.000001f); // Scale for meters
        
        totalWeight += weight;
        weightedSum += valueExtractor(particle) * weight;
    }
    
    return (totalWeight > 0.0f) ? (weightedSum / totalWeight) : 0.0f;
}

RockType ParticleSimulationEngine::InterpolateRockType(float worldX, float worldZ, 
                                                      const std::vector<size_t>& particleIndices) const {
    if (particleIndices.empty()) {
        return RockType::IGNEOUS_BASALT;
    }
    
    // Count occurrences of each rock type with distance weighting
    std::vector<float> typeWeights(static_cast<int>(RockType::COUNT), 0.0f);
    glm::vec2 queryPos(worldX, worldZ);
    
    for (size_t idx : particleIndices) {
        if (idx >= particles_.size() || !particles_[idx].isActive) continue;
        
        const auto& particle = particles_[idx];
        float distance = CalculateDistance(queryPos, particle.position);
        float weight = 1.0f / (1.0f + distance * distance * 0.000001f);
        
        int typeIndex = static_cast<int>(particle.rockType);
        if (typeIndex >= 0 && typeIndex < static_cast<int>(RockType::COUNT)) {
            typeWeights[typeIndex] += weight;
        }
    }
    
    // Return rock type with highest weight
    int maxIndex = 0;
    for (int i = 1; i < static_cast<int>(RockType::COUNT); ++i) {
        if (typeWeights[i] > typeWeights[maxIndex]) {
            maxIndex = i;
        }
    }
    
    return static_cast<RockType>(maxIndex);
}

std::vector<size_t> ParticleSimulationEngine::GetParticlesInRadius(float worldX, float worldZ, float radius) const {
    return spatialGrid_->FindInRadius(glm::vec2(worldX, worldZ), radius);
}

void ParticleSimulationEngine::AddParticle(float worldX, float worldZ, const TectonicParticle& particle) {
    TectonicParticle newParticle = particle;
    newParticle.position = glm::vec2(worldX, worldZ);
    
    particles_.push_back(newParticle);
    size_t index = particles_.size() - 1;
    spatialGrid_->Insert(index, newParticle.position);
}

void ParticleSimulationEngine::CleanupParticles() {
    // Remove particles that are no longer active or outside world bounds
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [this](const TectonicParticle& p) {
                return !p.isActive || !IsInWorldBounds(p.position);
            }),
        particles_.end());
}

size_t ParticleSimulationEngine::GetParticleCount() const {
    return std::count_if(particles_.begin(), particles_.end(),
        [](const TectonicParticle& p) { return p.isActive; });
}

void ParticleSimulationEngine::Reset() {
    particles_.clear();
    spatialGrid_->Clear();
    continentCount_ = 0;
}

float ParticleSimulationEngine::CalculateDistance(const glm::vec2& pos1, const glm::vec2& pos2) const {
    return glm::length(pos2 - pos1);
}

bool ParticleSimulationEngine::IsInWorldBounds(const glm::vec2& position) const {
    return position.x >= 0.0f && position.x <= worldSizeM_ &&
           position.y >= 0.0f && position.y <= worldSizeM_;
}

glm::vec2 ParticleSimulationEngine::ClampToWorldBounds(const glm::vec2& position) const {
    return glm::vec2(
        std::max(0.0f, std::min(worldSizeM_, position.x)),
        std::max(0.0f, std::min(worldSizeM_, position.y))
    );
}

// ==========================================
// TECTONIC FORCE SYSTEM IMPLEMENTATION
// ==========================================

void ParticleSimulationEngine::ApplyTectonicForces(float timeStepYears) {
    static int logCount = 0;
    if (logCount < 3) {
        std::cout << "[TectonicForces] Applying tectonic forces for " << particles_.size() << " particles" << std::endl;
        logCount++;
    }
    
    for (auto& particle : particles_) {
        if (!particle.isActive) continue;
        
        // Calculate mantle convection forces
        glm::vec2 convectionForce = CalculateConvectionForce(particle, timeStepYears);
        
        // Apply force to particle velocity (with realistic scaling)
        float forceScale = timeStepYears * 0.01f; // Scale force application
        particle.velocity += convectionForce * forceScale;
        
        // Limit velocity to realistic continental drift speeds
        float speed = glm::length(particle.velocity);
        if (speed > CONTINENTAL_DRIFT_SPEED * 2.0f) {
            particle.velocity = glm::normalize(particle.velocity) * CONTINENTAL_DRIFT_SPEED * 2.0f;
        }
    }
}

glm::vec2 ParticleSimulationEngine::CalculateConvectionForce(const TectonicParticle& particle, float timeStepYears) {
    // Create convection cells using noise-based patterns
    // This simulates mantle convection driving plate motion
    
    float worldNormX = particle.position.x / worldSizeM_;
    float worldNormZ = particle.position.y / worldSizeM_;
    
    // Multiple convection scales for realistic mantle flow
    float largeCellNoise = std::sin(worldNormX * 2.0f * M_PI) * std::cos(worldNormZ * 2.0f * M_PI);
    float mediumCellNoise = std::sin(worldNormX * 6.0f * M_PI) * std::cos(worldNormZ * 4.0f * M_PI) * 0.5f;
    float smallCellNoise = std::sin(worldNormX * 12.0f * M_PI) * std::cos(worldNormZ * 8.0f * M_PI) * 0.25f;
    
    // Combine different scales
    float convectionStrength = largeCellNoise + mediumCellNoise + smallCellNoise;
    
    // Calculate force direction based on convection gradient
    float gradientX = std::cos(worldNormX * 2.0f * M_PI) * std::cos(worldNormZ * 2.0f * M_PI) * 2.0f * M_PI / worldSizeM_;
    float gradientZ = std::sin(worldNormX * 2.0f * M_PI) * (-std::sin(worldNormZ * 2.0f * M_PI)) * 2.0f * M_PI / worldSizeM_;
    
    // Scale force by convection strength and realistic mantle velocity
    float mantleVelocityScale = 0.1f; // 10cm/year mantle flow
    glm::vec2 force(gradientX * convectionStrength * mantleVelocityScale,
                    gradientZ * convectionStrength * mantleVelocityScale);
    
    return force;
}

void ParticleSimulationEngine::ProcessTectonicBoundaries(float timeStepYears) {
    static int logCount = 0;
    if (logCount < 3) {
        std::cout << "[TectonicBoundaries] Processing plate boundaries" << std::endl;
        logCount++;
    }
    
    // Check all particle pairs for boundary interactions
    for (size_t i = 0; i < particles_.size(); ++i) {
        if (!particles_[i].isActive) continue;
        
        auto nearbyParticles = FindNearbyParticles(particles_[i].position.x, particles_[i].position.y,
                                                  particles_[i].influenceRadius * 1.5f);
        
        for (size_t j : nearbyParticles) {
            if (j <= i || !particles_[j].isActive) continue;
            
            ProcessPlateBoundary(particles_[i], particles_[j], timeStepYears);
        }
    }
}

void ParticleSimulationEngine::ProcessPlateBoundary(TectonicParticle& particle1, TectonicParticle& particle2, float timeStepYears) {
    float distance = CalculateDistance(particle1.position, particle2.position);
    float interactionRadius = (particle1.influenceRadius + particle2.influenceRadius) * 0.5f;
    
    if (distance > interactionRadius) return;
    
    // Calculate relative motion
    glm::vec2 relativeVelocity = particle1.velocity - particle2.velocity;
    glm::vec2 direction = (particle2.position - particle1.position) / distance;
    
    // Project relative velocity onto direction vector
    float relativeSpeed = glm::dot(relativeVelocity, direction);
    
    // Determine boundary type based on relative motion
    if (relativeSpeed < -0.01f) {
        // Convergent boundary - particles moving toward each other
        float compressionForce = -relativeSpeed * distance / interactionRadius;
        ApplyMountainBuilding(particle1, particle2, compressionForce);
        
    } else if (relativeSpeed > 0.01f) {
        // Divergent boundary - particles moving apart
        float extensionForce = relativeSpeed * distance / interactionRadius;
        ApplyRiftFormation(particle1, particle2, extensionForce);
        
    } else {
        // Transform boundary - particles sliding past each other
        // Accumulate shear stress
        float shearStress = glm::length(relativeVelocity) * 0.1f;
        particle1.stress += shearStress * timeStepYears;
        particle2.stress += shearStress * timeStepYears;
    }
}

void ParticleSimulationEngine::ApplyMountainBuilding(TectonicParticle& particle1, TectonicParticle& particle2, float compressionForce) {
    // Only apply mountain building to continental particles
    if (particle1.elevation < 0.0f && particle2.elevation < 0.0f) return; // Both oceanic
    
    static int mountainCount = 0;
    if (mountainCount < 5) {
        std::cout << "[MountainBuilding] Creating mountains with compression force: " << compressionForce << std::endl;
        mountainCount++;
    }
    
    // Calculate elevation increase based on compression
    float elevationIncrease = compressionForce * 50.0f; // 50m per unit compression force
    elevationIncrease = std::min(elevationIncrease, 20.0f); // Max 20m per timestep
    
    // Apply elevation gain to both particles
    particle1.elevation += elevationIncrease;
    particle2.elevation += elevationIncrease;
    
    // Clamp to realistic bounds
    particle1.elevation = std::min(particle1.elevation, 8848.0f); // Mt. Everest height
    particle2.elevation = std::min(particle2.elevation, 8848.0f);
    
    // Increase crustal thickness due to thickening
    float thickeningRate = compressionForce * 1000.0f; // 1km per unit compression
    particle1.crustalThickness += thickeningRate;
    particle2.crustalThickness += thickeningRate;
    
    // Clamp crustal thickness
    particle1.crustalThickness = std::min(particle1.crustalThickness, 70000.0f); // Max 70km crust
    particle2.crustalThickness = std::min(particle2.crustalThickness, 70000.0f);
    
    // Increase stress due to compression
    particle1.stress += compressionForce * 10.0f;
    particle2.stress += compressionForce * 10.0f;
    
    // Change rock type to metamorphic due to high pressure
    if (compressionForce > 0.5f) {
        particle1.rockType = RockType::METAMORPHIC_MARBLE;
        particle2.rockType = RockType::METAMORPHIC_MARBLE;
    }
}

void ParticleSimulationEngine::ApplyRiftFormation(TectonicParticle& particle1, TectonicParticle& particle2, float extensionForce) {
    static int riftCount = 0;
    if (riftCount < 5) {
        std::cout << "[RiftFormation] Creating rift with extension force: " << extensionForce << std::endl;
        riftCount++;
    }
    
    // Calculate elevation decrease based on extension
    float elevationDecrease = extensionForce * 30.0f; // 30m per unit extension force
    elevationDecrease = std::min(elevationDecrease, 15.0f); // Max 15m per timestep
    
    // Apply elevation loss to both particles (rift valley formation)
    particle1.elevation -= elevationDecrease;
    particle2.elevation -= elevationDecrease;
    
    // Prevent excessive depths
    particle1.elevation = std::max(particle1.elevation, -4000.0f); // Max 4km deep
    particle2.elevation = std::max(particle2.elevation, -4000.0f);
    
    // Thin the crust due to extension
    float thinningRate = extensionForce * 800.0f; // 800m per unit extension
    particle1.crustalThickness -= thinningRate;
    particle2.crustalThickness -= thinningRate;
    
    // Maintain minimum crustal thickness
    particle1.crustalThickness = std::max(particle1.crustalThickness, 5000.0f); // Min 5km crust
    particle2.crustalThickness = std::max(particle2.crustalThickness, 5000.0f);
    
    // Reduce stress due to extension (relief of compression)
    particle1.stress = std::max(0.0f, particle1.stress - extensionForce * 5.0f);
    particle2.stress = std::max(0.0f, particle2.stress - extensionForce * 5.0f);
    
    // If rift becomes deep enough, create new oceanic crust
    if (particle1.elevation < -1000.0f) {
        particle1.rockType = RockType::IGNEOUS_BASALT; // Oceanic basalt
        particle1.age = 0.0f; // New crust
    }
    if (particle2.elevation < -1000.0f) {
        particle2.rockType = RockType::IGNEOUS_BASALT;
        particle2.age = 0.0f;
    }
}

} // namespace World
} // namespace VoxelCastle
