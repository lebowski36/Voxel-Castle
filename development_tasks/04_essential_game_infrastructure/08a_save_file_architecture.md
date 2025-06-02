# Save File Architecture Implementation Plan

## Overview
This document details the implementation plan for the save file architecture in the Voxel Castle game. The save system will provide persistence for voxel modifications, player state, and game settings, with support for auto-saving, incremental saving, and crash recovery.

## Design Goals

1. **Efficiency**: Minimize storage requirements and performance impact
2. **Reliability**: Ensure data integrity even during crashes or power failures
3. **Extensibility**: Support future game features through versioned saves
4. **Usability**: Provide intuitive save/load functionality for users

## Save File Format

### Directory Structure
```
saves/
  └── [save_name]/
      ├── metadata.json       # Save metadata (version, timestamp, etc.)
      ├── player.json         # Player state (position, inventory, etc.)
      ├── settings.json       # Game settings specific to this save
      ├── screenshot.png      # Screenshot thumbnail for save selection UI
      ├── chunks/             # Directory containing chunk data
      │   ├── manifest.json   # List of all saved chunks with timestamps
      │   ├── chunk_X_Z.bin   # Binary chunk data files (compressed)
      │   └── ...
      └── entities/           # Directory for entity data
          ├── manifest.json   # List of all saved entities
          └── entities.bin    # Binary entity data
```

### File Formats

1. **metadata.json**
```json
{
  "version": "1.0.0",
  "saveDate": "2025-06-02T15:30:45Z",
  "playTime": 3600,
  "worldSeed": 12345,
  "worldName": "My Voxel World",
  "playerPosition": {"x": 123.45, "y": 64.0, "z": -78.9},
  "cameraMode": "STRATEGIC_MODE"
}
```

2. **chunk_X_Z.bin** (Binary Format)
- Header (16 bytes):
  - Magic number (4 bytes): "VCWC" (Voxel Castle World Chunk)
  - Version (4 bytes): e.g., 0x00000001
  - Chunk coordinates (8 bytes): X (4 bytes) and Z (4 bytes)
- Segment Bitmap (2 bytes): Indicates which segments are stored
- Per-segment Data:
  - Compression type (1 byte): 0=none, 1=zlib, 2=LZ4
  - Compressed size (4 bytes)
  - Segment data (compressed binary voxel data)

3. **chunks/manifest.json**
```json
{
  "chunksVersion": 1,
  "lastSaved": "2025-06-02T15:30:45Z",
  "chunks": [
    {"x": 0, "z": 0, "lastModified": "2025-06-02T15:28:30Z"},
    {"x": 1, "z": 0, "lastModified": "2025-06-02T15:30:00Z"},
    {"x": 0, "z": 1, "lastModified": "2025-06-02T15:29:15Z"}
  ]
}
```

## Core Components

### 1. Serialization Framework

#### Serializable Interface
```cpp
class Serializable {
public:
    virtual ~Serializable() = default;
    
    // Serialize to binary
    virtual void serialize(BinaryWriter& writer) const = 0;
    
    // Deserialize from binary
    virtual void deserialize(BinaryReader& reader) = 0;
    
    // Get data version number
    virtual uint32_t getVersion() const = 0;
};
```

#### Binary Reader/Writer
```cpp
class BinaryWriter {
public:
    void write(const void* data, size_t size);
    void writeInt8(int8_t value);
    void writeUInt8(uint8_t value);
    void writeInt32(int32_t value);
    void writeUInt32(uint32_t value);
    void writeFloat(float value);
    void writeString(const std::string& value);
    void writeCompressed(const void* data, size_t size);
};

class BinaryReader {
public:
    void read(void* data, size_t size);
    int8_t readInt8();
    uint8_t readUInt8();
    int32_t readInt32();
    uint32_t readUInt32();
    float readFloat();
    std::string readString();
    size_t readCompressed(void* data, size_t maxSize);
};
```

### 2. WorldManager Extensions

#### Chunk Modification Tracking
```cpp
// Add to WorldManager class
struct WorldCoordXZHash {
    std::size_t operator()(const WorldCoordXZ& coord) const {
        return std::hash<int_fast64_t>()(coord.x) ^ 
               (std::hash<int_fast64_t>()(coord.z) << 1);
    }
};

// Track modified chunks
std::unordered_set<WorldCoordXZ, WorldCoordXZHash> modifiedChunks;
std::unordered_map<WorldCoordXZ, std::chrono::system_clock::time_point, WorldCoordXZHash> chunkModificationTimes;

// Add to setVoxel method
void setVoxel(...) {
    // Existing code...
    
    // Track modification
    WorldCoordXZ coord = {worldToColumnBaseX(worldX), worldToColumnBaseZ(worldZ)};
    modifiedChunks.insert(coord);
    chunkModificationTimes[coord] = std::chrono::system_clock::now();
}
```

#### Chunk Serialization Implementation
```cpp
// Save a single chunk to binary
bool WorldManager::saveChunkToBinary(const ChunkColumn* column, const std::string& fileName) {
    if (!column) return false;
    
    std::ofstream file(fileName, std::ios::binary);
    if (!file.is_open()) return false;
    
    BinaryWriter writer(file);
    
    // Write header
    writer.writeUInt32(0x56435743); // "VCWC" magic number
    writer.writeUInt32(1);          // Version 1
    writer.writeInt64(column->getBaseX());
    writer.writeInt64(column->getBaseZ());
    
    // Determine which segments exist and are modified
    uint16_t segmentBitmap = 0;
    for (int i = 0; i < ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
        const ChunkSegment* segment = column->getChunkSegment(i);
        if (segment && !segment->isEmpty()) {
            segmentBitmap |= (1 << i);
        }
    }
    
    // Write segment bitmap
    writer.writeUInt16(segmentBitmap);
    
    // Write each segment that exists and is not empty
    for (int i = 0; i < ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
        if (segmentBitmap & (1 << i)) {
            const ChunkSegment* segment = column->getChunkSegment(i);
            if (segment) {
                // Get raw voxel data
                std::vector<uint8_t> rawData;
                rawData.reserve(ChunkSegment::SEGMENT_WIDTH * 
                               ChunkSegment::SEGMENT_HEIGHT * 
                               ChunkSegment::SEGMENT_DEPTH * sizeof(VoxelEngine::World::Voxel));
                
                // Copy voxel data to buffer
                for (int x = 0; x < ChunkSegment::SEGMENT_WIDTH; ++x) {
                    for (int y = 0; y < ChunkSegment::SEGMENT_HEIGHT; ++y) {
                        for (int z = 0; z < ChunkSegment::SEGMENT_DEPTH; ++z) {
                            VoxelEngine::World::Voxel voxel = segment->getVoxel(x, y, z);
                            // Pack voxel data into buffer
                            rawData.push_back(static_cast<uint8_t>(voxel.type));
                            // Add additional voxel properties if needed
                        }
                    }
                }
                
                // Compress data using LZ4
                uint8_t compressionType = 2; // LZ4
                writer.writeUInt8(compressionType);
                writer.writeCompressed(rawData.data(), rawData.size());
            }
        }
    }
    
    return true;
}

// Load a chunk from binary
bool WorldManager::loadChunkFromBinary(const std::string& fileName, ChunkColumn* column) {
    if (!column) return false;
    
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) return false;
    
    BinaryReader reader(file);
    
    // Read and verify header
    uint32_t magicNumber = reader.readUInt32();
    if (magicNumber != 0x56435743) return false; // "VCWC"
    
    uint32_t version = reader.readUInt32();
    if (version != 1) return false; // Only support version 1 for now
    
    int64_t baseX = reader.readInt64();
    int64_t baseZ = reader.readInt64();
    
    // Verify chunk coordinates match
    if (baseX != column->getBaseX() || baseZ != column->getBaseZ()) {
        return false;
    }
    
    // Read segment bitmap
    uint16_t segmentBitmap = reader.readUInt16();
    
    // Read each segment
    for (int i = 0; i < ChunkColumn::CHUNKS_PER_COLUMN; ++i) {
        if (segmentBitmap & (1 << i)) {
            // Get or create segment
            ChunkSegment* segment = column->getOrCreateChunkSegment(i);
            if (!segment) continue;
            
            // Read compression type
            uint8_t compressionType = reader.readUInt8();
            
            // Read compressed data
            std::vector<uint8_t> decompressedData;
            decompressedData.resize(ChunkSegment::SEGMENT_WIDTH * 
                                   ChunkSegment::SEGMENT_HEIGHT * 
                                   ChunkSegment::SEGMENT_DEPTH);
            
            size_t bytesRead = reader.readCompressed(decompressedData.data(), decompressedData.size());
            if (bytesRead == 0) return false;
            
            // Apply voxel data to segment
            size_t index = 0;
            for (int x = 0; x < ChunkSegment::SEGMENT_WIDTH; ++x) {
                for (int y = 0; y < ChunkSegment::SEGMENT_HEIGHT; ++y) {
                    for (int z = 0; z < ChunkSegment::SEGMENT_DEPTH; ++z) {
                        if (index < decompressedData.size()) {
                            VoxelEngine::World::VoxelType type = 
                                static_cast<VoxelEngine::World::VoxelType>(decompressedData[index++]);
                            VoxelEngine::World::Voxel voxel(type);
                            segment->setVoxel(x, y, z, voxel);
                            // Read additional voxel properties if needed
                        }
                    }
                }
            }
            
            // Mark segment as generated but not dirty
            segment->setGenerated(true);
            segment->markDirty(false);
        }
    }
    
    return true;
}

// Get list of all modified chunks
std::vector<WorldCoordXZ> WorldManager::getModifiedChunks() const {
    return std::vector<WorldCoordXZ>(modifiedChunks.begin(), modifiedChunks.end());
}

// Reset modified chunks tracking after save
void WorldManager::clearModifiedChunks() {
    modifiedChunks.clear();
    chunkModificationTimes.clear();
}
```

### 3. SaveManager Class

```cpp
// Save information structure
struct SaveInfo {
    std::string name;
    std::string displayName;
    std::string timestamp;
    uint64_t playTimeSeconds;
    std::string screenshotPath;
    std::string lastPlayedDate;
    bool hasQuickSave;
    bool hasAutoSave;
};

class SaveManager {
public:
    SaveManager(Game* game);
    ~SaveManager();
    
    // Initialize manager with a save directory
    bool initialize(const std::string& baseSaveDir);
    
    // Create a new save with given name
    bool createSave(const std::string& saveName, const std::string& displayName = "");
    
    // Save current game state (main interface)
    bool saveGame(const std::string& saveName, bool isQuickSave = false);
    
    // Load game from save
    bool loadGame(const std::string& saveName);
    
    // Quick save to special slot
    bool quickSave();
    
    // Quick load from special slot
    bool quickLoad();
    
    // Auto-save at regular intervals
    void startAutoSave(int intervalMinutes);
    void stopAutoSave();
    
    // Force an immediate auto-save
    bool performAutoSave();
    
    // Take screenshot for save preview
    bool captureScreenshot(const std::string& savePath);
    
    // Get list of available saves
    std::vector<SaveInfo> listSaves();
    
    // Get detailed info about a specific save
    SaveInfo getSaveInfo(const std::string& saveName);
    
    // Delete a save
    bool deleteSave(const std::string& saveName);
    
    // Check if save/load operation is in progress
    bool isOperationInProgress() const { return isSaving_ || isLoading_; }
    
    // Get current save name
    std::string getCurrentSaveName() const { return currentSaveName_; }
    
private:
    // Save metadata (game version, timestamp, player position, etc)
    bool saveMetadata(const std::string& savePath);
    
    // Save player data (inventory, stats, etc)
    bool savePlayerData(const std::string& savePath);
    
    // Save chunks (optimized for background thread)
    bool saveChunks(const std::string& savePath, bool incrementalOnly = false);
    
    // Update chunk manifest
    bool updateChunkManifest(const std::string& savePath, 
                            const std::vector<WorldCoordXZ>& savedChunks);
    
    // Create backup of existing save before modifying
    bool createBackup(const std::string& saveName);
    
    // Auto-save thread function
    void autoSaveThreadFunction();
    
    // Handle save errors and recovery
    void handleSaveError(const std::string& saveName, const std::string& errorMessage);
    
    // Log save/load operations
    void logOperation(const std::string& operation, const std::string& saveName, bool success);
    
    // Chunk saving helper functions
    bool saveModifiedChunks(const std::string& chunksDir);
    bool ensureSaveDirectories(const std::string& savePath);
    
    // Internal state
    Game* game_;                   // Reference to main game object
    WorldManager* worldManager_;   // Reference to world manager
    std::string baseSaveDirectory_;
    std::string currentSaveName_;
    bool isSaving_;
    bool isLoading_;
    
    // Auto-save
    bool autoSaveEnabled_;
    int autoSaveInterval_;         // Minutes between auto-saves
    int autoSaveRotateCount_;      // Number of auto-save backups to keep
    std::thread autoSaveThread_;
    std::atomic<bool> stopAutoSaveThread_;
    std::mutex saveMutex_;
    
    // Save statistics
    struct SaveStats {
        size_t totalChunks;
        size_t modifiedChunks;
        size_t totalSaveSize;
        size_t compressedSize;
        double saveTimeMs;
    } lastSaveStats_;
};

// Implementation of key methods

bool SaveManager::saveGame(const std::string& saveName, bool isQuickSave) {
    // Acquire mutex for thread safety
    std::lock_guard<std::mutex> lock(saveMutex_);
    
    if (isSaving_ || isLoading_) {
        return false; // Already in progress
    }
    
    // Set save state
    isSaving_ = true;
    
    // Notify game of save operation starting
    game_->changeState(GameState::SAVING);
    
    // Start time for statistics
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create temporary save path
    std::string savePath = baseSaveDirectory_ + "/" + saveName;
    std::string tempPath = savePath + "_temp";
    
    // Create backup of existing save
    if (std::filesystem::exists(savePath)) {
        createBackup(saveName);
    }
    
    // Ensure temp directory exists
    ensureSaveDirectories(tempPath);
    
    // Take screenshot for save preview
    captureScreenshot(tempPath);
    
    // Save metadata
    if (!saveMetadata(tempPath)) {
        handleSaveError(saveName, "Failed to save metadata");
        return false;
    }
    
    // Save player data
    if (!savePlayerData(tempPath)) {
        handleSaveError(saveName, "Failed to save player data");
        return false;
    }
    
    // Save chunks
    SaveStats stats;
    stats.totalChunks = worldManager_->getChunkCount();
    stats.modifiedChunks = worldManager_->modifiedChunks.size();
    
    if (!saveChunks(tempPath)) {
        handleSaveError(saveName, "Failed to save chunks");
        return false;
    }
    
    // Calculate save time
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.saveTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    lastSaveStats_ = stats;
    
    // Log statistics
    std::cout << "[SaveManager] Saved " << stats.modifiedChunks 
              << " modified chunks out of " << stats.totalChunks
              << " in " << stats.saveTimeMs << "ms" << std::endl;
    
    // Atomic directory rename for data integrity
    try {
        if (std::filesystem::exists(savePath)) {
            std::filesystem::remove_all(savePath);
        }
        std::filesystem::rename(tempPath, savePath);
    } catch (const std::filesystem::filesystem_error& e) {
        handleSaveError(saveName, "Failed to finalize save: " + std::string(e.what()));
        return false;
    }
    
    // Update current save name
    currentSaveName_ = saveName;
    
    // Clear modified chunks tracking
    worldManager_->clearModifiedChunks();
    
    // Notify game of save operation completion
    game_->changeState(GameState::PLAYING);
    isSaving_ = false;
    
    return true;
}

void SaveManager::autoSaveThreadFunction() {
    using namespace std::chrono_literals;
    
    while (!stopAutoSaveThread_) {
        // Sleep for the interval
        for (int i = 0; i < autoSaveInterval_ * 60 && !stopAutoSaveThread_; ++i) {
            std::this_thread::sleep_for(1s);
        }
        
        // Skip if stopping or no modified chunks
        if (stopAutoSaveThread_ || worldManager_->modifiedChunks.empty()) {
            continue;
        }
        
        // Generate auto-save name
        std::string autoSaveName = currentSaveName_.empty() ? 
                                  "autosave" : 
                                  currentSaveName_ + "_auto";
        
        // Attempt auto-save
        std::cout << "[SaveManager] Performing auto-save..." << std::endl;
        bool success = saveGame(autoSaveName, false);
        
        // Clean up old auto-saves if needed
        if (success && autoSaveRotateCount_ > 0) {
            // Find and remove old auto-saves
            auto saves = listSaves();
            // Implementation for rotating auto-saves
        }
    }
}

bool SaveManager::loadGame(const std::string& saveName) {
    // Acquire mutex for thread safety
    std::lock_guard<std::mutex> lock(saveMutex_);
    
    if (isSaving_ || isLoading_) {
        return false; // Already in progress
    }
    
    // Set loading state
    isLoading_ = true;
    
    // Notify game of load operation starting
    game_->changeState(GameState::LOADING);
    
    std::string savePath = baseSaveDirectory_ + "/" + saveName;
    
    // Check if save exists
    if (!std::filesystem::exists(savePath)) {
        handleSaveError(saveName, "Save does not exist");
        return false;
    }
    
    // Read metadata first to verify version compatibility
    GameMetadata metadata;
    if (!JsonUtils::readMetadata(savePath + "/metadata.json", metadata)) {
        handleSaveError(saveName, "Failed to read metadata");
        return false;
    }
    
    // Reset world
    worldManager_->reset();
    
    // Load player data
    if (!loadPlayerData(savePath)) {
        handleSaveError(saveName, "Failed to load player data");
        return false;
    }
    
    // Read chunk manifest
    std::vector<ChunkInfo> chunkList;
    if (!JsonUtils::readChunkManifest(savePath + "/chunks/manifest.json", chunkList)) {
        handleSaveError(saveName, "Failed to read chunk manifest");
        return false;
    }
    
    // Load chunks near player first
    glm::vec3 playerPos = game_->getPlayerPosition();
    
    // Sort chunks by distance to player
    std::sort(chunkList.begin(), chunkList.end(),
        [&playerPos](const ChunkInfo& a, const ChunkInfo& b) {
            float distA = glm::distance(
                glm::vec3(a.x * ChunkSegment::SEGMENT_WIDTH, 0, a.z * ChunkSegment::SEGMENT_DEPTH),
                playerPos);
            float distB = glm::distance(
                glm::vec3(b.x * ChunkSegment::SEGMENT_WIDTH, 0, b.z * ChunkSegment::SEGMENT_DEPTH),
                playerPos);
            return distA < distB;
        });
    
    // Load chunks (nearest first)
    int loadedChunks = 0;
    for (const auto& chunk : chunkList) {
        // Construct chunk filename
        std::string chunkFile = savePath + "/chunks/chunk_" + 
                               std::to_string(chunk.x) + "_" + 
                               std::to_string(chunk.z) + ".bin";
        
        // Create ChunkColumn
        ChunkColumn* column = worldManager_->getOrCreateChunkColumn(
            chunk.x * ChunkSegment::SEGMENT_WIDTH, 
            chunk.z * ChunkSegment::SEGMENT_DEPTH);
        
        if (column) {
            if (loadChunkFromBinary(chunkFile, column)) {
                loadedChunks++;
            }
        }
        
        // Update progress every 10 chunks
        if (loadedChunks % 10 == 0) {
            float progress = static_cast<float>(loadedChunks) / chunkList.size();
            // Update load progress indicator
        }
        
        // Break after loading essential chunks to show something quickly
        if (loadedChunks >= 25) {
            // Continue loading others in background
            break;
        }
    }
    
    // Set current save name
    currentSaveName_ = saveName;
    
    // Notify game of load completion
    game_->changeState(GameState::PLAYING);
    isLoading_ = false;
    
    // Continue loading remaining chunks in background
    // This would be implemented with a separate thread
    
    return true;
}
```
```

### 4. JSON Utilities

```cpp
namespace JsonUtils {
    // Write metadata to JSON file
    bool writeMetadata(const std::string& filePath, 
                      const GameMetadata& metadata);
    
    // Read metadata from JSON file
    bool readMetadata(const std::string& filePath, 
                     GameMetadata& metadata);
    
    // Write chunk manifest
    bool writeChunkManifest(const std::string& filePath,
                           const std::vector<ChunkInfo>& chunks);
    
    // Read chunk manifest
    bool readChunkManifest(const std::string& filePath,
                          std::vector<ChunkInfo>& chunks);
}
```

## Implementation Phases

### Phase 1: Core Serialization Framework
1. Create BinaryReader/Writer classes
2. Create Serializable interface
3. Implement JSON utility functions
4. Add modification tracking to WorldManager

### Phase 2: Basic Save/Load Functionality
1. Create SaveManager class
2. Implement single-chunk serialization
3. Add save/load functions to Game class
4. Create basic save/load menu UI
5. Test manual saving and loading

### Phase 3: Incremental and Auto-Save
1. Implement chunk manifest tracking
2. Add auto-save background thread
3. Create auto-save configuration options
4. Implement quick save/load hotkeys

### Phase 4: Reliability and Polish
1. Add atomic save operations
2. Implement save backup system
3. Add save thumbnails
4. Create save browser UI
5. Add error recovery mechanisms

## Integration with Game State Management

The save system will integrate with the Game State Management in these ways:

1. **State Transitions**:
   - Add SAVING and LOADING states
   - Ensure proper transitions between states

2. **Menu Integration**:
   - Add Save/Load options to pause menu
   - Create dedicated save management UI

3. **Auto-Save Triggers**:
   - Auto-save when entering menu
   - Auto-save at regular intervals during gameplay
   - Auto-save before potentially dangerous operations

4. **Game Start Flow**:
   - Add option to load last save on startup
   - Show save selection screen in main menu

## Technical Considerations

### Compression Strategy
- Use LZ4 for faster compression/decompression
- Only compress segments with significant data
- Test compression ratio vs. performance

### Threading Model
- Save operations in background thread
- Use thread pool for multiple chunk operations
- Ensure thread safety for concurrent access

### Error Handling
- Backup old save before writing new one
- Validate save data integrity with checksums
- Implement progressive fallback for corrupted saves

## Success Criteria

- Chunk modifications persist between game sessions
- Auto-save operates without noticeable performance impact
- Saved game loads with all voxel modifications intact
- Crash recovery restores recent changes
- Save files are reasonably sized and load quickly

## Future Extensions

1. **Cloud Saves**: Integration with platform cloud save systems
2. **Save Migration**: Tools for upgrading old save formats
3. **Save Sharing**: Export/import functionality for sharing worlds
4. **Save Versioning**: Multiple versions of the same save
5. **World Regions**: Support for partial world loading/saving

## Dependencies

- JSON library (e.g., nlohmann/json)
- Compression library (LZ4 or zlib)
- Screenshot capture functionality
- File I/O utilities
