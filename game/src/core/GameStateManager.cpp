#include "core/GameStateManager.h"
#include "core/game.h" // For GameState enum
#include <iostream>
#include <algorithm>

namespace VoxelCastle {
namespace Core {

GameStateManager::GameStateManager() 
    : currentState_(GameState::STRATEGIC_MODE)
    , previousState_(GameState::STRATEGIC_MODE) {
    setupTransitionRules();
}

void GameStateManager::initialize(GameState initialState) {
    currentState_ = initialState;
    previousState_ = initialState;
    
    if (debugLogging_) {
        std::cout << "[GameStateManager] Initialized with state: " << getStateName(initialState) << std::endl;
    }
}

bool GameStateManager::requestStateChange(GameState newState, bool force) {
    if (newState == currentState_) {
        return true; // Already in requested state
    }

    // Validate transition unless forced
    if (!force && !isTransitionValid(currentState_, newState)) {
        if (debugLogging_) {
            std::cout << "[GameStateManager] Transition blocked: " << getStateName(currentState_) 
                      << " -> " << getStateName(newState) << std::endl;
        }
        return false;
    }

    executeStateTransition(newState);
    return true;
}

bool GameStateManager::isTransitionValid(GameState from, GameState to) const {
    return checkTransitionRules(from, to);
}

bool GameStateManager::pushState(GameState newState) {
    if (!isTransitionValid(currentState_, newState)) {
        return false;
    }

    stateStack_.push_back(currentState_);
    executeStateTransition(newState);
    
    if (debugLogging_) {
        std::cout << "[GameStateManager] Pushed state, stack size: " << stateStack_.size() << std::endl;
    }
    
    return true;
}

bool GameStateManager::popState() {
    if (stateStack_.empty()) {
        if (debugLogging_) {
            std::cout << "[GameStateManager] Cannot pop state: stack is empty" << std::endl;
        }
        return false;
    }

    GameState restoredState = stateStack_.back();
    stateStack_.pop_back();
    
    executeStateTransition(restoredState);
    
    if (debugLogging_) {
        std::cout << "[GameStateManager] Popped state, stack size: " << stateStack_.size() << std::endl;
    }
    
    return true;
}

void GameStateManager::registerStateChangeCallback(StateChangeCallback callback) {
    callbacks_.push_back(callback);
}

void GameStateManager::clearCallbacks() {
    callbacks_.clear();
}

bool GameStateManager::isPlaying() const {
    return currentState_ == GameState::PLAYING || 
           currentState_ == GameState::FIRST_PERSON_MODE || 
           currentState_ == GameState::STRATEGIC_MODE || 
           currentState_ == GameState::HYBRID_MODE;
}

bool GameStateManager::isPaused() const {
    return currentState_ == GameState::MENU || 
           currentState_ == GameState::PAUSED;
}

bool GameStateManager::isInTransition() const {
    return currentState_ == GameState::TRANSITIONING ||
           currentState_ == GameState::LOADING ||
           currentState_ == GameState::SAVING ||
           currentState_ == GameState::AUTO_SAVING;
}

bool GameStateManager::canAcceptInput() const {
    return isPlaying() && !isInTransition();
}

bool GameStateManager::isInMenu() const {
    return currentState_ == GameState::MENU;
}

std::string GameStateManager::getStateName(GameState state) {
    switch (state) {
        case GameState::PLAYING:              return "PLAYING";
        case GameState::MENU:                 return "MENU";
        case GameState::FIRST_PERSON_MODE:    return "FIRST_PERSON_MODE";
        case GameState::STRATEGIC_MODE:       return "STRATEGIC_MODE";
        case GameState::HYBRID_MODE:          return "HYBRID_MODE";
        case GameState::TRANSITIONING:        return "TRANSITIONING";
        case GameState::LOADING:              return "LOADING";
        case GameState::SAVING:               return "SAVING";
        case GameState::PAUSED:               return "PAUSED";
        case GameState::AUTO_SAVING:          return "AUTO_SAVING";
        default:                              return "UNKNOWN";
    }
}

void GameStateManager::setupTransitionRules() {
    // Clear existing rules
    transitionRules_.clear();
    
    auto addRule = [this](GameState from, GameState to) {
        transitionRules_[static_cast<int>(from)].push_back(static_cast<int>(to));
    };

    // From PLAYING - legacy state, can transition to control modes or menu
    addRule(GameState::PLAYING, GameState::FIRST_PERSON_MODE);
    addRule(GameState::PLAYING, GameState::STRATEGIC_MODE);
    addRule(GameState::PLAYING, GameState::HYBRID_MODE);
    addRule(GameState::PLAYING, GameState::MENU);
    addRule(GameState::PLAYING, GameState::PAUSED);

    // From FIRST_PERSON_MODE
    addRule(GameState::FIRST_PERSON_MODE, GameState::STRATEGIC_MODE);
    addRule(GameState::FIRST_PERSON_MODE, GameState::HYBRID_MODE);
    addRule(GameState::FIRST_PERSON_MODE, GameState::MENU);
    addRule(GameState::FIRST_PERSON_MODE, GameState::PAUSED);
    addRule(GameState::FIRST_PERSON_MODE, GameState::TRANSITIONING);
    addRule(GameState::FIRST_PERSON_MODE, GameState::SAVING);
    addRule(GameState::FIRST_PERSON_MODE, GameState::AUTO_SAVING);

    // From STRATEGIC_MODE
    addRule(GameState::STRATEGIC_MODE, GameState::FIRST_PERSON_MODE);
    addRule(GameState::STRATEGIC_MODE, GameState::HYBRID_MODE);
    addRule(GameState::STRATEGIC_MODE, GameState::MENU);
    addRule(GameState::STRATEGIC_MODE, GameState::PAUSED);
    addRule(GameState::STRATEGIC_MODE, GameState::TRANSITIONING);
    addRule(GameState::STRATEGIC_MODE, GameState::SAVING);
    addRule(GameState::STRATEGIC_MODE, GameState::AUTO_SAVING);

    // From HYBRID_MODE
    addRule(GameState::HYBRID_MODE, GameState::FIRST_PERSON_MODE);
    addRule(GameState::HYBRID_MODE, GameState::STRATEGIC_MODE);
    addRule(GameState::HYBRID_MODE, GameState::MENU);
    addRule(GameState::HYBRID_MODE, GameState::PAUSED);
    addRule(GameState::HYBRID_MODE, GameState::TRANSITIONING);

    // From MENU - can return to any playing state
    addRule(GameState::MENU, GameState::FIRST_PERSON_MODE);
    addRule(GameState::MENU, GameState::STRATEGIC_MODE);
    addRule(GameState::MENU, GameState::HYBRID_MODE);
    addRule(GameState::MENU, GameState::PLAYING); // For backwards compatibility
    addRule(GameState::MENU, GameState::LOADING);

    // From PAUSED - similar to menu
    addRule(GameState::PAUSED, GameState::FIRST_PERSON_MODE);
    addRule(GameState::PAUSED, GameState::STRATEGIC_MODE);
    addRule(GameState::PAUSED, GameState::HYBRID_MODE);
    addRule(GameState::PAUSED, GameState::MENU);

    // From TRANSITIONING - can go to any playing state
    addRule(GameState::TRANSITIONING, GameState::FIRST_PERSON_MODE);
    addRule(GameState::TRANSITIONING, GameState::STRATEGIC_MODE);
    addRule(GameState::TRANSITIONING, GameState::HYBRID_MODE);

    // From LOADING - typically goes to a playing state
    addRule(GameState::LOADING, GameState::FIRST_PERSON_MODE);
    addRule(GameState::LOADING, GameState::STRATEGIC_MODE);
    addRule(GameState::LOADING, GameState::HYBRID_MODE);
    addRule(GameState::LOADING, GameState::MENU); // If load fails

    // From SAVING/AUTO_SAVING - return to previous playing state
    addRule(GameState::SAVING, GameState::FIRST_PERSON_MODE);
    addRule(GameState::SAVING, GameState::STRATEGIC_MODE);
    addRule(GameState::SAVING, GameState::HYBRID_MODE);
    addRule(GameState::SAVING, GameState::MENU);

    addRule(GameState::AUTO_SAVING, GameState::FIRST_PERSON_MODE);
    addRule(GameState::AUTO_SAVING, GameState::STRATEGIC_MODE);
    addRule(GameState::AUTO_SAVING, GameState::HYBRID_MODE);
}

bool GameStateManager::checkTransitionRules(GameState from, GameState to) const {
    auto it = transitionRules_.find(static_cast<int>(from));
    if (it == transitionRules_.end()) {
        return false; // No rules defined for this state
    }

    const auto& allowedStates = it->second;
    return std::find(allowedStates.begin(), allowedStates.end(), static_cast<int>(to)) != allowedStates.end();
}

void GameStateManager::executeStateTransition(GameState newState) {
    GameState oldState = currentState_;
    previousState_ = currentState_;
    currentState_ = newState;

    if (debugLogging_) {
        std::cout << "[GameStateManager] State transition: " << getStateName(oldState) 
                  << " -> " << getStateName(newState) << std::endl;
    }

    notifyStateChange(oldState, newState);
}

void GameStateManager::notifyStateChange(GameState from, GameState to) {
    for (const auto& callback : callbacks_) {
        callback(from, to);
    }
}

} // namespace Core
} // namespace VoxelCastle
