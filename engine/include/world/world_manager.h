#ifndef VOXELCASTLE_WORLD_MANAGER_H
#define VOXELCASTLE_WORLD_MANAGER_H


// Standard library includes first

#include <map>
#include <memory>
#include <cstdint>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <glm/vec3.hpp> // For glm::vec3


#include "world/chunk_column.h" // For ::VoxelCastle::World::ChunkColumn and ::VoxelCastle::World::ChunkColumnCoord
#include "world/voxel.h"      // For ::VoxelEngine::World::Voxel
#include "world/quadtree.h"   // For Quadtree spatial partitioning
#include "world/world_generator.h" // For WorldGenerator
#include "rendering/texture_atlas.h" // For VoxelEngine::Rendering::TextureAtlas
#include "rendering/mesh_builder.h"  // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/voxel_mesh.h" // For VoxelEngine::Rendering::VoxelMesh
#include "rendering/mesh_job_system.h" // For VoxelEngine::Rendering::MeshJobSystem


namespace VoxelCastle {
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
 * at world coordinates. It also manages the mesh generation for dirty chunk segments.
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
     * @return The ::VoxelEngine::World::Voxel at the specified location. If the location is in an
     *         unloaded or non-existent chunk, a default voxel (e.g., AIR) is returned.
     */
    ::VoxelEngine::World::Voxel getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const;

    /**
     * @brief Sets the voxel at the specified world coordinates.
     *
     * This method determines the correct ChunkColumn and ChunkSegment for the given coordinates
     * and updates the voxel data. It will create the necessary ChunkColumn and ChunkSegment
     * if they do not already exist. The affected ChunkSegment will be marked as dirty.
     * @param worldX The global X-coordinate of the voxel.
     * @param worldY The global Y-coordinate of the voxel.
     * @param worldZ The global Z-coordinate of the voxel.
     * @param voxel The ::VoxelEngine::World::Voxel data to set at the specified location.
     */
    void setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const ::VoxelEngine::World::Voxel& voxel);

    /**
     * @brief Sets the voxel at the specified world coordinates using a VoxelType.
     *
     * This method determines the correct ChunkColumn and ChunkSegment for the given coordinates
     * and updates the voxel data. It will create the necessary ChunkColumn and ChunkSegment
     * if they do not already exist. The affected ChunkSegment will be marked as dirty.
     * A Voxel object will be created from the provided VoxelType.
     * @param worldX The global X-coordinate of the voxel.
     * @param worldY The global Y-coordinate of the voxel.
     * @param worldZ The global Z-coordinate of the voxel.
     * @param type The ::VoxelEngine::World::VoxelType to set at the specified location.
     */
    void setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, ::VoxelEngine::World::VoxelType type);

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
     * If a new ChunkColumn is created, its segments are initialized with a checkerboard pattern.
     * @param worldX The global X-coordinate of the ChunkColumn.
     * @param worldZ The global Z-coordinate of the ChunkColumn.
     * @return A pointer to the ChunkColumn at the specified coordinates.
     */
    ::VoxelCastle::World::ChunkColumn* getOrCreateChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);

    /**
     * @brief Iterates through all chunk segments and rebuilds their meshes if they are marked dirty.
     * @param atlas The texture atlas to use for mesh generation.
     * @param meshBuilder The mesh builder to use for generating mesh data.
     */
    void updateDirtyMeshes(::VoxelEngine::Rendering::TextureAtlas& atlas, ::VoxelEngine::Rendering::MeshBuilder& meshBuilder);

    // New: Asynchronous mesh update (enqueues jobs, returns immediately)
    void enqueueDirtyMeshJobs(::VoxelEngine::Rendering::TextureAtlas& atlas, ::VoxelEngine::Rendering::MeshBuilder& meshBuilder);

    // New: Called on main thread to upload finished meshes
    void processFinishedMeshJobs();

    /**
     * @brief Retrieves all valid VoxelMesh pointers from all chunk segments.
     * @return A vector of const pointers to VoxelMesh objects.
     */
    std::vector<const ::VoxelEngine::Rendering::VoxelMesh*> getAllSegmentMeshes() const;

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
    std::map<::VoxelCastle::World::WorldCoordXZ, std::shared_ptr<::VoxelCastle::World::ChunkColumn>> m_chunkColumns;

    /**
     * @brief Quadtree for spatial partitioning of ChunkColumns (XZ plane).
     */
    std::unique_ptr<::world::Quadtree> m_chunkQuadtree;

    /**
     * @brief Data structure for mesh job data.
     * Contains information needed to process mesh generation jobs.
     */
    struct MeshJobData {
        std::shared_ptr<VoxelCastle::World::ChunkColumn> column;
        VoxelCastle::World::ChunkSegment* segment;
        std::unique_ptr<::VoxelEngine::Rendering::VoxelMesh> mesh;
    };

    /**
     * @brief Queue of mesh jobs waiting to be processed by worker threads.
     * Protected by m_meshJobMutex.
     */
    std::queue<MeshJobData> m_pendingMeshJobs;

    /**
     * @brief Queue of completed mesh jobs ready for main thread upload.
     * Protected by m_meshJobMutex.
     */
    std::vector<MeshJobData> m_completedMeshJobs;

    /**
     * @brief Mutex to protect access to mesh job queues.
     */
    std::mutex m_meshJobMutex;

    /**
     * @brief Thread pool for processing mesh generation jobs.
     */
    std::unique_ptr<::VoxelEngine::Rendering::MeshJobSystem> m_meshJobSystem;

public:
    /**
     * @brief Query all ChunkColumns in a given XZ region (inclusive).
     * @param xMin Minimum X of region.
     * @param zMin Minimum Z of region.
     * @param xMax Maximum X of region.
     * @param zMax Maximum Z of region.
     * @return Vector of pointers to ChunkColumns in the region.
     */
    std::vector<::VoxelCastle::World::ChunkColumn*> queryChunkColumnsInRegion(int32_t xMin, int32_t zMin, int32_t xMax, int32_t zMax) const;

    /**
     * @brief Updates active chunks based on the center world position and load radius.
     * @param centerWorldPosition The center position in the world.
     * @param loadRadiusInSegments The radius in segments to load around the center.
     * @param generator The world generator to use for creating new chunks.
     */
    void updateActiveChunks(const glm::vec3& centerWorldPosition, int loadRadiusInSegments, WorldGenerator& generator);

    /**
     * @brief Marks all segments in all chunk columns as dirty.
     * This is useful when a global change (like debug mode switch) requires all meshes to be rebuilt.
     */
    void markAllSegmentsDirty();

private:
    // Helper to convert world coordinates to ChunkColumn base coordinates
};

} // namespace World
} // namespace VoxelCastle

#endif // VOXELCASTLE_WORLD_MANAGER_H
