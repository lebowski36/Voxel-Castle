#include "core/GameLogic.h"
#include "core/game.h"
#include "core/CameraMode.h"
#include "physics/PlayerPhysics.h"
#include "world/world_manager.h"
#include "rendering/texture_atlas.h"
#include "rendering/mesh_builder.h"
#include "interaction/BlockPlacement.h"
#include <iostream>
#include <flecs.h> // For flecs::world definition
#include "platform/Window.h" // For Window definition
#include <chrono>
#include <sstream>
#include <iomanip>

namespace GameLogic {

// Timestamp helper for game logic
static std::string getTimestampGL() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void update(Game& game, float deltaTime) {
    static int frameCount = 0;
    frameCount++;
    
    // Log every 100 frames or when block actions occur
    bool shouldLogFrame = (frameCount % 100 == 0) || game.hasPendingBlockAction();
    
    if (shouldLogFrame) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Update start, frame: " << frameCount 
                  << ", deltaTime: " << deltaTime << "s" << std::endl;
    }
    
    // Always process mouse look if mouse is captured and there was movement
    // This ensures camera panning works in all modes
    if (game.isMouseCaptured() && game.getCamera() && (game.getMouseDeltaX() != 0.0f || game.getMouseDeltaY() != 0.0f)) {
        game.getCamera()->processMouse(game.getMouseDeltaX(), game.getMouseDeltaY());
        // mouseDeltaX_ and mouseDeltaY_ are reset in processInput() each frame before polling
    }

    // Handle camera/player movement based on current camera mode
    if (game.getCamera()) {
        if (game.getCameraMode() == CameraMode::FREE_FLYING) {
            // Use original free-flying camera movement
            game.getCamera()->processKeyboard(deltaTime, game.isForward(), game.isBackward(), 
                                            game.isLeft(), game.isRight(), game.isUp(), game.isDown(), 
                                            game.getSpeedMultiplier());
        } else if (game.getCameraMode() == CameraMode::FIRST_PERSON) {
            // Use the dedicated PlayerPhysics system for first-person movement
            PlayerPhysics::update(game, deltaTime);
        }
    }

    // Handle pending block placement/removal actions (from mouse clicks)
    if (game.hasPendingBlockAction()) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] CRITICAL: Processing pending block action: " 
                  << (game.isBlockPlacement() ? "PLACEMENT" : "REMOVAL") << std::endl;
        try {
            std::cout << "[" << getTimestampGL() << "] [GameLogic] Calling BlockPlacement::handleMouseClick..." << std::endl;
            BlockPlacement::handleMouseClick(game, game.isBlockPlacement());
            std::cout << "[" << getTimestampGL() << "] [GameLogic] Block action completed successfully" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[" << getTimestampGL() << "] [GameLogic] EXCEPTION during block action: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "[" << getTimestampGL() << "] [GameLogic] UNKNOWN EXCEPTION during block action!" << std::endl;
        }
        game.clearPendingBlockAction(); // Reset flag after handling
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Block action processing completed" << std::endl;
    }

    // Progress ECS systems
    if (shouldLogFrame) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Progressing ECS systems..." << std::endl;
    }
    if (game.getECS()) {
        game.getECS()->progress(deltaTime); // Pass deltaTime if your system uses it
    }

    // Update game window (if it has its own update logic, e.g., title, etc.)
    if (shouldLogFrame) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Updating window..." << std::endl;
    }
    if (game.getWindow()) {
        game.getWindow()->update(); 
    }
    if (shouldLogFrame) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Window update completed" << std::endl;
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
    if (shouldLogFrame) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Processing mesh jobs..." << std::endl;
    }
    if (game.getWorldManager() && game.getTextureAtlas() && game.getMeshBuilder()) {
        game.getWorldManager()->enqueueDirtyMeshJobs(*game.getTextureAtlas(), *game.getMeshBuilder());
        game.getWorldManager()->processFinishedMeshJobs();
    }
    if (shouldLogFrame) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Mesh jobs processing completed" << std::endl;
    }
    
    if (shouldLogFrame) {
        std::cout << "[" << getTimestampGL() << "] [GameLogic] Update completed successfully" << std::endl;
    }
}

} // namespace GameLogic
