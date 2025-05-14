#pragma once

class Game; // Forward declaration

namespace GameInput {
    /**
     * Process SDL events and update Game state (input flags, camera, quitting, etc.).
     * Accesses Game's private members via friend declaration.
     */
    void processInput(Game& game); // Changed ::Game& to Game&
}
