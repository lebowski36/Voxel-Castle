#include "world/seed_world_generator.h"
#include "world/world_seed.h"
#include "world/world_parameters.h"
#include <iostream>
#include <memory>

using namespace VoxelCastle::World;

int main() {
    std::cout << "=== Testing Tectonic Integration with SeedWorldGenerator ===" << std::endl;
    
    try {
        // Create world seed and parameters
        auto seed = std::make_shared<WorldSeed>(12345);
        auto params = std::make_shared<WorldParameters>();
        
        // Create generator
        SeedWorldGenerator generator(seed, params);
        
        // Initialize tectonic simulation
        std::cout << "Initializing tectonic simulation..." << std::endl;
        generator.initializeTectonicSimulation(512.0f); // 512km x 512km world
        
        // Generate some regional data to test integration
        std::cout << "Generating regional data..." << std::endl;
        for (int x = -2; x <= 2; ++x) {
            for (int z = -2; z <= 2; ++z) {
                RegionalData data = generator.generateRegionalData(x, z);
                
                std::cout << "Region (" << x << ", " << z << "): "
                          << "Biome=" << static_cast<int>(data.primaryBiome)
                          << ", Elevation=" << data.elevation
                          << ", PlateId=" << data.geological.dominantPlateId
                          << ", TectonicStress=" << data.geological.tectonicStress
                          << ", TerrainType=" << static_cast<int>(data.geological.terrainType)
                          << ", CrustalThickness=" << data.geological.crustalThickness << "km"
                          << std::endl;
            }
        }
        
        std::cout << "✅ Tectonic integration test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
