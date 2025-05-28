#include "interaction/BlockPlacement.h"
#include "core/game.h"
#include "SpectatorCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <glad/glad.h> // Add this to check OpenGL errors

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
    std::cout << "\n[BlockPlacement] ========= MOUSE CLICK START =========" << std::endl;
    std::cout << "[BlockPlacement] Click type: " << (isLeftClick ? "LEFT (place)" : "RIGHT (remove)") << std::endl;
    std::cout << "[BlockPlacement] Window status: " << (game.getWindow() ? "Valid" : "NULL") << std::endl;
    
    try {
        std::cout << "[BlockPlacement] About to get camera..." << std::endl;
        auto camera = game.getCamera();
        std::cout << "[BlockPlacement] Camera obtained: " << camera << std::endl;
        if (camera) {
            glm::vec3 pos = camera->getPosition();
            std::cout << "[BlockPlacement] Camera position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
        }
        
        std::cout << "[BlockPlacement] About to get world manager..." << std::endl;
        auto worldManager = game.getWorldManager();
        std::cout << "[BlockPlacement] WorldManager obtained: " << worldManager << std::endl;
        
        if (!camera) {
            std::cout << "[BlockPlacement] ERROR: Camera is null!" << std::endl;
            return;
        }
        
        if (!worldManager) {
            std::cout << "[BlockPlacement] ERROR: WorldManager is null!" << std::endl;
            return;
        }
        
        std::cout << "[BlockPlacement] Both pointers valid, starting raycast..." << std::endl;
        
        // Clear any existing OpenGL errors
        GLenum prevErr;
        while ((prevErr = glGetError()) != GL_NO_ERROR) {
            std::cout << "[BlockPlacement] Clearing previous OpenGL error: 0x" 
                      << std::hex << prevErr << std::dec << std::endl;
        }
        
        // Perform raycast to find target
        RaycastResult rayResult = raycast(camera, worldManager, 50.0f);
        
        // Check for OpenGL errors after raycast
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "[BlockPlacement] OpenGL error after raycast: 0x" 
                      << std::hex << err << std::dec << std::endl;
        }
        
        std::cout << "[BlockPlacement] Raycast completed. Hit: " << rayResult.hit << std::endl;
        
        if (!rayResult.hit) {
            std::cout << "[BlockPlacement] No block in range" << std::endl;
            return;
        }
        
        std::cout << "[BlockPlacement] Hit block at (" << rayResult.blockPosition.x 
                  << ", " << rayResult.blockPosition.y << ", " << rayResult.blockPosition.z << ")" << std::endl;
        std::cout << "[BlockPlacement] Adjacent position: (" << rayResult.adjacentPosition.x 
                  << ", " << rayResult.adjacentPosition.y << ", " << rayResult.adjacentPosition.z << ")" << std::endl;
        
        std::cout << "[BlockPlacement] Processing " << (isLeftClick ? "placement" : "removal") << std::endl;
    
        if (isLeftClick) {
            std::cout << "[BlockPlacement] Checking placement validity..." << std::endl;
            // Place block at adjacent position
            if (isValidPlacement(game, rayResult.adjacentPosition, worldManager)) {
                std::cout << "[BlockPlacement] Placement valid, getting block type..." << std::endl;
                VoxelEngine::World::VoxelType blockType = getCurrentBlockType(game);
                std::cout << "[BlockPlacement] Block type: " << static_cast<int>(blockType) << std::endl;
                
                std::cout << "[BlockPlacement] Setting voxel..." << std::endl;
                
                // Clear any OpenGL errors before setVoxel
                while (glGetError() != GL_NO_ERROR) {}
                
                worldManager->setVoxel(static_cast<int_fast64_t>(rayResult.adjacentPosition.x), 
                                     static_cast<int_fast64_t>(rayResult.adjacentPosition.y), 
                                     static_cast<int_fast64_t>(rayResult.adjacentPosition.z), 
                                     blockType);
                                     
                // Check for OpenGL errors immediately after setVoxel
                GLenum errAfterSetVoxel = glGetError();
                if (errAfterSetVoxel != GL_NO_ERROR) {
                    std::cerr << "[BlockPlacement] OpenGL error IMMEDIATELY AFTER setVoxel (placement): 0x" 
                              << std::hex << errAfterSetVoxel << std::dec << std::endl;
                }
                
                std::cout << "[BlockPlacement] Placed " << static_cast<int>(blockType) << " block at (" 
                          << rayResult.adjacentPosition.x << ", " << rayResult.adjacentPosition.y << ", " << rayResult.adjacentPosition.z << ")" << std::endl;
                          
                std::cout << "[BlockPlacement] Marking chunk dirty..." << std::endl;
                // Clear OpenGL errors before marking dirty
                while (glGetError() != GL_NO_ERROR) {}
                markChunkDirtyForPosition(worldManager, rayResult.adjacentPosition);
                // Check for OpenGL errors immediately after marking dirty
                GLenum errAfterMarkDirty = glGetError();
                if (errAfterMarkDirty != GL_NO_ERROR) {
                    std::cerr << "[BlockPlacement] OpenGL error IMMEDIATELY AFTER markChunkDirtyForPosition (placement): 0x" 
                              << std::hex << errAfterMarkDirty << std::dec << std::endl;
                }
                std::cout << "[BlockPlacement] Chunk marked dirty successfully" << std::endl;
            } else {
                std::cout << "[BlockPlacement] Cannot place block at that location" << std::endl;
            }
        } else {
            std::cout << "[BlockPlacement] Removing block..." << std::endl;
            // Remove block (right click)

            // Clear any OpenGL errors before setVoxel
            while (glGetError() != GL_NO_ERROR) {}

            worldManager->setVoxel(static_cast<int_fast64_t>(rayResult.blockPosition.x), 
                                 static_cast<int_fast64_t>(rayResult.blockPosition.y), 
                                 static_cast<int_fast64_t>(rayResult.blockPosition.z), 
                                 VoxelEngine::World::VoxelType::AIR);

            // Check for OpenGL errors immediately after setVoxel
            GLenum errAfterSetVoxelRemoval = glGetError();
            if (errAfterSetVoxelRemoval != GL_NO_ERROR) {
                std::cerr << "[BlockPlacement] OpenGL error IMMEDIATELY AFTER setVoxel (removal): 0x" 
                          << std::hex << errAfterSetVoxelRemoval << std::dec << std::endl;
            }
            
            std::cout << "[BlockPlacement] Removed block at (" 
                      << rayResult.blockPosition.x << ", " << rayResult.blockPosition.y << ", " << rayResult.blockPosition.z << ")" << std::endl;
                      
            std::cout << "[BlockPlacement] Marking chunk dirty for removal..." << std::endl;
            // Clear OpenGL errors before marking dirty
            while (glGetError() != GL_NO_ERROR) {}
            markChunkDirtyForPosition(worldManager, rayResult.blockPosition);
            // Check for OpenGL errors immediately after marking dirty
            GLenum errAfterMarkDirtyRemoval = glGetError();
            if (errAfterMarkDirtyRemoval != GL_NO_ERROR) {
                std::cerr << "[BlockPlacement] OpenGL error IMMEDIATELY AFTER markChunkDirtyForPosition (removal): 0x" 
                          << std::hex << errAfterMarkDirtyRemoval << std::dec << std::endl;
            }
            std::cout << "[BlockPlacement] Chunk marked dirty successfully" << std::endl;
        }
        
        std::cout << "[BlockPlacement] === MOUSE CLICK END ===" << std::endl;
    
    } catch (const std::exception& e) {
        std::cout << "[BlockPlacement] EXCEPTION caught: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "[BlockPlacement] UNKNOWN EXCEPTION caught!" << std::endl;
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
    
    std::cout << "Selected block type: " << static_cast<int>(availableBlocks[currentIndex]) << std::endl;
}

bool BlockPlacement::isValidPlacement(const Game& game, 
                                    const glm::ivec3& position,
                                    VoxelCastle::World::WorldManager* worldManager) {
    // Check if position is within world bounds (basic check)
    // You might want to add more sophisticated bounds checking based on your world size
    if (position.y < 0 || position.y > 255) {
        return false;
    }
    
    // Check if the position is already occupied
    VoxelEngine::World::Voxel existingVoxel = worldManager->getVoxel(position.x, position.y, position.z);
    if (existingVoxel.id != static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)) {
        return false;
    }
    
    // Check if placement would interfere with player position
    glm::vec3 playerPos = const_cast<Game&>(game).getPlayerPosition();
    glm::vec3 blockPos = glm::vec3(position);
    
    // Simple collision check - ensure block isn't placed inside player
    float distance = glm::distance(playerPos, blockPos);
    if (distance < 2.0f) { // Player needs some space
        return false;
    }
    
    return true;
}

void BlockPlacement::markChunkDirtyForPosition(VoxelCastle::World::WorldManager* worldManager,
                                              const glm::ivec3& position) {
    if (!worldManager) {
        std::cout << "[BlockPlacement] ERROR: WorldManager is null, cannot mark chunk dirty" << std::endl;
        return;
    }
    
    // Calculate which chunk column this position belongs to
    int_fast64_t chunkX = VoxelCastle::World::WorldManager::worldToColumnBaseX(position.x);
    int_fast64_t chunkZ = VoxelCastle::World::WorldManager::worldToColumnBaseZ(position.z);
    
    // Get the chunk column
    auto* chunkColumn = worldManager->getChunkColumn(chunkX, chunkZ);
    if (!chunkColumn) {
        std::cout << "[BlockPlacement] Warning: No chunk column found at (" << chunkX << ", " << chunkZ << ")" << std::endl;
        return;
    }
    
    // Calculate which segment within the column
    int_fast32_t segmentY = static_cast<int_fast32_t>(position.y / VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT);
    
    // Get the segment and mark it dirty
    auto* segment = chunkColumn->getSegmentByIndex(segmentY);
    if (segment) {
        segment->markDirty(true);
        std::cout << "[BlockPlacement] Marked chunk segment (" << chunkX << ", " << segmentY << ", " << chunkZ << ") dirty for mesh update" << std::endl;
    } else {
        std::cout << "[BlockPlacement] Warning: No chunk segment found at Y index " << segmentY << std::endl;
    }
}
