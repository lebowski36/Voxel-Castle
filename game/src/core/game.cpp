#include <cmath> // For std::floor
#include "../../include/core/game.h"       // Defines Game, forward declares SpectatorCamera
#include "../../include/SpectatorCamera.h" // Full definition of SpectatorCamera
#include "../../include/core/GameInitializer.h"    // Initialization and shutdown helper
#include "core/WorldSetup.h" // Added for world setup
#include "core/WorldSetupGlobals.h"
#include "core/GameRenderer.h" // Added for rendering logic

// UI System includes
#include "ui/UISystem.h"
#include "ui/elements/BlockSelectionUI.h"
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
      worldGenerator_(nullptr), // Added initialization
      textureAtlas_(nullptr),
      meshBuilder_(nullptr),
      meshRenderer_(nullptr),
      camera_(nullptr),
      isRunning_(false),
      // lastFrameTime_ will be initialized in initialize()
      mouseCaptured_(true),
      speedMultiplier_(1.0f),
      forward_(false), backward_(false), left_(false), right_(false), up_(false), down_(false),
      manualVoxelChangeRequested_(false),
      mouseDeltaX_(0.0f),
      mouseDeltaY_(0.0f),
      uiSystem_(nullptr),
      blockSelectionUI_(nullptr)
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

    // Set the global initial camera position for world setup
    if (camera_) {
        g_initialCameraPosition = camera_->getPosition();
    } else {
        g_initialCameraPosition = glm::vec3(0.0f);
    }

    // World content initialization delegated to WorldSetup module

    // [Dynamic Chunk Loading] Static world initialization removed. Chunks will be loaded dynamically as the camera moves.

    // Initial mesh build after world content is set up
    if (isRunning_ && worldManager_ && textureAtlas_ && meshBuilder_) {
        worldManager_->updateDirtyMeshes(*textureAtlas_, *meshBuilder_);
    }
    
    // Initialize UI system
    if (isRunning_ && meshRenderer_ && textureAtlas_) {
        uiSystem_ = std::make_unique<VoxelEngine::UI::UISystem>();
        if (!uiSystem_->initialize(screenWidth_, screenHeight_, projectRoot_)) {
            std::cerr << "Failed to initialize UI system" << std::endl;
            isRunning_ = false;
            return false;
        }
        
        // Get the texture atlas OpenGL texture ID from mesh renderer
        GLuint atlasTextureId = meshRenderer_->getTextureAtlasID();
        
        // Create block selection UI element with proper constructor parameters
        blockSelectionUI_ = std::make_shared<VoxelEngine::UI::BlockSelectionUI>(
            &uiSystem_->getRenderer(), 
            textureAtlas_.get(), 
            atlasTextureId
        );
        
        // Position it at bottom center of screen
        float uiSize = 120.0f; // Reasonable size for UI element
        float centerX = (screenWidth_ - uiSize) / 2.0f;
        float bottomY = screenHeight_ - uiSize - 50.0f; // 50px margin from bottom
        
        std::cout << "[Game] Positioning BlockSelectionUI at bottom center (" << centerX << ", " << bottomY 
                  << ") with screen size " << screenWidth_ << "x" << screenHeight_ << std::endl;
        
        blockSelectionUI_->setPosition(centerX, bottomY);
        blockSelectionUI_->setSize(uiSize, uiSize);
        
        // Debug: Let's verify the position was set correctly
        std::cout << "[Game] Actual BlockSelectionUI position: (" 
                  << blockSelectionUI_->getPosition().x << ", " 
                  << blockSelectionUI_->getPosition().y << ")" << std::endl;
        
        // Add to UI system
        uiSystem_->addElement(blockSelectionUI_);
        
        std::cout << "[Game] UI system initialized successfully with BlockSelectionUI" << std::endl;
    }
    
    return isRunning_;
}

void Game::run() {
    if (!isRunning_) {
        std::cerr << "Game::run() called but game is not initialized or already shut down." << std::endl;
        return;
    }

    std::cout << "[" << getTimestampGame() << "] [Game] Starting main game loop..." << std::endl;
    // lastFrameTime_ is initialized in initialize()
    
    int frameCount = 0;
    auto lastWindowCheckTime = std::chrono::steady_clock::now();

    while(isRunning_ && gameWindow_ && gameWindow_->isRunning()) {
        frameCount++;
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime_).count();
        lastFrameTime_ = currentTime;
        
        // Check window state periodically - more frequently during block operations
        bool shouldLogFrame = (frameCount % 50 == 0) || hasPendingBlockAction();
        auto timeSinceWindowCheck = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastWindowCheckTime);
        
        if (shouldLogFrame || timeSinceWindowCheck.count() > 1000) { // Check at least every second
            bool windowRunning = gameWindow_ && gameWindow_->isRunning();
            std::cout << "[" << getTimestampGame() << "] [Game] Frame " << frameCount 
                     << ", window valid: " << (gameWindow_ != nullptr) 
                     << ", window running: " << windowRunning
                     << ", deltaTime: " << deltaTime << "s" << std::endl;
            lastWindowCheckTime = currentTime;
            
            if (!windowRunning) {
                std::cerr << "[" << getTimestampGame() << "] [Game] CRITICAL: Window stopped running! Breaking main loop." << std::endl;
                break;
            }
        }

        // Ensure deltaTime is not excessively large (e.g., after a breakpoint or long pause)
        // This can prevent jerky movements or physics explosions.
        // Adjust max_deltaTime as needed, e.g., 0.1f (100ms) or 0.25f
        const float max_deltaTime = 0.25f; 
        if (deltaTime > max_deltaTime) {
            deltaTime = max_deltaTime;
        }

        if (shouldLogFrame) {
            std::cout << "[" << getTimestampGame() << "] [Game] Processing input..." << std::endl;
        }
        processInput();

        // If processInput decided to quit (e.g. SDL_EVENT_QUIT), isRunning_ will be false.
        // Check before proceeding to update and render to allow a clean exit.
        if (!isRunning_) {
            std::cout << "[" << getTimestampGame() << "] [Game] isRunning_ set to false, breaking loop" << std::endl;
            break; 
        }
        
        if (shouldLogFrame) {
            std::cout << "[" << getTimestampGame() << "] [Game] Calling update..." << std::endl;
        }
        update(deltaTime);
        
        if (shouldLogFrame) {
            std::cout << "[" << getTimestampGame() << "] [Game] Calling render..." << std::endl;
        }
        render();
        
        if (shouldLogFrame) {
            std::cout << "[" << getTimestampGame() << "] [Game] Frame " << frameCount << " completed successfully" << std::endl;
        }

        // Optional: Cap framerate or yield to prevent 100% CPU usage if vsync is off
        // For now, relying on vsync or letting it run as fast as possible.
        // std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Example minimal yield
    }
    std::cout << "[" << getTimestampGame() << "] [Game] Main game loop ended after " << frameCount << " frames." << std::endl;
}

void Game::shutdown() {
    // Clean up UI system first
    if (blockSelectionUI_) {
        blockSelectionUI_.reset();
    }
    if (uiSystem_) {
        uiSystem_->shutdown();
        uiSystem_.reset();
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

// Delegates all per-frame game logic to GameLogic module
#include "core/GameLogic.h"
void Game::update(float deltaTime) {
    GameLogic::update(*this, deltaTime);

    if (camera_ && worldManager_ && worldGenerator_) {
        glm::vec3 cameraPos = camera_->getPosition();
        int loadRadiusInSegments = 4; // Increased from 3 to extend visibility
        // Always call updateActiveChunks every frame to ensure chunks are loaded even for small camera movements
        worldManager_->updateActiveChunks(cameraPos, loadRadiusInSegments, *worldGenerator_);
    }
    
    // Update UI system
    if (uiSystem_) {
        uiSystem_->update(deltaTime);
        
        // Sync block selection UI with current block type
        if (blockSelectionUI_ && blockSelectionUI_->getCurrentBlockType() != currentBlockType_) {
            blockSelectionUI_->setCurrentBlockType(currentBlockType_);
        }
    }
}

void Game::render() {
    if (!camera_ || !worldManager_ || !meshRenderer_ || !gameWindow_ || !textureAtlas_) {
        std::cerr << "Game::render - Required components not available." << std::endl;
        return; 
    }

    // Clear the color and depth buffers BEFORE rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Delegate rendering to GameRenderer
    GameRenderer::renderGame(
        *camera_,
        *meshRenderer_,
        *textureAtlas_,
        *gameWindow_,
        worldManager_->getAllSegmentMeshes(),
        screenWidth_,
        screenHeight_
    );
    
    // Verify the viewport is properly set for UI rendering
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (viewport[2] != screenWidth_ || viewport[3] != screenHeight_) {
        std::cout << "Correcting viewport before UI rendering: " 
                  << viewport[0] << "," << viewport[1] << "," << viewport[2] << "x" << viewport[3] 
                  << " to 0,0," << screenWidth_ << "x" << screenHeight_ << std::endl;
        glViewport(0, 0, screenWidth_, screenHeight_);
    }
    
    // Render UI system on top of the 3D world
    if (uiSystem_) {
        // Reset any OpenGL error state before UI rendering
        while (glGetError() != GL_NO_ERROR) {} 
        
        uiSystem_->render();
    }
    
    // Swap buffers AFTER all rendering (3D world + UI) is complete
    if (gameWindow_) {
        gameWindow_->render();
    }
}
