#include "world/SpatialHashGrid.h"
#include <algorithm>
#include <cmath>

namespace VoxelCastle {
namespace World {

SpatialHashGrid::SpatialHashGrid(float worldSizeM, float cellSize)
    : worldSizeM_(worldSizeM)
    , cellSize_(cellSize)
    , gridWidth_(static_cast<int>(std::ceil(worldSizeM / cellSize)))
    , gridHeight_(static_cast<int>(std::ceil(worldSizeM / cellSize))) {
    
    // Reserve space for efficiency
    grid_.reserve(gridWidth_ * gridHeight_ / 4); // Estimate 25% occupancy
}

void SpatialHashGrid::Clear() {
    grid_.clear();
}

void SpatialHashGrid::Insert(size_t particleIndex, const glm::vec2& position) {
    auto [gridX, gridZ] = WorldToGrid(position);
    
    if (IsValidGridCoord(gridX, gridZ)) {
        int key = GridToKey(gridX, gridZ);
        grid_[key].push_back(particleIndex);
    }
}

std::vector<size_t> SpatialHashGrid::FindInRadius(const glm::vec2& position, float radius) const {
    std::vector<size_t> result;
    
    // Get all particles in overlapping grid cells
    auto overlappingCells = GetOverlappingCells(position, radius);
    
    for (const auto& [gridX, gridZ] : overlappingCells) {
        int key = GridToKey(gridX, gridZ);
        auto it = grid_.find(key);
        
        if (it != grid_.end()) {
            for (size_t particleIndex : it->second) {
                result.push_back(particleIndex);
            }
        }
    }
    
    // Remove duplicates that might occur at cell boundaries
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    
    return result;
}

std::vector<size_t> SpatialHashGrid::FindInRadiusApproximate(const glm::vec2& position, float radius) const {
    // Faster approximate search - just returns all particles in overlapping cells
    // without distance checking. Caller must filter by actual distance if needed.
    return FindInRadius(position, radius);
}

std::vector<size_t> SpatialHashGrid::GetCellParticles(int gridX, int gridZ) const {
    if (!IsValidGridCoord(gridX, gridZ)) {
        return {};
    }
    
    int key = GridToKey(gridX, gridZ);
    auto it = grid_.find(key);
    
    return (it != grid_.end()) ? it->second : std::vector<size_t>{};
}

std::pair<int, int> SpatialHashGrid::WorldToGrid(const glm::vec2& position) const {
    int gridX = static_cast<int>(std::floor(position.x / cellSize_));
    int gridZ = static_cast<int>(std::floor(position.y / cellSize_));
    
    // Clamp to valid grid range
    gridX = std::max(0, std::min(gridWidth_ - 1, gridX));
    gridZ = std::max(0, std::min(gridHeight_ - 1, gridZ));
    
    return {gridX, gridZ};
}

glm::vec2 SpatialHashGrid::GridToWorld(int gridX, int gridZ) const {
    return glm::vec2(
        (gridX + 0.5f) * cellSize_,
        (gridZ + 0.5f) * cellSize_
    );
}

int SpatialHashGrid::GridToKey(int gridX, int gridZ) const {
    return gridX * gridHeight_ + gridZ;
}

bool SpatialHashGrid::IsValidGridCoord(int gridX, int gridZ) const {
    return gridX >= 0 && gridX < gridWidth_ &&
           gridZ >= 0 && gridZ < gridHeight_;
}

std::vector<std::pair<int, int>> SpatialHashGrid::GetOverlappingCells(const glm::vec2& center, float radius) const {
    std::vector<std::pair<int, int>> cells;
    
    // Calculate grid bounds that overlap with the circular area
    int minGridX = static_cast<int>(std::floor((center.x - radius) / cellSize_));
    int maxGridX = static_cast<int>(std::ceil((center.x + radius) / cellSize_));
    int minGridZ = static_cast<int>(std::floor((center.y - radius) / cellSize_));
    int maxGridZ = static_cast<int>(std::ceil((center.y + radius) / cellSize_));
    
    // Clamp to valid grid range
    minGridX = std::max(0, minGridX);
    maxGridX = std::min(gridWidth_ - 1, maxGridX);
    minGridZ = std::max(0, minGridZ);
    maxGridZ = std::min(gridHeight_ - 1, maxGridZ);
    
    // Add all cells in the bounding rectangle
    for (int gridX = minGridX; gridX <= maxGridX; ++gridX) {
        for (int gridZ = minGridZ; gridZ <= maxGridZ; ++gridZ) {
            cells.emplace_back(gridX, gridZ);
        }
    }
    
    return cells;
}

} // namespace World
} // namespace VoxelCastle
