#include <iostream>
#include <cassert>
#include <filesystem>
#include <vector>
#include <cmath>
#include "world/regional_data.h"

using namespace VoxelCastle::World;

// Test helper functions
bool FloatEquals(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}

void TestRegionalDataCreation() {
    std::cout << "Testing RegionalData creation..." << std::endl;
    
    RegionalData data;
    data.regionX = 10;
    data.regionY = 20;
    data.averageElevation = 150.5f;
    data.minElevation = 50.0f;
    data.maxElevation = 300.0f;
    data.temperature = 18.5f;
    data.precipitation = 800.0f;
    data.humidity = 0.65f;
    data.biomeType = BiomeType::TEMPERATE_FOREST;
    
    assert(data.regionX == 10);
    assert(data.regionY == 20);
    assert(FloatEquals(data.averageElevation, 150.5f));
    assert(FloatEquals(data.temperature, 18.5f));
    assert(data.biomeType == BiomeType::TEMPERATE_FOREST);
    
    std::cout << "✓ RegionalData creation test passed" << std::endl;
}

void TestRegionalDataSerialization() {
    std::cout << "Testing RegionalData serialization..." << std::endl;
    
    // Create test data
    RegionalData originalData;
    originalData.regionX = 42;
    originalData.regionY = 84;
    originalData.averageElevation = 275.25f;
    originalData.minElevation = 100.0f;
    originalData.maxElevation = 450.0f;
    originalData.temperature = 22.3f;
    originalData.precipitation = 1200.5f;
    originalData.humidity = 0.78f;
    originalData.biomeType = BiomeType::TROPICAL_RAINFOREST;
    
    // Test serialization
    std::vector<uint8_t> serializedData = originalData.Serialize();
    assert(!serializedData.empty());
    
    // Test deserialization
    RegionalData deserializedData;
    bool deserializeSuccess = deserializedData.Deserialize(serializedData);
    assert(deserializeSuccess);
    
    // Verify data integrity
    assert(deserializedData.regionX == originalData.regionX);
    assert(deserializedData.regionY == originalData.regionY);
    assert(FloatEquals(deserializedData.averageElevation, originalData.averageElevation));
    assert(FloatEquals(deserializedData.minElevation, originalData.minElevation));
    assert(FloatEquals(deserializedData.maxElevation, originalData.maxElevation));
    assert(FloatEquals(deserializedData.temperature, originalData.temperature));
    assert(FloatEquals(deserializedData.precipitation, originalData.precipitation));
    assert(FloatEquals(deserializedData.humidity, originalData.humidity));
    assert(deserializedData.biomeType == originalData.biomeType);
    
    std::cout << "✓ RegionalData serialization test passed" << std::endl;
}

void TestRegionalDataValidation() {
    std::cout << "Testing RegionalData validation..." << std::endl;
    
    // Test valid data
    RegionalData validData;
    validData.regionX = 0;
    validData.regionY = 0;
    validData.averageElevation = 100.0f;
    validData.minElevation = 50.0f;
    validData.maxElevation = 200.0f;
    validData.temperature = 15.0f;
    validData.precipitation = 500.0f;
    validData.humidity = 0.5f;
    validData.biomeType = BiomeType::GRASSLAND;
    
    assert(validData.IsValid());
    
    // Test invalid data - min > max elevation
    RegionalData invalidData1 = validData;
    invalidData1.minElevation = 300.0f;
    invalidData1.maxElevation = 200.0f;
    assert(!invalidData1.IsValid());
    
    // Test invalid data - average outside min/max range
    RegionalData invalidData2 = validData;
    invalidData2.averageElevation = 25.0f; // Below minElevation
    assert(!invalidData2.IsValid());
    
    // Test invalid data - humidity out of range
    RegionalData invalidData3 = validData;
    invalidData3.humidity = 1.5f; // Above 1.0
    assert(!invalidData3.IsValid());
    
    std::cout << "✓ RegionalData validation test passed" << std::endl;
}

void TestRegionalDatabaseFileOperations() {
    std::cout << "Testing RegionalDatabase file operations..." << std::endl;
    
    const std::string testDir = "test_regional_data";
    const std::string testWorldName = "test_world";
    
    // Clean up any existing test data
    if (std::filesystem::exists(testDir)) {
        std::filesystem::remove_all(testDir);
    }
    
    // Create test database
    RegionalDatabase database(testDir);
    
    // Create test regional data
    std::vector<RegionalData> testRegions;
    
    // Region (0, 0) - Grassland
    RegionalData region1;
    region1.regionX = 0;
    region1.regionY = 0;
    region1.averageElevation = 120.0f;
    region1.minElevation = 100.0f;
    region1.maxElevation = 150.0f;
    region1.temperature = 20.0f;
    region1.precipitation = 600.0f;
    region1.humidity = 0.6f;
    region1.biomeType = BiomeType::GRASSLAND;
    testRegions.push_back(region1);
    
    // Region (1, 0) - Forest
    RegionalData region2;
    region2.regionX = 1;
    region2.regionY = 0;
    region2.averageElevation = 180.0f;
    region2.minElevation = 150.0f;
    region2.maxElevation = 250.0f;
    region2.temperature = 18.0f;
    region2.precipitation = 800.0f;
    region2.humidity = 0.75f;
    region2.biomeType = BiomeType::TEMPERATE_FOREST;
    testRegions.push_back(region2);
    
    // Region (0, 1) - Desert
    RegionalData region3;
    region3.regionX = 0;
    region3.regionY = 1;
    region3.averageElevation = 80.0f;
    region3.minElevation = 60.0f;
    region3.maxElevation = 120.0f;
    region3.temperature = 35.0f;
    region3.precipitation = 100.0f;
    region3.humidity = 0.2f;
    region3.biomeType = BiomeType::DESERT;
    testRegions.push_back(region3);
    
    // Save all regions
    for (const auto& region : testRegions) {
        bool saveSuccess = database.SaveRegionalData(testWorldName, region);
        assert(saveSuccess);
    }
    
    std::cout << "✓ Saved " << testRegions.size() << " regions to database" << std::endl;
    
    // Load and verify each region
    for (const auto& originalRegion : testRegions) {
        RegionalData loadedRegion;
        bool loadSuccess = database.LoadRegionalData(testWorldName, originalRegion.regionX, originalRegion.regionY, loadedRegion);
        assert(loadSuccess);
        
        // Verify data integrity
        assert(loadedRegion.regionX == originalRegion.regionX);
        assert(loadedRegion.regionY == originalRegion.regionY);
        assert(FloatEquals(loadedRegion.averageElevation, originalRegion.averageElevation));
        assert(FloatEquals(loadedRegion.minElevation, originalRegion.minElevation));
        assert(FloatEquals(loadedRegion.maxElevation, originalRegion.maxElevation));
        assert(FloatEquals(loadedRegion.temperature, originalRegion.temperature));
        assert(FloatEquals(loadedRegion.precipitation, originalRegion.precipitation));
        assert(FloatEquals(loadedRegion.humidity, originalRegion.humidity));
        assert(loadedRegion.biomeType == originalRegion.biomeType);
        assert(loadedRegion.IsValid());
    }
    
    std::cout << "✓ Loaded and verified " << testRegions.size() << " regions from database" << std::endl;
    
    // Test loading non-existent region
    RegionalData nonExistentRegion;
    bool loadNonExistent = database.LoadRegionalData(testWorldName, 999, 999, nonExistentRegion);
    assert(!loadNonExistent);
    
    std::cout << "✓ Non-existent region load correctly failed" << std::endl;
    
    // Test GetRegionFilePath
    std::string expectedPath = testDir + "/test_world/region_0_0.dat";
    std::string actualPath = database.GetRegionFilePath(testWorldName, 0, 0);
    assert(actualPath == expectedPath);
    
    std::cout << "✓ Region file path generation test passed" << std::endl;
    
    // Clean up test data
    std::filesystem::remove_all(testDir);
    
    std::cout << "✓ RegionalDatabase file operations test passed" << std::endl;
}

void TestRegionalDatabaseBoundaryConditions() {
    std::cout << "Testing RegionalDatabase boundary conditions..." << std::endl;
    
    const std::string testDir = "test_boundary_data";
    const std::string testWorldName = "boundary_world";
    
    // Clean up any existing test data
    if (std::filesystem::exists(testDir)) {
        std::filesystem::remove_all(testDir);
    }
    
    RegionalDatabase database(testDir);
    
    // Test negative coordinates
    RegionalData negativeRegion;
    negativeRegion.regionX = -5;
    negativeRegion.regionY = -10;
    negativeRegion.averageElevation = 200.0f;
    negativeRegion.minElevation = 150.0f;
    negativeRegion.maxElevation = 250.0f;
    negativeRegion.temperature = 10.0f;
    negativeRegion.precipitation = 400.0f;
    negativeRegion.humidity = 0.5f;
    negativeRegion.biomeType = BiomeType::TAIGA;
    
    bool saveNegative = database.SaveRegionalData(testWorldName, negativeRegion);
    assert(saveNegative);
    
    RegionalData loadedNegative;
    bool loadNegative = database.LoadRegionalData(testWorldName, -5, -10, loadedNegative);
    assert(loadNegative);
    assert(loadedNegative.regionX == -5);
    assert(loadedNegative.regionY == -10);
    assert(loadedNegative.biomeType == BiomeType::TAIGA);
    
    // Test large coordinates
    RegionalData largeRegion;
    largeRegion.regionX = 100000;
    largeRegion.regionY = -50000;
    largeRegion.averageElevation = 50.0f;
    largeRegion.minElevation = 0.0f;
    largeRegion.maxElevation = 100.0f;
    largeRegion.temperature = 25.0f;
    largeRegion.precipitation = 200.0f;
    largeRegion.humidity = 0.3f;
    largeRegion.biomeType = BiomeType::TUNDRA;
    
    bool saveLarge = database.SaveRegionalData(testWorldName, largeRegion);
    assert(saveLarge);
    
    RegionalData loadedLarge;
    bool loadLarge = database.LoadRegionalData(testWorldName, 100000, -50000, loadedLarge);
    assert(loadLarge);
    assert(loadedLarge.regionX == 100000);
    assert(loadedLarge.regionY == -50000);
    assert(loadedLarge.biomeType == BiomeType::TUNDRA);
    
    // Clean up test data
    std::filesystem::remove_all(testDir);
    
    std::cout << "✓ RegionalDatabase boundary conditions test passed" << std::endl;
}

void TestCorruptedDataHandling() {
    std::cout << "Testing corrupted data handling..." << std::endl;
    
    // Test deserialization with insufficient data
    std::vector<uint8_t> shortData = {1, 2, 3}; // Too short
    RegionalData testData;
    bool deserializeShort = testData.Deserialize(shortData);
    assert(!deserializeShort);
    
    // Test deserialization with empty data
    std::vector<uint8_t> emptyData;
    bool deserializeEmpty = testData.Deserialize(emptyData);
    assert(!deserializeEmpty);
    
    std::cout << "✓ Corrupted data handling test passed" << std::endl;
}

int main() {
    std::cout << "=== RegionalData System Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    try {
        TestRegionalDataCreation();
        TestRegionalDataSerialization();
        TestRegionalDataValidation();
        TestRegionalDatabaseFileOperations();
        TestRegionalDatabaseBoundaryConditions();
        TestCorruptedDataHandling();
        
        std::cout << std::endl;
        std::cout << "🎉 All RegionalData tests passed successfully!" << std::endl;
        std::cout << "✓ RegionalData serialization and deserialization working correctly" << std::endl;
        std::cout << "✓ RegionalDatabase file I/O operations working correctly" << std::endl;
        std::cout << "✓ Data validation and error handling working correctly" << std::endl;
        std::cout << "✓ Boundary conditions handled properly" << std::endl;
        std::cout << std::endl;
        std::cout << "The RegionalData system is ready for integration into the advanced world generation workflow." << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
