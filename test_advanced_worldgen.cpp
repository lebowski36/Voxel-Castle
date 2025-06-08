#include <iostream>
#include <fstream>
#include <map>

#include "engine/include/world/seed_world_generator.h"
#include "engine/include/world/biome/biome_registry.h"
#include "engine/include/world/biome/biome_types.h"
#include "engine/include/world/world_coordinates.h"
#include "engine/include/world/world_parameters.h"

using namespace VoxelCastle::World;

int main() {
    try {
        std::cout << "🌍 Advanced World Generation Test\n";
        std::cout << "==================================\n\n";
        
        // Initialize biome registry
        BiomeRegistry::initialize();
        std::cout << "✅ Biome registry initialized\n";
        
        // Create seed and parameters
        auto seed = std::make_shared<WorldSeed>(12345);
        auto params = std::make_shared<WorldParameters>();
        
        std::cout << "🌱 World seed: " << seed->getMasterSeed() << "\n";
        
        // Create seed world generator
        SeedWorldGenerator generator(seed, params);
        std::cout << "✅ World generator created\n\n";
        
        // Test regional data generation
        std::cout << "🗺️  Testing regional data generation...\n";
        std::map<BiomeType, int> biomeCount;
        
        for (int x = -2; x <= 2; x++) {
            for (int z = -2; z <= 2; z++) {
                RegionalData regional = generator.getRegionalData(x * 1000, z * 1000);
                biomeCount[regional.primaryBiome]++;
                
                std::cout << "Region (" << x << "," << z << "): " 
                          << BiomeTypeToString(regional.primaryBiome) 
                          << " | Temp: " << regional.temperature << "°C"
                          << " | Humidity: " << regional.humidity << "%"
                          << " | Elevation: " << regional.elevation << "m\n";
            }
        }
        
        std::cout << "\n📊 BIOME DISTRIBUTION (25 regions):\n";
        std::cout << "===================================\n";
        for (const auto& pair : biomeCount) {
            float percentage = (float)pair.second / 25.0f * 100.0f;
            const BiomeData& data = BiomeRegistry::getBiomeData(pair.first);
            std::cout << "  " << BiomeTypeToString(pair.first) 
                      << ": " << pair.second << " regions (" << percentage << "%)\n";
            std::cout << "    Surface Block: " << (int)data.surfaceBlock 
                      << ", Subsurface Block: " << (int)data.subsurfaceBlock << "\n";
        }
        
        std::cout << "\n🚀 READY FOR ADVANCED FEATURES!\n";
        std::cout << "===============================\n";
        std::cout << "The biome system is working! Next steps:\n";
        std::cout << "1. 🏔️  Implement tectonic plate simulation\n";
        std::cout << "2. 🌊 Add hydraulic erosion simulation\n";
        std::cout << "3. 🌡️  Create climate modeling system\n";
        std::cout << "4. 🗺️  Build regional temperature/precipitation maps\n";
        std::cout << "5. 🎨 Add world generation visualization UI\n";
        std::cout << "6. ⛰️  Implement geological stratification\n";
        std::cout << "7. 🏛️  Add structure and ruin generation\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}
