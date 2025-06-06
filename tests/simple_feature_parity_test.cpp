#include "world/world_generator.h"
#include "world/seed_world_generator.h"
#include "world/world_seed.h"
#include "world/world_parameters.h"
#include "world/chunk_segment.h"
#include <iostream>
#include <memory>
#include <cassert>

using namespace VoxelCastle::World;

int main() {
    std::cout << "=== Feature Parity Test ===" << std::endl;
    
    try {
        // Create test seed and parameters
        auto testSeed = std::make_shared<WorldSeed>(12345ULL);
        auto testParameters = std::make_shared<WorldParameters>(WorldPreset::NORMAL);
        
        // Create generators
        auto legacyGenerator = std::make_unique<WorldGenerator>();
        auto seedGenerator = std::make_unique<SeedWorldGenerator>(testSeed, testParameters, true); // Legacy compatible mode
        
        // Set the same seed for legacy generator
        legacyGenerator->setSeed(12345ULL);
        
        std::cout << "Generators created successfully" << std::endl;
        
        // Generate the same chunk with both generators
        ChunkSegment legacySegment;
        ChunkSegment seedSegment;
        
        const int testWorldX = 0;
        const int testWorldY = 0;
        const int testWorldZ = 0;
        
        std::cout << "Generating legacy chunk..." << std::endl;
        legacyGenerator->generateChunkSegment(legacySegment, testWorldX, testWorldY, testWorldZ);
        
        std::cout << "Generating seed-based chunk..." << std::endl;
        seedGenerator->generateChunkSegment(seedSegment, testWorldX, testWorldY, testWorldZ);
        
        // Compare voxel data
        int differences = 0;
        int totalVoxels = ChunkSegment::CHUNK_WIDTH * ChunkSegment::CHUNK_HEIGHT * ChunkSegment::CHUNK_DEPTH;
        
        std::cout << "Comparing voxel data..." << std::endl;
        
        for (int x = 0; x < ChunkSegment::CHUNK_WIDTH; ++x) {
            for (int y = 0; y < ChunkSegment::CHUNK_HEIGHT; ++y) {
                for (int z = 0; z < ChunkSegment::CHUNK_DEPTH; ++z) {
                    auto legacyVoxel = legacySegment.getVoxel(x, y, z);
                    auto seedVoxel = seedSegment.getVoxel(x, y, z);
                    
                    if (legacyVoxel.id != seedVoxel.id) {
                        differences++;
                        
                        // Log first few differences for debugging
                        if (differences <= 10) {
                            std::cout << "Difference at (" << x << "," << y << "," << z << "): "                         << "Legacy=" << static_cast<int>(legacyVoxel.id)
                           << " Seed=" << static_cast<int>(seedVoxel.id) << std::endl;
                        }
                    }
                }
            }
        }
        
        std::cout << "=== RESULTS ===" << std::endl;
        std::cout << "Total differences: " << differences << " out of " << totalVoxels << " voxels" << std::endl;
        std::cout << "Match percentage: " << (100.0 * (totalVoxels - differences) / totalVoxels) << "%" << std::endl;
        
        if (differences == 0) {
            std::cout << "✅ PERFECT MATCH: Legacy and seed generators produce identical output!" << std::endl;
        } else {
            std::cout << "❌ DIFFERENCES FOUND: Generators do not match exactly" << std::endl;
            std::cout << "   This indicates we need to tune the seed generator parameters" << std::endl;
        }
        
        // Test reproducibility with same seed
        std::cout << "\n=== Reproducibility Test ===" << std::endl;
        auto generator1 = std::make_unique<SeedWorldGenerator>(testSeed, testParameters, false); // Seed variation mode
        auto generator2 = std::make_unique<SeedWorldGenerator>(testSeed, testParameters, false); // Seed variation mode
        
        ChunkSegment segment1;
        ChunkSegment segment2;
        
        generator1->generateChunkSegment(segment1, 0, 0, 0);
        generator2->generateChunkSegment(segment2, 0, 0, 0);
        
        int reproducibilityDifferences = 0;
        for (int x = 0; x < ChunkSegment::CHUNK_WIDTH; ++x) {
            for (int y = 0; y < ChunkSegment::CHUNK_HEIGHT; ++y) {
                for (int z = 0; z < ChunkSegment::CHUNK_DEPTH; ++z) {
                    if (segment1.getVoxel(x, y, z).id != segment2.getVoxel(x, y, z).id) {
                        reproducibilityDifferences++;
                    }
                }
            }
        }
        
        if (reproducibilityDifferences == 0) {
            std::cout << "✅ REPRODUCIBILITY: Same seed produces identical results" << std::endl;
        } else {
            std::cout << "❌ REPRODUCIBILITY FAILED: Same seed produces different results (" 
                      << reproducibilityDifferences << " differences)" << std::endl;
        }
        
        // Test that different seeds produce different results
        std::cout << "\n=== Variation Test ===" << std::endl;
        auto differentSeed = std::make_shared<WorldSeed>(54321ULL);
        auto differentGenerator = std::make_unique<SeedWorldGenerator>(differentSeed, testParameters, false); // Seed variation mode
        
        ChunkSegment differentSegment;
        differentGenerator->generateChunkSegment(differentSegment, 0, 0, 0);
        
        int variationDifferences = 0;
        for (int x = 0; x < ChunkSegment::CHUNK_WIDTH; ++x) {
            for (int y = 0; y < ChunkSegment::CHUNK_HEIGHT; ++y) {
                for (int z = 0; z < ChunkSegment::CHUNK_DEPTH; ++z) {
                    if (segment1.getVoxel(x, y, z).id != differentSegment.getVoxel(x, y, z).id) {
                        variationDifferences++;
                    }
                }
            }
        }
        
        if (variationDifferences > 0) {
            std::cout << "✅ VARIATION: Different seeds produce different results (" 
                      << variationDifferences << " differences)" << std::endl;
        } else {
            std::cout << "❌ VARIATION FAILED: Different seeds produce identical results" << std::endl;
        }
        
        std::cout << "\n=== Feature Parity Test Complete ===" << std::endl;
        
        return (differences == 0 && reproducibilityDifferences == 0 && variationDifferences > 0) ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
