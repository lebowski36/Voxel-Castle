#include "../../include/core/game.h"       // Defines Game, forward declares SpectatorCamera
#include "../../include/SpectatorCamera.h" // Full definition of SpectatorCamera
#include "../../include/core/GameInitializer.h"    // Initialization and shutdown helper
#include "core/WorldSetup.h" // Added for world setup

// Include headers that will be needed for the actual implementations later
#include <iostream> 
#include <vector> // For potential use in migrated logic
#include <string> // For potential use in migrated logic
#include <fstream> // For potential use in migrated logic
#include <sstream> // For potential use in migrated logic
#include <chrono> // For std::chrono for timing
#include <set> // Required for std::set
#include <algorithm> // For std::min/max (potentially useful, good to have)

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
    
    // World content initialization delegated to WorldSetup module
    if (isRunning_ && worldManager_ && worldGenerator_) { // Ensure basic systems are up
        GameWorldSetup::initializeStaticWorld(*worldManager_, *worldGenerator_);
    }

    // Initial mesh build after world content is set up
    if (isRunning_ && worldManager_ && textureAtlas_ && meshBuilder_) {
        worldManager_->updateDirtyMeshes(*textureAtlas_, *meshBuilder_);
    }
    return isRunning_;
}

void Game::run() {
    if (!isRunning_) {
        std::cerr << "Game::run() called but game is not initialized or already shut down." << std::endl;
        return;
    }

    std::cout << "Game::run() - Starting main game loop..." << std::endl;
    // lastFrameTime_ is initialized in initialize()

    while(isRunning_ && gameWindow_ && gameWindow_->isRunning()) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime_).count();
        lastFrameTime_ = currentTime;

        // Ensure deltaTime is not excessively large (e.g., after a breakpoint or long pause)
        // This can prevent jerky movements or physics explosions.
        // Adjust max_deltaTime as needed, e.g., 0.1f (100ms) or 0.25f
        const float max_deltaTime = 0.25f; 
        if (deltaTime > max_deltaTime) {
            deltaTime = max_deltaTime;
        }

        processInput();

        // If processInput decided to quit (e.g. SDL_EVENT_QUIT), isRunning_ will be false.
        // Check before proceeding to update and render to allow a clean exit.
        if (!isRunning_) {
            break; 
        }

        update(deltaTime);
        render();

        // Optional: Cap framerate or yield to prevent 100% CPU usage if vsync is off
        // For now, relying on vsync or letting it run as fast as possible.
        // std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Example minimal yield
    }
    std::cout << "Game::run() - Main game loop ended." << std::endl;
}

void Game::shutdown() {
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

void Game::update(float deltaTime) {
    // Process mouse look if mouse is captured and there was movement
    if (mouseCaptured_ && camera_ && (mouseDeltaX_ != 0.0f || mouseDeltaY_ != 0.0f)) {
        camera_->processMouse(mouseDeltaX_, mouseDeltaY_);
        // mouseDeltaX_ and mouseDeltaY_ are reset in processInput() each frame before polling
    }

    // Process keyboard movement for the camera
    if (camera_) {
        camera_->processKeyboard(deltaTime, forward_, backward_, left_, right_, up_, down_, speedMultiplier_);
    }

    // Progress ECS systems
    if (ecs_) {
        ecs_->progress(deltaTime); // Pass deltaTime if your system uses it
    }

    // Update game window (if it has its own update logic, e.g., title, etc.)
    if (gameWindow_) {
        gameWindow_->update(); 
    }

    // --- Game Specific Update Logic ---
    if (manualVoxelChangeRequested_ && worldManager_) {
        std::cout << "M key pressed. Inverting checkerboard pattern in segment (0,0,0)..." << std::endl;
        VoxelCastle::World::ChunkColumn* column = worldManager_->getChunkColumn(0, 0);
        if (column) {
            VoxelCastle::World::ChunkSegment* segment = column->getSegmentByIndex(0); // Target the first segment
            if (segment) {
                for (uint8_t lsy = 0; lsy < VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT; ++lsy) {
                    for (uint8_t lsz = 0; lsz < VoxelCastle::World::ChunkSegment::CHUNK_DEPTH; ++lsz) {
                        for (uint8_t lsx = 0; lsx < VoxelCastle::World::ChunkSegment::CHUNK_WIDTH; ++lsx) {
                            int_fast64_t worldX = static_cast<int_fast64_t>(lsx);
                            int_fast64_t worldY = static_cast<int_fast64_t>(lsy);
                            int_fast64_t worldZ = static_cast<int_fast64_t>(lsz);

                            ::VoxelEngine::World::Voxel currentVoxel = worldManager_->getVoxel(worldX, worldY, worldZ);
                            ::VoxelEngine::World::VoxelType currentType = static_cast<::VoxelEngine::World::VoxelType>(currentVoxel.id);
                            ::VoxelEngine::World::VoxelType newType = currentType;

                            if (currentType == static_cast<::VoxelEngine::World::VoxelType>(1)) {
                                newType = static_cast<::VoxelEngine::World::VoxelType>(2);
                            } else if (currentType == static_cast<::VoxelEngine::World::VoxelType>(2)) {
                                newType = static_cast<::VoxelEngine::World::VoxelType>(1);
                            }

                            if (newType != currentType) {
                                worldManager_->setVoxel(worldX, worldY, worldZ, newType);
                            }
                        } // end lsx loop
                    } // end lsz loop
                } // end lsy loop
                std::cout << "Checkerboard inversion applied. Segment (0,0,0) marked dirty." << std::endl;
            } else { // else for if (segment)
                std::cerr << "Error: Could not get segment 0 for inversion." << std::endl;
            } // end if (segment)
        } else { // else for if (column)
            std::cerr << "Error: Could not get chunk column (0,0) for inversion." << std::endl;
        } // end if (column)
        manualVoxelChangeRequested_ = false; // Reset the flag
    } // end if (manualVoxelChangeRequested_ && worldManager_)

    // Update meshes if any segments are dirty
    if (worldManager_ && textureAtlas_ && meshBuilder_) {
        worldManager_->updateDirtyMeshes(*textureAtlas_, *meshBuilder_);
    }
}

void Game::render() {
    static int frameCounter = 0; // Add a static frame counter

    if (!camera_ || !worldManager_ || !meshRenderer_ || !gameWindow_ || !textureAtlas_) {
        return; 
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 view = camera_->getViewMatrix();
    glm::mat4 proj = camera_->getProjectionMatrix();

    auto meshesToRender = worldManager_->getAllSegmentMeshes();
    
    // Frame summary logging - now only every 100 frames
    if (frameCounter % 100 == 0) { // Check if it's the 100th frame
        if (!meshesToRender.empty()) {
            std::set<glm::vec3, Vec3Comparator> uniquePositions;
            
            unsigned int meshCount = 0;
            unsigned int totalVertices = 0;
            unsigned int totalIndices = 0;

            for (const auto* vMesh : meshesToRender) {
                if (vMesh && vMesh->isInitialized()) {
                    meshCount++;
                    totalVertices += vMesh->getVertexCount();
                    totalIndices += vMesh->getIndexCount();
                    uniquePositions.insert(vMesh->getWorldPosition());
                }
            }

            std::ostringstream positionsStream;
            for (const auto& pos : uniquePositions) {
                positionsStream << "(" << pos.x << "," << pos.y << "," << pos.z << ") ";
            }

            std::cout << "[Game::render Frame Summary] Frame: " << frameCounter 
                      << ", Meshes: " << meshCount 
                      << ", Vertices: " << totalVertices 
                      << ", Indices: " << totalIndices
                      << ", Unique Positions: " << uniquePositions.size() << " [" << positionsStream.str() << "]" << std::endl;
        }
    }

    frameCounter++; // Increment frame counter

    for (const auto* vMesh : meshesToRender) {
        if (vMesh && vMesh->isInitialized()) { 
             meshRenderer_->uploadMesh(*vMesh); 
             glm::mat4 model = glm::translate(glm::mat4(1.0f), vMesh->getWorldPosition());
             meshRenderer_->draw(model, view, proj);
        }
    }
    
    // Debug Atlas Rendering (optional)
    // VoxelEngine::Rendering::Debug::drawDebugAtlasQuad(textureAtlas_->getTextureID(), screenWidth_, screenHeight_); 
    // VoxelEngine::Rendering::Debug::drawSingleTileDebugQuad(textureAtlas_->getTextureID(), screenWidth_, screenHeight_);

    gameWindow_->render(); 
}
