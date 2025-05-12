#ifndef VOXELCASTLE_WORLD_MANAGER_H
#define VOXELCASTLE_WORLD_MANAGER_H

#include "world/chunk_column.h" // For ::VoxelCastle::World::ChunkColumn and ::VoxelCastle::World::ChunkColumnCoord
#include "world/voxel.h"      // For ::VoxelEngine::World::Voxel
#include <map>                // Changed from unordered_map to map
#include "world/quadtree.h"   // For Quadtree spatial partitioning
#include <memory>
#include <cstdint>            // For int_fast64_t

/**
 * @brief Namespace for Voxel Castle specific game logic and world representation.
 */
namespace VoxelCastle {
/**
 * @brief Namespace for world-related structures and management within the Voxel Castle context.
 */
namespace World {

/**
 * @struct WorldCoordXZ
 * @brief Represents the XZ coordinates in the world, used as a key for storing ChunkColumns.
 *
 * This struct is used to uniquely identify a ChunkColumn based on its horizontal position.
 * It includes comparison operators to allow its use as a key in std::map.
 */
struct WorldCoordXZ {
    int_fast64_t x;
    int_fast64_t z;

    /**
     * @brief Comparison operator for ordering WorldCoordXZ, enabling use in std::map.
     * @param other The other WorldCoordXZ to compare against.
     * @return True if this coordinate is less than the other, false otherwise.
     */
    bool operator<(const WorldCoordXZ& other) const {
        if (x != other.x) {
            return x < other.x;
        }
        return z < other.z;
    }
};

/**
 * @class WorldManager
 * @brief Manages the overall voxel world, including loading, accessing, and modifying ChunkColumns.
 *
 * The WorldManager is the primary interface for interacting with the game world's voxel data.
 * It handles the creation and storage of ChunkColumns and provides methods to get/set individual voxels
 * at world coordinates.
 */
class WorldManager {
public:
    /**
     * @brief Default constructor for the WorldManager.
     */
    WorldManager();

    /**
     * @brief Gets the voxel at the specified world coordinates.
     *
     * This method determines the correct ChunkColumn and ChunkSegment for the given coordinates
     * and retrieves the voxel data.
     * @param worldX The global X-coordinate of the voxel.
     * @param worldY The global Y-coordinate of the voxel.
     * @param worldZ The global Z-coordinate of the voxel.
     * @return The VoxelEngine::World::Voxel at the specified location. If the location is in an
     *         unloaded or non-existent chunk, a default voxel (e.g., AIR) is returned.
     */
    ::VoxelEngine::World::Voxel getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const;

    /**
     * @brief Sets the voxel at the specified world coordinates.
     *
     * This method determines the correct ChunkColumn and ChunkSegment for the given coordinates
     * and updates the voxel data. It will create the necessary ChunkColumn and ChunkSegment
     * if they do not already exist.
     * @param worldX The global X-coordinate of the voxel.
     * @param worldY The global Y-coordinate of the voxel.
     * @param worldZ The global Z-coordinate of the voxel.
     * @param voxel The VoxelEngine::World::Voxel data to set at the specified location.
     */
    void setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const ::VoxelEngine::World::Voxel& voxel);

    /**
     * @brief Retrieves an existing ChunkColumn at the given XZ coordinates.
     *
     * This method retrieves a ChunkColumn if it exists at the specified coordinates.
     * @param worldX The global X-coordinate of the ChunkColumn.
     * @param worldZ The global Z-coordinate of the ChunkColumn.
     * @return A pointer to the ChunkColumn at the specified coordinates, or nullptr if it doesn't exist.
     */
    ::VoxelCastle::World::ChunkColumn* getChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);
    const ::VoxelCastle::World::ChunkColumn* getChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) const;

    /**
     * @brief Retrieves an existing ChunkColumn or creates a new one if it doesn't exist at the given XZ coordinates.
     * @param worldX The global X-coordinate of the ChunkColumn.
     * @param worldZ The global Z-coordinate of the ChunkColumn.
     * @return A pointer to the ChunkColumn at the specified coordinates.
     */
    ::VoxelCastle::World::ChunkColumn* getOrCreateChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);

    /**
     * @brief Converts global world X coordinate to ChunkColumn base X coordinate.
     * @param worldX The global X-coordinate.
     * @return The base X-coordinate of the ChunkColumn.
     */
    static int_fast64_t worldToColumnBaseX(int_fast64_t worldX);

    /**
     * @brief Converts global world Z coordinate to ChunkColumn base Z coordinate.
     * @param worldZ The global Z-coordinate.
     * @return The base Z-coordinate of the ChunkColumn.
     */
    static int_fast64_t worldToColumnBaseZ(int_fast64_t worldZ);

    /**
     * @brief Map storing all active ChunkColumns, keyed by their XZ world coordinates.
     */
    std::map< ::VoxelCastle::World::ChunkColumnCoord, std::unique_ptr< ::VoxelCastle::World::ChunkColumn>> m_chunkColumns;

    /**
     * @brief Quadtree for spatial partitioning of ChunkColumns (XZ plane).
     */
    std::unique_ptr<world::Quadtree> m_chunkQuadtree;

public:
    /**
     * @brief Query all ChunkColumns in a given XZ region (inclusive).
     * @param xMin Minimum X of region.
     * @param zMin Minimum Z of region.
     * @param xMax Maximum X of region.
     * @param zMax Maximum Z of region.
     * @return Vector of pointers to ChunkColumns in the region.
     */
    std::vector<ChunkColumn*> queryChunkColumnsInRegion(int32_t xMin, int32_t zMin, int32_t xMax, int32_t zMax) const;
};

} // namespace World
} // namespace VoxelCastle

#endif // VOXELCASTLE_WORLD_MANAGER_H
