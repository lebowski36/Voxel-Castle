/*
 * Game Serialization Test Implementation
 * 
 * This file contains the test caller for the serialization framework.
 * 
 * STATUS: Phase 1 (Core Serialization Framework) ✅ COMPLETED
 * - Binary serialization/deserialization working
 * - Serializable interface implemented and tested
 * - WorldManager modification tracking implemented
 * - All tests passing, ready for Phase 2 (SaveManager class)
 */

#include "core/game.h"
#include <iostream>

// Forward declaration
namespace VoxelCastle {
namespace Core {
    bool testSerialization();
}
}

void Game::runSerializationTest() {
    std::cout << "\n=== Running Serialization Test ===" << std::endl;
    
    if (VoxelCastle::Core::testSerialization()) {
        std::cout << "[Game] Serialization test completed successfully!" << std::endl;
    } else {
        std::cout << "[Game] Serialization test failed!" << std::endl;
    }
    
    std::cout << "=== Serialization Test Complete ===\n" << std::endl;
}
