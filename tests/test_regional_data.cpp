#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>
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
    data.regionZ = 20;
    data.elevation = 150.5f;
    data.temperature = 18.5f;
    data.humidity = 65.0f;
    data.precipitation = 800.0f;
    data.primaryBiome = BiomeType::FOREST;
    
    assert(data.regionX == 10);
    assert(data.regionZ == 20);
    assert(FloatEquals(data.elevation, 150.5f));
    assert(FloatEquals(data.temperature, 18.5f));
    assert(data.primaryBiome == BiomeType::FOREST);
    
    std::cout << "✓ RegionalData creation test passed" << std::endl;
}

void TestRegionalDataSerialization() {
    std::cout << "Testing RegionalData serialization..." << std::endl;
    
    // Create test data
    RegionalData originalData;
    originalData.regionX = 42;
    originalData.regionZ = 84;
    originalData.elevation = 275.25f;
    originalData.temperature = 12.5f;
    originalData.humidity = 55.0f;
    originalData.precipitation = 600.0f;
    originalData.primaryBiome = BiomeType::MOUNTAINS;
    
    // Serialize the data
    std::vector<uint8_t> serializedData;
    bool serializeSuccess = originalData.SerializeToBinary(serializedData);
    assert(serializeSuccess);
    assert(!serializedData.empty());
    
    // Deserialize the data
    RegionalData deserializedData;
    bool deserializeSuccess = deserializedData.DeserializeFromBinary(serializedData);
    assert(deserializeSuccess);
    
    // Verify the data matches
    assert(deserializedData.regionX == originalData.regionX);
    assert(deserializedData.regionZ == originalData.regionZ);
    assert(FloatEquals(deserializedData.elevation, originalData.elevation));
    assert(FloatEquals(deserializedData.temperature, originalData.temperature));
    assert(FloatEquals(deserializedData.humidity, originalData.humidity));
    assert(FloatEquals(deserializedData.precipitation, originalData.precipitation));
    assert(deserializedData.primaryBiome == originalData.primaryBiome);
    
    std::cout << "✓ RegionalData serialization test passed" << std::endl;
}

void TestRegionalDataValidation() {
    std::cout << "Testing RegionalData validation..." << std::endl;
    
    // Test valid data
    RegionalData validData;
    validData.regionX = 0;
    validData.regionZ = 0;
    validData.elevation = 100.0f;
    validData.temperature = 20.0f;
    validData.humidity = 50.0f;
    validData.precipitation = 1000.0f;
    validData.primaryBiome = BiomeType::PLAINS;
    
    assert(validData.IsValid());
    
    std::cout << "✓ RegionalData validation test passed" << std::endl;
}

void TestRegionalDataFileOperations() {
    std::cout << "Testing RegionalData file operations..." << std::endl;
    
    // Create test directory
    const std::string testDir = "test_regional_data";
    std::filesystem::create_directories(testDir);
    
    // Create test data
    RegionalData testData;
    testData.regionX = 5;
    testData.regionZ = 15;
    testData.elevation = 80.5f;
    testData.temperature = 22.0f;
    testData.humidity = 70.0f;
    testData.precipitation = 1200.0f;
    testData.primaryBiome = BiomeType::FOREST;
    
    // Save to file
    std::string testFilepath = testDir + "/test_region_5_15.regc";
    bool saveSuccess = testData.SaveToFile(testFilepath);
    assert(saveSuccess);
    assert(std::filesystem::exists(testFilepath));
    
    // Load from file
    RegionalData loadedData;
    bool loadSuccess = loadedData.LoadFromFile(testFilepath);
    assert(loadSuccess);
    
    // Verify data matches
    assert(loadedData.regionX == testData.regionX);
    assert(loadedData.regionZ == testData.regionZ);
    assert(FloatEquals(loadedData.elevation, testData.elevation));
    assert(FloatEquals(loadedData.temperature, testData.temperature));
    assert(loadedData.primaryBiome == testData.primaryBiome);
    
    // Clean up
    std::filesystem::remove_all(testDir);
    
    std::cout << "✓ RegionalData file operations test passed" << std::endl;
}

void TestRegionalDatabaseOperations() {
    std::cout << "Testing RegionalDatabase operations..." << std::endl;
    
    // Create test directory
    const std::string testDir = "test_regional_database";
    std::filesystem::create_directories(testDir);
    
    // Initialize database
    RegionalDatabase database;
    bool initSuccess = database.Initialize(testDir);
    assert(initSuccess);
    
    // Create test regions
    RegionalData region1;
    region1.regionX = 0;
    region1.regionZ = 0;
    region1.elevation = 120.0f;
    region1.temperature = 15.0f;
    region1.humidity = 60.0f;
    region1.precipitation = 800.0f;
    region1.primaryBiome = BiomeType::PLAINS;
    
    RegionalData region2;
    region2.regionX = 1;
    region2.regionZ = 0;
    region2.elevation = 180.0f;
    region2.temperature = 10.0f;
    region2.humidity = 40.0f;
    region2.precipitation = 600.0f;
    region2.primaryBiome = BiomeType::FOREST;
    
    RegionalData region3;
    region3.regionX = 0;
    region3.regionZ = 1;
    region3.elevation = 80.0f;
    region3.temperature = 25.0f;
    region3.humidity = 30.0f;
    region3.precipitation = 200.0f;
    region3.primaryBiome = BiomeType::DESERT;
    
    // Save regions to database
    bool save1 = database.SetRegionalData(region1.regionX, region1.regionZ, region1);
    bool save2 = database.SetRegionalData(region2.regionX, region2.regionZ, region2);
    bool save3 = database.SetRegionalData(region3.regionX, region3.regionZ, region3);
    assert(save1 && save2 && save3);
    
    // Load regions from database
    RegionalData loadedRegion1, loadedRegion2, loadedRegion3;
    bool load1 = database.GetRegionalData(0, 0, loadedRegion1);
    bool load2 = database.GetRegionalData(1, 0, loadedRegion2);
    bool load3 = database.GetRegionalData(0, 1, loadedRegion3);
    assert(load1 && load2 && load3);
    
    // Verify loaded data matches original
    assert(loadedRegion1.regionX == region1.regionX);
    assert(loadedRegion1.regionZ == region1.regionZ);
    assert(FloatEquals(loadedRegion1.elevation, region1.elevation));
    assert(loadedRegion1.primaryBiome == region1.primaryBiome);
    
    // Test loading non-existent region
    RegionalData nonExistentRegion;
    bool loadNonExistent = database.GetRegionalData(999, 999, nonExistentRegion);
    assert(!loadNonExistent);
    
    // Verify file path generation
    std::string actualPath = database.GetRegionFilePath(0, 0);
    assert(!actualPath.empty());
    
    // Clean up
    database.Shutdown();
    std::filesystem::remove_all(testDir);
    
    std::cout << "✓ RegionalDatabase operations test passed" << std::endl;
}

void TestRegionalDatabaseBoundaryConditions() {
    std::cout << "Testing RegionalDatabase boundary conditions..." << std::endl;
    
    // Create test directory
    const std::string testDir = "test_boundary_conditions";
    std::filesystem::create_directories(testDir);
    
    // Initialize database
    RegionalDatabase database;
    bool initSuccess = database.Initialize(testDir);
    assert(initSuccess);
    
    // Test negative coordinates
    RegionalData negativeRegion;
    negativeRegion.regionX = -5;
    negativeRegion.regionZ = -10;
    negativeRegion.elevation = 200.0f;
    negativeRegion.temperature = 5.0f;
    negativeRegion.humidity = 80.0f;
    negativeRegion.precipitation = 1500.0f;
    negativeRegion.primaryBiome = BiomeType::MOUNTAINS;
    
    bool saveNegative = database.SetRegionalData(negativeRegion.regionX, negativeRegion.regionZ, negativeRegion);
    assert(saveNegative);
    
    RegionalData loadedNegative;
    bool loadNegative = database.GetRegionalData(-5, -10, loadedNegative);
    assert(loadNegative);
    assert(loadedNegative.regionX == -5);
    assert(loadedNegative.regionZ == -10);
    assert(loadedNegative.primaryBiome == BiomeType::MOUNTAINS);
    
    // Test very large coordinates
    RegionalData largeRegion;
    largeRegion.regionX = 100000;
    largeRegion.regionZ = -50000;
    largeRegion.elevation = 50.0f;
    largeRegion.temperature = -10.0f;
    largeRegion.humidity = 20.0f;
    largeRegion.precipitation = 100.0f;
    largeRegion.primaryBiome = BiomeType::OCEAN;
    
    bool saveLarge = database.SetRegionalData(largeRegion.regionX, largeRegion.regionZ, largeRegion);
    assert(saveLarge);
    
    RegionalData loadedLarge;
    bool loadLarge = database.GetRegionalData(100000, -50000, loadedLarge);
    assert(loadLarge);
    assert(loadedLarge.regionX == 100000);
    assert(loadedLarge.regionZ == -50000);
    assert(loadedLarge.primaryBiome == BiomeType::OCEAN);
    
    // Clean up
    database.Shutdown();
    std::filesystem::remove_all(testDir);
    
    std::cout << "✓ RegionalDatabase boundary conditions test passed" << std::endl;
}

void TestCorruptedDataHandling() {
    std::cout << "Testing corrupted data handling..." << std::endl;
    
    // Test deserialization with corrupted/incomplete data
    RegionalData testData;
    
    // Test with too-short data
    std::vector<uint8_t> shortData = {0x01, 0x02, 0x03};
    bool deserializeShort = testData.DeserializeFromBinary(shortData);
    assert(!deserializeShort); // Should fail
    
    // Test with empty data
    std::vector<uint8_t> emptyData;
    bool deserializeEmpty = testData.DeserializeFromBinary(emptyData);
    assert(!deserializeEmpty); // Should fail
    
    std::cout << "✓ Corrupted data handling test passed" << std::endl;
}

void TestRoundTripIntegrity() {
    std::cout << "Testing round-trip data integrity..." << std::endl;
    
    // Create comprehensive test data
    RegionalData originalData;
    originalData.regionX = -12345;
    originalData.regionZ = 67890;
    originalData.elevation = 1234.5678f;
    originalData.temperature = -15.25f;
    originalData.humidity = 99.99f;
    originalData.precipitation = 0.01f;
    originalData.primaryBiome = BiomeType::DESERT;
    
    // Serialize
    std::vector<uint8_t> serializedData;
    bool serializeSuccess = originalData.SerializeToBinary(serializedData);
    assert(serializeSuccess);
    
    // Deserialize
    RegionalData deserializedData;
    bool deserializeSuccess = deserializedData.DeserializeFromBinary(serializedData);
    assert(deserializeSuccess);
    
    // Verify exact match
    assert(deserializedData.regionX == originalData.regionX);
    assert(deserializedData.regionZ == originalData.regionZ);
    assert(FloatEquals(deserializedData.elevation, originalData.elevation));
    assert(FloatEquals(deserializedData.temperature, originalData.temperature));
    assert(FloatEquals(deserializedData.humidity, originalData.humidity));
    assert(FloatEquals(deserializedData.precipitation, originalData.precipitation));
    assert(deserializedData.primaryBiome == originalData.primaryBiome);
    
    std::cout << "✓ Round-trip integrity test passed" << std::endl;
}

void TestAdvancedDataStructures() {
    std::cout << "Testing advanced data structures..." << std::endl;
    
    // Create test data with advanced structures
    RegionalData testData;
    testData.regionX = 100;
    testData.regionZ = 200;
    testData.elevation = 500.0f;
    testData.temperature = 10.0f;
    testData.humidity = 45.0f;
    testData.precipitation = 900.0f;
    testData.primaryBiome = BiomeType::MOUNTAINS;
    testData.generationSeed = 12345678ULL;
    testData.generationTime = 1640995200; // Jan 1, 2022
    testData.simulationLevel = 75;
    
    // Initialize advanced data structures with test values
    testData.geological.soilDepth = 2.3f;
    testData.geological.bedrockDepth = 25.0f;
    testData.geological.mineralRichness = 0.85f;
    testData.geological.tectonicStability = 0.75f;
    
    testData.hydrological.drainageArea = 15.0f;
    testData.hydrological.runoffCoefficient = 0.75f;
    testData.hydrological.infiltrationRate = 0.8f;
    testData.hydrological.groundwaterLevel = 5.2f;
    
    testData.climate.annualTemperature = 8.5f;
    testData.climate.temperatureRange = 25.0f;
    testData.climate.annualPrecipitation = 850.0f;
    testData.climate.annualHumidity = 0.7f;
    
    // Test validation
    assert(testData.IsValid());
    assert(testData.geological.IsValid());
    assert(testData.hydrological.IsValid());
    assert(testData.climate.IsValid());
    
    // Test serialization/deserialization round-trip
    std::vector<uint8_t> serializedData;
    bool serializeSuccess = testData.SerializeToBinary(serializedData);
    assert(serializeSuccess);
    assert(!serializedData.empty());
    
    RegionalData deserializedData;
    bool deserializeSuccess = deserializedData.DeserializeFromBinary(serializedData);
    assert(deserializeSuccess);
    
    // Verify advanced data was preserved
    assert(deserializedData.generationSeed == testData.generationSeed);
    assert(deserializedData.generationTime == testData.generationTime);
    assert(deserializedData.simulationLevel == testData.simulationLevel);
    
    assert(FloatEquals(deserializedData.geological.soilDepth, testData.geological.soilDepth));
    assert(FloatEquals(deserializedData.geological.bedrockDepth, testData.geological.bedrockDepth));
    assert(FloatEquals(deserializedData.geological.mineralRichness, testData.geological.mineralRichness));
    assert(FloatEquals(deserializedData.geological.tectonicStability, testData.geological.tectonicStability));
    
    assert(FloatEquals(deserializedData.hydrological.drainageArea, testData.hydrological.drainageArea));
    assert(FloatEquals(deserializedData.hydrological.runoffCoefficient, testData.hydrological.runoffCoefficient));
    assert(FloatEquals(deserializedData.hydrological.infiltrationRate, testData.hydrological.infiltrationRate));
    assert(FloatEquals(deserializedData.hydrological.groundwaterLevel, testData.hydrological.groundwaterLevel));
    
    assert(FloatEquals(deserializedData.climate.annualTemperature, testData.climate.annualTemperature));
    assert(FloatEquals(deserializedData.climate.temperatureRange, testData.climate.temperatureRange));
    assert(FloatEquals(deserializedData.climate.annualPrecipitation, testData.climate.annualPrecipitation));
    assert(FloatEquals(deserializedData.climate.annualHumidity, testData.climate.annualHumidity));
    
    // Test validation on deserialized data
    assert(deserializedData.IsValid());
    assert(deserializedData.geological.IsValid());
    assert(deserializedData.hydrological.IsValid());
    assert(deserializedData.climate.IsValid());
    
    std::cout << "✓ Advanced data structures test passed" << std::endl;
}

int main() {
    std::cout << "=== RegionalData System Test Suite ===" << std::endl;
    
    try {
        TestRegionalDataCreation();
        TestRegionalDataSerialization();
        TestRegionalDataValidation();
        TestRegionalDataFileOperations();
        TestRegionalDatabaseOperations();
        TestRegionalDatabaseBoundaryConditions();
        TestCorruptedDataHandling();
        TestRoundTripIntegrity();
        TestAdvancedDataStructures();
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        std::cout << "RegionalData system is working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
