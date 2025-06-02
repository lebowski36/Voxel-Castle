#ifndef VOXELCASTLE_CORE_SAVE_MANAGER_H
#define VOXELCASTLE_CORE_SAVE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <glm/vec3.hpp>
#include "core/CameraMode.h"

// Forward declarations
class Game; // Game class is in global scope, not in VoxelCastle::Core
namespace VoxelCastle {
namespace Core {
// Forward declarations for VoxelCastle::Core classes
}
namespace World {
class WorldManager;
struct WorldCoordXZ;
}
}

namespace VoxelCastle {
namespace Core {

/**
 * @struct SaveInfo
 * @brief Information about a save file.
 */
struct SaveInfo {
    std::string name;
    std::string displayName;
    std::string timestamp;
    uint64_t playTimeSeconds;
    std::string screenshotPath;
    std::string lastPlayedDate;
    bool hasQuickSave;
    bool hasAutoSave;
    
    // Game state information
    glm::vec3 playerPosition;
    CameraMode cameraMode;
};

/**
 * @class SaveManager
 * @brief Manages save and load operations for the game.
 */
class SaveManager {
public:
    /**
     * @brief Constructor.
     * @param game Pointer to the main game object.
     */
    explicit SaveManager(Game* game);

    /**
     * @brief Constructor with WorldManager directly.
     * @param worldManager Pointer to the world manager.
     */
    explicit SaveManager(VoxelCastle::World::WorldManager* worldManager);

    /**
     * @brief Destructor.
     */
    ~SaveManager();

    /**
     * @brief Initialize the save manager with a base save directory.
     * @param baseSaveDir The base directory for save files.
     * @return True if initialization was successful.
     */
    bool initialize(const std::string& baseSaveDir);

    /**
     * @brief Set the world manager reference (avoids circular dependencies).
     * @param worldManager Pointer to the world manager.
     */
    void setWorldManager(VoxelCastle::World::WorldManager* worldManager);

    /**
     * @brief Create a new save with the given name.
     * @param saveName The internal name of the save.
     * @param displayName The display name for the save (optional).
     * @return True if the save was created successfully.
     */
    bool createSave(const std::string& saveName, const std::string& displayName = "");

    /**
     * @brief Save the current game state.
     * @param saveName The name of the save.
     * @param playerPosition Current player/camera position.
     * @param cameraMode Current camera mode.
     * @param isQuickSave Whether this is a quick save operation.
     * @return True if the save was successful.
     */
    bool saveGame(const std::string& saveName, const glm::vec3& playerPosition, 
                  CameraMode cameraMode, bool isQuickSave = false);

    /**
     * @brief Load a game from a save file.
     * @param saveName The name of the save to load.
     * @param saveInfo Output parameter for loaded save information.
     * @return True if the load was successful.
     */
    bool loadGame(const std::string& saveName, SaveInfo& saveInfo);

    /**
     * @brief Perform a quick save operation.
     * @param playerPosition Current player/camera position.
     * @param cameraMode Current camera mode.
     * @return True if the quick save was successful.
     */
    bool quickSave(const glm::vec3& playerPosition, CameraMode cameraMode);

    /**
     * @brief Perform a quick load operation.
     * @param saveInfo Output parameter for loaded save information.
     * @return True if the quick load was successful.
     */
    bool quickLoad(SaveInfo& saveInfo);

    /**
     * @brief Start auto-save functionality.
     * @param intervalMinutes Time interval between auto-saves in minutes.
     */
    void startAutoSave(int intervalMinutes);

    /**
     * @brief Stop auto-save functionality.
     */
    void stopAutoSave();

    /**
     * @brief Force an immediate auto-save.
     * @return True if the auto-save was successful.
     */
    bool performAutoSave();

    /**
     * @brief Get a list of available saves.
     * @return Vector of SaveInfo structures.
     */
    std::vector<SaveInfo> listSaves();

    /**
     * @brief Get detailed information about a specific save.
     * @param saveName The name of the save.
     * @return SaveInfo structure with save details.
     */
    SaveInfo getSaveInfo(const std::string& saveName);

    /**
     * @brief Delete a save file.
     * @param saveName The name of the save to delete.
     * @return True if the save was deleted successfully.
     */
    bool deleteSave(const std::string& saveName);

    /**
     * @brief Check if a save/load operation is currently in progress.
     * @return True if an operation is in progress.
     */
    bool isOperationInProgress() const;

    /**
     * @brief Get the name of the currently loaded save.
     * @return The current save name, or empty string if no save is loaded.
     */
    std::string getCurrentSaveName() const;

private:
    /**
     * @brief Save metadata for a save file.
     * @param savePath The path to the save directory.
     * @param playerPosition Current player/camera position.
     * @param cameraMode Current camera mode.
     * @return True if metadata was saved successfully.
     */
    bool saveMetadata(const std::string& savePath, const glm::vec3& playerPosition, CameraMode cameraMode);

    /**
     * @brief Load metadata from a save file.
     * @param savePath The path to the save directory.
     * @param saveInfo Output parameter for save information.
     * @return True if metadata was loaded successfully.
     */
    bool loadMetadata(const std::string& savePath, SaveInfo& saveInfo);

    /**
     * @brief Save player data.
     * @param savePath The path to the save directory.
     * @return True if player data was saved successfully.
     */
    bool savePlayerData(const std::string& savePath);

    /**
     * @brief Load player data.
     * @param savePath The path to the save directory.
     * @return True if player data was loaded successfully.
     */
    bool loadPlayerData(const std::string& savePath);

    /**
     * @brief Save modified chunks.
     * @param savePath The path to the save directory.
     * @param incrementalOnly If true, only save chunks that have been modified.
     * @return True if chunks were saved successfully.
     */
    bool saveChunks(const std::string& savePath, bool incrementalOnly = false);

    /**
     * @brief Load chunks from save files.
     * @param savePath The path to the save directory.
     * @return True if chunks were loaded successfully.
     */
    bool loadChunks(const std::string& savePath);

    /**
     * @brief Update the chunk manifest file.
     * @param savePath The path to the save directory.
     * @param savedChunks List of chunks that were saved.
     * @return True if manifest was updated successfully.
     */
    bool updateChunkManifest(const std::string& savePath, 
                            const std::vector<::VoxelCastle::World::WorldCoordXZ>& savedChunks);

    /**
     * @brief Create a backup of an existing save.
     * @param saveName The name of the save to back up.
     * @return True if backup was created successfully.
     */
    bool createBackup(const std::string& saveName);

    /**
     * @brief Ensure save directories exist.
     * @param savePath The path to the save directory.
     * @return True if directories were created successfully.
     */
    bool ensureSaveDirectories(const std::string& savePath);

    /**
     * @brief Handle save errors and recovery.
     * @param saveName The name of the save that failed.
     * @param errorMessage The error message.
     */
    void handleSaveError(const std::string& saveName, const std::string& errorMessage);

    /**
     * @brief Log save/load operations.
     * @param operation The operation being performed.
     * @param saveName The name of the save.
     * @param success Whether the operation was successful.
     */
    void logOperation(const std::string& operation, const std::string& saveName, bool success);

    /**
     * @brief Auto-save thread function.
     */
    void autoSaveThreadFunction();

    // Member variables
    Game* game_;                          ///< Pointer to the main game object
    ::VoxelCastle::World::WorldManager* worldManager_; ///< Pointer to the world manager
    std::string baseSaveDirectory_;       ///< Base directory for save files
    std::string currentSaveName_;         ///< Name of the currently loaded save
    
    // Operation state
    std::atomic<bool> isSaving_;          ///< Whether a save operation is in progress
    std::atomic<bool> isLoading_;         ///< Whether a load operation is in progress
    
    // Auto-save
    bool autoSaveEnabled_;                ///< Whether auto-save is enabled
    int autoSaveInterval_;                ///< Minutes between auto-saves
    int autoSaveRotateCount_;             ///< Number of auto-save backups to keep
    std::thread autoSaveThread_;          ///< Auto-save background thread
    std::atomic<bool> stopAutoSaveThread_; ///< Flag to stop auto-save thread
    std::mutex saveMutex_;                ///< Mutex for thread safety

    // Save statistics
    struct SaveStats {
        size_t totalChunks;
        size_t modifiedChunks;
        double saveTimeMs;
    } lastSaveStats_;                     ///< Statistics from the last save operation
};

} // namespace Core
} // namespace VoxelCastle

#endif // VOXELCASTLE_CORE_SAVE_MANAGER_H
