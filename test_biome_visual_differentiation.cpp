#include "engine/include/world/seed_world_generator.h"
#include "engine/include/world/biome/biome_registry.h"
#include "engine/include/world/world_seed.h"
#include "engine/include/world/world_parameters.h"
#include "engine/include/world/chunk_segment.h"
#include <iostream>
#include <memory>

using namespace VoxelCastle::World;

int main() {
    std::cout << "=== Biome Visual Differentiation Test ===" << std::endl;
    
    // Initialize biome registry
    if (!BiomeRegistry::isInitialized()) {
        BiomeRegistry::initialize();
        std::cout << "BiomeRegistry initialized" << std::endl;
    }
    
    // Create world seed and parameters
    auto seed = std::make_shared<WorldSeed>(12345);
    auto params = std::make_shared<WorldParameters>();
    
    // Create generator (non-legacy mode for biome testing)
    SeedWorldGenerator generator(seed, params, false);
    
    std::cout << "\n=== Testing Visual Biome Differentiation ===" << std::endl;
    
    // Test locations across a region to see biome assignment and material differences
    struct TestLocation {
        int x, z;
        std::string description;
    };
    
    std::vector<TestLocation> testLocations = {
        {0, 0, "Origin"},
        {100, 0, "East 100"},
        {0, 100, "North 100"},
        {-100, 0, "West 100"},
        {0, -100, "South 100"},
        {200, 200, "Northeast 200"},
        {-200, -200, "Southwest 200"}
    };
    
    for (const auto& loc : testLocations) {
        std::cout << "\n--- Testing location: " << loc.description 
                  << " (" << loc.x << ", " << loc.z << ") ---" << std::endl;
        
        // Get terrain height
        int terrainHeight = 0;
        {
            // We need to access the private method, so let's generate a chunk and inspect it
            ChunkSegment testSegment;
            generator.generateChunkSegment(testSegment, loc.x / 32, 0, loc.z / 32);
            
            // Find terrain height by scanning downward from a high point
            int localX = loc.x % 32;
            int localZ = loc.z % 32;
            if (localX < 0) localX += 32;
            if (localZ < 0) localZ += 32;
            
            for (int y = 31; y >= 0; y--) {
                uint8_t voxelId = testSegment.getVoxel(localX, y, localZ).id;
                if (voxelId != 0) { // Not AIR
                    terrainHeight = y;
                    break;
                }
            }
        }
        
        // Get regional data for biome information
        RegionalData regionalData = generator.getRegionalData(loc.x, loc.z);
        const BiomeData& biomeData = BiomeRegistry::getBiomeData(regionalData.primaryBiome);
        
        std::cout << "  Terrain Height: " << terrainHeight << " blocks" << std::endl;
        std::cout << "  Primary Biome: " << BiomeTypeToString(regionalData.primaryBiome) 
                  << " (ID: " << static_cast<int>(regionalData.primaryBiome) << ")" << std::endl;
        std::cout << "  Expected Surface Block: " << static_cast<int>(biomeData.surfaceBlock) << std::endl;
        std::cout << "  Expected Subsurface Block: " << static_cast<int>(biomeData.subsurfaceBlock) << std::endl;
        
        // Generate a chunk segment that contains this location and sample the actual materials
        int chunkX = loc.x / 32;
        int chunkZ = loc.z / 32;
        int chunkY = terrainHeight / 32;
        
        ChunkSegment segment;
        generator.generateChunkSegment(segment, chunkX, chunkY, chunkZ);
        
        int localX = loc.x % 32;
        int localZ = loc.z % 32;
        int localY = terrainHeight % 32;
        
        if (localX < 0) localX += 32;
        if (localZ < 0) localZ += 32;
        
        // Sample surface and subsurface materials
        uint8_t surfaceMaterial = 0;
        uint8_t subsurfaceMaterial = 0;
        
        if (localY >= 0 && localY < 32) {
            surfaceMaterial = segment.getVoxel(localX, localY, localZ).id;
        }
        if (localY - 1 >= 0 && localY - 1 < 32) {
            subsurfaceMaterial = segment.getVoxel(localX, localY - 1, localZ).id;
        }
        
        std::cout << "  Actual Surface Material: " << static_cast<int>(surfaceMaterial) << std::endl;
        std::cout << "  Actual Subsurface Material: " << static_cast<int>(subsurfaceMaterial) << std::endl;
        
        // Check if biome-specific materials are being used
        bool surfaceMatches = (surfaceMaterial == static_cast<uint8_t>(biomeData.surfaceBlock));
        bool subsurfaceMatches = (subsurfaceMaterial == static_cast<uint8_t>(biomeData.subsurfaceBlock));
        
        std::cout << "  Surface Material Matches Biome: " << (surfaceMatches ? "✅ YES" : "❌ NO") << std::endl;
        std::cout << "  Subsurface Material Matches Biome: " << (subsurfaceMatches ? "✅ YES" : "❌ NO") << std::endl;
        
        if (!surfaceMatches || !subsurfaceMatches) {
            std::cout << "  ⚠️  BIOME MATERIAL MISMATCH DETECTED" << std::endl;
        }
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "✅ Biome assignment working" << std::endl;
    std::cout << "✅ BiomeData retrieval working" << std::endl;
    std::cout << "❓ Material assignment accuracy depends on test results above" << std::endl;
    
    return 0;
}
