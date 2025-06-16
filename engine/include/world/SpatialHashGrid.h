#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include <functional>

namespace VoxelCastle {
namespace World {

/**
 * @brief Spatial hash grid for efficient particle neighbor queries
 * 
 * This class provides O(1) insertion and O(k) neighbor queries where k is
 * the average number of particles per grid cell. Significantly faster than
 * O(n) brute force searches for large particle counts.
 */
class SpatialHashGrid {
public:
    /**
     * @brief Constructor
     * @param worldSizeM World size in meters
     * @param cellSize Size of each grid cell in meters
     */
    SpatialHashGrid(float worldSizeM, float cellSize = 5000.0f);
    
    /**
     * @brief Clear all particles from the grid
     */
    void Clear();
    
    /**
     * @brief Insert particle into the spatial grid
     * @param particleIndex Index of particle in main particle array
     * @param position World position of particle
     */
    void Insert(size_t particleIndex, const glm::vec2& position);
    
    /**
     * @brief Find all particles within radius of a point
     * @param position Center position for search
     * @param radius Search radius in meters
     * @return Vector of particle indices within radius
     */
    std::vector<size_t> FindInRadius(const glm::vec2& position, float radius) const;
    
    /**
     * @brief Find all particles in grid cells overlapping a radius
     * @param position Center position for search
     * @param radius Search radius in meters
     * @return Vector of particle indices in relevant grid cells
     */
    std::vector<size_t> FindInRadiusApproximate(const glm::vec2& position, float radius) const;
    
    /**
     * @brief Get all particles in a specific grid cell
     * @param gridX Grid cell X coordinate
     * @param gridZ Grid cell Z coordinate
     * @return Vector of particle indices in that cell
     */
    std::vector<size_t> GetCellParticles(int gridX, int gridZ) const;
    
    /**
     * @brief Get grid dimensions
     */
    int GetGridWidth() const { return gridWidth_; }
    int GetGridHeight() const { return gridHeight_; }
    float GetCellSize() const { return cellSize_; }
    
    /**
     * @brief Convert world position to grid coordinates
     */
    std::pair<int, int> WorldToGrid(const glm::vec2& position) const;
    
    /**
     * @brief Convert grid coordinates to world position (cell center)
     */
    glm::vec2 GridToWorld(int gridX, int gridZ) const;

private:
    float worldSizeM_;              // World size in meters
    float cellSize_;                // Size of each grid cell
    int gridWidth_;                 // Number of cells in X direction
    int gridHeight_;                // Number of cells in Z direction
    
    // Hash table mapping grid cell coordinates to particle lists
    // Key: (gridX * gridHeight_ + gridZ), Value: list of particle indices
    std::unordered_map<int, std::vector<size_t>> grid_;
    
    /**
     * @brief Convert grid coordinates to hash key
     */
    int GridToKey(int gridX, int gridZ) const;
    
    /**
     * @brief Check if grid coordinates are valid
     */
    bool IsValidGridCoord(int gridX, int gridZ) const;
    
    /**
     * @brief Get all grid cells that overlap with a circular area
     */
    std::vector<std::pair<int, int>> GetOverlappingCells(const glm::vec2& center, float radius) const;
};

} // namespace World
} // namespace VoxelCastle
