#include "platform/Window.h" // Adjust path as necessary based on include directories
#include <iostream>

// Flecs - ECS
#include <flecs.h>

// ECS Components
#include "ecs/components/position_component.h"
#include "ecs/components/velocity_component.h"
#include "ecs/components/renderable_component.h"

// ECS Systems
#include "ecs/systems/movement_system.h"

// GLM Headers - Will be used by other engine systems, keep includes for now if generally useful
#define GLM_FORCE_SILENT_WARNINGS // Optional: To suppress GLM warnings if any
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(int argc, char* argv[]) {
    // Initialize Flecs world
    flecs::world ecs;

    // Register systems
    VoxelEngine::ecs::MovementSystem movementSystem(ecs);
    // Future systems will be registered here

    Window gameWindow("Voxel Fortress - Alpha", 800, 600);

    if (!gameWindow.init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        return -1;
    }

    while (gameWindow.isRunning()) {
        gameWindow.handleEvents();

        // ECS: Progress systems
        ecs.progress(); // This runs all registered systems

        gameWindow.update();
        gameWindow.render();
    }

    // cleanUp is called by Window destructor
    return 0;
}
