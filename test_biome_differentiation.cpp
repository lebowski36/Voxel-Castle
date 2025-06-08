#include "world/seed_world_generator.h"
#include "world/world_seed.h"
#include "world/world_parameters.h"
#include "world/chunk_segment.h"
#include "world/biome/biome_registry.h"
#include "world/biome/biome_types.h"
#include <iostream>
#include <memory>

using namespace VoxelCastle::World;

int main() {
    std::cout << "=== Biome Differentiation Test ===" << std::endl;
    
    // Initialize biome registry
    BiomeRegistry::initialize();
    
    // Create a seed and parameters
    auto seed = std::make_shared<WorldSeed>(42);
    auto params = std::make_shared<WorldParameters>();
    
    // Create generator in enhanced mode (not legacy compatible)
    auto generator = std::make_unique<SeedWorldGenerator>(seed, params, false);
    
    // Test different biomes at different locations
    std::vector<std::pair<int, int>> testCoords = {
        {0, 0},     // Origin
        {100, 0},   // East 
        {0, 100},   // North
        {100, 100}, // Northeast
        {-100, -100} // Southwest
    };
    
    for (const auto& coord : testCoords) {
        // Get regional data for this location
        RegionalData regionalData = generator->getRegionalData(coord.first, coord.second);
        
        // Generate a small segment to see what materials are used
        ChunkSegment segment;
        int chunkX = coord.first / ChunkSegment::CHUNK_WIDTH;
        int chunkZ = coord.second / ChunkSegment::CHUNK_DEPTH;
        
        generator->generateChunkSegment(segment, chunkX, 0, chunkZ);
        
        // Sample some voxels to see material types
        auto surfaceVoxel = segment.getVoxel(0, 16, 0); // Should be above terrain typically
        auto groundVoxel = segment.getVoxel(0, 8, 0);   // Should be in ground typically
        
        std::cout << "Location (" << coord.first << ", " << coord.second << "):" << std::endl;
        std::cout << "  Biome: " << static_cast<int>(regionalData.primaryBiome) << std::endl;
        std::cout << "  Biome name: ";
        switch (regionalData.primaryBiome) {
            case BiomeType::PLAINS: std::cout << "PLAINS"; break;
            case BiomeType::FOREST: std::cout << "FOREST"; break;
            case BiomeType::DESERT: std::cout << "DESERT"; break;
            case BiomeType::MOUNTAINS: std::cout << "MOUNTAINS"; break;
            case BiomeType::OCEAN: std::cout << "OCEAN"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << std::endl;
        
        // Get biome data to show expected materials
        const BiomeData& biomeData = BiomeRegistry::getBiomeData(regionalData.primaryBiome);
        std::cout << "  Expected surface: " << static_cast<int>(biomeData.surfaceBlock) << std::endl;
        std::cout << "  Expected subsurface: " << static_cast<int>(biomeData.subsurfaceBlock) << std::endl;
        std::cout << "  Sampled surface voxel: " << static_cast<int>(surfaceVoxel.id) << std::endl;
        std::cout << "  Sampled ground voxel: " << static_cast<int>(groundVoxel.id) << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}
