#ifndef VOXELCASTLE_VOXEL_FACE_PATTERNS_H
#define VOXELCASTLE_VOXEL_FACE_PATTERNS_H

#include "voxel_types.h"
#include <unordered_map>
#include <cstdint>

namespace VoxelEngine {
namespace World {

/**
 * @enum FacePattern
 * @brief Defines how textures are applied to block faces for efficient atlas packing
 * 
 * This classification system determines how many texture atlas slots each block type requires
 * and enables efficient multi-atlas texture packing.
 */
enum class FacePattern : uint8_t {
    /**
     * All 6 faces use the same texture
     * Examples: Stone, metal blocks, gems, most processed materials
     * Atlas Usage: 1 slot in main atlas only
     */
    UNIFORM = 0,
    
    /**
     * Top and bottom faces use the same texture, all 4 side faces use a different texture
     * Examples: Wood logs (end grain on top/bottom, bark on sides)
     * Atlas Usage: 1 slot in main atlas (top/bottom), 1 slot in side atlas (sides)
     */
    TOP_BOTTOM_DIFFERENT = 1,
    
    /**
     * Top, sides, and bottom all use different textures
     * Examples: Grass block (green top, dirt sides, dirt bottom)
     * Atlas Usage: 1 slot in main atlas (top), 1 slot in side atlas (sides), 1 slot in bottom atlas (bottom)
     */
    ALL_DIFFERENT = 2,
    
    /**
     * Each face can have a unique texture with directional consideration
     * Examples: Doors, windows, mechanisms with orientation
     * Atlas Usage: Multiple slots across atlases for front/back/left/right faces
     * Note: This pattern is reserved for future directional block implementation
     */
    DIRECTIONAL = 3,
    
    /**
     * All 6 faces use different textures (advanced blocks)
     * Examples: Furnaces (front face different), multiblocks, complex machinery
     * Atlas Usage: 6 slots distributed across atlases (top, bottom, front, back, left, right)
     */
    ALL_FACES_DIFFERENT = 4
};

/**
 * @brief Face enumeration for texture lookup
 */
enum class Face : uint8_t {
    TOP = 0,
    BOTTOM = 1,
    FRONT = 2,
    BACK = 3,
    LEFT = 4,
    RIGHT = 5
};

/**
 * @brief Block face pattern configuration mapping
 * 
 * This map defines the face pattern for all 256 block types, enabling the texture
 * atlas system to determine the optimal packing strategy for each block.
 */
extern const std::unordered_map<VoxelType, FacePattern> BLOCK_FACE_PATTERNS;

/**
 * @brief Get the face pattern for a given voxel type
 * @param voxelType The voxel type to query
 * @return The face pattern for the voxel type
 */
FacePattern getFacePattern(VoxelType voxelType);

/**
 * @brief Get the number of atlas slots required for a face pattern
 * @param pattern The face pattern to query
 * @return The number of texture atlas slots required
 */
uint8_t getAtlasSlotCount(FacePattern pattern);

/**
 * @brief Check if a face pattern requires a side atlas
 * @param pattern The face pattern to check
 * @return true if the pattern requires side atlas textures
 */
bool requiresSideAtlas(FacePattern pattern);

/**
 * @brief Check if a face pattern requires a bottom atlas
 * @param pattern The face pattern to check
 * @return true if the pattern requires bottom atlas textures
 */
bool requiresBottomAtlas(FacePattern pattern);

} // namespace World
} // namespace VoxelEngine

#endif // VOXELCASTLE_VOXEL_FACE_PATTERNS_H
