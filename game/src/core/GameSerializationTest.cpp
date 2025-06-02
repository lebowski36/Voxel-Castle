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
