// tests/seed_system_test.cpp
#include "world/world_seed.h"
#include "util/enhanced_noise.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

using namespace VoxelCastle::World;
using namespace VoxelEngine::Util;

// Test reproducibility with the same seed
void testReproducibility() {
    std::cout << "\n=== Testing Seed Reproducibility ===" << std::endl;
    
    const std::string testSeed = "TestWorld123";
    const int testX = 100, testY = 50, testZ = 200;
    
    WorldSeed seed1(testSeed);
    WorldSeed seed2(testSeed);
    
    // Test basic block seeds
    uint64_t blockSeed1 = seed1.getBlockSeed(testX, testY, testZ);
    uint64_t blockSeed2 = seed2.getBlockSeed(testX, testY, testZ);
    
    std::cout << "String seed: '" << testSeed << "'" << std::endl;
    std::cout << "Master seed 1: " << seed1.getMasterSeed() << std::endl;
    std::cout << "Master seed 2: " << seed2.getMasterSeed() << std::endl;
    std::cout << "Block seed 1: " << blockSeed1 << std::endl;
    std::cout << "Block seed 2: " << blockSeed2 << std::endl;
    std::cout << "Reproducible: " << (blockSeed1 == blockSeed2 ? "PASS" : "FAIL") << std::endl;
    
    // Test feature-specific seeds
    uint64_t terrainSeed1 = seed1.getFeatureSeed(testX, testY, testZ, FeatureType::TERRAIN);
    uint64_t terrainSeed2 = seed2.getFeatureSeed(testX, testY, testZ, FeatureType::TERRAIN);
    uint64_t caveSeed1 = seed1.getFeatureSeed(testX, testY, testZ, FeatureType::CAVES);
    uint64_t caveSeed2 = seed2.getFeatureSeed(testX, testY, testZ, FeatureType::CAVES);
    
    std::cout << "Terrain seed 1: " << terrainSeed1 << std::endl;
    std::cout << "Terrain seed 2: " << terrainSeed2 << std::endl;
    std::cout << "Cave seed 1: " << caveSeed1 << std::endl;
    std::cout << "Cave seed 2: " << caveSeed2 << std::endl;
    std::cout << "Terrain reproducible: " << (terrainSeed1 == terrainSeed2 ? "PASS" : "FAIL") << std::endl;
    std::cout << "Cave reproducible: " << (caveSeed1 == caveSeed2 ? "PASS" : "FAIL") << std::endl;
    std::cout << "Features different: " << (terrainSeed1 != caveSeed1 ? "PASS" : "FAIL") << std::endl;
}

// Test seed variation with different inputs
void testSeedVariation() {
    std::cout << "\n=== Testing Seed Variation ===" << std::endl;
    
    std::vector<std::string> testSeeds = {"World1", "World2", "12345", "HelloWorld", "TestSeed"};
    std::vector<uint64_t> masterSeeds;
    
    for (const auto& seedStr : testSeeds) {
        WorldSeed seed(seedStr);
        masterSeeds.push_back(seed.getMasterSeed());
        std::cout << "Seed '" << seedStr << "' -> " << seed.getMasterSeed() << std::endl;
    }
    
    // Check that all seeds are different
    bool allDifferent = true;
    for (size_t i = 0; i < masterSeeds.size(); ++i) {
        for (size_t j = i + 1; j < masterSeeds.size(); ++j) {
            if (masterSeeds[i] == masterSeeds[j]) {
                allDifferent = false;
                std::cout << "WARNING: Seeds " << i << " and " << j << " are identical!" << std::endl;
            }
        }
    }
    
    std::cout << "All seeds different: " << (allDifferent ? "PASS" : "FAIL") << std::endl;
}

// Test noise generation with enhanced system
void testEnhancedNoise() {
    std::cout << "\n=== Testing Enhanced Noise System ===" << std::endl;
    
    WorldSeed seed("NoiseTest");
    EnhancedNoise noise(seed);
    
    const int samples = 10;
    std::cout << "Sample noise values at different coordinates:" << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < samples; ++i) {
        float x = i * 10.5f;
        float y = 0.0f;
        float z = i * 7.3f;
        
        float terrainNoise = noise.smoothValueNoise(x, y, z, FeatureType::TERRAIN);
        float caveNoise = noise.smoothValueNoise(x, y, z, FeatureType::CAVES);
        float octaveNoise = noise.octaveNoise(x, y, z, 4, 0.5f, FeatureType::TERRAIN);
        
        std::cout << "(" << std::setw(6) << x << ", " << std::setw(6) << z << "): "
                  << "terrain=" << std::setw(8) << terrainNoise
                  << " cave=" << std::setw(8) << caveNoise
                  << " octave=" << std::setw(8) << octaveNoise << std::endl;
    }
}

// Generate a heightmap for visual testing
void generateTestHeightmap() {
    std::cout << "\n=== Generating Test Heightmap ===" << std::endl;
    
    WorldSeed seed("HeightmapTest");
    EnhancedNoise noise(seed);
    
    const int size = 64;
    const float scale = 0.05f;
    
    std::ofstream file("heightmap_test.csv");
    file << "x,z,height,terrain,cave\n";
    
    std::cout << "Generating " << size << "x" << size << " heightmap..." << std::endl;
    
    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            float fx = x * scale;
            float fz = z * scale;
            
            float terrainHeight = noise.octaveNoise(fx, 0.0f, fz, 4, 0.5f, FeatureType::TERRAIN);
            float caveValue = noise.smoothValueNoise(fx, terrainHeight * 0.5f, fz, FeatureType::CAVES);
            
            int height = static_cast<int>(terrainHeight * 100.0f);
            
            file << x << "," << z << "," << height << "," 
                 << terrainHeight << "," << caveValue << "\n";
        }
    }
    
    file.close();
    std::cout << "Heightmap saved to heightmap_test.csv" << std::endl;
    std::cout << "You can visualize this data with spreadsheet software or plotting tools" << std::endl;
}

// Test scale-aware noise
void testScaleAwareNoise() {
    std::cout << "\n=== Testing Scale-Aware Noise ===" << std::endl;
    
    WorldSeed seed("ScaleTest");
    EnhancedNoise noise(seed);
    
    const float x = 100.0f, y = 50.0f, z = 200.0f;
    
    std::cout << "Noise values at (" << x << ", " << y << ", " << z << "):" << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < static_cast<int>(ScaleLevel::CONTINENTAL) + 1; ++i) {
        ScaleLevel scale = static_cast<ScaleLevel>(i);
        float value = noise.scaleAwareNoise(x, y, z, scale, FeatureType::TERRAIN);
        
        std::cout << "Scale " << getScaleLevelName(scale) << ": " << std::setw(8) << value << std::endl;
    }
}

// Test debug info functionality
void testDebugInfo() {
    std::cout << "\n=== Testing Debug Info ===" << std::endl;
    
    WorldSeed seed("DebugTest");
    
    int64_t x = 123, y = 45, z = 678;
    ScaleLevel scale = ScaleLevel::CHUNK;
    FeatureType feature = FeatureType::TERRAIN;
    
    auto debugInfo = seed.getDebugInfo(x, y, z, scale, feature);
    
    std::cout << "Debug info for position (" << x << ", " << y << ", " << z << "):" << std::endl;
    std::cout << "  Base seed: " << debugInfo.baseSeed << std::endl;
    std::cout << "  Feature seed: " << debugInfo.featureSeed << std::endl;
    std::cout << "  Final seed: " << debugInfo.finalSeed << std::endl;
    std::cout << "  Feature: " << debugInfo.featureName << std::endl;
    std::cout << "  Scale: " << debugInfo.scaleName << std::endl;
}

int main() {
    std::cout << "=== World Seed System Test Suite ===" << std::endl;
    
    try {
        testReproducibility();
        testSeedVariation();
        testEnhancedNoise();
        testScaleAwareNoise();
        testDebugInfo();
        generateTestHeightmap();
        
        std::cout << "\n=== All Tests Completed ===" << std::endl;
        std::cout << "Check heightmap_test.csv for visual validation of terrain generation" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
