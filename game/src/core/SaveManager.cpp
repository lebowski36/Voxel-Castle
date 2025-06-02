#include "core/game.h"
#include "core/SaveManager.h"
#include "core/JsonUtils.h"
#include "world/world_manager.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace VoxelCastle {
namespace Core {

SaveManager::SaveManager(Game* game)
    : game_(game)
    , worldManager_(nullptr)
    , isSaving_(false)
    , isLoading_(false)
    , autoSaveEnabled_(false)
    , autoSaveInterval_(5) // Default 5 minutes
    , autoSaveRotateCount_(3)
    , stopAutoSaveThread_(false)
    , lastSaveStats_{0, 0, 0.0}
{
    // Note: worldManager_ will be set during initialize() to avoid circular dependency
}

SaveManager::SaveManager(VoxelCastle::World::WorldManager* worldManager)
    : game_(nullptr)
    , worldManager_(worldManager)
    , isSaving_(false)
    , isLoading_(false)
    , autoSaveEnabled_(false)
    , autoSaveInterval_(5) // Default 5 minutes
    , autoSaveRotateCount_(3)
    , stopAutoSaveThread_(false)
    , lastSaveStats_{0, 0, 0.0}
{
    // WorldManager is directly provided
}

SaveManager::~SaveManager() {
    stopAutoSave();
}

bool SaveManager::initialize(const std::string& baseSaveDir) {
    baseSaveDirectory_ = baseSaveDir;
    
    // Note: worldManager_ should be set via setWorldManager() method
    // to avoid circular dependency issues
    
    // Create base save directory if it doesn't exist
    try {
        std::filesystem::create_directories(baseSaveDirectory_);
        std::cout << "[SaveManager] Initialized with save directory: " << baseSaveDirectory_ << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Failed to create save directory: " << e.what() << std::endl;
        return false;
    }
}

void SaveManager::setWorldManager(VoxelCastle::World::WorldManager* worldManager) {
    worldManager_ = worldManager;
    std::cout << "[SaveManager] WorldManager reference set" << std::endl;
}

bool SaveManager::createSave(const std::string& saveName, const std::string& displayName) {
    if (saveName.empty()) {
        std::cerr << "[SaveManager] Cannot create save with empty name" << std::endl;
        return false;
    }
    
    std::string savePath = baseSaveDirectory_ + "/" + saveName;
    
    // Check if save already exists
    if (std::filesystem::exists(savePath)) {
        std::cout << "[SaveManager] Save already exists: " << saveName << std::endl;
        return true; // Not an error - save exists
    }
    
    // Create save directory structure
    if (!ensureSaveDirectories(savePath)) {
        return false;
    }
    
    // Create initial metadata
    std::string metadataPath = savePath + "/metadata.json";
    std::string worldName = displayName.empty() ? saveName : displayName;
    glm::vec3 defaultPosition(0.0f, 70.0f, 0.0f); // Default spawn position
    
    std::string metadataJson = JsonUtils::createMetadataJson(
        "1.0.0",           // version
        worldName,         // world name
        defaultPosition,   // player position
        0,                 // play time
        "STRATEGIC_MODE"   // camera mode
    );
    
    if (!JsonUtils::writeJsonToFile(metadataPath, metadataJson)) {
        std::cerr << "[SaveManager] Failed to create initial metadata for save: " << saveName << std::endl;
        return false;
    }
    
    std::cout << "[SaveManager] Created new save: " << saveName << std::endl;
    return true;
}

bool SaveManager::saveGame(const std::string& saveName, bool isQuickSave) {
    // Acquire mutex for thread safety
    std::lock_guard<std::mutex> lock(saveMutex_);
    
    if (isSaving_ || isLoading_) {
        std::cout << "[SaveManager] Save operation already in progress" << std::endl;
        return false;
    }
    
    // Set save state
    isSaving_ = true;
    
    std::cout << "[SaveManager] Starting save operation: " << saveName << std::endl;
    
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create save path
    std::string savePath = baseSaveDirectory_ + "/" + saveName;
    std::string tempPath = savePath + "_temp";
    
    // Create backup of existing save
    if (std::filesystem::exists(savePath)) {
        if (!createBackup(saveName)) {
            std::cerr << "[SaveManager] Failed to create backup for save: " << saveName << std::endl;
            // Continue anyway - backup failure shouldn't prevent saving
        }
    }
    
    // Ensure temp directory exists
    if (!ensureSaveDirectories(tempPath)) {
        handleSaveError(saveName, "Failed to create save directories");
        isSaving_ = false;
        return false;
    }
    
    // Save metadata
    if (!saveMetadata(tempPath)) {
        handleSaveError(saveName, "Failed to save metadata");
        isSaving_ = false;
        return false;
    }
    
    // Save player data
    if (!savePlayerData(tempPath)) {
        handleSaveError(saveName, "Failed to save player data");
        isSaving_ = false;
        return false;
    }
    
    // Save chunks
    if (!saveChunks(tempPath, true)) { // Incremental save
        handleSaveError(saveName, "Failed to save chunks");
        isSaving_ = false;
        return false;
    }
    
    // Calculate save statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    lastSaveStats_.saveTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    if (worldManager_) {
        lastSaveStats_.totalChunks = worldManager_->getChunkCount();
        lastSaveStats_.modifiedChunks = worldManager_->getModifiedChunks().size();
    }
    
    // Atomic directory rename for data integrity
    try {
        if (std::filesystem::exists(savePath)) {
            std::filesystem::remove_all(savePath);
        }
        std::filesystem::rename(tempPath, savePath);
    } catch (const std::filesystem::filesystem_error& e) {
        handleSaveError(saveName, "Failed to finalize save: " + std::string(e.what()));
        isSaving_ = false;
        return false;
    }
    
    // Update current save name
    currentSaveName_ = saveName;
    
    // Clear modified chunks tracking
    if (worldManager_) {
        worldManager_->clearModifiedChunks();
    }
    
    // Log success
    std::cout << "[SaveManager] Save completed successfully: " << saveName 
              << " (" << lastSaveStats_.modifiedChunks << " chunks in " 
              << lastSaveStats_.saveTimeMs << "ms)" << std::endl;
    
    logOperation("SAVE", saveName, true);
    isSaving_ = false;
    return true;
}

bool SaveManager::loadGame(const std::string& saveName) {
    // Acquire mutex for thread safety
    std::lock_guard<std::mutex> lock(saveMutex_);
    
    if (isSaving_ || isLoading_) {
        std::cout << "[SaveManager] Load operation already in progress" << std::endl;
        return false;
    }
    
    // Set loading state
    isLoading_ = true;
    
    std::cout << "[SaveManager] Starting load operation: " << saveName << std::endl;
    
    std::string savePath = baseSaveDirectory_ + "/" + saveName;
    
    // Check if save exists
    if (!std::filesystem::exists(savePath)) {
        handleSaveError(saveName, "Save does not exist");
        isLoading_ = false;
        return false;
    }
    
    // Load metadata first to validate save
    SaveInfo saveInfo;
    if (!loadMetadata(savePath, saveInfo)) {
        handleSaveError(saveName, "Failed to load metadata");
        isLoading_ = false;
        return false;
    }
    
    // Load player data
    if (!loadPlayerData(savePath)) {
        handleSaveError(saveName, "Failed to load player data");
        isLoading_ = false;
        return false;
    }
    
    // Load chunks
    if (!loadChunks(savePath)) {
        handleSaveError(saveName, "Failed to load chunks");
        isLoading_ = false;
        return false;
    }
    
    // Update current save name
    currentSaveName_ = saveName;
    
    // Log success
    std::cout << "[SaveManager] Load completed successfully: " << saveName << std::endl;
    logOperation("LOAD", saveName, true);
    
    isLoading_ = false;
    return true;
}

bool SaveManager::quickSave() {
    return saveGame("quicksave", true);
}

bool SaveManager::quickLoad() {
    return loadGame("quicksave");
}

void SaveManager::startAutoSave(int intervalMinutes) {
    if (autoSaveEnabled_) {
        stopAutoSave(); // Stop existing auto-save first
    }
    
    autoSaveInterval_ = intervalMinutes;
    autoSaveEnabled_ = true;
    stopAutoSaveThread_ = false;
    
    autoSaveThread_ = std::thread(&SaveManager::autoSaveThreadFunction, this);
    
    std::cout << "[SaveManager] Auto-save started with " << intervalMinutes << " minute interval" << std::endl;
}

void SaveManager::stopAutoSave() {
    if (autoSaveEnabled_) {
        stopAutoSaveThread_ = true;
        autoSaveEnabled_ = false;
        
        if (autoSaveThread_.joinable()) {
            autoSaveThread_.join();
        }
        
        std::cout << "[SaveManager] Auto-save stopped" << std::endl;
    }
}

bool SaveManager::performAutoSave() {
    return saveGame("autosave", false);
}

std::vector<SaveInfo> SaveManager::listSaves() {
    std::vector<SaveInfo> saves;
    
    try {
        if (!std::filesystem::exists(baseSaveDirectory_)) {
            return saves; // Empty list if directory doesn't exist
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(baseSaveDirectory_)) {
            if (entry.is_directory()) {
                SaveInfo saveInfo;
                if (loadMetadata(entry.path().string(), saveInfo)) {
                    saveInfo.name = entry.path().filename().string();
                    saves.push_back(saveInfo);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Error listing saves: " << e.what() << std::endl;
    }
    
    return saves;
}

SaveInfo SaveManager::getSaveInfo(const std::string& saveName) {
    SaveInfo saveInfo;
    std::string savePath = baseSaveDirectory_ + "/" + saveName;
    
    if (loadMetadata(savePath, saveInfo)) {
        saveInfo.name = saveName;
    }
    
    return saveInfo;
}

bool SaveManager::deleteSave(const std::string& saveName) {
    try {
        std::string savePath = baseSaveDirectory_ + "/" + saveName;
        
        if (std::filesystem::exists(savePath)) {
            std::filesystem::remove_all(savePath);
            std::cout << "[SaveManager] Deleted save: " << saveName << std::endl;
            return true;
        } else {
            std::cerr << "[SaveManager] Save does not exist: " << saveName << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Error deleting save " << saveName << ": " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::isOperationInProgress() const {
    return isSaving_ || isLoading_;
}

std::string SaveManager::getCurrentSaveName() const {
    return currentSaveName_;
}

bool SaveManager::saveMetadata(const std::string& savePath) {
    try {
        // Get current game state
        glm::vec3 playerPosition(0.0f, 70.0f, 0.0f); // TODO: Get from player/camera
        std::string cameraMode = "STRATEGIC_MODE";     // TODO: Get from game state
        uint64_t playTime = 0;                         // TODO: Get from game timer
        
        std::string metadataJson = JsonUtils::createMetadataJson(
            "1.0.0",
            currentSaveName_.empty() ? "Voxel World" : currentSaveName_,
            playerPosition,
            playTime,
            cameraMode
        );
        
        std::string metadataPath = savePath + "/metadata.json";
        return JsonUtils::writeJsonToFile(metadataPath, metadataJson);
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception saving metadata: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::loadMetadata(const std::string& savePath, SaveInfo& saveInfo) {
    try {
        std::string metadataPath = savePath + "/metadata.json";
        std::string jsonContent;
        
        if (!JsonUtils::readJsonFromFile(metadataPath, jsonContent)) {
            return false;
        }
        
        std::string version, worldName, cameraMode;
        glm::vec3 playerPosition;
        uint64_t playTime;
        
        if (JsonUtils::parseMetadataJson(jsonContent, version, worldName, playerPosition, playTime, cameraMode)) {
            saveInfo.displayName = worldName;
            saveInfo.playTimeSeconds = playTime;
            saveInfo.hasQuickSave = false; // TODO: Implement
            saveInfo.hasAutoSave = false;  // TODO: Implement
            saveInfo.screenshotPath = savePath + "/screenshot.png";
            saveInfo.timestamp = JsonUtils::getCurrentTimestamp();
            saveInfo.lastPlayedDate = JsonUtils::getCurrentTimestamp();
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception loading metadata: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::savePlayerData(const std::string& savePath) {
    // TODO: Implement player data saving when player system is available
    // For now, create an empty player.json file
    try {
        std::string playerPath = savePath + "/player.json";
        std::string playerJson = "{\n  \"version\": \"1.0.0\",\n  \"placeholder\": true\n}";
        return JsonUtils::writeJsonToFile(playerPath, playerJson);
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception saving player data: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::loadPlayerData(const std::string& savePath) {
    // TODO: Implement player data loading when player system is available
    // For now, just check if the file exists
    std::string playerPath = savePath + "/player.json";
    return std::filesystem::exists(playerPath);
}

bool SaveManager::saveChunks(const std::string& savePath, bool incrementalOnly) {
    if (!worldManager_) {
        std::cerr << "[SaveManager] WorldManager not available for chunk saving" << std::endl;
        return false;
    }
    
    try {
        // Create chunks directory
        std::string chunksPath = savePath + "/chunks";
        std::filesystem::create_directories(chunksPath);
        
        // Get modified chunks
        std::vector<::VoxelCastle::World::WorldCoordXZ> chunksToSave;
        if (incrementalOnly) {
            chunksToSave = worldManager_->getModifiedChunks();
        } else {
            // TODO: Implement full chunk saving when needed
            chunksToSave = worldManager_->getModifiedChunks();
        }
        
        std::cout << "[SaveManager] Saving " << chunksToSave.size() << " chunks" << std::endl;
        
        // For now, just create the chunk manifest
        // TODO: Implement actual chunk serialization in the next step
        if (!updateChunkManifest(savePath, chunksToSave)) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception saving chunks: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::loadChunks(const std::string& savePath) {
    // TODO: Implement chunk loading
    // For now, just check if chunks directory exists
    std::string chunksPath = savePath + "/chunks";
    return std::filesystem::exists(chunksPath);
}

bool SaveManager::updateChunkManifest(const std::string& savePath, 
                                     const std::vector<::VoxelCastle::World::WorldCoordXZ>& savedChunks) {
    try {
        std::string manifestPath = savePath + "/chunks/manifest.json";
        
        // Create simple manifest JSON
        std::ostringstream manifestJson;
        manifestJson << "{\n";
        manifestJson << "  \"chunksVersion\": 1,\n";
        manifestJson << "  \"lastSaved\": \"" << JsonUtils::getCurrentTimestamp() << "\",\n";
        manifestJson << "  \"chunks\": [\n";
        
        for (size_t i = 0; i < savedChunks.size(); ++i) {
            const auto& chunk = savedChunks[i];
            manifestJson << "    {\"x\": " << chunk.x << ", \"z\": " << chunk.z 
                        << ", \"lastModified\": \"" << JsonUtils::getCurrentTimestamp() << "\"}";
            if (i < savedChunks.size() - 1) {
                manifestJson << ",";
            }
            manifestJson << "\n";
        }
        
        manifestJson << "  ]\n";
        manifestJson << "}";
        
        return JsonUtils::writeJsonToFile(manifestPath, manifestJson.str());
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception updating chunk manifest: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::createBackup(const std::string& saveName) {
    try {
        std::string savePath = baseSaveDirectory_ + "/" + saveName;
        std::string backupPath = savePath + "_backup_" + std::to_string(std::time(nullptr));
        
        if (std::filesystem::exists(savePath)) {
            std::filesystem::copy(savePath, backupPath, std::filesystem::copy_options::recursive);
            std::cout << "[SaveManager] Created backup: " << backupPath << std::endl;
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception creating backup: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::ensureSaveDirectories(const std::string& savePath) {
    try {
        std::filesystem::create_directories(savePath);
        std::filesystem::create_directories(savePath + "/chunks");
        std::filesystem::create_directories(savePath + "/entities");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception creating save directories: " << e.what() << std::endl;
        return false;
    }
}

void SaveManager::handleSaveError(const std::string& saveName, const std::string& errorMessage) {
    std::cerr << "[SaveManager] Save error for '" << saveName << "': " << errorMessage << std::endl;
    logOperation("ERROR", saveName, false);
}

void SaveManager::logOperation(const std::string& operation, const std::string& saveName, bool success) {
    std::string status = success ? "SUCCESS" : "FAILED";
    std::cout << "[SaveManager] " << operation << " " << status << ": " << saveName 
              << " at " << JsonUtils::getCurrentTimestamp() << std::endl;
}

void SaveManager::autoSaveThreadFunction() {
    using namespace std::chrono_literals;
    
    while (!stopAutoSaveThread_) {
        // Sleep for the interval
        for (int i = 0; i < autoSaveInterval_ * 60 && !stopAutoSaveThread_; ++i) {
            std::this_thread::sleep_for(1s);
        }
        
        if (stopAutoSaveThread_) {
            break;
        }
        
        // Perform auto-save if not already saving/loading
        if (!isOperationInProgress()) {
            std::cout << "[SaveManager] Performing scheduled auto-save..." << std::endl;
            performAutoSave();
        } else {
            std::cout << "[SaveManager] Skipping auto-save - operation in progress" << std::endl;
        }
    }
}

} // namespace Core
} // namespace VoxelCastle
