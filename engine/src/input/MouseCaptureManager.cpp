#include "input/MouseCaptureManager.h"
#include <iostream>

namespace VoxelEngine {
namespace Input {

bool MouseCaptureManager::initialize(SDL_Window* window, CaptureMode initialMode) {
    if (initialized_) {
        std::cout << "[MouseCaptureManager] Already initialized" << std::endl;
        return true;
    }

    if (!window) {
        std::cout << "[MouseCaptureManager] Invalid window parameter" << std::endl;
        return false;
    }

    window_ = window;
    initialized_ = true;
    
    // Set the initial mode
    if (!setCaptureMode(initialMode)) {
        std::cout << "[MouseCaptureManager] Failed to set initial capture mode" << std::endl;
        initialized_ = false;
        window_ = nullptr;
        return false;
    }

    std::cout << "[MouseCaptureManager] Initialized successfully with mode: " 
              << (initialMode == CaptureMode::CAPTURED ? "CAPTURED" : "FREE") << std::endl;
    return true;
}

void MouseCaptureManager::shutdown() {
    if (initialized_) {
        // Release mouse capture before shutdown
        setCaptureMode(CaptureMode::FREE);
        initialized_ = false;
        window_ = nullptr;
        std::cout << "[MouseCaptureManager] Shutdown complete" << std::endl;
    }
}

bool MouseCaptureManager::setCaptureMode(CaptureMode mode) {
    if (!initialized_ || !window_) {
        std::cout << "[MouseCaptureManager] Not initialized, cannot set capture mode" << std::endl;
        return false;
    }

    if (currentMode_ == mode) {
        // Already in the desired mode
        return true;
    }

    if (applySDLSettings(mode)) {
        currentMode_ = mode;
        std::cout << "[MouseCaptureManager] Capture mode changed to: " 
                  << (mode == CaptureMode::CAPTURED ? "CAPTURED" : "FREE") << std::endl;
        return true;
    }

    std::cout << "[MouseCaptureManager] Failed to apply SDL settings for mode: " 
              << (mode == CaptureMode::CAPTURED ? "CAPTURED" : "FREE") << std::endl;
    return false;
}

bool MouseCaptureManager::applySDLSettings(CaptureMode mode) {
    switch (mode) {
        case CaptureMode::CAPTURED:
            // Hide cursor and enable relative mouse mode for camera control
            if (!SDL_HideCursor()) {
                std::cout << "[MouseCaptureManager] Failed to hide cursor: " << SDL_GetError() << std::endl;
                return false;
            }
            
            if (!SDL_SetWindowRelativeMouseMode(window_, true)) {
                std::cout << "[MouseCaptureManager] Failed to enable relative mouse mode: " << SDL_GetError() << std::endl;
                // Try to restore cursor visibility
                SDL_ShowCursor();
                return false;
            }
            break;

        case CaptureMode::FREE:
            // Show cursor and disable relative mouse mode for UI interaction
            if (!SDL_SetWindowRelativeMouseMode(window_, false)) {
                std::cout << "[MouseCaptureManager] Failed to disable relative mouse mode: " << SDL_GetError() << std::endl;
                return false;
            }
            
            if (!SDL_ShowCursor()) {
                std::cout << "[MouseCaptureManager] Failed to show cursor: " << SDL_GetError() << std::endl;
                return false;
            }
            break;

        default:
            std::cout << "[MouseCaptureManager] Unknown capture mode" << std::endl;
            return false;
    }

    return true;
}

} // namespace Input
} // namespace VoxelEngine
