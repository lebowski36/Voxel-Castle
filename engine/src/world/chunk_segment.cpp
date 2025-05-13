#include "world/chunk_segment.h"
#include <stdexcept> // For std::out_of_range
#include "rendering/mesh_builder.h" // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/texture_atlas.h" // For VoxelEngine::Rendering::TextureAtlas

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

        void ChunkSegment::rebuildMesh(VoxelEngine::Rendering::TextureAtlas& atlas, VoxelEngine::Rendering::MeshBuilder& meshBuilder) {
            if (!mMesh) {
                mMesh = std::make_unique<VoxelEngine::Rendering::VoxelMesh>();
            }
            // For now, let's use Greedy Meshing as it's the more advanced one we have.
            // If issues arise, we can switch to buildNaiveMesh for simpler debugging.
            *mMesh = meshBuilder.buildGreedyMesh(*this, atlas);
            // Potentially, MeshBuilder methods could take a VoxelMesh& to fill directly
            // instead of returning by value, to avoid a copy if VoxelMesh is large.
            // For now, the assignment from the returned VoxelMesh is fine.

            markDirty(false); // Mesh is now up-to-date
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
