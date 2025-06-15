#include "world/FractalContinentGenerator.h"
#include "util/noise.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include <stack>
#include <map>
#include <limits>

namespace VoxelCastle {
namespace World {

FractalContinentGenerator::FractalContinentGenerator(uint64_t seed) 
    : seed_(seed) {
    std::cout << "[FractalContinentGenerator] Initialized with seed: " << seed << std::endl;
}

void FractalContinentGenerator::GenerateContinentalFoundation(
    ContinuousField<float>& elevationField,
    ContinuousField<RockType>& rockTypes,
    ContinuousField<float>& mantleStress,
    float worldSizeKm) {
    
    std::cout << "[FractalContinentGenerator] Generating continental foundation for " 
              << worldSizeKm << "km world" << std::endl;
    
    // Step 1: Generate continental plates using Voronoi-based distribution
    generateContinentalPlates(worldSizeKm);
    
    // Step 2: Generate ocean basins between continents
    generateOceanBasins(worldSizeKm);
    
    // Step 3: Generate fractal coastlines with realistic elevation
    generateCoastlines(elevationField, worldSizeKm);
    
    // Step 4: Generate L-System river network templates
    generateRiverTemplates(worldSizeKm);
    
    // Step 5: Generate mountain ridge templates and apply elevation
    generateMountainRidges(elevationField, worldSizeKm);
    
    // Step 6: Assign rock types based on continental vs oceanic crust
    assignRockTypes(rockTypes, worldSizeKm);
    
    // Step 7: Initialize mantle stress patterns based on continental layout
    initializeMantleStress(mantleStress, worldSizeKm);
    
    std::cout << "[FractalContinentGenerator] Generated " << continentalPlates_.size() 
              << " continental plates, " << oceanBasins_.size() << " ocean basins, "
              << riverTemplates_.size() << " river templates, and " 
              << mountainRidges_.size() << " mountain ridges" << std::endl;
}

void FractalContinentGenerator::generateContinentalPlates(float worldSizeKm) {
    std::mt19937 rng(seed_);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Determine number of continents based on world size (3-7 for 256km world)
    int numContinents = 3 + (seed_ % 5); // 3-7 continents
    float worldSizeMeters = worldSizeKm * 1000.0f;
    
    std::cout << "[FractalContinentGenerator] Generating " << numContinents << " continental plates" << std::endl;
    
    // DEBUGGING: Clear existing plates
    continentalPlates_.clear();
    std::cout << "[FractalContinentGenerator] DEBUG: Cleared existing plates, size now: " << continentalPlates_.size() << std::endl;
    
    // Generate continental seeds using Mitchell's best-candidate algorithm
    auto continentalSeeds = generateContinentalSeeds(worldSizeKm, numContinents);
    
    std::cout << "[FractalContinentGenerator] DEBUG: Generated " << continentalSeeds.size() << " continental seeds" << std::endl;
    
    // Create continental plates with organic size variation
    for (int i = 0; i < numContinents; ++i) {
        ContinentalPlate plate;
        plate.center = continentalSeeds[i];
        plate.plateId = i;
        
        // Use fractal noise to determine continent size (more organic than pure random)
        float sizeNoise = fractionalBrownianMotion(plate.center.x * 0.0001f, plate.center.y * 0.0001f, 3, 0.7f);
        float normalizedSizeNoise = (sizeNoise + 1.0f) * 0.5f; // Normalize to 0-1
        
        // Create diverse continent sizes: some large landmasses, some smaller islands
        float sizeVariation;
        if (normalizedSizeNoise > 0.7f) {
            // Large continents (20-45% of world size)
            sizeVariation = 0.20f + (0.25f * dist(rng));
        } else if (normalizedSizeNoise > 0.4f) {
            // Medium continents (10-25% of world size)
            sizeVariation = 0.10f + (0.15f * dist(rng));
        } else {
            // Small continents/large islands (3-12% of world size)
            sizeVariation = 0.03f + (0.09f * dist(rng));
        }
        
        plate.radius = worldSizeMeters * sizeVariation * 0.5f; // Convert to radius
        
        // Base elevation varies with continent size (larger = higher average elevation)
        float elevationBase = 50.0f + (sizeVariation * 800.0f); // Larger continents are higher
        float elevationVariation = 150.0f * dist(rng);
        plate.elevation = elevationBase + elevationVariation;
        
        // Assign dominant rock type based on location and size
        float rockChoice = dist(rng);
        float locationFactor = fractionalBrownianMotion(plate.center.x * 0.0002f, plate.center.y * 0.0002f, 2, 0.5f);
        
        if (locationFactor > 0.3f && sizeVariation > 0.15f) {
            // Large continents tend to have more granite (ancient cores)
            plate.dominantRockType = RockType::IGNEOUS_GRANITE;
        } else if (rockChoice < 0.4f) {
            plate.dominantRockType = RockType::SEDIMENTARY_SANDSTONE;
        } else if (rockChoice < 0.7f) {
            plate.dominantRockType = RockType::METAMORPHIC_QUARTZITE;
        } else {
            plate.dominantRockType = RockType::IGNEOUS_GRANITE;
        }
        
        // Tectonic activity varies by plate size and location
        float baseTectonicActivity = 0.2f + (0.6f * dist(rng));
        float sizeFactor = std::min(1.0f, sizeVariation / 0.3f); // Larger plates more active
        plate.tectonicActivity = baseTectonicActivity * (0.7f + 0.6f * sizeFactor);
        
        continentalPlates_.push_back(plate);
        
        std::cout << "[FractalContinentGenerator] DEBUG: Created plate " << i << " at (" 
                  << plate.center.x/1000.0f << "km, " << plate.center.y/1000.0f 
                  << "km), radius=" << plate.radius/1000.0f << "km, elevation=" 
                  << plate.elevation << "m. Total plates: " << continentalPlates_.size() << std::endl;
    }
    
    std::cout << "[FractalContinentGenerator] DEBUG: Final continental plates count: " << continentalPlates_.size() << std::endl;
}

std::vector<glm::vec2> FractalContinentGenerator::generateContinentalSeeds(float worldSizeKm, int numContinents) {
    std::mt19937 rng(seed_);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    std::vector<glm::vec2> seeds;
    float worldSizeMeters = worldSizeKm * 1000.0f;
    
    // Use fractal clustering instead of uniform distribution
    // Generate primary clusters first, then place continents within clusters
    int numClusters = std::max(1, numContinents / 2); // 1-3 clusters for 3-6 continents
    std::vector<glm::vec2> clusterCenters;
    
    // Generate cluster centers with some randomness but avoid edges
    for (int i = 0; i < numClusters; ++i) {
        glm::vec2 clusterCenter;
        
        // Bias toward interesting areas using noise
        float searchAttempts = 50;
        float bestNoise = -1.0f;
        glm::vec2 bestPos;
        
        for (int attempt = 0; attempt < searchAttempts; ++attempt) {
            glm::vec2 candidate;
            // Keep clusters away from world edges
            candidate.x = (0.2f + 0.6f * dist(rng)) * worldSizeMeters;
            candidate.y = (0.2f + 0.6f * dist(rng)) * worldSizeMeters;
            
            // Use noise to find interesting cluster locations
            float noiseValue = fractionalBrownianMotion(candidate.x * 0.0001f, candidate.y * 0.0001f, 4, 0.6f);
            if (noiseValue > bestNoise) {
                bestNoise = noiseValue;
                bestPos = candidate;
            }
        }
        
        clusterCenters.push_back(bestPos);
    }
    
    // Place continents within clusters with organic distribution
    int continentsPlaced = 0;
    for (int cluster = 0; cluster < numClusters && continentsPlaced < numContinents; ++cluster) {
        int continentsInCluster = (numContinents - continentsPlaced) / (numClusters - cluster);
        if (continentsInCluster < 1) continentsInCluster = 1;
        
        glm::vec2 clusterCenter = clusterCenters[cluster];
        float clusterRadius = worldSizeMeters * 0.3f; // Clusters can span 30% of world
        
        for (int i = 0; i < continentsInCluster && continentsPlaced < numContinents; ++i) {
            glm::vec2 continentPos;
            bool positionFound = false;
            int maxAttempts = 200;
            
            for (int attempt = 0; attempt < maxAttempts; ++attempt) {
                // Generate position within cluster using fractal distribution
                float angle = dist(rng) * 2.0f * M_PI;
                float distance = std::pow(dist(rng), 0.7f) * clusterRadius; // Bias toward cluster center
                
                // Add fractal offset for organic positioning
                float fractalOffsetX = fractionalBrownianMotion(angle * 10.0f, distance * 0.001f, 3, 0.5f) * clusterRadius * 0.3f;
                float fractalOffsetY = fractionalBrownianMotion((angle + M_PI) * 10.0f, distance * 0.001f, 3, 0.5f) * clusterRadius * 0.3f;
                
                continentPos = clusterCenter + glm::vec2(
                    std::cos(angle) * distance + fractalOffsetX,
                    std::sin(angle) * distance + fractalOffsetY
                );
                
                // Keep within world bounds
                continentPos.x = std::max(worldSizeMeters * 0.05f, std::min(worldSizeMeters * 0.95f, continentPos.x));
                continentPos.y = std::max(worldSizeMeters * 0.05f, std::min(worldSizeMeters * 0.95f, continentPos.y));
                
                // Check minimum distance to existing continents (but allow closer than before for organic clustering)
                float minDistance = worldSizeMeters * 0.08f; // Reduced from 0.15f
                bool tooClose = false;
                
                for (const auto& existingSeed : seeds) {
                    if (glm::length(continentPos - existingSeed) < minDistance) {
                        tooClose = true;
                        break;
                    }
                }
                
                if (!tooClose) {
                    positionFound = true;
                    break;
                }
            }
            
            if (positionFound) {
                seeds.push_back(continentPos);
                continentsPlaced++;
            } else {
                // Fall back to any valid position if organic placement fails
                glm::vec2 fallbackPos;
                fallbackPos.x = (0.1f + 0.8f * dist(rng)) * worldSizeMeters;
                fallbackPos.y = (0.1f + 0.8f * dist(rng)) * worldSizeMeters;
                seeds.push_back(fallbackPos);
                continentsPlaced++;
            }
        }
    }
    
    return seeds;
}

void FractalContinentGenerator::generateOceanBasins(float worldSizeKm) {
    // Ocean basins are areas not covered by continental plates
    // For now, we'll create a single large ocean basin covering everything else
    OceanBasin mainOcean;
    
    // Ocean depth varies from -1000m to -2000m
    std::mt19937 rng(seed_ + 1000); // Offset seed for ocean generation
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    mainOcean.depth = -1000.0f - (1000.0f * dist(rng)); // -1000m to -2000m
    mainOcean.tectonicActivity = 0.1f + (0.3f * dist(rng)); // Low tectonic activity
    
    // Create boundary as world edges (simplified for now)
    float worldSizeMeters = worldSizeKm * 1000.0f;
    mainOcean.boundary = {
        {0.0f, 0.0f},
        {worldSizeMeters, 0.0f},
        {worldSizeMeters, worldSizeMeters},
        {0.0f, worldSizeMeters}
    };
    
    oceanBasins_.push_back(mainOcean);
}

void FractalContinentGenerator::generateCoastlines(ContinuousField<float>& elevationField, float worldSizeKm) {
    int width = elevationField.getWidth();
    int height = elevationField.getHeight();
    float spacing = elevationField.getSampleSpacing();
    
    std::cout << "[FractalContinentGenerator] Generating organic coastlines using fractal domains..." << std::endl;
    
    // NOTE: DO NOT clear continentalPlates_ here - they were created by generateContinentalPlates()
    // and are needed for the final count reporting
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            glm::vec2 worldPos(x * spacing, z * spacing);
            
            // Generate completely organic continental structure using fractal domains
            float elevation = generateOrganicContinentalElevation(worldPos, worldSizeKm);
            
            elevationField.setSample(x, z, elevation);
        }
    }
    
    std::cout << "[FractalContinentGenerator] Generated organic continental structure" << std::endl;
}

void FractalContinentGenerator::generateRiverTemplates(float worldSizeKm) {
    std::mt19937 rng(seed_ + 3000); // Offset seed for river generation
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_int_distribution<int> riverCountDist(2, 4); // 2-4 rivers per continent
    
    std::cout << "[FractalContinentGenerator] Generating river network templates..." << std::endl;
    
    for (const auto& continent : continentalPlates_) {
        int numRivers = riverCountDist(rng);
        
        for (int i = 0; i < numRivers; ++i) {
            RiverTemplate river;
            river.continentId = continent.plateId;
            
            // River source: random point in inner continent (30-70% of radius)
            float sourceAngle = dist(rng) * 2.0f * M_PI;
            float sourceDistance = continent.radius * (0.3f + 0.4f * dist(rng));
            river.source = continent.center + glm::vec2(
                std::cos(sourceAngle) * sourceDistance,
                std::sin(sourceAngle) * sourceDistance
            );
            
            // River mouth: point on continent edge (coastline)
            float mouthAngle = dist(rng) * 2.0f * M_PI;
            river.mouth = continent.center + glm::vec2(
                std::cos(mouthAngle) * continent.radius,
                std::sin(mouthAngle) * continent.radius
            );
            
            // Generate main stem using L-System
            river.mainStem = generateLSystemRiver(river.source, river.mouth, worldSizeKm, 3);
            
            // Generate tributaries (simplified for now)
            int numTributaries = 1 + (rng() % 3); // 1-3 tributaries per river
            for (int t = 0; t < numTributaries; ++t) {
                if (river.mainStem.size() > 2) {
                    int branchIndex = 1 + (rng() % (river.mainStem.size() - 1));
                    glm::vec2 branchStart = river.source + glm::vec2(
                        (dist(rng) - 0.5f) * 1000.0f,
                        (dist(rng) - 0.5f) * 1000.0f
                    );
                    std::vector<glm::vec2> tributary = generateLSystemRiver(
                        branchStart, river.mainStem[branchIndex], worldSizeKm, 2
                    );
                    river.tributaries.push_back(tributary);
                }
            }
            
            riverTemplates_.push_back(river);
        }
    }
}

void FractalContinentGenerator::generateMountainRidges(ContinuousField<float>& elevationField, float worldSizeKm) {
    std::mt19937 rng(seed_ + 4000); // Offset seed for mountain generation
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    std::cout << "[FractalContinentGenerator] Generating mountain ridge templates..." << std::endl;
    
    int width = elevationField.getWidth();
    int height = elevationField.getHeight();
    float spacing = elevationField.getSampleSpacing();
    
    // Generate mountain ridges along continental collision zones
    for (size_t i = 0; i < continentalPlates_.size(); ++i) {
        for (size_t j = i + 1; j < continentalPlates_.size(); ++j) {
            const auto& plate1 = continentalPlates_[i];
            const auto& plate2 = continentalPlates_[j];
            
            float distance = glm::length(plate1.center - plate2.center);
            float combinedRadius = plate1.radius + plate2.radius;
            
            // If plates are close enough, create mountain ridge between them
            if (distance < combinedRadius * 1.2f) {
                MountainRidge ridge;
                ridge.continentId = plate1.plateId; // Assign to first plate
                ridge.baseElevation = std::max(plate1.elevation, plate2.elevation);
                ridge.maxElevation = ridge.baseElevation + 800.0f + (400.0f * dist(rng)); // +800-1200m
                ridge.isVolcanic = dist(rng) < 0.3f; // 30% chance of volcanic activity
                
                // Generate ridge line using L-System
                ridge.ridgeLine = generateLSystemMountainRidge(plate1.center, plate2.center, worldSizeKm, 2);
                
                mountainRidges_.push_back(ridge);
                
                // Apply elevation to the field
                for (const auto& point : ridge.ridgeLine) {
                    int gridX = static_cast<int>(point.x / spacing);
                    int gridZ = static_cast<int>(point.y / spacing);
                    
                    if (gridX >= 0 && gridX < width && gridZ >= 0 && gridZ < height) {
                        // Apply mountain elevation with falloff
                        float currentElevation = elevationField.getSample(gridX, gridZ);
                        float mountainElevation = ridge.baseElevation + (ridge.maxElevation - ridge.baseElevation) * dist(rng);
                        float finalElevation = std::max(currentElevation, mountainElevation);
                        elevationField.setSample(gridX, gridZ, finalElevation);
                    }
                }
            }
        }
    }
}

void FractalContinentGenerator::assignRockTypes(ContinuousField<RockType>& rockTypes, float worldSizeKm) {
    int width = rockTypes.getWidth();
    int height = rockTypes.getHeight();
    float spacing = rockTypes.getSampleSpacing();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            glm::vec2 worldPos(x * spacing, z * spacing);
            
            if (isInOcean(worldPos)) {
                // Ocean areas get oceanic crust (basalt)
                rockTypes.setSample(x, z, RockType::IGNEOUS_BASALT);
            } else {
                // Continental areas get continental crust
                ContinentalPlate* nearestPlate = findNearestContinent(worldPos);
                if (nearestPlate) {
                    rockTypes.setSample(x, z, nearestPlate->dominantRockType);
                } else {
                    // Fallback to granite
                    rockTypes.setSample(x, z, RockType::IGNEOUS_GRANITE);
                }
            }
        }
    }
}

void FractalContinentGenerator::initializeMantleStress(ContinuousField<float>& mantleStress, float worldSizeKm) {
    int width = mantleStress.getWidth();
    int height = mantleStress.getHeight();
    float spacing = mantleStress.getSampleSpacing();
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            glm::vec2 worldPos(x * spacing, z * spacing);
            
            // Base stress level
            float stress = 0.1f; // Low background stress
            
            // Higher stress near plate boundaries
            for (const auto& plate : continentalPlates_) {
                float distanceToEdge = std::abs(glm::length(worldPos - plate.center) - plate.radius);
                float plateEdgeInfluence = std::exp(-distanceToEdge / 10000.0f); // 10km falloff
                stress += plate.tectonicActivity * plateEdgeInfluence * 0.5f;
            }
            
            // Clamp stress to reasonable range
            stress = std::min(1.0f, stress);
            mantleStress.setSample(x, z, stress);
        }
    }
}

// Helper methods
ContinentalPlate* FractalContinentGenerator::findNearestContinent(const glm::vec2& point) {
    ContinentalPlate* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();
    
    for (auto& plate : continentalPlates_) {
        float distance = glm::length(point - plate.center);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearest = &plate;
        }
    }
    
    return nearest;
}

bool FractalContinentGenerator::isInOcean(const glm::vec2& point) const {
    for (const auto& plate : continentalPlates_) {
        float distance = glm::length(point - plate.center);
        if (distance <= plate.radius) {
            return false; // Inside a continent
        }
    }
    return true; // Not inside any continent, must be ocean
}

float FractalContinentGenerator::calculateDistanceToNearestContinent(const glm::vec2& point) const {
    float nearestDistance = std::numeric_limits<float>::max();
    
    for (const auto& plate : continentalPlates_) {
        float distance = glm::length(point - plate.center);
        nearestDistance = std::min(nearestDistance, distance);
    }
    
    return nearestDistance;
}

float FractalContinentGenerator::generateOceanDepth(const glm::vec2& point, float worldSizeKm) const {
    std::mt19937 rng(seed_ + 2000); // Offset seed for ocean features
    
    float worldSizeMeters = worldSizeKm * 1000.0f;
    float centerX = worldSizeMeters * 0.5f;
    float centerZ = worldSizeMeters * 0.5f;
    
    // Distance from world center affects depth (deeper toward center)
    float distanceFromCenter = glm::length(point - glm::vec2(centerX, centerZ));
    float maxDistance = worldSizeMeters * 0.7f; // 70% of world radius
    float centerFactor = std::min(1.0f, distanceFromCenter / maxDistance);
    
    // Check for mid-ocean ridges (elevated seafloor between continents)
    float ridgeNoise = perlinNoise(point.x, point.y, 0.0002f);
    bool isRidge = ridgeNoise > 0.6f;
    
    // Check for oceanic trenches (deep areas near continental margins)
    float nearestContinentDist = std::numeric_limits<float>::max();
    for (const auto& plate : continentalPlates_) {
        float dist = glm::length(point - plate.center);
        nearestContinentDist = std::min(nearestContinentDist, dist);
    }
    
    bool isTrench = (nearestContinentDist > 0 && nearestContinentDist < worldSizeMeters * 0.15f) &&
                    (perlinNoise(point.x, point.y, 0.0005f) < -0.5f);
    
    float baseDepth;
    if (isRidge) {
        // Mid-ocean ridges: elevated seafloor (+500m from deep ocean)
        baseDepth = -1000.0f + 500.0f; // -500m depth
    } else if (isTrench) {
        // Oceanic trenches: very deep (-2500m clamped to -2000m)
        baseDepth = -2000.0f; // Clamped maximum depth
    } else {
        // Regular ocean basin depth based on distance from center
        float minDepth = -1000.0f; // Shallow ocean
        float maxDepth = -1800.0f; // Abyssal plains
        baseDepth = minDepth + (maxDepth - minDepth) * centerFactor;
    }
    
    // Add some noise variation
    float depthNoise = fractionalBrownianMotion(point.x, point.y, 4, 0.3f);
    float finalDepth = baseDepth + (depthNoise * 200.0f);
    
    // Ensure depth stays within realistic bounds
    return std::max(-2000.0f, std::min(-100.0f, finalDepth));
}

// L-System implementation for river networks
std::vector<glm::vec2> FractalContinentGenerator::generateLSystemRiver(const glm::vec2& source, const glm::vec2& target, 
                                                                       float worldSizeKm, int iterations) const {
    // River L-System rules for branching patterns
    std::map<char, std::string> riverRules = {
        {'F', "F[+F]F[-F]F"},
        {'+', "+"},
        {'-', "-"},
        {'[', "["},
        {']', "]"}
    };
    
    std::string axiom = "F";
    std::string lsystem = applyLSystemRules(axiom, iterations, riverRules);
    
    glm::vec2 direction = glm::normalize(target - source);
    float stepSize = glm::length(target - source) / (lsystem.length() * 0.3f);
    
    return interpretLSystemString(lsystem, source, direction, stepSize);
}

std::vector<glm::vec2> FractalContinentGenerator::generateLSystemMountainRidge(const glm::vec2& start, const glm::vec2& end,
                                                                               float worldSizeKm, int iterations) const {
    // Mountain ridge L-System rules for realistic ridge patterns
    std::map<char, std::string> ridgeRules = {
        {'F', "F+F-F-F+F"},
        {'+', "+"},
        {'-', "-"}
    };
    
    std::string axiom = "F";
    std::string lsystem = applyLSystemRules(axiom, iterations, ridgeRules);
    
    glm::vec2 direction = glm::normalize(end - start);
    float stepSize = glm::length(end - start) / (lsystem.length() * 0.5f);
    
    return interpretLSystemString(lsystem, start, direction, stepSize);
}

std::string FractalContinentGenerator::applyLSystemRules(const std::string& axiom, int iterations, 
                                                         const std::map<char, std::string>& rules) const {
    std::string current = axiom;
    
    for (int i = 0; i < iterations; ++i) {
        std::string next;
        
        for (char c : current) {
            auto it = rules.find(c);
            if (it != rules.end()) {
                next += it->second;
            } else {
                next += c;
            }
        }
        
        current = next;
    }
    
    return current;
}

std::vector<glm::vec2> FractalContinentGenerator::interpretLSystemString(const std::string& lsystem, const glm::vec2& start,
                                                                         const glm::vec2& direction, float stepSize) const {
    std::vector<glm::vec2> path;
    glm::vec2 position = start;
    glm::vec2 currentDirection = direction;
    
    std::stack<std::pair<glm::vec2, glm::vec2>> stateStack;
    
    float angleIncrement = M_PI / 6.0f; // 30 degrees
    
    path.push_back(position);
    
    for (char c : lsystem) {
        switch (c) {
            case 'F':
                // Move forward
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
                
            case '+':
                // Turn right
                {
                    float cos_angle = std::cos(angleIncrement);
                    float sin_angle = std::sin(angleIncrement);
                    glm::vec2 newDirection;
                    newDirection.x = currentDirection.x * cos_angle - currentDirection.y * sin_angle;
                    newDirection.y = currentDirection.x * sin_angle + currentDirection.y * cos_angle;
                    currentDirection = newDirection;
                }
                break;
                
            case '-':
                // Turn left
                {
                    float cos_angle = std::cos(-angleIncrement);
                    float sin_angle = std::sin(-angleIncrement);
                    glm::vec2 newDirection;
                    newDirection.x = currentDirection.x * cos_angle - currentDirection.y * sin_angle;
                    newDirection.y = currentDirection.x * sin_angle + currentDirection.y * cos_angle;
                    currentDirection = newDirection;
                }
                break;
                
            case '[':
                // Push state
                stateStack.push({position, currentDirection});
                break;
                
            case ']':
                // Pop state
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    // Use plate-specific seed for consistent but unique patterns per continent
    uint64_t plateSpecificSeed = seed_ + (plate->plateId * 12345);
    
    glm::vec2 relativePos = worldPos - plate->center;
    float distanceFromCenter = glm::length(relativePos);
    float normalizedDistance = distanceFromCenter / plate->radius;
    
    // MUCH more aggressive domain warping for highly organic shapes
    float warpScale = 0.0005f; // Increased frequency for more detail
    float warpStrength = plate->radius * 0.8f; // Doubled warp strength
    
    // Multi-layer domain warping for extremely organic shapes
    float warpX1 = fractionalBrownianMotion(worldPos.x * warpScale, worldPos.y * warpScale, 6, 0.7f) * warpStrength;
    float warpY1 = fractionalBrownianMotion((worldPos.x + 1234.0f) * warpScale, (worldPos.y + 1234.0f) * warpScale, 6, 0.7f) * warpStrength;
    
    // Second layer of warping for even more chaos
    float warpX2 = fractionalBrownianMotion(worldPos.x * warpScale * 3.0f, worldPos.y * warpScale * 3.0f, 4, 0.5f) * warpStrength * 0.3f;
    float warpY2 = fractionalBrownianMotion((worldPos.x + 5678.0f) * warpScale * 3.0f, (worldPos.y + 5678.0f) * warpScale * 3.0f, 4, 0.5f) * warpStrength * 0.3f;
    
    glm::vec2 warpedPos = worldPos + glm::vec2(warpX1 + warpX2, warpY1 + warpY2);
    
    // Much more dramatic amplitude for highly irregular coastlines
    float baseScale = 0.0004f; // Increased frequency
    float amplitude = plate->radius * 0.5f; // Doubled from 0.25f - MUCH more variation
    
    // Multiple noise layers with varying characteristics for maximum organic complexity
    float continentalShape = fractionalBrownianMotion(warpedPos.x * baseScale * 0.2f, warpedPos.y * baseScale * 0.2f, 4, 0.8f) * amplitude;
    float coastalFeatures = fractionalBrownianMotion(warpedPos.x * baseScale * 1.0f, warpedPos.y * baseScale * 1.0f, 6, 0.6f) * amplitude * 0.8f;
    float coastalDetail = fractionalBrownianMotion(warpedPos.x * baseScale * 4.0f, warpedPos.y * baseScale * 4.0f, 8, 0.4f) * amplitude * 0.6f;
    float microDetail = fractionalBrownianMotion(warpedPos.x * baseScale * 12.0f, warpedPos.y * baseScale * 12.0f, 6, 0.3f) * amplitude * 0.3f;
    
    // Ridged noise for dramatic coastal features (fjords, cliffs, peninsulas)
    float ridgedFeatures1 = (1.0f - std::abs(perlinNoise(warpedPos.x * baseScale * 1.5f, warpedPos.y * baseScale * 1.5f, 1.0f)));
    ridgedFeatures1 = std::pow(ridgedFeatures1, 1.5f) * amplitude * 0.7f;
    
    float ridgedFeatures2 = (1.0f - std::abs(perlinNoise(warpedPos.x * baseScale * 6.0f, warpedPos.y * baseScale * 6.0f, 1.0f)));
    ridgedFeatures2 = std::pow(ridgedFeatures2, 2.0f) * amplitude * 0.4f;
    
    // Multiple angular harmonics for maximum irregularity
    float angle = std::atan2(relativePos.y, relativePos.x);
    float angularVar1 = std::sin(angle * 2.0f + plateSpecificSeed) * amplitude * 0.3f;
    float angularVar2 = std::sin(angle * 5.0f + plateSpecificSeed * 2) * amplitude * 0.2f;
    float angularVar3 = std::sin(angle * 11.0f + plateSpecificSeed * 3) * amplitude * 0.15f;
    float angularVar4 = std::sin(angle * 23.0f + plateSpecificSeed * 4) * amplitude * 0.1f;
    
    // Turbulence for chaotic coastlines
    float turbulence = std::abs(fractionalBrownianMotion(warpedPos.x * baseScale * 8.0f, warpedPos.y * baseScale * 8.0f, 6, 0.5f)) * amplitude * 0.4f;
    
    // Combine all variations for maximum organic complexity
    float totalNoise = continentalShape + coastalFeatures + coastalDetail + microDetail + 
                      ridgedFeatures1 + ridgedFeatures2 + angularVar1 + angularVar2 + angularVar3 + angularVar4 + turbulence;
    
    // Apply organic falloff (not linear - use power function for more natural transitions)
    float falloffFactor = 1.0f;
    if (normalizedDistance < 0.3f) {
        // Smooth interior with organic increase toward coast
        falloffFactor = std::pow(normalizedDistance / 0.3f, 0.7f);
    } else if (normalizedDistance > 0.9f) {
        // Organic falloff beyond coastline
        float beyondCoast = (normalizedDistance - 0.9f) / 0.3f;
        falloffFactor = std::max(0.1f, 1.0f - std::pow(beyondCoast, 1.5f));
    }
    
    return totalNoise * falloffFactor;
}

float FractalContinentGenerator::perlinNoise(float x, float y, float frequency) const {
    // Use existing noise utility
    return VoxelEngine::Util::smoothValueNoise(x * frequency, 0.0f, y * frequency);
}

float FractalContinentGenerator::fractionalBrownianMotion(float x, float y, int octaves, float persistence) const {
    float result = 0.0f;
    float amplitude = 1.0f;
    float frequency = 0.001f;
    
    for (int i = 0; i < octaves; ++i) {
        result += amplitude * perlinNoise(x, y, frequency);
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return result;
}

glm::vec2 FractalContinentGenerator::generateCoastlinePoint(const glm::vec2& basePoint, float detail) const {
    // Add fractal detail to coastline points
    float detailNoise = fractionalBrownianMotion(basePoint.x, basePoint.y, 6, 0.6f);
    return basePoint + glm::vec2(detailNoise * detail, detailNoise * detail);
}

float FractalContinentGenerator::generateOrganicContinentalElevation(const glm::vec2& worldPos, float worldSizeKm) {
    float worldSizeMeters = worldSizeKm * 1000.0f;
    
    // Normalize position to 0-1 range for consistent noise sampling
    glm::vec2 normalizedPos = worldPos / worldSizeMeters;
    
    // Multiple fractal domain layers for HIGHLY organic continental structure
    
    // 1. Continental mass distribution - large scale features with MUCH more variation
    float continentalScale = 0.6f;  // Smaller scale = more variation
    float continentalBase = fractionalBrownianMotion(normalizedPos.x * continentalScale, normalizedPos.y * continentalScale, 6, 0.9f);
    
    // 2. AGGRESSIVE domain warping for highly irregular shapes
    float warpStrength = 0.7f; // Increased from 0.3f for much more distortion
    float warpX = fractionalBrownianMotion(normalizedPos.x * 1.5f + 1000.0f, normalizedPos.y * 1.5f + 1000.0f, 8, 0.8f) * warpStrength;
    float warpY = fractionalBrownianMotion(normalizedPos.x * 1.5f + 2000.0f, normalizedPos.y * 1.5f + 2000.0f, 8, 0.8f) * warpStrength;
    
    glm::vec2 warpedPos = normalizedPos + glm::vec2(warpX, warpY);
    
    // 3. Continental-scale features with warped coordinates and MORE octaves
    float continentMask = fractionalBrownianMotion(warpedPos.x * 0.8f, warpedPos.y * 0.8f, 8, 0.8f);
    
    // 4. Multiple ridged noise layers for VERY dramatic coastlines
    float ridged1 = 1.0f - std::abs(perlinNoise(warpedPos.x * 3.5f, warpedPos.y * 3.5f, 1.0f));
    ridged1 = std::pow(ridged1, 2.5f); // More dramatic ridges
    
    float ridged2 = 1.0f - std::abs(perlinNoise(warpedPos.x * 7.0f + 500.0f, warpedPos.y * 7.0f + 500.0f, 1.0f));
    ridged2 = std::pow(ridged2, 3.0f); // Even sharper ridges
    
    float ridged3 = 1.0f - std::abs(perlinNoise(warpedPos.x * 12.0f + 1500.0f, warpedPos.y * 12.0f + 1500.0f, 1.0f));
    ridged3 = std::pow(ridged3, 2.0f); // Fine-scale ridges
    
    // 5. Much more fine detail for coastal complexity
    float detail = fractionalBrownianMotion(warpedPos.x * 12.0f, warpedPos.y * 12.0f, 10, 0.3f);
    
    // 6. TRIPLE domain warping for maximum organic complexity
    float warp2X = fractionalBrownianMotion(warpedPos.x * 6.0f + 3000.0f, warpedPos.y * 6.0f + 3000.0f, 6, 0.7f) * 0.4f;
    float warp2Y = fractionalBrownianMotion(warpedPos.x * 6.0f + 4000.0f, warpedPos.y * 6.0f + 4000.0f, 6, 0.7f) * 0.4f;
    
    glm::vec2 doubleWarpedPos = warpedPos + glm::vec2(warp2X, warp2Y);
    
    // 7. Third level warping for ultimate irregularity
    float warp3X = fractionalBrownianMotion(doubleWarpedPos.x * 10.0f + 5000.0f, doubleWarpedPos.y * 10.0f + 5000.0f, 4, 0.6f) * 0.2f;
    float warp3Y = fractionalBrownianMotion(doubleWarpedPos.x * 10.0f + 6000.0f, doubleWarpedPos.y * 10.0f + 6000.0f, 4, 0.6f) * 0.2f;
    
    glm::vec2 tripleWarpedPos = doubleWarpedPos + glm::vec2(warp3X, warp3Y);
    
    // 8. Additional organic shape with triple-warped coordinates
    float organicShape = fractionalBrownianMotion(tripleWarpedPos.x * 2.5f, tripleWarpedPos.y * 2.5f, 9, 0.7f);
    
    // 9. Archipelago-style island chains
    float islandChains = fractionalBrownianMotion(tripleWarpedPos.x * 15.0f + 7000.0f, tripleWarpedPos.y * 15.0f + 7000.0f, 6, 0.5f);
    
    // Combine all layers for MAXIMUM organic complexity with more dramatic weighting
    float combinedNoise = (continentalBase * 0.3f + continentMask * 0.25f + organicShape * 0.25f) + 
                         (ridged1 * 0.35f + ridged2 * 0.25f + ridged3 * 0.15f) + 
                         (detail * 0.15f) + (islandChains * 0.1f);
    
    // More aggressive sea level threshold for more irregular coastlines
    float seaLevel = 0.0f;
    float landThreshold = 0.05f; // Lower threshold = more irregular coastlines
    
    if (combinedNoise > landThreshold) {
        // Above sea level - continental landmass with more dramatic elevation
        float landHeight = (combinedNoise - landThreshold) / (1.0f - landThreshold); // Normalize to 0-1
        
        // Apply highly organic elevation scaling
        float baseElevation = 20.0f + (landHeight * 500.0f); // Higher base elevation
        
        // Add MUCH more dramatic mountain ranges
        float mountainFactor = (ridged1 + ridged2 * 0.7f + ridged3 * 0.3f) * 0.6f;
        float mountainHeight = mountainFactor * 1200.0f * landHeight; // Much higher mountains
        
        // Add more terrain variation
        float terrainVariation = detail * 150.0f * landHeight;
        
        return baseElevation + mountainHeight + terrainVariation;
    } else {
        // Below sea level - ocean with more varied depths
        float oceanDepth = (landThreshold - combinedNoise) / landThreshold; // Normalize ocean depth
        float baseDepth = -30.0f - (oceanDepth * 500.0f); // Ocean depths from -30m to -530m
        
        // Add more ocean floor variation
        float oceanVariation = detail * 80.0f * oceanDepth;
        
        return baseDepth - oceanVariation;
    }
}

} // namespace World
} // namespace VoxelCastle
