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

// Voxel World
#include "world/world_manager.h" // Defines VoxelCastle::World::WorldManager
#include "world/voxel_types.h" // Defines VoxelEngine::World::VoxelType
#include "world/voxel.h"       // Defines VoxelEngine::World::Voxel
#include "world/test_quadtree.h" // Added for Quadtree tests
#include "world/test_voxel_system.h" // Added for Voxel System tests
#include "world/test_mesh_generation.h" // Added for Mesh Generation tests

// Mesh Generation
#include "rendering/mesh_builder.h" // For VoxelEngine::Rendering::MeshBuilder
#include "world/chunk_segment.h"    // For VoxelCastle::World::ChunkSegment
#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h" // Added for TextureAtlas

// GLM Headers - Will be used by other engine systems, keep includes for now if generally useful
#define GLM_FORCE_SILENT_WARNINGS // Optional: To suppress GLM warnings if any
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(int argc, char* argv[]) {
    std::cout << "Starting VoxelFortress Game" << std::endl;

    // Initialize Flecs world
    flecs::world ecs;

    // Register systems
    VoxelEngine::ecs::MovementSystem movementSystem(ecs);
    // Future systems will be registered here

    VoxelCastle::World::WorldManager worldManager; // Keep a single instance for the game

    // Run all tests
    std::cout << "--- Running All Tests ---" << std::endl;
    VoxelCastle::World::runQuadtreeTests();     // Run Quadtree tests
    VoxelCastle::World::runVoxelSystemTests();  // Run Voxel System tests
    VoxelCastle::World::runMeshGenerationTests(); // Run Mesh Generation tests
    std::cout << "--- All Tests Completed ---" << std::endl;

    // Use a larger window size for visibility
    Window gameWindow("Voxel Fortress - Alpha", 1280, 720);

    if (!gameWindow.init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        return -1;
    }

    // --- OpenGL State Setup ---
    // Enable depth testing for correct 3D rendering
    glEnable(GL_DEPTH_TEST);
    // Disable face culling for debug (see all faces)
    glDisable(GL_CULL_FACE);
    // Set filled polygon mode for normal rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // --- Mesh Rendering Setup ---
    // Create a visible ground plane (16x16, y=8) alternating between DIRT and STONE
    VoxelCastle::World::ChunkSegment groundSegment;
    int groundY = 0;
    for (int x = 0; x < VoxelCastle::World::SEGMENT_WIDTH; ++x) {
        for (int z = 0; z < VoxelCastle::World::SEGMENT_DEPTH; ++z) {
            uint8_t type = ((x + z) % 2 == 0)
                ? static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT)
                : static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE);
            groundSegment.setVoxel(x, groundY, z, VoxelEngine::World::Voxel(type));
        }
    }

    VoxelEngine::Rendering::TextureAtlas atlas;
    VoxelEngine::Rendering::VoxelMesh groundMesh = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(groundSegment, atlas);

    // MeshRenderer setup
    VoxelEngine::Rendering::MeshRenderer meshRenderer;
    meshRenderer.uploadMesh(groundMesh);

    // Camera setup
    glm::mat4 model = glm::mat4(1.0f);
    // Camera above and back, looking down at the center of the ground
    glm::mat4 view = glm::lookAt(
        glm::vec3(16, 32, 32), // camera position (x, y, z)
        glm::vec3(16, 0, 16),  // look at center of ground
        glm::vec3(0, 1, 0));   // up
    float aspect = static_cast<float>(gameWindow.getWidth()) / static_cast<float>(gameWindow.getHeight());
    glm::mat4 proj = glm::perspective(glm::radians(35.0f), aspect, 0.1f, 300.0f);

    // Print first few mesh vertex positions for debug
    std::cout << "First 8 mesh vertex positions:" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(8, groundMesh.vertices.size()); ++i) {
        const auto& v = groundMesh.vertices[i];
        std::cout << "  [" << i << "] (" << v.position.x << ", " << v.position.y << ", " << v.position.z << ")" << std::endl;
    }

    // Set clear color to light blue for debug
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    while (gameWindow.isRunning()) {
        gameWindow.handleEvents();

        // ECS: Progress systems
        ecs.progress();

        gameWindow.update();

        // Clear and render mesh
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        meshRenderer.draw(model, view, proj);

        gameWindow.render();
    }

    // cleanUp is called by Window destructor
    return 0;
}
