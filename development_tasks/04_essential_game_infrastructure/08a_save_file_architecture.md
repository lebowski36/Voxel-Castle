# Save File Architecture Implementation Plan

## Overview
Implementation plan for the save file architecture in Voxel Castle. The save system provides persistence for voxel modifications, player state, and game settings, with support for auto-saving, incremental saving, and crash recovery.

## Design Goals

1. **Efficiency**: Minimize storage requirements and performance impact
2. **Reliability**: Ensure data integrity even during crashes or power failures
3. **Extensibility**: Support future game features through versioned saves
4. **Usability**: Provide intuitive save/load functionality for users

## Current Implementation Status

### ✅ COMPLETED
- **F5/F9 Key Bindings**: Quick save/load hotkeys implemented
- **Camera Position & Mode Saving/Loading**: Real player position and camera mode persistence
- **Basic File Structure**: Directory structure and metadata files
- **Chunk Modification Tracking**: WorldManager tracks modified chunks
- **Chunk Serialization**: Basic binary chunk save/load with compression support
- **Save Directory Protection**: saves/ folder in .gitignore

### 🔄 IN PROGRESS
- **Hybrid Chunk Management System**: Implementation of temporary caching for unloaded chunks

### 📋 PENDING
- **Menu System Integration**: Wire up save/load to game menu UI
- **Auto-save System**: Scheduled auto-saving functionality
- **Save Thumbnails**: Screenshot capture for save preview

## Implementation Status

✅ **COMPLETED:**
- F5/F9 Key Bindings for quick save/load
- Camera Position & Mode Saving/Loading
- Basic file structure and metadata handling
- Console output cleanup
- Save directory protection (.gitignore)
- Chunk modification tracking in WorldManager
- Basic chunk serialization (binary format with VCWC header)
- SaveManager class with core save/load functionality

🔄 **IN PROGRESS:**
- Hybrid chunk management system (detailed below)
- Menu system integration

⏳ **PENDING:**
- Auto-save system
- Save thumbnails/screenshots
- Compression optimization
- Error recovery mechanisms

## Hybrid Chunk Management System

### Core Concept
The hybrid system ensures that all player modifications are preserved, even for chunks that get unloaded when the player moves away from them.

### System Components

#### 1. Chunk Tracking System
- When a chunk is modified, it's marked as "dirty" in memory (✅ implemented)
- When a chunk is about to be unloaded (player moves away):
  - The chunk's data is serialized to a temporary cache (RAM or disk)
  - The chunk coordinates remain in the "modified chunks" list
  - The chunk can be safely unloaded from active memory

#### 2. Chunk Retrieval
- If the player returns to the area during the same gameplay session:
  - Engine checks if the chunk exists in the temporary cache
  - If found, loaded from cache instead of regenerated
  - This preserves player modifications and is faster than regeneration

#### 3. Save System Integration
- When player performs explicit save (F5/quick save):
  - All currently loaded modified chunks are saved
  - Plus all previously unloaded-but-modified chunks from temporary cache
  - After saving, temporary cache can be cleared (memory optimization)

#### 4. Cache Management
- When a save completes, the "modified chunks" list is cleared
- Temporary cache is only cleared after saving, not when chunks unload
- This prevents data loss and ensures all modifications are captured

### Benefits
- Fast re-loading of chunks during same gameplay session
- Ability to save all modified chunks, even those not currently loaded
- No data loss when chunks are unloaded due to distance
- Memory efficiency with optional cache clearing
- Seamless experience for players exploring large areas

## Implementation Phases

### Phase 1: Core Serialization Framework ✅ COMPLETED
1. ✅ Create BinaryReader/Writer classes
2. ✅ Create Serializable interface
3. ✅ Implement JSON utility functions (deferred to Phase 2)
4. ✅ Add modification tracking to WorldManager

### Phase 2: Basic Save/Load Functionality ✅ IN PROGRESS
1. ✅ Implement JSON utility functions
2. ✅ Create SaveManager class
3. ✅ Implement single-chunk serialization
4. ✅ Add save/load functions to Game class
5. ✅ Implement F5/F9 quick save/load hotkeys
6. ⏳ Create basic save/load menu UI
7. ✅ Test manual saving and loading

### Phase 3: Incremental and Auto-Save
1. ⏳ Implement chunk manifest tracking
2. ⏳ Add auto-save background thread
3. ⏳ Create auto-save configuration options
4. ✅ Implement quick save/load hotkeys

### Phase 4: Reliability and Polish
1. ⏳ Add atomic save operations
2. ⏳ Implement save backup system
3. ⏳ Add save thumbnails
4. ⏳ Create save browser UI
5. ⏳ Add error recovery mechanisms

## Hybrid Chunk Management System

The save system will use a hybrid approach to manage chunk modifications across play sessions and when chunks are unloaded from memory. This system ensures no player modifications are lost while maintaining memory efficiency.

### 1. Chunk Tracking System

- **In-Memory Tracking**: 
  - When a chunk is modified, it's marked as "dirty" in memory (already implemented)
  - The chunk's coordinates are added to the `m_modifiedChunks` set in WorldManager
  - A timestamp is recorded in `m_chunkModificationTimes` map

- **Unloading Behavior**: 
  - When a chunk is about to be unloaded (player moves away), two actions occur:
    1. The chunk's data is serialized to a temporary cache either in RAM or on disk
    2. The chunk coordinates remain in the "modified chunks" list

### 2. Temporary Cache Implementation

```cpp
// Add to WorldManager class
struct CachedChunkData {
    std::vector<uint8_t> serializedData;
    std::chrono::system_clock::time_point modificationTime;
    bool isDirty;
};

// Cache for unloaded-but-modified chunks
std::unordered_map<WorldCoordXZ, CachedChunkData, WorldCoordXZHash> m_unloadedModifiedChunks;

// Method to serialize a chunk to the temporary cache
void cacheChunkBeforeUnloading(const WorldCoordXZ& coord, ChunkColumn* column) {
    // Skip if this chunk hasn't been modified
    if (m_modifiedChunks.find(coord) == m_modifiedChunks.end()) {
        return;
    }
    
    // Serialize the chunk to the cache
    CachedChunkData cacheData;
    cacheData.modificationTime = m_chunkModificationTimes[coord];
    cacheData.isDirty = true;
    
    // Serialize chunk data to memory buffer
    std::vector<uint8_t> buffer;
    // ... [Serialization code]
    
    cacheData.serializedData = std::move(buffer);
    m_unloadedModifiedChunks[coord] = std::move(cacheData);
}
```

### 3. Chunk Retrieval Process

- **When player returns to a previously visited area**:
  - The engine first checks if the chunk exists in the temporary cache
  - If found in cache, it's deserialized and loaded from the cache instead of being regenerated
  - This preserves player modifications and is faster than regenerating terrain

```cpp
// Method to check cache before generating a new chunk
ChunkColumn* getOrCreateChunkColumnWithCache(int64_t worldX, int64_t worldZ) {
    WorldCoordXZ coord = {worldToColumnBaseX(worldX), worldToColumnBaseZ(worldZ)};
    
    // Check if in active chunks first
    ChunkColumn* column = getChunkColumn(worldX, worldZ);
    if (column) {
        return column;
    }
    
    // Check if in cache
    auto cacheIt = m_unloadedModifiedChunks.find(coord);
    if (cacheIt != m_unloadedModifiedChunks.end()) {
        // Create new column and deserialize from cache
        column = getOrCreateChunkColumn(worldX, worldZ);
        
        // Deserialize from cached data
        // ... [Deserialization code]
        
        // Remove from cache to free memory
        m_unloadedModifiedChunks.erase(cacheIt);
        
        return column;
    }
    
    // Not in cache, create normally
    return getOrCreateChunkColumn(worldX, worldZ);
}
```

### 4. Save System Integration

- **When the player performs a save**:
  - All currently loaded modified chunks are saved directly
  - All unloaded-but-modified chunks from the temporary cache are also saved
  - Both sets of chunks are included in the chunk manifest

```cpp
// Method to get all modified chunks (both loaded and cached)
std::vector<WorldCoordXZ> getAllModifiedChunks() const {
    // Start with in-memory modified chunks
    std::vector<WorldCoordXZ> allModified(m_modifiedChunks.begin(), m_modifiedChunks.end());
    
    // Add cached chunks
    for (const auto& pair : m_unloadedModifiedChunks) {
        // Only add if not already in the list
        if (m_modifiedChunks.find(pair.first) == m_modifiedChunks.end()) {
            allModified.push_back(pair.first);
        }
    }
    
    return allModified;
}
```

### 5. Cache Management

- **After a successful save**:
  - The "modified chunks" list is cleared
  - The temporary cache can optionally be cleared to free memory
  - Alternatively, frequently accessed chunks can be kept in cache for faster loading

### 6. Benefits of This Approach

1. **Data Persistence**: No modifications are lost when chunks are unloaded
2. **Performance**: Fast re-loading of chunks during the same gameplay session
3. **Completeness**: All modified chunks are saved, even those not currently loaded
4. **Memory Efficiency**: Optional cache clearing to manage memory usage
5. **Seamless Experience**: Players don't experience loss of modifications when returning to areas

This hybrid approach balances the need for data persistence with memory efficiency and performance, ensuring that player modifications are reliably preserved throughout the game session and across saves.
