
#include <future>
#include <queue>
#include <mutex>
#include <cmath> // For std::floor
#include <set> // For std::set
#include <iostream> // For std::cout

#include "world/world_manager.h"
#include "world/chunk_segment.h" // For SEGMENT_WIDTH, SEGMENT_DEPTH, SEGMENT_HEIGHT and ChunkSegment class
#include "world/chunk_column.h"  // For ChunkColumn class and CHUNKS_PER_COLUMN
#include "world/world_generator.h"  // For procedural world generation
#include "rendering/texture_atlas.h" // For VoxelEngine::Rendering::TextureAtlas
#include "rendering/mesh_builder.h"  // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/voxel_mesh.h"    // For VoxelEngine::Rendering::VoxelMesh

namespace VoxelCastle {
namespace World {

WorldManager::WorldManager() {
    // Initialize Quadtree to cover a large world region (e.g., -1M to +1M in XZ)
    world::AABB2D bounds{-1000000, -1000000, 1000000, 1000000};
    m_chunkQuadtree = std::make_unique<world::Quadtree>(bounds);
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

        // Procedural world generation for each segment
        for (uint8_t i = 0; i < VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
            VoxelCastle::World::ChunkSegment* segment = column->getSegmentByIndex(i);
            if (segment) {
                // Pass world coordinates for this segment
                VoxelCastle::World::WorldGenerator::generateChunkSegment(*segment, coord.x, i, coord.z);
                // setVoxel marks the segment dirty, so it will be meshed on the first updateDirtyMeshes call
            }
        }

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
    if (!m_meshJobSystem) {
        m_meshJobSystem = std::make_unique<VoxelEngine::Rendering::MeshJobSystem>(std::thread::hardware_concurrency());
        std::cout << "[MeshJobSystem] Initialized with " << std::thread::hardware_concurrency() << " threads." << std::endl;
    }
    int enqueued = 0;
    for (auto const& [coord, columnPtr] : m_chunkColumns) {
        if (columnPtr) {
            for (uint8_t i = 0; i < VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                VoxelCastle::World::ChunkSegment* segment = columnPtr->getSegmentByIndex(i);
                if (segment && segment->isDirty() && !segment->mIsRebuildingMesh) {
                    segment->mIsRebuildingMesh = true;
                    // Copy pointers for lambda capture
                    auto* segPtr = segment;
                    auto* colPtr = columnPtr.get();
                    auto* worldPtr = this;
                    // Copy atlas and meshBuilder by reference (safe: read-only)
                    // Capture "this" to access member variables in the lambda
                    m_meshJobSystem->enqueue([this, segPtr, colPtr, worldPtr, &atlas, &meshBuilder, i]() {
                        auto mesh = std::make_unique<VoxelEngine::Rendering::VoxelMesh>();
                        *mesh = meshBuilder.buildGreedyMesh(*segPtr, atlas, [=](int x, int y, int z) {
                            int_fast64_t worldX = colPtr->getBaseX() + x;
                            int_fast64_t worldY = i * VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT + y;
                            int_fast64_t worldZ = colPtr->getBaseZ() + z;
                            return worldPtr->getVoxel(worldX, worldY, worldZ);
                        });
                        mesh->setInitialized(true);
                        mesh->setWorldPosition(glm::vec3(
                            static_cast<float>(colPtr->getBaseX()),
                            static_cast<float>(i * VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT),
                            static_cast<float>(colPtr->getBaseZ())));
                        
                        // Queue result for main thread using our member variables
                        std::lock_guard<std::mutex> lock(m_meshJobMutex);
                        m_completedMeshJobs.push_back(MeshJobData{segPtr, std::move(mesh)});
                    });
                    ++enqueued;
                }
            }
        }
    }
    if (enqueued > 0) {
        std::cout << "[MeshJobSystem] Enqueued " << enqueued << " mesh jobs." << std::endl;
    }
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
                ++processed;
            }
        }
        m_completedMeshJobs.clear();
    }
    if (processed > 0) {
        std::cout << "[MeshJobSystem] Uploaded " << processed << " finished meshes to main thread." << std::endl;
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
    int centerSegX = static_cast<int>(std::floor(centerWorldPosition.x / ChunkSegment::CHUNK_WIDTH));
    int centerSegY = static_cast<int>(std::floor(centerWorldPosition.y / ChunkSegment::CHUNK_HEIGHT));
    int centerSegZ = static_cast<int>(std::floor(centerWorldPosition.z / ChunkSegment::CHUNK_DEPTH));

    std::set<std::pair<int, int>> activeColumns;

    for (int x = centerSegX - loadRadiusInSegments; x <= centerSegX + loadRadiusInSegments; ++x) {
        for (int z = centerSegZ - loadRadiusInSegments; z <= centerSegZ + loadRadiusInSegments; ++z) {
            activeColumns.insert({x, z});
            for (int y = centerSegY - loadRadiusInSegments; y <= centerSegY + loadRadiusInSegments; ++y) {
                ChunkColumn* column = getOrCreateChunkColumn(x, z);
                ChunkSegment* segment = column->getOrCreateSegment(y);

                if (segment->isEmpty() && !segment->isGenerated()) {
                    generator.generateChunkSegment(*segment, x, y, z);
                    segment->markDirty(true);
                    segment->setGenerated(true); // Mark the segment as generated
                }
            }
        }
    }

    // Unload chunks outside the active radius
    for (auto it = m_chunkColumns.begin(); it != m_chunkColumns.end();) {
        const auto& [coord, columnPtr] = *it;
        if (activeColumns.find({coord.x, coord.z}) == activeColumns.end()) {
            it = m_chunkColumns.erase(it); // Unload the column
        } else {
            ++it;
        }
    }
}

} // namespace World
} // namespace VoxelCastle
