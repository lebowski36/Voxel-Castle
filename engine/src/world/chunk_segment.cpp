#include "world/chunk_segment.h"
#include <stdexcept> // For std::out_of_range
#include "rendering/mesh_builder.h" // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/texture_atlas.h" // For VoxelEngine::Rendering::TextureAtlas
#include <chrono> // For timing
#include <iostream> // For logging time

namespace VoxelCastle
{
    namespace World
    {

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
            mIsDirty = dirty;
        }

        bool ChunkSegment::isDirty() const {
            return mIsDirty;
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
                                       int_fast64_t columnWorldZ) {
            auto startTime = std::chrono::high_resolution_clock::now();

            if (!mMesh) {
                mMesh = std::make_unique<VoxelEngine::Rendering::VoxelMesh>();
            }
            
            *mMesh = meshBuilder.buildGreedyMesh(*this, atlas);
            mMesh->setInitialized(true); 

            // Calculate and set the world position for this mesh
            float segmentWorldX = static_cast<float>(columnWorldX); // SEGMENT_WIDTH is implicitly handled by columnWorldX being the base
            float segmentWorldY = static_cast<float>(segmentYIndex * SEGMENT_HEIGHT);
            float segmentWorldZ = static_cast<float>(columnWorldZ); // SEGMENT_DEPTH is implicitly handled by columnWorldZ being the base

            // DEBUG: Print out the inputs and calculated world position
            std::cout << "[ChunkSegment::rebuildMesh] Debug Info:" << std::endl;
            std::cout << "  Indices: colX=" << columnWorldX << ", segY=" << segmentYIndex << ", colZ=" << columnWorldZ << std::endl;
            std::cout << "  SegmentDims: W=" << SEGMENT_WIDTH << ", H=" << SEGMENT_HEIGHT << ", D=" << SEGMENT_DEPTH << std::endl;
            std::cout << "  Calculated World Pos: (" << segmentWorldX << ", " << segmentWorldY << ", " << segmentWorldZ << ")" << std::endl;

            mMesh->setWorldPosition(glm::vec3(segmentWorldX, segmentWorldY, segmentWorldZ));

            markDirty(false); 

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            std::cout << "[ChunkSegment] RebuildMesh for segment took " << duration.count() << " microseconds.\n";
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
