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
    
    // Generate continental seeds using Mitchell's best-candidate algorithm
    auto continentalSeeds = generateContinentalSeeds(worldSizeKm, numContinents);
    
    // Create continental plates with varying sizes
    for (int i = 0; i < numContinents; ++i) {
        ContinentalPlate plate;
        plate.center = continentalSeeds[i];
        plate.plateId = i;
        
        // Size variation: largest ~40% of world, smallest ~5%
        float sizeVariation = 0.05f + (0.35f * dist(rng)); // 5% to 40% of world size
        plate.radius = worldSizeMeters * sizeVariation * 0.5f; // Convert to radius
        
        // Base elevation for continents (0m to +400m)
        plate.elevation = 100.0f + (300.0f * dist(rng)); // 100m to 400m base elevation
        
        // Assign dominant rock type (continental crust)
        float rockChoice = dist(rng);
        if (rockChoice < 0.4f) {
            plate.dominantRockType = RockType::IGNEOUS_GRANITE;
        } else if (rockChoice < 0.7f) {
            plate.dominantRockType = RockType::METAMORPHIC_QUARTZITE;
        } else {
            plate.dominantRockType = RockType::SEDIMENTARY_SANDSTONE;
        }
        
        // Tectonic activity varies by plate
        plate.tectonicActivity = 0.3f + (0.7f * dist(rng)); // 0.3 to 1.0
        
        continentalPlates_.push_back(plate);
    }
}

std::vector<glm::vec2> FractalContinentGenerator::generateContinentalSeeds(float worldSizeKm, int numContinents) {
    std::mt19937 rng(seed_);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    std::vector<glm::vec2> seeds;
    float worldSizeMeters = worldSizeKm * 1000.0f;
    float minDistance = worldSizeMeters * 0.15f; // Minimum 15% of world size between continents
    
    // Mitchell's best-candidate algorithm for good distribution
    int maxAttempts = 100;
    
    for (int i = 0; i < numContinents; ++i) {
        glm::vec2 bestCandidate;
        float bestDistance = 0.0f;
        
        for (int attempt = 0; attempt < maxAttempts; ++attempt) {
            // Generate random candidate position
            glm::vec2 candidate;
            candidate.x = dist(rng) * worldSizeMeters;
            candidate.y = dist(rng) * worldSizeMeters;
            
            // Find distance to nearest existing seed
            float nearestDistance = worldSizeMeters; // Start with max possible
            for (const auto& existingSeed : seeds) {
                float distance = glm::length(candidate - existingSeed);
                nearestDistance = std::min(nearestDistance, distance);
            }
            
            // Keep candidate with best (largest) minimum distance
            if (nearestDistance > bestDistance) {
                bestDistance = nearestDistance;
                bestCandidate = candidate;
            }
        }
        
        // Accept best candidate if it meets minimum distance requirement or we're out of attempts
        if (bestDistance >= minDistance || seeds.empty()) {
            seeds.push_back(bestCandidate);
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
    std::uniform_real_distribution<float> depthDist(-2000.0f, -1000.0f);
    
    mainOcean.depth = depthDist(rng);
    mainOcean.tectonicActivity = 0.2f; // Lower activity in ocean basins
    
    // Boundary is the world boundary for now (can be refined later)
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
    
    std::cout << "[FractalContinentGenerator] Generating realistic fractal coastlines for " << width << "x" << height 
              << " grid with " << spacing << "m spacing" << std::endl;
    
    // Initialize all points to ocean depth
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float worldX = x * spacing;
            float worldZ = z * spacing;
            glm::vec2 worldPos(worldX, worldZ);
            
            // Find nearest continental plate
            ContinentalPlate* nearestPlate = findNearestContinent(worldPos);
            
            if (nearestPlate) {
                float distanceToCenter = glm::length(worldPos - nearestPlate->center);
                
                // Generate realistic fractal coastline using multiple noise layers
                float coastlineNoise = generateCoastlineNoise(worldPos, nearestPlate);
                
                // Determine if this point is land or water based on fractal coastline
                float effectiveRadius = nearestPlate->radius + coastlineNoise;
                
                if (distanceToCenter < effectiveRadius) {
                    // Inside fractal continent - generate realistic terrain
                    float elevationNoise = fractionalBrownianMotion(worldX, worldZ, 7, 0.6f);
                    
                    // Distance-based elevation with realistic variation
                    float centerFactor = 1.0f - (distanceToCenter / effectiveRadius);
                    centerFactor = std::pow(centerFactor, 0.5f); // More gradual elevation changes
                    
                    // Add local terrain variation
                    float localNoise = perlinNoise(worldX * 0.001f, worldZ * 0.001f, 1.0f) * 150.0f;
                    float mediumNoise = perlinNoise(worldX * 0.0005f, worldZ * 0.0005f, 1.0f) * 300.0f;
                    
                    float elevation = nearestPlate->elevation * centerFactor + 
                                    (elevationNoise * 100.0f) + localNoise + mediumNoise;
                    
                    // Ensure minimum land elevation
                    elevation = std::max(5.0f, elevation);
                    
                    // Add coastal variation near shoreline
                    float coastalDistance = effectiveRadius - distanceToCenter;
                    if (coastalDistance < 5000.0f) { // Within 5km of coast
                        float coastalFactor = coastalDistance / 5000.0f;
                        float coastalNoise = fractionalBrownianMotion(worldX * 2.0f, worldZ * 2.0f, 4, 0.4f);
                        elevation *= (0.3f + 0.7f * coastalFactor) + (coastalNoise * 20.0f * coastalFactor);
                        elevation = std::max(1.0f, elevation);
                    }
                    
                    elevationField.setSample(x, z, elevation);
                } else if (distanceToCenter < effectiveRadius + 10000.0f) {
                    // Continental shelf zone with fractal variation
                    float shelfDistance = distanceToCenter - effectiveRadius;
                    float shelfFactor = shelfDistance / 10000.0f; // 10km shelf
                    float shelfNoise = fractionalBrownianMotion(worldX * 1.5f, worldZ * 1.5f, 5, 0.4f);
                    
                    float shelfDepth = -5.0f - (shelfFactor * 195.0f) + (shelfNoise * 30.0f);
                    shelfDepth = std::max(-200.0f, std::min(-1.0f, shelfDepth));
                    
                    elevationField.setSample(x, z, shelfDepth);
                } else {
                    // Deep ocean with realistic variation
                    float oceanDepth = generateOceanDepth(worldPos, worldSizeKm);
                    elevationField.setSample(x, z, oceanDepth);
                }
            } else {
                // No nearby continent - deep ocean
                float oceanDepth = generateOceanDepth(worldPos, worldSizeKm);
                elevationField.setSample(x, z, oceanDepth);
            }
        }
        }
    }
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
            
            // River source: high elevation area near continent center
            float sourceAngle = dist(rng) * 2.0f * M_PI;
            float sourceDistance = continent.radius * (0.3f + 0.4f * dist(rng)); // 30-70% from center
            river.source = continent.center + glm::vec2(
                std::cos(sourceAngle) * sourceDistance,
                std::sin(sourceAngle) * sourceDistance
            );
            
            // River mouth: find nearest coastline
            float mouthAngle = dist(rng) * 2.0f * M_PI;
            float mouthDistance = continent.radius * (0.9f + 0.1f * dist(rng)); // Near edge
            river.mouth = continent.center + glm::vec2(
                std::cos(mouthAngle) * mouthDistance,
                std::sin(mouthAngle) * mouthDistance
            );
            
            // Generate L-System river network
            river.mainStem = generateLSystemRiver(river.source, river.mouth, worldSizeKm, 3);
            
            // Generate 2-3 major tributaries
            int numTributaries = 2 + (rng() % 2);
            for (int t = 0; t < numTributaries; ++t) {
                if (river.mainStem.size() > 4) {
                    // Pick a point along the main stem for tributary junction
                    size_t junctionIndex = 2 + (rng() % (river.mainStem.size() - 3));
                    glm::vec2 junction = river.mainStem[junctionIndex];
                    
                    // Generate tributary source point
                    float tribAngle = dist(rng) * 2.0f * M_PI;
                    float tribDistance = continent.radius * (0.2f + 0.3f * dist(rng));
                    glm::vec2 tribSource = junction + glm::vec2(
                        std::cos(tribAngle) * tribDistance,
                        std::sin(tribAngle) * tribDistance
                    );
                    
                    std::vector<glm::vec2> tributary = generateLSystemRiver(tribSource, junction, worldSizeKm, 2);
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
            
            // Check if plates are close enough for mountain formation
            if (distance < combinedRadius * 1.5f) {
                MountainRidge ridge;
                ridge.continentId = plate1.plateId; // Assign to first plate
                ridge.baseElevation = 400.0f + (400.0f * dist(rng)); // 400-800m base
                ridge.maxElevation = ridge.baseElevation + (600.0f + 400.0f * dist(rng)); // +600-1000m peaks
                ridge.isVolcanic = (dist(rng) < 0.3f); // 30% chance of volcanic activity
                
                // Create ridge line between collision zones
                glm::vec2 midpoint = (plate1.center + plate2.center) * 0.5f;
                glm::vec2 direction = glm::normalize(plate2.center - plate1.center);
                glm::vec2 perpendicular = glm::vec2(-direction.y, direction.x);
                
                // Generate L-System mountain ridge
                glm::vec2 ridgeStart = midpoint + perpendicular * (combinedRadius * 0.3f);
                glm::vec2 ridgeEnd = midpoint - perpendicular * (combinedRadius * 0.3f);
                ridge.ridgeLine = generateLSystemMountainRidge(ridgeStart, ridgeEnd, worldSizeKm, 2);
                
                mountainRidges_.push_back(ridge);
                
                // Apply mountain elevation to the elevation field
                for (const auto& ridgePoint : ridge.ridgeLine) {
                    int x = static_cast<int>(ridgePoint.x / spacing);
                    int z = static_cast<int>(ridgePoint.y / spacing);
                    
                    if (x >= 0 && x < width && z >= 0 && z < height) {
                        float currentElevation = elevationField.getSample(x, z);
                        if (currentElevation > 0.0f) { // Only affect land areas
                            float mountainElevation = ridge.baseElevation + 
                                (ridge.maxElevation - ridge.baseElevation) * dist(rng);
                            elevationField.setSample(x, z, std::max(currentElevation, mountainElevation));
                            
                            // Add mountain influence to nearby points
                            int radius = 3;
                            for (int dx = -radius; dx <= radius; ++dx) {
                                for (int dz = -radius; dz <= radius; ++dz) {
                                    int nx = x + dx;
                                    int nz = z + dz;
                                    if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
                                        float distance = std::sqrt(dx*dx + dz*dz);
                                        if (distance <= radius) {
                                            float influence = 1.0f - (distance / radius);
                                            float neighborElevation = elevationField.getSample(nx, nz);
                                            if (neighborElevation > 0.0f) {
                                                float addedHeight = mountainElevation * influence * 0.5f;
                                                elevationField.setSample(nx, nz, neighborElevation + addedHeight);
                                            }
                                        }
                                    }
                                }
                            }
                        }
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
            float worldX = x * spacing;
            float worldZ = z * spacing;
            glm::vec2 worldPos(worldX, worldZ);
            
            if (isInOcean(worldPos)) {
                // Oceanic crust - primarily basalt
                rockTypes.setSample(x, z, RockType::IGNEOUS_BASALT);
            } else {
                // Continental crust - use continental plate's dominant rock type
                ContinentalPlate* nearestPlate = findNearestContinent(worldPos);
                if (nearestPlate) {
                    // Add some variation to the dominant rock type
                    float rockVariation = perlinNoise(worldX, worldZ, 0.001f);
                    
                    if (rockVariation > 0.7f) {
                        // 30% chance of different rock type
                        rockTypes.setSample(x, z, RockType::SEDIMENTARY_LIMESTONE);
                    } else {
                        rockTypes.setSample(x, z, nearestPlate->dominantRockType);
                    }
                } else {
                    rockTypes.setSample(x, z, RockType::IGNEOUS_GRANITE); // Default
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
            float worldX = x * spacing;
            float worldZ = z * spacing;
            glm::vec2 worldPos(worldX, worldZ);
            
            // Initialize mantle stress based on continental layout
            ContinentalPlate* nearestPlate = findNearestContinent(worldPos);
            
            if (nearestPlate) {
                float distanceToCenter = glm::length(worldPos - nearestPlate->center);
                float plateInfluence = std::exp(-distanceToCenter / (nearestPlate->radius * 2.0f));
                
                // Higher stress near plate boundaries, lower at centers
                float stressValue = nearestPlate->tectonicActivity * (1.0f - plateInfluence * 0.5f);
                
                // Add fractal variation
                float stressNoise = fractionalBrownianMotion(worldX, worldZ, 4, 0.4f);
                stressValue += stressNoise * 0.3f;
                
                mantleStress.setSample(x, z, stressValue);
            } else {
                // Ocean areas have different stress patterns
                float stressNoise = fractionalBrownianMotion(worldX, worldZ, 3, 0.3f);
                mantleStress.setSample(x, z, stressNoise * 0.5f);
            }
        }
    }
}

// Helper methods implementation

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
        if (distance < plate.radius) {
            return false; // Inside a continent
        }
    }
    return true; // Not inside any continent, must be ocean
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
        {'F', "F[+F]F[-F]F"},  // Forward with branches
        {'+', "+"},             // Turn right
        {'-', "-"},             // Turn left
        {'[', "["},             // Start branch
        {']', "]"}              // End branch
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
        {'F', "F+F-F-F+F"},    // Forward with zigzag pattern
        {'+', "+"},             // Turn right
        {'-', "-"}              // Turn left
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
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
            case '+':
                {
                    float cos_a = std::cos(angleIncrement);
                    float sin_a = std::sin(angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '-':
                {
                    float cos_a = std::cos(-angleIncrement);
                    float sin_a = std::sin(-angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '[':
                stateStack.push(std::make_pair(position, currentDirection));
                break;
            case ']':
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                    path.push_back(position);
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    float scale = 0.0001f;
    float amplitude = 8000.0f;
    
    // Multi-layer fractal noise for realistic coastlines
    float largeScale = perlinNoise(worldPos.x * scale * 0.5f, worldPos.y * scale * 0.5f, 1.0f) * amplitude;
    float mediumScale = perlinNoise(worldPos.x * scale * 2.0f, worldPos.y * scale * 2.0f, 1.0f) * amplitude * 0.4f;
    float smallScale = perlinNoise(worldPos.x * scale * 8.0f, worldPos.y * scale * 8.0f, 1.0f) * amplitude * 0.15f;
    
    // Angular variation
    glm::vec2 relativePos = worldPos - plate->center;
    float angle = std::atan2(relativePos.y, relativePos.x);
    float directionalNoise = std::sin(angle * 3.0f) * amplitude * 0.2f;
    
    float totalNoise = largeScale + mediumScale + smallScale + directionalNoise;
    
    // Reduce variation far from coastline
    float distanceFromCenter = glm::length(relativePos);
    float centerFactor = std::min(1.0f, distanceFromCenter / (plate->radius * 0.8f));
    float variationFactor = std::sin(centerFactor * M_PI) * 0.8f + 0.2f;
    
    return totalNoise * variationFactor;
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
        {'F', "F[+F]F[-F]F"},  // Forward with branches
        {'+', "+"},             // Turn right
        {'-', "-"},             // Turn left
        {'[', "["},             // Start branch
        {']', "]"}              // End branch
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
        {'F', "F+F-F-F+F"},    // Forward with zigzag pattern
        {'+', "+"},             // Turn right
        {'-', "-"}              // Turn left
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
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
            case '+':
                {
                    float cos_a = std::cos(angleIncrement);
                    float sin_a = std::sin(angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '-':
                {
                    float cos_a = std::cos(-angleIncrement);
                    float sin_a = std::sin(-angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '[':
                stateStack.push(std::make_pair(position, currentDirection));
                break;
            case ']':
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                    path.push_back(position);
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    float scale = 0.0001f;
    float amplitude = 8000.0f;
    
    // Multi-layer fractal noise for realistic coastlines
    float largeScale = perlinNoise(worldPos.x * scale * 0.5f, worldPos.y * scale * 0.5f, 1.0f) * amplitude;
    float mediumScale = perlinNoise(worldPos.x * scale * 2.0f, worldPos.y * scale * 2.0f, 1.0f) * amplitude * 0.4f;
    float smallScale = perlinNoise(worldPos.x * scale * 8.0f, worldPos.y * scale * 8.0f, 1.0f) * amplitude * 0.15f;
    
    // Angular variation
    glm::vec2 relativePos = worldPos - plate->center;
    float angle = std::atan2(relativePos.y, relativePos.x);
    float directionalNoise = std::sin(angle * 3.0f) * amplitude * 0.2f;
    
    float totalNoise = largeScale + mediumScale + smallScale + directionalNoise;
    
    // Reduce variation far from coastline
    float distanceFromCenter = glm::length(relativePos);
    float centerFactor = std::min(1.0f, distanceFromCenter / (plate->radius * 0.8f));
    float variationFactor = std::sin(centerFactor * M_PI) * 0.8f + 0.2f;
    
    return totalNoise * variationFactor;
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
        {'F', "F[+F]F[-F]F"},  // Forward with branches
        {'+', "+"},             // Turn right
        {'-', "-"},             // Turn left
        {'[', "["},             // Start branch
        {']', "]"}              // End branch
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
        {'F', "F+F-F-F+F"},    // Forward with zigzag pattern
        {'+', "+"},             // Turn right
        {'-', "-"}              // Turn left
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
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
            case '+':
                {
                    float cos_a = std::cos(angleIncrement);
                    float sin_a = std::sin(angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '-':
                {
                    float cos_a = std::cos(-angleIncrement);
                    float sin_a = std::sin(-angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '[':
                stateStack.push(std::make_pair(position, currentDirection));
                break;
            case ']':
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                    path.push_back(position);
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    float scale = 0.0001f;
    float amplitude = 8000.0f;
    
    // Multi-layer fractal noise for realistic coastlines
    float largeScale = perlinNoise(worldPos.x * scale * 0.5f, worldPos.y * scale * 0.5f, 1.0f) * amplitude;
    float mediumScale = perlinNoise(worldPos.x * scale * 2.0f, worldPos.y * scale * 2.0f, 1.0f) * amplitude * 0.4f;
    float smallScale = perlinNoise(worldPos.x * scale * 8.0f, worldPos.y * scale * 8.0f, 1.0f) * amplitude * 0.15f;
    
    // Angular variation
    glm::vec2 relativePos = worldPos - plate->center;
    float angle = std::atan2(relativePos.y, relativePos.x);
    float directionalNoise = std::sin(angle * 3.0f) * amplitude * 0.2f;
    
    float totalNoise = largeScale + mediumScale + smallScale + directionalNoise;
    
    // Reduce variation far from coastline
    float distanceFromCenter = glm::length(relativePos);
    float centerFactor = std::min(1.0f, distanceFromCenter / (plate->radius * 0.8f));
    float variationFactor = std::sin(centerFactor * M_PI) * 0.8f + 0.2f;
    
    return totalNoise * variationFactor;
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
        {'F', "F[+F]F[-F]F"},  // Forward with branches
        {'+', "+"},             // Turn right
        {'-', "-"},             // Turn left
        {'[', "["},             // Start branch
        {']', "]"}              // End branch
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
        {'F', "F+F-F-F+F"},    // Forward with zigzag pattern
        {'+', "+"},             // Turn right
        {'-', "-"}              // Turn left
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
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
            case '+':
                {
                    float cos_a = std::cos(angleIncrement);
                    float sin_a = std::sin(angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '-':
                {
                    float cos_a = std::cos(-angleIncrement);
                    float sin_a = std::sin(-angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '[':
                stateStack.push(std::make_pair(position, currentDirection));
                break;
            case ']':
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                    path.push_back(position);
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    float scale = 0.0001f;
    float amplitude = 8000.0f;
    
    // Multi-layer fractal noise for realistic coastlines
    float largeScale = perlinNoise(worldPos.x * scale * 0.5f, worldPos.y * scale * 0.5f, 1.0f) * amplitude;
    float mediumScale = perlinNoise(worldPos.x * scale * 2.0f, worldPos.y * scale * 2.0f, 1.0f) * amplitude * 0.4f;
    float smallScale = perlinNoise(worldPos.x * scale * 8.0f, worldPos.y * scale * 8.0f, 1.0f) * amplitude * 0.15f;
    
    // Angular variation
    glm::vec2 relativePos = worldPos - plate->center;
    float angle = std::atan2(relativePos.y, relativePos.x);
    float directionalNoise = std::sin(angle * 3.0f) * amplitude * 0.2f;
    
    float totalNoise = largeScale + mediumScale + smallScale + directionalNoise;
    
    // Reduce variation far from coastline
    float distanceFromCenter = glm::length(relativePos);
    float centerFactor = std::min(1.0f, distanceFromCenter / (plate->radius * 0.8f));
    float variationFactor = std::sin(centerFactor * M_PI) * 0.8f + 0.2f;
    
    return totalNoise * variationFactor;
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
        {'F', "F[+F]F[-F]F"},  // Forward with branches
        {'+', "+"},             // Turn right
        {'-', "-"},             // Turn left
        {'[', "["},             // Start branch
        {']', "]"}              // End branch
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
        {'F', "F+F-F-F+F"},    // Forward with zigzag pattern
        {'+', "+"},             // Turn right
        {'-', "-"}              // Turn left
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
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
            case '+':
                {
                    float cos_a = std::cos(angleIncrement);
                    float sin_a = std::sin(angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '-':
                {
                    float cos_a = std::cos(-angleIncrement);
                    float sin_a = std::sin(-angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '[':
                stateStack.push(std::make_pair(position, currentDirection));
                break;
            case ']':
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                    path.push_back(position);
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    float scale = 0.0001f;
    float amplitude = 8000.0f;
    
    // Multi-layer fractal noise for realistic coastlines
    float largeScale = perlinNoise(worldPos.x * scale * 0.5f, worldPos.y * scale * 0.5f, 1.0f) * amplitude;
    float mediumScale = perlinNoise(worldPos.x * scale * 2.0f, worldPos.y * scale * 2.0f, 1.0f) * amplitude * 0.4f;
    float smallScale = perlinNoise(worldPos.x * scale * 8.0f, worldPos.y * scale * 8.0f, 1.0f) * amplitude * 0.15f;
    
    // Angular variation
    glm::vec2 relativePos = worldPos - plate->center;
    float angle = std::atan2(relativePos.y, relativePos.x);
    float directionalNoise = std::sin(angle * 3.0f) * amplitude * 0.2f;
    
    float totalNoise = largeScale + mediumScale + smallScale + directionalNoise;
    
    // Reduce variation far from coastline
    float distanceFromCenter = glm::length(relativePos);
    float centerFactor = std::min(1.0f, distanceFromCenter / (plate->radius * 0.8f));
    float variationFactor = std::sin(centerFactor * M_PI) * 0.8f + 0.2f;
    
    return totalNoise * variationFactor;
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
        {'F', "F[+F]F[-F]F"},  // Forward with branches
        {'+', "+"},             // Turn right
        {'-', "-"},             // Turn left
        {'[', "["},             // Start branch
        {']', "]"}              // End branch
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
        {'F', "F+F-F-F+F"},    // Forward with zigzag pattern
        {'+', "+"},             // Turn right
        {'-', "-"}              // Turn left
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
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
            case '+':
                {
                    float cos_a = std::cos(angleIncrement);
                    float sin_a = std::sin(angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '-':
                {
                    float cos_a = std::cos(-angleIncrement);
                    float sin_a = std::sin(-angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '[':
                stateStack.push(std::make_pair(position, currentDirection));
                break;
            case ']':
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                    path.push_back(position);
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    float scale = 0.0001f;
    float amplitude = 8000.0f;
    
    // Multi-layer fractal noise for realistic coastlines
    float largeScale = perlinNoise(worldPos.x * scale * 0.5f, worldPos.y * scale * 0.5f, 1.0f) * amplitude;
    float mediumScale = perlinNoise(worldPos.x * scale * 2.0f, worldPos.y * scale * 2.0f, 1.0f) * amplitude * 0.4f;
    float smallScale = perlinNoise(worldPos.x * scale * 8.0f, worldPos.y * scale * 8.0f, 1.0f) * amplitude * 0.15f;
    
    // Angular variation
    glm::vec2 relativePos = worldPos - plate->center;
    float angle = std::atan2(relativePos.y, relativePos.x);
    float directionalNoise = std::sin(angle * 3.0f) * amplitude * 0.2f;
    
    float totalNoise = largeScale + mediumScale + smallScale + directionalNoise;
    
    // Reduce variation far from coastline
    float distanceFromCenter = glm::length(relativePos);
    float centerFactor = std::min(1.0f, distanceFromCenter / (plate->radius * 0.8f));
    float variationFactor = std::sin(centerFactor * M_PI) * 0.8f + 0.2f;
    
    return totalNoise * variationFactor;
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
        {'F', "F[+F]F[-F]F"},  // Forward with branches
        {'+', "+"},             // Turn right
        {'-', "-"},             // Turn left
        {'[', "["},             // Start branch
        {']', "]"}              // End branch
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
        {'F', "F+F-F-F+F"},    // Forward with zigzag pattern
        {'+', "+"},             // Turn right
        {'-', "-"}              // Turn left
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
                position += currentDirection * stepSize;
                path.push_back(position);
                break;
            case '+':
                {
                    float cos_a = std::cos(angleIncrement);
                    float sin_a = std::sin(angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '-':
                {
                    float cos_a = std::cos(-angleIncrement);
                    float sin_a = std::sin(-angleIncrement);
                    glm::vec2 newDir;
                    newDir.x = currentDirection.x * cos_a - currentDirection.y * sin_a;
                    newDir.y = currentDirection.x * sin_a + currentDirection.y * cos_a;
                    currentDirection = newDir;
                }
                break;
            case '[':
                stateStack.push(std::make_pair(position, currentDirection));
                break;
            case ']':
                if (!stateStack.empty()) {
                    auto state = stateStack.top();
                    stateStack.pop();
                    position = state.first;
                    currentDirection = state.second;
                    path.push_back(position);
                }
                break;
        }
    }
    
    return path;
}

float FractalContinentGenerator::generateCoastlineNoise(const glm::vec2& worldPos, const ContinentalPlate* plate) const {
    float scale = 0.0001f;
    float amplitude = 8000.0f;
    
    // Multi-layer fractal noise for realistic coastlines
    float largeScale = perlinNoise(worldPos.x * scale * 0.5f, worldPos.y * scale * 0.5f, 1.0f) * amplitude;
    float mediumScale = perlinNoise(worldPos.x * scale * 2.0f, worldPos.y * scale * 2.0f, 1.0f) * amplitude * 0.4f;
    float smallScale = perlinNoise(worldPos.x * scale * 8.0f, worldPos.y * scale * 8.0f, 1.0f) * amplitude * 0.15f;
    
    // Angular variation
    glm::vec2 relativePos = worldPos - plate->center;
    float angle = std::atan2(relativePos.y, relativePos.x);
    float directionalNoise = std::sin(angle * 3.0f) * amplitude * 0.2f;
    
    float totalNoise = largeScale + mediumScale + smallScale + directionalNoise;
    
    // Reduce variation far from coastline
    float distanceFromCenter = glm::length(relativePos);
    float centerFactor = std::min(1.0f, distanceFromCenter / (plate->radius * 0.8f));
    float variationFactor = std::sin(centerFactor * M_PI) * 0.8f + 0.2f;
    
    return totalNoise * variationFactor;
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

} // namespace World
} // namespace VoxelCastle