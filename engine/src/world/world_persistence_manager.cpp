#include "world/world_persistence_manager.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

namespace VoxelCastle {
namespace World {

WorldPersistenceManager::WorldPersistenceManager() {
    InitializeWorldsDirectory();
}

bool WorldPersistenceManager::CreateWorld(const std::string& worldName, uint64_t seed,
                                         const std::string& gameMode,
                                         const std::string& worldType,
                                         bool generateStructures) {
    try {
        // Sanitize world name for filesystem
        std::string sanitizedName = SanitizeWorldName(worldName);
        if (sanitizedName.empty()) {
            std::cerr << "[ERROR] Invalid world name: " << worldName << std::endl;
            return false;
        }
        
        // Check if world already exists
        if (WorldExists(sanitizedName)) {
            std::cerr << "[ERROR] World already exists: " << sanitizedName << std::endl;
            return false;
        }
        
        // Create world path
        std::string worldPath = GetWorldPath(sanitizedName);
        
        // Create world directories
        if (!CreateWorldDirectories(worldPath)) {
            std::cerr << "[ERROR] Failed to create world directories: " << worldPath << std::endl;
            return false;
        }
        
        // Create world metadata
        WorldMetadata metadata(worldName, seed);
        metadata.gameMode = gameMode;
        metadata.worldType = worldType;
        metadata.generateStructures = generateStructures;
        
        // Save metadata to level.dat
        if (!metadata.SaveToFile(worldPath)) {
            std::cerr << "[ERROR] Failed to save world metadata for: " << worldName << std::endl;
            return false;
        }
        
        // Store as current world
        currentWorldMetadata_ = std::make_unique<WorldMetadata>(metadata);
        currentWorldPath_ = worldPath;
        
        // Trigger callback if set
        if (onWorldCreated_) {
            onWorldCreated_(worldPath, metadata);
        }
        
        std::cout << "[INFO] Successfully created world: " << worldName << " at " << worldPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception while creating world '" << worldName << "': " << e.what() << std::endl;
        return false;
    }
}

bool WorldPersistenceManager::CreateWorld(const std::string& worldName, const WorldMetadata& metadata) {
    try {
        // Sanitize world name for filesystem
        std::string sanitizedName = SanitizeWorldName(worldName);
        if (sanitizedName.empty()) {
            std::cerr << "[ERROR] Invalid world name: " << worldName << std::endl;
            return false;
        }
        
        // Check if world already exists
        if (WorldExists(sanitizedName)) {
            std::cerr << "[ERROR] World already exists: " << sanitizedName << std::endl;
            return false;
        }
        
        // Create world path
        std::string worldPath = GetWorldPath(sanitizedName);
        
        // Create world directories
        if (!CreateWorldDirectories(worldPath)) {
            std::cerr << "[ERROR] Failed to create world directories: " << worldPath << std::endl;
            return false;
        }
        
        // Copy metadata and set timestamps
        WorldMetadata metadataCopy = metadata;
        metadataCopy.createdDate = std::chrono::system_clock::now();
        metadataCopy.lastPlayed = metadataCopy.createdDate;
        
        // Save metadata to level.dat
        if (!metadataCopy.SaveToFile(worldPath)) {
            std::cerr << "[ERROR] Failed to save world metadata for: " << worldName << std::endl;
            return false;
        }
        
        // Store as current world
        currentWorldMetadata_ = std::make_unique<WorldMetadata>(metadataCopy);
        currentWorldPath_ = worldPath;
        
        // Trigger callback if set
        if (onWorldCreated_) {
            onWorldCreated_(worldPath, metadataCopy);
        }
        
        std::cout << "[INFO] Successfully created world: " << worldName << " at " << worldPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception while creating world '" << worldName << "': " << e.what() << std::endl;
        return false;
    }
}

std::vector<WorldPersistenceManager::WorldInfo> WorldPersistenceManager::GetWorldList() {
    std::vector<WorldInfo> worlds;
    
    try {
        if (!fs::exists(worldsDirectory_) || !fs::is_directory(worldsDirectory_)) {
            std::cout << "[INFO] Worlds directory does not exist or is not a directory: " << worldsDirectory_ << std::endl;
            return worlds;
        }
        
        // Iterate through world directories
        for (const auto& entry : fs::directory_iterator(worldsDirectory_)) {
            if (entry.is_directory()) {
                std::string worldPath = entry.path().string();
                
                if (IsValidWorldDirectory(worldPath)) {
                    WorldInfo info;
                    info.worldPath = worldPath;
                    info.worldName = entry.path().filename().string();
                    
                    // Try to load metadata
                    if (info.metadata.LoadFromFile(worldPath)) {
                        info.isValid = true;
                    } else {
                        info.isValid = false;
                        std::cerr << "[WARNING] Found world directory without valid level.dat: " << worldPath << std::endl;
                    }
                    
                    worlds.push_back(info);
                }
            }
        }
        
        // Sort worlds by last played time (most recent first)
        std::sort(worlds.begin(), worlds.end(), [](const WorldInfo& a, const WorldInfo& b) {
            return a.metadata.lastPlayed > b.metadata.lastPlayed;
        });
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception while getting world list: " << e.what() << std::endl;
    }
    
    return worlds;
}

bool WorldPersistenceManager::LoadWorld(const std::string& worldName) {
    try {
        std::string worldPath = GetWorldPath(worldName);
        
        if (!IsValidWorldDirectory(worldPath)) {
            std::cerr << "[ERROR] Invalid world directory: " << worldPath << std::endl;
            return false;
        }
        
        // Load metadata
        auto metadata = std::make_unique<WorldMetadata>();
        if (!metadata->LoadFromFile(worldPath)) {
            std::cerr << "[ERROR] Failed to load world metadata for: " << worldName << std::endl;
            return false;
        }
        
        // Update last played time
        metadata->lastPlayed = std::chrono::system_clock::now();
        metadata->SaveToFile(worldPath);
        
        // Set as current world
        currentWorldMetadata_ = std::move(metadata);
        currentWorldPath_ = worldPath;
        
        // Trigger callback if set
        if (onWorldLoaded_) {
            onWorldLoaded_(worldPath, *currentWorldMetadata_);
        }
        
        std::cout << "[INFO] Successfully loaded world: " << worldName << " from " << worldPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception while loading world '" << worldName << "': " << e.what() << std::endl;
        return false;
    }
}

bool WorldPersistenceManager::WorldExists(const std::string& worldName) {
    std::string worldPath = GetWorldPath(worldName);
    return IsValidWorldDirectory(worldPath);
}

std::string WorldPersistenceManager::GetWorldsDirectory() const {
    return worldsDirectory_;
}

std::string WorldPersistenceManager::GetWorldPath(const std::string& worldName) const {
    return worldsDirectory_ + "/" + SanitizeWorldName(worldName);
}

bool WorldPersistenceManager::CreateWorldDirectories(const std::string& worldPath) {
    try {
        // Create main world directory
        fs::create_directories(worldPath);
        
        // Create subdirectories
        fs::create_directories(worldPath + "/region");
        fs::create_directories(worldPath + "/playerdata");
        fs::create_directories(worldPath + "/data");
        fs::create_directories(worldPath + "/screenshots");
        
        std::cout << "[INFO] Created world directory structure: " << worldPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to create world directories '" << worldPath << "': " << e.what() << std::endl;
        return false;
    }
}

const WorldMetadata* WorldPersistenceManager::GetCurrentWorldMetadata() const {
    return currentWorldMetadata_.get();
}

bool WorldPersistenceManager::SaveCurrentWorldMetadata() {
    if (!currentWorldMetadata_ || currentWorldPath_.empty()) {
        std::cerr << "[ERROR] No current world to save metadata for" << std::endl;
        return false;
    }
    
    return currentWorldMetadata_->SaveToFile(currentWorldPath_);
}

void WorldPersistenceManager::UpdateLastPlayed() {
    if (currentWorldMetadata_) {
        currentWorldMetadata_->lastPlayed = std::chrono::system_clock::now();
    }
}

void WorldPersistenceManager::AddPlayTime(uint64_t seconds) {
    if (currentWorldMetadata_) {
        currentWorldMetadata_->playTime += seconds;
    }
}

bool WorldPersistenceManager::DeleteWorld(const std::string& worldName) {
    try {
        std::string worldPath = GetWorldPath(worldName);
        
        if (!IsValidWorldDirectory(worldPath)) {
            std::cerr << "[ERROR] World does not exist: " << worldName << std::endl;
            return false;
        }
        
        // Remove the entire world directory
        fs::remove_all(worldPath);
        
        // Clear current world if it's the one being deleted
        if (currentWorldMetadata_ && currentWorldMetadata_->worldName == worldName) {
            currentWorldMetadata_.reset();
            currentWorldPath_.clear();
        }
        
        std::cout << "[INFO] Successfully deleted world: " << worldName << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to delete world '" << worldName << "': " << e.what() << std::endl;
        return false;
    }
}

void WorldPersistenceManager::SetWorldCreatedCallback(WorldCreatedCallback callback) {
    onWorldCreated_ = callback;
}

void WorldPersistenceManager::SetWorldLoadedCallback(WorldLoadedCallback callback) {
    onWorldLoaded_ = callback;
}

std::string WorldPersistenceManager::SanitizeWorldName(const std::string& worldName) const {
    std::string sanitized;
    
    for (char c : worldName) {
        if (std::isalnum(c) || c == '_' || c == '-' || c == ' ') {
            if (c == ' ') {
                sanitized += '_';
            } else {
                sanitized += c;
            }
        }
        // Skip invalid characters
    }
    
    // Trim length to reasonable limit
    if (sanitized.length() > 50) {
        sanitized = sanitized.substr(0, 50);
    }
    
    return sanitized;
}

bool WorldPersistenceManager::IsValidWorldDirectory(const std::string& worldPath) const {
    return fs::exists(worldPath) && 
           fs::is_directory(worldPath) && 
           fs::exists(worldPath + "/level.dat");
}

void WorldPersistenceManager::InitializeWorldsDirectory() {
    // Set worlds directory relative to executable
    worldsDirectory_ = "./worlds";
    
    try {
        // Create worlds directory if it doesn't exist
        if (!fs::exists(worldsDirectory_)) {
            fs::create_directories(worldsDirectory_);
            std::cout << "[INFO] Created worlds directory: " << worldsDirectory_ << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to initialize worlds directory: " << e.what() << std::endl;
        // Fallback to current directory
        worldsDirectory_ = "./";
    }
}

} // namespace World
} // namespace VoxelCastle
