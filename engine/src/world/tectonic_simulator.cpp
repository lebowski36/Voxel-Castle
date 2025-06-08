#include "world/tectonic_simulator.h"
#include <algorithm>
#include <random>
#include <cmath>
#include <limits>
#include <cassert>
#include <iostream>

namespace VoxelCastle {

TectonicSimulator::TectonicSimulator()
    : seed_(0)
    , worldSize_(100.0f)
    , simulationComplete_(false)
    , timeStep_(0.0f)
    , totalSimulationTime_(0.0f)
    , mapResolution_(64)
{
}

TectonicSimulator::~TectonicSimulator() = default;

void TectonicSimulator::InitializePlates(uint32_t seed, float worldSize, uint32_t plateCount) {
    seed_ = seed;
    worldSize_ = worldSize;
    simulationComplete_ = false;
    totalSimulationTime_ = 0.0f;
    
    // Clear existing data
    plates_.clear();
    boundaries_.clear();
    
    // Determine plate count if not specified
    if (plateCount == 0) {
        plateCount = GetOptimalPlateCount(worldSize);
    }
    
    // Generate initial plates
    GeneratePlates();
    
    // Create spatial grid for optimization
    BuildSpatialGrid();
    
    // Initialize stress and terrain maps
    stressMap_.assign(mapResolution_, std::vector<float>(mapResolution_, 0.0f));
    terrainMap_.assign(mapResolution_, std::vector<TerrainType>(mapResolution_, TerrainType::STABLE));
    elevationMap_.assign(mapResolution_, std::vector<float>(mapResolution_, 0.0f));
}

void TectonicSimulator::SimulatePlateMovement(float simulationTime, uint32_t timeSteps) {
    if (plates_.empty()) {
        return;
    }
    
    float deltaTime = simulationTime / static_cast<float>(timeSteps);
    timeStep_ = deltaTime;
    
    for (uint32_t step = 0; step < timeSteps; step++) {
        // Detect plate collisions and update boundaries
        DetectPlateCollisions();
        
        // Calculate stress accumulation
        CalculateStressAccumulation(deltaTime);
        
        // Update plate positions based on movement
        UpdatePlatePositions(deltaTime);
        
        // Rebuild spatial grid if plates moved significantly
        if (step % 5 == 0) { // Rebuild every 5 steps
            BuildSpatialGrid();
        }
    }
    
    totalSimulationTime_ += simulationTime;
    simulationComplete_ = true;
}

void TectonicSimulator::GenerateBoundaryFeatures() {
    if (!simulationComplete_) {
        return;
    }
    
    // Process each boundary and generate terrain features
    for (auto& boundary : boundaries_) {
        switch (boundary.type) {
            case BoundaryType::CONVERGENT:
                HandleConvergentBoundary(boundary);
                break;
            case BoundaryType::DIVERGENT:
                HandleDivergentBoundary(boundary);
                break;
            case BoundaryType::TRANSFORM:
                HandleTransformBoundary(boundary);
                break;
            case BoundaryType::PASSIVE:
                // No special terrain features for passive boundaries
                break;
        }
    }
    
    // Generate final terrain maps
    GenerateTerrainMaps();
}

TerrainType TectonicSimulator::GetTerrainTypeAtPosition(glm::vec2 worldPos) const {
    if (!simulationComplete_) {
        return TerrainType::STABLE;
    }
    
    return InterpolateTerrainType(worldPos);
}

float TectonicSimulator::GetTectonicStress(glm::vec2 worldPos) const {
    if (!simulationComplete_) {
        return 0.0f;
    }
    
    return InterpolateStress(worldPos);
}

float TectonicSimulator::GetElevationModifier(glm::vec2 worldPos) const {
    if (!simulationComplete_) {
        return 0.0f;
    }
    
    return InterpolateElevationModifier(worldPos);
}

const TectonicPlate* TectonicSimulator::GetDominantPlate(glm::vec2 worldPos) const {
    if (plates_.empty()) {
        return nullptr;
    }
    
    // First try the polygon-based containment for plates with proper boundaries
    for (const auto& plate : plates_) {
        if (plate.boundary.size() >= 3 && plate.ContainsPosition(worldPos)) {
            return &plate;
        }
    }
    
    // Fallback: find the closest plate by center distance (Voronoi cell approach)
    float minDistance = std::numeric_limits<float>::max();
    const TectonicPlate* closestPlate = nullptr;
    
    for (const auto& plate : plates_) {
        float distance = glm::length(worldPos - plate.centerPosition);
        if (distance < minDistance) {
            minDistance = distance;
            closestPlate = &plate;
        }
    }
    
    return closestPlate;
}

void TectonicSimulator::Reset() {
    plates_.clear();
    boundaries_.clear();
    stressMap_.clear();
    terrainMap_.clear();
    elevationMap_.clear();
    spatialGrid_.reset();
    simulationComplete_ = false;
    totalSimulationTime_ = 0.0f;
}

// Private implementation methods

void TectonicSimulator::GeneratePlates() {
    uint32_t plateCount = GetOptimalPlateCount(worldSize_);
    std::mt19937 rng(seed_);
    std::uniform_real_distribution<float> positionDist(0.0f, worldSize_);
    std::uniform_real_distribution<float> typeDist(0.0f, 1.0f);
    
    plates_.reserve(plateCount);
    
    // Generate plate centers using Poisson disk sampling for better distribution
    std::vector<glm::vec2> centers;
    float minDistance = worldSize_ / std::sqrt(plateCount * 2.0f); // Minimum distance between centers
    
    for (uint32_t attempt = 0; attempt < plateCount * 10 && centers.size() < plateCount; attempt++) {
        glm::vec2 candidate(positionDist(rng), positionDist(rng));
        
        bool validPosition = true;
        for (const auto& existing : centers) {
            if (glm::length(candidate - existing) < minDistance) {
                validPosition = false;
                break;
            }
        }
        
        if (validPosition) {
            centers.push_back(candidate);
        }
    }
    
    // Create plates from centers
    for (size_t i = 0; i < centers.size(); i++) {
        PlateType type;
        float typeRoll = typeDist(rng);
        
        if (typeRoll < 0.6f) {
            type = PlateType::CONTINENTAL;
        } else if (typeRoll < 0.9f) {
            type = PlateType::OCEANIC;
        } else {
            type = PlateType::MICROPLATE;
        }
        
        TectonicPlate plate(static_cast<uint32_t>(i), centers[i], type);
        plate.movementVector = GenerateTypicalMovement(type, seed_ + static_cast<uint32_t>(i));
        
        // Calculate plate area based on Voronoi cell size (approximation)
        float averageArea = (worldSize_ * worldSize_) / centers.size();
        std::uniform_real_distribution<float> areaDist(averageArea * 0.5f, averageArea * 1.5f);
        plate.area = areaDist(rng);
        
        plates_.push_back(plate);
    }
    
    // Generate Voronoi boundaries
    GenerateVoronoiBoundaries();
}

void TectonicSimulator::GenerateVoronoiBoundaries() {
    // Simple approach: generate boundary points by finding edge points of Voronoi cells
    uint32_t boundaryResolution = 32; // Resolution for boundary generation
    float step = worldSize_ / boundaryResolution;
    
    for (auto& plate : plates_) {
        plate.boundary.clear();
        
        // Find boundary points by checking neighborhood
        for (uint32_t y = 0; y < boundaryResolution; y++) {
            for (uint32_t x = 0; x < boundaryResolution; x++) {
                glm::vec2 testPos(x * step, y * step);
                
                // Find closest plate to this position
                float minDist = std::numeric_limits<float>::max();
                uint32_t closestPlate = 0;
                
                for (size_t i = 0; i < plates_.size(); i++) {
                    float dist = glm::length(testPos - plates_[i].centerPosition);
                    if (dist < minDist) {
                        minDist = dist;
                        closestPlate = static_cast<uint32_t>(i);
                    }
                }
                
                // If this point belongs to current plate, check if it's on boundary
                if (closestPlate == plate.plateId) {
                    bool onBoundary = false;
                    
                    // Check neighboring positions
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue;
                            
                            glm::vec2 neighborPos((x + dx) * step, (y + dy) * step);
                            if (neighborPos.x < 0 || neighborPos.x >= worldSize_ ||
                                neighborPos.y < 0 || neighborPos.y >= worldSize_) {
                                onBoundary = true;
                                break;
                            }
                            
                            // Find closest plate to neighbor
                            float neighborMinDist = std::numeric_limits<float>::max();
                            uint32_t neighborClosestPlate = 0;
                            
                            for (size_t i = 0; i < plates_.size(); i++) {
                                float dist = glm::length(neighborPos - plates_[i].centerPosition);
                                if (dist < neighborMinDist) {
                                    neighborMinDist = dist;
                                    neighborClosestPlate = static_cast<uint32_t>(i);
                                }
                            }
                            
                            if (neighborClosestPlate != closestPlate) {
                                onBoundary = true;
                                break;
                            }
                        }
                        if (onBoundary) break;
                    }
                    
                    if (onBoundary) {
                        plate.boundary.push_back(testPos);
                    }
                }
            }
        }
    }
}

void TectonicSimulator::BuildSpatialGrid() {
    const uint32_t gridSize = 16; // 16x16 grid for spatial optimization
    spatialGrid_ = std::make_unique<SpatialGrid>();
    spatialGrid_->gridWidth = gridSize;
    spatialGrid_->gridHeight = gridSize;
    spatialGrid_->cellSize = worldSize_ / gridSize;
    spatialGrid_->cells.assign(gridSize, std::vector<uint32_t>());
    
    for (uint32_t y = 0; y < gridSize; y++) {
        spatialGrid_->cells[y].assign(gridSize, UINT32_MAX);
        for (uint32_t x = 0; x < gridSize; x++) {
            glm::vec2 cellCenter((x + 0.5f) * spatialGrid_->cellSize, (y + 0.5f) * spatialGrid_->cellSize);
            
            // Find closest plate to cell center
            float minDist = std::numeric_limits<float>::max();
            uint32_t closestPlate = UINT32_MAX;
            
            for (const auto& plate : plates_) {
                float dist = glm::length(cellCenter - plate.centerPosition);
                if (dist < minDist) {
                    minDist = dist;
                    closestPlate = plate.plateId;
                }
            }
            
            spatialGrid_->cells[y][x] = closestPlate;
        }
    }
}

void TectonicSimulator::DetectPlateCollisions() {
    boundaries_.clear();
    
    // Check all plate pairs for interactions
    for (size_t i = 0; i < plates_.size(); i++) {
        for (size_t j = i + 1; j < plates_.size(); j++) {
            const auto& plate1 = plates_[i];
            const auto& plate2 = plates_[j];
            
            // Check if plates are close enough to interact
            float distance = glm::length(plate1.centerPosition - plate2.centerPosition);
            float interactionRadius = std::sqrt((plate1.area + plate2.area) / 3.14159f) * 1.2f; // Increase multiplier
            
            if (distance < interactionRadius) {
                BoundaryType boundaryType = ClassifyBoundaryType(plate1, plate2);
                PlateBoundary boundary(plate1.plateId, plate2.plateId, boundaryType);
                boundary.interactionStrength = CalculateInteractionStrength(plate1, plate2);
                boundaries_.push_back(boundary);
            }
        }
    }
}

BoundaryType TectonicSimulator::ClassifyBoundaryType(const TectonicPlate& plate1, const TectonicPlate& plate2) const {
    glm::vec2 relativePosition = plate2.centerPosition - plate1.centerPosition;
    glm::vec2 normalizedRelPos = glm::normalize(relativePosition);
    
    // Calculate relative movement vectors
    glm::vec2 relativeMovement = plate2.movementVector - plate1.movementVector;
    
    // Dot product with relative position indicates convergent/divergent
    float radialComponent = glm::dot(relativeMovement, normalizedRelPos);
    
    // Cross product magnitude indicates transform motion
    float tangentialComponent = std::abs(relativeMovement.x * normalizedRelPos.y - relativeMovement.y * normalizedRelPos.x);
    
    if (std::abs(radialComponent) > tangentialComponent) {
        return (radialComponent < 0) ? BoundaryType::CONVERGENT : BoundaryType::DIVERGENT;
    } else if (tangentialComponent > 1.0f) { // Threshold for significant transform motion
        return BoundaryType::TRANSFORM;
    } else {
        return BoundaryType::PASSIVE;
    }
}

float TectonicSimulator::CalculateInteractionStrength(const TectonicPlate& plate1, const TectonicPlate& plate2) const {
    float distance = glm::length(plate1.centerPosition - plate2.centerPosition);
    float relativeSpeed = glm::length(plate2.movementVector - plate1.movementVector);
    float maxInteractionDistance = std::sqrt((plate1.area + plate2.area) / 3.14159f);
    
    float distanceFactor = 1.0f - (distance / maxInteractionDistance);
    float speedFactor = relativeSpeed / 20.0f; // Normalize to typical plate speeds
    
    return std::clamp(distanceFactor * speedFactor, 0.0f, 1.0f);
}

void TectonicSimulator::HandleConvergentBoundary(PlateBoundary& boundary) {
    // Find the plates involved
    const TectonicPlate* plate1 = nullptr;
    const TectonicPlate* plate2 = nullptr;
    
    for (const auto& plate : plates_) {
        if (plate.plateId == boundary.plate1Id) plate1 = &plate;
        if (plate.plateId == boundary.plate2Id) plate2 = &plate;
    }
    
    if (!plate1 || !plate2) return;
    
    // Mountain formation at convergent boundaries
    boundary.stress += boundary.interactionStrength * timeStep_ * 0.1f;
    
    // Generate contact points for mountain ranges
    glm::vec2 midpoint = (plate1->centerPosition + plate2->centerPosition) * 0.5f;
    boundary.contactPoints.push_back(midpoint);
}

void TectonicSimulator::HandleDivergentBoundary(PlateBoundary& boundary) {
    // Rift formation at divergent boundaries
    boundary.stress += boundary.interactionStrength * timeStep_ * 0.05f; // Less stress than convergent
}

void TectonicSimulator::HandleTransformBoundary(PlateBoundary& boundary) {
    // Fault line formation at transform boundaries
    boundary.stress += boundary.interactionStrength * timeStep_ * 0.15f; // High stress, different pattern
}

void TectonicSimulator::CalculateStressAccumulation(float deltaTime) {
    // Update stress based on plate interactions
    for (auto& boundary : boundaries_) {
        float stressIncrease = boundary.interactionStrength * deltaTime * 0.1f;
        boundary.stress = std::min(boundary.stress + stressIncrease, 1.0f);
    }
}

void TectonicSimulator::UpdatePlatePositions(float deltaTime) {
    // Update plate positions based on movement vectors
    for (auto& plate : plates_) {
        // Convert movement from km/million years to km per timestep
        glm::vec2 movement = plate.movementVector * (deltaTime / 1000000.0f);
        plate.centerPosition += movement;
        
        // Keep plates within world bounds
        plate.centerPosition.x = std::clamp(plate.centerPosition.x, 0.0f, worldSize_);
        plate.centerPosition.y = std::clamp(plate.centerPosition.y, 0.0f, worldSize_);
    }
}

void TectonicSimulator::GenerateTerrainMaps() {
    float cellSize = worldSize_ / mapResolution_;
    
    for (uint32_t y = 0; y < mapResolution_; y++) {
        for (uint32_t x = 0; x < mapResolution_; x++) {
            glm::vec2 worldPos((x + 0.5f) * cellSize, (y + 0.5f) * cellSize);
            
            float maxStress = 0.0f;
            TerrainType terrainType = TerrainType::STABLE;
            float elevationMod = 0.0f;
            
            // Check all boundaries for influence at this position
            for (const auto& boundary : boundaries_) {
                for (const auto& contactPoint : boundary.contactPoints) {
                    float distance = glm::length(worldPos - contactPoint);
                    float influence = std::exp(-distance / (worldSize_ * 0.1f)); // Exponential falloff
                    
                    if (influence > 0.01f) { // Significant influence
                        float localStress = boundary.stress * influence;
                        if (localStress > maxStress) {
                            maxStress = localStress;
                            
                            switch (boundary.type) {
                                case BoundaryType::CONVERGENT:
                                    terrainType = TerrainType::MOUNTAIN;
                                    elevationMod = localStress * 2000.0f; // Up to 2km elevation
                                    break;
                                case BoundaryType::DIVERGENT:
                                    terrainType = TerrainType::RIFT;
                                    elevationMod = -localStress * 500.0f; // Down to -500m
                                    break;
                                case BoundaryType::TRANSFORM:
                                    terrainType = TerrainType::FAULT;
                                    elevationMod = localStress * 200.0f; // Small elevation changes
                                    break;
                                default:
                                    terrainType = TerrainType::STABLE;
                                    elevationMod = 0.0f;
                                    break;
                            }
                        }
                    }
                }
            }
            
            stressMap_[y][x] = maxStress;
            terrainMap_[y][x] = terrainType;
            elevationMap_[y][x] = elevationMod;
        }
    }
}

uint32_t TectonicSimulator::GetOptimalPlateCount(float worldSize) const {
    // Scale plate count with world size (roughly proportional to sqrt of area)
    float normalizedSize = worldSize / 1000.0f; // Normalize to 1000km reference
    uint32_t baseCount = static_cast<uint32_t>(std::sqrt(normalizedSize) * 15.0f);
    return std::clamp(baseCount, 5u, 50u); // Between 5 and 50 plates
}

float TectonicSimulator::InterpolateStress(glm::vec2 worldPos) const {
    float cellSize = worldSize_ / mapResolution_;
    float x = worldPos.x / cellSize;
    float y = worldPos.y / cellSize;
    
    uint32_t x0 = static_cast<uint32_t>(std::floor(x));
    uint32_t y0 = static_cast<uint32_t>(std::floor(y));
    uint32_t x1 = std::min(x0 + 1, mapResolution_ - 1);
    uint32_t y1 = std::min(y0 + 1, mapResolution_ - 1);
    
    float fx = x - x0;
    float fy = y - y0;
    
    // Bilinear interpolation
    float v00 = stressMap_[y0][x0];
    float v10 = stressMap_[y0][x1];
    float v01 = stressMap_[y1][x0];
    float v11 = stressMap_[y1][x1];
    
    float v0 = v00 * (1.0f - fx) + v10 * fx;
    float v1 = v01 * (1.0f - fx) + v11 * fx;
    
    return v0 * (1.0f - fy) + v1 * fy;
}

TerrainType TectonicSimulator::InterpolateTerrainType(glm::vec2 worldPos) const {
    float cellSize = worldSize_ / mapResolution_;
    uint32_t x = static_cast<uint32_t>(worldPos.x / cellSize);
    uint32_t y = static_cast<uint32_t>(worldPos.y / cellSize);
    
    x = std::min(x, mapResolution_ - 1);
    y = std::min(y, mapResolution_ - 1);
    
    return terrainMap_[y][x];
}

float TectonicSimulator::InterpolateElevationModifier(glm::vec2 worldPos) const {
    float cellSize = worldSize_ / mapResolution_;
    float x = worldPos.x / cellSize;
    float y = worldPos.y / cellSize;
    
    uint32_t x0 = static_cast<uint32_t>(std::floor(x));
    uint32_t y0 = static_cast<uint32_t>(std::floor(y));
    uint32_t x1 = std::min(x0 + 1, mapResolution_ - 1);
    uint32_t y1 = std::min(y0 + 1, mapResolution_ - 1);
    
    float fx = x - x0;
    float fy = y - y0;
    
    // Bilinear interpolation
    float v00 = elevationMap_[y0][x0];
    float v10 = elevationMap_[y0][x1];
    float v01 = elevationMap_[y1][x0];
    float v11 = elevationMap_[y1][x1];
    
    float v0 = v00 * (1.0f - fx) + v10 * fx;
    float v1 = v01 * (1.0f - fx) + v11 * fx;
    
    return v0 * (1.0f - fy) + v1 * fy;
}

} // namespace VoxelCastle
