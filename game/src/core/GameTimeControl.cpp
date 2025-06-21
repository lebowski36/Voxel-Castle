#include "core/game.h"
#include "core/TimeManager.h"

using namespace VoxelCastle::Core;

// Time control method implementations - delegating to TimeManager

bool Game::isPauseSimulation() const {
    return timeManager_ ? timeManager_->isPaused() : false;
}

bool Game::togglePauseSimulation() {
    if (timeManager_) {
        bool isPaused = timeManager_->togglePause();
        
        // TODO: Update UI elements based on pause state using new UI system
        // The new MenuSystem will handle pause indicators when implemented
        
        return isPaused;
    }
    return false;
}

void Game::setTimeScale(TimeManager::TimeScale scale) {
    if (timeManager_) {
        timeManager_->setTimeScale(scale);
    }
}

void Game::increaseTimeScale() {
    if (!timeManager_) return;
    
    TimeManager::TimeScale currentScale = timeManager_->getTimeScale();
    
    // Increase to next level
    switch (currentScale) {
        case TimeManager::TimeScale::VERY_SLOW:
            timeManager_->setTimeScale(TimeManager::TimeScale::SLOW);
            break;
        case TimeManager::TimeScale::SLOW:
            timeManager_->setTimeScale(TimeManager::TimeScale::NORMAL);
            break;
        case TimeManager::TimeScale::NORMAL:
            timeManager_->setTimeScale(TimeManager::TimeScale::FAST);
            break;
        case TimeManager::TimeScale::FAST:
            timeManager_->setTimeScale(TimeManager::TimeScale::VERY_FAST);
            break;
        default:
            // Already at max or custom value, do nothing
            break;
    }
}

void Game::decreaseTimeScale() {
    if (!timeManager_) return;
    
    TimeManager::TimeScale currentScale = timeManager_->getTimeScale();
    
    // Decrease to next level
    switch (currentScale) {
        case TimeManager::TimeScale::VERY_FAST:
            timeManager_->setTimeScale(TimeManager::TimeScale::FAST);
            break;
        case TimeManager::TimeScale::FAST:
            timeManager_->setTimeScale(TimeManager::TimeScale::NORMAL);
            break;
        case TimeManager::TimeScale::NORMAL:
            timeManager_->setTimeScale(TimeManager::TimeScale::SLOW);
            break;
        case TimeManager::TimeScale::SLOW:
            timeManager_->setTimeScale(TimeManager::TimeScale::VERY_SLOW);
            break;
        default:
            // Already at min or custom value, do nothing
            break;
    }
}

float Game::getTimeScaleFactor() const {
    return timeManager_ ? timeManager_->getTimeScaleFactor() : 1.0f;
}

std::string Game::getTimeScaleString() const {
    return timeManager_ ? timeManager_->getTimeScaleString() : "Normal (1.0x)";
}
