#pragma once

#include "world/world_metadata.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace VoxelCastle {
namespace World {

/**
 * @brief Manages world persistence, creation, and directory structure
 * 
 * Handles the world persistence system including:
 * - World directory creation and management
 * - World metadata (level.dat) management
 * - World listing and selection
 * - Integration with world generation systems
 * 
 * This is separate from WorldManager which handles chunk/voxel operations.
 */
class WorldPersistenceManager {
public:
    struct WorldInfo {
        std::string worldName;
        std::string worldPath;
        WorldMetadata metadata;
        bool isValid = false;
    };
    
    // Constructor
    WorldPersistenceManager();
    ~WorldPersistenceManager() = default;
    
    // World creation
    bool CreateWorld(const std::string& worldName, uint64_t seed, 
                    const std::string& gameMode = "creative",
                    const std::string& worldType = "normal",
                    bool generateStructures = true);
    
    // World loading and listing
    std::vector<WorldInfo> GetWorldList();
    bool LoadWorld(const std::string& worldName);
    bool WorldExists(const std::string& worldName);
    
    // World directory management
    std::string GetWorldsDirectory() const;
    std::string GetWorldPath(const std::string& worldName) const;
    bool CreateWorldDirectories(const std::string& worldPath);
    
    // Current world management
    const WorldMetadata* GetCurrentWorldMetadata() const;
    bool SaveCurrentWorldMetadata();
    void UpdateLastPlayed();
    void AddPlayTime(uint64_t seconds);
    
    // World deletion (future implementation)
    bool DeleteWorld(const std::string& worldName);
    
    // Callbacks for integration
    using WorldCreatedCallback = std::function<void(const std::string& worldPath, const WorldMetadata& metadata)>;
    using WorldLoadedCallback = std::function<void(const std::string& worldPath, const WorldMetadata& metadata)>;
    
    void SetWorldCreatedCallback(WorldCreatedCallback callback);
    void SetWorldLoadedCallback(WorldLoadedCallback callback);
    
private:
    std::string worldsDirectory_;
    std::unique_ptr<WorldMetadata> currentWorldMetadata_;
    std::string currentWorldPath_;
    
    WorldCreatedCallback onWorldCreated_;
    WorldLoadedCallback onWorldLoaded_;
    
    // Internal helpers
    std::string SanitizeWorldName(const std::string& worldName) const;
    bool IsValidWorldDirectory(const std::string& worldPath) const;
    void InitializeWorldsDirectory();
};

} // namespace World
} // namespace VoxelCastle
