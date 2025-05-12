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
    // Place a 2x2x2 cube of voxels at the center of the chunk for visibility
    VoxelCastle::World::ChunkSegment edgeSegment;
    int centerX = VoxelCastle::World::SEGMENT_WIDTH / 2 - 1;
    int centerY = VoxelCastle::World::SEGMENT_HEIGHT / 2 - 1;
    int centerZ = VoxelCastle::World::SEGMENT_DEPTH / 2 - 1;
    for (int x = centerX; x <= centerX + 1; ++x) {
        for (int y = centerY; y <= centerY + 1; ++y) {
            for (int z = centerZ; z <= centerZ + 1; ++z) {
                edgeSegment.setVoxel(x, y, z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
            }
        }
    }

    // Optionally, add voxels along the 12 edges for a more visible "frame" (uncomment to test):
    // for (int i = 0; i <= maxX; ++i) {
    //     edgeSegment.setVoxel(i, 0, 0, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(i, 0, maxZ, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(i, maxY, 0, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(i, maxY, maxZ, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    // }
    // for (int j = 0; j <= maxY; ++j) {
    //     edgeSegment.setVoxel(0, j, 0, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(maxX, j, 0, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(0, j, maxZ, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(maxX, j, maxZ, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    // }
    // for (int k = 0; k <= maxZ; ++k) {
    //     edgeSegment.setVoxel(0, 0, k, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(maxX, 0, k, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(0, maxY, k, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    //     edgeSegment.setVoxel(maxX, maxY, k, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    // }

    VoxelEngine::Rendering::VoxelMesh greedyMeshEdge = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(edgeSegment);

    // MeshRenderer setup
    VoxelEngine::Rendering::MeshRenderer meshRenderer;
    meshRenderer.uploadMesh(greedyMeshEdge);

    // Camera setup
    glm::mat4 model = glm::mat4(1.0f);
    // Move camera closer and reduce FOV for larger, more prominent blocks
    glm::mat4 view = glm::lookAt(glm::vec3(24, 24, 48), glm::vec3(16, 16, 16), glm::vec3(0, 1, 0));
    float aspect = static_cast<float>(gameWindow.getWidth()) / static_cast<float>(gameWindow.getHeight());
    glm::mat4 proj = glm::perspective(glm::radians(35.0f), aspect, 0.1f, 300.0f);

    // Print first few mesh vertex positions for debug
    std::cout << "First 8 mesh vertex positions:" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(8, greedyMeshEdge.vertices.size()); ++i) {
        const auto& v = greedyMeshEdge.vertices[i];
        std::cout << "  [" << i << "] (" << v.position.x << ", " << v.position.y << ", " << v.position.z << ")" << std::endl;
    }

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
