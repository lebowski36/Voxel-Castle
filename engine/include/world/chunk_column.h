
#ifndef CHUNK_COLUMN_H
#define CHUNK_COLUMN_H

// Standard library includes first
#include <map>
#include <memory>
#include <cstdint> // For int_fast32_t, int_fast64_t

#include "world/chunk_segment.h" // Provides ::VoxelCastle::World::ChunkSegment and dimension constants
#include "world/voxel.h"         // Provides ::VoxelEngine::World::Voxel

namespace VoxelCastle {
namespace World {
        /**
         * @struct ChunkColumnCoord
         * @brief Represents a simple XZ coordinate pair for chunk columns.
         *
         * Used as a key in std::map or std::unordered_map.
         */
        struct ChunkColumnCoord
        {
            int_fast64_t x; ///< X-coordinate of the chunk column.
            int_fast64_t z; ///< Z-coordinate of the chunk column.

            /**
             * @brief Comparison operator for sorting ChunkColumnCoord.
             * @param other The other ChunkColumnCoord to compare against.
             * @return True if this coordinate is less than the other.
             */
            bool operator<(const ChunkColumnCoord& other) const
            {
                if (x < other.x) return true;
                if (x > other.x) return false;
                return z < other.z;
            }
        };

        /**
         * @class ChunkColumn
         * @brief Represents a column of ChunkSegments, extending vertically through the world.
         *
         * A ChunkColumn manages a stack of ChunkSegments at a specific XZ world coordinate.
         * It is responsible for creating, accessing, and managing these segments.
         */
        class ChunkColumn
        {
        public:
            // Define how many segments are stacked vertically in a column
            static constexpr uint8_t CHUNKS_PER_COLUMN = 16;

            /**
             * @brief Returns the base X coordinate of this chunk column.
             */
            int_fast64_t getBaseX() const { return m_coordinates.x; }

            /**
             * @brief Returns the base Z coordinate of this chunk column.
             */
            int_fast64_t getBaseZ() const { return m_coordinates.z; }
        public:
            /**
             * @brief Constructs a ChunkColumn at the given XZ world coordinates.
             *
             * Initializes the column, potentially pre-allocating or lazily loading its ChunkSegments.
             * @param worldX The X-coordinate of this column in the world.
             * @param worldZ The Z-coordinate of this column in the world.
             */
            ChunkColumn(int_fast64_t worldX, int_fast64_t worldZ);

            /**
             * @brief Gets the XZ coordinates of this chunk column.
             * @return The ChunkColumnCoord representing the XZ coordinates.
             */
            ChunkColumnCoord getCoordinates() const;

            /**
             * @brief Gets a specific voxel from the column using world coordinates.
             *
             * The worldY coordinate will determine which segment to access.
             * @param worldX The world X-coordinate.
             * @param worldY The world Y-coordinate.
             * @param worldZ The world Z-coordinate.
             * @return The VoxelEngine::World::Voxel at the given coordinates.
             */
            ::VoxelEngine::World::Voxel getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const;

            /**
             * @brief Sets a specific voxel in the column using world coordinates.
             *
             * The worldY coordinate will determine which segment to access or create.
             * @param worldX The world X-coordinate.
             * @param worldY The world Y-coordinate.
             * @param worldZ The world Z-coordinate.
             * @param voxel The VoxelEngine::World::Voxel to set.
             */
            void setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const ::VoxelEngine::World::Voxel& voxel);

            /**
             * @brief Retrieves a pointer to a ChunkSegment by its vertical index within the column.
             *
             * @param segmentIndex The vertical index of the segment (0 to CHUNKS_PER_COLUMN - 1).
             * @return A pointer to the ChunkSegment if it exists and the index is valid, otherwise nullptr.
             */
            ChunkSegment* getSegmentByIndex(uint8_t segmentIndex);
            const ChunkSegment* getSegmentByIndex(uint8_t segmentIndex) const;

            /**
             * @brief Gets a segment by its Y index.
             *
             * 0 for segment at world Y 0-31, 1 for 32-63, etc.
             * @param segmentYIndex The Y index of the segment.
             * @return A pointer to the ChunkSegment. Returns nullptr if the segment doesn't exist.
             */
            ::VoxelCastle::World::ChunkSegment* getSegment(int_fast32_t segmentYIndex);

            /**
             * @brief Gets a segment by its Y index (const version).
             *
             * 0 for segment at world Y 0-31, 1 for 32-63, etc.
             * @param segmentYIndex The Y index of the segment.
             * @return A const pointer to the ChunkSegment. Returns nullptr if the segment doesn't exist.
             */
            const ::VoxelCastle::World::ChunkSegment* getSegment(int_fast32_t segmentYIndex) const;

            /**
             * @brief Gets or creates a segment by its Y index.
             *
             * If the segment doesn't exist, it will be created.
             * @param segmentYIndex The Y index of the segment.
             * @return A pointer to the ChunkSegment.
             */
            ::VoxelCastle::World::ChunkSegment* getOrCreateSegment(int_fast32_t segmentYIndex);

            /**
             * @brief Converts world Y coordinate to segment Y index.
             * @param worldY The world Y-coordinate.
             * @return The segment Y index.
             */
            static int_fast32_t worldYToSegmentYIndex(int_fast64_t worldY);

            /**
             * @brief Converts world coordinates to local segment coordinates.
             *
             * @param worldX The world X-coordinate.
             * @param worldY The world Y-coordinate.
             * @param worldZ The world Z-coordinate.
             * @param segmentX Reference to store the local X-coordinate.
             * @param segmentY Reference to store the local Y-coordinate.
             * @param segmentZ Reference to store the local Z-coordinate.
             * @param columnWorldX The world X-coordinate of the column.
             * @param columnWorldZ The world Z-coordinate of the column.
             */
            static void worldToLocalSegmentCoords(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ,
                                                int_fast32_t& segmentX, int_fast32_t& segmentY, int_fast32_t& segmentZ,
                                                int_fast64_t columnWorldX, int_fast64_t columnWorldZ);

        private:
            ChunkColumnCoord m_coordinates; ///< World XZ coordinates of this column (at its origin 0,0).
            /**
             * @brief Map of segment Y-index to ChunkSegment.
             *
             * Y-index is worldY / SEGMENT_HEIGHT.
             */
            std::map<int_fast32_t, std::unique_ptr<::VoxelCastle::World::ChunkSegment>> m_segments;
        };

    } // namespace World
} // namespace VoxelCastle

#endif // CHUNK_COLUMN_H
