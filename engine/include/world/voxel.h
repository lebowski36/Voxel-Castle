// filepath: engine/include/world/voxel.h
#ifndef VOXEL_ENGINE_VOXEL_H
#define VOXEL_ENGINE_VOXEL_H

#include <cstdint>

/**
 * @brief Namespace for core voxel engine functionalities, including world representation.
 */
namespace VoxelEngine {
/**
 * @brief Namespace for world-related structures and management, such as voxels, chunks, and world data.
 */
namespace World {

/**
 * @struct Voxel
 * @brief Represents a single voxel in the game world.
 *
 * This structure defines the basic unit of the voxel world. Currently, it only
 * contains an ID representing the type of the voxel.
 */
struct Voxel {
    /**
     * @brief The type identifier for this voxel.
     *
     * This ID typically corresponds to an entry in an enum or a lookup table
     * that defines the voxel's properties (e.g., material, texture, solidity).
     * See VoxelType enum in voxel_types.h.
     */
    uint8_t id;
    // Future additions could include:
    // uint8_t light_level;
    // uint8_t orientation;
    // etc.

    /**
     * @brief Default constructor. Initializes voxel to a default state (e.g., AIR).
     *
     * By default, a voxel is initialized with an ID of 0, which typically
     * represents 'AIR' or an empty voxel.
     */
    Voxel() : id(0) {}

    /**
     * @brief Constructs a Voxel with a specific type ID.
     * @param type_id The ID representing the voxel's type.
     */
    explicit Voxel(uint8_t type_id) : id(type_id) {}
};

} // namespace World
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_VOXEL_H
