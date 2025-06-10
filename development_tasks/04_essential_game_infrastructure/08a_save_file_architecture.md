# World Persistence & Management System ✅ COMPLETED
*Created: 2024-12-10 14:30*
*Last Updated: 2025-06-10 21:30*

## Overview
Implementation plan for Minecraft-style world persistence in Voxel Castle. The system provides automatic chunk persistence, world creation/loading, and player state management without manual save operations.

**STATUS: ✅ COMPLETED - World persistence system fully operational**
- ✅ World directory structure and metadata system implemented
- ✅ JSON-based level.dat format working
- ✅ WorldPersistenceManager class operational
- ✅ Integration with world generation UI complete
- ✅ World creation, saving, and basic loading functionality working
- ✅ World storage location: `./build/worlds/{worldName}/`

**Implemented Components:**
- `engine/src/world/world_persistence_manager.cpp` - Core persistence logic
- `engine/src/world/world_metadata.cpp` - JSON metadata handling
- `engine/src/ui/WorldSimulationUI.cpp` - UI integration for world saving
- World validation, duplicate detection, and error handling

## Design Philosophy - Minecraft-Style Persistence

### Core Principles
1. **No Manual Saves**: Block changes persist immediately, eliminating need for F5/F9
2. **Continuous Persistence**: Every modification is automatically written to disk
3. **World-Centric**: Games are "worlds" that exist continuously, not "save files"
4. **Atomic Chunks**: Each chunk is independently persistent and crash-safe
5. **Immediate Durability**: Changes survive crashes/power failures automatically

### Architecture Overview
```
/worlds/
  world_name_1/
    level.dat           # World metadata, spawn point, settings
    region/             # Chunk data in region files
      r.0.0.mca        # 32x32 chunk regions (Minecraft format)
      r.0.1.mca
      r.1.0.mca
    playerdata/         # Individual player states
      player_uuid.dat
    data/               # Additional world data
      villages.dat
      structures.dat
```

## Implementation Status

### 🚧 MAJOR ARCHITECTURE REDESIGN (December 2024)
**REMOVING LEGACY SAVE/LOAD SYSTEM - IMPLEMENTING MINECRAFT-STYLE PERSISTENCE**

### 📋 CURRENT TASKS

#### Task 1: World Creation & Management Menu System
**Priority**: HIGH - Foundation for all world persistence
**Status**: 🔄 IN PROGRESS

**CURRENT ISSUES IDENTIFIED:**
- [ ] Game starts directly into world loading, bypassing menu entirely
- [ ] Menu buttons for "Load World" and "Create New World" only show placeholder messages
- [ ] No actual world management system implemented
- [ ] Need complete startup flow redesign
- [x] ✅ **RESOLVED: ~~CRITICAL: Chunk Loading Flickering Bug~~** - Fixed issue where chunks would cycle through different terrain data after resuming from the main menu. Root cause was shared OpenGL buffers (VAO/VBO/EBO) in MeshRenderer. Each VoxelMesh now has its own OpenGL buffers, eliminating the shared buffer bug.
- [x] ✅ **RESOLVED: ~~CRITICAL: ESC Menu State Bug~~** - ESC key shows "Cannot pop state: stack is empty" errors repeatedly, indicating GameStateManager state stack corruption
- [x] ✅ **RESOLVED: ~~Texture Path Duplication~~** - Fixed path duplication in texture loading
- [x] ✅ **RESOLVED: ~~CRITICAL: MeshRenderer Initialization Failure~~** - Fixed renderer initialization and proper shader setup

**Subtasks**:
- [x] **F11 Fullscreen Toggle**: Add fullscreen hotkey support ✅ COMPLETED
- [x] **UI Positioning System**: Ensure UI elements are properly positioned on screen resize, fullscreen toggle, and game state transitions ✅ COMPLETED
  - [x] Implemented proper HUD positioning at bottom center of screen
  - [x] Ensured UI element positions update correctly during state transitions
- [ ] **CRITICAL: Fix Game Startup Flow**:
  - [x] Add MAIN_MENU state to GameState enum
  - [ ] Create proper state transition from MAIN_MENU → world loading
  - [ ] Replace Legacy World Generation with seed-based world selection system
- [ ] **World Selection Dialog Implementation**:
  - [ ] Create WorldSelectionDialog class with actual world scanning
  - [ ] Implement world list display and selection
  - [ ] Add world filtering and sorting options
  - [ ] Create world deletion functionality with confirmation dialog
  - [ ] Add world details panel (last played, size, game mode)
- [ ] **Create New World Dialog Implementation**:
  - [ ] Create CreateWorldDialog class with form inputs
  - [ ] Implement world name text input field
  - [ ] Add seed input field with random seed generation option
  - [ ] Add world type selection dropdown
  - [ ] Add game mode selection dropdown
  - [ ] Implement structure generation checkbox
  - [ ] Add create and cancel buttons
- [ ] **World Loading Screen**:
  - [ ] Create LoadingScreen class
  - [ ] Implement progress bar for world generation/loading
  - [ ] Add status message display
  - [ ] Add estimated time remaining calculation
  - [ ] Implement cancel button to return to world selection

#### Task 2: Direct Chunk Persistence System
**Priority**: HIGH - Core persistence mechanism
**Status**: 🔲 NOT STARTED

**Subtasks**:
- [ ] **Remove SaveManager Class**: Eliminate centralized save system
- [ ] **ChunkColumn Auto-Save**: Add `isDirty` flag and direct file I/O
- [ ] **Background Save Thread**: Non-blocking chunk writing
- [ ] **Region File Format**: Implement Minecraft-compatible region files
- [ ] **Chunk Compression**: LZ4/Zlib compression for chunk data

#### Task 3: Player State Persistence
**Priority**: MEDIUM - Player experience
**Status**: 🔲 NOT STARTED

**Subtasks**:
- [ ] **Periodic Auto-Save**: Save player position every 10-15 seconds
- [ ] **Event-Based Saves**: Immediate save on critical events
- [ ] **Player Data Format**: Position, inventory, health, etc.
- [ ] **Multi-Player Support**: Individual player data files

#### Task 4: Legacy System Removal
**Priority**: LOW - Cleanup
**Status**: 🔲 NOT STARTED

**Subtasks**:
- [ ] **Remove F5/F9 Hotkeys**: No more manual save controls
- [ ] **Update UI Text**: Remove "save game" references
- [ ] **Documentation Update**: Reflect new persistence model

#### Task 5: Seed-Based World Generation System
**Priority**: MEDIUM - World variety and reproducibility
**Status**: 🔄 IN PROGRESS

**Subtasks**:
- [x] **Basic Seed System Implementation**: Initial setup for seed-based generation ✅ COMPLETED
  - Added getBlockSeed method to generate unique seeds for block positions
  - Basic seed-to-noise integration for terrain variety
- [ ] **Advanced Seed Systems**: 
  - Convert text/numeric seeds to reproducible deterministic generation
  - Full biome and structure seeding
- [ ] **World Type Variants**: 
  - Normal: Standard terrain with caves, ores, structures
  - Amplified: Extreme terrain generation with tall mountains
- [ ] **Terrain Generation**: Enhance noise-based terrain with seed consistency
- [ ] **Structure Generation**: Villages, dungeons, strongholds based on seed
- [ ] **Biome System**: Different biomes with seed-based placement
- [ ] **Cave Generation**: Underground cave systems with consistent seed generation
- [ ] **Ore Distribution**: Deterministic ore placement based on world seed

#### Task 6: Fix Texture System Architecture
**Priority**: HIGH - Required for UI functionality
**Status**: ✅ COMPLETED

**Subtasks**:
- [x] **TextureAtlas Methods**: Ensure TextureAtlas properly handles texture loading, storage and ID retrieval ✅ COMPLETED
- [x] **Ensure MeshRenderer Integration**: Update how MeshRenderer uses TextureAtlas texture IDs ✅ COMPLETED
- [x] **HUD & UI Integration**: Fix how UI elements like HUD obtain texture IDs from TextureAtlas ✅ COMPLETED
- [x] **Clean up Legacy Texture Code**: Remove any duplicate texture loading logic ✅ COMPLETED

## 🔧 IMMEDIATE TROUBLESHOOTING TASKS
**Priority**: BLOCKING - Must resolve before continuing save system implementation

### Investigation Steps (Execute in Order)

#### Step 1: GameStateManager Stack Corruption Investigation
**Issue**: ✅ RESOLVED: ESC key causing "Cannot pop state: stack is empty" errors when in main menu with no active game
**Investigation Tasks**:
- [x] Examine GameStateManager state stack implementation ✅ COMPLETED
- [x] Check state push/pop balance in menu transitions ✅ COMPLETED
- [x] Verify MAIN_MENU → GAMEPLAY → MAIN_MENU state flow ✅ COMPLETED
- [x] Look for missing state pushes when opening menus ✅ COMPLETED
- [x] Check if state stack is being cleared incorrectly ✅ COMPLETED
- [x] **Add condition to ESC key handler**: ESC key should not trigger state pop when in main menu with no active game session ✅ COMPLETED

#### Step 2: Chunk Loading Flickering Root Cause Analysis
**Issue**: ✅ RESOLVED: Chunks rapidly switching between different data when "Resume Game" is clicked
**Investigation Tasks**:
- [x] Compare git diff of last 4-5 commits affecting world loading ✅ COMPLETED
- [x] Check WorldManager chunk loading logic changes ✅ COMPLETED
- [x] Examine MeshRenderer changes that might affect chunk display ✅ COMPLETED
- [x] Look for race conditions in mesh generation/display ✅ COMPLETED
- [x] Check if multiple world initializations are occurring ✅ COMPLETED
- [x] Verify chunk coordinate calculations haven't changed ✅ COMPLETED

#### Step 3: MeshRenderer Initialization Failure Investigation
**Issue**: ✅ RESOLVED: "Renderer not ready due to shader, buffer, or texture generation failure"
**Investigation Tasks**:
- [x] Check MeshRenderer constructor requirements ✅ COMPLETED
- [x] Verify OpenGL context is available when MeshRenderer initializes ✅ COMPLETED
- [x] Check shader compilation/loading in MeshRenderer ✅ COMPLETED
- [x] Verify texture atlas loading happens before MeshRenderer init ✅ COMPLETED
- [x] Look for initialization order dependencies ✅ COMPLETED

#### Step 4: Texture Path Duplication Bug Fix
**Issue**: ✅ RESOLVED: Path shows `/assets/textures/assets/textures/atlas.png` (duplicated)
**Investigation Tasks**:
- [x] Check TextureAtlas::loadTexture() path resolution logic ✅ COMPLETED
- [x] Verify BASE_DIRECTORY constant usage ✅ COMPLETED
- [x] Look for double path concatenation in texture loading ✅ COMPLETED
- [x] Check if relative vs absolute path handling is correct ✅ COMPLETED

### Troubleshooting Action Plan

#### Phase A: State Management Fix (Highest Priority)
✅ COMPLETED
1. **GameStateManager Investigation**: Examined state stack operations and identified the issue
2. **ESC Key Handler Fix**: Modified the ESC key handler in InputManager.cpp to check game state before toggling:
   ```cpp
   if (game.getGameState() != GameState::MAIN_MENU || 
       (game.getStateManager() && game.getStateManager()->hasStateInStack())) {
       // Only toggle menu if we're not in main menu with empty stack
       game.toggleMenu();
   } else {
       std::cout << "[INFO] ESC key ignored in main menu with no previous state" << std::endl;
   }
   ```
3. **Added Helper Method**: Added `hasStateInStack()` method to GameStateManager to check for empty state stack
4. **Added Accessor**: Added `getStateManager()` method to Game class for state stack checking
5. **Test Results**: ESC key now properly ignores presses when in main menu with no active game, but still works when returning from gameplay to menu

This fix ensures the ESC key behaves properly in all contexts: it opens/closes the menu during gameplay but does nothing when already at the main menu with no previous game state to return to.

#### Phase B: Chunk Loading Fix (Critical for Gameplay)
✅ COMPLETED
1. **Git History Analysis**: Identified issues in recent commits affecting world/chunk loading
2. **World Loading Flow**: Traced execution path from "Resume Game" click
3. **Mesh Generation**: Identified shared buffer issue in mesh renderer
4. **Race Condition Check**: Verified VoxelMesh instances needed their own OpenGL buffers
5. **Fix and Test**: Updated MeshRenderer to use per-mesh buffers, verified chunk loading now works correctly

#### Phase C: Renderer Initialization Fix (Important for Visuals)
✅ COMPLETED
1. **Initialization Order**: Ensured proper OpenGL context before MeshRenderer initialization
2. **Shader System**: Fixed shader loading and compilation
3. **Texture Dependencies**: Ensured texture atlas loads before renderer initialization
4. **Error Handling**: Improved error messages for renderer failures

#### Phase D: Path Resolution Fix (Minor but Important)
✅ COMPLETED
1. **Path Logic Review**: Fixed duplicate path concatenation
2. **Test Texture Loading**: Verified atlas.png loads from correct path
3. **Validation**: Ensured no other path duplication issues exist

### UI Architecture Insights (From Recent Investigation)

Our recent work on fixing UI positioning has revealed important aspects of the UI architecture:

1. **UI Position Management**:
   - UI elements have dedicated positioning methods like `centerBottomOfScreen()` and `centerOnScreen()`
   - Positions must be updated when: initializing, resizing window, toggling fullscreen, and during state transitions
   - Current implementation successfully handles all these scenarios

2. **UI-State Integration**:
   - UI visibility and positioning is tightly coupled with game state transitions
   - UI elements properly show/hide based on GameState (menu vs. gameplay)
   - State transitions now correctly update UI element positions

3. **Window Integration**:
   - UI is resolution-independent, positioning based on current window dimensions
   - Fullscreen toggle works correctly with both F11 and menu checkbox

These insights will be valuable when implementing the World Selection and Creation UI components.

### Expected Outcomes After Fixes
- [x] ✅ FIXED: Chunks load with correct terrain after resuming game
- [x] ✅ FIXED: MeshRenderer initializes successfully 
- [x] ✅ FIXED: Textures load from correct paths
- [x] ✅ FIXED: UI positioned correctly at all times (game start, resume, resize, fullscreen)
- [x] ✅ FIXED: ESC key disabled in main menu when no game is running
- [ ] Game state transitions work smoothly
- [ ] No console errors during normal menu/game flow

---

## Detailed Implementation Plan

### Task 1: World Creation & Management Menu System

#### 1.1 Main Menu Integration
**File**: `game/src/ui/MenuSystem.cpp`, `game/include/ui/MenuSystem.h`

**Implementation**:
- [ ] Add "Play" button to main menu (replaces current direct game start)
- [ ] Add "Create New World" button
- [ ] Add "Load World" button  
- [ ] Add "Options" button
- [ ] Add "Quit" button

**UI Layout**:
```
╔══════════════════════════════════════╗
║              VOXEL CASTLE            ║
║                                      ║
║         [    Play Game    ]          ║
║         [ Create New World ]         ║
║         [   Load World    ]          ║
║         [    Options     ]           ║
║         [     Quit       ]           ║
╚══════════════════════════════════════╝
```

#### 1.2 Create New World Dialog
**Files**: `game/src/ui/dialogs/CreateWorldDialog.cpp`, `game/include/ui/dialogs/CreateWorldDialog.h`

**Features**:
- [ ] **World Name**: Text input field (default: "New World")
- [ ] **Seed**: Number/text input (default: random)
- [ ] **World Type**: Dropdown (Normal, Flat, Amplified)
- [ ] **Game Mode**: Dropdown (Creative, Survival, Adventure)
- [ ] **Generate Structures**: Checkbox (villages, dungeons, etc.)
- [ ] **Create World** button
- [ ] **Cancel** button

**Dialog Layout**:
```
╔══════════════════════════════════════╗
║           Create New World           ║
║                                      ║
║ World Name: [_______________]        ║
║ Seed:       [_______________]        ║
║ World Type: [Normal        ▼]        ║
║ Game Mode:  [Creative      ▼]        ║
║ □ Generate Structures                ║
║                                      ║
║        [Create World] [Cancel]       ║
╚══════════════════════════════════════╝
```

#### 1.3 World Selection List
**Files**: `game/src/ui/dialogs/WorldListDialog.cpp`, `game/include/ui/dialogs/WorldListDialog.h`

**Features**:
- [ ] Scrollable list of existing worlds
- [ ] For each world show:
  - [ ] World name
  - [ ] Last played date/time
  - [ ] World size (MB)
  - [ ] Game mode
  - [ ] World version
- [ ] **Play Selected World** button
- [ ] **Delete World** button (with confirmation)
- [ ] **Create New World** button
- [ ] **Cancel** button

**World List Layout**:
```
╔══════════════════════════════════════╗
║             Select World             ║
║                                      ║
║ ┌──────────────────────────────────┐ ║
║ │ ► My Castle World                │ ║
**Files**: `game/src/core/Game.cpp`, `platform/src/Window.cpp`

**Implementation**:
- [x] Add F11 key binding in input handler ✅ COMPLETED
- [x] Add `Window::toggleFullscreen()` method ✅ COMPLETED
- [x] Support borderless windowed fullscreen ✅ COMPLETED
- [x] Preserve resolution when switching modes ✅ COMPLETED
- [x] Handle multi-monitor setups properly ✅ COMPLETED

#### 1.5 World Loading Screen
**Files**: `game/src/ui/LoadingScreen.cpp`, `game/include/ui/LoadingScreen.h`

**Features**:
- [ ] Progress bar for world generation/loading
- [ ] Status messages ("Generating terrain...", "Loading chunks...", etc.)
- [ ] Cancel button (return to world selection)
- [ ] Estimated time remaining

**Loading Screen Layout**:
```
╔══════════════════════════════════════╗
║           Loading World...           ║
║                                      ║
║ Generating terrain chunks...         ║
║ ████████████░░░░░░░░  60%            ║
║                                      ║
║ Estimated time remaining: 30s        ║
║                                      ║
║              [Cancel]                ║
╚══════════════════════════════════════╝
```

### Task 2: Direct Chunk Persistence System

#### 2.1 Remove SaveManager Architecture
**Files**: Remove entire SaveManager class system

**Changes**:
COMPLETED- [ ] Delete `game/src/core/SaveManager.cpp` and `game/include/core/SaveManager.h`
- [ ] Remove SaveManager references from Game class
- [ ] Remove F5/F9 hotkey system
- [ ] Remove centralized save/load logic

#### 2.2 ChunkColumn Auto-Persistence
**Files**: `engine/src/world/chunk_column.cpp`, `engine/include/world/chunk_column.h`

**New Methods**:
```cpp
class ChunkColumn {
private:
    bool m_isDirty = false;
    std::chrono::steady_clock::time_point m_lastSaveTime;
    std::string m_filePath;

public:
    // Mark chunk as needing save
    void markDirty() { m_isDirty = true; }
    
    // Save chunk to disk (non-blocking)
    void saveToFile();
    
    // Check if chunk needs saving
    bool needsSave() const;
    
    // Load chunk from disk
    static std::unique_ptr<ChunkColumn> loadFromFile(const std::string& path);
};
```

#### 2.3 Background Persistence Thread
**Files**: `engine/src/world/world_manager.cpp`, `engine/include/world/world_manager.h`

**Implementation**:
```cpp
class WorldManager {
private:
    std::thread m_saveThread;
    std::atomic<bool> m_saveThreadRunning{true};
    std::queue<WorldCoordXZ> m_chunkSaveQueue;
    std::mutex m_saveQueueMutex;

public:
    // Background thread that saves dirty chunks
    void chunkSaveWorker();
    
    // Queue chunk for saving
    void queueChunkSave(const WorldCoordXZ& coord);
    
    // Modified setVoxel to trigger auto-save
    void setVoxel(int64_t x, int64_t y, int64_t z, const Voxel& voxel) override;
};
```

#### 2.4 Region File Format Implementation
**Files**: `engine/src/world/RegionFile.cpp`, `engine/include/world/RegionFile.h`

**Minecraft-Compatible Format**:
- [ ] 32x32 chunk regions stored in `.mca` files
- [ ] Header with chunk timestamps and offsets
- [ ] LZ4/Zlib compression for chunk data
- [ ] Atomic writes to prevent corruption

### Task 3: World Directory Structure

#### 3.1 World Management Class
**Files**: `game/src/world/WorldManager.cpp`, `game/include/world/WorldManager.h`

**Responsibilities**:
- [ ] Create new world directories
- [ ] Scan for existing worlds
- [ ] Load world metadata
- [ ] Generate initial spawn chunks

#### 3.2 Level.dat Format
**File**: Each world's `level.dat` file

**JSON Format**:
```json
{
  "version": 1,
  "worldName": "My Castle World",
  "seed": 1234567890,
  "gameMode": "creative",
  "worldType": "normal",
  "spawnX": 0,
  "spawnY": 64,
  "spawnZ": 0,
  "generateStructures": true,
  "createdDate": "2024-12-06T10:30:00Z",
  "lastPlayed": "2024-12-06T15:45:23Z",
  "playTime": 7200
}
```

#### 3.3 Directory Structure
```
/worlds/
  castle_world_1/
    level.dat              # World metadata
    region/                # Chunk data
      r.0.0.mca           # Region files (32x32 chunks each)
      r.0.1.mca
      r.-1.0.mca
    playerdata/            # Player information
      player.dat          # Single-player data
    data/                  # Generated structures, etc.
      structures.dat
    screenshots/           # Optional world thumbnails
      thumbnail.png
```

## Technical Implementation Details

### World Creation Flow
1. **Menu Selection**: Player clicks "Create New World"
2. **Input Dialog**: Player enters name, seed, settings
3. **Directory Creation**: Create `/worlds/world_name/` structure
4. **Metadata Generation**: Create `level.dat` with settings
5. **Initial Generation**: Generate spawn area chunks (3x3 chunk area)
6. **Player Spawn**: Place player at spawn point
7. **Continue Generation**: Generate chunks as player explores

### Chunk Persistence Flow
1. **Block Change**: Player places/destroys block
2. **Mark Dirty**: ChunkColumn.markDirty() called immediately
3. **Queue Save**: Add chunk coordinates to save queue
4. **Background Save**: Worker thread saves chunk to region file
5. **Atomic Write**: Ensure save completes or fails cleanly

### World Loading Flow
1. **World Selection**: Player chooses world from list
2. **Metadata Loading**: Read `level.dat` for world settings
3. **Player Positioning**: Set player position to last known location
4. **Chunk Loading**: Load chunks around player from region files
5. **Dynamic Loading**: Load additional chunks as player moves

## Integration Points

### Game Class Changes
**File**: `game/src/core/Game.cpp`

**New Flow**:
```cpp
// Remove old save/load methods
// Remove saveManager_ member
// Add world selection state

enum class GameState {
    MAIN_MENU,
    WORLD_SELECTION,
    CREATE_WORLD,
    PLAYING,
    // ...
};
```

### Input Handler Changes
**File**: `game/src/core/Game.cpp`

**Key Binding Updates**:
```cpp
// Remove F5/F9 save/load bindings
// Add F11 fullscreen toggle
case SDLK_F11:
    window_->toggleFullscreen();
    break;
```

### Menu System Integration
**Files**: Menu system files

**New Menu Flow**:
1. Main Menu → World Selection
2. World Selection → Create World Dialog (optional)
3. World Selection → World Loading Screen
4. Loading Screen → In-Game

## Migration Plan

### Phase 1: World Management Foundation
- [ ] Implement world directory structure
- [ ] Create world metadata system (level.dat)
- [ ] Build world creation/selection UI
- [x] Add F11 fullscreen toggle ✅ COMPLETED
- [ ] Implement loading screen with progress

### Phase 2: Direct Chunk Persistence  
- [ ] Remove SaveManager class entirely
- [ ] Implement ChunkColumn auto-save methods
- [ ] Add background save thread to WorldManager
- [ ] Implement region file format

### Phase 3: Seed-Based World Generation
- [ ] Implement core seed system for deterministic generation
- [ ] Create world type generators (Normal, Flat, Amplified)
- [ ] Add biome generation system
- [ ] Implement structure generation (villages, dungeons, etc.)
- [ ] Add cave and ore generation systems

### Phase 4: Integration & Testing
- [ ] Connect menu system to world management
- [ ] Test world creation/loading flow with different world types
- [ ] Verify chunk persistence works correctly
- [ ] Performance testing with auto-save and world generation
- [ ] Test seed reproducibility across sessions

### Phase 5: Polish & Documentation
- [ ] Add loading progress indicators for world generation phases
- [ ] Implement world deletion/backup functionality
- [ ] Add world thumbnails/screenshots
- [ ] Update all documentation
- [ ] Remove legacy save/load references

## Benefits of New Architecture

1. **Simpler Code**: No complex SaveManager, direct chunk persistence
2. **Better Performance**: No centralized save bottlenecks
3. **Minecraft-like UX**: Familiar world selection and auto-save behavior
4. **Crash Safety**: Atomic chunk saves prevent data corruption
5. **Scalability**: Background saves don't block gameplay

## Files to Update

### Remove (Legacy Save System)
- `game/src/core/SaveManager.cpp`
- `game/include/core/SaveManager.h`
- All F5/F9 hotkey handlers
- Manual save/load menu options

### Create (New World System)
- `game/src/ui/dialogs/CreateWorldDialog.cpp`
- `game/include/ui/dialogs/CreateWorldDialog.h`
- `game/src/ui/dialogs/WorldListDialog.cpp`
- `game/include/ui/dialogs/WorldListDialog.h`
- `game/src/world/WorldManager.cpp`
- `game/include/world/WorldManager.h`
- `engine/src/world/RegionFile.cpp`
- `engine/include/world/RegionFile.h`
- `game/src/ui/LoadingScreen.cpp`
- `game/include/ui/LoadingScreen.h`

### Create (World Generation System)
- `engine/src/world/WorldGenerator.cpp`
- `engine/include/world/WorldGenerator.h`
- `engine/src/world/generators/NormalWorldGenerator.cpp`
- `engine/include/world/generators/NormalWorldGenerator.h`
- `engine/src/world/generators/FlatWorldGenerator.cpp`
- `engine/include/world/generators/FlatWorldGenerator.h`
- `engine/src/world/generators/AmplifiedWorldGenerator.cpp`
- `engine/include/world/generators/AmplifiedWorldGenerator.h`
- `engine/src/world/biomes/BiomeGenerator.cpp`
- `engine/include/world/biomes/BiomeGenerator.h`
- `engine/src/world/structures/StructureGenerator.cpp`
- `engine/include/world/structures/StructureGenerator.h`
- `engine/src/world/generators/CaveGenerator.cpp`
- `engine/include/world/generators/CaveGenerator.h`
- `engine/src/world/generators/OreGenerator.cpp`
- `engine/include/world/generators/OreGenerator.h`

### Modify (Integration)
- `game/src/core/Game.cpp` - Remove SaveManager, add world states
- `game/src/ui/MenuSystem.cpp` - Add world selection flow
- `engine/src/world/world_manager.cpp` - Add auto-save thread
- `engine/src/world/chunk_column.cpp` - Add persistence methods
- `platform/src/Window.cpp` - Add fullscreen toggle

### Task 5: Seed-Based World Generation Implementation

#### 5.1 Seed System Architecture
**Files**: `engine/src/world/WorldGenerator.cpp`, `engine/include/world/WorldGenerator.h`

**Core Seed System**:
```cpp
class WorldGenerator {
private:
    uint64_t m_worldSeed;
    std::mt19937_64 m_rng;
    
public:
    // Initialize with seed (string or numeric)
    void setSeed(const std::string& seedString);
    void setSeed(uint64_t numericSeed);
    
    // Get deterministic random for specific coordinates
    uint64_t getRegionSeed(int32_t x, int32_t z);
    uint64_t getChunkSeed(int32_t x, int32_t z);
    uint64_t getBlockSeed(int64_t x, int64_t y, int64_t z);
};
```

#### 5.2 World Type Implementation
**Files**: `engine/src/world/generators/`, multiple files

**World Type Classes**:
- `NormalWorldGenerator.cpp` - Standard terrain with caves and structures
- `FlatWorldGenerator.cpp` - Flat creative world with customizable layers
- `AmplifiedWorldGenerator.cpp` - Extreme terrain with 2x height variation

**World Type Selection**:
```cpp
enum class WorldType {
    NORMAL,
    FLAT,
    AMPLIFIED
};

class WorldTypeFactory {
public:
    static std::unique_ptr<WorldGenerator> createGenerator(WorldType type, uint64_t seed);
};
```

#### 5.3 Biome System Implementation
**Files**: `engine/src/world/biomes/`, multiple files

**Biome Types**:
- Plains - Flat grassy areas with occasional trees
- Forest - Dense tree coverage with varied heights
- Mountains - High altitude terrain with stone exposure
- Desert - Sandy terrain with cacti and limited vegetation
- Ocean - Large water bodies with underwater features
- Caves - Underground biome with special generation rules

**Biome Generation**:
```cpp
class BiomeGenerator {
private:
    PerlinNoise m_temperatureNoise;
    PerlinNoise m_humidityNoise;
    
public:
    BiomeType getBiome(int64_t x, int64_t z, uint64_t seed);
    float getTemperature(int64_t x, int64_t z);
    float getHumidity(int64_t x, int64_t z);
};
```

#### 5.4 Structure Generation System
**Files**: `engine/src/world/structures/`, multiple files

**Structure Types**:
- Villages - Groups of buildings with NPCs
- Dungeons - Underground rooms with spawners and loot
- Strongholds - Large underground fortresses
- Mineshafts - Abandoned mining tunnels
- Caves - Natural cave systems

**Structure Placement**:
```cpp
class StructureGenerator {
public:
    // Check if structure should generate at location
    bool shouldGenerateStructure(StructureType type, int32_t chunkX, int32_t chunkZ, uint64_t seed);
    
    // Generate structure in chunk
    void generateStructure(StructureType type, ChunkColumn* chunk, uint64_t seed);
};
```

#### 5.5 Cave and Ore Generation
**Files**: `engine/src/world/generators/CaveGenerator.cpp`, `engine/src/world/generators/OreGenerator.cpp`

**Cave System**:
```cpp
class CaveGenerator {
private:
    PerlinNoise m_caveNoise;
    PerlinNoise m_tunnelNoise;
    
public:
    // Generate cave systems using 3D noise
    bool isCaveBlock(int64_t x, int64_t y, int64_t z, uint64_t seed);
    
    // Generate specific cave types (caverns, tunnels, ravines)
    void generateCaves(ChunkColumn* chunk, uint64_t seed);
};
```

**Ore Distribution**:
```cpp
class OreGenerator {
public:
    // Different ore types with specific y-level distributions
    void generateCoal(ChunkColumn* chunk, uint64_t seed);    // y: 5-52
    void generateIron(ChunkColumn* chunk, uint64_t seed);    // y: 5-54
    void generateGold(ChunkColumn* chunk, uint64_t seed);    // y: 5-29
    void generateDiamond(ChunkColumn* chunk, uint64_t seed); // y: 5-12
    
    // Ore vein generation with realistic clustering
    void generateOreVein(ChunkColumn* chunk, VoxelType ore, int centerX, int centerY, int centerZ, int size);
};
```

#### 5.6 Texture ID Handling
**Files**: `engine/src/render/TextureAtlas.cpp`, `engine/include/render/TextureAtlas.h`

**Texture ID Management**:
```cpp
class TextureAtlas {
private:
    std::unordered_map<std::string, Texture> m_textures;
    std::vector<std::string> m_textureIDs;

public:
    // Load texture from file and assign an ID
    std::string loadTexture(const std::string& filePath);
    
    // Retrieve texture by ID
    const Texture& getTexture(const std::string& textureID) const;
    
    // Clean up textures
    void clear();
};
```

**MeshRenderer Integration**:
```cpp
class MeshRenderer {
private:
    TextureAtlas& m_textureAtlas;

public:
    // Constructor
    MeshRenderer(TextureAtlas& textureAtlas);
    
    // Render mesh with texture
    void render(const Mesh& mesh, const std::string& textureID);
};
```

**HUD & UI Integration**:
- Update HUD and UI element code to obtain texture IDs from TextureAtlas
- Ensure consistent texture usage across the application
