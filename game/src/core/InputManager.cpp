#include "core/InputManager.h"
#include "core/game.h"
#include "core/CameraMode.h"
#include "platform/Window.h" // Corrected path to Window.h
#include "world/world_manager.h" // Required for markAllSegmentsDirty
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>

// Define the global debug render mode variable
DebugRenderMode g_debugRenderMode = DebugRenderMode::NORMAL;

namespace GameInput {

void processInput(Game& game) {
    // Debug render mode toggle (P key)
    // Reset mouse delta at frame start
    game.mouseDeltaX_ = 0.0f;
    game.mouseDeltaY_ = 0.0f;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            game.isRunning_ = false;
        }
        else if (e.type == SDL_EVENT_KEY_DOWN) {
            if (e.key.scancode == SDL_SCANCODE_P) {
                DebugRenderMode oldMode = g_debugRenderMode;
                g_debugRenderMode = static_cast<DebugRenderMode>((static_cast<int>(g_debugRenderMode) + 1) % 2);
                bool modeChanged = oldMode != g_debugRenderMode;

                switch (g_debugRenderMode) {
                    case DebugRenderMode::NORMAL:
                        std::cout << "[Debug] Render mode: NORMAL" << std::endl;
                        break;
                    case DebugRenderMode::WIREFRAME:
                        std::cout << "[Debug] Render mode: WIREFRAME" << std::endl;
                        break;
                }

                if (modeChanged && game.getWorldManager()) {
                    // If mode changed to or from WIREFRAME, mark all segments dirty
                    if (oldMode == DebugRenderMode::WIREFRAME || g_debugRenderMode == DebugRenderMode::WIREFRAME) {
                        game.getWorldManager()->markAllSegmentsDirty();
                        std::cout << "[InputManager] Debug mode changed, marked all segments dirty." << std::endl;
                    }
                }
            }
            switch (e.key.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    game.mouseCaptured_ = !game.mouseCaptured_;
                    if (game.gameWindow_ && game.gameWindow_->getSDLWindow()) {
                        SDL_SetWindowRelativeMouseMode(game.gameWindow_->getSDLWindow(), game.mouseCaptured_);
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
        else if (e.type == SDL_EVENT_MOUSE_MOTION && game.mouseCaptured_) {
            game.mouseDeltaX_ += static_cast<float>(e.motion.xrel);
            game.mouseDeltaY_ += static_cast<float>(-e.motion.yrel);
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
