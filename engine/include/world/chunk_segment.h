#ifndef VOXELCASTLE_CHUNK_SEGMENT_H
#define VOXELCASTLE_CHUNK_SEGMENT_H

#include <array>
#include <cstddef> // For std::size_t
#include <cstdint> // For int_fast32_t
#include "world/voxel.h" // Defines ::VoxelEngine::World::Voxel

/**
 * @brief Namespace for Voxel Castle specific game logic and world representation.
 */
namespace VoxelCastle {
/**
 * @brief Namespace for world-related structures and management within the Voxel Castle context.
 */
namespace World {

    // Chunk Segment Dimensions
    constexpr int_fast32_t SEGMENT_WIDTH = 32;
    constexpr int_fast32_t SEGMENT_HEIGHT = 32;
    constexpr int_fast32_t SEGMENT_DEPTH = 32;

    /**
     * @class ChunkSegment
     * @brief Represents a 32x32x32 segment of voxels within a ChunkColumn.
     *
     * A ChunkSegment is the smallest unit of voxel storage that is managed individually
     * for purposes like rendering and data streaming. It holds a 3D array of voxels.
     */
    class ChunkSegment {
    public:
        static const int CHUNK_WIDTH = 32;
        static const int CHUNK_HEIGHT = 32;
        static const int CHUNK_DEPTH = 32;

        /**
         * @brief Gets the dimension of the chunk segment based on the axis.
         * @param axis The axis (0 for X, 1 for Y, 2 for Z).
         * @return The dimension of the chunk segment for the given axis.
         */
        static int getDimension(int axis);

        /**
         * @brief Default constructor.
         *
         * Initializes a new ChunkSegment, typically filling it with VoxelEngine::World::VoxelType::AIR voxels.
         */
        ChunkSegment();

        /**
         * @brief Constructor that initializes the segment with a specific voxel type.
         *
         * @param initialVoxel The voxel type to initialize the segment with.
         */
        explicit ChunkSegment(const ::VoxelEngine::World::Voxel& initialVoxel);

        /**
         * @brief Gets the voxel at the specified local coordinates within this segment.
         *
         * Coordinates are local to the segment (0-31 for x, y, z).
         * @param x The local x-coordinate (0 to SEGMENT_WIDTH - 1).
         * @param y The local y-coordinate (0 to SEGMENT_HEIGHT - 1).
         * @param z The local z-coordinate (0 to SEGMENT_DEPTH - 1).
         * @return The VoxelEngine::World::Voxel at the given coordinates.
         * @throws std::out_of_range if coordinates are outside the segment bounds.
         */
        ::VoxelEngine::World::Voxel getVoxel(int_fast32_t x, int_fast32_t y, int_fast32_t z) const;

        /**
         * @brief Sets the voxel at the specified local coordinates within this segment.
         *
         * Coordinates are local to the segment (0-31 for x, y, z).
         * @param x The local x-coordinate (0 to SEGMENT_WIDTH - 1).
         * @param y The local y-coordinate (0 to SEGMENT_HEIGHT - 1).
         * @param z The local z-coordinate (0 to SEGMENT_DEPTH - 1).
         * @param voxel The VoxelEngine::World::Voxel to set at the given coordinates.
         * @throws std::out_of_range if coordinates are outside the segment bounds.
         */
        void setVoxel(int_fast32_t x, int_fast32_t y, int_fast32_t z, const ::VoxelEngine::World::Voxel& voxel);

        /**
         * @brief Checks if the given coordinates are valid within the segment.
         *
         * @param x The x-coordinate to check.
         * @param y The y-coordinate to check.
         * @param z The z-coordinate to check.
         * @return True if the coordinates are valid, false otherwise.
         */
        static bool areCoordinatesValid(int_fast32_t x, int_fast32_t y, int_fast32_t z);

        /**
         * @brief Gets the width of the chunk segment.
         * @return The width of the chunk segment.
         */
        static constexpr int_fast32_t getWidth() { return SEGMENT_WIDTH; }

        /**
         * @brief Gets the height of the chunk segment.
         * @return The height of the chunk segment.
         */
        static constexpr int_fast32_t getHeight() { return SEGMENT_HEIGHT; }

        /**
         * @brief Gets the depth of the chunk segment.
         * @return The depth of the chunk segment.
         */
        static constexpr int_fast32_t getDepth() { return SEGMENT_DEPTH; }

    private:
        /**
         * @brief 3D array storing the voxels in this segment.
         *
         * Voxels are stored in a flat std::array, and accessed via a 3D to 1D index mapping.
         * The size is SEGMENT_WIDTH * SEGMENT_HEIGHT * SEGMENT_DEPTH.
         */
        std::array<::VoxelEngine::World::Voxel,
                   SEGMENT_WIDTH * SEGMENT_HEIGHT * SEGMENT_DEPTH> m_voxels;

        /**
         * @brief Calculates the 1D array index for a given 3D local coordinate.
         * @param x The local x-coordinate.
         * @param y The local y-coordinate.
         * @param z The local z-coordinate.
         * @return The 1D index corresponding to the 3D coordinates.
         */
        static std::size_t getIndex(int_fast32_t x, int_fast32_t y, int_fast32_t z);
    };

} // namespace World
} // namespace VoxelCastle

#endif // VOXELCASTLE_CHUNK_SEGMENT_H
