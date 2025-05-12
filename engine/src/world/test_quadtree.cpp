#include "world/test_quadtree.h"
#include "world/world_manager.h"
#include "world/chunk_column.h"
#include "world/quadtree.h" // For AABB2D
#include <iostream>
#include <vector>
#include <cassert> // For assert

// Helper to check if a specific column is in a list of columns
bool containsColumn(const std::vector<VoxelCastle::World::ChunkColumn*>& columns, VoxelCastle::World::ChunkColumn* target) {
    for (const auto* col : columns) {
        if (col == target) {
            return true;
        }
    }
    return false;
}

void VoxelCastle::World::runQuadtreeTests() {
    std::cout << "Running Quadtree Integration Tests..." << std::endl;
    WorldManager wm;

    // Test 1: Basic insertion and query
    std::cout << "  Test 1: Basic insertion and single column query..." << std::endl;
    ChunkColumn* c1 = wm.getOrCreateChunkColumn(0, 0);
    assert(c1 != nullptr && "Failed to create c1");
    assert(c1->getBaseX() == 0 && c1->getBaseZ() == 0 && "c1 coordinates incorrect");

    world::AABB2D region1{ -5, -5, 5, 5 }; // xMin, zMin, xMax, zMax
    std::vector<ChunkColumn*> results1 = wm.queryChunkColumnsInRegion(region1.xMin, region1.zMin, region1.xMax, region1.zMax);
    assert(results1.size() == 1 && "Test 1 Failed: Expected 1 column");
    assert(results1[0] == c1 && "Test 1 Failed: Incorrect column returned");
    std::cout << "    Test 1 Passed." << std::endl;

    // Test 2: Multiple insertions and query
    std::cout << "  Test 2: Multiple insertions and query..." << std::endl;
    ChunkColumn* c2 = wm.getOrCreateChunkColumn(32, 0); // Assuming SEGMENT_WIDTH/DEPTH is 16, this is 2 chunks away
    ChunkColumn* c3 = wm.getOrCreateChunkColumn(0, 32);
    assert(c2 != nullptr && "Failed to create c2");
    assert(c3 != nullptr && "Failed to create c3");

    world::AABB2D region2{ -5, -5, 40, 5 }; // Should pick up c1 and c2
    std::vector<ChunkColumn*> results2 = wm.queryChunkColumnsInRegion(region2.xMin, region2.zMin, region2.xMax, region2.zMax);
    assert(results2.size() == 2 && "Test 2 Failed: Expected 2 columns");
    assert(containsColumn(results2, c1) && "Test 2 Failed: c1 missing");
    assert(containsColumn(results2, c2) && "Test 2 Failed: c2 missing");
    std::cout << "    Test 2 Passed." << std::endl;

    // Test 3: Query for a non-existent column in an area
    std::cout << "  Test 3: Query empty region..." << std::endl;
    world::AABB2D region3{ 100, 100, 110, 110 };
    std::vector<ChunkColumn*> results3 = wm.queryChunkColumnsInRegion(region3.xMin, region3.zMin, region3.xMax, region3.zMax);
    assert(results3.empty() && "Test 3 Failed: Expected 0 columns");
    std::cout << "    Test 3 Passed." << std::endl;

    // Test 4: Query encompassing all created columns
    std::cout << "  Test 4: Query encompassing all columns..." << std::endl;
    ChunkColumn* c4 = wm.getOrCreateChunkColumn(-32, -32);
    assert(c4 != nullptr && "Failed to create c4");
    world::AABB2D region4{ -40, -40, 40, 40 };
    std::vector<ChunkColumn*> results4 = wm.queryChunkColumnsInRegion(region4.xMin, region4.zMin, region4.xMax, region4.zMax);
    // Expect c1, c2, c3, c4
    assert(results4.size() == 4 && "Test 4 Failed: Expected 4 columns");
    assert(containsColumn(results4, c1) && "Test 4 Failed: c1 missing");
    assert(containsColumn(results4, c2) && "Test 4 Failed: c2 missing");
    assert(containsColumn(results4, c3) && "Test 4 Failed: c3 missing");
    assert(containsColumn(results4, c4) && "Test 4 Failed: c4 missing");
    std::cout << "    Test 4 Passed." << std::endl;
    
    // Test 5: Query region outside all columns
    std::cout << "  Test 5: Query region completely outside all columns..." << std::endl;
    world::AABB2D region5{ 200, 200, 250, 250 };
    std::vector<ChunkColumn*> results5 = wm.queryChunkColumnsInRegion(region5.xMin, region5.zMin, region5.xMax, region5.zMax);
    assert(results5.empty() && "Test 5 Failed: Expected 0 columns in distant region");
    std::cout << "    Test 5 Passed." << std::endl;

    std::cout << "All Quadtree Integration Tests Passed!" << std::endl;
}
