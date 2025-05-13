#include "../include/SpectatorCamera.h"
#include "platform/Window.h" // Adjust path as necessary based on include directories
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream> // Added
#include <sstream> // Added
#include <glad/glad.h>
#include "rendering/render_utils.h" // Added for shader loading utilities
#include "rendering/debug_utils.h" // Added for debug drawing utilities

// Flecs - ECS
#include <flecs.h>

// ECS Components
#include "ecs/components/position_component.h"
#include "ecs/components/velocity_component.h"
#include "ecs/components/renderable_component.h"

// ECS Systems
#include "ecs/systems/movement_system.h"

// Voxel World
#include "world/world_manager.h" // Defines VoxelCastle::World::WorldManager
#include "world/voxel_types.h" // Defines VoxelEngine::World::VoxelType
#include "world/voxel.h"       // Defines VoxelEngine::World::Voxel
#include "world/test_quadtree.h" // Added for Quadtree tests
#include "world/test_voxel_system.h" // Added for Voxel System tests

// Mesh Generation
#include "rendering/mesh_builder.h" // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h" // Added for TextureAtlas
#include <chrono> // For std::chrono for timing
#include <thread> // For std::this_thread::sleep_for

// GLM Headers - Will be used by other engine systems, keep includes for now if generally useful
#define GLM_FORCE_SILENT_WARNINGS // Optional: To suppress GLM warnings if any
#define GLM_ENABLE_EXPERIMENTAL // Enable GLM experimental extensions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // Ensure this is included after enabling experimental extensions

const int SCREEN_WIDTH = 2400; // 3x original width
const int SCREEN_HEIGHT = 1800; // 3x original height

int main(int /*argc*/, char* /*argv*/[]) { // Suppress unused parameter warnings
    std::cout << "Starting VoxelFortress Game" << std::endl;

    // Initialize Flecs world
    flecs::world ecs;

    // Register systems
    VoxelEngine::ecs::MovementSystem movementSystem(ecs);
    // Future systems will be registered here

    VoxelCastle::World::WorldManager worldManager; // Keep a single instance for the game

    // Run all tests (excluding mesh generation tests that might rely on old direct building)
    std::cout << "--- Running Core Tests ---" << std::endl;
    VoxelCastle::World::runQuadtreeTests();     // Run Quadtree tests
    VoxelCastle::World::runVoxelSystemTests();  // Run Voxel System tests
    std::cout << "--- Core Tests Completed ---" << std::endl;

    // Use a larger window size for visibility
    Window gameWindow("Voxel Fortress - Alpha", SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!gameWindow.init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        return -1;
    }

    // --- OpenGL State Setup ---
    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Enable face culling for performance (render only front faces)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); 
    glFrontFace(GL_CCW); // Define front faces by counter-clockwise winding order
    // Set filled polygon mode for normal rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Debug OpenGL state
    std::cout << "OpenGL debug: Setting up rendering state" << std::endl;
    // Properly check if depth test is enabled
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    std::cout << "Depth testing enabled: " << (depthTestEnabled ? "Yes" : "No") << std::endl;
    GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    std::cout << "Face culling enabled: " << (cullFaceEnabled ? "Yes" : "No") << std::endl;
    

    // --- World and Rendering Setup ---
    VoxelEngine::Rendering::TextureAtlas atlas; // Create texture atlas
    VoxelEngine::Rendering::MeshBuilder meshBuilder; // Create mesh builder

    // Initialize a 1x1x1 chunk area (one ChunkColumn)
    worldManager.getOrCreateChunkColumn(0, 0); 

    // Initial mesh build for all dirty segments
    worldManager.updateDirtyMeshes(atlas, meshBuilder);

    // MeshRenderer setup - it will now manage multiple meshes
    VoxelEngine::Rendering::MeshRenderer meshRenderer;

    // Setup debug atlas quad
    // Pass projectRoot, SCREEN_WIDTH, SCREEN_HEIGHT
    const std::string projectRoot = "/home/system-x1/Projects/Voxel Castle/";
    VoxelEngine::Rendering::Debug::setupDebugAtlasQuad(projectRoot, SCREEN_WIDTH, SCREEN_HEIGHT);
    VoxelEngine::Rendering::Debug::setupSingleTileDebugQuad(SCREEN_WIDTH, SCREEN_HEIGHT); // Pass SCREEN_WIDTH, SCREEN_HEIGHT

    // Camera setup (SpectatorCamera)
    SpectatorCamera camera(
        glm::vec3(16.0f, 24.0f, 48.0f), // position
        -90.0f, 0.0f,                   // yaw, pitch
        70.0f,                          // fov
        static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), // aspect
        0.1f, 500.0f                    // near, far
    );
    std::cout << "  Field of view: " << camera.fov << " degrees" << std::endl;

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f); // A slightly more pleasant blue
    std::cout << "Clear color set to (0.1, 0.2, 0.3)" << std::endl;
    
    std::cout << "Starting main rendering loop" << std::endl;

    int frameCount = 0;
    glm::mat4 model = glm::mat4(1.0f); // Model matrix for meshes (identity, as positions are world coords)

    auto startTime = std::chrono::steady_clock::now();
    static bool manualVoxelChangeRequested = false; // Added for M key trigger

    // Main loop
    // --- Input State ---
    static bool mouseCaptured = true;
    float speedMultiplier = 1.0f;
    bool forward = false, backward = false, left = false, right = false, up = false, down = false;

    while (gameWindow.isRunning()) {
        // --- Input Handling ---
        float deltaTime = 1.0f / 60.0f; // TODO: Use real frame timing

        SDL_Event e;
        float mouseDeltaX = 0.0f;
        float mouseDeltaY = 0.0f;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                gameWindow.cleanUp();
                return 0;
            }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        mouseCaptured = !mouseCaptured;
                        SDL_SetWindowRelativeMouseMode(gameWindow.getSDLWindow(), mouseCaptured);
                        break;
                    case SDL_SCANCODE_W: forward = true; break;
                    case SDL_SCANCODE_S: backward = true; break;
                    case SDL_SCANCODE_A: left = true; break;
                    case SDL_SCANCODE_D: right = true; break;
                    case SDL_SCANCODE_Q: down = true; break;
                    case SDL_SCANCODE_E: up = true; break;
                    case SDL_SCANCODE_LSHIFT: speedMultiplier = 3.0f; break;
                    case SDL_SCANCODE_SPACE: up = true; break;
                    case SDL_SCANCODE_LCTRL: down = true; break;
                    case SDL_SCANCODE_M: // Added M key handling
                        manualVoxelChangeRequested = true;
                        break;
                    default: break;
                }
            }
            if (e.type == SDL_EVENT_KEY_UP) {
                switch (e.key.scancode) {
                    case SDL_SCANCODE_W: forward = false; break;
                    case SDL_SCANCODE_S: backward = false; break;
                    case SDL_SCANCODE_A: left = false; break;
                    case SDL_SCANCODE_D: right = false; break;
                    case SDL_SCANCODE_Q: down = false; break;
                    case SDL_SCANCODE_E: up = false; break;
                    case SDL_SCANCODE_LSHIFT: speedMultiplier = 1.0f; break;
                    case SDL_SCANCODE_SPACE: up = false; break;
                    case SDL_SCANCODE_LCTRL: down = false; break;
                    default: break;
                }
            }
            if (e.type == SDL_EVENT_MOUSE_MOTION && mouseCaptured) {
                mouseDeltaX += static_cast<float>(e.motion.xrel);
                mouseDeltaY += static_cast<float>(-e.motion.yrel); // Invert Y for FPS
            }
            if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                camera.updateAspect(static_cast<float>(e.window.data1) / static_cast<float>(e.window.data2));
                glViewport(0, 0, e.window.data1, e.window.data2);
            }
        }

        // Always process mouse look if mouse is captured, even if no event this frame
        if (mouseCaptured && (mouseDeltaX != 0.0f || mouseDeltaY != 0.0f)) {
            camera.processMouse(mouseDeltaX, mouseDeltaY);
        }

        camera.processKeyboard(deltaTime, forward, backward, left, right, up, down, speedMultiplier);
        ecs.progress();
        gameWindow.update();

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = camera.getProjectionMatrix();

        // --- Game Logic & Mesh Updates ---
        if (manualVoxelChangeRequested) {
            std::cout << "M key pressed. Inverting checkerboard pattern in segment (0,0,0)..." << std::endl;
            VoxelCastle::World::ChunkColumn* column = worldManager.getChunkColumn(0, 0);
            if (column) {
                VoxelCastle::World::ChunkSegment* segment = column->getSegmentByIndex(0); // Target the first segment
                if (segment) {
                    for (uint8_t lsy = 0; lsy < VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT; ++lsy) {
                        for (uint8_t lsz = 0; lsz < VoxelCastle::World::ChunkSegment::CHUNK_DEPTH; ++lsz) {
                            for (uint8_t lsx = 0; lsx < VoxelCastle::World::ChunkSegment::CHUNK_WIDTH; ++lsx) {
                                // For segment at (0,0,0), local coords are world coords
                                int_fast64_t worldX = static_cast<int_fast64_t>(lsx);
                                int_fast64_t worldY = static_cast<int_fast64_t>(lsy);
                                int_fast64_t worldZ = static_cast<int_fast64_t>(lsz);

                                ::VoxelEngine::World::Voxel currentVoxel = worldManager.getVoxel(worldX, worldY, worldZ);
                                ::VoxelEngine::World::VoxelType currentType = static_cast<::VoxelEngine::World::VoxelType>(currentVoxel.id); // Corrected: type_id to id
                                ::VoxelEngine::World::VoxelType newType = currentType;

                                // Invert types 1 and 2, leave others (like AIR) alone
                                if (currentType == static_cast<::VoxelEngine::World::VoxelType>(1)) {
                                    newType = static_cast<::VoxelEngine::World::VoxelType>(2);
                                } else if (currentType == static_cast<::VoxelEngine::World::VoxelType>(2)) {
                                    newType = static_cast<::VoxelEngine::World::VoxelType>(1);
                                }

                                if (newType != currentType) {
                                    worldManager.setVoxel(worldX, worldY, worldZ, newType);
                                }
                            }
                        }
                    }
                    std::cout << "Checkerboard inversion applied. Segment (0,0,0) marked dirty." << std::endl;
                } else {
                    std::cerr << "Error: Could not get segment 0 for inversion." << std::endl;
                }
            } else {
                std::cerr << "Error: Could not get chunk column (0,0) for inversion." << std::endl;
            }
            manualVoxelChangeRequested = false; // Reset the flag
        }

        // Update meshes if any segments are dirty
        worldManager.updateDirtyMeshes(atlas, meshBuilder);

        // --- Rendering ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render all segment meshes
        auto meshesToRender = worldManager.getAllSegmentMeshes();
        for (const auto* vMesh : meshesToRender) {
            if (vMesh) { // Ensure mesh is not null
                 meshRenderer.uploadMesh(*vMesh); // Re-upload mesh data (can be optimized)
                 meshRenderer.draw(model, view, proj);
            }
        }
        
        // Debug Atlas Rendering (optional)
        // VoxelEngine::Rendering::Debug::drawDebugAtlasQuad(atlas.getTextureID(), SCREEN_WIDTH, SCREEN_HEIGHT); 
        // VoxelEngine::Rendering::Debug::drawSingleTileDebugQuad(atlas.getTextureID(), SCREEN_WIDTH, SCREEN_HEIGHT);

        gameWindow.render();
        frameCount++;

        // Optional: Cap framerate or yield
        // std::this_thread::sleep_for(std::chrono::milliseconds(16)); 
    }

    VoxelEngine::Rendering::Debug::cleanupDebugQuads(); // Added cleanup call

    return 0;
}
