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
#include <unordered_set>
#include <unordered_map>
#include <chrono>
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

    /**
     * @brief Equality operator for WorldCoordXZ, enabling use in std::unordered_set and std::unordered_map.
     * @param other The other WorldCoordXZ to compare against.
     * @return True if coordinates are equal, false otherwise.
     */
    bool operator==(const WorldCoordXZ& other) const {
        return x == other.x && z == other.z;
    }
};

/**
 * @struct WorldCoordXZHash
 * @brief Hash function for WorldCoordXZ to enable use in unordered containers.
 */
struct WorldCoordXZHash {
    std::size_t operator()(const WorldCoordXZ& coord) const {
        return std::hash<int_fast64_t>()(coord.x) ^ 
               (std::hash<int_fast64_t>()(coord.z) << 1);
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
     * @brief Retrieves an existing ChunkColumn or creates a new empty one without world generation.
     * This method is specifically for loading chunks from save files where world generation should be skipped.
     * @param worldX The global X-coordinate of the ChunkColumn.
     * @param worldZ The global Z-coordinate of the ChunkColumn.
     * @return A pointer to the ChunkColumn at the specified coordinates.
     */
    ::VoxelCastle::World::ChunkColumn* getOrCreateEmptyChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);

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
     * @brief Get modification time for a specific chunk.
     * @param coord The chunk coordinate to query.
     * @return The time point when the chunk was last modified, or epoch if not found.
     */
    std::chrono::system_clock::time_point getChunkModificationTime(const WorldCoordXZ& coord) const;

    /**
     * @brief Mark a chunk as loaded from save file, so it skips world generation.
     * @param worldX The X coordinate of the chunk.
     * @param worldZ The Z coordinate of the chunk.
     */
    void markChunkLoaded(int_fast64_t worldX, int_fast64_t worldZ);

    /**
     * @brief Check if a chunk was loaded from a save file.
     * @param worldX The X coordinate of the chunk.
     * @param worldZ The Z coordinate of the chunk.
     * @return True if the chunk was loaded from a save file, false otherwise.
     */
    bool isChunkLoaded(int_fast64_t worldX, int_fast64_t worldZ) const;

    /**
     * @brief Sets the loading state flag.
     * @param isLoading Whether chunks are currently being loaded from a save.
     */
    void setLoadingState(bool isLoading);

    /**
     * @brief Clear the list of loaded chunks.
     * Should be called when resetting the world.
     */
    void clearLoadedChunks();

    /**
     * @brief Query chunk columns within a specified region.
     * @param xMin Minimum X coordinate of the region.
     * @param zMin Minimum Z coordinate of the region.
     * @param xMax Maximum X coordinate of the region.
     * @param zMax Maximum Z coordinate of the region.
     * @return Vector of chunk column pointers within the region.
     */
    std::vector<ChunkColumn*> queryChunkColumnsInRegion(int32_t xMin, int32_t zMin, int32_t xMax, int32_t zMax) const;

    /**
     * @brief Update active chunks based on center position and load radius.
     * @param centerWorldPosition The center position in world coordinates.
     * @param loadRadiusInSegments The radius to load chunks within, in segments.
     * @param generator The world generator to use for creating new chunks.
     */
    void updateActiveChunks(const glm::vec3& centerWorldPosition, int loadRadiusInSegments, WorldGenerator& generator);

    /**
     * @brief Mark all chunk segments as dirty for mesh regeneration.
     */
    void markAllSegmentsDirty();

    /**
     * @brief Reset the world by clearing all chunks and related data.
     */
    void resetWorld();

    /**
     * @brief Get list of all modified chunk coordinates.
     * @return Vector of WorldCoordXZ coordinates that have been modified.
     */
    std::vector<WorldCoordXZ> getModifiedChunks() const;

    /**
     * @brief Clear the list of modified chunks.
     */
    void clearModifiedChunks();

    /**
     * @brief Mark a chunk as modified (for save system integration).
     * @param worldX The world X coordinate.
     * @param worldZ The world Z coordinate.
     */
    void markChunkAsModified(int_fast64_t worldX, int_fast64_t worldZ);

    /**
     * @brief Get the total number of chunk columns.
     * @return The number of chunk columns currently loaded.
     */
    size_t getChunkCount() const;

private:
    // Helper to convert world coordinates to ChunkColumn base coordinates
    
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

    /**
     * @brief Set of chunk coordinates that have been modified since last save.
     * Used for incremental saving to only save chunks that have changed.
     */
    std::unordered_set<WorldCoordXZ, WorldCoordXZHash> m_modifiedChunks;

    /**
     * @brief Map storing modification timestamps for chunks.
     * Used to track when each chunk was last modified.
     */
    std::unordered_map<WorldCoordXZ, std::chrono::system_clock::time_point, WorldCoordXZHash> m_chunkModificationTimes;
    
    // Set of chunks that were loaded from save files - these should skip world generation
    std::unordered_set<WorldCoordXZ, WorldCoordXZHash> m_loadedChunks;
    
    // Flag to indicate if we're currently loading chunks from a save file
    bool m_isLoadingFromSave = false;
};

} // namespace World
} // namespace VoxelCastle

#endif // VOXELCASTLE_WORLD_MANAGER_H
