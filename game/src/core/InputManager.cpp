#include "core/InputManager.h"
#include "core/game.h"
#include "core/CameraMode.h"
#include "platform/Window.h" // Corrected path to Window.h
#include "world/world_manager.h" // Required for markAllSegmentsDirty
#include "interaction/BlockPlacement.h" // For block placement handling
#include "utils/debug_logger.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>

// Define the global debug render mode variable
DebugRenderMode g_debugRenderMode = DebugRenderMode::NORMAL;

// Global variables to track window information for mouse confinement
static int g_windowPosX = 0;
static int g_windowPosY = 0;
static int g_windowWidth = 0;
static int g_windowHeight = 0;
static bool g_windowInfoValid = false;

// Function to check if mouse is close to window edge and recenter if needed
bool checkAndConfineMouseToWindow(Game& game) {
    if (!game.isMouseCaptured() || !game.getWindow() || !game.getWindow()->getSDLWindow()) {
        return false;
    }

    // Update window position and size if needed
    if (!g_windowInfoValid) {
        SDL_GetWindowPosition(game.getWindow()->getSDLWindow(), &g_windowPosX, &g_windowPosY);
        SDL_GetWindowSize(game.getWindow()->getSDLWindow(), &g_windowWidth, &g_windowHeight);
        g_windowInfoValid = true;
        
        DEBUG_LOG("InputManager", "Updated window info: pos=(" + std::to_string(g_windowPosX) + 
                "," + std::to_string(g_windowPosY) + "), size=" + 
                std::to_string(g_windowWidth) + "x" + std::to_string(g_windowHeight));
    }
    
    // Get current global mouse position
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Calculate window bounds with padding (20px from edge)
    const int PADDING = 20;
    float minX = g_windowPosX + PADDING;
    float maxX = g_windowPosX + g_windowWidth - PADDING;
    float minY = g_windowPosY + PADDING;
    float maxY = g_windowPosY + g_windowHeight - PADDING;
    
    // Check if mouse is near the edge of the window
    bool nearEdge = (mouseX <= minX || mouseX >= maxX || mouseY <= minY || mouseY >= maxY);
    
    if (nearEdge) {
        // Recenter the mouse
        int centerX = g_windowPosX + g_windowWidth / 2;
        int centerY = g_windowPosY + g_windowHeight / 2;
        
        DEBUG_LOG("InputManager", "Mouse near edge: (" + std::to_string(mouseX) + "," + 
                std::to_string(mouseY) + "), recentering to (" + 
                std::to_string(centerX) + "," + std::to_string(centerY) + ")");
        
        // Warp mouse to center of window
        SDL_WarpMouseGlobal(centerX, centerY);
        return true;
    }
    
    return false;
}

namespace GameInput {

void processInput(Game& game) {
    // Debug render mode toggle (P key)
    // Reset mouse delta at frame start
    game.setMouseDeltaX(0.0f);
    game.setMouseDeltaY(0.0f);
    
    // Software mouse confinement check
    if (game.isMouseCaptured()) {
        checkAndConfineMouseToWindow(game);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // Log ALL events for diagnostic purposes
        DEBUG_LOG("InputManager", "SDL Event: type=" + std::to_string(e.type) + 
                 " (SDL_EVENT_QUIT=" + std::to_string(SDL_EVENT_QUIT) + 
                 ", SDL_EVENT_MOUSE_BUTTON_DOWN=" + std::to_string(SDL_EVENT_MOUSE_BUTTON_DOWN) + ")");
        
        // Comment out or modify the general polling log
        // std::cout << "[InputManager] Polled event type: " << e.type << std::endl << std::flush; 

        // Only log significant events to reduce spam
        if (e.type != SDL_EVENT_MOUSE_MOTION) { // Add condition to exclude mouse motion
            if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_KEY_DOWN || 
                e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP ||
                e.type == SDL_EVENT_MOUSE_WHEEL) {
                std::cout << "[InputManager] Significant event type: " << e.type << std::endl << std::flush;
            } else {
                 // Optionally, log other non-mouse-motion events if desired, or keep this silent
                 // std::cout << "[InputManager] Other event type: " << e.type << std::endl << std::flush;
            }
        }
        
        // Debug: Log mouse button events for left/right clicks only
        // Log mouse events at DEBUG level (less verbose than before)
        if ((e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) &&
            (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT)) {
            DEBUG_LOG("InputManager", "Mouse " + std::string(e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? "DOWN" : "UP") + 
                     ", button=" + std::string(e.button.button == SDL_BUTTON_LEFT ? "LEFT" : "RIGHT") +
                     ", mouseCaptured=" + std::to_string(game.isMouseCaptured()));
        }
        
        if (e.type == SDL_EVENT_QUIT) {
            CRITICAL_LOG("InputManager", "SDL_EVENT_QUIT received - shutting down");
            game.isRunning_ = false;
        }
        else if (e.type == SDL_EVENT_KEY_DOWN) {
            if (e.key.scancode == SDL_SCANCODE_P) {
                DebugRenderMode oldMode = g_debugRenderMode;
                g_debugRenderMode = static_cast<DebugRenderMode>((static_cast<int>(g_debugRenderMode) + 1) % 2);
                bool modeChanged = oldMode != g_debugRenderMode;

                switch (g_debugRenderMode) {
                    case DebugRenderMode::NORMAL:
                        std::cout << "[Debug] Render mode: NORMAL" << std::endl << std::flush; // Added flush
                        break;
                    case DebugRenderMode::WIREFRAME:
                        std::cout << "[Debug] Render mode: WIREFRAME" << std::endl << std::flush; // Added flush
                        break;
                }

                if (modeChanged && game.getWorldManager()) {
                    // If mode changed to or from WIREFRAME, mark all segments dirty
                    if (oldMode == DebugRenderMode::WIREFRAME || g_debugRenderMode == DebugRenderMode::WIREFRAME) {
                        game.getWorldManager()->markAllSegmentsDirty();
                        std::cout << "[InputManager] Debug mode changed, marked all segments dirty." << std::endl << std::flush; // Added flush
                    }
                }
            }
            switch (e.key.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    // Toggle mouse capture state using the Game class methods
                    game.setMouseCaptured(!game.isMouseCaptured());
                    if (game.getWindow() && game.getWindow()->getSDLWindow()) {
                        SDL_Window* window = game.getWindow()->getSDLWindow();
                        
                        // Force update of window info on next frame
                        g_windowInfoValid = false;
                        
                        if (game.isMouseCaptured()) {
                            // Enable mouse capture: hide cursor, relative mode, and confine to window
                            SDL_SetWindowRelativeMouseMode(window, true);
                            SDL_SetWindowMouseGrab(window, true);
                            
                            // Try all available confinement methods (some may not work on Wayland)
                            SDL_SetWindowMouseRect(window, nullptr); // Confine to entire window
                            
                            // Get window position and size for software confinement
                            int winX, winY, winW, winH;
                            SDL_GetWindowPosition(window, &winX, &winY);
                            SDL_GetWindowSize(window, &winW, &winH);
                            
                            // Center the mouse in the window as starting point
                            int centerX = winX + winW / 2;
                            int centerY = winY + winH / 2;
                            SDL_WarpMouseGlobal(centerX, centerY);
                            
                            INFO_LOG("InputManager", "Mouse capture ENABLED - software confinement active, centered at (" + 
                                    std::to_string(centerX) + ", " + std::to_string(centerY) + "), window bounds: " +
                                    std::to_string(winX) + "," + std::to_string(winY) + " " + 
                                    std::to_string(winW) + "x" + std::to_string(winH));
                        } else {
                            // Disable mouse capture: show cursor, absolute mode, release grab
                            SDL_SetWindowRelativeMouseMode(window, false);
                            SDL_SetWindowMouseGrab(window, false);
                            SDL_SetWindowMouseRect(window, nullptr); // Remove confinement
                            
                            INFO_LOG("InputManager", "Mouse capture DISABLED - cursor visible, absolute mode, grab released");
                        }
                        
                        std::cout << "[InputManager] Mouse capture toggled: " << game.isMouseCaptured() << std::endl << std::flush; // Added flush
                    }
                    break;
                case SDL_SCANCODE_O:
                    game.toggleCameraMode();
                    break;
                case SDL_SCANCODE_W: game.forward_ = true; break;
                case SDL_SCANCODE_S: game.backward_ = true; break;
                case SDL_SCANCODE_A: game.left_ = true; break;
                case SDL_SCANCODE_D: game.right_ = true; break;
                case SDL_SCANCODE_Q: game.down_ = true; break;
                case SDL_SCANCODE_E: game.up_ = true; break;
                case SDL_SCANCODE_LSHIFT: 
                    if (game.getCameraMode() == CameraMode::FREE_FLYING) {
                        game.speedMultiplier_ = 3.0f; 
                    } else {
                        game.sprinting_ = true;
                    }
                    break;
                case SDL_SCANCODE_SPACE: 
                    if (game.getCameraMode() == CameraMode::FREE_FLYING) {
                        game.up_ = true; 
                    } else {
                        game.jumping_ = true;
                    }
                    break;
                case SDL_SCANCODE_LCTRL: 
                    if (game.getCameraMode() == CameraMode::FREE_FLYING) {
                        game.down_ = true; 
                    } else {
                        game.crouching_ = true;
                    }
                    break;
                case SDL_SCANCODE_M: game.manualVoxelChangeRequested_ = true; break;
                default: break;
            }
        }
        else if (e.type == SDL_EVENT_KEY_UP) {
            switch (e.key.scancode) {
                case SDL_SCANCODE_W: game.forward_ = false; break;
                case SDL_SCANCODE_S: game.backward_ = false; break;
                case SDL_SCANCODE_A: game.left_ = false; break;
                case SDL_SCANCODE_D: game.right_ = false; break;
                case SDL_SCANCODE_Q: game.down_ = false; break;
                case SDL_SCANCODE_E: game.up_ = false; break;
                case SDL_SCANCODE_LSHIFT: 
                    if (game.getCameraMode() == CameraMode::FREE_FLYING) {
                        game.speedMultiplier_ = 1.0f; 
                    } else {
                        game.sprinting_ = false;
                    }
                    break;
                case SDL_SCANCODE_SPACE: 
                    if (game.getCameraMode() == CameraMode::FREE_FLYING) {
                        game.up_ = false; 
                    } else {
                        game.jumping_ = false;
                    }
                    break;
                case SDL_SCANCODE_LCTRL: 
                    if (game.getCameraMode() == CameraMode::FREE_FLYING) {
                        game.down_ = false; 
                    } else {
                        game.crouching_ = false;
                    }
                    break;
                default: break;
            }
        }
        else if (e.type == SDL_EVENT_MOUSE_MOTION && game.isMouseCaptured()) {
            game.setMouseDeltaX(game.getMouseDeltaX() + static_cast<float>(e.motion.xrel));
            game.setMouseDeltaY(game.getMouseDeltaY() + static_cast<float>(-e.motion.yrel));
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && game.isMouseCaptured()) {
            DEBUG_LOG("InputManager", "Mouse button DOWN detected - button=" + std::to_string(e.button.button) + 
                     ", click_x=" + std::to_string(e.button.x) + ", click_y=" + std::to_string(e.button.y));
            
            // Check if world is ready for block operations before processing mouse clicks
            if (!game.isWorldReadyForBlockOperations()) {
                WARN_LOG("InputManager", "World not ready for block operations - ignoring mouse click");
                continue; // Skip this event but continue processing other events
            }
            
            if (e.button.button == SDL_BUTTON_LEFT) {
                INFO_LOG("InputManager", "LEFT mouse button - Setting pendingBlockAction=true, isBlockPlacement=true");
                game.leftMousePressed_ = true;
                game.pendingBlockAction_ = true;  // Request block placement on next frame
                game.isBlockPlacement_ = true;    // It's a placement (not removal)
                DEBUG_LOG("InputManager", "LEFT click processed successfully");
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                INFO_LOG("InputManager", "RIGHT mouse button - Setting pendingBlockAction=true, isBlockPlacement=false");
                game.rightMousePressed_ = true;
                game.pendingBlockAction_ = true;  // Request block removal on next frame
                game.isBlockPlacement_ = false;   // It's a removal (not placement)
                DEBUG_LOG("InputManager", "RIGHT click processed successfully");
            }
            DEBUG_LOG("InputManager", "After setting flags: pendingAction=" + 
                     std::to_string(game.hasPendingBlockAction()) + ", isPlacement=" + std::to_string(game.isBlockPlacement()));
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && game.isMouseCaptured()) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                game.leftMousePressed_ = false;
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                game.rightMousePressed_ = false;
            }
        }
        else if (e.type == SDL_EVENT_MOUSE_WHEEL && game.isMouseCaptured()) {
            // Mouse wheel cycles through block types
            bool forward = e.wheel.y > 0;
            BlockPlacement::cycleBlockType(game, forward);
        }
        else if (e.type == SDL_EVENT_WINDOW_RESIZED) {
            if (game.camera_) {
                game.camera_->updateAspect(static_cast<float>(e.window.data1) / static_cast<float>(e.window.data2));
            }
            glViewport(0, 0, e.window.data1, e.window.data2);
        }
    }
}

} // namespace GameInput
