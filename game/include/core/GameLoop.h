#pragma once

#include <chrono>

// Forward declaration
class Game;

/**
 * @brief Handles the main game loop execution
 * 
 * This class extracts the game loop logic from the Game class to improve modularity.
 * It manages frame timing, delta time calculation, and coordinates update/render calls.
 */
class GameLoop {
public:
    GameLoop() = default;
    ~GameLoop() = default;

    /**
     * @brief Run the main game loop
     * @param game Reference to the Game instance
     * @return Exit code (0 for normal exit)
     */
    int run(Game& game);

private:
    /**
     * @brief Calculate delta time between frames
     * @param currentTime Current frame time
     * @param lastFrameTime Previous frame time
     * @return Delta time in seconds
     */
    float calculateDeltaTime(const std::chrono::steady_clock::time_point& currentTime,
                           const std::chrono::steady_clock::time_point& lastFrameTime);

    /**
     * @brief Apply frame rate limiting if needed
     * @param frameStart Time when frame processing started
     */
    void limitFrameRate(const std::chrono::steady_clock::time_point& frameStart);

    // Frame timing constants
    static constexpr int TARGET_FPS = 60;
    static constexpr std::chrono::milliseconds FRAME_DURATION{1000 / TARGET_FPS};
};
