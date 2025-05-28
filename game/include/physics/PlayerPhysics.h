#ifndef PLAYER_PHYSICS_H
#define PLAYER_PHYSICS_H

#include <glm/glm.hpp>
#include "../core/CameraMode.h"

class Game;
class SpectatorCamera;
namespace VoxelCastle { namespace World { class WorldManager; } }

// Constants for physics
namespace PhysicsConstants {
    // World scale: Each voxel is 0.25m (25cm) in the world
    const float GRAVITY = 9.81f;           // m/s²
    const float TERMINAL_VELOCITY = 50.0f;  // Maximum fall speed in m/s
    const float PLAYER_HEIGHT = 7.2f;       // Player stands 1.8 meters tall (7.2 voxel units)
    const float EYE_HEIGHT = 6.6f;         // Eyes are at 1.65 meters (6.6 voxel units) from feet
    const float PLAYER_RADIUS = 1.2f;       // Player radius is 0.3 meters (1.2 voxel units)
    const float WALK_SPEED = 4.5f;          // Normal walking speed in voxel units/s
    const float SPRINT_SPEED = 9.0f;        // Sprinting speed in voxel units/s
    const float CROUCH_SPEED = 2.5f;        // Crouching speed in voxel units/s
    const float CROUCH_HEIGHT = 3.6f;       // Height when crouching is 0.9 meters (3.6 voxel units)
    const float JUMP_VELOCITY = 5.4f;       // Initial jump velocity for ~1.2 voxel height
    const float STEP_HEIGHT = 1.0f;         // Maximum height for automatic step-up (1 voxel = 25cm)
}

/**
 * PlayerPhysics handles physics calculations for the player in first-person mode
 * This includes gravity, collision detection, and movement physics
 */
class PlayerPhysics {
public:
    /**
     * Updates player physics based on the current state of the game
     * @param game Reference to the game object
     * @param deltaTime Time elapsed since last frame
     */
    static void update(Game& game, float deltaTime);

private:
    /**
     * Apply gravity and vertical movement
     * @param game Reference to the game object
     * @param deltaTime Time elapsed since last frame
     */
    static void applyGravity(Game& game, float deltaTime);
    
    /**
     * Apply horizontal movement based on input
     * @param game Reference to the game object
     * @param deltaTime Time elapsed since last frame
     * @param camera Camera for direction vectors
     */
    static void applyMovement(Game& game, float deltaTime, const SpectatorCamera* camera);
    
    /**
     * Handle collision detection with the world
     * @param game Reference to the game object
     * @param worldManager World manager for voxel queries
     */
    static void handleCollision(Game& game, VoxelCastle::World::WorldManager* worldManager);
};

#endif // PLAYER_PHYSICS_H
