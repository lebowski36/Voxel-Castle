#include "../../include/core/game.h"       // Defines Game, forward declares SpectatorCamera
#include "../../include/SpectatorCamera.h" // Full definition of SpectatorCamera

// Include headers that will be needed for the actual implementations later
#include <iostream> 
#include <vector> // For potential use in migrated logic
#include <string> // For potential use in migrated logic
#include <fstream> // For potential use in migrated logic
#include <sstream> // For potential use in migrated logic
#include <chrono> // For std::chrono for timing
#include <set> // Required for std::set
#include <algorithm> // For std::min/max (potentially useful, good to have)

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
    std::cout << "Game::initialize() - Initializing Game Window..." << std::endl;

    gameWindow_ = std::make_unique<Window>("Voxel Fortress - Alpha", screenWidth_, screenHeight_);
    if (!gameWindow_ || !gameWindow_->init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        isRunning_ = false; // Ensure isRunning is false if init fails
        return false;
    }
    std::cout << "Game Window initialized successfully." << std::endl;

    // --- OpenGL State Setup ---
    std::cout << "Game::initialize() - Setting up OpenGL state..." << std::endl;
    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Enable face culling for performance (render only front faces)
    glEnable(GL_CULL_FACE); // Re-enabled
    glCullFace(GL_BACK); 
    glFrontFace(GL_CCW); // Define front faces by counter-clockwise winding order
    // Set filled polygon mode for normal rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Changed to LINE for troubleshooting
    
    // Debug OpenGL state
    // Properly check if depth test is enabled
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    std::cout << "  OpenGL: Depth testing enabled: " << (depthTestEnabled ? "Yes" : "No") << std::endl;
    GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    std::cout << "  OpenGL: Face culling enabled: " << (cullFaceEnabled ? "Yes" : "No") << std::endl;
    std::cout << "OpenGL state setup complete." << std::endl;

    // --- ECS Setup ---
    std::cout << "Game::initialize() - Setting up ECS..." << std::endl;
    ecs_ = std::make_unique<flecs::world>();
    if (!ecs_) {
        std::cerr << "Failed to initialize Flecs ECS world!" << std::endl;
        // Consider if this is a fatal error that should return false
        // For now, proceeding, but logging the error.
    } else {
        // Register systems
        // Note: movementSystem's lifetime is currently tied to the initialize() scope.
        // If the MovementSystem object itself needs to persist, it should be a member of Game class.
        // However, Flecs systems, once registered by such an object, usually persist with the world.
        VoxelEngine::ecs::MovementSystem movementSystem(*ecs_); 
        std::cout << "  ECS: MovementSystem registered." << std::endl;
        // Future systems will be registered here
        std::cout << "ECS setup complete." << std::endl;
    }

    // --- World and Rendering Systems Setup ---
    std::cout << "Game::initialize() - Setting up World and Rendering Systems..." << std::endl;
    worldManager_ = std::make_unique<VoxelCastle::World::WorldManager>();
    if (!worldManager_) {
        std::cerr << "Failed to initialize WorldManager!" << std::endl;
        // Potentially return false or handle error
    } else {
        std::cout << "  WorldManager initialized." << std::endl;
    }

    worldGenerator_ = std::make_unique<VoxelCastle::World::WorldGenerator>(); // Added instantiation
    if (!worldGenerator_) {
        std::cerr << "Failed to initialize WorldGenerator!" << std::endl;
        // Potentially return false or handle error
    } else {
        std::cout << "  WorldGenerator initialized." << std::endl;
    }

    textureAtlas_ = std::make_unique<VoxelEngine::Rendering::TextureAtlas>();
    if (!textureAtlas_) {
        std::cerr << "Failed to initialize TextureAtlas!" << std::endl;
        // Potentially return false or handle error
    } else {
        std::cout << "  TextureAtlas initialized." << std::endl;
    }

    meshBuilder_ = std::make_unique<VoxelEngine::Rendering::MeshBuilder>();
    if (!meshBuilder_) {
        std::cerr << "Failed to initialize MeshBuilder!" << std::endl;
        // Potentially return false or handle error
    } else {
        std::cout << "  MeshBuilder initialized." << std::endl;
    }

    meshRenderer_ = std::make_unique<VoxelEngine::Rendering::MeshRenderer>();
    if (!meshRenderer_) {
        std::cerr << "Failed to initialize MeshRenderer!" << std::endl;
        // Potentially return false or handle error
    } else {
        std::cout << "  MeshRenderer initialized." << std::endl;
    }
    std::cout << "World and Rendering Systems setup complete." << std::endl;

    // --- Initial World Content ---
    std::cout << "Game::initialize() - Setting up initial world content..." << std::endl;
    if (worldManager_ && worldGenerator_) { // Ensure worldGenerator_ is also valid
        initializeWorldContent(); // Call new method
        std::cout << "  Initial world content setup via initializeWorldContent() complete." << std::endl;
    } else {
        std::cerr << "  Skipping initial world content setup as WorldManager or WorldGenerator is null." << std::endl;
    }

    // --- Initial Mesh Build ---
    std::cout << "Game::initialize() - Performing initial mesh build..." << std::endl;
    if (worldManager_ && textureAtlas_ && meshBuilder_) {
        worldManager_->updateDirtyMeshes(*textureAtlas_, *meshBuilder_);
        std::cout << "  Initial mesh build complete." << std::endl;
    } else {
        std::cerr << "  Skipping initial mesh build due to null WorldManager, TextureAtlas, or MeshBuilder." << std::endl;
    }

    // --- Debug Utilities Setup ---
    std::cout << "Game::initialize() - Setting up debug utilities..." << std::endl;
    VoxelEngine::Rendering::Debug::setupDebugAtlasQuad(projectRoot_, screenWidth_, screenHeight_);
    VoxelEngine::Rendering::Debug::setupSingleTileDebugQuad(screenWidth_, screenHeight_);
    std::cout << "  Debug utilities setup complete." << std::endl;

    // --- Camera Setup ---
    std::cout << "Game::initialize() - Setting up camera..." << std::endl;
    camera_ = std::make_unique<SpectatorCamera>(
        glm::vec3(16.0f, 24.0f, 48.0f), // position
        -90.0f, 0.0f,                   // yaw, pitch
        70.0f,                          // fov
        static_cast<float>(screenWidth_) / static_cast<float>(screenHeight_), // aspect
        0.1f, 500.0f                    // near, far
    );
    if (!camera_) {
        std::cerr << "Failed to initialize SpectatorCamera!" << std::endl;
        // Consider this a fatal error for now and return false
        isRunning_ = false;
        return false;
    }
    std::cout << "  SpectatorCamera initialized. Field of view: " << camera_->fov << " degrees" << std::endl;
    std::cout << "Camera setup complete." << std::endl;

    // --- Background Color ---
    std::cout << "Game::initialize() - Setting clear color..." << std::endl;
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f); // A slightly more pleasant blue
    std::cout << "  Clear color set to (0.1, 0.2, 0.3, 1.0)." << std::endl;
    
    // Initialize timing and running state
    std::cout << "Game::initialize() - Finalizing initialization..." << std::endl;
    lastFrameTime_ = std::chrono::steady_clock::now();
    // mouseCaptured_ is true by default (from constructor/header)
    // input flags (forward_, etc.) are false by default (from constructor/header)
    
    isRunning_ = true; 
    std::cout << "Game initialization complete. isRunning_ = true." << std::endl;
    return true;
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
    std::cout << "Game::shutdown() - Initiating shutdown sequence..." << std::endl;

    // 1. Cleanup Debug Utilities
    std::cout << "  Shutting down Debug Utilities..." << std::endl;
    VoxelEngine::Rendering::Debug::cleanupDebugQuads();
    std::cout << "  Debug Utilities shutdown complete." << std::endl;

    // 2. Cleanup Game Window
    // This typically involves destroying the SDL window and quitting SDL subsystems
    // if this was the last window.
    if (gameWindow_) {
        std::cout << "  Shutting down Game Window..." << std::endl;
        gameWindow_->cleanUp(); // This should handle SDL_DestroyWindow and potentially SDL_Quit
        gameWindow_.reset(); // Release the unique_ptr
        std::cout << "  Game Window shutdown complete." << std::endl;
    } else {
        std::cout << "  Game Window was already null." << std::endl;
    }

    // 3. Release other resources managed by unique_ptr.
    // Their destructors will be called automatically when the Game object is destroyed,
    // but explicit reset() can be used for controlled shutdown order or logging.
    std::cout << "  Releasing rendering resources..." << std::endl;
    if (meshRenderer_) {
        meshRenderer_.reset();
        std::cout << "    MeshRenderer released." << std::endl;
    }
    if (meshBuilder_) {
        meshBuilder_.reset();
        std::cout << "    MeshBuilder released." << std::endl;
    }
    if (textureAtlas_) {
        textureAtlas_.reset();
        std::cout << "    TextureAtlas released." << std::endl;
    }
    
    std::cout << "  Releasing world and ECS..." << std::endl;
    if (worldManager_) {
        worldManager_.reset();
        std::cout << "    WorldManager released." << std::endl;
    }
    if (ecs_) {
        ecs_.reset(); // Flecs world destructor will clean up entities, components, systems.
        std::cout << "    ECS world released." << std::endl;
    }

    if (camera_) {
        camera_.reset();
        std::cout << "    Camera released." << std::endl;
    }
    if (worldGenerator_) { // Added for shutdown
        worldGenerator_.reset();
        std::cout << "    WorldGenerator released." << std::endl;
    }

    // 4. Mark as not running
    isRunning_ = false;
    std::cout << "Game shutdown sequence complete. isRunning_ = false." << std::endl;
}

void Game::initializeWorldContent() {
    std::cout << "Game::initializeWorldContent() - Generating initial static world area..." << std::endl;
    if (!worldManager_ || !worldGenerator_) {
        std::cerr << "Error: WorldManager or WorldGenerator is null in initializeWorldContent." << std::endl;
        return;
    }

    const int initialLoadRadiusInSegments = 1; // Creates a 3x3 area of chunk columns
    const int centerSegX = 0;
    const int centerSegZ = 0;
    const int minYSegments = 0; // Generate segments at Y=0 and Y=1
    const int maxYSegments = 1;

    for (int sgX = centerSegX - initialLoadRadiusInSegments; sgX <= centerSegX + initialLoadRadiusInSegments; ++sgX) {
        for (int sgZ = centerSegZ - initialLoadRadiusInSegments; sgZ <= centerSegZ + initialLoadRadiusInSegments; ++sgZ) {
            // Ensure the ChunkColumn exists. Assuming getOrCreateChunkColumn takes column indices (which are segment XZ indices)
            VoxelCastle::World::ChunkColumn* column = worldManager_->getOrCreateChunkColumn(sgX, sgZ);
            if (!column) {
                std::cerr << "Failed to get or create chunk column at (" << sgX << ", " << sgZ << ")" << std::endl;
                continue; // Skip to next column if this one fails
            }
            std::cout << "  Processing Chunk Column (" << sgX << ", " << sgZ << ")" << std::endl;

            for (int sgY = minYSegments; sgY <= maxYSegments; ++sgY) {
                VoxelCastle::World::ChunkSegment* segment = column->getOrCreateSegment(sgY);
                if (segment) {
                    // For this initial setup, we always regenerate.
                    // A more sophisticated check could be segment->isEmpty() or similar.
                    std::cout << "    Generating segment (" << sgX << ", " << sgY << ", " << sgZ << ")" << std::endl;
                    worldGenerator_->generateChunkSegment(*segment, sgX, sgY, sgZ);
                    segment->markDirty(true);
                    std::cout << "      Segment (" << sgX << ", " << sgY << ", " << sgZ << ") generated and marked dirty." << std::endl;
                } else {
                    std::cerr << "    Failed to get or create segment at Y index " << sgY << " for column (" << sgX << ", " << sgZ << ")" << std::endl;
                }
            }
        }
    }
    std::cout << "Game::initializeWorldContent() - Initial static world area generation attempt complete." << std::endl;
}

void Game::processInput() {
    // Reset mouse delta values at the beginning of each frame's input processing
    mouseDeltaX_ = 0.0f;
    mouseDeltaY_ = 0.0f;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            isRunning_ = false; // Signal the game loop to terminate
            // gameWindow_->cleanUp(); // This will be handled in Game::shutdown()
            // return 0; // No direct return, isRunning_ will stop the loop in Game::run()
        }
        if (e.type == SDL_EVENT_KEY_DOWN) {
            switch (e.key.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    mouseCaptured_ = !mouseCaptured_;
                    if (gameWindow_ && gameWindow_->getSDLWindow()) { // Ensure window is valid
                        SDL_SetWindowRelativeMouseMode(gameWindow_->getSDLWindow(), mouseCaptured_);
                    }
                    break;
                case SDL_SCANCODE_W: forward_ = true; break;
                case SDL_SCANCODE_S: backward_ = true; break;
                case SDL_SCANCODE_A: left_ = true; break;
                case SDL_SCANCODE_D: right_ = true; break;
                case SDL_SCANCODE_Q: down_ = true; break; // Or use for other actions
                case SDL_SCANCODE_E: up_ = true; break;   // Or use for other actions
                case SDL_SCANCODE_LSHIFT: speedMultiplier_ = 3.0f; break;
                case SDL_SCANCODE_SPACE: up_ = true; break;
                case SDL_SCANCODE_LCTRL: down_ = true; break;
                case SDL_SCANCODE_M: 
                    manualVoxelChangeRequested_ = true;
                    break;
                default: break;
            }
        }
        if (e.type == SDL_EVENT_KEY_UP) {
            switch (e.key.scancode) {
                case SDL_SCANCODE_W: forward_ = false; break;
                case SDL_SCANCODE_S: backward_ = false; break;
                case SDL_SCANCODE_A: left_ = false; break;
                case SDL_SCANCODE_D: right_ = false; break;
                case SDL_SCANCODE_Q: down_ = false; break;
                case SDL_SCANCODE_E: up_ = false; break;
                case SDL_SCANCODE_LSHIFT: speedMultiplier_ = 1.0f; break;
                case SDL_SCANCODE_SPACE: up_ = false; break;
                case SDL_SCANCODE_LCTRL: down_ = false; break;
                default: break;
            }
        }
        if (e.type == SDL_EVENT_MOUSE_MOTION && mouseCaptured_) {
            mouseDeltaX_ += static_cast<float>(e.motion.xrel);
            mouseDeltaY_ += static_cast<float>(-e.motion.yrel); // Invert Y for FPS camera style
        }
        if (e.type == SDL_EVENT_WINDOW_RESIZED) {
            if (camera_) { // Ensure camera is valid
                camera_->updateAspect(static_cast<float>(e.window.data1) / static_cast<float>(e.window.data2));
            }
            glViewport(0, 0, e.window.data1, e.window.data2);
        }
    }
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
