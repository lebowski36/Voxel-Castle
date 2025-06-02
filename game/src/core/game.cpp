#include <cmath> // For std::floor
#include "../../include/core/game.h"       // Defines Game, forward declares SpectatorCamera
#include "../../include/SpectatorCamera.h" // Full definition of SpectatorCamera
#include "../../include/core/GameInitializer.h"    // Initialization and shutdown helper
#include "core/WorldSetup.h" // Added for world setup
#include "core/WorldSetupGlobals.h"
#include "core/GameRenderer.h" // Added for rendering logic
#include "core/GameLoop.h" // Game loop management
#include "core/GameRenderCoordinator.h" // Game render coordination
#include "utils/debug_logger.h" // For DEBUG_LOG

// UI System includes
#include "ui/UISystem.h"
#include "ui/MenuSystem.h"
#include "ui/elements/HUD.h"
#include "ui/elements/UIPanel.h"

// Include headers that will be needed for the actual implementations later
#include <iostream> 
#include <vector> // For potential use in migrated logic
#include <string> // For potential use in migrated logic
#include <fstream> // For potential use in migrated logic
#include <sstream> // For potential use in migrated logic
#include <chrono> // For std::chrono for timing
#include <set> // Required for std::set
#include <algorithm> // For std::min/max (potentially useful, good to have)
#include <iomanip>

#include "core/InputManager.h"  // Input handling module
// SDL for events, window relative mouse mode
#include <SDL3/SDL.h>

// GLAD for OpenGL functions
#include <glad/glad.h>

// GLM for math types (will be used by camera, etc.)
#define GLM_FORCE_SILENT_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL
// #include <glm/glm.hpp> // Already included via game.h or other headers like SpectatorCamera.h
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Mouse capture manager
#include "input/MouseCaptureManager.h"

// Forward declared class implementations (will be needed when unique_ptrs are initialized)
#include "platform/Window.h" 
#include "flecs.h" 
#include "world/world_manager.h"
#include "world/world_generator.h" 
#include "rendering/texture_atlas.h"
#include "rendering/mesh_builder.h"
#include "rendering/mesh_renderer.h"

// ECS Components & Systems (if directly used or for registration)
#include "ecs/components/position_component.h"
#include "ecs/components/velocity_component.h"
#include "ecs/components/renderable_component.h"
#include "ecs/systems/movement_system.h"

// Debug utilities (will be needed for setup/cleanup)
#include "rendering/debug_utils.h"
#include "rendering/render_utils.h"

// Timestamp helper for game
static std::string getTimestampGame() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

Game::Game() 
    : gameWindow_(nullptr),
      ecs_(nullptr),
      worldManager_(nullptr),
      worldGenerator_(nullptr),
      textureAtlas_(nullptr),
      meshBuilder_(nullptr),
      meshRenderer_(nullptr),
      camera_(nullptr),
      menuSystem_(nullptr),
      hudSystem_(nullptr),
      mouseCaptureManager_(nullptr),
      gameLoop_(std::make_unique<GameLoop>()),
      renderCoordinator_(std::make_unique<VoxelCastle::Core::GameRenderCoordinator>()),
      isRunning_(false),
      lastFrameTime_(),
      mouseCaptured_(true),
      speedMultiplier_(1.0f),
      forward_(false), backward_(false), left_(false), right_(false), up_(false), down_(false),
      manualVoxelChangeRequested_(false),
      mouseDeltaX_(0.0f),
      mouseDeltaY_(0.0f)
      // screenWidth_ and screenHeight_ are const and initialized in header
      // projectRoot_ is const and initialized in header
{
    // Constructor body can be empty if all initialization is done in the initializer list
    // or will be done in initialize()
}

Game::~Game() {
    // Destructor: Ensure shutdown is called if it hasn't been explicitly.
    // This is a safeguard. Proper resource management should ensure shutdown() is called before destruction.
    if (isRunning_) { // A simple check; could be more robust, e.g., checking if pointers are non-null
        std::cerr << "Warning: Game destructor called while still considered running. Forcing shutdown." << std::endl;
        shutdown(); 
    }
}

bool Game::initialize() {
    // Delegate to GameInitializer
    auto result = GameInitializer::initialize(screenWidth_, screenHeight_, projectRoot_.c_str());
    gameWindow_ = std::move(result.gameWindow);
    ecs_ = std::move(result.ecs);
    worldManager_ = std::move(result.worldManager);
    worldGenerator_ = std::move(result.worldGenerator);
    textureAtlas_ = std::move(result.textureAtlas);
    meshBuilder_ = std::move(result.meshBuilder);
    meshRenderer_ = std::move(result.meshRenderer);
    camera_ = std::move(result.camera);
    lastFrameTime_ = result.lastFrameTime;
    isRunning_ = result.isRunning;

    // Initialize mouse capture manager
    mouseCaptureManager_ = std::make_unique<VoxelEngine::Input::MouseCaptureManager>();
    SDL_Window* sdlWindow = gameWindow_ ? gameWindow_->getSDLWindow() : nullptr;
    if (!sdlWindow) {
        std::cout << "[Game] Warning: No valid SDL window for mouse capture manager" << std::endl;
    } else if (!mouseCaptureManager_->initialize(sdlWindow, VoxelEngine::Input::MouseCaptureManager::CaptureMode::CAPTURED)) {
        std::cout << "[Game] Warning: Failed to initialize mouse capture manager" << std::endl;
    } else {
        std::cout << "[Game] Mouse capture manager initialized - cursor should now be hidden" << std::endl;
    }

    // Set the global initial camera position for world setup
    if (camera_) {
        g_initialCameraPosition = camera_->getPosition();
    } else {
        g_initialCameraPosition = glm::vec3(0.0f);
    }

    // Initialize world timer
    worldInitTime_ = std::chrono::steady_clock::now();
    isWorldFullyLoaded_ = false; // Will be set to true after initial chunk loading

    // World content initialization delegated to WorldSetup module

    // [Dynamic Chunk Loading] Static world initialization removed. Chunks will be loaded dynamically as the camera moves.

    // Initial mesh build after world content is set up
    if (isRunning_ && worldManager_ && textureAtlas_ && meshBuilder_) {
        worldManager_->updateDirtyMeshes(*textureAtlas_, *meshBuilder_);
        
        // Mark world as ready after initial mesh building - but add a delay for safety
        // We'll actually set this flag in the game loop after some time has passed
    }
    
    // Initialize UI system
    if (isRunning_ && meshRenderer_ && textureAtlas_) {
        menuSystem_ = std::make_unique<VoxelEngine::UI::MenuSystem>();
        if (!menuSystem_->initialize(screenWidth_, screenHeight_, projectRoot_)) {
            std::cerr << "Failed to initialize UI system" << std::endl;
            isRunning_ = false;
            return false;
        }
        
        // Set up menu closed callback to ensure proper game state restoration
        menuSystem_->setOnMenuClosed([this]() {
            // Only restore game state if we're still in menu state
            // This prevents issues if ESC was used to close menu instead of button
            if (gameState_ == GameState::MENU) {
                gameState_ = GameState::PLAYING;
                setMouseCaptured(true);
                
                // Ensure game UI is visible again
                if (hudSystem_) {
                    hudSystem_->setVisible(true);
                }
                
                std::cout << "[Game] Menu closed via callback - game resumed, cursor hidden" << std::endl;
            }
        });
        
        // Set up fullscreen toggle callback
        menuSystem_->setOnFullscreenToggled([this](bool enable) -> bool {
            // If enable is true, we want to enable fullscreen
            // If enable is false, we want to disable fullscreen
            // Our toggleFullscreen() just toggles the current state, so we need to check
            // if the current state already matches what we want
            if (isFullscreen() != enable) {
                return toggleFullscreen();
            }
            return true; // Already in desired state
        });
        
        // Set up exit request callback
        menuSystem_->setOnExitRequest([this]() {
            std::cout << "[Game] Exit requested from menu" << std::endl;
            requestExit();
        });
        
        // Get the texture atlas OpenGL texture ID from mesh renderer
        GLuint atlasTextureId = meshRenderer_->getTextureAtlasID();
        
        // Create HUD element with proper constructor parameters
        hudSystem_ = std::make_shared<VoxelEngine::UI::HUD>(
            &menuSystem_->getRenderer(), 
            textureAtlas_.get(), 
            atlasTextureId
        );
        
        // Position it at bottom center of screen
        float uiSize = 120.0f; // Reasonable size for UI element
        float centerX = (screenWidth_ - uiSize) / 2.0f;
        float bottomY = screenHeight_ - uiSize - 50.0f; // 50px margin from bottom
        
        DEBUG_LOG("Game", "Positioning HUD at bottom center (" + std::to_string(static_cast<int>(centerX)) + 
                  ", " + std::to_string(static_cast<int>(bottomY)) + ") with screen size " + 
                  std::to_string(screenWidth_) + "x" + std::to_string(screenHeight_));
        
        hudSystem_->setPosition(centerX, bottomY);
        hudSystem_->setSize(uiSize, uiSize);
        hudSystem_->setVisible(true); // Ensure it's visible by default in gameplay mode
        
        // Debug: Let's verify the position was set correctly
        DEBUG_LOG("Game", "Actual HUD position: (" + 
                  std::to_string(static_cast<int>(hudSystem_->getPosition().x)) + ", " + 
                  std::to_string(static_cast<int>(hudSystem_->getPosition().y)) + ")");
        
        // Add to UI system
        menuSystem_->addElement(hudSystem_);
        
        std::cout << "[Game] UI system initialized successfully with HUD" << std::endl;
    }
    
    // Add clean startup message
    std::cout << "\n=== Voxel Fortress Game ===" << std::endl;
    std::cout << "[INFO] Game ready - Press ESC for menu" << std::endl;
    
    return isRunning_;
}

void Game::run() {
    if (!isRunning_) {
        std::cerr << "Game::run() called but game is not initialized or already shut down." << std::endl;
        return;
    }

    if (!gameLoop_) {
        std::cerr << "Game::run() called but GameLoop is not initialized." << std::endl;
        return;
    }

    // Delegate to GameLoop
    int exitCode = gameLoop_->run(*this);
    
    // Handle exit code if needed
    if (exitCode != 0) {
        std::cerr << "GameLoop returned non-zero exit code: " << exitCode << std::endl;
    }
}

void Game::shutdown() {
    // Clean up mouse capture manager
    if (mouseCaptureManager_) {
        mouseCaptureManager_->shutdown();
        mouseCaptureManager_.reset();
    }
    
    // Clean up UI system first
    if (hudSystem_) {
        hudSystem_.reset();
    }
    if (menuSystem_) {
        menuSystem_->shutdown();
        menuSystem_.reset();
    }
    
    GameInitializer::InitResult resources;
    resources.gameWindow = std::move(gameWindow_);
    resources.ecs = std::move(ecs_);
    resources.worldManager = std::move(worldManager_);
    resources.worldGenerator = std::move(worldGenerator_);
    resources.textureAtlas = std::move(textureAtlas_);
    resources.meshBuilder = std::move(meshBuilder_);
    resources.meshRenderer = std::move(meshRenderer_);
    resources.camera = std::move(camera_);
    resources.isRunning = isRunning_;
    GameInitializer::shutdown(resources, screenWidth_, screenHeight_, projectRoot_.c_str());
    isRunning_ = false;
    
    std::cout << "[INFO] Game session ended - goodbye!" << std::endl;
}

void Game::processInput() {
    // Delegate to InputManager
    GameInput::processInput(*this);
}

void Game::toggleCameraMode() {
    if (cameraMode_ == CameraMode::FREE_FLYING) {
        // Switch to first-person mode
        cameraMode_ = CameraMode::FIRST_PERSON;
        // Initialize player position from current camera position
        if (camera_) {
            playerPosition_ = camera_->getPosition();
            playerVelocity_ = glm::vec3(0.0f);
            isOnGround_ = false;
            fallVelocity_ = 0.0f;
        }
        std::cout << "[Game] Switched to FIRST_PERSON camera mode" << std::endl;
    } else {
        // Switch to free-flying mode
        cameraMode_ = CameraMode::FREE_FLYING;
        // Sync camera position with player position
        if (camera_) {
            camera_->setPosition(playerPosition_);
        }
        std::cout << "[Game] Switched to FREE_FLYING camera mode" << std::endl;
    }
}

// --- UI Input Handling ---
void Game::handleMenuInput(float mouseX, float mouseY, bool clicked) {
    if (menuSystem_ && isMenuOpen()) {
        // Pass input to the menu system
        bool inputHandled = menuSystem_->handleInput(mouseX, mouseY, clicked);
        
        // Only log significant menu interactions (clicks)
        if (clicked) {
            DEBUG_LOG("Game", "Menu interaction at (" + std::to_string(static_cast<int>(mouseX)) + 
                     ", " + std::to_string(static_cast<int>(mouseY)) + ") - " + 
                     (inputHandled ? "handled" : "not handled"));
        }
    }
}

// --- Game State Management ---
void Game::toggleMenu() {
    if (gameState_ == GameState::PLAYING) {
        gameState_ = GameState::MENU;
        setMouseCaptured(false); // Show cursor for menu navigation
        
        // Hide game UI elements when menu is open
        if (hudSystem_) {
            hudSystem_->setVisible(false);
        }
        
        // Show menu
        if (menuSystem_) {
            menuSystem_->showMainMenu();
        }
        
        std::cout << "[Game] Menu opened - game paused, cursor visible" << std::endl;
    } else {
        gameState_ = GameState::PLAYING;
        setMouseCaptured(true); // Hide cursor for gameplay
        
        // Show game UI elements when menu is closed
        if (hudSystem_) {
            hudSystem_->setVisible(true);
        }
        
        // Hide menu
        if (menuSystem_) {
            menuSystem_->closeMenus();
        }
        
        std::cout << "[Game] Menu closed - game resumed, cursor hidden" << std::endl;
    }
}

bool Game::toggleFullscreen() {
    if (!gameWindow_) {
        std::cerr << "[Game] Cannot toggle fullscreen: Window is null" << std::endl;
        return false;
    }
    
    bool success = gameWindow_->toggleFullscreen();
    
    if (success) {
        // Get new window dimensions
        int width = gameWindow_->getWidth();
        int height = gameWindow_->getHeight();
        
        std::cout << "[Game] Fullscreen toggled. New window dimensions: " << width << "x" << height << std::endl;
        
        // Update camera aspect ratio
        if (camera_) {
            camera_->updateAspect(static_cast<float>(width) / static_cast<float>(height));
            std::cout << "[Game] Camera aspect ratio updated to: " << width << "x" << height << std::endl;
        }
        
        // Update UI systems - but keep menu sizes the same
        if (menuSystem_) {
            // First update fullscreen state since this doesn't resize anything
            menuSystem_->updateFullscreenState(isFullscreen());
            
            // Store current menu sizes
            glm::vec2 mainSize = menuSystem_->getMainMenuSize();
            glm::vec2 settingsSize = menuSystem_->getSettingsMenuSize();
            
            // Update screen dimensions in the renderer
            menuSystem_->updateScreenSize(width, height);
            
            // Double-check menu sizes were preserved properly
            std::cout << "[Game] Menu system updated for new screen size: " << width << "x" << height << std::endl;
        }
        
        // Update HUD position for new screen size
        if (hudSystem_) {
            float uiSize = 120.0f; // Same size as in initialization
            float centerX = (width - uiSize) / 2.0f;
            float bottomY = height - uiSize - 50.0f; // 50px margin from bottom
            
            hudSystem_->setPosition(centerX, bottomY);
            DEBUG_LOG("Game", "HUD repositioned to (" + std::to_string(static_cast<int>(centerX)) + 
                      ", " + std::to_string(static_cast<int>(bottomY)) + ") for screen size: " + 
                      std::to_string(width) + "x" + std::to_string(height));
        }
        
        // Update render coordinator if needed
        if (renderCoordinator_) {
            // If there's a method to update the render coordinator, call it here
            std::cout << "[Game] Render coordinator notified of resolution change" << std::endl;
        }
        
        // Note: We can't update screenWidth_ and screenHeight_ because they are const
        std::cout << "[Game] New window dimensions: " << width << "x" << height 
                  << " (original: " << screenWidth_ << "x" << screenHeight_ << ")" << std::endl;
    }
    
    std::cout << "[Game] Fullscreen toggled: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    return success;
}

bool Game::isFullscreen() const {
    if (!gameWindow_) {
        return false;
    }
    return gameWindow_->isFullscreen();
}

void Game::requestExit() {
    std::cout << "[Game] Exit requested. Shutting down..." << std::endl;
    isRunning_ = false;
}

// Delegates all per-frame game logic to GameLogic module
#include "core/GameLogic.h"
void Game::update(float deltaTime) {
    // Check if world is ready for block operations (world loading logic)
    if (!isWorldFullyLoaded_) {
        auto currentTime = std::chrono::steady_clock::now();
        auto timeSinceInit = std::chrono::duration_cast<std::chrono::seconds>(currentTime - worldInitTime_).count();
        if (timeSinceInit >= 5 && worldManager_) { // Wait 5 seconds for chunks to load
            // Additional check: ensure we have chunks around the player
            if (camera_) {
                glm::vec3 playerPos = camera_->getPosition();
                int_fast64_t chunkX = VoxelCastle::World::WorldManager::worldToColumnBaseX(static_cast<int_fast64_t>(playerPos.x));
                int_fast64_t chunkZ = VoxelCastle::World::WorldManager::worldToColumnBaseZ(static_cast<int_fast64_t>(playerPos.z));
                const auto* chunkColumn = worldManager_->getChunkColumn(chunkX, chunkZ);
                if (chunkColumn) {
                    isWorldFullyLoaded_ = true;
                    std::cout << "[Game] World marked as fully loaded and ready for block operations" << std::endl;
                }
            }
        }
    }

    GameLogic::update(*this, deltaTime);

    if (camera_ && worldManager_ && worldGenerator_) {
        glm::vec3 cameraPos = camera_->getPosition();
        int loadRadiusInSegments = 4; // Increased from 3 to extend visibility
        // Always call updateActiveChunks every frame to ensure chunks are loaded even for small camera movements
        worldManager_->updateActiveChunks(cameraPos, loadRadiusInSegments, *worldGenerator_);
    }
    
    // Update UI system
    if (menuSystem_) {
        menuSystem_->update(deltaTime);
        
        // Sync block selection UI with current block type
        if (hudSystem_ && hudSystem_->getCurrentBlockType() != currentBlockType_) {
            hudSystem_->setCurrentBlockType(currentBlockType_);
        }
    }
}

void Game::render() {
    if (!camera_ || !worldManager_ || !meshRenderer_ || !gameWindow_ || !textureAtlas_ || !renderCoordinator_) {
        std::cerr << "Game::render - Required components not available." << std::endl;
        return; 
    }

    // Use the actual current window dimensions instead of the original screen dimensions
    int currentWidth = gameWindow_->getWidth();
    int currentHeight = gameWindow_->getHeight();

    // Delegate all rendering to the GameRenderCoordinator
    renderCoordinator_->render(
        *camera_,
        *meshRenderer_,
        *textureAtlas_,
        *gameWindow_,
        *worldManager_,
        menuSystem_.get(),
        currentWidth,
        currentHeight
    );
}

bool Game::isWorldReadyForBlockOperations() const {
    // Check if world is marked as fully loaded
    if (!isWorldFullyLoaded_) {
        return false;
    }
    
    // Ensure minimum time has passed since world initialization (safety buffer)
    auto now = std::chrono::steady_clock::now();
    auto timeSinceInit = std::chrono::duration_cast<std::chrono::seconds>(now - worldInitTime_).count();
    if (timeSinceInit < 3) { // Require at least 3 seconds after initialization
        return false;
    }
    
    // Check that essential systems are available
    if (!worldManager_ || !camera_) {
        return false;
    }
    
    // Additional safety: check that we have some chunks loaded around the player
    glm::vec3 playerPos = camera_->getPosition();
    int_fast64_t chunkX = VoxelCastle::World::WorldManager::worldToColumnBaseX(static_cast<int_fast64_t>(playerPos.x));
    int_fast64_t chunkZ = VoxelCastle::World::WorldManager::worldToColumnBaseZ(static_cast<int_fast64_t>(playerPos.z));
    
    // Check that the chunk column where the player is located exists
    const auto* chunkColumn = worldManager_->getChunkColumn(chunkX, chunkZ);
    if (!chunkColumn) {
        return false;
    }
    
    return true;
}

// Mouse capture methods implementation
bool Game::isMouseCaptured() const {
    if (mouseCaptureManager_) {
        return mouseCaptureManager_->isCaptured();
    }
    return mouseCaptured_; // Fallback to old boolean
}

void Game::setMouseCaptured(bool captured) {
    mouseCaptured_ = captured; // Keep the old boolean in sync
    
    if (mouseCaptureManager_) {
        auto mode = captured ? VoxelEngine::Input::MouseCaptureManager::CaptureMode::CAPTURED 
                            : VoxelEngine::Input::MouseCaptureManager::CaptureMode::FREE;
        mouseCaptureManager_->setCaptureMode(mode);
    }
}

bool Game::isWindowRunning() const {
    return gameWindow_ && gameWindow_->isRunning();
}

bool Game::hasWindow() const {
    return gameWindow_ != nullptr;
}
