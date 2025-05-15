
#include "core/WorldSetup.h"
#include "core/WorldSetupGlobals.h"
#include "world/world_manager.h"
#include "world/world_generator.h"
#include "world/chunk_column.h"
#include "world/chunk_segment.h"

#include <iostream> // For std::cout, std::cerr

namespace GameWorldSetup {

void initializeStaticWorld(VoxelCastle::World::WorldManager& worldManager, 
                           VoxelCastle::World::WorldGenerator& worldGenerator) {
    std::cout << "GameWorldSetup::initializeStaticWorld() - Generating initial static world area..." << std::endl;

    // This logic will be moved from Game::initializeWorldContent()
    // For now, it's a placeholder.
    // Example structure (to be replaced with actual logic from game.cpp):

    const int initialLoadRadiusInSegments = 1; // Creates a 3x3 area of chunk columns (adjust as needed)
    // Get camera position from a global variable set at startup
    glm::vec3 cameraPos = g_initialCameraPosition;

    int centerSegX = static_cast<int>(std::floor(cameraPos.x / VoxelCastle::World::ChunkSegment::CHUNK_WIDTH));
    int centerSegZ = static_cast<int>(std::floor(cameraPos.z / VoxelCastle::World::ChunkSegment::CHUNK_DEPTH));
    const int minYSegments = 0; // Generate segments at Y=0 and Y=1
    const int maxYSegments = 1;

    for (int sgX = centerSegX - initialLoadRadiusInSegments; sgX <= centerSegX + initialLoadRadiusInSegments; ++sgX) {
        for (int sgZ = centerSegZ - initialLoadRadiusInSegments; sgZ <= centerSegZ + initialLoadRadiusInSegments; ++sgZ) {
            // Ensure the ChunkColumn exists.
            VoxelCastle::World::ChunkColumn* column = worldManager.getOrCreateChunkColumn(sgX, sgZ);
            if (!column) {
                std::cerr << "Failed to get or create chunk column at (" << sgX << ", " << sgZ << ")" << std::endl;
                continue; 
            }
            std::cout << "  Processing Chunk Column (" << sgX << ", " << sgZ << ")" << std::endl;

            for (int sgY = minYSegments; sgY <= maxYSegments; ++sgY) {
                VoxelCastle::World::ChunkSegment* segment = column->getOrCreateSegment(sgY);
                if (segment) {
                    std::cout << "    Generating segment (" << sgX << ", " << sgY << ", " << sgZ << ")" << std::endl;
                    // Note: WorldGenerator::generateChunkSegment is static
                    VoxelCastle::World::WorldGenerator::generateChunkSegment(*segment, sgX, sgY, sgZ);
                    segment->markDirty(true);
                    std::cout << "      Segment (" << sgX << ", " << sgY << ", " << sgZ << ") generated and marked dirty." << std::endl;
                } else {
                    std::cerr << "    Failed to get or create segment at Y index " << sgY << " for column (" << sgX << ", " << sgZ << ")" << std::endl;
                }
            }
        }
    }
    std::cout << "GameWorldSetup::initializeStaticWorld() - Initial static world area generation attempt complete." << std::endl;
}

} // namespace GameWorldSetup
