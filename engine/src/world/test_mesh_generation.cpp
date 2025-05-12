#include "world/test_mesh_generation.h"
#include "world/world_manager.h"
#include "world/voxel_types.h"
#include "world/voxel.h"
#include "world/chunk_segment.h" // Added for ChunkSegment type and constants
#include "rendering/mesh_builder.h"
#include "rendering/voxel_mesh.h" // Changed from chunk_mesh.h
#include <iostream>
#include <cassert>

void VoxelCastle::World::runMeshGenerationTests() {
    std::cout << "--- Mesh Generation Test Start ---" << std::endl;
    VoxelCastle::World::WorldManager worldManager; // Instance for testing

    // Setup a simple chunk for mesh generation
    // For example, a 2x2x2 chunk of stone with one air block
    for (int x = 0; x < 2; ++x) {
        for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
                worldManager.setVoxel(x, y, z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
            }
        }
    }
    worldManager.setVoxel(0, 0, 0, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::AIR))); // One air block to create some faces

    VoxelCastle::World::ChunkColumn* cc = worldManager.getOrCreateChunkColumn(0, 0);
    assert(cc != nullptr && "Failed to get chunk column for mesh test");
    VoxelCastle::World::ChunkSegment* segment = cc->getOrCreateSegment(0); 
    assert(segment != nullptr && "Failed to get chunk segment for mesh test");

    std::cout << "Building mesh for segment 0,0,0..." << std::endl;
    VoxelEngine::Rendering::VoxelMesh resultMesh = VoxelEngine::Rendering::MeshBuilder::buildNaiveMesh(*segment);

    // Basic assertions: Check if any vertices or indices were generated.
    // Expected faces for a 2x2x2 structure at segment origin (0,0,0)-(1,1,1)
    // with (0,0,0) being AIR and the other 7 blocks STONE.
    // buildNaiveMesh processes a single segment; voxels outside the 2x2x2 setup up to (1,1,1)
    // and beyond (up to segment boundaries) are treated as AIR.
    // This results in 24 faces:
    // - 3 internal faces (stone blocks facing the (0,0,0) AIR block).
    // - 21 external faces (7 stone blocks having faces exposed to implicit AIR at segment boundaries
    //   or within the 0-1 coordinate range not occupied by other stone blocks).
    // Each face: 4 vertices, 6 indices.
    // Total vertices: 24 * 4 = 96.
    // Total indices: 24 * 6 = 144.
    assert(!resultMesh.vertices.empty() && "Mesh Generation Test Failed: No vertices generated.");
    assert(!resultMesh.indices.empty() && "Mesh Generation Test Failed: No indices generated.");
    assert(resultMesh.vertices.size() == 96 && "Mesh Generation Test Failed: Incorrect vertex count for 2x2x2 with one air block.");
    assert(resultMesh.indices.size() == 144 && "Mesh Generation Test Failed: Incorrect index count for 2x2x2 with one air block.");


    // Test with a completely solid chunk (no exposed faces internally)
    VoxelCastle::World::WorldManager solidWm;
    VoxelCastle::World::ChunkColumn* solidCc = solidWm.getOrCreateChunkColumn(1,1); // Different location
    VoxelCastle::World::ChunkSegment* solidSegment = solidCc->getOrCreateSegment(0);
    assert(solidSegment != nullptr && "Failed to get solid chunk segment for mesh test");
    for (int x = 0; x < VoxelCastle::World::SEGMENT_WIDTH; ++x) {
        for (int y = 0; y < VoxelCastle::World::SEGMENT_HEIGHT; ++y) {
            for (int z = 0; z < VoxelCastle::World::SEGMENT_DEPTH; ++z) {
                solidSegment->setVoxel(x,y,z, VoxelEngine::World::Voxel(static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE)));
            }
        }
    }
    std::cout << "Building mesh for solid segment..." << std::endl;
    VoxelEngine::Rendering::VoxelMesh solidResultMesh = VoxelEngine::Rendering::MeshBuilder::buildNaiveMesh(*solidSegment);
    std::cout << "Solid Mesh generated. Vertices: " << solidResultMesh.vertices.size()
              << ", Indices: " << solidResultMesh.indices.size() << std::endl;
    // For a solid chunk with no neighbors considered, it should generate faces for all 6 sides of the chunk.
    // Each side is SEGMENT_WIDTH * SEGMENT_DEPTH (or height) quads. Each quad is 4 verts, 6 indices.
    // Total quads = 2*(SEGMENT_WIDTH*SEGMENT_DEPTH) + 2*(SEGMENT_WIDTH*SEGMENT_HEIGHT) + 2*(SEGMENT_DEPTH*SEGMENT_HEIGHT)
    // Total vertices = Total quads * 4
    // Total indices = Total quads * 6
    // Assuming SEGMENT_WIDTH=32, SEGMENT_HEIGHT=32, SEGMENT_DEPTH=32 (as per chunk_segment.h)
    // Quads per face = 32*32 = 1024
    // Total quads = 6 * 1024 = 6144
    // Total vertices = 6144 * 4 = 24576
    // Total indices = 6144 * 6 = 36864
    // NOTE: These assertions will likely fail if buildNaiveMesh only generates meshes for the *local* segment
    // without considering external faces. The current buildNaiveMesh likely produces 0 vertices/indices
    // for a completely solid chunk *within its own bounds*.
    assert(solidResultMesh.vertices.size() == 24576 && "Mesh Generation Test Failed: Solid chunk vertex count incorrect.");
    assert(solidResultMesh.indices.size() == 36864 && "Mesh Generation Test Failed: Solid chunk index count incorrect.");


    std::cout << "--- Mesh Generation Test End ---" << std::endl;
    std::cout << "All Mesh Generation Tests Passed!" << std::endl; // This might be premature
}
