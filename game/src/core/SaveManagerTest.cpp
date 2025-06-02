#include "core/SaveManagerTest.h"
#include "core/SaveManager.h"
#include "core/JsonUtils.h"
#include <iostream>
#include <filesystem>
#include <glm/vec3.hpp>

namespace VoxelCastle {
namespace Core {

void runSaveManagerTest() {
    std::cout << "\n=== SaveManager & JSON Utilities Test ===" << std::endl;
    std::cout << "Testing Phase 2 Save/Load System Implementation" << std::endl;
    
    bool allTestsPassed = true;
    
    // === Test 1: JSON Utilities ===
    std::cout << "\n--- Test 1: JSON Utilities ---" << std::endl;
    
    // Test metadata creation
    std::string version = "1.0.0";
    std::string worldName = "Test Voxel World";
    glm::vec3 playerPos(123.45f, 64.0f, -78.9f);
    uint64_t playTime = 3600; // 1 hour
    std::string cameraMode = "STRATEGIC_MODE";
    
    std::string metadataJson = JsonUtils::createMetadataJson(version, worldName, playerPos, playTime, cameraMode);
    std::cout << "Created metadata JSON:" << std::endl;
    std::cout << metadataJson << std::endl;
    
    // Test metadata parsing
    std::string parsedVersion, parsedWorldName, parsedCameraMode;
    glm::vec3 parsedPlayerPos;
    uint64_t parsedPlayTime;
    
    bool parseSuccess = JsonUtils::parseMetadataJson(
        metadataJson, parsedVersion, parsedWorldName, parsedPlayerPos, parsedPlayTime, parsedCameraMode
    );
    
    if (parseSuccess) {
        std::cout << "✅ JSON parsing successful" << std::endl;
        std::cout << "  Version: " << parsedVersion << std::endl;
        std::cout << "  World Name: " << parsedWorldName << std::endl;
        std::cout << "  Player Position: (" << parsedPlayerPos.x << ", " << parsedPlayerPos.y << ", " << parsedPlayerPos.z << ")" << std::endl;
        std::cout << "  Play Time: " << parsedPlayTime << " seconds" << std::endl;
        std::cout << "  Camera Mode: " << parsedCameraMode << std::endl;
        
        // Validate parsed values
        bool valuesMatch = (parsedVersion == version) && 
                          (parsedWorldName == worldName) &&
                          (abs(parsedPlayerPos.x - playerPos.x) < 0.01f) &&
                          (abs(parsedPlayerPos.y - playerPos.y) < 0.01f) &&
                          (abs(parsedPlayerPos.z - playerPos.z) < 0.01f) &&
                          (parsedPlayTime == playTime) &&
                          (parsedCameraMode == cameraMode);
        
        if (valuesMatch) {
            std::cout << "✅ All parsed values match original data" << std::endl;
        } else {
            std::cout << "❌ Parsed values don't match original data" << std::endl;
            allTestsPassed = false;
        }
    } else {
        std::cout << "❌ JSON parsing failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Test file I/O
    std::string testFilePath = "./test_metadata.json";
    bool writeSuccess = JsonUtils::writeJsonToFile(testFilePath, metadataJson);
    if (writeSuccess) {
        std::cout << "✅ JSON file write successful" << std::endl;
        
        std::string readJson;
        bool readSuccess = JsonUtils::readJsonFromFile(testFilePath, readJson);
        if (readSuccess && readJson == metadataJson) {
            std::cout << "✅ JSON file read successful and content matches" << std::endl;
        } else {
            std::cout << "❌ JSON file read failed or content doesn't match" << std::endl;
            allTestsPassed = false;
        }
        
        // Clean up test file
        std::filesystem::remove(testFilePath);
    } else {
        std::cout << "❌ JSON file write failed" << std::endl;
        allTestsPassed = false;
    }
    
    // === Test 2: SaveManager Initialization ===
    std::cout << "\n--- Test 2: SaveManager Initialization ---" << std::endl;
    
    // Create SaveManager with WorldManager directly (avoid Game dependency for testing)
    SaveManager saveManager(static_cast<VoxelCastle::World::WorldManager*>(nullptr));
    
    std::string testSaveDir = "./test_saves";
    bool initSuccess = saveManager.initialize(testSaveDir);
    if (initSuccess) {
        std::cout << "✅ SaveManager initialization successful" << std::endl;
        
        // Check if directory was created
        if (std::filesystem::exists(testSaveDir)) {
            std::cout << "✅ Save directory created successfully" << std::endl;
        } else {
            std::cout << "❌ Save directory not created" << std::endl;
            allTestsPassed = false;
        }
    } else {
        std::cout << "❌ SaveManager initialization failed" << std::endl;
        allTestsPassed = false;
    }
    
    // === Test 3: Save Creation ===
    std::cout << "\n--- Test 3: Save Creation ---" << std::endl;
    
    std::string testSaveName = "test_save_1";
    std::string testDisplayName = "Test Save Display Name";
    
    bool createSuccess = saveManager.createSave(testSaveName, testDisplayName);
    if (createSuccess) {
        std::cout << "✅ Save creation successful" << std::endl;
        
        // Check if save directory and files were created
        std::string savePath = testSaveDir + "/" + testSaveName;
        std::string metadataPath = savePath + "/metadata.json";
        
        if (std::filesystem::exists(savePath) && std::filesystem::exists(metadataPath)) {
            std::cout << "✅ Save directory and metadata file created" << std::endl;
            
            // Test save info retrieval
            SaveInfo saveInfo = saveManager.getSaveInfo(testSaveName);
            if (!saveInfo.displayName.empty()) {
                std::cout << "✅ Save info retrieval successful" << std::endl;
                std::cout << "  Display Name: " << saveInfo.displayName << std::endl;
                std::cout << "  Play Time: " << saveInfo.playTimeSeconds << " seconds" << std::endl;
            } else {
                std::cout << "❌ Save info retrieval failed" << std::endl;
                allTestsPassed = false;
            }
        } else {
            std::cout << "❌ Save files not created properly" << std::endl;
            allTestsPassed = false;
        }
    } else {
        std::cout << "❌ Save creation failed" << std::endl;
        allTestsPassed = false;
    }
    
    // === Test 4: Save Listing ===
    std::cout << "\n--- Test 4: Save Listing ---" << std::endl;
    
    std::vector<SaveInfo> saves = saveManager.listSaves();
    if (!saves.empty()) {
        std::cout << "✅ Save listing successful, found " << saves.size() << " save(s)" << std::endl;
        for (const auto& save : saves) {
            std::cout << "  - " << save.name << " (" << save.displayName << ")" << std::endl;
        }
    } else {
        std::cout << "❌ No saves found (expected at least the test save)" << std::endl;
        allTestsPassed = false;
    }
    
    // === Test 5: Timestamp Functions ===
    std::cout << "\n--- Test 5: Timestamp Functions ---" << std::endl;
    
    std::string currentTimestamp = JsonUtils::getCurrentTimestamp();
    std::cout << "Current timestamp: " << currentTimestamp << std::endl;
    
    if (!currentTimestamp.empty() && currentTimestamp.length() > 10) {
        std::cout << "✅ Timestamp generation successful" << std::endl;
    } else {
        std::cout << "❌ Timestamp generation failed" << std::endl;
        allTestsPassed = false;
    }
    
    // === Test 6: Operation State ===
    std::cout << "\n--- Test 6: Operation State ---" << std::endl;
    
    bool operationInProgress = saveManager.isOperationInProgress();
    if (!operationInProgress) {
        std::cout << "✅ Operation state tracking working (no operations in progress)" << std::endl;
    } else {
        std::cout << "❌ Operation state incorrect (should be false when idle)" << std::endl;
        allTestsPassed = false;
    }
    
    // === Cleanup ===
    std::cout << "\n--- Cleanup ---" << std::endl;
    
    try {
        if (std::filesystem::exists(testSaveDir)) {
            std::filesystem::remove_all(testSaveDir);
            std::cout << "✅ Test files cleaned up" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "⚠️ Cleanup warning: " << e.what() << std::endl;
    }
    
    // === Final Results ===
    std::cout << "\n=== Test Results ===" << std::endl;
    if (allTestsPassed) {
        std::cout << "🎉 ALL TESTS PASSED! SaveManager and JSON utilities are working correctly." << std::endl;
        std::cout << "Phase 2 implementation is ready for integration." << std::endl;
    } else {
        std::cout << "❌ Some tests failed. Please review the implementation." << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}

} // namespace Core
} // namespace VoxelCastle
