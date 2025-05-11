#ifndef VOXELCASTLE_WORLD_MANAGER_H
#define VOXELCASTLE_WORLD_MANAGER_H

#include "world/chunk_column.h" // For ::VoxelCastle::World::ChunkColumn and ::VoxelCastle::World::ChunkColumnCoord
#include "world/voxel.h"      // For ::VoxelEngine::World::Voxel
#include <map>                // Changed from unordered_map to map
#include <memory>
#include <cstdint>            // For int_fast64_t

namespace VoxelCastle {
    namespace World {

        class WorldManager {
        public:
            WorldManager();

            // Get a specific voxel from the world using world coordinates.
            ::VoxelEngine::World::Voxel getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const;

            // Set a specific voxel in the world using world coordinates.
            // This may involve creating new chunk columns or segments.
            void setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const ::VoxelEngine::World::Voxel& voxel);

            // Get a chunk column by its world XZ coordinates.
            // Returns nullptr if the column doesn't exist.
            // Parameters changed to worldX, worldZ for clarity, matching getOrCreateChunkColumn
            ::VoxelCastle::World::ChunkColumn* getChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);
            const ::VoxelCastle::World::ChunkColumn* getChunkColumn(int_fast64_t worldX, int_fast64_t worldZ) const;

            // Get or create a chunk column by its world XZ coordinates.
            // If the column doesn't exist, it will be created.
            // Renamed from loadOrGenerateChunkColumn for consistency
            ::VoxelCastle::World::ChunkColumn* getOrCreateChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);

            // Helper to convert world X coordinate to its column's base X coordinate
            static int_fast64_t worldToColumnBaseX(int_fast64_t worldX);
            // Helper to convert world Z coordinate to its column's base Z coordinate
            static int_fast64_t worldToColumnBaseZ(int_fast64_t worldZ);

        private:
            // Map of chunk column coordinates to ChunkColumn objects
            // Changed to std::map, uses ChunkColumnCoord::operator<
            std::map< ::VoxelCastle::World::ChunkColumnCoord, std::unique_ptr< ::VoxelCastle::World::ChunkColumn>> m_chunkColumns;
        };

    } // namespace World
} // namespace VoxelCastle

#endif // VOXELCASTLE_WORLD_MANAGER_H
