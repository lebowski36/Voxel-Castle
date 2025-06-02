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
std::unordered_set<WorldCoordXZ, WorldCoordXZHash> modifiedChunks;

// Add to setVoxel method
void setVoxel(...) {
    // Existing code...
    
    // Track modification
    WorldCoordXZ coord = {worldToColumnBaseX(worldX), worldToColumnBaseZ(worldZ)};
    modifiedChunks.insert(coord);
}
```

#### Serialization Methods
```cpp
// Save a single chunk to binary
bool saveChunkToBinary(const ChunkColumn* column, std::string fileName);

// Load a chunk from binary
bool loadChunkFromBinary(std::string fileName, ChunkColumn* column);

// Get list of all modified chunks
std::vector<WorldCoordXZ> getModifiedChunks() const;

// Reset modified chunks tracking after save
void clearModifiedChunks();
```

### 3. SaveManager Class

```cpp
class SaveManager {
public:
    SaveManager();
    ~SaveManager();
    
    // Initialize manager with a save directory
    bool initialize(const std::string& baseSaveDir);
    
    // Create a new save with given name
    bool createSave(const std::string& saveName);
    
    // Save current game state
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
    
    // Get list of available saves
    std::vector<SaveInfo> listSaves();
    
    // Delete a save
    bool deleteSave(const std::string& saveName);
    
private:
    // Save metadata
    bool saveMetadata(const std::string& savePath);
    
    // Save player data
    bool savePlayerData(const std::string& savePath);
    
    // Save chunks (can be run in background thread)
    bool saveChunks(const std::string& savePath);
    
    // Thread management
    void saveThreadFunction();
    
    // Internal state
    std::string baseSaveDirectory_;
    std::string currentSaveName_;
    bool isSaving_;
    bool isLoading_;
    
    // Auto-save
    bool autoSaveEnabled_;
    int autoSaveInterval_;
    std::thread autoSaveThread_;
    std::atomic<bool> stopAutoSaveThread_;
};
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
