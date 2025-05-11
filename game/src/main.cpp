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
    // Initialize Flecs world
    flecs::world ecs;

    // Register systems
    VoxelEngine::ecs::MovementSystem movementSystem(ecs);
    // Future systems will be registered here

    // Initialize World Manager
    VoxelCastle::World::WorldManager worldManager; // Corrected namespace

    // --- Basic Voxel/Chunk Testing ---
    std::cout << "--- Voxel System Test Start ---" << std::endl;

    // Define some coordinates for testing (as individual components)
    int_fast64_t p1x = 0, p1y = 0, p1z = 0;
    int_fast64_t p2x = 10, p2y = 5, p2z = 20;
    int_fast64_t p3x = 31, p3y = 31, p3z = 31; // Edge of a segment
    int_fast64_t p4x = 32, p4y = 32, p4z = 32; // Next segment

    // Set some voxels
    std::cout << "Setting voxels..." << std::endl;
    worldManager.setVoxel(p1x, p1y, p1z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    worldManager.setVoxel(p2x, p2y, p2z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT)));
    worldManager.setVoxel(p3x, p3y, p3z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS)));
    worldManager.setVoxel(p4x, p4y, p4z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR)));

    // Get and print voxel types
    std::cout << "Retrieving voxels..." << std::endl;
    VoxelEngine::World::Voxel voxel1 = worldManager.getVoxel(p1x, p1y, p1z);
    VoxelEngine::World::Voxel voxel2 = worldManager.getVoxel(p2x, p2y, p2z);
    VoxelEngine::World::Voxel voxel3 = worldManager.getVoxel(p3x, p3y, p3z);
    VoxelEngine::World::Voxel voxel4 = worldManager.getVoxel(p4x, p4y, p4z);

    std::cout << "Voxel at (" << p1x << "," << p1y << "," << p1z << "): Type ID " << static_cast<int>(voxel1.id) << " (Expected STONE: " << static_cast<int>(VoxelEngine::World::VoxelType::STONE) << ")" << std::endl; // Corrected to .id
    std::cout << "Voxel at (" << p2x << "," << p2y << "," << p2z << "): Type ID " << static_cast<int>(voxel2.id) << " (Expected DIRT: " << static_cast<int>(VoxelEngine::World::VoxelType::DIRT) << ")" << std::endl; // Corrected to .id
    std::cout << "Voxel at (" << p3x << "," << p3y << "," << p3z << "): Type ID " << static_cast<int>(voxel3.id) << " (Expected GRASS: " << static_cast<int>(VoxelEngine::World::VoxelType::GRASS) << ")" << std::endl; // Corrected to .id
    std::cout << "Voxel at (" << p4x << "," << p4y << "," << p4z << "): Type ID " << static_cast<int>(voxel4.id) << " (Expected AIR: " << static_cast<int>(VoxelEngine::World::VoxelType::AIR) << ")" << std::endl; // Corrected to .id

    // Test getting a voxel from a non-existent chunk (should be AIR by default)
    int_fast64_t p_non_existent_x = 1000, p_non_existent_y = 1000, p_non_existent_z = 1000;
    VoxelEngine::World::Voxel voxel_non_existent = worldManager.getVoxel(p_non_existent_x, p_non_existent_y, p_non_existent_z);
    std::cout << "Voxel at (" << p_non_existent_x << "," << p_non_existent_y << "," << p_non_existent_z << "): Type ID " << static_cast<int>(voxel_non_existent.id) << " (Expected AIR: " << static_cast<int>(VoxelEngine::World::VoxelType::AIR) << ")" << std::endl; // Corrected to .id

    std::cout << "--- Voxel System Test End ---" << std::endl;
    // --- End Basic Voxel/Chunk Testing ---

    // --- Mesh Generation Test ---
    std::cout << "\n--- Mesh Generation Test Start ---" << std::endl;
    {
        VoxelCastle::World::ChunkSegment testSegment;
        // Place a single stone block at local coordinates (1,1,1) within the segment
        std::cout << "Setting a single STONE voxel at (1,1,1) in test segment." << std::endl;
        testSegment.setVoxel(1, 1, 1, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));

        std::cout << "Building naive mesh for the test segment..." << std::endl;
        VoxelEngine::Rendering::VoxelMesh generatedMesh = VoxelEngine::Rendering::MeshBuilder::buildNaiveMesh(testSegment);

        std::cout << "Mesh generated:" << std::endl;
        std::cout << "  Vertices: " << generatedMesh.vertices.size() << std::endl;
        std::cout << "  Indices: " << generatedMesh.indices.size() << std::endl;
        std::cout << "  Triangles: " << generatedMesh.indices.size() / 3 << std::endl;

        // Expected for a single block: 24 vertices, 36 indices (12 triangles)
        if (generatedMesh.vertices.size() == 24 && generatedMesh.indices.size() == 36) {
            std::cout << "  Result: PASSED (matches expected counts for a single block)" << std::endl;
        } else {
            std::cout << "  Result: FAILED (expected 24 vertices and 36 indices for a single block)" << std::endl;
        }

        // Test with a 2x1x1 line of blocks
        VoxelCastle::World::ChunkSegment lineSegment;
        std::cout << "\nSetting a 2x1x1 line of DIRT voxels at (1,1,1) and (2,1,1) in new test segment." << std::endl;
        lineSegment.setVoxel(1, 1, 1, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT)));
        lineSegment.setVoxel(2, 1, 1, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT)));
        
        std::cout << "Building naive mesh for the line segment..." << std::endl;
        VoxelEngine::Rendering::VoxelMesh lineMesh = VoxelEngine::Rendering::MeshBuilder::buildNaiveMesh(lineSegment);

        std::cout << "Line Mesh generated:" << std::endl;
        std::cout << "  Vertices: " << lineMesh.vertices.size() << std::endl;
        std::cout << "  Indices: " << lineMesh.indices.size() << std::endl;
        std::cout << "  Triangles: " << lineMesh.indices.size() / 3 << std::endl;
        // Expected for 2 adjacent blocks: 10 faces * 4 vertices/face = 40 vertices. 10 faces * 2 triangles/face * 3 indices/triangle = 60 indices (20 triangles)
        if (lineMesh.vertices.size() == 40 && lineMesh.indices.size() == 60) {
            std::cout << "  Result: PASSED (matches expected counts for a 2x1x1 line of blocks)" << std::endl;
        } else {
            std::cout << "  Result: FAILED (expected 40 vertices and 60 indices for a 2x1x1 line)" << std::endl;
        }

        // --- Greedy Meshing Tests ---
        std::cout << "\n--- Greedy Meshing Tests Start ---" << std::endl;

        // Test 1: Single Block (same as naive test segment)
        std::cout << "\nTest 1: Single STONE voxel at (1,1,1)" << std::endl;
        VoxelEngine::Rendering::VoxelMesh greedyMeshSingle = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(testSegment);
        std::cout << "Greedy Mesh (Single Block):" << std::endl;
        std::cout << "  Vertices: " << greedyMeshSingle.vertices.size() << std::endl;
        std::cout << "  Indices: " << greedyMeshSingle.indices.size() << std::endl;
        std::cout << "  Triangles: " << greedyMeshSingle.indices.size() / 3 << std::endl;
        // Expected for a single block (Greedy should be same as Naive): 6 faces * 4 vertices/face = 24 vertices. 6 faces * 2 triangles/face * 3 indices/triangle = 36 indices (12 triangles)
        if (greedyMeshSingle.vertices.size() == 24 && greedyMeshSingle.indices.size() == 36) {
            std::cout << "  Result: PASSED" << std::endl;
        } else {
            std::cout << "  Result: FAILED (Expected 24 vertices, 36 indices)" << std::endl;
        }

        // Test 2: 2x1x1 Line of Blocks (same as naive lineSegment)
        std::cout << "\nTest 2: 2x1x1 line of DIRT voxels at (1,1,1) and (2,1,1)" << std::endl;
        VoxelEngine::Rendering::VoxelMesh greedyMeshLine = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(lineSegment);
        std::cout << "Greedy Mesh (2x1x1 Line):" << std::endl;
        std::cout << "  Vertices: " << greedyMeshLine.vertices.size() << std::endl;
        std::cout << "  Indices: " << greedyMeshLine.indices.size() << std::endl;
        std::cout << "  Triangles: " << greedyMeshLine.indices.size() / 3 << std::endl;
        // Expected for 2x1x1 line:
        // Top/Bottom faces: 2 quads of 2x1 -> 2 * 4 = 8 vert
        // Front/Back faces: 2 quads of 2x1 -> 2 * 4 = 8 vert
        // Left/Right faces: 2 quads of 1x1 -> 2 * 4 = 8 vert
        // Total faces = 6. Total Vertices = 6 * 4 = 24. Total Indices = 6 * 6 = 36.
        // This is because the internal face is culled, and the adjacent faces are merged.
        // It should form one 2x1x1 cuboid.
        if (greedyMeshLine.vertices.size() == 24 && greedyMeshLine.indices.size() == 36) {
            std::cout << "  Result: PASSED" << std::endl;
        } else {
            std::cout << "  Result: FAILED (Expected 24 vertices, 36 indices for a merged 2x1x1 cuboid)" << std::endl;
        }

        // Test 3: 2x2x1 Flat Plane
        VoxelCastle::World::ChunkSegment planeSegment;
        std::cout << "\nTest 3: 2x2x1 GRASS plane at y=1" << std::endl;
        planeSegment.setVoxel(0, 1, 0, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS)));
        planeSegment.setVoxel(1, 1, 0, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS)));
        planeSegment.setVoxel(0, 1, 1, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS)));
        planeSegment.setVoxel(1, 1, 1, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS)));

        VoxelEngine::Rendering::VoxelMesh naiveMeshPlane = VoxelEngine::Rendering::MeshBuilder::buildNaiveMesh(planeSegment);
        std::cout << "Naive Mesh (2x2x1 Plane): Vertices: " << naiveMeshPlane.vertices.size() << ", Indices: " << naiveMeshPlane.indices.size() << std::endl;

        VoxelEngine::Rendering::VoxelMesh greedyMeshPlane = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(planeSegment);
        std::cout << "Greedy Mesh (2x2x1 Plane):" << std::endl;
        std::cout << "  Vertices: " << greedyMeshPlane.vertices.size() << std::endl;
        std::cout << "  Indices: " << greedyMeshPlane.indices.size() << std::endl;
        std::cout << "  Triangles: " << greedyMeshPlane.indices.size() / 3 << std::endl;
        // Expected for 2x2x1 plane (4 blocks):
        // Top face: 1 quad of 2x2 -> 4 vert
        // Bottom face: 1 quad of 2x2 -> 4 vert
        // Side faces: 4 quads of 2x1 -> 4 * 4 = 16 vert
        // Total faces = 1 (top) + 1 (bottom) + 4 (sides) = 6 faces.
        // Total Vertices = 6 * 4 = 24. Total Indices = 6 * 6 = 36.
        if (greedyMeshPlane.vertices.size() == 24 && greedyMeshPlane.indices.size() == 36) {
            std::cout << "  Result: PASSED" << std::endl;
        } else {
            std::cout << "  Result: FAILED (Expected 24 vertices, 36 indices for a 2x2x1 merged plane)" << std::endl;
        }

        // Test 4: 2x2x2 Cube
        VoxelCastle::World::ChunkSegment cubeSegment;
        std::cout << "\nTest 4: 2x2x2 STONE cube" << std::endl;
        for (int x = 0; x < 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                for (int z = 0; z < 2; ++z) {
                    cubeSegment.setVoxel(x, y, z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
                    // Log voxel initialization
                    auto voxel = cubeSegment.getVoxel(x, y, z);
                    std::cout << "Initialized voxel at (" << x << ", " << y << ", " << z << ") with ID: " << static_cast<int>(voxel.id) << std::endl;
                }
            }
        }
        VoxelEngine::Rendering::VoxelMesh naiveMeshCube = VoxelEngine::Rendering::MeshBuilder::buildNaiveMesh(cubeSegment);
        std::cout << "Naive Mesh (2x2x2 Cube): Vertices: " << naiveMeshCube.vertices.size() << ", Indices: " << naiveMeshCube.indices.size() << std::endl;

        VoxelEngine::Rendering::VoxelMesh greedyMeshCube = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(cubeSegment);
        std::cout << "Greedy Mesh (2x2x2 Cube):" << std::endl;
        std::cout << "  Vertices: " << greedyMeshCube.vertices.size() << std::endl;
        std::cout << "  Indices: " << greedyMeshCube.indices.size() << std::endl;
        std::cout << "  Triangles: " << greedyMeshCube.indices.size() / 3 << std::endl;
        // Expected for 2x2x2 cube (8 blocks):
        // Each of the 6 faces of the cube will be a single 2x2 quad.
        // Total faces = 6. Total Vertices = 6 * 4 = 24. Total Indices = 6 * 6 = 36.
        if (greedyMeshCube.vertices.size() == 24 && greedyMeshCube.indices.size() == 36) {
            std::cout << "  Result: PASSED" << std::endl;
        } else {
            std::cout << "  Result: FAILED (Expected 24 vertices, 36 indices for a 2x2x2 merged cube)" << std::endl;
        }
        std::cout << "--- Greedy Meshing Tests End ---" << std::endl;


    }
    std::cout << "--- Mesh Generation Test End ---" << std::endl;

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
    // Set wireframe mode for debugging
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // --- Mesh Rendering Setup ---
    // Build a test mesh (2x2x2 cube)
    VoxelCastle::World::ChunkSegment cubeSegment;
    for (int x = 0; x < 2; ++x) {
        for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
                cubeSegment.setVoxel(x, y, z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
            }
        }
    }
    VoxelEngine::Rendering::VoxelMesh greedyMeshCube = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(cubeSegment);

    // MeshRenderer setup
    VoxelEngine::Rendering::MeshRenderer meshRenderer;
    meshRenderer.uploadMesh(greedyMeshCube);

    // Camera setup
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(4, 4, 8), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0));
    float aspect = static_cast<float>(gameWindow.getWidth()) / static_cast<float>(gameWindow.getHeight());
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    // Print first few mesh vertex positions for debug
    std::cout << "First 8 mesh vertex positions:" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(8, greedyMeshCube.vertices.size()); ++i) {
        const auto& v = greedyMeshCube.vertices[i];
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
