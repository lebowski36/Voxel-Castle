#include "core/GameLogic.h"
#include "core/game.h"
#include "world/world_manager.h"
#include "rendering/texture_atlas.h"
#include "rendering/mesh_builder.h"
#include <iostream>

namespace GameLogic {

void update(Game& game, float deltaTime) {
    // Process mouse look if mouse is captured and there was movement
    if (game.mouseCaptured_ && game.camera_ && (game.mouseDeltaX_ != 0.0f || game.mouseDeltaY_ != 0.0f)) {
        game.camera_->processMouse(game.mouseDeltaX_, game.mouseDeltaY_);
        // mouseDeltaX_ and mouseDeltaY_ are reset in processInput() each frame before polling
    }

    // Process keyboard movement for the camera
    if (game.camera_) {
        game.camera_->processKeyboard(deltaTime, game.forward_, game.backward_, game.left_, game.right_, game.up_, game.down_, game.speedMultiplier_);
    }

    // Progress ECS systems
    if (game.ecs_) {
        game.ecs_->progress(deltaTime); // Pass deltaTime if your system uses it
    }

    // Update game window (if it has its own update logic, e.g., title, etc.)
    if (game.gameWindow_) {
        game.gameWindow_->update(); 
    }

    // --- Game Specific Update Logic ---
    if (game.manualVoxelChangeRequested_ && game.worldManager_) {
        std::cout << "M key pressed. Inverting checkerboard pattern in segment (0,0,0)..." << std::endl;
        VoxelCastle::World::ChunkColumn* column = game.worldManager_->getChunkColumn(0, 0);
        if (column) {
            VoxelCastle::World::ChunkSegment* segment = column->getSegmentByIndex(0); // Target the first segment
            if (segment) {
                for (uint8_t lsy = 0; lsy < VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT; ++lsy) {
                    for (uint8_t lsz = 0; lsz < VoxelCastle::World::ChunkSegment::CHUNK_DEPTH; ++lsz) {
                        for (uint8_t lsx = 0; lsx < VoxelCastle::World::ChunkSegment::CHUNK_WIDTH; ++lsx) {
                            int_fast64_t worldX = static_cast<int_fast64_t>(lsx);
                            int_fast64_t worldY = static_cast<int_fast64_t>(lsy);
                            int_fast64_t worldZ = static_cast<int_fast64_t>(lsz);

                            ::VoxelEngine::World::Voxel currentVoxel = game.worldManager_->getVoxel(worldX, worldY, worldZ);
                            ::VoxelEngine::World::VoxelType currentType = static_cast<::VoxelEngine::World::VoxelType>(currentVoxel.id);
                            ::VoxelEngine::World::VoxelType newType = currentType;

                            if (currentType == static_cast<::VoxelEngine::World::VoxelType>(1)) {
                                newType = static_cast<::VoxelEngine::World::VoxelType>(2);
                            } else if (currentType == static_cast<::VoxelEngine::World::VoxelType>(2)) {
                                newType = static_cast<::VoxelEngine::World::VoxelType>(1);
                            }

                            if (newType != currentType) {
                                game.worldManager_->setVoxel(worldX, worldY, worldZ, newType);
                            }
                        }
                    }
                }
                std::cout << "Checkerboard inversion applied. Segment (0,0,0) marked dirty." << std::endl;
            } else {
                std::cerr << "Error: Could not get segment 0 for inversion." << std::endl;
            }
        } else {
            std::cerr << "Error: Could not get chunk column (0,0) for inversion." << std::endl;
        }
        game.manualVoxelChangeRequested_ = false; // Reset the flag
    }

    // Update meshes if any segments are dirty
    if (game.worldManager_ && game.textureAtlas_ && game.meshBuilder_) {
        game.worldManager_->updateDirtyMeshes(*game.textureAtlas_, *game.meshBuilder_);
    }
}

} // namespace GameLogic
