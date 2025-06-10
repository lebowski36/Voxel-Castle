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
    std::cout << "[TectonicSimulator] InitializePlates: seed=" << seed << ", worldSize=" << worldSize << ", plateCount=" << plateCount << std::endl;
    
    seed_ = seed;
    worldSize_ = worldSize;
    simulationComplete_ = false;
    totalSimulationTime_ = 0.0f;
    
    // Clear existing data
    plates_.clear();
    boundaries_.clear();
    std::cout << "[TectonicSimulator] Cleared existing data" << std::endl;
    
    // Determine plate count if not specified
    if (plateCount == 0) {
        plateCount = GetOptimalPlateCount(worldSize);
        std::cout << "[TectonicSimulator] Auto-determined plate count: " << plateCount << std::endl;
    }
    
    // Generate initial plates
    std::cout << "[TectonicSimulator] Generating plates..." << std::endl;
    GeneratePlates();
    std::cout << "[TectonicSimulator] Generated " << plates_.size() << " plates" << std::endl;
    
    // Create spatial grid for optimization
    std::cout << "[TectonicSimulator] Building spatial grid..." << std::endl;
    BuildSpatialGrid();
    std::cout << "[TectonicSimulator] Spatial grid built" << std::endl;
    
    // Initialize stress and terrain maps
    std::cout << "[TectonicSimulator] Initializing stress map (" << mapResolution_ << "x" << mapResolution_ << ")..." << std::endl;
    stressMap_.assign(mapResolution_, std::vector<float>(mapResolution_, 0.0f));
    std::cout << "[TectonicSimulator] Stress map initialized" << std::endl;
    
    std::cout << "[TectonicSimulator] Initializing terrain map..." << std::endl;
    terrainMap_.assign(mapResolution_, std::vector<TerrainType>(mapResolution_, TerrainType::STABLE));
    std::cout << "[TectonicSimulator] Terrain map initialized" << std::endl;
    
    std::cout << "[TectonicSimulator] Initializing elevation map..." << std::endl;
    elevationMap_.assign(mapResolution_, std::vector<float>(mapResolution_, 0.0f));
    std::cout << "[TectonicSimulator] Elevation map initialized" << std::endl;
    
    std::cout << "[TectonicSimulator] InitializePlates complete" << std::endl;
}

void TectonicSimulator::SimulatePlateMovement(float simulationTime, uint32_t timeSteps) {
    std::cout << "[TectonicSimulator] Starting simulation with " << plates_.size() << " plates" << std::endl;
    
    if (plates_.empty()) {
        std::cout << "[TectonicSimulator] No plates to simulate" << std::endl;
        return;
    }
    
    std::cout << "[TectonicSimulator] Calculating deltaTime: " << simulationTime << " / " << timeSteps << std::endl;
    float deltaTime = simulationTime / static_cast<float>(timeSteps);
    timeStep_ = deltaTime;
    std::cout << "[TectonicSimulator] Delta time: " << deltaTime << std::endl;
    
    for (uint32_t step = 0; step < timeSteps; step++) {
        std::cout << "[TectonicSimulator] Step " << step << " / " << timeSteps << std::endl;
        
        // Detect plate collisions and update boundaries
        std::cout << "[TectonicSimulator] Detecting plate collisions..." << std::endl;
        DetectPlateCollisions();
        std::cout << "[TectonicSimulator] Plate collisions detected" << std::endl;
        
        // Calculate stress accumulation
        std::cout << "[TectonicSimulator] Calculating stress accumulation..." << std::endl;
        CalculateStressAccumulation(deltaTime);
        std::cout << "[TectonicSimulator] Stress accumulation calculated" << std::endl;
        
        // Update plate positions based on movement
        std::cout << "[TectonicSimulator] Updating plate positions..." << std::endl;
        UpdatePlatePositions(deltaTime);
        std::cout << "[TectonicSimulator] Plate positions updated" << std::endl;
        
        // Rebuild spatial grid if plates moved significantly
        if (step % 5 == 0) { // Rebuild every 5 steps
            std::cout << "[TectonicSimulator] Rebuilding spatial grid..." << std::endl;
            BuildSpatialGrid();
            std::cout << "[TectonicSimulator] Spatial grid rebuilt" << std::endl;
        }
    }
    
    std::cout << "[TectonicSimulator] Simulation complete" << std::endl;
    
    // Mark simulation as complete BEFORE generating boundary features
    simulationComplete_ = true;
    
    // Generate terrain features and elevation maps based on boundary interactions
    std::cout << "[TectonicSimulator] Generating boundary features and terrain maps..." << std::endl;
    GenerateBoundaryFeatures();
    std::cout << "[TectonicSimulator] Boundary features and terrain maps generated" << std::endl;
    
    totalSimulationTime_ += simulationTime;
}

void TectonicSimulator::GenerateBoundaryFeatures() {
    if (!simulationComplete_) {
        std::cout << "[TectonicSimulator] GenerateBoundaryFeatures: Simulation not complete yet, skipping" << std::endl;
        return;
    }
    
    std::cout << "[TectonicSimulator] GenerateBoundaryFeatures: Processing " << boundaries_.size() << " boundaries" << std::endl;
    
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
    
    std::cout << "[TectonicSimulator] GenerateBoundaryFeatures: Completed boundary processing, calling GenerateTerrainMaps" << std::endl;
    
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
    
    std::cout << "[TectonicSimulator] DetectPlateCollisions: Checking " << plates_.size() << " plates for interactions..." << std::endl;
    
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
                
                // Debug for first few boundaries
                if (boundaries_.size() <= 3) {
                    std::cout << "[TectonicSimulator] Boundary " << boundaries_.size() 
                              << ": plates " << plate1.plateId << "-" << plate2.plateId
                              << " distance=" << distance << " interactionRadius=" << interactionRadius
                              << " type=" << static_cast<int>(boundaryType) << std::endl;
                }
            }
        }
    }
    
    std::cout << "[TectonicSimulator] DetectPlateCollisions: Created " << boundaries_.size() << " boundaries" << std::endl;
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
    
    // Generate multiple contact points along the boundary line for mountain ranges
    glm::vec2 midpoint = (plate1->centerPosition + plate2->centerPosition) * 0.5f;
    glm::vec2 direction = glm::normalize(plate2->centerPosition - plate1->centerPosition);
    glm::vec2 perpendicular = glm::vec2(-direction.y, direction.x);
    
    // Create a line of contact points perpendicular to the plate connection
    float boundaryLength = std::sqrt((plate1->area + plate2->area) / 3.14159f) * 0.8f;
    int numPoints = static_cast<int>(boundaryLength / 50.0f) + 3; // At least 3 points
    
    boundary.contactPoints.clear(); // Clear existing points
    for (int i = 0; i < numPoints; i++) {
        float offset = (i - numPoints/2.0f) * (boundaryLength / numPoints);
        glm::vec2 contactPoint = midpoint + perpendicular * offset;
        boundary.contactPoints.push_back(contactPoint);
    }
    
    // Debug output
    static int convergentCallCount = 0;
    if (convergentCallCount < 2) {
        std::cout << "[TectonicSimulator] HandleConvergentBoundary: Added " << numPoints 
                  << " contact points for plates " << boundary.plate1Id << "-" << boundary.plate2Id
                  << " at midpoint (" << midpoint.x << "," << midpoint.y << ")"
                  << " stress=" << boundary.stress << std::endl;
        convergentCallCount++;
    }
}

void TectonicSimulator::HandleDivergentBoundary(PlateBoundary& boundary) {
    // Find the plates involved
    const TectonicPlate* plate1 = nullptr;
    const TectonicPlate* plate2 = nullptr;
    
    for (const auto& plate : plates_) {
        if (plate.plateId == boundary.plate1Id) plate1 = &plate;
        if (plate.plateId == boundary.plate2Id) plate2 = &plate;
    }
    
    if (!plate1 || !plate2) return;
    
    // Rift formation at divergent boundaries
    boundary.stress += boundary.interactionStrength * timeStep_ * 0.05f; // Less stress than convergent
    
    // Generate multiple contact points along the rift valley
    glm::vec2 midpoint = (plate1->centerPosition + plate2->centerPosition) * 0.5f;
    glm::vec2 direction = glm::normalize(plate2->centerPosition - plate1->centerPosition);
    glm::vec2 perpendicular = glm::vec2(-direction.y, direction.x);
    
    // Create a line of contact points for the rift valley
    float riftLength = std::sqrt((plate1->area + plate2->area) / 3.14159f) * 0.6f;
    int numPoints = static_cast<int>(riftLength / 60.0f) + 2; // At least 2 points
    
    boundary.contactPoints.clear(); // Clear existing points
    for (int i = 0; i < numPoints; i++) {
        float offset = (i - numPoints/2.0f) * (riftLength / numPoints);
        glm::vec2 contactPoint = midpoint + perpendicular * offset;
        boundary.contactPoints.push_back(contactPoint);
    }
}

void TectonicSimulator::HandleTransformBoundary(PlateBoundary& boundary) {
    // Find the plates involved
    const TectonicPlate* plate1 = nullptr;
    const TectonicPlate* plate2 = nullptr;
    
    for (const auto& plate : plates_) {
        if (plate.plateId == boundary.plate1Id) plate1 = &plate;
        if (plate.plateId == boundary.plate2Id) plate2 = &plate;
    }
    
    if (!plate1 || !plate2) return;
    
    // Fault line formation at transform boundaries
    boundary.stress += boundary.interactionStrength * timeStep_ * 0.15f; // High stress, different pattern
    
    // Generate multiple contact points along the fault line
    glm::vec2 midpoint = (plate1->centerPosition + plate2->centerPosition) * 0.5f;
    glm::vec2 direction = glm::normalize(plate2->centerPosition - plate1->centerPosition);
    glm::vec2 perpendicular = glm::vec2(-direction.y, direction.x);
    
    // Create a line of contact points for the fault line
    float faultLength = std::sqrt((plate1->area + plate2->area) / 3.14159f) * 0.7f;
    int numPoints = static_cast<int>(faultLength / 70.0f) + 2; // At least 2 points
    
    boundary.contactPoints.clear(); // Clear existing points
    for (int i = 0; i < numPoints; i++) {
        float offset = (i - numPoints/2.0f) * (faultLength / numPoints);
        glm::vec2 contactPoint = midpoint + perpendicular * offset;
        boundary.contactPoints.push_back(contactPoint);
    }
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
    
    std::cout << "[TectonicSimulator] GenerateTerrainMaps - Processing " << boundaries_.size() << " boundaries" << std::endl;
    
    // Debug: check boundary data
    int totalContactPoints = 0;
    for (size_t i = 0; i < boundaries_.size() && i < 3; i++) {
        const auto& boundary = boundaries_[i];
        totalContactPoints += boundary.contactPoints.size();
        std::cout << "[TectonicSimulator] Boundary " << i << ": stress=" << boundary.stress 
                  << " contactPoints=" << boundary.contactPoints.size() 
                  << " type=" << static_cast<int>(boundary.type) 
                  << " interactionStrength=" << boundary.interactionStrength << std::endl;
    }
    std::cout << "[TectonicSimulator] Total contact points across all boundaries: " << totalContactPoints << std::endl;
    
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
    // Check if position is outside world bounds - return default stress
    if (worldPos.x < 0.0f || worldPos.y < 0.0f || 
        worldPos.x >= worldSize_ || worldPos.y >= worldSize_) {
        return 0.0f; // Default stress for out-of-bounds positions
    }
    
    float cellSize = worldSize_ / mapResolution_;
    float x = worldPos.x / cellSize;
    float y = worldPos.y / cellSize;
    
    uint32_t x0 = static_cast<uint32_t>(std::floor(x));
    uint32_t y0 = static_cast<uint32_t>(std::floor(y));
    uint32_t x1 = std::min(x0 + 1, mapResolution_ - 1);
    uint32_t y1 = std::min(y0 + 1, mapResolution_ - 1);
    
    // Additional safety check - ensure indices are within bounds
    x0 = std::min(x0, mapResolution_ - 1);
    y0 = std::min(y0, mapResolution_ - 1);
    
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
    // Check if position is outside world bounds - return default terrain type
    if (worldPos.x < 0.0f || worldPos.y < 0.0f || 
        worldPos.x >= worldSize_ || worldPos.y >= worldSize_) {
        return TerrainType::STABLE; // Default terrain type for out-of-bounds positions
    }
    
    float cellSize = worldSize_ / mapResolution_;
    uint32_t x = static_cast<uint32_t>(worldPos.x / cellSize);
    uint32_t y = static_cast<uint32_t>(worldPos.y / cellSize);
    
    // Ensure indices are within bounds
    x = std::min(x, mapResolution_ - 1);
    y = std::min(y, mapResolution_ - 1);
    
    return terrainMap_[y][x];
}

float TectonicSimulator::InterpolateElevationModifier(glm::vec2 worldPos) const {
    // Check if position is outside world bounds - return default elevation modifier
    if (worldPos.x < 0.0f || worldPos.y < 0.0f || 
        worldPos.x >= worldSize_ || worldPos.y >= worldSize_) {
        std::cout << "[TectonicSimulator] Position (" << worldPos.x << "," << worldPos.y 
                  << ") outside bounds [0," << worldSize_ << "] - returning 0" << std::endl;
        return 0.0f; // Default elevation modifier for out-of-bounds positions
    }
    
    float cellSize = worldSize_ / mapResolution_;
    float x = worldPos.x / cellSize;
    float y = worldPos.y / cellSize;
    
    uint32_t x0 = static_cast<uint32_t>(std::floor(x));
    uint32_t y0 = static_cast<uint32_t>(std::floor(y));
    uint32_t x1 = std::min(x0 + 1, mapResolution_ - 1);
    uint32_t y1 = std::min(y0 + 1, mapResolution_ - 1);
    
    // Additional safety check - ensure indices are within bounds
    x0 = std::min(x0, mapResolution_ - 1);
    y0 = std::min(y0, mapResolution_ - 1);
    
    float fx = x - x0;
    float fy = y - y0;
    
    // Bilinear interpolation
    float v00 = elevationMap_[y0][x0];
    float v10 = elevationMap_[y0][x1];
    float v01 = elevationMap_[y1][x0];
    float v11 = elevationMap_[y1][x1];
    
    // Debug: log elevation map values for first few calls
    static int debugCount = 0;
    if (debugCount < 5) {
        std::cout << "[TectonicSimulator] ElevationModifier debug - Pos(" << worldPos.x << "," << worldPos.y 
                  << ") cellSize:" << cellSize << " grid(" << x0 << "," << y0 << ")" << std::endl;
        std::cout << "[TectonicSimulator] Elevation values: v00=" << v00 << " v10=" << v10 
                  << " v01=" << v01 << " v11=" << v11 << std::endl;
        debugCount++;
    }
    
    float v0 = v00 * (1.0f - fx) + v10 * fx;
    float v1 = v01 * (1.0f - fx) + v11 * fx;
    
    return v0 * (1.0f - fy) + v1 * fy;
}

} // namespace VoxelCastle
