// filepath: engine/include/world/voxel.h
#ifndef VOXEL_ENGINE_VOXEL_H
#define VOXEL_ENGINE_VOXEL_H

#include <cstdint>
#include "world/voxel_types.h" // Added to ensure VoxelEngine::World::VoxelType is declared
#include <glm/glm.hpp>      // For glm::ivec3 and glm::vec3
#include <glm/vec3.hpp>     // For glm::vec3 specifically (can be redundant if glm/glm.hpp is used)
#include <glm/gtc/type_ptr.hpp> // For type_ptr if needed, good to have with GLM

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

/**
 * @struct Voxel
 * @brief Represents a single voxel in the game world.
 *
 * Contains the type ID and packed lighting information.
 * Lighting is stored as a single byte: upper 4 bits = sunlight, lower 4 bits = block light.
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

    /**
     * @brief Packed lighting information.
     * Upper 4 bits: sunlight (0-15), lower 4 bits: block light (0-15).
     */
    uint8_t light_level;

    /**
     * @brief Default constructor. Initializes voxel to a default state (e.g., AIR, no light).
     */
    Voxel() : id(0), light_level(0) {}

    /**
     * @brief Constructs a Voxel with a specific type ID and optional light level.
     * @param type_id The ID representing the voxel's type.
     * @param light The packed light value (default 0).
     */
    explicit Voxel(uint8_t type_id, uint8_t light = 0) : id(type_id), light_level(light) {}

    // --- Lighting helpers ---
    static constexpr uint8_t MAX_LIGHT = 15;

    void setSunlight(uint8_t level) {
        light_level = (light_level & 0x0F) | ((level & 0x0F) << 4);
    }
    uint8_t getSunlight() const {
        return (light_level >> 4) & 0x0F;
    }
    void setBlockLight(uint8_t level) {
        light_level = (light_level & 0xF0) | (level & 0x0F);
    }
    uint8_t getBlockLight() const {
        return light_level & 0x0F;
    }
};

/**
 * @typedef VoxelPosition
 * @brief Represents the position of a voxel in the world using GLM's ivec3.
 */
using VoxelPosition = glm::ivec3;

/**
 * @typedef Normal
 * @brief Represents a normal vector using GLM's vec3.
 */
using Normal = glm::vec3;

} // namespace World
} // namespace VoxelEngine

#endif //VOXEL_ENGINE_VOXEL_H
