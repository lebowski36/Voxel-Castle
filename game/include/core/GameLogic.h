#pragma once

class Game;

namespace GameLogic {
    /**
     * @brief Handles per-frame game logic updates (e.g., camera, ECS, custom logic).
     * @param game Reference to the main Game object.
     * @param deltaTime Time elapsed since last frame.
     */
    void update(Game& game, float deltaTime);
}
