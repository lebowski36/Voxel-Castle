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

// Game Core
#include "../include/core/game.h" // Corrected include path

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
    std::cout << "Starting VoxelFortress Game - Refactored" << std::endl;

    Game game;

    if (game.initialize()) {
        game.run();
    } else {
        std::cerr << "Game failed to initialize. Exiting." << std::endl;
        // Potentially return an error code or perform minimal cleanup if necessary
        // before game.shutdown() is called (or if it's not safe to call).
        // For now, we assume shutdown can still be called.
    }

    game.shutdown();

    std::cout << "VoxelFortress Game Ended Gracefully." << std::endl;
    return 0;


    // All legacy and prototype code removed. The main game loop is now fully modularized in Game.
}
