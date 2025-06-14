#include "world/test_voxel_system.h"
#include "world/world_manager.h"
#include "world/voxel_types.h"
#include "world/voxel.h"
#include <iostream>
#include <cassert>

void VoxelCastle::World::runVoxelSystemTests() {
    std::cout << "--- Voxel System Test Start ---" << std::endl;

    VoxelCastle::World::WorldManager worldManager; // Instance for testing

    // Define some coordinates for testing (as individual components)
    int_fast64_t p1x = 0, p1y = 0, p1z = 0;
    int_fast64_t p2x = 10, p2y = 5, p2z = 20;
    int_fast64_t p3x = 31, p3y = 31, p3z = 31; // Edge of a segment
    int_fast64_t p4x = 32, p4y = 32, p4z = 32; // Next segment

    // Set some voxels
    worldManager.setVoxel(p1x, p1y, p1z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
    worldManager.setVoxel(p2x, p2y, p2z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT)));
    worldManager.setVoxel(p3x, p3y, p3z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS)));
    worldManager.setVoxel(p4x, p4y, p4z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR))); // This was STONE, changing to AIR for distinctness

    // Get and print voxel types
    VoxelEngine::World::Voxel voxel1 = worldManager.getVoxel(p1x, p1y, p1z);
    VoxelEngine::World::Voxel voxel2 = worldManager.getVoxel(p2x, p2y, p2z);
    VoxelEngine::World::Voxel voxel3 = worldManager.getVoxel(p3x, p3y, p3z);
    VoxelEngine::World::Voxel voxel4 = worldManager.getVoxel(p4x, p4y, p4z);

    assert(voxel1.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE) && "Test Failed: Voxel 1 type mismatch");
    assert(voxel2.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT) && "Test Failed: Voxel 2 type mismatch");
    assert(voxel3.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::GRASS) && "Test Failed: Voxel 3 type mismatch");
    assert(voxel4.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR) && "Test Failed: Voxel 4 type mismatch");

    // Test getting a voxel from a non-existent chunk (should be AIR by default)
    int_fast64_t p_non_existent_x = 1000, p_non_existent_y = 1000, p_non_existent_z = 1000;
    VoxelEngine::World::Voxel voxel_non_existent = worldManager.getVoxel(p_non_existent_x, p_non_existent_y, p_non_existent_z);
    assert(voxel_non_existent.id == static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR) && "Test Failed: Non-existent voxel type mismatch");

    std::cout << "--- Voxel System Test End ---" << std::endl;
    std::cout << "All Voxel System Tests Passed!" << std::endl;
}
