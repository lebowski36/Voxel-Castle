#include "world/chunk_segment.h"
#include "world/voxel.h" // For VoxelType
#include <stdexcept> // For std::out_of_range
#include "rendering/mesh_builder.h" // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/texture_atlas.h" // For VoxelEngine::Rendering::TextureAtlas
#include "world/world_manager.h" // For VoxelCastle::World::WorldManager
#include <chrono> // For timing
#include <iostream> // For logging time
#include <iomanip> // For timestamp formatting
#include <sstream> // For string streams

namespace VoxelCastle
{
    namespace World
    {
        // Helper function to get current timestamp string
        static std::string getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
            ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
            return ss.str();
        }

        // Static variables for aggregated logging
        static int g_rebuild_count_since_last_log = 0;
        static long long g_accumulated_rebuild_time_micros = 0;
        static const int G_LOG_INTERVAL_REBUILDS = 100; // Log every 100 rebuilds

        ChunkSegment::ChunkSegment()
        {
            // Default initialize all voxels. Assuming Voxel has a default constructor
            // that might set it to 'air' or an 'uninitialized' state.
            // For explicit initialization to 'air' (e.g., type_id 0):
            ::VoxelEngine::World::Voxel airVoxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)}; // Ensure AIR is used
            m_voxels.fill(airVoxel);
            mIsDirty = true; // New segments are dirty by default
            // mMesh is implicitly nullptr by default for std::unique_ptr
        }

        ChunkSegment::ChunkSegment(const ::VoxelEngine::World::Voxel& initialVoxel)
        {
            m_voxels.fill(initialVoxel);
            mIsDirty = true; // New segments are dirty by default
        }

        ::VoxelEngine::World::Voxel ChunkSegment::getVoxel(int_fast32_t x, int_fast32_t y, int_fast32_t z) const
        {
            if (!areCoordinatesValid(x, y, z))
            {
                // Return AIR for out-of-bounds access, crucial for meshing algorithms
                return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
            }
            return m_voxels[getIndex(x, y, z)];
        }

        void ChunkSegment::setVoxel(int_fast32_t x, int_fast32_t y, int_fast32_t z, const ::VoxelEngine::World::Voxel& voxel)
        {
            if (!areCoordinatesValid(x, y, z))
            {
                // Option 1: Throw an exception
                throw std::out_of_range("Voxel coordinates are out of segment bounds.");
                // Option 2: Do nothing or log an error
                // return;
            }
            // Only mark dirty if the voxel actually changes.
            if (m_voxels[getIndex(x, y, z)].id != voxel.id) {
                m_voxels[getIndex(x, y, z)] = voxel;
                markDirty(); // Mark the segment as dirty if a voxel changes
            }
        }


        void ChunkSegment::markDirty(bool dirty) {
            if (dirty) {
                if (mIsDirty) {
                    // Already dirty, no-op (prevents feedback loop and log spam)
                    return;
                }
                if (mIsRebuildingMesh) {
                    std::cout << "[ERROR] markDirty(true) called during mesh rebuild! This may cause a feedback loop." << std::endl;
                }
            }
            mIsDirty = dirty;
        }

        bool ChunkSegment::isDirty() const {
            return mIsDirty;
        }

        bool ChunkSegment::isEmpty() const {
            for (const auto& voxel : m_voxels) {
                if (voxel.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Rebuilds the renderable mesh for this chunk segment.
         *
         * This method is called when the segment is marked as dirty (e.g., after voxel changes).
         * It utilizes the provided `TextureAtlas` for texture coordinate information and the
         * `MeshBuilder` to construct the actual mesh geometry (e.g., using Greedy Meshing).
         *
         * If no mesh exists yet (i.e., `mMesh` is `nullptr`), a new `VoxelMesh` instance is created.
         * The chosen meshing algorithm (e.g., `buildGreedyMesh`) from the `meshBuilder` is then invoked,
         * passing this `ChunkSegment` and the `atlas` as sources of voxel and texture data.
         * The resulting `VoxelMesh` data is assigned to this segment's `mMesh`.
         *
         * After the mesh is successfully rebuilt, the segment's dirty flag is cleared.
         * Performance timing for the mesh generation is also logged to standard output.
         *
         * @param atlas The `TextureAtlas` containing texture information for different voxel types.
         * @param meshBuilder The `MeshBuilder` responsible for generating the mesh vertices and indices.
         * @param columnWorldX The world X-coordinate of the chunk column this segment belongs to.
         * @param segmentYIndex The Y-index of this segment within its column.
         * @param columnWorldZ The world Z-coordinate of the chunk column this segment belongs to.
         */
        void ChunkSegment::rebuildMesh(VoxelEngine::Rendering::TextureAtlas& atlas, 
                                       VoxelEngine::Rendering::MeshBuilder& meshBuilder,
                                       int_fast64_t columnWorldX,
                                       int_fast32_t segmentYIndex,
                                       int_fast64_t columnWorldZ,
                                       const WorldManager* worldManager) {
            
            std::string timestamp = getCurrentTimestamp();
            
            // [CHUNK_DEBUG] Track chunk data integrity through pipeline
            std::cout << "[CHUNK_DEBUG:" << timestamp << "] MESH_REBUILD_START for chunk (colX=" 
                      << columnWorldX << ", segY=" << segmentYIndex << ", colZ=" << columnWorldZ << ")" << std::endl;
            
            // Sample voxels at key positions to track data integrity
            std::cout << "[CHUNK_DEBUG:" << timestamp << "] VOXEL_SAMPLING:" << std::endl;
            std::cout << "  Corner (0,0,0): type=" << static_cast<int>(getVoxel(0,0,0).id) << std::endl;
            std::cout << "  Center (16,16,16): type=" << static_cast<int>(getVoxel(16,16,16).id) << std::endl;
            std::cout << "  Corner (31,31,31): type=" << static_cast<int>(getVoxel(31,31,31).id) << std::endl;
            
            // Count non-air voxels
            int nonAirCount = 0;
            int stoneCount = 0, grassCount = 0, dirtCount = 0;
            for (int x = 0; x < CHUNK_WIDTH; x += 8) { // Sample every 8th voxel
                for (int y = 0; y < CHUNK_HEIGHT; y += 8) {
                    for (int z = 0; z < CHUNK_DEPTH; z += 8) {
                        auto voxel = getVoxel(x, y, z);
                        if (voxel.id != static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)) {
                            nonAirCount++;
                            if (voxel.id == static_cast<uint8_t>(::VoxelEngine::World::VoxelType::STONE)) stoneCount++;
                            else if (voxel.id == static_cast<uint8_t>(::VoxelEngine::World::VoxelType::GRASS)) grassCount++;
                            else if (voxel.id == static_cast<uint8_t>(::VoxelEngine::World::VoxelType::DIRT)) dirtCount++;
                        }
                    }
                }
            }
            std::cout << "  Sampled counts: nonAir=" << nonAirCount << ", stone=" << stoneCount 
                      << ", grass=" << grassCount << ", dirt=" << dirtCount << std::endl;
            
            // [DEBUG] Only print summary every N rebuilds (see below)
            mIsRebuildingMesh = true;
            static int logCounter = 0; // Static counter to track logging frequency
            const int logInterval = 500; // Log every 500 rebuilds

            auto startTime = std::chrono::high_resolution_clock::now();

            if (!mMesh) {
                mMesh = std::make_unique<VoxelEngine::Rendering::VoxelMesh>();
            }

            // Lambda: convert local segment (x, y, z) to world coordinates, then use worldManager->getVoxel
            // This lambda ensures cross-chunk and world-edge visibility checks. Out-of-bounds is always treated as air.

            // Construct chunk coordinates for the MeshBuilder
            glm::ivec3 currentChunkCoords(static_cast<int>(columnWorldX), 
                                        static_cast<int>(segmentYIndex), 
                                        static_cast<int>(columnWorldZ));

            // Using Two-Phase Greedy meshing for improved performance and quality
            // To revert to legacy greedy meshing, replace buildTwoPhaseGreedyMesh with buildGreedyMesh
            *mMesh = meshBuilder.buildTwoPhaseGreedyMesh(*this, atlas, [=](int x, int y, int z) {
                int_fast64_t worldX = columnWorldX * CHUNK_WIDTH + x;
                int_fast64_t worldY = segmentYIndex * CHUNK_HEIGHT + y;
                int_fast64_t worldZ = columnWorldZ * CHUNK_DEPTH + z;
                auto mod = [](int_fast64_t a, int_fast64_t b) -> int_fast32_t {
                    int_fast32_t r = static_cast<int_fast32_t>(a % b);
                    return r < 0 ? r + b : r;
                };
                if (worldManager) {
                    auto* column = worldManager->getChunkColumn(
                        VoxelCastle::World::WorldManager::worldToColumnBaseX(worldX),
                        VoxelCastle::World::WorldManager::worldToColumnBaseZ(worldZ)
                    );
                    if (!column) {
                        return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
                    }
                    int_fast32_t segY = static_cast<int_fast32_t>(worldY / VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
                    auto* segment = column->getSegment(segY);
                    if (!segment) {
                        return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
                    }
                    int_fast32_t localX = mod(worldX - column->getBaseX(), VoxelCastle::World::ChunkSegment::CHUNK_WIDTH);
                    int_fast32_t localY = mod(worldY, VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
                    int_fast32_t localZ = mod(worldZ - column->getBaseZ(), VoxelCastle::World::ChunkSegment::CHUNK_DEPTH);
                    // Defensive: ensure local indices are in bounds
                    if (localX < 0 || localX >= VoxelCastle::World::ChunkSegment::CHUNK_WIDTH ||
                        localY < 0 || localY >= VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT ||
                        localZ < 0 || localZ >= VoxelCastle::World::ChunkSegment::CHUNK_DEPTH) {
                        return ::VoxelEngine::World::Voxel{static_cast<uint8_t>(::VoxelEngine::World::VoxelType::AIR)};
                    }
                    return segment->getVoxel(localX, localY, localZ);
                } else {
                    return this->getVoxel(x, y, z);
                }
            }, currentChunkCoords); // Pass currentChunkCoords as the last argument
            mMesh->setInitialized(true);
            
            std::cout << "[CHUNK_DEBUG:" << timestamp << "] MESH_REBUILD_COMPLETE:" << std::endl;
            std::cout << "  Vertices: " << mMesh->vertices.size() << ", Indices: " << mMesh->indices.size() << std::endl;
            if (mMesh->vertices.size() == 0) {
                std::cout << "  [WARNING] Mesh has 0 vertices! Segment may be empty or meshing failed." << std::endl;
            }
            
            // Log world position for verification
            float segmentWorldX = static_cast<float>(columnWorldX * VoxelCastle::World::ChunkSegment::CHUNK_WIDTH);
            float segmentWorldY = static_cast<float>(segmentYIndex * VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
            float segmentWorldZ = static_cast<float>(columnWorldZ * VoxelCastle::World::ChunkSegment::CHUNK_DEPTH);
            std::cout << "  World position: (" << segmentWorldX << ", " << segmentWorldY << ", " << segmentWorldZ << ")" << std::endl;

            mMesh->setWorldPosition(glm::vec3(segmentWorldX, segmentWorldY, segmentWorldZ));
            markDirty(false);
            std::cout << "[CHUNK_DEBUG:" << timestamp << "] Segment marked clean after mesh rebuild." << std::endl;
            mIsRebuildingMesh = false;

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

            g_accumulated_rebuild_time_micros += duration.count();
            g_rebuild_count_since_last_log++;

            if (g_rebuild_count_since_last_log >= logInterval) {
                logCounter++;
                std::cout << "[ChunkSegment] Aggregated RebuildMesh Stats (" << g_rebuild_count_since_last_log << " rebuilds):" << std::endl;
                std::cout << "  Sample Debug Info (last in batch):" << std::endl;
                std::cout << "    Indices: colX=" << columnWorldX << ", segY=" << segmentYIndex << ", colZ=" << columnWorldZ << std::endl;
                std::cout << "    SegmentDims: W=" << VoxelCastle::World::ChunkSegment::CHUNK_WIDTH 
                          << ", H=" << VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT 
                          << ", D=" << VoxelCastle::World::ChunkSegment::CHUNK_DEPTH << std::endl;
                std::cout << "    Calculated World Pos: (" << segmentWorldX << ", " << segmentWorldY << ", " << segmentWorldZ << ")" << std::endl;
                std::cout << "  Total Time for Batch: " << g_accumulated_rebuild_time_micros << " microseconds." << std::endl;
                std::cout << "  Average Time per Rebuild: " << (g_accumulated_rebuild_time_micros / g_rebuild_count_since_last_log) << " microseconds." << std::endl;

                g_accumulated_rebuild_time_micros = 0;
                g_rebuild_count_since_last_log = 0;
            }
        }

        const VoxelEngine::Rendering::VoxelMesh* ChunkSegment::getMesh() const {
            return mMesh.get();
        }

        bool ChunkSegment::areCoordinatesValid(int_fast32_t x, int_fast32_t y, int_fast32_t z)
        {
            return x >= 0 && x < SEGMENT_WIDTH &&
                   y >= 0 && y < SEGMENT_HEIGHT &&
                   z >= 0 && z < SEGMENT_DEPTH;
        }

        std::size_t ChunkSegment::getIndex(int_fast32_t x, int_fast32_t y, int_fast32_t z)
        {
            return static_cast<std::size_t>(x) * (SEGMENT_HEIGHT * SEGMENT_DEPTH) +
                   static_cast<std::size_t>(y) * SEGMENT_DEPTH +
                   static_cast<std::size_t>(z);
        }

        int ChunkSegment::getDimension(int axis) {
            switch (axis) {
                case 0: return VoxelCastle::World::ChunkSegment::getWidth();  // X-axis
                case 1: return VoxelCastle::World::ChunkSegment::getHeight(); // Y-axis
                case 2: return VoxelCastle::World::ChunkSegment::getDepth();  // Z-axis
                default: throw std::out_of_range("Invalid axis for getDimension. Must be 0, 1, or 2.");
            }
        }

    } // namespace World
} // namespace VoxelCastle
