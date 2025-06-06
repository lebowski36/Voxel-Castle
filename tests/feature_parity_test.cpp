#include <gtest/gtest.h>
#include "world/world_generator.h"
#include "world/seed_world_generator.h"
#include "world/world_seed.h"
#include "world/world_parameters.h"
#include "world/chunk_segment.h"
#include <memory>

using namespace VoxelCastle::World;

class FeatureParityTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test seed and parameters
        testSeed = std::make_shared<WorldSeed>(12345ULL);
        testParameters = std::make_shared<WorldParameters>("starter");
        
        // Create generators
        legacyGenerator = std::make_unique<WorldGenerator>();
        seedGenerator = std::make_unique<SeedWorldGenerator>(testSeed, testParameters);
        
        // Set the same seed for legacy generator
        legacyGenerator->setSeed(12345ULL);
    }

    std::shared_ptr<WorldSeed> testSeed;
    std::shared_ptr<WorldParameters> testParameters;
    std::unique_ptr<WorldGenerator> legacyGenerator;
    std::unique_ptr<SeedWorldGenerator> seedGenerator;
};

TEST_F(FeatureParityTest, SameTerrainHeights) {
    // Generate the same chunk with both generators
    ChunkSegment legacySegment;
    ChunkSegment seedSegment;
    
    const int testWorldX = 0;
    const int testWorldY = 0;
    const int testWorldZ = 0;
    
    legacyGenerator->generateChunkSegment(legacySegment, testWorldX, testWorldY, testWorldZ);
    seedGenerator->generateChunkSegment(seedSegment, testWorldX, testWorldY, testWorldZ);
    
    // Compare voxel data
    bool heightsMatch = true;
    int differences = 0;
    
    for (int x = 0; x < ChunkSegment::CHUNK_WIDTH; ++x) {
        for (int y = 0; y < ChunkSegment::CHUNK_HEIGHT; ++y) {
            for (int z = 0; z < ChunkSegment::CHUNK_DEPTH; ++z) {
                auto legacyVoxel = legacySegment.getVoxel(x, y, z);
                auto seedVoxel = seedSegment.getVoxel(x, y, z);
                
                if (legacyVoxel.getType() != seedVoxel.getType()) {
                    heightsMatch = false;
                    differences++;
                    
                    // Log first few differences for debugging
                    if (differences <= 5) {
                        std::cout << "Difference at (" << x << "," << y << "," << z << "): "
                                  << "Legacy=" << static_cast<int>(legacyVoxel.getType()) 
                                  << " Seed=" << static_cast<int>(seedVoxel.getType()) << std::endl;
                    }
                }
            }
        }
    }
    
    std::cout << "Total differences: " << differences << " out of " 
              << (ChunkSegment::CHUNK_WIDTH * ChunkSegment::CHUNK_HEIGHT * ChunkSegment::CHUNK_DEPTH) 
              << " voxels" << std::endl;
    
    EXPECT_TRUE(heightsMatch) << "Terrain heights should match exactly between legacy and seed generators";
}

TEST_F(FeatureParityTest, SameBlockTypes) {
    // Test block type distribution
    ChunkSegment legacySegment;
    ChunkSegment seedSegment;
    
    legacyGenerator->generateChunkSegment(legacySegment, 0, 0, 0);
    seedGenerator->generateChunkSegment(seedSegment, 0, 0, 0);
    
    // Count block types in both segments
    std::map<uint8_t, int> legacyCounts;
    std::map<uint8_t, int> seedCounts;
    
    for (int x = 0; x < ChunkSegment::CHUNK_WIDTH; ++x) {
        for (int y = 0; y < ChunkSegment::CHUNK_HEIGHT; ++y) {
            for (int z = 0; z < ChunkSegment::CHUNK_DEPTH; ++z) {
                legacyCounts[legacySegment.getVoxel(x, y, z).getType()]++;
                seedCounts[seedSegment.getVoxel(x, y, z).getType()]++;
            }
        }
    }
    
    // Compare block type distributions
    EXPECT_EQ(legacyCounts, seedCounts) << "Block type distributions should match exactly";
}

TEST_F(FeatureParityTest, MultipleChunksConsistent) {
    // Test multiple chunks to ensure consistency
    const int numChunks = 4;
    
    for (int i = 0; i < numChunks; ++i) {
        ChunkSegment legacySegment;
        ChunkSegment seedSegment;
        
        legacyGenerator->generateChunkSegment(legacySegment, i, 0, 0);
        seedGenerator->generateChunkSegment(seedSegment, i, 0, 0);
        
        // Quick comparison - just check a few sample points
        for (int sample = 0; sample < 10; ++sample) {
            int x = sample % ChunkSegment::CHUNK_WIDTH;
            int y = sample % ChunkSegment::CHUNK_HEIGHT;
            int z = sample % ChunkSegment::CHUNK_DEPTH;
            
            auto legacyVoxel = legacySegment.getVoxel(x, y, z);
            auto seedVoxel = seedSegment.getVoxel(x, y, z);
            
            EXPECT_EQ(legacyVoxel.getType(), seedVoxel.getType()) 
                << "Chunk " << i << " sample (" << x << "," << y << "," << z << ") should match";
        }
    }
}

TEST_F(FeatureParityTest, DifferentSeedsProduceDifferentTerrain) {
    // Test that different seeds produce different terrain
    auto seed1 = std::make_shared<WorldSeed>(12345ULL);
    auto seed2 = std::make_shared<WorldSeed>(54321ULL);
    
    auto generator1 = std::make_unique<SeedWorldGenerator>(seed1, testParameters);
    auto generator2 = std::make_unique<SeedWorldGenerator>(seed2, testParameters);
    
    ChunkSegment segment1;
    ChunkSegment segment2;
    
    generator1->generateChunkSegment(segment1, 0, 0, 0);
    generator2->generateChunkSegment(segment2, 0, 0, 0);
    
    // Should have some differences
    bool foundDifference = false;
    for (int x = 0; x < ChunkSegment::CHUNK_WIDTH && !foundDifference; ++x) {
        for (int y = 0; y < ChunkSegment::CHUNK_HEIGHT && !foundDifference; ++y) {
            for (int z = 0; z < ChunkSegment::CHUNK_DEPTH && !foundDifference; ++z) {
                if (segment1.getVoxel(x, y, z).getType() != segment2.getVoxel(x, y, z).getType()) {
                    foundDifference = true;
                }
            }
        }
    }
    
    EXPECT_TRUE(foundDifference) << "Different seeds should produce different terrain";
}

TEST_F(FeatureParityTest, SameSeedReproducesExactTerrain) {
    // Test that the same seed produces identical terrain
    auto generator1 = std::make_unique<SeedWorldGenerator>(testSeed, testParameters);
    auto generator2 = std::make_unique<SeedWorldGenerator>(testSeed, testParameters);
    
    ChunkSegment segment1;
    ChunkSegment segment2;
    
    generator1->generateChunkSegment(segment1, 0, 0, 0);
    generator2->generateChunkSegment(segment2, 0, 0, 0);
    
    // Should be identical
    for (int x = 0; x < ChunkSegment::CHUNK_WIDTH; ++x) {
        for (int y = 0; y < ChunkSegment::CHUNK_HEIGHT; ++y) {
            for (int z = 0; z < ChunkSegment::CHUNK_DEPTH; ++z) {
                EXPECT_EQ(segment1.getVoxel(x, y, z).getType(), segment2.getVoxel(x, y, z).getType())
                    << "Same seed should produce identical voxels at (" << x << "," << y << "," << z << ")";
            }
        }
    }
}
