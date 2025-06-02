#include "core/TimeManager.h"
#include <flecs.h>
#include <iostream>
#include <algorithm>

namespace VoxelCastle {
namespace Core {

TimeManager::TimeManager(flecs::world* world) 
    : isPaused_(false)
    , timeScaleFactor_(1.0f)
    , currentTimeScale_(TimeScale::NORMAL)
    , ecsWorld_(world) {
    // Initialize systems that should always run regardless of pause state
    alwaysRunSystems_ = {"RenderSystem", "InputSystem", "UISystem", "HUDSystem"};
}

void TimeManager::initialize() {
    if (!ecsWorld_) {
        std::cerr << "[TimeManager] Error: Cannot initialize - ECS world is null" << std::endl;
        return;
    }

    std::cout << "[TimeManager] Initialized with scale: " << getTimeScaleString() << std::endl;
}

float TimeManager::update(float realDeltaTime) {
    // If paused, return 0 delta time for simulation systems
    if (isPaused_) {
        return 0.0f;
    }
    
    // Apply time scale factor to get simulation delta time
    float scaledDeltaTime = realDeltaTime * timeScaleFactor_;
    
    // Update accumulated time counters
    accumulatedTime_ += realDeltaTime;
    simulationTime_ += scaledDeltaTime;
    
    return scaledDeltaTime;
}

bool TimeManager::pause() {
    if (isPaused_) {
        return true; // Already paused
    }
    
    // Store current time scale to restore later
    previousTimeScaleFactor_ = timeScaleFactor_;
    isPaused_ = true;
    
    // Apply pause state to systems
    applyTimeScaleToSystems();
    
    std::cout << "[TimeManager] Simulation paused" << std::endl;
    return true;
}

bool TimeManager::resume() {
    if (!isPaused_) {
        return true; // Already running
    }
    
    // Restore previous time scale
    isPaused_ = false;
    timeScaleFactor_ = previousTimeScaleFactor_;
    
    // Apply resumed state to systems
    applyTimeScaleToSystems();
    
    std::cout << "[TimeManager] Simulation resumed at " << getTimeScaleString() << std::endl;
    return true;
}

bool TimeManager::togglePause() {
    if (isPaused_) {
        resume();
    } else {
        pause();
    }
    return isPaused_;
}

bool TimeManager::isPaused() const {
    return isPaused_;
}

void TimeManager::setTimeScale(TimeScale scale) {
    if (scale == TimeScale::PAUSED) {
        pause();
        return;
    }
    
    // Store old scale for callbacks
    float oldScale = timeScaleFactor_;
    
    // Update current scale
    currentTimeScale_ = scale;
    timeScaleFactor_ = timeScaleToFactor(scale);
    
    // If we were paused, resume
    if (isPaused_) {
        resume();
    } else {
        // Apply new scale to systems
        applyTimeScaleToSystems();
    }
    
    // Notify listeners
    for (const auto& callback : timeScaleCallbacks_) {
        callback(oldScale, timeScaleFactor_);
    }
    
    std::cout << "[TimeManager] Time scale set to: " << getTimeScaleString() << std::endl;
}

void TimeManager::setTimeScaleFactor(float scale) {
    if (scale <= 0.0f) {
        pause();
        return;
    }
    
    // Store old scale for callbacks
    float oldScale = timeScaleFactor_;
    
    // Update current scale
    timeScaleFactor_ = scale;
    currentTimeScale_ = factorToTimeScale(scale);
    
    // If we were paused, resume
    if (isPaused_) {
        resume();
    } else {
        // Apply new scale to systems
        applyTimeScaleToSystems();
    }
    
    // Notify listeners
    for (const auto& callback : timeScaleCallbacks_) {
        callback(oldScale, timeScaleFactor_);
    }
    
    std::cout << "[TimeManager] Time scale factor set to: " << scale << "x" << std::endl;
}

TimeManager::TimeScale TimeManager::getTimeScale() const {
    return currentTimeScale_;
}

float TimeManager::getTimeScaleFactor() const {
    return timeScaleFactor_;
}

std::string TimeManager::getTimeScaleString() const {
    if (isPaused_) {
        return "Paused";
    }
    
    std::string name;
    switch (currentTimeScale_) {
        case TimeScale::VERY_SLOW:
            name = "Very Slow (0.25x)";
            break;
        case TimeScale::SLOW:
            name = "Slow (0.5x)";
            break;
        case TimeScale::NORMAL:
            name = "Normal (1.0x)";
            break;
        case TimeScale::FAST:
            name = "Fast (2.0x)";
            break;
        case TimeScale::VERY_FAST:
            name = "Very Fast (4.0x)";
            break;
        default:
            name = "Custom (" + std::to_string(timeScaleFactor_) + "x)";
    }
    
    return name;
}

void TimeManager::registerTimeScaleChangeCallback(TimeScaleChangeCallback callback) {
    timeScaleCallbacks_.push_back(callback);
}

bool TimeManager::shouldSystemPause(const char* systemName) const {
    if (!isPaused_) {
        return false; // Nothing pauses when game is running
    }
    
    // Check if system is in the always-run list
    std::string sysName(systemName);
    auto it = std::find(alwaysRunSystems_.begin(), alwaysRunSystems_.end(), sysName);
    
    // If found in always-run list, don't pause
    return it == alwaysRunSystems_.end();
}

void TimeManager::applyTimeScaleToSystems() {
    if (!ecsWorld_) {
        return;
    }
    
    // Future implementation: Use Flecs to adjust system time scaling
    // For now, the update() method will handle time scaling
}

float TimeManager::timeScaleToFactor(TimeScale scale) const {
    switch (scale) {
        case TimeScale::PAUSED:    return 0.0f;
        case TimeScale::VERY_SLOW: return 0.25f;
        case TimeScale::SLOW:      return 0.5f;
        case TimeScale::NORMAL:    return 1.0f;
        case TimeScale::FAST:      return 2.0f;
        case TimeScale::VERY_FAST: return 4.0f;
        default:                   return 1.0f;
    }
}

TimeManager::TimeScale TimeManager::factorToTimeScale(float factor) const {
    if (factor <= 0.0f)  return TimeScale::PAUSED;
    if (factor < 0.375f) return TimeScale::VERY_SLOW; // 0.25 + (0.5-0.25)/2
    if (factor < 0.75f)  return TimeScale::SLOW;      // 0.5 + (1.0-0.5)/2
    if (factor < 1.5f)   return TimeScale::NORMAL;    // 1.0 + (2.0-1.0)/2
    if (factor < 3.0f)   return TimeScale::FAST;      // 2.0 + (4.0-2.0)/2
    return TimeScale::VERY_FAST;
}

} // namespace Core
} // namespace VoxelCastle
