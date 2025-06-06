#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include "world/voxel_face_patterns.h"
#include "world/voxel_types.h"

using namespace VoxelEngine::World;

/**
 * @brief Test suite for the voxel face pattern classification system
 */
class VoxelFacePatternsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup if needed
    }
    
    void TearDown() override {
        // Test cleanup if needed
    }
};

/**
 * @brief Test that all defined voxel types have face pattern mappings
 */
TEST_F(VoxelFacePatternsTest, AllVoxelTypesHaveMappings) {
    // Test a sampling of voxel types across all categories
    std::vector<VoxelType> testVoxels = {
        // Basic terrain
        VoxelType::AIR, VoxelType::STONE, VoxelType::DIRT, VoxelType::GRASS,
        VoxelType::SAND, VoxelType::GRAVEL, VoxelType::CLAY, VoxelType::BEDROCK,
        
        // Stone varieties
        VoxelType::GRANITE, VoxelType::LIMESTONE, VoxelType::MARBLE, VoxelType::SANDSTONE,
        
        // Ores
        VoxelType::COAL_ORE, VoxelType::IRON_ORE, VoxelType::GOLD_ORE, VoxelType::GEM_DIAMOND,
        
        // Organic
        VoxelType::WOOD_OAK, VoxelType::WOOD_PINE, VoxelType::LEAVES_OAK, VoxelType::MUSHROOM_BROWN,
        
        // Fluids
        VoxelType::WATER, VoxelType::LAVA, VoxelType::OIL,
        
        // Processed materials
        VoxelType::STONE_BRICK, VoxelType::OAK_PLANKS, VoxelType::IRON_BLOCK, VoxelType::CLAY_BRICK,
        
        // Functional blocks
        VoxelType::DOOR_WOOD, VoxelType::CHEST_WOOD, VoxelType::TORCH, VoxelType::TABLE_WOOD,
        
        // Advanced materials
        VoxelType::CRYSTAL_CLEAR, VoxelType::CONCRETE, VoxelType::BLOCK_INVISIBLE
    };
    
    for (VoxelType voxelType : testVoxels) {
        FacePattern pattern = getFacePattern(voxelType);
        
        // Should not be an invalid pattern (all patterns are valid in our enum)
        EXPECT_GE(static_cast<uint8_t>(pattern), 0);
        EXPECT_LE(static_cast<uint8_t>(pattern), 3);
    }
}

/**
 * @brief Test uniform pattern blocks
 */
TEST_F(VoxelFacePatternsTest, UniformPatternBlocks) {
    std::vector<VoxelType> uniformBlocks = {
        VoxelType::STONE,
        VoxelType::GRANITE,
        VoxelType::IRON_BLOCK,
        VoxelType::SAND,
        VoxelType::LEAVES_OAK
    };
    
    for (VoxelType voxelType : uniformBlocks) {
        FacePattern pattern = getFacePattern(voxelType);
        EXPECT_EQ(pattern, FacePattern::UNIFORM);
        
        // Check utility functions
        EXPECT_EQ(getAtlasSlotCount(pattern), 1);
        EXPECT_FALSE(requiresSideAtlas(pattern));
        EXPECT_FALSE(requiresBottomAtlas(pattern));
    }
}

/**
 * @brief Test top-bottom different pattern blocks
 */
TEST_F(VoxelFacePatternsTest, TopBottomDifferentPatternBlocks) {
    std::vector<VoxelType> topBottomBlocks = {
        VoxelType::WOOD_OAK,
        VoxelType::WOOD_PINE,
        VoxelType::WOOD_BIRCH,
        VoxelType::CACTUS,
        VoxelType::OAK_BEAM
    };
    
    for (VoxelType voxelType : topBottomBlocks) {
        FacePattern pattern = getFacePattern(voxelType);
        EXPECT_EQ(pattern, FacePattern::TOP_BOTTOM_DIFFERENT);
        
        // Check utility functions
        EXPECT_EQ(getAtlasSlotCount(pattern), 2);
        EXPECT_TRUE(requiresSideAtlas(pattern));
        EXPECT_FALSE(requiresBottomAtlas(pattern));
    }
}

/**
 * @brief Test all different pattern blocks
 */
TEST_F(VoxelFacePatternsTest, AllDifferentPatternBlocks) {
    std::vector<VoxelType> allDifferentBlocks = {
        VoxelType::GRASS
    };
    
    for (VoxelType voxelType : allDifferentBlocks) {
        FacePattern pattern = getFacePattern(voxelType);
        EXPECT_EQ(pattern, FacePattern::ALL_DIFFERENT);
        
        // Check utility functions
        EXPECT_EQ(getAtlasSlotCount(pattern), 3);
        EXPECT_TRUE(requiresSideAtlas(pattern));
        EXPECT_TRUE(requiresBottomAtlas(pattern));
    }
}

/**
 * @brief Test directional pattern blocks
 */
TEST_F(VoxelFacePatternsTest, DirectionalPatternBlocks) {
    std::vector<VoxelType> directionalBlocks = {
        VoxelType::DOOR_WOOD,
        VoxelType::CHEST_WOOD,
        VoxelType::TORCH,
        VoxelType::FURNACE,
        VoxelType::BOOKSHELF
    };
    
    for (VoxelType voxelType : directionalBlocks) {
        FacePattern pattern = getFacePattern(voxelType);
        EXPECT_EQ(pattern, FacePattern::DIRECTIONAL);
        
        // Check utility functions
        EXPECT_GE(getAtlasSlotCount(pattern), 4);
        EXPECT_TRUE(requiresSideAtlas(pattern));
        EXPECT_TRUE(requiresBottomAtlas(pattern));
    }
}

/**
 * @brief Test that unmapped voxel types return default pattern
 */
TEST_F(VoxelFacePatternsTest, UnmappedVoxelTypesReturnDefault) {
    // Test with a value that should not be in our mapping
    // (casting to VoxelType for test purposes - normally this would be invalid)
    VoxelType unmappedType = static_cast<VoxelType>(200); // Reserved range
    
    FacePattern pattern = getFacePattern(unmappedType);
    EXPECT_EQ(pattern, FacePattern::UNIFORM); // Default fallback
}

/**
 * @brief Test atlas slot count calculation for all patterns
 */
TEST_F(VoxelFacePatternsTest, AtlasSlotCountCalculation) {
    EXPECT_EQ(getAtlasSlotCount(FacePattern::UNIFORM), 1);
    EXPECT_EQ(getAtlasSlotCount(FacePattern::TOP_BOTTOM_DIFFERENT), 2);
    EXPECT_EQ(getAtlasSlotCount(FacePattern::ALL_DIFFERENT), 3);
    EXPECT_EQ(getAtlasSlotCount(FacePattern::DIRECTIONAL), 4);
}

/**
 * @brief Test atlas requirement functions
 */
TEST_F(VoxelFacePatternsTest, AtlasRequirementFunctions) {
    // Test side atlas requirements
    EXPECT_FALSE(requiresSideAtlas(FacePattern::UNIFORM));
    EXPECT_TRUE(requiresSideAtlas(FacePattern::TOP_BOTTOM_DIFFERENT));
    EXPECT_TRUE(requiresSideAtlas(FacePattern::ALL_DIFFERENT));
    EXPECT_TRUE(requiresSideAtlas(FacePattern::DIRECTIONAL));
    
    // Test bottom atlas requirements
    EXPECT_FALSE(requiresBottomAtlas(FacePattern::UNIFORM));
    EXPECT_FALSE(requiresBottomAtlas(FacePattern::TOP_BOTTOM_DIFFERENT));
    EXPECT_TRUE(requiresBottomAtlas(FacePattern::ALL_DIFFERENT));
    EXPECT_TRUE(requiresBottomAtlas(FacePattern::DIRECTIONAL));
}

/**
 * @brief Test mapping coverage for specific block categories
 */
TEST_F(VoxelFacePatternsTest, BlockCategoryCoverage) {
    // Test that important block categories have appropriate patterns
    
    // All wood logs should be TOP_BOTTOM_DIFFERENT
    std::vector<VoxelType> woodLogs = {
        VoxelType::WOOD_OAK, VoxelType::WOOD_PINE, 
        VoxelType::WOOD_BIRCH, VoxelType::WOOD_MAHOGANY
    };
    for (VoxelType wood : woodLogs) {
        EXPECT_EQ(getFacePattern(wood), FacePattern::TOP_BOTTOM_DIFFERENT);
    }
    
    // All metal blocks should be UNIFORM
    std::vector<VoxelType> metalBlocks = {
        VoxelType::IRON_BLOCK, VoxelType::COPPER_BLOCK,
        VoxelType::GOLD_BLOCK, VoxelType::SILVER_BLOCK
    };
    for (VoxelType metal : metalBlocks) {
        EXPECT_EQ(getFacePattern(metal), FacePattern::UNIFORM);
    }
    
    // All doors should be DIRECTIONAL
    std::vector<VoxelType> doors = {
        VoxelType::DOOR_WOOD, VoxelType::DOOR_IRON, VoxelType::DOOR_REINFORCED
    };
    for (VoxelType door : doors) {
        EXPECT_EQ(getFacePattern(door), FacePattern::DIRECTIONAL);
    }
}

/**
 * @brief Test specific block patterns that are critical for world generation
 */
TEST_F(VoxelFacePatternsTest, WorldGenCriticalBlocks) {
    // These blocks are heavily used in world generation and must have correct patterns
    EXPECT_EQ(getFacePattern(VoxelType::STONE), FacePattern::UNIFORM);
    EXPECT_EQ(getFacePattern(VoxelType::DIRT), FacePattern::UNIFORM);
    EXPECT_EQ(getFacePattern(VoxelType::GRASS), FacePattern::ALL_DIFFERENT);
    EXPECT_EQ(getFacePattern(VoxelType::WATER), FacePattern::UNIFORM);
    EXPECT_EQ(getFacePattern(VoxelType::SAND), FacePattern::UNIFORM);
    EXPECT_EQ(getFacePattern(VoxelType::WOOD_OAK), FacePattern::TOP_BOTTOM_DIFFERENT);
    EXPECT_EQ(getFacePattern(VoxelType::LEAVES_OAK), FacePattern::UNIFORM);
}

/**
 * @brief Performance test for face pattern lookup
 */
TEST_F(VoxelFacePatternsTest, FacePatternLookupPerformance) {
    // This test ensures that face pattern lookup is fast enough for real-time use
    const int NUM_LOOKUPS = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_LOOKUPS; ++i) {
        // Test with different voxel types
        VoxelType testType = static_cast<VoxelType>(i % 180); // Cycle through defined types
        FacePattern pattern = getFacePattern(testType);
        (void)pattern; // Suppress unused variable warning
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should complete 100,000 lookups in under 100ms (very generous threshold)
    EXPECT_LT(duration.count(), 100000);
    
    std::cout << "Face pattern lookup performance: " 
              << NUM_LOOKUPS << " lookups in " 
              << duration.count() << " microseconds" << std::endl;
}
