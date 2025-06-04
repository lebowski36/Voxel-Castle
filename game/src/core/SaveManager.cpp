#include "core/game.h"
#include "core/SaveManager.h"
#include "core/JsonUtils.h"
#include "world/world_manager.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <set>

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

bool SaveManager::saveGame(const std::string& saveName, const glm::vec3& playerPosition, 
                          CameraMode cameraMode, float cameraYaw, float cameraPitch, bool isQuickSave) {
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
    if (!saveMetadata(tempPath, playerPosition, cameraMode, cameraYaw, cameraPitch)) {
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
        std::cout << "[SaveManager] DEBUG: Clearing modified chunks tracking after successful save" << std::endl;
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

bool SaveManager::loadGame(const std::string& saveName, SaveInfo& saveInfo) {
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
    if (!loadMetadata(savePath, saveInfo)) {
        handleSaveError(saveName, "Failed to load metadata");
        isLoading_ = false;
        return false;
    }
    
    // Store the save name in saveInfo
    saveInfo.name = saveName;
    
    // Reset the world before loading (clear all existing chunks)
    if (worldManager_) {
        worldManager_->resetWorld();
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

bool SaveManager::quickSave(const glm::vec3& playerPosition, CameraMode cameraMode, float cameraYaw, float cameraPitch) {
    return saveGame("quicksave", playerPosition, cameraMode, cameraYaw, cameraPitch, true);
}

bool SaveManager::quickLoad(SaveInfo& saveInfo) {
    return loadGame("quicksave", saveInfo);
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
    // TODO: Get real position and camera mode from Game class when autosave is implemented
    glm::vec3 fallbackPosition(0.0f, 70.0f, 0.0f);
    CameraMode fallbackMode = CameraMode::FREE_FLYING;
    float fallbackYaw = -90.0f;
    float fallbackPitch = 0.0f;
    return saveGame("autosave", fallbackPosition, fallbackMode, fallbackYaw, fallbackPitch, false);
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

bool SaveManager::saveMetadata(const std::string& savePath, const glm::vec3& playerPosition, CameraMode cameraMode, float cameraYaw, float cameraPitch) {
    try {
        // Convert camera mode to string
        std::string cameraModeStr;
        switch (cameraMode) {
            case CameraMode::FREE_FLYING:
                cameraModeStr = "FREE_FLYING";
                break;
            case CameraMode::FIRST_PERSON:
                cameraModeStr = "FIRST_PERSON";
                break;
            default:
                cameraModeStr = "FREE_FLYING"; // Default fallback
                break;
        }
        
        // Get current game state
        uint64_t playTime = 0; // TODO: Get from game timer when implemented
        
        std::string metadataJson = JsonUtils::createMetadataJson(
            "1.0.0",
            currentSaveName_.empty() ? "Voxel World" : currentSaveName_,
            playerPosition,
            playTime,
            cameraModeStr,
            cameraYaw,
            cameraPitch
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
        float cameraYaw, cameraPitch;
        
        if (JsonUtils::parseMetadataJson(jsonContent, version, worldName, playerPosition, playTime, cameraMode, cameraYaw, cameraPitch)) {
            saveInfo.displayName = worldName;
            saveInfo.playTimeSeconds = playTime;
            saveInfo.hasQuickSave = false; // TODO: Implement
            saveInfo.hasAutoSave = false;  // TODO: Implement
            saveInfo.screenshotPath = savePath + "/screenshot.png";
            saveInfo.timestamp = JsonUtils::getCurrentTimestamp();
            saveInfo.lastPlayedDate = JsonUtils::getCurrentTimestamp();
            
            // Store position and camera mode
            saveInfo.playerPosition = playerPosition;
            saveInfo.cameraYaw = cameraYaw;
            saveInfo.cameraPitch = cameraPitch;
            if (cameraMode == "FREE_FLYING") {
                saveInfo.cameraMode = CameraMode::FREE_FLYING;
            } else if (cameraMode == "FIRST_PERSON") {
                saveInfo.cameraMode = CameraMode::FIRST_PERSON;
            } else {
                saveInfo.cameraMode = CameraMode::FREE_FLYING; // Default fallback
            }
            
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
        
        std::cout << "[SaveManager] DEBUG: Getting modified chunks from WorldManager" << std::endl;
        std::cout << "[SaveManager] DEBUG: Found " << chunksToSave.size() << " modified chunks to save" << std::endl;
        for (const auto& coord : chunksToSave) {
            std::cout << "[SaveManager] DEBUG: Modified chunk: (" << coord.x << ", " << coord.z << ")" << std::endl;
        }
        
        std::cout << "[SaveManager] Saving " << chunksToSave.size() << " chunks" << std::endl;
        
        // Update the chunk manifest
        if (!updateChunkManifest(savePath, chunksToSave)) {
            return false;
        }
        
        // Save each modified chunk
        size_t chunksSaved = 0;
        for (const auto& chunkCoord : chunksToSave) {
            // Get the chunk column
            auto* chunkColumn = worldManager_->getChunkColumn(chunkCoord.x, chunkCoord.z);
            if (chunkColumn) {
                // Create a binary file for the chunk
                std::string chunkFilePath = chunksPath + "/chunk_" + 
                                           std::to_string(chunkCoord.x) + "_" + 
                                           std::to_string(chunkCoord.z) + ".bin";
                
                // Open a binary file for writing
                std::ofstream chunkFile(chunkFilePath, std::ios::binary);
                if (!chunkFile) {
                    std::cerr << "[SaveManager] Failed to create chunk file: " << chunkFilePath << std::endl;
                    continue;
                }
                
                // Write a simple header with magic number "VCWC" (Voxel Castle World Chunk)
                const char* magicNumber = "VCWC";
                chunkFile.write(magicNumber, 4);
                
                // Write version (uint32_t)
                uint32_t version = 1;
                chunkFile.write(reinterpret_cast<char*>(&version), sizeof(version));
                
                // Write chunk coordinates (2 int64_t)
                int64_t x = chunkCoord.x;
                int64_t z = chunkCoord.z;
                chunkFile.write(reinterpret_cast<char*>(&x), sizeof(x));
                chunkFile.write(reinterpret_cast<char*>(&z), sizeof(z));
                
                // Write segment bitmap - which segments exist
                uint16_t segmentBitmap = 0;
                for (int i = 0; i < ::VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                    auto* segment = chunkColumn->getSegmentByIndex(i);
                    if (segment && segment->isGenerated()) {
                        segmentBitmap |= (1 << i);
                    }
                }
                chunkFile.write(reinterpret_cast<char*>(&segmentBitmap), sizeof(segmentBitmap));
                
                // For each segment that exists, write its data
                for (int i = 0; i < ::VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                    if (segmentBitmap & (1 << i)) {
                        auto* segment = chunkColumn->getSegmentByIndex(i);
                        if (segment) {
                            // Write segment data directly (uncompressed for this phase)
                            for (int y = 0; y < ::VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT; ++y) {
                                for (int z = 0; z < ::VoxelCastle::World::ChunkSegment::CHUNK_DEPTH; ++z) {
                                    for (int x = 0; x < ::VoxelCastle::World::ChunkSegment::CHUNK_WIDTH; ++x) {
                                        VoxelEngine::World::Voxel voxel = segment->getVoxel(x, y, z);
                                        chunkFile.write(reinterpret_cast<char*>(&voxel), sizeof(voxel));
                                    }
                                }
                            }
                        }
                    }
                }
                
                chunkFile.close();
                chunksSaved++;
            }
        }
        
        std::cout << "[SaveManager] Successfully saved " << chunksSaved << " of " << chunksToSave.size() << " chunks" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception saving chunks: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::loadChunks(const std::string& savePath) {
    if (!worldManager_) {
        std::cerr << "[SaveManager] WorldManager not available for chunk loading" << std::endl;
        return false;
    }
    
    try {
        // Set the world manager to loading mode to prevent world generation
        worldManager_->setLoadingState(true);
        
        std::string chunksPath = savePath + "/chunks";
        
        // Check if chunks directory exists
        if (!std::filesystem::exists(chunksPath)) {
            std::cerr << "[SaveManager] Chunks directory does not exist: " << chunksPath << std::endl;
            worldManager_->setLoadingState(false);
            return false;
        }
        
        // Read chunk manifest to identify which chunks to load
        std::string manifestPath = chunksPath + "/manifest.json";
        std::string manifestJson;
        if (!JsonUtils::readJsonFromFile(manifestPath, manifestJson)) {
            std::cerr << "[SaveManager] Failed to read chunk manifest: " << manifestPath << std::endl;
            return false;
        }
        
        // Parse the manifest
        std::vector<::VoxelCastle::World::WorldCoordXZ> chunksToLoad;
        std::istringstream manifestStream(manifestJson);
        std::string line;
        bool inChunksArray = false;
        while (std::getline(manifestStream, line)) {
            if (line.find("\"chunks\":") != std::string::npos) {
                inChunksArray = true;
                continue;
            }
            
            if (inChunksArray && line.find("{") != std::string::npos) {
                // Parse chunk coordinates
                size_t xPos = line.find("\"x\":");
                size_t zPos = line.find("\"z\":");
                
                if (xPos != std::string::npos && zPos != std::string::npos) {
                    int64_t x = 0, z = 0;
                    try {
                        size_t xValueStart = line.find(':', xPos) + 1;
                        size_t xValueEnd = line.find(',', xValueStart);
                        x = std::stoll(line.substr(xValueStart, xValueEnd - xValueStart));
                        
                        size_t zValueStart = line.find(':', zPos) + 1;
                        size_t zValueEnd = line.find(',', zValueStart);
                        if (zValueEnd == std::string::npos) {
                            zValueEnd = line.find('}', zValueStart);
                        }
                        z = std::stoll(line.substr(zValueStart, zValueEnd - zValueStart));
                        
                        ::VoxelCastle::World::WorldCoordXZ coord;
                        coord.x = x;
                        coord.z = z;
                        chunksToLoad.push_back(coord);
                    } catch (const std::exception& e) {
                        std::cerr << "[SaveManager] Error parsing chunk coordinates: " << e.what() << std::endl;
                    }
                }
            }
        }
        
        if (chunksToLoad.empty()) {
            std::cout << "[SaveManager] No chunks to load from manifest" << std::endl;
            return true; // Not an error, just no chunks
        }
        
        std::cout << "[SaveManager] Loading " << chunksToLoad.size() << " chunks" << std::endl;
        
        for (const auto& coord : chunksToLoad) {
            std::cout << "[SaveManager] DEBUG: Will load chunk: (" << coord.x << ", " << coord.z << ")" << std::endl;
        }
        
        // Load each chunk
        size_t chunksLoaded = 0;
        for (const auto& chunkCoord : chunksToLoad) {
            std::string chunkFilePath = chunksPath + "/chunk_" + 
                                       std::to_string(chunkCoord.x) + "_" + 
                                       std::to_string(chunkCoord.z) + ".bin";
            
            // Check if the chunk file exists
            if (!std::filesystem::exists(chunkFilePath)) {
                std::cerr << "[SaveManager] Chunk file not found: " << chunkFilePath << std::endl;
                continue;
            }
            
            // Open the chunk file for reading
            std::ifstream chunkFile(chunkFilePath, std::ios::binary);
            if (!chunkFile) {
                std::cerr << "[SaveManager] Failed to open chunk file: " << chunkFilePath << std::endl;
                continue;
            }
            
            // Read and verify magic number
            char magicBuffer[5] = {0};
            chunkFile.read(magicBuffer, 4);
            if (std::string(magicBuffer) != "VCWC") {
                std::cerr << "[SaveManager] Invalid chunk file format (wrong magic number): " << chunkFilePath << std::endl;
                continue;
            }
            
            // Read version
            uint32_t version;
            chunkFile.read(reinterpret_cast<char*>(&version), sizeof(version));
            if (version != 1) {
                std::cerr << "[SaveManager] Unsupported chunk version: " << version << std::endl;
                continue;
            }
            
            // Read chunk coordinates
            int64_t x, z;
            chunkFile.read(reinterpret_cast<char*>(&x), sizeof(x));
            chunkFile.read(reinterpret_cast<char*>(&z), sizeof(z));
            
            // Verify coordinates match
            if (x != chunkCoord.x || z != chunkCoord.z) {
                std::cerr << "[SaveManager] Chunk file coordinates mismatch: " 
                      << "expected (" << chunkCoord.x << ", " << chunkCoord.z << "), "
                      << "found (" << x << ", " << z << ")" << std::endl;
                continue;
            }
            
            // Read segment bitmap
            uint16_t segmentBitmap;
            chunkFile.read(reinterpret_cast<char*>(&segmentBitmap), sizeof(segmentBitmap));
            
            // Mark this chunk as loaded to prevent world generation
            worldManager_->markChunkLoaded(chunkCoord.x, chunkCoord.z);
            
            // Get or create the chunk column without world generation
            auto* chunkColumn = worldManager_->getOrCreateEmptyChunkColumn(chunkCoord.x, chunkCoord.z);
            if (!chunkColumn) {
                std::cerr << "[SaveManager] Failed to create chunk column for coordinates: " 
                      << "(" << chunkCoord.x << ", " << chunkCoord.z << ")" << std::endl;
                continue;
            }
            
            // Load each segment that exists in the file
            for (int i = 0; i < ::VoxelCastle::World::ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
                if (segmentBitmap & (1 << i)) {
                    auto* segment = chunkColumn->getOrCreateSegment(i);
                    if (segment) {
                        // Read segment data
                        for (int y = 0; y < ::VoxelCastle::World::ChunkSegment::CHUNK_HEIGHT; ++y) {
                            for (int z = 0; z < ::VoxelCastle::World::ChunkSegment::CHUNK_DEPTH; ++z) {
                                for (int x = 0; x < ::VoxelCastle::World::ChunkSegment::CHUNK_WIDTH; ++x) {
                                    VoxelEngine::World::Voxel voxel;
                                    chunkFile.read(reinterpret_cast<char*>(&voxel), sizeof(voxel));
                                    
                                    if (chunkFile.good()) {
                                        segment->setVoxel(x, y, z, voxel);
                                    }
                                }
                            }
                        }
                        
                        // Mark segment as generated and dirty to trigger mesh rebuild
                        segment->setGenerated(true);
                        segment->markDirty(true);
                    }
                }
            }
            
            chunkFile.close();
            chunksLoaded++;
        }
        
        std::cout << "[SaveManager] Successfully loaded " << chunksLoaded << " of " << chunksToLoad.size() << " chunks" << std::endl;
        
        // CRITICAL FIX: Mark loaded chunks as modified so they can be saved again in future saves
        if (chunksLoaded > 0) {
            std::cout << "[SaveManager] DEBUG: Re-marking " << chunksLoaded << " loaded chunks as modified for future saves" << std::endl;
            for (const auto& chunkCoord : chunksToLoad) {
                worldManager_->markChunkAsModified(chunkCoord.x, chunkCoord.z);
            }
        }
        
        // Reset loading state but keep tracked loaded chunks
        worldManager_->setLoadingState(false);
        
        // Note: Segments are automatically marked dirty during setVoxel calls, no need to mark all dirty
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception loading chunks: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::updateChunkManifest(const std::string& savePath, 
                                     const std::vector<::VoxelCastle::World::WorldCoordXZ>& savedChunks) {
    try {
        std::string manifestPath = savePath + "/chunks/manifest.json";
        
        // Read existing manifest to merge with new chunks
        std::set<::VoxelCastle::World::WorldCoordXZ> allChunks;
        
        // Parse existing manifest if it exists
        if (std::filesystem::exists(manifestPath)) {
            std::string existingManifest;
            if (JsonUtils::readJsonFromFile(manifestPath, existingManifest)) {
                std::istringstream manifestStream(existingManifest);
                std::string line;
                bool inChunksArray = false;
                while (std::getline(manifestStream, line)) {
                    if (line.find("\"chunks\":") != std::string::npos) {
                        inChunksArray = true;
                        continue;
                    }
                    
                    if (inChunksArray && line.find("{") != std::string::npos) {
                        // Parse chunk coordinates
                        size_t xPos = line.find("\"x\":");
                        size_t zPos = line.find("\"z\":");
                        
                        if (xPos != std::string::npos && zPos != std::string::npos) {
                            try {
                                size_t xValueStart = line.find(':', xPos) + 1;
                                size_t xValueEnd = line.find(',', xValueStart);
                                int64_t x = std::stoll(line.substr(xValueStart, xValueEnd - xValueStart));
                                
                                size_t zValueStart = line.find(':', zPos) + 1;
                                size_t zValueEnd = line.find(',', zValueStart);
                                if (zValueEnd == std::string::npos) {
                                    zValueEnd = line.find('}', zValueStart);
                                }
                                int64_t z = std::stoll(line.substr(zValueStart, zValueEnd - zValueStart));
                                
                                ::VoxelCastle::World::WorldCoordXZ coord;
                                coord.x = x;
                                coord.z = z;
                                allChunks.insert(coord);
                            } catch (const std::exception& e) {
                                std::cerr << "[SaveManager] Error parsing existing chunk coordinates: " << e.what() << std::endl;
                            }
                        }
                    }
                }
            }
        }
        
        // Add new chunks to the set (set automatically handles duplicates)
        for (const auto& chunk : savedChunks) {
            allChunks.insert(chunk);
        }
        
        // Create updated manifest JSON with all chunks
        std::ostringstream manifestJson;
        manifestJson << "{\n";
        manifestJson << "  \"chunksVersion\": 1,\n";
        manifestJson << "  \"lastSaved\": \"" << JsonUtils::getCurrentTimestamp() << "\",\n";
        manifestJson << "  \"chunks\": [\n";
        
        size_t index = 0;
        for (const auto& chunk : allChunks) {
            manifestJson << "    {\"x\": " << chunk.x << ", \"z\": " << chunk.z 
                        << ", \"lastModified\": \"" << JsonUtils::getCurrentTimestamp() << "\"}";
            if (index < allChunks.size() - 1) {
                manifestJson << ",";
            }
            manifestJson << "\n";
            index++;
        }
        
        manifestJson << "  ]\n";
        manifestJson << "}";
        
        std::cout << "[SaveManager] Updated manifest with " << allChunks.size() << " total chunks (" 
                  << savedChunks.size() << " newly saved)" << std::endl;
        
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
