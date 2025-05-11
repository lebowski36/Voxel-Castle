#ifndef CHUNK_COLUMN_H
#define CHUNK_COLUMN_H

#include "world/chunk_segment.h" // Provides ::VoxelCastle::World::ChunkSegment and dimension constants
#include "world/voxel.h"         // Provides ::VoxelEngine::World::Voxel
#include <map>
#include <memory>
#include <cstdint> // For int_fast32_t, int_fast64_t

namespace VoxelCastle
{
    namespace World
    {
        // Using a struct for simple XZ coordinate pair
        struct ChunkColumnCoord
        {
            int_fast64_t x;
            int_fast64_t z;

            // For use as a key in std::map or std::unordered_map
            bool operator<(const ChunkColumnCoord& other) const
            {
                if (x < other.x) return true;
                if (x > other.x) return false;
                return z < other.z;
            }
        };

        class ChunkColumn
        {
        public:
            // Constructor: Requires the XZ world coordinates of this column
            ChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);

            // Get the XZ coordinates of this chunk column
            ChunkColumnCoord getCoordinates() const;

            // Get a specific voxel from the column using world coordinates.
            // The worldY coordinate will determine which segment to access.
            ::VoxelEngine::World::Voxel getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const;

            // Set a specific voxel in the column using world coordinates.
            // The worldY coordinate will determine which segment to access or create.
            void setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const ::VoxelEngine::World::Voxel& voxel);

            // Get a segment by its Y index (0 for segment at world Y 0-31, 1 for 32-63, etc.)
            // Returns nullptr if the segment doesn't exist.
            ::VoxelCastle::World::ChunkSegment* getSegment(int_fast32_t segmentYIndex);
            const ::VoxelCastle::World::ChunkSegment* getSegment(int_fast32_t segmentYIndex) const;

            // Get or create a segment by its Y index.
            // If the segment doesn't exist, it will be created.
            ::VoxelCastle::World::ChunkSegment* getOrCreateSegment(int_fast32_t segmentYIndex);

            // Helper to convert world Y coordinate to segment Y index
            static int_fast32_t worldYToSegmentYIndex(int_fast64_t worldY);

            // Helper to convert world coordinates to local segment coordinates
            static void worldToLocalSegmentCoords(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ,
                                                int_fast32_t& segmentX, int_fast32_t& segmentY, int_fast32_t& segmentZ,
                                                int_fast64_t columnWorldX, int_fast64_t columnWorldZ);

        private:
            ChunkColumnCoord m_coordinates; // World XZ coordinates of this column (at its origin 0,0)
            // Map of segment Y-index to ChunkSegment. Y-index is worldY / SEGMENT_HEIGHT.
            std::map<int_fast32_t, std::unique_ptr<::VoxelCastle::World::ChunkSegment>> m_segments;
        };

    } // namespace World
} // namespace VoxelCastle

#endif // CHUNK_COLUMN_H
