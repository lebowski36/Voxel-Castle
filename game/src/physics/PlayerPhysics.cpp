#include "physics/PlayerPhysics.h"
#include "core/game.h"
#include "SpectatorCamera.h" // Include from standard include path
#include "world/world_manager.h"
#include <iostream>
#include <cmath>

void PlayerPhysics::update(Game& game, float deltaTime) {
    // Only apply physics in first-person mode
    if (game.getCameraMode() != CameraMode::FIRST_PERSON) {
        return;
    }

    // Get required components
    auto worldManager = game.getWorldManager();
    auto camera = game.getCamera();
    
    if (!worldManager || !camera) {
        return;
    }

    // Apply gravity
    applyGravity(game, deltaTime);
    
    // Apply horizontal movement
    applyMovement(game, deltaTime, camera);
    
    // Handle collision detection
    handleCollision(game, worldManager);
    
    // Synchronize camera position with player position
    camera->setPosition(game.getPlayerPosition());
}

void PlayerPhysics::applyGravity(Game& game, float deltaTime) {
    glm::vec3& playerVel = game.getPlayerVelocity();
    bool& isOnGround = game.isPlayerOnGround();
    
    // Apply gravity if not on ground
    if (!isOnGround) {
        playerVel.y -= PhysicsConstants::GRAVITY * deltaTime;
        
        // Clamp to terminal velocity
        if (playerVel.y < -PhysicsConstants::TERMINAL_VELOCITY) {
            playerVel.y = -PhysicsConstants::TERMINAL_VELOCITY;
        }
    }
    
    // Handle jumping (only if on ground)
    if (game.isJumping() && isOnGround) {
        playerVel.y = PhysicsConstants::JUMP_VELOCITY;
        isOnGround = false;
    }
}

void PlayerPhysics::applyMovement(Game& game, float deltaTime, const SpectatorCamera* camera) {
    glm::vec3& playerPos = game.getPlayerPosition();
    glm::vec3& playerVel = game.getPlayerVelocity();
    
    // Calculate movement direction based on input
    glm::vec3 moveDir(0.0f);
    
    if (game.isForward()) moveDir.z -= 1.0f;
    if (game.isBackward()) moveDir.z += 1.0f;
    if (game.isLeft()) moveDir.x -= 1.0f;
    if (game.isRight()) moveDir.x += 1.0f;
    
    if (glm::length(moveDir) > 0.0f) {
        // Normalize direction vector
        moveDir = glm::normalize(moveDir);
        
        // Convert to world space using camera orientation
        glm::vec3 front = camera->getFront();
        glm::vec3 right = camera->getRight();
        
        // Keep only horizontal components (zero out y)
        front.y = 0.0f;
        right.y = 0.0f;
        
        if (glm::length(front) > 0.0f) front = glm::normalize(front);
        if (glm::length(right) > 0.0f) right = glm::normalize(right);
        
        moveDir = right * moveDir.x + front * moveDir.z;
        
        // Determine movement speed based on state
        float moveSpeed = PhysicsConstants::WALK_SPEED;
        if (game.isSprinting()) moveSpeed = PhysicsConstants::SPRINT_SPEED;
        if (game.isCrouching()) moveSpeed = PhysicsConstants::CROUCH_SPEED;
        
        // Arcade-style instant velocity change
        playerVel.x = moveDir.x * moveSpeed;
        playerVel.z = moveDir.z * moveSpeed;
    } else {
        // No input - stop horizontal movement (arcade style)
        playerVel.x = 0.0f;
        playerVel.z = 0.0f;
    }
    
    // Update position based on velocity
    playerPos += playerVel * deltaTime;
}

void PlayerPhysics::handleCollision(Game& game, VoxelCastle::World::WorldManager* worldManager) {
    glm::vec3& playerPos = game.getPlayerPosition();
    glm::vec3& playerVel = game.getPlayerVelocity();
    bool& isOnGround = game.isPlayerOnGround();
    
    // Collision - prevent falling through the ground
    // Find the voxel below the player
    int floorX = static_cast<int>(std::floor(playerPos.x));
    int floorY = static_cast<int>(std::floor(playerPos.y - PhysicsConstants::GROUND_OFFSET));
    int floorZ = static_cast<int>(std::floor(playerPos.z));
    
    // Check if the block below is solid
    VoxelEngine::World::Voxel blockBelow = worldManager->getVoxel(floorX, floorY, floorZ);
    bool isSolidBelow = blockBelow.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR);
    
    if (isSolidBelow) {
        // Calculate exact ground position
        float groundY = static_cast<float>(floorY + 1) + PhysicsConstants::GROUND_OFFSET;
        
        // If we're falling and hit the ground
        if (playerVel.y < 0 && playerPos.y < groundY) {
            playerPos.y = groundY;
            playerVel.y = 0.0f;
            isOnGround = true;
        }
    } else {
        // No solid block below - we're in the air
        isOnGround = false;
    }
    
    // TODO: Add more sophisticated collision detection for horizontal movement
    // TODO: Implement step-up logic for small height differences
    // TODO: Add fall damage calculation

    // Debug output
    std::cout << "\rPos: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z 
              << ") Vel: (" << playerVel.x << ", " << playerVel.y << ", " << playerVel.z 
              << ") OnGround: " << isOnGround << "   ";
    std::cout.flush();
}
