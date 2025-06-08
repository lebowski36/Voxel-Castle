#include "world/chunk_column.h"
#include "world/chunk_segment.h" // For dimension constants
#include "world/world_coordinates.h" // For new coordinate system
#include <stdexcept> // For std::out_of_range
#include <cmath>     // For std::floor
#include <chrono>    // For timestamps
#include <iomanip>   // For time formatting
#include <sstream>   // For string streams
#include "utils/debug_logger_stub.h" // For debugging

namespace VoxelCastle
{
    namespace World
    {

        ChunkColumn::ChunkColumn(int_fast64_t worldX, int_fast64_t worldZ)
            : m_coordinates{worldX, worldZ}
        {
            // No longer pre-initialize all segments due to the large ±128 chunk range (256 total chunks).
            // Segments are now created on-demand via getOrCreateSegment() for better memory efficiency.
            // This supports the new coordinate system defined in world_coordinates.h.
            
            // Debug: Log chunk column creation with timestamp
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
            ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            
            VoxelCastle::Utils::logToFile("[" + ss.str() + "] [CHUNK_CREATE] ChunkColumn created at coordinates (" + 
                std::to_string(worldX) + ", " + std::to_string(worldZ) + ") - supports ±128 chunk range");
        }

        ChunkColumnCoord ChunkColumn::getCoordinates() const
        {
            return m_coordinates;
        }

        ::VoxelEngine::World::Voxel ChunkColumn::getVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ) const {
            // Use new coordinate system with ±128 chunk range
            using namespace VoxelCastle::World::WorldCoordinates;
            
            // Validate coordinates are within world bounds
            if (!IsValidBlockY(static_cast<int32_t>(worldY))) {
                return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
            }
            
            // Calculate segment Y index using new coordinate system
            int32_t segmentYIndex = BlockYToChunkY(static_cast<int32_t>(worldY));
            int32_t localY = BlockYToLocalY(static_cast<int32_t>(worldY));

            const ChunkSegment* segment = getSegment(segmentYIndex);
            if (segment) {
                // Calculate local X and Z coordinates
                int32_t localX = static_cast<int32_t>(worldX - m_coordinates.x);
                int32_t localZ = static_cast<int32_t>(worldZ - m_coordinates.z);
                
                // Ensure local coordinates are within valid chunk bounds
                if (localX >= 0 && localX < CHUNK_SIZE_VOXELS && 
                    localZ >= 0 && localZ < CHUNK_SIZE_VOXELS) {
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
                            std::to_string(worldY) + "," + std::to_string(worldZ) + ") chunkY=" + std::to_string(segmentYIndex) +
                            " local(" + std::to_string(localX) + "," + std::to_string(localY) + "," + std::to_string(localZ) + 
                            ") voxel=" + std::to_string(voxel.id));
                        getCount++;
                    }
                    
                    return voxel;
                }
            }
            // Segment doesn't exist or coordinates are out of bounds
            return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
        }

        void ChunkColumn::setVoxel(int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ, const VoxelEngine::World::Voxel& voxel) {
            // Use new coordinate system with ±128 chunk range
            using namespace VoxelCastle::World::WorldCoordinates;
            
            // Validate coordinates are within world bounds
            if (!IsValidBlockY(static_cast<int32_t>(worldY))) {
                return; // Silently ignore out-of-bounds coordinates
            }
            
            // Calculate segment Y index using new coordinate system
            int32_t segmentYIndex = BlockYToChunkY(static_cast<int32_t>(worldY));
            int32_t localY = BlockYToLocalY(static_cast<int32_t>(worldY));

            // Get or create the segment for this Y level
            ChunkSegment* segment = getOrCreateSegment(segmentYIndex);
            if (segment) {
                // Calculate local X and Z coordinates
                int32_t localX = static_cast<int32_t>(worldX - m_coordinates.x);
                int32_t localZ = static_cast<int32_t>(worldZ - m_coordinates.z);
                
                // Ensure local coordinates are within valid chunk bounds
                if (localX >= 0 && localX < CHUNK_SIZE_VOXELS && 
                    localZ >= 0 && localZ < CHUNK_SIZE_VOXELS) {
                    
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
                            std::to_string(worldY) + "," + std::to_string(worldZ) + ") chunkY=" + std::to_string(segmentYIndex) +
                            " local(" + std::to_string(localX) + "," + std::to_string(localY) + "," + std::to_string(localZ) + 
                            ") voxel=" + std::to_string(voxel.id));
                        setCount++;
                    }
                    segment->setVoxel(localX, localY, localZ, voxel);
                }
            }
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
            // Use new coordinate system for ±128 chunk range
            using namespace VoxelCastle::World::WorldCoordinates;
            return BlockYToChunkY(static_cast<int32_t>(worldY));
        }

        void ChunkColumn::worldToLocalSegmentCoords(
            int_fast64_t worldX, int_fast64_t worldY, int_fast64_t worldZ,
            int_fast32_t& segmentX, int_fast32_t& segmentY, int_fast32_t& segmentZ,
            int_fast64_t columnWorldX, int_fast64_t columnWorldZ)
        {
            // Use new coordinate system for ±128 chunk range
            using namespace VoxelCastle::World::WorldCoordinates;
            
            int_fast64_t relativeX = worldX - columnWorldX;
            int_fast64_t relativeZ = worldZ - columnWorldZ;

            segmentX = static_cast<int_fast32_t>(relativeX % CHUNK_SIZE_VOXELS);
            if (segmentX < 0) segmentX += CHUNK_SIZE_VOXELS;

            segmentY = BlockYToLocalY(static_cast<int32_t>(worldY));

            segmentZ = static_cast<int_fast32_t>(relativeZ % CHUNK_SIZE_VOXELS);
            if (segmentZ < 0) segmentZ += CHUNK_SIZE_VOXELS;
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
