
#include "core/GameLogic.h"
#include "core/game.h"
#include "world/world_manager.h"
#include "rendering/texture_atlas.h"
#include "rendering/mesh_builder.h"
#include <iostream>
#include <flecs.h> // For flecs::world definition
#include "platform/Window.h" // For Window definition

namespace GameLogic {

void update(Game& game, float deltaTime) {
    // Process mouse look if mouse is captured and there was movement
    if (game.isMouseCaptured() && game.getCamera() && (game.getMouseDeltaX() != 0.0f || game.getMouseDeltaY() != 0.0f)) {
        game.getCamera()->processMouse(game.getMouseDeltaX(), game.getMouseDeltaY());
        // mouseDeltaX_ and mouseDeltaY_ are reset in processInput() each frame before polling
    }

    // Process keyboard movement for the camera
    if (game.getCamera()) {
        game.getCamera()->processKeyboard(deltaTime, game.isForward(), game.isBackward(), game.isLeft(), game.isRight(), game.isUp(), game.isDown(), game.getSpeedMultiplier());
    }

    // Progress ECS systems
    if (game.getECS()) {
        game.getECS()->progress(deltaTime); // Pass deltaTime if your system uses it
    }

    // Update game window (if it has its own update logic, e.g., title, etc.)
    if (game.getWindow()) {
        game.getWindow()->update(); 
    }

    // --- Game Specific Update Logic ---
    if (game.isManualVoxelChangeRequested() && game.getWorldManager()) {
        std::cout << "M key pressed. Inverting checkerboard pattern in segment (0,0,0)..." << std::endl;
        VoxelCastle::World::ChunkColumn* column = game.getWorldManager()->getChunkColumn(0, 0);
        if (column) {
            VoxelCastle::World::ChunkSegment* segment = column->getSegmentByIndex(0); // Target the first segment
            if (segment) {
                for (uint8_t lsy = 0; lsy < VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT; ++lsy) {
                    for (uint8_t lsz = 0; lsz < VoxelCastle::World::ChunkSegment::CHUNK_DEPTH; ++lsz) {
                        for (uint8_t lsx = 0; lsx < VoxelCastle::World::ChunkSegment::CHUNK_WIDTH; ++lsx) {
                            int_fast64_t worldX = static_cast<int_fast64_t>(lsx);
                            int_fast64_t worldY = static_cast<int_fast64_t>(lsy);
                            int_fast64_t worldZ = static_cast<int_fast64_t>(lsz);

                            ::VoxelEngine::World::Voxel currentVoxel = game.getWorldManager()->getVoxel(worldX, worldY, worldZ);
                            ::VoxelEngine::World::VoxelType currentType = static_cast<::VoxelEngine::World::VoxelType>(currentVoxel.id);
                            ::VoxelEngine::World::VoxelType newType = currentType;

                            if (currentType == static_cast<::VoxelEngine::World::VoxelType>(1)) {
                                newType = static_cast<::VoxelEngine::World::VoxelType>(2);
                            } else if (currentType == static_cast<::VoxelEngine::World::VoxelType>(2)) {
                                newType = static_cast<::VoxelEngine::World::VoxelType>(1);
                            }

                            if (newType != currentType) {
                                game.getWorldManager()->setVoxel(worldX, worldY, worldZ, newType);
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
        game.setManualVoxelChangeRequested(false); // Reset the flag
    }

    // Asynchronous mesh generation: enqueue jobs and upload finished meshes
    if (game.getWorldManager() && game.getTextureAtlas() && game.getMeshBuilder()) {
        game.getWorldManager()->enqueueDirtyMeshJobs(*game.getTextureAtlas(), *game.getMeshBuilder());
        game.getWorldManager()->processFinishedMeshJobs();
    }
}

} // namespace GameLogic
