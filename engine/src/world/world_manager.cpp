#include <future>
#include <queue>
#include <mutex>
#include <cmath> // For std::floor
#include <set> // For std::set
#include <iostream> // For std::cout
#include <chrono>
#include <thread>
#include <iomanip> // For std::put_time

#include "world/world_manager.h"
#include "world/chunk_segment.h" // For SEGMENT_WIDTH, SEGMENT_DEPTH, SEGMENT_HEIGHT and ChunkSegment class
#include "world/chunk_column.h"  // For ChunkColumn class and CHUNKS_PER_COLUMN
#include "world/world_generator.h"  // For procedural world generation
#include "rendering/texture_atlas.h" // For VoxelEngine::Rendering::TextureAtlas
#include "rendering/mesh_builder.h"  // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/voxel_mesh.h"    // For VoxelEngine::Rendering::VoxelMesh
#include "utils/logging_utils.h"
#include "../../game/include/utils/debug_logger.h"

// Forward declaration to avoid circular dependency
namespace VoxelCastle {
namespace Core {
class SaveManager;
}
}

namespace VoxelCastle {
namespace World {

WorldManager::WorldManager() {
    // Initialize Quadtree to cover a large world region (e.g., -1M to +1M in XZ)
    world::AABB2D bounds{-1000000, -1000000, 1000000, 1000000};
    m_chunkQuadtree = std::make_unique<world::Quadtree>(bounds);

    // Redirect verbose logs to file-based logging
    // Initialize file-based logging here
}

::VoxelEngine::World::Voxel WorldManager::getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const {
    int_fast64_t colX = worldToColumnBaseX(worldX);
    int_fast64_t colZ = worldToColumnBaseZ(worldZ);

    const ChunkColumn* column = getChunkColumn(colX, colZ);
    if (column) {
        return column->getVoxel(worldX, worldY, worldZ);
    }
    return ::VoxelEngine::World::Voxel{0}; // Default voxel (e.g., air) if column doesn't exist
}

void WorldManager::setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const ::VoxelEngine::World::Voxel& voxel) {
    int_fast64_t colX = worldToColumnBaseX(worldX);
    int_fast64_t colZ = worldToColumnBaseZ(worldZ);

    ChunkColumn* column = getOrCreateChunkColumn(colX, colZ);
    if (column) {
        // Corrected: Call ChunkColumn::setVoxel with world coordinates and the provided voxel object.
        column->setVoxel(worldX, worldY, worldZ, voxel);
        
        // Track modification for save system
        WorldCoordXZ coord = {colX, colZ};
        m_modifiedChunks.insert(coord);
        m_chunkModificationTimes[coord] = std::chrono::system_clock::now();
        
        // === CONTINUOUS AUTO-SAVE: Save chunk immediately if enabled ===
        if (m_continuousAutoSaveEnabled && m_immediateSaveCallback) {
            DEBUG_LOG("WorldManager", "Continuous auto-save: Block modified at (" + 
                     std::to_string(worldX) + ", " + std::to_string(worldY) + ", " + std::to_string(worldZ) + 
                     "), triggering immediate chunk save");
            // Call the registered callback to save the chunk immediately
            m_immediateSaveCallback(colX, colZ);
        }
        
        DEBUG_LOG("WorldManager", "Block modified at (" + std::to_string(worldX) + ", " + 
                 std::to_string(worldY) + ", " + std::to_string(worldZ) + ") in chunk (" + 
                 std::to_string(colX) + ", " + std::to_string(colZ) + "). Total modified chunks: " + 
                 std::to_string(m_modifiedChunks.size()));
    }
    // Else: Potentially log an error if column creation failed, though getOrCreate should handle it or throw
}

// This overload takes a VoxelType and creates a Voxel object.
// It then calls the above setVoxel overload.
void WorldManager::setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, ::VoxelEngine::World::VoxelType type) {
    // Create the Voxel object using its explicit constructor.
    // Voxel constructor: explicit Voxel(uint8_t type_id, uint8_t light = 0)
    ::VoxelEngine::World::Voxel voxel_to_set{static_cast<uint8_t>(type)}; // Default light level is handled by Voxel constructor

    // Call the other WorldManager::setVoxel overload that takes a const Voxel&
    this->setVoxel(worldX, worldY, worldZ, voxel_to_set);
}

ChunkColumn* WorldManager::getChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) {
    WorldCoordXZ coord{worldX, worldZ};
    auto it = m_chunkColumns.find(coord);
    if (it != m_chunkColumns.end()) {
        return it->second.get();
    }
    return nullptr;
}

const ChunkColumn* WorldManager::getChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) const {
    WorldCoordXZ coord{worldX, worldZ};
    auto it = m_chunkColumns.find(coord);
    if (it != m_chunkColumns.end()) {
        return it->second.get();
    }
    return nullptr;
}

ChunkColumn* WorldManager::getOrCreateChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) {
    WorldCoordXZ coord{worldX, worldZ}; // These are already base coordinates
    ChunkColumn* column = getChunkColumn(coord.x, coord.z);
    if (!column) {
        // Column doesn't exist, create it
        auto newColumnUniquePtr = std::make_unique<ChunkColumn>(coord.x, coord.z);
        column = newColumnUniquePtr.get(); // Get raw pointer before moving

        // Skip world generation if this chunk was loaded from a save file
        // or if we're currently in loading mode (to prevent generating terrain for chunks that will be loaded next)
        bool skipWorldGen = m_isLoadingFromSave || isChunkLoaded(coord.x, coord.z);
        
        if (!skipWorldGen) {
            // Procedural world generation for each segment
            for (uint8_t i = 0; i < VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                VoxelCastle::World::ChunkSegment* segment = column->getSegmentByIndex(i);
                if (segment) {
                    // Pass world coordinates for this segment
                    VoxelCastle::World::WorldGenerator::generateChunkSegment(*segment, coord.x, i, coord.z);
                    // setVoxel marks the segment dirty, so it will be meshed on the first updateDirtyMeshes call
                }
            }
        } else {
            std::cout << "[WorldManager] Skipping world generation for loaded chunk: (" 
                      << coord.x << ", " << coord.z << ")" << std::endl;
        }

        m_chunkColumns[coord] = std::move(newColumnUniquePtr);
        // Insert into Quadtree for spatial queries
        if (m_chunkQuadtree) {
            m_chunkQuadtree->insert(static_cast<int32_t>(coord.x), static_cast<int32_t>(coord.z), column);
        }
    }
    return column;
}

ChunkColumn* WorldManager::getOrCreateEmptyChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) {
    WorldCoordXZ coord{worldX, worldZ}; // These are already base coordinates
    ChunkColumn* column = getChunkColumn(coord.x, coord.z);
    if (!column) {
        // Column doesn't exist, create it without world generation
        auto newColumnUniquePtr = std::make_unique<ChunkColumn>(coord.x, coord.z);
        column = newColumnUniquePtr.get(); // Get raw pointer before moving
        
        // Mark this as a loaded chunk to prevent world generation later
        markChunkLoaded(coord.x, coord.z);
        
        std::cout << "[WorldManager] Created empty chunk column for loading: (" 
                  << coord.x << ", " << coord.z << ")" << std::endl;

        m_chunkColumns[coord] = std::move(newColumnUniquePtr);
        // Insert into Quadtree for spatial queries
        if (m_chunkQuadtree) {
            m_chunkQuadtree->insert(static_cast<int32_t>(coord.x), static_cast<int32_t>(coord.z), column);
        }
    }
    return column;
}

void WorldManager::updateDirtyMeshes(VoxelEngine::Rendering::TextureAtlas& atlas, VoxelEngine::Rendering::MeshBuilder& meshBuilder) {
    // For compatibility: call enqueueDirtyMeshJobs and processFinishedMeshJobs
    enqueueDirtyMeshJobs(atlas, meshBuilder);
    processFinishedMeshJobs();
}

void WorldManager::enqueueDirtyMeshJobs(VoxelEngine::Rendering::TextureAtlas& atlas, VoxelEngine::Rendering::MeshBuilder& meshBuilder) {
    // std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][WorldManager] enqueueDirtyMeshJobs() START - Thread: " << std::this_thread::get_id() << std::endl;
    
    if (!m_meshJobSystem) {
        m_meshJobSystem = std::make_unique<VoxelEngine::Rendering::MeshJobSystem>(std::thread::hardware_concurrency());
        std::cout << "[INFO] ECS system initialized" << std::endl;
    }
    int enqueued = 0;
    // std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][WorldManager] Checking " << m_chunkColumns.size() << " chunk columns for dirty segments" << std::endl;
    for (auto const& [coord, columnPtr] : m_chunkColumns) {
        if (columnPtr) {
            // Hold a shared_ptr to the column for the duration of the mesh job
            std::shared_ptr<VoxelCastle::World::ChunkColumn> columnSharedPtr = columnPtr;
            for (uint8_t i = 0; i < VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                VoxelCastle::World::ChunkSegment* segment = columnSharedPtr->getSegmentByIndex(i);
                if (segment && segment->isDirty() && !segment->mIsRebuildingMesh) {
                    std::string segmentDetails = "Found dirty segment at column (" + std::to_string(coord.x) + ", " + std::to_string(coord.z) + ") segment Y=" + std::to_string(static_cast<int>(i));
                    DEBUG_LOG("WorldManager", segmentDetails);
                    segment->mIsRebuildingMesh = true;
                    // Copy pointers for lambda capture
                    auto segPtr = segment;
                    auto colPtr = columnSharedPtr;
                    auto worldPtr = this;
                    // Copy atlas and meshBuilder by reference (safe: read-only)
                    // Capture shared_ptr by value to keep column alive during job
                    m_meshJobSystem->enqueue([this, segPtr, colPtr, worldPtr, &atlas, &meshBuilder, i]() {
                        auto mesh = std::make_unique<VoxelEngine::Rendering::VoxelMesh>();

                        // Determine the chunk coordinates for this specific segment
                        glm::ivec3 currentChunkCoords(
                            static_cast<int>(colPtr->getBaseX()),      // X coordinate of the column
                            static_cast<int>(i),                        // Y index of the segment in the column
                            static_cast<int>(colPtr->getBaseZ())       // Z coordinate of the column
                        );

                        // Using Two-Phase Greedy meshing for improved performance and quality
                        // To revert to legacy greedy meshing, replace buildTwoPhaseGreedyMesh with buildGreedyMesh
                        *mesh = meshBuilder.buildTwoPhaseGreedyMesh(*segPtr, atlas, [=](int x, int y, int z) {
                            int_fast64_t worldX = colPtr->getBaseX() + x;
                            int_fast64_t worldY = i * VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT + y;
                            int_fast64_t worldZ = colPtr->getBaseZ() + z;
                            return worldPtr->getVoxel(worldX, worldY, worldZ);
                        }, currentChunkCoords); // Pass the calculated chunkCoords

                        mesh->setInitialized(true);
                        mesh->setWorldPosition(glm::vec3(
                            static_cast<float>(colPtr->getBaseX()),
                            static_cast<float>(i * VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT),
                            static_cast<float>(colPtr->getBaseZ())));
                        // Queue result for main thread using our member variables
                        std::lock_guard<std::mutex> lock(m_meshJobMutex);
                        m_completedMeshJobs.push_back(MeshJobData{colPtr, segPtr, std::move(mesh)});
                    });
                    ++enqueued;
                }
            }
        }
    }
    if (enqueued > 0) {
        DEBUG_LOG("MeshJobSystem", "Enqueued " + std::to_string(enqueued) + " mesh jobs.");
    }
    // std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][WorldManager] enqueueDirtyMeshJobs() END" << std::endl;
}

void WorldManager::processFinishedMeshJobs() {
    int processed = 0;
    {
        std::lock_guard<std::mutex> lock(m_meshJobMutex);
        for (auto& jobData : m_completedMeshJobs) {
            if (jobData.segment && jobData.mesh) {
                // ChunkSegment::mMesh is now public for async access
                jobData.segment->mMesh = std::move(jobData.mesh);
                jobData.segment->mMesh->setInitialized(true);
                jobData.segment->markDirty(false);
                jobData.segment->mIsRebuildingMesh = false;
                // jobData.column (shared_ptr) keeps the column alive until this point
                ++processed;
            }
        }
        m_completedMeshJobs.clear();
    }
    static int uploadLogCounter = 0;
    const int uploadLogInterval = 60; // Print every 60 calls/frames
    uploadLogCounter++;
    if (processed > 0 && (uploadLogCounter % uploadLogInterval == 0)) {
        DEBUG_LOG("MeshJobSystem", "Uploaded " + std::to_string(processed) + " finished meshes to main thread (interval summary).");
    }
}

std::vector<const VoxelEngine::Rendering::VoxelMesh*> WorldManager::getAllSegmentMeshes() const {
    std::vector<const VoxelEngine::Rendering::VoxelMesh*> meshes;
    for (auto const& [coord, columnPtr] : m_chunkColumns) {
        if (columnPtr) {
            for (uint8_t i = 0; i < VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                VoxelCastle::World::ChunkSegment* segment = columnPtr->getSegmentByIndex(i);
                if (segment) {
                    const VoxelEngine::Rendering::VoxelMesh* mesh = segment->getMesh();
                    if (mesh && mesh->getVertexCount() > 0) { // Use new getVertexCount()
                        meshes.push_back(mesh);
                    }
                }
            }
        }
    }
    return meshes;
}

std::vector<ChunkColumn*> WorldManager::queryChunkColumnsInRegion(int32_t xMin, int32_t zMin, int32_t xMax, int32_t zMax) const {
    if (!m_chunkQuadtree) return {};
    world::AABB2D region{xMin, zMin, xMax, zMax};
    return m_chunkQuadtree->queryRegion(region);
}

// Static functions must be inside the correct namespace block

int_fast64_t WorldManager::worldToColumnBaseX(int_fast64_t worldX) {
    return static_cast<int_fast64_t>(std::floor(static_cast<double>(worldX) / VoxelCastle::World::ChunkSegment::CHUNK_WIDTH)) * VoxelCastle::World::ChunkSegment::CHUNK_WIDTH;
}

int_fast64_t WorldManager::worldToColumnBaseZ(int_fast64_t worldZ) {
    return static_cast<int_fast64_t>(std::floor(static_cast<double>(worldZ) / VoxelCastle::World::ChunkSegment::CHUNK_DEPTH)) * VoxelCastle::World::ChunkSegment::CHUNK_DEPTH;
}

void WorldManager::updateActiveChunks(const glm::vec3& centerWorldPosition, int loadRadiusInSegments, WorldGenerator& generator) {
// Calculate chunk column coordinates (not segment indices)

// Extended chunk grid - was 3x3 (radius 1), now 5x5 (radius 2) for better visibility
const int forcedRadius = 2; // 2 in each direction = 5x5 chunk grid
int centerColX = static_cast<int>(std::floor(centerWorldPosition.x / ChunkSegment::CHUNK_WIDTH));
int centerColZ = static_cast<int>(std::floor(centerWorldPosition.z / ChunkSegment::CHUNK_DEPTH));
int centerSegY = static_cast<int>(std::floor(centerWorldPosition.y / ChunkSegment::CHUNK_HEIGHT));

std::set<std::pair<int, int>> activeColumns;

for (int colX = centerColX - forcedRadius; colX <= centerColX + forcedRadius; ++colX) {
    for (int colZ = centerColZ - forcedRadius; colZ <= centerColZ + forcedRadius; ++colZ) {
        // Convert chunk indices to base world coordinates
        int baseColX = colX * ChunkSegment::CHUNK_WIDTH;
        int baseColZ = colZ * ChunkSegment::CHUNK_DEPTH;
        activeColumns.insert({baseColX, baseColZ});
        ChunkColumn* column = getOrCreateChunkColumn(baseColX, baseColZ);
        for (int segY = centerSegY - loadRadiusInSegments; segY <= centerSegY + loadRadiusInSegments; ++segY) {
            ChunkSegment* segment = column->getOrCreateSegment(segY);
            if (segment->isEmpty() && !segment->isGenerated()) {
                generator.generateChunkSegment(*segment, baseColX, segY, baseColZ);
                segment->markDirty(true);
                segment->setGenerated(true); // Mark the segment as generated
            }
        }
    }
}

// Defer unloading: mark columns for removal, but only erase after mesh jobs are done
// CRITICAL FIX: Don't unload chunks during initial loading phase to prevent flickering
if (!m_isLoadingFromSave) {
    std::vector<WorldCoordXZ> columnsToUnload;
    for (auto it = m_chunkColumns.begin(); it != m_chunkColumns.end(); ++it) {
        const auto& [coord, columnPtr] = *it;
        if (activeColumns.find({coord.x, coord.z}) == activeColumns.end()) {
            columnsToUnload.push_back(coord);
        }
    }
    // Only erase columns if there are no running mesh jobs (simple version: after processFinishedMeshJobs)
    // In a real system, you may want to check for jobs referencing these columns
    if (!m_meshJobSystem || m_meshJobSystem->runningJobs() == 0) {
        for (const auto& coord : columnsToUnload) {
            m_chunkColumns.erase(coord);
        }
    }
}
}

void WorldManager::markAllSegmentsDirty() {
    for (auto& pair : m_chunkColumns) {
        if (pair.second) {
            pair.second->markAllSegmentsDirty();
        }
    }
    std::cout << "[WorldManager] Marked all segments dirty for mesh regeneration." << std::endl;
}

void WorldManager::resetWorld() {
    std::cout << "[WorldManager] Resetting world - clearing all chunks" << std::endl;
    
    // Clear all chunk columns
    m_chunkColumns.clear();
    
    // Clear modified chunks tracking
    m_modifiedChunks.clear();
    m_chunkModificationTimes.clear();
    
    // Clear loaded chunks tracking
    m_loadedChunks.clear();
    m_isLoadingFromSave = false;
    
    // Reset the quadtree if it exists
    if (m_chunkQuadtree) {
        m_chunkQuadtree.reset();
        // Create new quadtree with proper AABB2D bounds
        ::world::AABB2D bounds = {-1024, -1024, 1024, 1024};
        m_chunkQuadtree = std::make_unique<::world::Quadtree>(bounds);
    }
    
    std::cout << "[WorldManager] World reset complete" << std::endl;
}

// === Save System Integration Implementation ===

std::vector<WorldCoordXZ> WorldManager::getModifiedChunks() const {
    return std::vector<WorldCoordXZ>(m_modifiedChunks.begin(), m_modifiedChunks.end());
}

void WorldManager::clearModifiedChunks() {
    m_modifiedChunks.clear();
    m_chunkModificationTimes.clear();
}

void WorldManager::markChunkAsModified(int_fast64_t worldX, int_fast64_t worldZ) {
    int_fast64_t colX = worldToColumnBaseX(worldX);
    int_fast64_t colZ = worldToColumnBaseZ(worldZ);
    
    WorldCoordXZ coord = {colX, colZ};
    m_modifiedChunks.insert(coord);
    m_chunkModificationTimes[coord] = std::chrono::system_clock::now();
    
    std::cout << "[WorldManager] DEBUG: Marked chunk (" << colX << ", " << colZ 
              << ") as modified. Total modified chunks: " << m_modifiedChunks.size() << std::endl;
}

size_t WorldManager::getChunkCount() const {
    return m_chunkColumns.size();
}

std::chrono::system_clock::time_point WorldManager::getChunkModificationTime(const WorldCoordXZ& coord) const {
    auto it = m_chunkModificationTimes.find(coord);
    if (it != m_chunkModificationTimes.end()) {
        return it->second;
    }
    return std::chrono::system_clock::time_point{}; // Return epoch if not found
}

void WorldManager::markChunkLoaded(int_fast64_t worldX, int_fast64_t worldZ) {
    WorldCoordXZ coord{worldX, worldZ};
    m_loadedChunks.insert(coord);
    std::cout << "[WorldManager] Marked chunk as loaded: (" << worldX << ", " << worldZ << ")" << std::endl;
}

bool WorldManager::isChunkLoaded(int_fast64_t worldX, int_fast64_t worldZ) const {
    WorldCoordXZ coord{worldX, worldZ};
    return m_loadedChunks.find(coord) != m_loadedChunks.end();
}

void WorldManager::setLoadingState(bool isLoading) {
    m_isLoadingFromSave = isLoading;
    if (isLoading) {
        std::cout << "[WorldManager] Entering load state - world generation will be skipped for loaded chunks" << std::endl;
    }
}

void WorldManager::clearLoadedChunks() {
    m_loadedChunks.clear();
    m_isLoadingFromSave = false;
}

// === CONTINUOUS AUTO-SAVE INTEGRATION ===

void WorldManager::setImmediateSaveCallback(std::function<void(int_fast64_t, int_fast64_t)> callback) {
    m_immediateSaveCallback = callback;
}

void WorldManager::enableContinuousAutoSave(bool enabled) {
    m_continuousAutoSaveEnabled = enabled;
    std::cout << "[WorldManager] Continuous auto-save " << (enabled ? "enabled" : "disabled") << std::endl;
}

} // namespace World
} // namespace VoxelCastle
