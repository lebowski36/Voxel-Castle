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

## Current Critical Issues (Requiring Investigation)

### 🔧 ACTIVE DEBUGGING TASKS

#### Issue 1: Camera Orientation Update Delay
**Status**: 🐛 **BUG IDENTIFIED**

**Problem Description**: 
- Camera yaw and pitch are saved and loaded correctly from save files
- However, the camera orientation is not immediately applied upon loading
- The camera appears to remain in its pre-load orientation until the player moves the mouse
- When mouse movement occurs, the camera "snaps" to the loaded orientation

**Exact Reproduction Steps**:
1. F5 to save current position and camera orientation
2. Turn camera right (change yaw/pitch)
3. F9 to load the saved game
4. Map reloads and camera position is correct, but camera direction remains as it was before F9
5. Move mouse - camera suddenly "flips" to the loaded orientation

**Current Investigation Findings**:
- `camera_->setYaw()` and `camera_->setPitch()` are being called in Game::loadGame() and Game::quickLoad()
- Camera position restoration works correctly (immediate effect)
- Camera orientation restoration has delayed effect (only after mouse input)

**Hypothesis**: 
- The SpectatorCamera class may need additional method calls to force immediate orientation update
- Possible missing view matrix recalculation or similar internal state update

**Next Steps**: 
- [ ] Investigate SpectatorCamera implementation for orientation update methods
- [ ] Check if there's a `updateViewMatrix()` or similar method that needs to be called
- [ ] Test calling additional camera update methods after setYaw/setPitch

---

#### Issue 2: Block Modifications Lost During Save/Load
**Status**: 🐛 **CRITICAL BUG**

**Problem Description**:
- Block placement/destruction works correctly during gameplay
- Saving appears to complete without errors
- Loading removes all placed blocks and restores original terrain
- Post-load behavior shows slow chunk loading and some chunks may not load at all

**Exact Reproduction Steps**:
1. Place or destroy blocks in the world
2. F5 to save the game
3. F9 to load the saved game
4. All block modifications are gone, world appears as originally generated
5. Chunks load slowly and some areas remain unloaded

**Current Investigation Findings**:
- Save system calls saveGame() which should capture modified chunks
- SaveManager is supposed to save chunk modifications via WorldManager
- Load system restores player position/orientation but loses block changes

**Suspected Root Causes**:
1. **Chunk Modification Tracking**: WorldManager may not be properly tracking which chunks have been modified
2. **Serialization Pipeline**: Modified chunks may not be getting serialized during save
3. **Deserialization Pipeline**: Chunk data may not be getting properly restored during load
4. **Manifest System**: The chunk manifest may not be correctly listing modified chunks
5. **Timing Issues**: Save/load may be happening before chunk modifications are committed

**Investigation Plan**:
- [ ] **Step 1: Verify Chunk Modification Tracking**
  - Add debug logging to block placement/destruction to confirm chunks are marked as modified
  - Check if WorldManager.m_modifiedChunks set contains expected chunk coordinates
  - Verify that block changes trigger modification flags
  
- [ ] **Step 2: Examine Save Pipeline**
  - Add debug logging to SaveManager.saveGame() to see what chunks are being saved
  - Verify that modified chunks are being serialized to disk
  - Check save file contents to confirm chunk data is present
  
- [ ] **Step 3: Examine Load Pipeline** 
  - Add debug logging to SaveManager.loadGame() to see what chunks are being loaded
  - Verify that chunk data is being deserialized correctly
  - Check if loaded chunks are replacing generated chunks properly
  
- [ ] **Step 4: Check Hybrid Chunk Management**
  - Verify that unloaded-but-modified chunks are being cached properly
  - Ensure cache is being checked during load operations
  - Confirm that all modified chunks (loaded + cached) are included in saves

**Files to Investigate**:
- `game/src/world/WorldManager.cpp` - Chunk modification tracking
- `game/src/core/SaveManager.cpp` - Save/load pipeline  
- `game/src/world/chunk/ChunkColumn.cpp` - Chunk serialization
- `game/src/core/game.cpp` - Save/load integration

**Debug Logging Locations Needed**:
- Block placement/destruction operations
- Chunk modification flag setting
- Save operation chunk enumeration
- Load operation chunk restoration
- Chunk cache operations

**Current Investigation Findings**:
(Space for recording findings during investigation)

---

### 🔄 IN PROGRESS
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

## Critical Issues Under Investigation

### Issue 1: Camera Orientation Load Delay ⚠️ CRITICAL
**Problem Description:**
Camera orientation (yaw/pitch) is loaded from save data but not applied immediately. The orientation only updates after mouse movement, causing a jarring transition.

**Reproduction Steps:**
1. F5 to save current camera position and orientation
2. Turn camera to look in a different direction  
3. F9 to load the saved state
4. Map reloads and camera position is correct, but orientation remains at the pre-load direction
5. Moving the mouse causes the camera to "snap" to the saved orientation

**Technical Analysis:**
- Camera position restoration works correctly
- Camera orientation values are being loaded and passed to `camera_->setYaw()` and `camera_->setPitch()`
- Issue appears to be that the camera orientation setters don't immediately update the camera's view matrix
- Camera likely needs `updateCameraVectors()` call after setting orientation

**Investigation Findings:**
- SpectatorCamera has `setYaw()` and `setPitch()` methods
- These methods don't call `updateCameraVectors()` internally
- Camera orientation changes are only applied when `processMouseMovement()` is called
- Need to force immediate camera vector update after loading orientation

**Solution Approach:**
- Add explicit `updateCameraVectors()` call after setting camera orientation in load operations
- Verify camera view matrix is updated immediately upon loading

### Issue 2: Block Modification Persistence Failure ⚠️ CRITICAL  
**Problem Description:**
Block modifications are not properly saved and restored. After save/load, all placed blocks disappear, and chunk loading behavior becomes erratic.

**Reproduction Steps:**
1. Place blocks in multiple locations across different chunks
2. F5 to save the game
3. F9 to load the saved state
4. All placed blocks have disappeared
5. Chunks load very slowly and some chunks don't load at all

**Technical Analysis:**
- Block placement system works correctly during gameplay
- Block modifications are tracked in WorldManager's `m_modifiedChunks` set
- Chunk serialization and deserialization exists but may not be working correctly
- Multiple potential failure points: tracking, serialization, deserialization, or chunk loading

**Investigation Findings:**
- WorldManager tracks modified chunks in `m_modifiedChunks` set
- Chunk serialization uses binary format with VCWC header
- SaveManager calls WorldManager to save/load chunk data
- Need to verify: chunk modification tracking, serialization pipeline, and deserialization process
- Slow chunk loading suggests potential issues with chunk recreation or data corruption

**Investigation Plan:**
1. **Verify Chunk Modification Tracking:**
   - Check if modified chunks are properly added to `m_modifiedChunks` set during block placement
   - Verify chunk coordinates are correctly tracked

2. **Test Serialization Pipeline:**
   - Add debug logging to chunk save operations
   - Verify chunk data is being written to files
   - Check file sizes and content validity

3. **Test Deserialization Pipeline:**
   - Add debug logging to chunk load operations  
   - Verify chunk files are being read correctly
   - Check if deserialized chunks are properly integrated into WorldManager

4. **Analyze Chunk Loading Behavior:**
   - Investigate why chunks load slowly after loading a save
   - Determine why some chunks don't load at all
   - Check if there's interference between saved chunks and procedural generation

**Next Steps:**
1. Add comprehensive debug logging to the chunk modification and save/load pipeline
2. Test individual components in isolation
3. Verify chunk file format and data integrity
4. Check integration between SaveManager and WorldManager

## Implementation Status
