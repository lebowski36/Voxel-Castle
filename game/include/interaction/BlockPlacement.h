#ifndef BLOCK_PLACEMENT_H
#define BLOCK_PLACEMENT_H

#include <glm/glm.hpp>
#include "world/world_manager.h"

class Game;
class SpectatorCamera;

/**
 * Ray casting result containing information about what was hit
 */
struct RaycastResult {
    bool hit = false;                    // Whether we hit anything
    glm::ivec3 blockPosition;           // Position of the block that was hit
    glm::ivec3 adjacentPosition;       // Position where a new block would be placed
    glm::vec3 hitPoint;                // Exact world position where ray hit
    glm::vec3 normal;                  // Face normal of the hit surface
    float distance;                    // Distance from ray origin to hit point
};

/**
 * BlockPlacement handles ray casting and block placement/removal logic
 */
class BlockPlacement {
public:
    /**
     * Perform ray casting from camera center to find block intersections
     * @param camera Camera to cast ray from
     * @param worldManager World manager for voxel queries
     * @param maxDistance Maximum distance to cast ray
     * @return Ray casting result
     */
    static RaycastResult raycast(const SpectatorCamera* camera, 
                                VoxelCastle::World::WorldManager* worldManager, 
                                float maxDistance = 50.0f);

    /**
     * Handle mouse click events for block placement/removal
     * @param game Game instance
     * @param isLeftClick True for left click (place), false for right click (remove)
     */
    static void handleMouseClick(Game& game, bool isLeftClick);

    /**
     * Get the currently selected block type
     * @param game Game instance
     * @return Current block type
     */
    static VoxelEngine::World::VoxelType getCurrentBlockType(const Game& game);

    /**
     * Cycle to the next available block type (for mouse wheel)
     * @param game Game instance
     * @param forward True to go forward, false to go backward
     */
    static void cycleBlockType(Game& game, bool forward);

private:
    /**
     * Check if a ray intersects with a voxel at given position
     * @param rayOrigin Ray starting point
     * @param rayDirection Ray direction (normalized)
     * @param voxelPos Voxel position to check
     * @param hitPoint Output hit point if intersection occurs
     * @param hitNormal Output normal of hit face
     * @param distance Output distance to hit point
     * @return True if intersection occurs
     */
    static bool rayVoxelIntersection(const glm::vec3& rayOrigin, 
                                   const glm::vec3& rayDirection,
                                   const glm::ivec3& voxelPos,
                                   glm::vec3& hitPoint,
                                   glm::vec3& hitNormal,
                                   float& distance);

    /**
     * Validate that a block placement position is valid
     * @param game Game instance (for player position check)
     * @param position Position to validate
     * @param worldManager World manager for bounds checking
     * @return True if placement is valid
     */
    static bool isValidPlacement(const Game& game, 
                               const glm::ivec3& position,
                               VoxelCastle::World::WorldManager* worldManager);
    
    /**
     * Check if a chunk position is safe for block modification
     * @param worldManager World manager for chunk access
     * @param position World position to check
     * @return True if position is safe to modify
     */
    static bool isChunkPositionSafe(VoxelCastle::World::WorldManager* worldManager,
                                   const glm::ivec3& position);

    /**
     * Mark the chunk segment containing the given position as dirty for mesh regeneration
     * @param worldManager World manager for chunk access
     * @param position World position that was modified
     */
    static void markChunkDirtyForPosition(VoxelCastle::World::WorldManager* worldManager,
                                        const glm::ivec3& position);
};

#endif // BLOCK_PLACEMENT_H
