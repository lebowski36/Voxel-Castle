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
    // In first-person mode, we only update the camera position but keep the camera orientation
    // This allows simultaneous movement and looking around
    camera->setPosition(game.getPlayerPosition());
    
    // Debug output to verify camera integration
    std::cout << "\rCamera Front: (" << camera->getFront().x << ", " << camera->getFront().y << ", " << camera->getFront().z << ")   ";
    std::cout.flush();
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
    
    // Fix inverted controls: W = forward (negative Z), S = backward (positive Z)
    if (game.isForward()) moveDir.z -= 1.0f;  // W key: forward is negative Z
    if (game.isBackward()) moveDir.z += 1.0f; // S key: backward is positive Z
    if (game.isLeft()) moveDir.x -= 1.0f;     // A key: left is negative X
    if (game.isRight()) moveDir.x += 1.0f;    // D key: right is positive X
    
    if (glm::length(moveDir) > 0.0f) {
        // Normalize direction vector
        moveDir = glm::normalize(moveDir);
        
        // Convert to world space using camera orientation
        // Use a horizontal front vector for consistent control direction with free camera
        glm::vec3 front = camera->getFront();
        glm::vec3 right = camera->getRight();
        
        // Keep only horizontal components (zero out y)
        front.y = 0.0f;
        right.y = 0.0f;
        
        if (glm::length(front) > 0.0f) front = glm::normalize(front);
        if (glm::length(right) > 0.0f) right = glm::normalize(right);
        
        // Match the SpectatorCamera control scheme:
        // forward = +front, backward = -front, left = -right, right = +right
        moveDir = right * moveDir.x + front * -moveDir.z; // Note the negative on moveDir.z to fix inverted W/S
        
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
    
    // Store old position for collision resolution
    glm::vec3 oldPosition = playerPos;
    
    // 1. Ground collision - prevent falling through the ground
    int floorX = static_cast<int>(std::floor(playerPos.x));
    int floorY = static_cast<int>(std::floor(playerPos.y - PhysicsConstants::EYE_HEIGHT));
    int floorZ = static_cast<int>(std::floor(playerPos.z));
    
    // Check if the block below is solid
    VoxelEngine::World::Voxel blockBelow = worldManager->getVoxel(floorX, floorY, floorZ);
    bool isSolidBelow = blockBelow.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR);
    
    if (isSolidBelow) {
        // Calculate exact ground position (feet at floor level, eyes at eye height above floor)
        float groundY = static_cast<float>(floorY + 1) + PhysicsConstants::EYE_HEIGHT;
        
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
    
    // 2. Horizontal collision detection with step-up logic
    if (glm::length(glm::vec2(playerVel.x, playerVel.z)) > 0.0f) {
        // Calculate direction of movement
        glm::vec3 moveDir = glm::normalize(glm::vec3(playerVel.x, 0.0f, playerVel.z));
        float checkDistance = PhysicsConstants::PLAYER_RADIUS;
        
        // Point at feet level - for checking if we can step up
        int frontX = static_cast<int>(std::floor(playerPos.x + moveDir.x * checkDistance));
        int frontY = static_cast<int>(std::floor(playerPos.y - PhysicsConstants::EYE_HEIGHT)); // Feet level
        int frontZ = static_cast<int>(std::floor(playerPos.z + moveDir.z * checkDistance));
        
        // Check if there's a block in front of us at feet level
        VoxelEngine::World::Voxel frontBlockFeet = worldManager->getVoxel(frontX, frontY, frontZ);
        bool isSolidFrontFeet = frontBlockFeet.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR);
        
        // Check if there's a block in front of us at knee level (for stepping up)
        VoxelEngine::World::Voxel frontBlockKnee = worldManager->getVoxel(frontX, frontY + 1, frontZ);
        bool isSolidFrontKnee = frontBlockKnee.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR);
        
        // Check if we have head clearance (two blocks up from feet)
        VoxelEngine::World::Voxel frontBlockHead = worldManager->getVoxel(
            frontX, frontY + static_cast<int>(PhysicsConstants::PLAYER_HEIGHT - 1), frontZ);
        bool hasHeadClearance = frontBlockHead.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR);
        
        // Check if there's space after stepping up (at new feet position)
        // Removed unused variable blockAboveStep
        
        // If we have a block at feet level but not at knee level, we can step up
        if (isSolidFrontFeet && !isSolidFrontKnee && hasHeadClearance && isOnGround) {
            // Apply step-up - move player up by 1.0 voxel height
            playerPos.y += 1.0f;
            std::cout << "Step up!" << std::endl;
        } 
        // If there's a solid block in front that we can't step onto
        else if ((isSolidFrontFeet || isSolidFrontKnee) && isOnGround) {
            // Can't step up, block horizontal movement
            playerPos.x = oldPosition.x;
            playerPos.z = oldPosition.z;
            playerVel.x = 0.0f;
            playerVel.z = 0.0f;
        }
    }
    
    // TODO: Add fall damage calculation
    
    // Debug output
    std::cout << "\rPos: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z 
              << ") Vel: (" << playerVel.x << ", " << playerVel.y << ", " << playerVel.z 
              << ") OnGround: " << isOnGround 
              << " EyeHeight: " << PhysicsConstants::EYE_HEIGHT << " voxels"
              << "   ";
    std::cout.flush();
}
