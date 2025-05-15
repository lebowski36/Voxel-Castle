
#ifndef VOXELCASTLE_VOXEL_TYPES_H
#define VOXELCASTLE_VOXEL_TYPES_H

// Standard library includes first
#include <cstdint>

namespace VoxelEngine {
namespace World {

/**
 * @enum VoxelType
 * @brief Defines the different types of voxels available in the game world.
 *
 * Each voxel type is represented by an 8-bit unsigned integer. This enum provides
 * named constants for common voxel types, making the code more readable and maintainable.
 * The underlying type is `uint8_t`, allowing for up to 256 distinct voxel types.
 */
enum class VoxelType : uint8_t {
    AIR = 0,   /**< Represents an empty space, typically transparent and non-collidable. */
    STONE,     /**< Represents stone, a common solid building material. */
    DIRT,      /**< Represents dirt, another common natural material. */
    GRASS,     /**< Represents grass, often found on top of dirt blocks. */
    WOOD,      /**< Represents wood, from trees, used for building. */
    LEAVES,    /**< Represents leaves, part of trees, often semi-transparent. */
    WATER,     /**< Represents water, a fluid with specific behaviors. */
    SAND,      /**< Represents sand, a granular material affected by gravity. */
    // Add more types as needed (e.g., GLASS, BRICK, LAVA, etc.)
};

} // namespace World
} // namespace VoxelEngine

#endif //VOXELCASTLE_VOXEL_TYPES_H
