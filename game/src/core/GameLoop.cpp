#include "../../include/core/GameLoop.h"
#include "../../include/core/game.h"
#include <iostream>
#include "../utils/debug_logger.h"
#include <thread>

int GameLoop::run(Game& game) {
    // Check if game is properly initialized
    if (!game.isRunning() || !game.isWindowRunning()) {
        std::cerr << "[GameLoop] Game is not initialized or not running." << std::endl;
        return -1;
    }

    std::cout << "[GameLoop] Starting main game loop..." << std::endl;
    INFO_LOG("GameLoop", "Starting main game loop...");
    
    int frameCount = 0;
    auto lastFrameTime = std::chrono::steady_clock::now();
    auto lastWindowCheckTime = std::chrono::steady_clock::now();

    while (game.isRunning() && game.isWindowRunning()) {
        frameCount++;
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = calculateDeltaTime(currentTime, lastFrameTime);
        lastFrameTime = currentTime;

        // Check window state periodically - more frequently during block operations
        bool shouldLogFrame = (frameCount % 50 == 0) || game.hasPendingBlockAction();
        auto timeSinceWindowCheck = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastWindowCheckTime);

        if (shouldLogFrame || timeSinceWindowCheck.count() > 1000) { // Check at least every second
            bool windowRunning = game.isWindowRunning();
            DEBUG_LOG("GameLoop", "Frame " + std::to_string(frameCount) +
                ", window valid: " + std::to_string(game.hasWindow()) +
                ", window running: " + std::to_string(windowRunning) +
                ", deltaTime: " + std::to_string(deltaTime) + "s");
            lastWindowCheckTime = currentTime;

            if (!windowRunning) {
                std::cerr << "[GameLoop] CRITICAL: Window stopped running! Breaking main loop." << std::endl;
                CRITICAL_LOG("GameLoop", "CRITICAL: Window stopped running! Breaking main loop.");
                break;
            }
        }

        // Ensure deltaTime is not excessively large (e.g., after a breakpoint or long pause)
        // This can prevent jerky movements or physics explosions.
        const float max_deltaTime = 0.25f;
        if (deltaTime > max_deltaTime) {
            deltaTime = max_deltaTime;
        }

        if (shouldLogFrame) {
            DEBUG_LOG("GameLoop", "Processing input...");
        }

        // Process input through Game's interface
        game.processInput();

        // Check if game wants to exit (processInput might have changed isRunning)
        if (!game.isRunning()) {
            INFO_LOG("GameLoop", "Game signaled exit, breaking loop");
            break;
        }

        if (shouldLogFrame) {
            DEBUG_LOG("GameLoop", "Calling update...");
        }
        game.update(deltaTime);

        if (shouldLogFrame) {
            DEBUG_LOG("GameLoop", "Calling render...");
        }
        game.render();

        if (shouldLogFrame) {
            DEBUG_LOG("GameLoop", "Frame " + std::to_string(frameCount) + " completed successfully");
        }

        // Apply frame rate limiting
        limitFrameRate(currentTime);
    }

    std::cout << "[GameLoop] Main game loop ended after " << frameCount << " frames." << std::endl;
    INFO_LOG("GameLoop", "Main game loop ended after " + std::to_string(frameCount) + " frames.");
    return 0;
}

float GameLoop::calculateDeltaTime(const std::chrono::steady_clock::time_point& currentTime,
                                 const std::chrono::steady_clock::time_point& lastFrameTime) {
    return std::chrono::duration<float>(currentTime - lastFrameTime).count();
}

void GameLoop::limitFrameRate(const std::chrono::steady_clock::time_point& frameStart) {
    auto frameEnd = std::chrono::steady_clock::now();
    auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
    
    if (frameDuration < FRAME_DURATION) {
        std::this_thread::sleep_for(FRAME_DURATION - frameDuration);
    }
}
