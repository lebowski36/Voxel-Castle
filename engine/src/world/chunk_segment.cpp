#include "world/chunk_segment.h"
#include <stdexcept> // For std::out_of_range

namespace VoxelCastle
{
    namespace World
    {

        ChunkSegment::ChunkSegment()
        {
            // Default initialize all voxels. Assuming Voxel has a default constructor
            // that might set it to 'air' or an 'uninitialized' state.
            // For explicit initialization to 'air' (e.g., type_id 0):
            ::VoxelEngine::World::Voxel airVoxel{0}; // Assuming 0 is air
            m_voxels.fill(airVoxel);
        }

        ChunkSegment::ChunkSegment(const ::VoxelEngine::World::Voxel& initialVoxel)
        {
            m_voxels.fill(initialVoxel);
        }

        ::VoxelEngine::World::Voxel ChunkSegment::getVoxel(int_fast32_t x, int_fast32_t y, int_fast32_t z) const
        {
            if (!areCoordinatesValid(x, y, z))
            {
                // Option 1: Throw an exception
                throw std::out_of_range("Voxel coordinates are out of segment bounds.");
                // Option 2: Return a default/error Voxel type
                // return ::VoxelEngine::World::Voxel{ERROR_VOXEL_TYPE_ID}; 
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
            m_voxels[getIndex(x, y, z)] = voxel;
        }

        bool ChunkSegment::areCoordinatesValid(int_fast32_t x, int_fast32_t y, int_fast32_t z)
        {
            return x >= 0 && x < SEGMENT_WIDTH &&
                   y >= 0 && y < SEGMENT_HEIGHT &&
                   z >= 0 && z < SEGMENT_DEPTH;
        }

        std::size_t ChunkSegment::getIndex(int_fast32_t x, int_fast32_t y, int_fast32_t z)
        {
            // Using X-Major order: index = (x * HEIGHT * DEPTH) + (y * DEPTH) + z
            // Using Y-Major order: index = (y * WIDTH * DEPTH) + (x * DEPTH) + z
            // Using Z-Major order (or row-major like C arrays): index = (z * WIDTH * HEIGHT) + (y * WIDTH) + x
            // Let's stick to Z-Major as it's common for 3D arrays laid out linearly.
            // Or, if thinking [height][row][col] -> [y][x][z] or [y][z][x]
            // If m_voxels[y][x][z] was the mental model for a 3D std::array, then flat is y * (WIDTH*DEPTH) + x * DEPTH + z
            // If m_voxels[x][y][z] was the mental model, then flat is x * (HEIGHT*DEPTH) + y * DEPTH + z

            // The current constants are SEGMENT_WIDTH (X), SEGMENT_HEIGHT (Y), SEGMENT_DEPTH (Z).
            // Standard linear mapping for a 3D array A[H][W][D] with indices (y,x,z) is y*W*D + x*D + z.
            // Or for A[W][H][D] with indices (x,y,z) is x*H*D + y*D + z.
            // Let's assume indexing m_voxels[x][y][z] conceptually.
            // return static_cast<std::size_t>(x) * SEGMENT_HEIGHT * SEGMENT_DEPTH +
            //        static_cast<std::size_t>(y) * SEGMENT_DEPTH +
            //        static_cast<std::size_t>(z);

            // Let's use the layout that corresponds to iterating X, then Y, then Z (column-major style for the first dimension)
            // Or more simply, if we consider the dimensions as (dim1, dim2, dim3) and access (i, j, k)
            // index = i * (dim2*dim3) + j * dim3 + k
            // If our dimensions are (WIDTH, HEIGHT, DEPTH) for (X, Y, Z)
            // index = x * (HEIGHT*DEPTH) + y * DEPTH + z
            return static_cast<std::size_t>(x) * (SEGMENT_HEIGHT * SEGMENT_DEPTH) +
                   static_cast<std::size_t>(y) * SEGMENT_DEPTH +
                   static_cast<std::size_t>(z);
        }

    } // namespace World
} // namespace VoxelCastle
