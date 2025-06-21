#include "core/game.h"
#include "core/GameStateManager.h"
#include <iostream>
#include "ui/MenuSystem.h"
#include "platform/Window.h"  // Added Window include
#include "utils/debug_logger.h" // Added for DEBUG_LOG

using namespace VoxelCastle::Core;

// Game state management method implementations - delegating to GameStateManager

GameState Game::getGameState() const { 
    return stateManager_ ? stateManager_->getCurrentState() : gameState_;
}

void Game::setGameState(GameState state) {
    if (stateManager_) {
        stateManager_->requestStateChange(state);
    } else {
        // Fallback if state manager not initialized
        gameState_ = state;
    }
}

bool Game::isMenuOpen() const {
    return stateManager_ ? stateManager_->isInMenu() : (gameState_ == GameState::MENU);
}

bool Game::isPlaying() const {
    return stateManager_ ? stateManager_->isPlaying() : 
        (gameState_ == GameState::PLAYING || 
         gameState_ == GameState::FIRST_PERSON_MODE || 
         gameState_ == GameState::STRATEGIC_MODE || 
         gameState_ == GameState::HYBRID_MODE);
}

bool Game::isPaused() const {
    return stateManager_ ? stateManager_->isPaused() : 
        (gameState_ == GameState::MENU || gameState_ == GameState::PAUSED);
}

bool Game::isInTransition() const {
    return stateManager_ ? stateManager_->isInTransition() : 
        (gameState_ == GameState::TRANSITIONING || 
         gameState_ == GameState::LOADING || 
         gameState_ == GameState::SAVING || 
         gameState_ == GameState::AUTO_SAVING);
}

bool Game::canAcceptInput() const {
    return stateManager_ ? stateManager_->canAcceptInput() : (isPlaying() && !isInTransition());
}

// State change event handler
void Game::onStateChanged(GameState from, GameState to) {
    // Suppress unused parameter warning
    (void)from;
    
    // Update the legacy gameState_ variable to maintain backward compatibility
    gameState_ = to;
    
    // Handle specific state transitions
    if (isPlaying() && !isPaused()) {
        // Entering gameplay state
        setMouseCaptured(true);
        
        // TODO: Show game UI elements using new UI system
        // The new UI system will handle HUD and crosshair rendering
        // once fully implemented
        
        // Set appropriate camera mode based on game state
        if (to == GameState::FIRST_PERSON_MODE && cameraMode_ != CameraMode::FIRST_PERSON) {
            setCameraMode(CameraMode::FIRST_PERSON);
        }
        else if (to == GameState::STRATEGIC_MODE && cameraMode_ != CameraMode::FREE_FLYING) {
            setCameraMode(CameraMode::FREE_FLYING);
        }
        
        std::cout << "[Game] Entered playing state: " << VoxelCastle::Core::GameStateManager::getStateName(to) << std::endl;
    }
    else if (to == GameState::MENU) {
        // Entering menu state
        setMouseCaptured(false);
        
        // TODO: Hide game UI elements using new UI system
        // The new UI system will handle HUD and crosshair visibility
        // once fully implemented
        
        // Show menu
        if (menuSystem_) {
            menuSystem_->showMainMenu();
        }
        
        std::cout << "[Game] Menu opened - game paused, cursor visible" << std::endl;
    }
    
    // Handle transition states
    if (to == GameState::LOADING) {
        std::cout << "[Game] Loading game state..." << std::endl;
        // Future: Show loading screen, etc.
    }
    else if (to == GameState::SAVING) {
        std::cout << "[Game] Saving game state..." << std::endl;
        // Future: Show saving indicator, etc.
    }
}
