#include "world/world_manager.h"
#include "world/chunk_segment.h" // For SEGMENT_WIDTH, SEGMENT_DEPTH, SEGMENT_HEIGHT
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
    // getOrCreateChunkColumn should guarantee a valid column pointer
    if (column) { 
        column->setVoxel(worldX, worldY, worldZ, voxel);
    }
    // Else: Potentially log an error if column creation failed, though getOrCreate should handle it or throw
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
        auto newColumn = std::make_unique<ChunkColumn>(coord.x, coord.z);
        column = newColumn.get();
        m_chunkColumns[coord] = std::move(newColumn);
        // Insert into Quadtree for spatial queries
        if (m_chunkQuadtree) {
            m_chunkQuadtree->insert(static_cast<int32_t>(coord.x), static_cast<int32_t>(coord.z), column);
        }
    }
    return column;
}

std::vector<ChunkColumn*> WorldManager::queryChunkColumnsInRegion(int32_t xMin, int32_t zMin, int32_t xMax, int32_t zMax) const {
    if (!m_chunkQuadtree) return {};
    world::AABB2D region{xMin, zMin, xMax, zMax};
    return m_chunkQuadtree->queryRegion(region);
}

// Static functions must be inside the correct namespace block

int_fast64_t WorldManager::worldToColumnBaseX(int_fast64_t worldX) {
    return static_cast<int_fast64_t>(std::floor(static_cast<double>(worldX) / VoxelCastle::World::SEGMENT_WIDTH)) * VoxelCastle::World::SEGMENT_WIDTH;
}

int_fast64_t WorldManager::worldToColumnBaseZ(int_fast64_t worldZ) {
    return static_cast<int_fast64_t>(std::floor(static_cast<double>(worldZ) / VoxelCastle::World::SEGMENT_DEPTH)) * VoxelCastle::World::SEGMENT_DEPTH;
}

} // namespace World
} // namespace VoxelCastle
