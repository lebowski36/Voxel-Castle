#include "interaction/BlockPlacement.h"
#include "core/game.h"
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
                    minDist = dist;
                    bestNormal = face.normal;
                    bestAdjacent = voxelPos + face.offset;
                }
            }
            
            result.normal = bestNormal;
            result.adjacentPosition = bestAdjacent;
            break;
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
    INFO_LOG("BlockPlacement", "World readiness check passed");

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
        INFO_LOG("BlockPlacement", "All safety checks passed, starting raycast...");
        
        // Clear any existing OpenGL errors
        GLenum prevErr;
        while ((prevErr = glGetError()) != GL_NO_ERROR) {
            WARN_LOG("BlockPlacement", "Clearing previous OpenGL error: 0x" + 
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
        
        INFO_LOG("BlockPlacement", "Hit block at (" + std::to_string(rayResult.blockPosition.x) + 
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
            ERROR_LOG("BlockPlacement", "Chunk at target position is not safe for modification! Aborting.");
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
                
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Placed " << static_cast<int>(blockType) << " block at (" 
                          << rayResult.adjacentPosition.x << ", " << rayResult.adjacentPosition.y << ", " << rayResult.adjacentPosition.z << ")" << std::endl;
                          
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Marking chunk dirty..." << std::endl;
                // Clear OpenGL errors before marking dirty
                while (glGetError() != GL_NO_ERROR) {}
                markChunkDirtyForPosition(worldManager, rayResult.adjacentPosition);
                // Check for OpenGL errors immediately after marking dirty
                GLenum errAfterMarkDirty = glGetError();
                if (errAfterMarkDirty != GL_NO_ERROR) {
                    std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] OpenGL error IMMEDIATELY AFTER markChunkDirtyForPosition (placement): 0x" 
                              << std::hex << errAfterMarkDirty << std::dec << std::endl;
                }
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Chunk marked dirty successfully" << std::endl;
            } else {
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Cannot place block at that location" << std::endl;
            }
        } else { // Right click (remove block)
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Removing block..." << std::endl;
            
            // Ensure the block being removed is not AIR
            VoxelEngine::World::Voxel voxelToRemove = worldManager->getVoxel(rayResult.blockPosition.x, rayResult.blockPosition.y, rayResult.blockPosition.z);
            if (voxelToRemove.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Cannot remove AIR block." << std::endl;
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
            
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Removed block at (" 
                      << rayResult.blockPosition.x << ", " << rayResult.blockPosition.y << ", " << rayResult.blockPosition.z << ")" << std::endl;

            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Marking chunk dirty for removal..." << std::endl;
            // Clear OpenGL errors before marking dirty
            while (glGetError() != GL_NO_ERROR) {}
            markChunkDirtyForPosition(worldManager, rayResult.blockPosition); // Mark the chunk of the removed block
            // Check for OpenGL errors immediately after marking dirty
            GLenum errAfterMarkDirty = glGetError();
            if (errAfterMarkDirty != GL_NO_ERROR) {
                std::cerr << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] OpenGL error IMMEDIATELY AFTER markChunkDirtyForPosition (removal): 0x" 
                          << std::hex << errAfterMarkDirty << std::dec << std::endl;
            }
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Chunk marked dirty successfully for removal" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] EXCEPTION caught: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] UNKNOWN EXCEPTION caught!" << std::endl;
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
    
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Selected block type: " << static_cast<int>(availableBlocks[currentIndex]) << std::endl;
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
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Chunk column doesn't exist at (" << chunkX << ", " << chunkZ << "), considering unsafe" << std::endl;
            return false;
        }
        
        // Calculate segment Y index
        int_fast32_t segmentY = static_cast<int_fast32_t>(position.y / VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
        
        // Check if segment exists and is not currently being rebuilt
        const auto* segment = chunkColumn->getSegmentByIndex(segmentY);
        if (!segment) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Segment doesn't exist at Y index " << segmentY << ", considering unsafe" << std::endl;
            return false;
        }
        
        // Check if mesh is currently being rebuilt (thread safety)
        if (segment->mIsRebuildingMesh) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Segment is currently rebuilding mesh, considering unsafe" << std::endl;
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
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] WorldManager is null in isValidPlacement" << std::endl;
        return false;
    }
    
    // Check if position is within world bounds (basic check)
    if (position.y < -100 || position.y > 400) {  // Extended range but still reasonable
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Position Y out of bounds: " << position.y << std::endl;
        return false;
    }
    
    try {
        // Check if the position is already occupied
        VoxelEngine::World::Voxel existingVoxel = worldManager->getVoxel(position.x, position.y, position.z);
        if (existingVoxel.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Position already occupied by voxel type " << static_cast<int>(existingVoxel.id) << std::endl;
            return false;
        }
        
        // Check if placement would interfere with player position - improved safety
        glm::vec3 playerPos = const_cast<Game&>(game).getPlayerPosition();
        glm::vec3 blockPos = glm::vec3(position);
        
        // Validate player position
        if (std::isnan(playerPos.x) || std::isnan(playerPos.y) || std::isnan(playerPos.z) ||
            std::isinf(playerPos.x) || std::isinf(playerPos.y) || std::isinf(playerPos.z)) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Player position is invalid (NaN/Inf)" << std::endl;
            return false;
        }
        
        // Enhanced collision check - prevent placing blocks too close to camera/player
        float distance = glm::distance(playerPos, blockPos);
        float minSafeDistance = 2.5f; // Increased minimum distance for safety
        
        if (distance < minSafeDistance) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Block too close to player (distance: " 
                      << distance << " < " << minSafeDistance << ")" << std::endl;
            return false;
        }
        
        // Additional safety: prevent placing blocks that would intersect with camera frustum near plane
        // This helps prevent crashes when placing very close to camera
        if (distance < 3.0f) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Warning: Block close to camera (distance: " 
                      << distance << "), but within acceptable range" << std::endl;
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
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] ERROR: WorldManager is null, cannot mark chunk dirty" << std::endl;
        return;
    }
    
    // Calculate which chunk column this position belongs to
    int_fast64_t chunkX = VoxelCastle::World::WorldManager::worldToColumnBaseX(position.x);
    int_fast64_t chunkZ = VoxelCastle::World::WorldManager::worldToColumnBaseZ(position.z);
    
    // Get the chunk column
    auto* chunkColumn = worldManager->getChunkColumn(chunkX, chunkZ);
    if (!chunkColumn) {
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Warning: No chunk column found at (" << chunkX << ", " << chunkZ << ")" << std::endl;
        return;
    }
    
    // Calculate which segment within the column
    int_fast32_t segmentY = static_cast<int_fast32_t>(position.y / VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
    
    // Get the segment and mark it dirty
    auto* segment = chunkColumn->getSegmentByIndex(segmentY);
    if (segment) {
        segment->markDirty(true);
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Marked chunk segment (" << chunkX << ", " << segmentY << ", " << chunkZ << ") dirty for mesh update" << std::endl;
    } else {
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "][BlockPlacement] Warning: No chunk segment found at Y index " << segmentY << std::endl;
    }
}
