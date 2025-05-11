#ifndef VOXELCASTLE_CHUNK_SEGMENT_H
#define VOXELCASTLE_CHUNK_SEGMENT_H

#include <array>
#include <cstddef> // For std::size_t
#include <cstdint> // For int_fast32_t
#include "world/voxel.h" // Defines ::VoxelEngine::World::Voxel

namespace VoxelCastle {
    namespace World {

        // Chunk Segment Dimensions
        constexpr int_fast32_t SEGMENT_WIDTH = 32;
        constexpr int_fast32_t SEGMENT_HEIGHT = 32;
        constexpr int_fast32_t SEGMENT_DEPTH = 32;

        class ChunkSegment {
        public:
            ChunkSegment();
            explicit ChunkSegment(const ::VoxelEngine::World::Voxel& initialVoxel);

            ::VoxelEngine::World::Voxel getVoxel(int_fast32_t x, int_fast32_t y, int_fast32_t z) const;
            void setVoxel(int_fast32_t x, int_fast32_t y, int_fast32_t z, const ::VoxelEngine::World::Voxel& voxel);

            static bool areCoordinatesValid(int_fast32_t x, int_fast32_t y, int_fast32_t z);

            static constexpr int_fast32_t getWidth() { return SEGMENT_WIDTH; }
            static constexpr int_fast32_t getHeight() { return SEGMENT_HEIGHT; }
            static constexpr int_fast32_t getDepth() { return SEGMENT_DEPTH; }

        private:
            std::array<::VoxelEngine::World::Voxel,
                       SEGMENT_WIDTH * SEGMENT_HEIGHT * SEGMENT_DEPTH> m_voxels;
            
            static std::size_t getIndex(int_fast32_t x, int_fast32_t y, int_fast32_t z);
        };

    } // namespace World
} // namespace VoxelCastle

#endif // VOXELCASTLE_CHUNK_SEGMENT_H
