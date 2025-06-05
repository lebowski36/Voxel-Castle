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
    }
    
    // Get current global mouse position and mouse button state
    float mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    
    // Get global mouse position (needed for Wayland which may report relative coords)
    float globalMouseX, globalMouseY;
    SDL_GetGlobalMouseState(&globalMouseX, &globalMouseY);
    
    // Calculate window bounds with minimal padding (5px from edge for more aggressive confinement)
    const int PADDING = 5;
    float minX = g_windowPosX + PADDING;
    float maxX = g_windowPosX + g_windowWidth - PADDING;
    float minY = g_windowPosY + PADDING;
    float maxY = g_windowPosY + g_windowHeight - PADDING;
    
    // Check if mouse is near the edge of the window or potentially outside
    bool nearEdge = (mouseX <= minX || mouseX >= maxX || mouseY <= minY || mouseY >= maxY);
    bool potentiallyOutside = (globalMouseX < g_windowPosX || globalMouseX >= g_windowPosX + g_windowWidth ||
                              globalMouseY < g_windowPosY || globalMouseY >= g_windowPosY + g_windowHeight);
    
    if (nearEdge || potentiallyOutside) {
        // Recenter the mouse - use floats for more precision
        float centerX = g_windowPosX + g_windowWidth / 2.0f;
        float centerY = g_windowPosY + g_windowHeight / 2.0f;
        
        // Try to forcibly recapture the mouse with multiple methods
        SDL_Window* window = game.getWindow()->getSDLWindow();
        SDL_SetWindowMouseGrab(window, true);
        
        // Warp mouse to center of window - use int for the API call
        SDL_WarpMouseGlobal(static_cast<int>(centerX), static_cast<int>(centerY));
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
    
    // Software mouse confinement check - check multiple times per frame for more aggressive confinement
    // This is especially important on Wayland where SDL's built-in confinement has known issues
    if (game.isMouseCaptured()) {
        // Check twice per frame to be more aggressive with confinement
        checkAndConfineMouseToWindow(game);
        
        // Second check with a small delay to catch fast mouse movements
        // This adds a backup check even if the mouse has moved since the first check
        SDL_Delay(1); // Tiny delay to catch mouse movements
        checkAndConfineMouseToWindow(game);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // Log ALL events for diagnostic purposes

        // Comment out or modify the general polling log
        // std::cout << "[InputManager] Polled event type: " << e.type << std::endl << std::flush; 

        // Only log significant events to reduce spam - redirect to file
        if (e.type != SDL_EVENT_MOUSE_MOTION) { // Add condition to exclude mouse motion
            if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_KEY_DOWN || 
                e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP ||
                e.type == SDL_EVENT_MOUSE_WHEEL) {
                DEBUG_LOG("InputManager", "Significant event type: " + std::to_string(e.type));
            }
        }
        
        // Debug: Log mouse button events for left/right clicks only
        // Log mouse events at DEBUG level (less verbose than before)
        if ((e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) &&
            (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT)) {
        }
        
        if (e.type == SDL_EVENT_QUIT) {
            CRITICAL_LOG("InputManager", "SDL_EVENT_QUIT received - shutting down");
            game.isRunning_ = false;
        }
        else if (e.type == SDL_EVENT_KEY_DOWN) {
            // Debug - print scancode of all keys - commented out as too verbose
            // Only uncomment for specific debugging sessions when needed
            // std::cout << "[DEBUG] Key pressed, scancode: " << e.key.scancode << std::endl;
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
                    std::cout << "[INFO] Key pressed: ESC" << std::endl;
                    // Check if we're in the main menu with empty stack before toggling
                    if (game.getGameState() != GameState::MAIN_MENU || 
                        (game.getStateManager() && game.getStateManager()->hasStateInStack())) {
                        // Only toggle menu if we're not in main menu with empty stack
                        game.toggleMenu();
                    } else {
                        std::cout << "[INFO] ESC key ignored in main menu with no previous state" << std::endl;
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
                
                // Testing keys
                case SDL_SCANCODE_T: // Test SaveManager (T key is always replaced with the current test)
                    std::cout << "[Input] Running SaveManager test..." << std::endl;
                    game.runSaveManagerTest();
                    break;
                
                // Save/Load keys
                case SDL_SCANCODE_F5: // Quick Save
                    std::cout << "[Save] Quick save triggered (F5)..." << std::endl;
                    game.quickSave();
                    break;
                    
                case SDL_SCANCODE_F9: // Quick Load
                    std::cout << "[Load] Quick load triggered (F9)..." << std::endl;
                    game.quickLoad();
                    break;
                
                case SDL_SCANCODE_F11: // Fullscreen Toggle
                    std::cout << "[Input] Fullscreen toggle triggered (F11)..." << std::endl;
                    game.toggleFullscreen();
                    break;
                
                // Time control keys
                case SDL_SCANCODE_PAUSE: // Add pause key
                case SDL_SCANCODE_P: // Alternative pause key (easier to use than PAUSE)
                case SDL_SCANCODE_BACKSPACE: // Alternative pause key
                    if (game.canAcceptInput()) {
                        bool isPaused = game.togglePauseSimulation();
                        std::cout << "[Time] Simulation " << (isPaused ? "paused" : "resumed") << std::endl;
                    }
                    break;
                    
                case SDL_SCANCODE_EQUALS: // + key for speed up
                case SDL_SCANCODE_KP_PLUS:
                case SDL_SCANCODE_RIGHTBRACKET: // Additional key for '+'
                    std::cout << "[DEBUG] Time speed-up key detected: " << e.key.scancode << std::endl;
                    if (game.canAcceptInput()) { // Allow in any playing state
                        game.increaseTimeScale();
                        std::cout << "[Time] Speed increased: " << game.getTimeScaleString() << std::endl;
                    }
                    break;
                    
                case SDL_SCANCODE_MINUS: // - key for slow down
                case SDL_SCANCODE_KP_MINUS:
                case SDL_SCANCODE_SLASH: // Additional key for '-'
                    std::cout << "[DEBUG] Time slow-down key detected: " << e.key.scancode << std::endl;
                    if (game.canAcceptInput()) { // Allow in any playing state
                        game.decreaseTimeScale();
                        std::cout << "[Time] Speed decreased: " << game.getTimeScaleString() << std::endl;
                    }
                    break;
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
            // Store mouse movement for camera
            game.setMouseDeltaX(game.getMouseDeltaX() + static_cast<float>(e.motion.xrel));
            game.setMouseDeltaY(game.getMouseDeltaY() + static_cast<float>(-e.motion.yrel));
            
            // Add additional confinement check on mouse movement for more aggressive containment
            checkAndConfineMouseToWindow(game);
            
            // With large/fast movements, also monitor and log the absolute position occasionally
            // This helps us catch if the mouse leaves the window despite relative mode
            // Commented out as too verbose - only uncomment for debugging mouse containment issues
            /*
            static int motionCounter = 0;
            if (++motionCounter % 10 == 0) { // Every 10th motion event
                float absX, absY;
                SDL_GetMouseState(&absX, &absY);
                float globalX, globalY;
                SDL_GetGlobalMouseState(&globalX, &globalY);
            }
            */
        }
        // Handle mouse button events when mouse is NOT captured (likely for UI)
        else if ((e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) && !game.isMouseCaptured()) {
            bool clicked = (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            // Pass UI input to the game instance to handle
            // We only care about left clicks for UI
            if (e.button.button == SDL_BUTTON_LEFT) {
                 game.handleMenuInput(static_cast<float>(e.button.x), static_cast<float>(e.button.y), clicked);
            }
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && game.isMouseCaptured()) {
            // Check if world is ready for block operations before processing mouse clicks
            if (!game.isWorldReadyForBlockOperations()) {
                WARN_LOG("InputManager", "World not ready for block operations - ignoring mouse click");
                continue; // Skip this event but continue processing other events
            }
            
            if (e.button.button == SDL_BUTTON_LEFT) {
                game.leftMousePressed_ = true;
                game.pendingBlockAction_ = true;  // Request block placement on next frame
                game.isBlockPlacement_ = true;    // It's a placement (not removal)
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                game.rightMousePressed_ = true;
                game.pendingBlockAction_ = true;  // Request block removal on next frame
                game.isBlockPlacement_ = false;   // It's a removal (not placement)
            }
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
            int width = e.window.data1;
            int height = e.window.data2;
            
            // Update camera aspect ratio
            if (game.camera_) {
                game.camera_->updateAspect(static_cast<float>(width) / static_cast<float>(height));
            }
            
            // Update viewport
            glViewport(0, 0, width, height);
            
            // Update HUD position for new screen size
            if (game.hudSystem_) {
                game.hudSystem_->centerBottomOfScreen(width, height, 50); // 50px margin
                DEBUG_LOG("InputManager", "HUD repositioned for window resize: " + 
                          std::to_string(width) + "x" + std::to_string(height));
            }
            
            // Update crosshair position for new screen size
            if (game.crosshairSystem_) {
                game.crosshairSystem_->centerOnScreen(width, height);
                DEBUG_LOG("InputManager", "Crosshair repositioned for window resize: " + 
                          std::to_string(width) + "x" + std::to_string(height));
            }
        }
        else if (e.type == SDL_EVENT_WINDOW_FOCUS_GAINED) {
            // Automatically enable mouse capture when window gains focus (unless menu is open)
            if (game.isPlaying()) {
                game.setMouseCaptured(true);
                DEBUG_LOG("InputManager", "Window gained focus - mouse capture enabled");
            }
        }
        else if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            // Handle window close button (X) click
            CRITICAL_LOG("InputManager", "Window close requested - shutting down");
            game.isRunning_ = false;
        }
    }
}

} // namespace GameInput
