#include "interaction/BlockPlacement.h"
#include "core/game.h"
#include "core/CameraMode.h"
#include "SpectatorCamera.h"
#include "utils/logging_utils.h"
#include "utils/debug_logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <glad/glad.h>

RaycastResult BlockPlacement::raycast(const SpectatorCamera* camera, 
                                     VoxelCastle::World::WorldManager* worldManager, 
                                     float maxDistance) {
    RaycastResult result;
    
    if (!camera || !worldManager) {
        return result;
    }

    // Get ray from camera center (screen center)
    glm::vec3 rayOrigin = camera->getPosition();
    glm::vec3 rayDirection = camera->getFront(); // Camera front vector points forward

    // Step along the ray in small increments to find intersections
    const float stepSize = 0.1f; // Small step size for accuracy
    float currentDistance = 0.0f;
    
    while (currentDistance < maxDistance) {
        glm::vec3 currentPos = rayOrigin + rayDirection * currentDistance;
        
        // Convert world position to voxel coordinates
        glm::ivec3 voxelPos = glm::ivec3(
            static_cast<int>(std::floor(currentPos.x)),
            static_cast<int>(std::floor(currentPos.y)),
            static_cast<int>(std::floor(currentPos.z))
        );
        
        // Check if this voxel exists and is not air
        VoxelEngine::World::Voxel voxel = worldManager->getVoxel(voxelPos.x, voxelPos.y, voxelPos.z);
        if (voxel.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
            // We hit a solid block!
            result.hit = true;
            result.blockPosition = voxelPos;
            result.hitPoint = currentPos;
            result.distance = currentDistance;
            
            // Calculate which face we hit and the adjacent position for placement
            glm::vec3 localHitPos = currentPos - glm::vec3(voxelPos);
            
            // Determine which face of the voxel we hit based on the local hit position
            float minDist = std::numeric_limits<float>::max();
            glm::vec3 bestNormal;
            glm::ivec3 bestAdjacent;
            
            // Check each face and find the closest one
            struct Face {
                glm::vec3 normal;
                glm::ivec3 offset;
                float planePos;
                bool checkAxis;
                int axis; // 0=x, 1=y, 2=z
            };
            
            Face faces[] = {
                { glm::vec3(-1, 0, 0), glm::ivec3(-1, 0, 0), 0.0f, true, 0 },  // Left face
                { glm::vec3(1, 0, 0),  glm::ivec3(1, 0, 0),  1.0f, true, 0 },  // Right face
                { glm::vec3(0, -1, 0), glm::ivec3(0, -1, 0), 0.0f, true, 1 },  // Bottom face
                { glm::vec3(0, 1, 0),  glm::ivec3(0, 1, 0),  1.0f, true, 1 },  // Top face
                { glm::vec3(0, 0, -1), glm::ivec3(0, 0, -1), 0.0f, true, 2 },  // Back face
                { glm::vec3(0, 0, 1),  glm::ivec3(0, 0, 1),  1.0f, true, 2 }   // Front face
            };
            
            for (const auto& face : faces) {
                float dist = std::abs(localHitPos[face.axis] - face.planePos);
                if (dist < minDist) {
                    // Check if this face is actually visible (not hidden by adjacent solid blocks)
                    glm::ivec3 adjacentVoxelPos = voxelPos + face.offset;
                    VoxelEngine::World::Voxel adjacentVoxel = worldManager->getVoxel(
                        adjacentVoxelPos.x, adjacentVoxelPos.y, adjacentVoxelPos.z);
                    
                    // Only allow selection of faces that are exposed (adjacent voxel is air)
                    bool isFaceVisible = (adjacentVoxel.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR));
                    
                    if (isFaceVisible) {
                        minDist = dist;
                        bestNormal = face.normal;
                        bestAdjacent = voxelPos + face.offset;
                    }
                }
            }
            
            // Only proceed if we found a visible face
            if (minDist < std::numeric_limits<float>::max()) {
                result.normal = bestNormal;
                result.adjacentPosition = bestAdjacent;
                break;
            } else {
                // No visible faces found, continue ray casting
                // This happens when all faces are hidden by adjacent solid blocks
            }
        }
        
        currentDistance += stepSize;
    }
    
    return result;
}

void BlockPlacement::handleMouseClick(Game& game, bool isLeftClick) {
    // Removed verbose logging for raycasting and block placement details.

    // Add game startup timing protection
    static auto gameStartTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - gameStartTime).count();
    
    if (elapsedSeconds < 3) {
        WARN_LOG("BlockPlacement", "SAFETY: Game too young (" + std::to_string(elapsedSeconds) + 
                "s), ignoring click to prevent initialization crashes");
        return;
    }
    
    // Early exit if critical components are not ready
    if (!game.getWindow()) {
        ERROR_LOG("BlockPlacement", "Window is null! Aborting click.");
        return;
    }
    DEBUG_LOG("BlockPlacement", "Window status: Valid");

    auto camera = game.getCamera();
    if (!camera) {
        ERROR_LOG("BlockPlacement", "Camera is null! Aborting click.");
        return;
    }
    glm::vec3 pos = camera->getPosition();
    DEBUG_LOG("BlockPlacement", "Camera position: (" + std::to_string(pos.x) + ", " + 
             std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")");

    auto worldManager = game.getWorldManager();
    if (!worldManager) {
        ERROR_LOG("BlockPlacement", "WorldManager is null! Aborting click.");
        return;
    }
    DEBUG_LOG("BlockPlacement", "WorldManager obtained successfully");

    // Check if world is ready for block operations (prevents crashes during early loading)
    if (!game.isWorldReadyForBlockOperations()) {
        WARN_LOG("BlockPlacement", "World not ready for block operations. Please wait for chunks to finish loading.");
        return;
    }
    DEBUG_LOG("BlockPlacement", "World readiness check passed");

    // Add critical safety checks for camera position validity
    if (std::isnan(pos.x) || std::isnan(pos.y) || std::isnan(pos.z) || 
        std::isinf(pos.x) || std::isinf(pos.y) || std::isinf(pos.z)) {
        ERROR_LOG("BlockPlacement", "Camera position is invalid (NaN/Inf)! Aborting click.");
        return;
    }
    
    // Check if camera front vector is valid
    glm::vec3 front = camera->getFront();
    if (std::isnan(front.x) || std::isnan(front.y) || std::isnan(front.z) ||
        std::isinf(front.x) || std::isinf(front.y) || std::isinf(front.z)) {
        ERROR_LOG("BlockPlacement", "Camera front vector is invalid! Aborting click.");
        return;
    }
    
    try {
        DEBUG_LOG("BlockPlacement", "All safety checks passed, starting raycast...");
        
        // Clear any existing OpenGL errors
        GLenum prevErr;
        while ((prevErr = glGetError()) != GL_NO_ERROR) {
            DEBUG_LOG("BlockPlacement", "Clearing previous OpenGL error: 0x" + 
                    std::to_string(prevErr));
        }
        
        // Perform raycast to find target
        RaycastResult rayResult = raycast(camera, worldManager, 50.0f);
        
        // Check for OpenGL errors after raycast
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            ERROR_LOG("BlockPlacement", "OpenGL error after raycast: 0x" + std::to_string(err));
        }
        
        DEBUG_LOG("BlockPlacement", "Raycast completed. Hit: " + std::to_string(rayResult.hit));
        
        if (!rayResult.hit) {
            DEBUG_LOG("BlockPlacement", "No block in range");
            return;
        }
        
        DEBUG_LOG("BlockPlacement", "Hit block at (" + std::to_string(rayResult.blockPosition.x) + 
                ", " + std::to_string(rayResult.blockPosition.y) + ", " + std::to_string(rayResult.blockPosition.z) + ")");
        DEBUG_LOG("BlockPlacement", "Adjacent position: (" + std::to_string(rayResult.adjacentPosition.x) + 
                 ", " + std::to_string(rayResult.adjacentPosition.y) + ", " + std::to_string(rayResult.adjacentPosition.z) + ")");
        
        // Validate target position coordinates
        if (std::abs(rayResult.blockPosition.x) > 1000000 || std::abs(rayResult.blockPosition.y) > 1000 || std::abs(rayResult.blockPosition.z) > 1000000) {
            ERROR_LOG("BlockPlacement", "Target position out of valid range! Aborting.");
            return;
        }
        
        if (isLeftClick && (std::abs(rayResult.adjacentPosition.x) > 1000000 || std::abs(rayResult.adjacentPosition.y) > 1000 || std::abs(rayResult.adjacentPosition.z) > 1000000)) {
            ERROR_LOG("BlockPlacement", "Adjacent position out of valid range! Aborting.");
            return;
        }
        
        // Additional safety check: ensure target chunk exists or can be created safely
        glm::ivec3 targetPos = isLeftClick ? rayResult.adjacentPosition : rayResult.blockPosition;
        if (!isChunkPositionSafe(worldManager, targetPos)) {
            DEBUG_LOG("BlockPlacement", "[ERROR] Chunk at target position is not safe for modification! Aborting.");
            return;
        }
        
        // Redirected verbose block placement logs to file-based logging
        VoxelCastle::Utils::logToFile("[" + VoxelEngine::Utils::getTimestamp() + "][BlockPlacement] Processing " + (isLeftClick ? "placement" : "removal"));
    
        if (isLeftClick) {
            VoxelCastle::Utils::logToFile("[" + VoxelEngine::Utils::getTimestamp() + "][BlockPlacement] Checking placement validity...");
            // Place block at adjacent position
            if (isValidPlacement(game, rayResult.adjacentPosition, worldManager)) {
                VoxelCastle::Utils::logToFile("[" + VoxelEngine::Utils::getTimestamp() + "][BlockPlacement] Placement valid, getting block type...");
                VoxelEngine::World::VoxelType blockType = getCurrentBlockType(game);
                VoxelCastle::Utils::logToFile("[" + VoxelEngine::Utils::getTimestamp() + "][BlockPlacement] Block type: " + std::to_string(static_cast<int>(blockType)));
                
                // Clear any OpenGL errors before setVoxel
                while (glGetError() != GL_NO_ERROR) {}
                
                worldManager->setVoxel(static_cast<int_fast64_t>(rayResult.adjacentPosition.x), 
                                     static_cast<int_fast64_t>(rayResult.adjacentPosition.y), 
                                     static_cast<int_fast64_t>(rayResult.adjacentPosition.z), 
                                     blockType);
                                     
                // Check for OpenGL errors immediately after setVoxel
                GLenum errAfterSetVoxel = glGetError();
                if (errAfterSetVoxel != GL_NO_ERROR) {
                    std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] OpenGL error IMMEDIATELY AFTER setVoxel (placement): 0x" 
                              << std::hex << errAfterSetVoxel << std::dec << std::endl;
                }
                
                DEBUG_LOG("BlockOperations", "Block placed at (" + std::to_string(rayResult.adjacentPosition.x) + 
                          ", " + std::to_string(rayResult.adjacentPosition.y) + ", " + std::to_string(rayResult.adjacentPosition.z) + ")");
                          
                DEBUG_LOG("BlockPlacement", "Marking chunk dirty...");
                // Clear OpenGL errors before marking dirty
                while (glGetError() != GL_NO_ERROR) {}
                markChunkDirtyForPosition(worldManager, rayResult.adjacentPosition);
                // Check for OpenGL errors immediately after marking dirty
                GLenum errAfterMarkDirty = glGetError();
                if (errAfterMarkDirty != GL_NO_ERROR) {
                    std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] OpenGL error IMMEDIATELY AFTER markChunkDirtyForPosition (placement): 0x" 
                              << std::hex << errAfterMarkDirty << std::dec << std::endl;
                }
                DEBUG_LOG("BlockPlacement", "Chunk marked dirty successfully");
            } else {
                DEBUG_LOG("BlockPlacement", "Cannot place block at that location");
            }
        } else { // Right click (remove block)
            DEBUG_LOG("BlockPlacement", "Removing block...");
            
            // Ensure the block being removed is not AIR
            VoxelEngine::World::Voxel voxelToRemove = worldManager->getVoxel(rayResult.blockPosition.x, rayResult.blockPosition.y, rayResult.blockPosition.z);
            if (voxelToRemove.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                DEBUG_LOG("BlockPlacement", "Cannot remove AIR block.");
                return; // Exit if trying to remove an AIR block
            }

            // Clear any OpenGL errors before setVoxel
            while (glGetError() != GL_NO_ERROR) {}

            worldManager->setVoxel(static_cast<int_fast64_t>(rayResult.blockPosition.x), 
                                 static_cast<int_fast64_t>(rayResult.blockPosition.y), 
                                 static_cast<int_fast64_t>(rayResult.blockPosition.z), 
                                 VoxelEngine::World::VoxelType::AIR); // Set to AIR

            GLenum errAfterSetVoxel = glGetError();
            if (errAfterSetVoxel != GL_NO_ERROR) {
                std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] OpenGL error IMMEDIATELY AFTER setVoxel (removal): 0x" 
                          << std::hex << errAfterSetVoxel << std::dec << std::endl;
            }
            
            DEBUG_LOG("BlockOperations", "Block removed at (" + std::to_string(rayResult.blockPosition.x) + 
                      ", " + std::to_string(rayResult.blockPosition.y) + ", " + std::to_string(rayResult.blockPosition.z) + ")");

            DEBUG_LOG("BlockPlacement", "Marking chunk dirty for removal...");
            // Clear OpenGL errors before marking dirty
            while (glGetError() != GL_NO_ERROR) {}
            markChunkDirtyForPosition(worldManager, rayResult.blockPosition); // Mark the chunk of the removed block
            // Check for OpenGL errors immediately after marking dirty
            GLenum errAfterMarkDirty = glGetError();
            if (errAfterMarkDirty != GL_NO_ERROR) {
                std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] OpenGL error IMMEDIATELY AFTER markChunkDirtyForPosition (removal): 0x" 
                          << std::hex << errAfterMarkDirty << std::dec << std::endl;
            }
            DEBUG_LOG("BlockPlacement", "Chunk marked dirty successfully for removal");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Block operation failed: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[ERROR] Unknown exception during block operation!" << std::endl;
    }
}

VoxelEngine::World::VoxelType BlockPlacement::getCurrentBlockType(const Game& game) {
    return game.getCurrentBlockType();
}

void BlockPlacement::cycleBlockType(Game& game, bool forward) {
    // List of available block types (excluding AIR)
    static const std::vector<VoxelEngine::World::VoxelType> availableBlocks = {
        VoxelEngine::World::VoxelType::STONE,
        VoxelEngine::World::VoxelType::DIRT,
        VoxelEngine::World::VoxelType::GRASS,
        VoxelEngine::World::VoxelType::SAND,
        VoxelEngine::World::VoxelType::WATER,
        VoxelEngine::World::VoxelType::WOOD,
        VoxelEngine::World::VoxelType::LEAVES
    };
    
    // Find current block type index
    VoxelEngine::World::VoxelType currentType = game.getCurrentBlockType();
    auto it = std::find(availableBlocks.begin(), availableBlocks.end(), currentType);
    
    int currentIndex = 0;
    if (it != availableBlocks.end()) {
        currentIndex = static_cast<int>(std::distance(availableBlocks.begin(), it));
    }
    
    // Cycle to next/previous block type
    if (forward) {
        currentIndex = (currentIndex + 1) % availableBlocks.size();
    } else {
        currentIndex = (currentIndex - 1 + availableBlocks.size()) % availableBlocks.size();
    }
    
    // Update the game state using the setter method
    game.setCurrentBlockType(availableBlocks[currentIndex]);
    
    DEBUG_LOG("BlockPlacement", "Selected block type: " + std::to_string(static_cast<int>(availableBlocks[currentIndex])));
}

bool BlockPlacement::isChunkPositionSafe(VoxelCastle::World::WorldManager* worldManager, const glm::ivec3& position) {
    if (!worldManager) {
        return false;
    }
    
    try {
        // Calculate chunk coordinates
        int_fast64_t chunkX = VoxelCastle::World::WorldManager::worldToColumnBaseX(position.x);
        int_fast64_t chunkZ = VoxelCastle::World::WorldManager::worldToColumnBaseZ(position.z);
        
        // Check if chunk column already exists (safer than creating new ones during block operations)
        const auto* chunkColumn = worldManager->getChunkColumn(chunkX, chunkZ);
        if (!chunkColumn) {
            DEBUG_LOG("BlockPlacement", "Chunk column doesn't exist at (" + std::to_string(chunkX) + ", " + std::to_string(chunkZ) + "), considering unsafe");
            return false;
        }
        
        // Calculate segment Y index
        int_fast32_t segmentY = static_cast<int_fast32_t>(position.y / VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
        
        // Check if segment exists and is not currently being rebuilt
        const auto* segment = chunkColumn->getSegmentByIndex(segmentY);
        if (!segment) {
            DEBUG_LOG("BlockPlacement", "Segment doesn't exist at Y index " + std::to_string(segmentY) + ", considering unsafe");
            return false;
        }
        
        // Check if mesh is currently being rebuilt (thread safety)
        if (segment->mIsRebuildingMesh) {
            DEBUG_LOG("BlockPlacement", "[ERROR] Chunk at target position is not safe for modification! Aborting.");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Exception in isChunkPositionSafe: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Unknown exception in isChunkPositionSafe" << std::endl;
        return false;
    }
}

bool BlockPlacement::isValidPlacement(const Game& game, 
                                    const glm::ivec3& position,
                                    VoxelCastle::World::WorldManager* worldManager) {
    if (!worldManager) {
        DEBUG_LOG("BlockPlacement", "WorldManager is null in isValidPlacement");
        return false;
    }
    
    // Check if position is within world bounds (basic check)
    if (position.y < -100 || position.y > 400) {  // Extended range but still reasonable
        DEBUG_LOG("BlockPlacement", "Position Y out of bounds: " + std::to_string(position.y));
        return false;
    }
    
    DEBUG_LOG("BlockPlacement", std::string("isValidPlacement called for position: (") + 
             std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
    
    try {
        // Check if the position is already occupied
        VoxelEngine::World::Voxel existingVoxel = worldManager->getVoxel(position.x, position.y, position.z);
        if (existingVoxel.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
            DEBUG_LOG("BlockPlacement", "Position already occupied by voxel type " + std::to_string(static_cast<int>(existingVoxel.id)));
            DEBUG_LOG("BlockPlacement", "Cannot place block at that location");
            return false;
        }
        
        // Check camera mode - only apply collision detection for first-person mode
        // Spectator cameras (FREE_FLYING mode) should have no collision restrictions
        CameraMode cameraMode = game.getCameraMode();
        DEBUG_LOG("BlockPlacement", std::string("Camera mode: ") + (cameraMode == CameraMode::FIRST_PERSON ? "FIRST_PERSON" : "FREE_FLYING"));
        
        if (cameraMode == CameraMode::FIRST_PERSON) {
            // Check if placement would interfere with player position - proper bounding box collision
            glm::vec3 cameraPos = const_cast<Game&>(game).getPlayerPosition();
            glm::vec3 blockPos = glm::vec3(position);
            
            // Validate camera position
            if (std::isnan(cameraPos.x) || std::isnan(cameraPos.y) || std::isnan(cameraPos.z) ||
                std::isinf(cameraPos.x) || std::isinf(cameraPos.y) || std::isinf(cameraPos.z)) {
                DEBUG_LOG("BlockPlacement", "Camera position is invalid (NaN/Inf)");
                return false;
            }
            
            // Calculate player's actual bounding box considering camera offset
            // Camera is at eye level (6.6 voxel units above feet)
            const float EYE_HEIGHT = 6.6f;        // Eyes are 6.6 voxel units above feet
            const float PLAYER_HEIGHT = 7.2f;     // Total player height (1.8m = 7.2 voxel units)
            const float PLAYER_RADIUS = 0.8f;     // Player radius (0.2m = 0.8 voxel units) - compromise between physics and placement
            
            // Calculate player's feet position from camera position
            glm::vec3 playerFeet = cameraPos;
            playerFeet.y -= EYE_HEIGHT;  // Camera is at eye level, so feet are EYE_HEIGHT below
            
            // Calculate player's head position
            glm::vec3 playerHead = playerFeet;
            playerHead.y += PLAYER_HEIGHT;
            
            DEBUG_LOG("BlockPlacement", std::string("Collision check - Camera pos: (") + 
                     std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.y) + ", " + std::to_string(cameraPos.z) + ")");
            DEBUG_LOG("BlockPlacement", std::string("Player feet pos: (") + 
                     std::to_string(playerFeet.x) + ", " + std::to_string(playerFeet.y) + ", " + std::to_string(playerFeet.z) + ")");
            DEBUG_LOG("BlockPlacement", std::string("Block pos: (") + 
                     std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
            
            // Player bounding box (capsule approximated as cylinder)
            // Bottom: playerFeet.y
            // Top: playerHead.y  
            // Center horizontally: playerFeet.x, playerFeet.z
            // Radius: PLAYER_RADIUS
            
            // Check if the block would intersect with the player's cylindrical bounding box
            // Block occupies space from position to position + vec3(1,1,1)
            glm::vec3 blockMin = blockPos;
            glm::vec3 blockMax = blockPos + glm::vec3(1.0f, 1.0f, 1.0f);
            
            // SPECIAL RULES for vertical placement:
            // 1. All blocks BELOW player feet should be placeable (no horizontal restriction)
            // 2. All blocks ABOVE 2m total height should be placeable (no horizontal restriction)
            // 3. Only blocks WITHIN the 2m zone should check horizontal collision
            
            const float TOTAL_CLEARANCE_HEIGHT = 8.0f; // 8 voxels = 2m total height from feet
            float clearanceHeight = playerFeet.y + TOTAL_CLEARANCE_HEIGHT;
            
            // Check if block is completely below player feet + small buffer - ALWAYS ALLOW
            // Add 0.25f buffer (quarter block) so blocks at feet level can be placed
            const float FEET_BUFFER = 0.25f; // Quarter block buffer below feet
            if (blockMax.y <= playerFeet.y + FEET_BUFFER) {
                DEBUG_LOG("BlockPlacement", std::string("Block below player feet (with buffer) - placement allowed. ") +
                         "Block Y max: " + std::to_string(blockMax.y) + " <= Player feet Y + buffer: " + std::to_string(playerFeet.y + FEET_BUFFER));
                return true; // Skip all collision checks for blocks below player + buffer
            }
            // Check if block is completely above 2m total height from feet - ALWAYS ALLOW  
            else if (blockMin.y >= clearanceHeight) {
                DEBUG_LOG("BlockPlacement", "Block above 2m zone from feet - placement allowed");
                return true; // Skip all collision checks for blocks above 2m from feet
            }
            // Only check collision for blocks within the player's body + clearance zone
            else {
                // Block is in the zone where we need to check horizontal collision
                bool verticalOverlap = (blockMax.y > playerFeet.y && blockMin.y < playerHead.y);
                
                if (verticalOverlap) {
                    // Only if there's actual body overlap, check horizontal distance
                    glm::vec2 playerCenter2D = glm::vec2(playerFeet.x, playerFeet.z);
                    
                    // Find closest point on block's horizontal bounds to player center
                    glm::vec2 closestPoint2D;
                    closestPoint2D.x = glm::clamp(playerCenter2D.x, blockMin.x, blockMax.x);
                    closestPoint2D.y = glm::clamp(playerCenter2D.y, blockMin.z, blockMax.z);
                    
                    // Calculate horizontal distance from player center to closest point on block
                    float horizontalDistance = glm::distance(playerCenter2D, closestPoint2D);
                    
                    // If horizontal distance is less than player radius AND there's vertical overlap, there's a collision
                    if (horizontalDistance < PLAYER_RADIUS) {
                        DEBUG_LOG("BlockPlacement", std::string("FIRST_PERSON mode: Block would intersect with player bounding box - ") +
                                 "horizontal distance: " + std::to_string(horizontalDistance) + 
                                 " < player radius: " + std::to_string(PLAYER_RADIUS) +
                                 ", block Y range: [" + std::to_string(blockMin.y) + ", " + std::to_string(blockMax.y) + "]" +
                                 ", player Y range: [" + std::to_string(playerFeet.y) + ", " + std::to_string(playerHead.y) + "]");
                        DEBUG_LOG("BlockPlacement", std::string("Player feet at (") + std::to_string(playerFeet.x) + 
                                 ", " + std::to_string(playerFeet.y) + ", " + std::to_string(playerFeet.z) + ")");
                        DEBUG_LOG("BlockPlacement", std::string("Block at (") + std::to_string(position.x) + 
                                 ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
                        return false;
                    }
                }
            }
            
            // The new collision system above handles all cases precisely:
            // - Blocks below feet: Always allowed
            // - Blocks above 2m from feet: Always allowed  
            // - Blocks within 2m zone from feet: Check horizontal collision only
        } else if (cameraMode == CameraMode::FREE_FLYING) {
            // Spectator mode - no collision restrictions
            DEBUG_LOG("BlockPlacement", "FREE_FLYING mode: Skipping collision detection for spectator camera");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Exception in isValidPlacement: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Unknown exception in isValidPlacement" << std::endl;
        return false;
    }
}

void BlockPlacement::markChunkDirtyForPosition(VoxelCastle::World::WorldManager* worldManager,
                                              const glm::ivec3& position) {
    if (!worldManager) {
        DEBUG_LOG("BlockPlacement", "ERROR: WorldManager is null, cannot mark chunk dirty");
        return;
    }
    
    // Calculate which chunk column this position belongs to
    int_fast64_t chunkX = VoxelCastle::World::WorldManager::worldToColumnBaseX(position.x);
    int_fast64_t chunkZ = VoxelCastle::World::WorldManager::worldToColumnBaseZ(position.z);
    
    // Get the chunk column
    auto* chunkColumn = worldManager->getChunkColumn(chunkX, chunkZ);
    if (!chunkColumn) {
        DEBUG_LOG("BlockPlacement", "Warning: No chunk column found at (" + std::to_string(chunkX) + ", " + std::to_string(chunkZ) + ")");
        return;
    }
    
    // Calculate which segment within the column
    int_fast32_t segmentY = static_cast<int_fast32_t>(position.y / VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
    
    // Get the segment and mark it dirty
    auto* segment = chunkColumn->getSegmentByIndex(segmentY);
    if (segment) {
        segment->markDirty(true);
        DEBUG_LOG("BlockPlacement", "Marked chunk segment (" + std::to_string(chunkX) + ", " + std::to_string(segmentY) + ", " + std::to_string(chunkZ) + ") dirty for mesh update");
    } else {
        DEBUG_LOG("BlockPlacement", "Warning: No chunk segment found at Y index " + std::to_string(segmentY));
    }
}
