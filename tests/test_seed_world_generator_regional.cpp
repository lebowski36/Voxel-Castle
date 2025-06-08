#include <gtest/gtest.h>
#include "world/seed_world_generator.h"
#include "world/world_seed.h"
#include "world/world_parameters.h"
#include "world/regional_data.h"
#include <memory>
#include <filesystem>
#include <set>

// Simple test that only tests the RegionalData integration without relying on game utilities
using namespace VoxelCastle::World;

class SeedWorldGeneratorRegionalTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test world seed and parameters
        worldSeed_ = std::make_shared<WorldSeed>(12345);
        worldParameters_ = std::make_shared<WorldParameters>();
        
        // Create test world directory
        testWorldPath_ = "test_worlds/regional_test_world";
        std::filesystem::remove_all(testWorldPath_);
    }
    
    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all("test_worlds");
    }
    
    std::shared_ptr<WorldSeed> worldSeed_;
    std::shared_ptr<WorldParameters> worldParameters_;
    std::string testWorldPath_;
};

TEST_F(SeedWorldGeneratorRegionalTest, ConstructorWithoutDatabase) {
    // Test generator creation without regional database - basic constructor test only
    SeedWorldGenerator generator(worldSeed_, worldParameters_);
    
    // Should work without database - basic validation
    EXPECT_EQ(generator.getWorldSeed(), worldSeed_);
    EXPECT_EQ(generator.getWorldParameters(), worldParameters_);
}

TEST_F(SeedWorldGeneratorRegionalTest, DefaultRegionalData) {
    // Test default regional data when no database is set
    SeedWorldGenerator generator(worldSeed_, worldParameters_);
    
    RegionalData data = generator.getRegionalData(100, 200);
    
    // Should return default values
    EXPECT_EQ(data.primaryBiome, BiomeType::PLAINS);
    EXPECT_EQ(data.temperature, 20.0f);
    EXPECT_EQ(data.humidity, 50.0f);
    EXPECT_EQ(data.elevation, 64.0f);
    EXPECT_EQ(data.precipitation, 800.0f);
}
