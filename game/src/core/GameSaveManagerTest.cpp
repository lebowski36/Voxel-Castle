/**
 * @file GameSaveManagerTest.cpp
 * @brief Game class SaveManager test methods
 */

#include "core/game.h"
#include "core/SaveManagerTest.h"
#include <iostream>

void Game::runSaveManagerTest() {
    std::cout << "[Game] Running SaveManager test..." << std::endl;
    
    try {
        VoxelCastle::Core::runSaveManagerTest();
        std::cout << "[Game] ✓ SaveManager test completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Game] ✗ SaveManager test failed: " << e.what() << std::endl;
    }
}
