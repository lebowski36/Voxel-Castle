#pragma once

#include "world/GeologicalTypes.h"
#include "world/SpatialHashGrid.h"
#include <vector>
#include <memory>
#include <random>
#include <glm/glm.hpp>

namespace VoxelCastle {
namespace World {

/**
 * @brief Particle-based geological simulation engine
 * 
 * This class implements continuous, resolution-independent geological simulation
 * using particles instead of grid-based approaches. Eliminates grid artifacts
 * and provides natural geological behavior through particle interactions.
 */
class ParticleSimulationEngine {
public:
    /**
     * @brief Tectonic particle representing crustal material
     */
    struct TectonicParticle {
        glm::vec2 position;         // World coordinates (not grid indices)
        glm::vec2 velocity;         // Continental drift velocity (m/year)
        float stress;               // Accumulated tectonic stress (Pa)
        float crustalThickness;     // Local crustal thickness (m)
        float elevation;            // Current elevation (m)
        float age;                  // Geological age (years)
        RockType rockType;          // Geological composition
        float temperature;          // Local temperature (affects rock behavior)
        float density;              // Crustal density (kg/m³)
        
        // Particle interaction properties
        float influenceRadius;      // Radius of influence for interactions (m)
        float mass;                 // Effective mass for physics calculations
        bool isActive;              // Whether particle participates in simulation
        
        TectonicParticle() : 
            position(0.0f), velocity(0.0f), stress(0.0f), crustalThickness(35000.0f),
            elevation(0.0f), age(0.0f), rockType(RockType::SEDIMENTARY_SANDSTONE),
            temperature(273.15f), density(2800.0f), influenceRadius(5000.0f),
            mass(1.0f), isActive(true) {}
    };
    
    /**
     * @brief Geological data sampled from particle simulation
     */
    struct GeologicalData {
        float elevation;            // Terrain elevation (m)
        float stress;               // Tectonic stress level
        float crustalThickness;     // Crustal thickness at this point
        RockType rockType;          // Dominant rock type
        float geologicalAge;        // Age of geological formation
        float temperature;          // Local temperature
        glm::vec2 velocity;         // Local crustal movement
        
        GeologicalData() : 
            elevation(0.0f), stress(0.0f), crustalThickness(35000.0f),
            rockType(RockType::SEDIMENTARY_SANDSTONE), geologicalAge(0.0f),
            temperature(273.15f), velocity(0.0f) {}
    };

public:
    /**
     * @brief Constructor
     * @param worldSizeKm World size in kilometers
     * @param seed Random seed for reproducible results
     */
    ParticleSimulationEngine(float worldSizeKm, uint64_t seed);
    
    /**
     * @brief Destructor
     */
    ~ParticleSimulationEngine();
    
    /**
     * @brief Initialize particle system with continental configuration
     * @param continentCount Number of continental masses to create
     * @param oceanRatio Fraction of world that should be ocean (0.0-1.0)
     */
    void InitializeParticles(int continentCount, float oceanRatio = 0.71f);
    
    /**
     * @brief Update particle physics for one simulation step
     * @param timeStepYears Time step in years
     */
    void UpdateParticlePhysics(float timeStepYears);
    
    /**
     * @brief Sample geological data at any world coordinate
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @return Geological data interpolated from nearby particles
     */
    GeologicalData SampleAt(float worldX, float worldZ) const;
    
    /**
     * @brief Sample elevation only (optimized for frequent queries)
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @return Terrain elevation interpolated from particles
     */
    float SampleElevationAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get all particles within radius of a point
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param radius Search radius (meters)
     * @return Vector of particle indices within radius
     */
    std::vector<size_t> GetParticlesInRadius(float worldX, float worldZ, float radius) const;
    
    /**
     * @brief Add tectonic particle at specific location
     * @param worldX X coordinate in world space (meters)
     * @param worldZ Z coordinate in world space (meters)
     * @param particle Particle data to add
     */
    void AddParticle(float worldX, float worldZ, const TectonicParticle& particle);
    
    /**
     * @brief Remove particles that have moved outside world bounds
     */
    void CleanupParticles();
    
    /**
     * @brief Get total number of active particles
     */
    size_t GetParticleCount() const;
    
    /**
     * @brief Get direct access to particle data (for debugging)
     */
    const std::vector<TectonicParticle>& GetParticles() const { return particles_; }
    
    /**
     * @brief Reset particle system to initial state
     */
    void Reset();

private:
    // World configuration
    float worldSizeKm_;             // World size in kilometers
    float worldSizeM_;              // World size in meters
    uint64_t seed_;                 // Random seed
    std::mt19937 rng_;              // Random number generator
    
    // Particle storage and management
    std::vector<TectonicParticle> particles_;
    std::unique_ptr<SpatialHashGrid> spatialGrid_;
    
    // Continental configuration
    int continentCount_;
    float oceanRatio_;
    
    // Physics parameters
    static constexpr float CONTINENTAL_DRIFT_SPEED = 0.05f;  // m/year (realistic plate speeds)
    static constexpr float PARTICLE_INTERACTION_RADIUS = 5000.0f;  // 5km default influence
    static constexpr float MIN_CRUSTAL_THICKNESS = 5000.0f;   // 5km minimum crust
    static constexpr float MAX_CRUSTAL_THICKNESS = 70000.0f;  // 70km maximum crust
    static constexpr float OCEAN_ELEVATION = -100.0f;        // Reasonable ocean depth
    static constexpr float CONTINENTAL_ELEVATION = 200.0f;   // Continental baseline
    static constexpr float CONTINENT_ELEVATION = 500.0f;     // Continental baseline
    
    // Particle initialization helpers
    void CreateContinentalParticles(int continentIndex, glm::vec2 center, float radius);
    void CreateOceanicParticles();
    void InitializeParticleProperties(TectonicParticle& particle, bool isContinental);
    
    // Physics simulation helpers
    void UpdateParticleMovement(TectonicParticle& particle, float timeStepYears);
    void ProcessParticleInteractions(float timeStepYears);
    void UpdateParticleStress(TectonicParticle& particle, float timeStepYears);
    void ProcessParticleCollisions(TectonicParticle& particle1, TectonicParticle& particle2);
    
    // Sampling and interpolation helpers
    std::vector<size_t> FindNearbyParticles(float worldX, float worldZ, float maxRadius = 10000.0f) const;
    float InterpolateFloat(float worldX, float worldZ, const std::vector<size_t>& particleIndices,
                          std::function<float(const TectonicParticle&)> valueExtractor) const;
    RockType InterpolateRockType(float worldX, float worldZ, const std::vector<size_t>& particleIndices) const;
    
    // Utility functions
    float CalculateDistance(const glm::vec2& pos1, const glm::vec2& pos2) const;
    bool IsInWorldBounds(const glm::vec2& position) const;
    glm::vec2 ClampToWorldBounds(const glm::vec2& position) const;
};

} // namespace World
} // namespace VoxelCastle
