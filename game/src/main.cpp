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

    }
    std::cout << "--- Mesh Generation Test End ---" << std::endl;
    // --- End Mesh Generation Test ---

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
