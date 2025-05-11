#ifndef WORLD_MANAGER_H
#define WORLD_MANAGER_H

#include "world/chunk_column.h"
#include "world/voxel.h"
#include <unordered_map>
#include <memory>
#include <cstdint> // For int_fast64_t

// Forward declaration for custom hash if needed for ChunkColumnCoord in unordered_map
// However, ChunkColumnCoord already has operator< for std::map. For unordered_map, a hash is needed.
namespace std {
    template <>
    struct hash<VoxelCastle::World::ChunkColumnCoord> {
        size_t operator()(const VoxelCastle::World::ChunkColumnCoord& coord) const noexcept;
    };
}

namespace VoxelCastle
{
    namespace World
    {
        class WorldManager
        {
        public:
            WorldManager();

            // Get a specific voxel from the world using world coordinates.
            Voxel getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const;

            // Set a specific voxel in the world using world coordinates.
            // This will load or create chunks/columns/segments as necessary.
            void setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const Voxel& voxel);

            // Get a chunk column by its XZ world coordinates.
            // Returns nullptr if the column isn't loaded.
            ChunkColumn* getChunkColumn(int_fast64_t columnX, int_fast64_t columnZ);
            const ChunkColumn* getChunkColumn(int_fast64_t columnX, int_fast64_t columnZ) const;

            // Load or create a chunk column at the given XZ world coordinates.
            // If the column doesn't exist, it will be created.
            ChunkColumn* loadOrGenerateChunkColumn(int_fast64_t columnX, int_fast64_t columnZ);

            // Helper to convert world X coordinate to column X coordinate
            static int_fast64_t worldXToColumnX(int_fast64_t worldX);
            // Helper to convert world Z coordinate to column Z coordinate
            static int_fast64_t worldZToColumnZ(int_fast64_t worldZ);

        private:
            // Storage for active chunk columns. Keyed by their world XZ base coordinates.
            // Using ChunkColumnCoord which needs a hash function for unordered_map.
            std::unordered_map<ChunkColumnCoord, std::unique_ptr<ChunkColumn>> m_chunkColumns;
        };

    } // namespace World
} // namespace VoxelCastle

#endif // WORLD_MANAGER_H
