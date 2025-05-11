#include "world/chunk_column.h"
#include "world/chunk_segment.h" // For dimension constants
#include <stdexcept> // For std::out_of_range
#include <cmath>     // For std::floor

namespace VoxelCastle
{
    namespace World
    {

        ChunkColumn::ChunkColumn(int_fast64_t worldX, int_fast64_t worldZ)
            : m_coordinates{worldX, worldZ}
        {
        }

        ChunkColumnCoord ChunkColumn::getCoordinates() const
        {
            return m_coordinates;
        }

        ::VoxelEngine::World::Voxel ChunkColumn::getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const
        {
            // Check if the provided worldX and worldZ fall within this column's XZ footprint
            if (worldX < m_coordinates.x || worldX >= m_coordinates.x + ::VoxelCastle::World::SEGMENT_WIDTH ||
                worldZ < m_coordinates.z || worldZ >= m_coordinates.z + ::VoxelCastle::World::SEGMENT_DEPTH)
            {
                throw std::out_of_range("World XZ coordinates are outside this chunk column.");
            }

            int_fast32_t segmentYIndex = worldYToSegmentYIndex(worldY);
            const ::VoxelCastle::World::ChunkSegment* segment = getSegment(segmentYIndex);

            if (!segment)
            {
                return ::VoxelEngine::World::Voxel{0}; // Assuming Voxel{0} is air or default
            }

            int_fast32_t localX, localY, localZ;
            worldToLocalSegmentCoords(worldX, worldY, worldZ, localX, localY, localZ, m_coordinates.x, m_coordinates.z);
            
            return segment->getVoxel(localX, localY, localZ);
        }

        void ChunkColumn::setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const ::VoxelEngine::World::Voxel& voxel)
        {
            if (worldX < m_coordinates.x || worldX >= m_coordinates.x + ::VoxelCastle::World::SEGMENT_WIDTH ||
                worldZ < m_coordinates.z || worldZ >= m_coordinates.z + ::VoxelCastle::World::SEGMENT_DEPTH)
            {
                throw std::out_of_range("World XZ coordinates are outside this chunk column.");
            }

            int_fast32_t segmentYIndex = worldYToSegmentYIndex(worldY);
            ::VoxelCastle::World::ChunkSegment* segment = getOrCreateSegment(segmentYIndex);

            if (!segment) {
                throw std::runtime_error("Failed to get or create chunk segment.");
            }

            int_fast32_t localX, localY, localZ;
            worldToLocalSegmentCoords(worldX, worldY, worldZ, localX, localY, localZ, m_coordinates.x, m_coordinates.z);

            segment->setVoxel(localX, localY, localZ, voxel);
        }

        ::VoxelCastle::World::ChunkSegment* ChunkColumn::getSegment(int_fast32_t segmentYIndex)
        {
            auto it = m_segments.find(segmentYIndex);
            if (it != m_segments.end())
            {
                return it->second.get();
            }
            return nullptr;
        }

        const ::VoxelCastle::World::ChunkSegment* ChunkColumn::getSegment(int_fast32_t segmentYIndex) const
        {
            auto it = m_segments.find(segmentYIndex);
            if (it != m_segments.end())
            {
                return it->second.get();
            }
            return nullptr;
        }

        ::VoxelCastle::World::ChunkSegment* ChunkColumn::getOrCreateSegment(int_fast32_t segmentYIndex)
        {
            ::VoxelCastle::World::ChunkSegment* segment = getSegment(segmentYIndex);
            if (!segment)
            {
                m_segments[segmentYIndex] = std::make_unique<::VoxelCastle::World::ChunkSegment>(); // Default constructor
                segment = m_segments[segmentYIndex].get();
            }
            return segment;
        }

        int_fast32_t ChunkColumn::worldYToSegmentYIndex(int_fast64_t worldY)
        {
            return static_cast<int_fast32_t>(std::floor(static_cast<double>(worldY) / ::VoxelCastle::World::SEGMENT_HEIGHT));
        }

        void ChunkColumn::worldToLocalSegmentCoords(
            int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ,
            int_fast32_t& segmentX, int_fast32_t& segmentY, int_fast32_t& segmentZ,
            int_fast64_t columnWorldX, int_fast64_t columnWorldZ)
        {
            int_fast64_t relativeX = worldX - columnWorldX;
            int_fast64_t relativeZ = worldZ - columnWorldZ;

            segmentX = static_cast<int_fast32_t>(relativeX % ::VoxelCastle::World::SEGMENT_WIDTH);
            if (segmentX < 0) segmentX += ::VoxelCastle::World::SEGMENT_WIDTH;

            segmentY = static_cast<int_fast32_t>(worldY % ::VoxelCastle::World::SEGMENT_HEIGHT);
            if (segmentY < 0) segmentY += ::VoxelCastle::World::SEGMENT_HEIGHT;

            segmentZ = static_cast<int_fast32_t>(relativeZ % ::VoxelCastle::World::SEGMENT_DEPTH);
            if (segmentZ < 0) segmentZ += ::VoxelCastle::World::SEGMENT_DEPTH;
        }

    } // namespace World
} // namespace VoxelCastle
