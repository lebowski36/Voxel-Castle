#pragma once

#include <SDL3/SDL.h>

namespace VoxelEngine {
namespace Input {

/**
 * @brief Manages mouse cursor visibility and capture modes for different game states
 * 
 * This class provides a centralized way to handle mouse capture behavior similar to Minecraft:
 * - During gameplay: cursor is hidden and mouse is captured for camera control
 * - During menus/UI: cursor is visible and can move freely
 */
class MouseCaptureManager {
public:
    enum class CaptureMode {
        FREE,        // Mouse visible, can move freely (for menus, UI)
        CAPTURED     // Mouse hidden, captured for camera control (for gameplay)
    };

    MouseCaptureManager() = default;
    ~MouseCaptureManager() = default;

    /**
     * @brief Set the mouse capture mode
     * @param mode The capture mode to apply
     * @return true if successful, false if failed
     */
    bool setCaptureMode(CaptureMode mode);

    /**
     * @brief Get current capture mode
     * @return The current capture mode
     */
    CaptureMode getCaptureMode() const { return currentMode_; }

    /**
     * @brief Check if mouse is currently captured
     * @return true if mouse is captured, false if free
     */
    bool isCaptured() const { return currentMode_ == CaptureMode::CAPTURED; }

    /**
     * @brief Initialize the mouse capture manager
     * @param window The SDL window to manage mouse capture for
     * @param initialMode The initial capture mode to set
     * @return true if successful, false if failed
     */
    bool initialize(SDL_Window* window, CaptureMode initialMode = CaptureMode::CAPTURED);

    /**
     * @brief Clean up resources
     */
    void shutdown();

private:
    CaptureMode currentMode_ = CaptureMode::FREE;
    SDL_Window* window_ = nullptr;
    bool initialized_ = false;

    /**
     * @brief Apply the SDL mouse settings for the given mode
     * @param mode The mode to apply
     * @return true if successful, false if failed
     */
    bool applySDLSettings(CaptureMode mode);
};

} // namespace Input
} // namespace VoxelEngine
