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
    
    // Calculate continental parameters
    float continentalArea = worldSizeM_ * worldSizeM_ * (1.0f - oceanRatio);
    float avgContinentArea = continentalArea / continentCount;
    float avgContinentRadius = std::sqrt(avgContinentArea / M_PI) * 0.8f; // Slightly smaller for spacing
    
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
                if (distance < avgContinentRadius * 1.5f) {
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
    // Use fractal distribution for realistic continental shapes
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radiusDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> densityDist(0.7f, 1.0f);
    
    // Calculate particle density based on continent size
    float continentArea = M_PI * radius * radius;
    int particleCount = static_cast<int>(continentArea / (PARTICLE_INTERACTION_RADIUS * PARTICLE_INTERACTION_RADIUS * 0.5f));
    particleCount = std::max(50, std::min(500, particleCount)); // Reasonable bounds
    
    for (int i = 0; i < particleCount; ++i) {
        TectonicParticle particle;
        
        // Generate position with fractal distribution (creates realistic continental shapes)
        float angle = angleDist(rng_);
        float r = std::sqrt(radiusDist(rng_)) * radius * densityDist(rng_);
        
        particle.position = center + glm::vec2(std::cos(angle) * r, std::sin(angle) * r);
        
        // Ensure particle is within world bounds
        particle.position = ClampToWorldBounds(particle.position);
        
        // Initialize continental properties
        InitializeParticleProperties(particle, true);
        particle.elevation = CONTINENT_ELEVATION + radiusDist(rng_) * 300.0f; // 500-800m elevation
        
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
    std::cout << "[ParticleSimulationEngine] UpdateParticlePhysics called with timeStep: " 
              << timeStepYears << " years, " << particles_.size() << " particles" << std::endl;
    
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
    
    // Update particle movement and interactions
    for (auto& particle : particles_) {
        if (!particle.isActive) continue;
        
        UpdateParticleMovement(particle, timeStepYears);
        UpdateParticleStress(particle, timeStepYears);
    }
    
    // Process particle interactions
    ProcessParticleInteractions(timeStepYears);
    
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

} // namespace World
} // namespace VoxelCastle
