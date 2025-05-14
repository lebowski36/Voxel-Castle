#include "world/world_manager.h"
#include "world/chunk_segment.h" // For SEGMENT_WIDTH, SEGMENT_DEPTH, SEGMENT_HEIGHT and ChunkSegment class
#include "world/chunk_column.h"  // For ChunkColumn class and CHUNKS_PER_COLUMN
#include "world/world_generator.h"  // For procedural world generation
#include "rendering/texture_atlas.h" // For VoxelEngine::Rendering::TextureAtlas
#include "rendering/mesh_builder.h"  // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/voxel_mesh.h"    // For VoxelEngine::Rendering::VoxelMesh
#include <cmath> // For std::floor


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
    ChunkColumnCoord coord{worldX, worldZ};
    auto it = m_chunkColumns.find(coord);
    if (it != m_chunkColumns.end()) {
        return it->second.get();
    }
    return nullptr;
}

const ChunkColumn* WorldManager::getChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) const {
    ChunkColumnCoord coord{worldX, worldZ};
    auto it = m_chunkColumns.find(coord);
    if (it != m_chunkColumns.end()) {
        return it->second.get();
    }
    return nullptr;
}

ChunkColumn* WorldManager::getOrCreateChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) {
    ChunkColumnCoord coord{worldX, worldZ}; // These are already base coordinates
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
    for (auto const& [coord, columnPtr] : m_chunkColumns) {
        if (columnPtr) {
            for (uint8_t i = 0; i < VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                VoxelCastle::World::ChunkSegment* segment = columnPtr->getSegmentByIndex(i);
                if (segment && segment->isDirty()) {
                    // Pass the column's world X and Z coordinates, and the segment's Y index
                    segment->rebuildMesh(atlas, meshBuilder, columnPtr->getBaseX(), i, columnPtr->getBaseZ());
                }
            }
        }
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

} // namespace World
} // namespace VoxelCastle
