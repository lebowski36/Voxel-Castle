#include "world/chunk_column.h"
#include "world/chunk_segment.h" // For dimension constants
#include <stdexcept> // For std::out_of_range
#include <cmath>     // For std::floor
#include <chrono>    // For timestamps
#include <iomanip>   // For time formatting
#include <sstream>   // For string streams
#include "../../game/include/utils/debug_logger.h" // For debugging

namespace VoxelCastle
{
    namespace World
    {

        ChunkColumn::ChunkColumn(int_fast64_t worldX, int_fast64_t worldZ)
            : m_coordinates{worldX, worldZ}
        {
            // Pre-initialize segments for the defined column height
            for (uint8_t i = 0; i < CHUNKS_PER_COLUMN; ++i) {
                m_segments[i] = std::make_unique<ChunkSegment>();
            }
            
            // Debug: Log chunk column creation with timestamp
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
            ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            
            VoxelCastle::Utils::logToFile("[" + ss.str() + "] [CHUNK_CREATE] ChunkColumn created at coordinates (" + 
                std::to_string(worldX) + ", " + std::to_string(worldZ) + ")");
        }

        ChunkColumnCoord ChunkColumn::getCoordinates() const
        {
            return m_coordinates;
        }

        ::VoxelEngine::World::Voxel ChunkColumn::getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const {
            // Calculate segment Y index and local Y coordinate
            int_fast64_t segmentYIndex = worldY / ChunkSegment::CHUNK_HEIGHT;
            uint8_t localY = static_cast<uint8_t>(worldY % ChunkSegment::CHUNK_HEIGHT);

            if (segmentYIndex < 0 || segmentYIndex >= CHUNKS_PER_COLUMN) {
                // Y-coordinate is outside the vertical bounds of this column
                return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
            }

            const ChunkSegment* segment = getSegmentByIndex(static_cast<uint8_t>(segmentYIndex));
            if (segment) {
                // Calculate local X and Z coordinates
                uint8_t localX = static_cast<uint8_t>(worldX - m_coordinates.x); // Changed m_baseX
                uint8_t localZ = static_cast<uint8_t>(worldZ - m_coordinates.z); // Changed m_baseZ
                auto voxel = segment->getVoxel(localX, localY, localZ);
                
                // Debug: Log first few voxel reads per chunk to detect data corruption
                static int getCount = 0;
                if (getCount < 10) {
                    auto now = std::chrono::system_clock::now();
                    auto time_t = std::chrono::system_clock::to_time_t(now);
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                    std::stringstream ss;
                    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
                    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
                    
                    VoxelCastle::Utils::logToFile("[" + ss.str() + "] [CHUNK_GET] Chunk(" + std::to_string(m_coordinates.x) + "," + 
                        std::to_string(m_coordinates.z) + ") world(" + std::to_string(worldX) + "," + 
                        std::to_string(worldY) + "," + std::to_string(worldZ) + ") local(" + 
                        std::to_string(localX) + "," + std::to_string(localY) + "," + std::to_string(localZ) + 
                        ") voxel=" + std::to_string(voxel.id));
                    getCount++;
                }
                
                return voxel;
            }
            // Segment doesn't exist (should not happen if constructor pre-allocates all)
            return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
        }

        void ChunkColumn::setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const VoxelEngine::World::Voxel& voxel) {
            // Calculate segment Y index and local Y coordinate
            int_fast64_t segmentYIndex = worldY / ChunkSegment::CHUNK_HEIGHT;
            uint8_t localY = static_cast<uint8_t>(worldY % ChunkSegment::CHUNK_HEIGHT);

            if (segmentYIndex < 0 || segmentYIndex >= CHUNKS_PER_COLUMN) {
                // Y-coordinate is outside the vertical bounds of this column, do nothing or log error
                return;
            }

            ChunkSegment* segment = getSegmentByIndex(static_cast<uint8_t>(segmentYIndex));
            if (segment) {
                // Calculate local X and Z coordinates
                uint8_t localX = static_cast<uint8_t>(worldX - m_coordinates.x); // Changed m_baseX
                uint8_t localZ = static_cast<uint8_t>(worldZ - m_coordinates.z); // Changed m_baseZ
                
                // Debug: Log voxel setting to track terrain generation
                static int setCount = 0;
                if (setCount < 50) {  // Log more sets since this is where terrain is generated
                    auto now = std::chrono::system_clock::now();
                    auto time_t = std::chrono::system_clock::to_time_t(now);
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
                    std::stringstream ss;
                    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
                    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
                    
                    VoxelCastle::Utils::logToFile("[" + ss.str() + "] [CHUNK_SET] Chunk(" + std::to_string(m_coordinates.x) + "," + 
                        std::to_string(m_coordinates.z) + ") world(" + std::to_string(worldX) + "," + 
                        std::to_string(worldY) + "," + std::to_string(worldZ) + ") local(" + 
                        std::to_string(localX) + "," + std::to_string(localY) + "," + std::to_string(localZ) + 
                        ") voxel=" + std::to_string(voxel.id));
                    setCount++;
                }
                segment->setVoxel(localX, localY, localZ, voxel);
            }
            // Else: Segment doesn't exist (should not happen if constructor pre-allocates all), do nothing or log error
        }

        ChunkSegment* ChunkColumn::getSegmentByIndex(uint8_t segmentYIndex) {
            if (segmentYIndex >= CHUNKS_PER_COLUMN) {
                // Log error or handle out of bounds access
                return nullptr;
            }
            // m_segments stores unique_ptr, so .get() returns raw pointer
            // Accessing map with [] is fine for non-const version
            return m_segments[segmentYIndex].get();
        }

        const ChunkSegment* ChunkColumn::getSegmentByIndex(uint8_t segmentYIndex) const {
            if (segmentYIndex >= CHUNKS_PER_COLUMN) {
                // Log error or handle out of bounds access
                return nullptr;
            }
            // Use .at() for const-correct access to the map
            // .at() throws std::out_of_range if key not found, which is fine here
            // as we pre-allocate all segments.
            try {
                return m_segments.at(segmentYIndex).get();
            } catch (const std::out_of_range& oor) {
                // This should ideally not happen if constructor pre-allocates all segments
                // and CHUNKS_PER_COLUMN is the source of truth for map keys.
                // Consider logging an error here.
                return nullptr;
            }
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
                m_segments[segmentYIndex] = std::make_unique<::VoxelCastle::World::ChunkSegment>(); 
                segment = m_segments[segmentYIndex].get();
            }
            return segment;
        }

        int_fast32_t ChunkColumn::worldYToSegmentYIndex(int_fast64_t worldY)
        {
            int_fast32_t index = static_cast<int_fast32_t>(std::floor(static_cast<double>(worldY) / ::VoxelCastle::World::SEGMENT_HEIGHT));
            return index;
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

        void ChunkColumn::markAllSegmentsDirty() {
            for (auto& pair : m_segments) {
                if (pair.second) {
                    pair.second->markDirty(true);
                }
            }
        }

    } // namespace World
} // namespace VoxelCastle
