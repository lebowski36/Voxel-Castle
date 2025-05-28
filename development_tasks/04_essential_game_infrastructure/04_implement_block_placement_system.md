# Task: Implement Block Placement System

## Status: 🔄 IN PROGRESS

## Overview
Implement interactive block placement and removal system with real-time mesh updates, block selection, and UI feedback. This system allows players to modify the voxel world through mouse interactions and see immediate visual feedback.

## Implementation Phases

### Phase 1: Core Block Interaction System ✅ COMPLETE
**Goal:** Implement basic block placement and removal with mouse controls.

- [x] **1.1: Mouse Ray Casting**
  - ✅ Implement screen-to-world ray casting from camera position
  - ✅ Calculate ray direction from mouse position (center of screen)
  - ✅ Add ray-voxel intersection testing to find target block/face
  - ✅ Return both the block being pointed at and the adjacent face for placement

- [x] **1.2: Block Placement Logic**
  - ✅ Implement left mouse button handler for block placement
  - ✅ Place new block adjacent to the face being pointed at
  - ✅ Ensure placement doesn't overlap with player position
  - ✅ Validate placement position is within world bounds

- [x] **1.3: Block Removal Logic**
  - ✅ Implement right mouse button handler for block removal
  - ✅ Remove block that mouse is pointing at (replace with AIR)
  - ✅ Ensure removal doesn't break world structure (if applicable)
  - ✅ Handle edge cases (trying to remove AIR blocks)

### Phase 2: Block Selection & UI System ✅ COMPLETE
**Goal:** Add block type selection and visual feedback.

- [x] **2.1: Block Type Management** ✅ COMPLETE
  - ✅ Create block type enumeration/list (exclude AIR from selectable types)
  - ✅ Implement current block type tracking in game state
  - ✅ Add mouse wheel scroll handler for cycling through block types
  - ✅ Initialize with reasonable default block type (e.g., STONE)

- [ ] **2.2: Block Selection UI**
  - Implement small square UI element at bottom center of screen
  - Display texture of currently selected block type
  - Ensure UI element scales properly with screen resolution
  - Add proper UI rendering in the game loop

- [ ] **2.3: Visual Feedback**
  - Add block outline/highlight for block being pointed at
  - Show placement preview (where new block will be placed)
  - Add visual confirmation when blocks are placed/removed
  - Consider adding sound effects for placement/removal actions

### Phase 3: Dynamic Mesh Updates 🐞 BUG HUNTING
**Goal:** Resolve critical bug causing window disappearance upon block modification. Update mesh generation system to handle real-time world changes.

**CURRENT BLOCKING ISSUE:**
- 🐞 **BUG**: When a visible voxel in a fully loaded chunk is modified (via left or right mouse click for placement/removal), the game window disappears. The application continues running in the console, requiring a manual `Ctrl+C` to terminate. This suggests a critical failure in rendering or window handling post-block modification.

**Previously Noted Issues (Potentially Related or Masked by Current Blocker):**
- ❌ **BLOCKING BUG (Original Note)**: When mouse is captured (Escape pressed), any mouse click causes immediate window close/crash (This might be the same issue, or a related one, now more specifically identified with block modification).
- ❌ **Missing mesh regeneration**: Block removal doesn't trigger mesh updates (missing `markChunkDirtyForPosition` call) - *This needs re-verification once the window disappearance is fixed.*
- ❌ **Potential threading issues**: Mesh updates may not be thread-safe with game loop - *To be investigated.*
- ❌ **Namespace inconsistencies**: Mixed VoxelCastle/VoxelEngine namespaces could cause runtime issues - *Review during investigation.*

**IMMEDIATE FIXES NEEDED (Revised Focus):**
1.  **Investigate and Fix Window Disappearance:** Determine the root cause of the game window vanishing upon block placement/removal in loaded chunks. This is the top priority.
2.  Verify `markChunkDirtyForPosition` is correctly called for both placement and removal.
3.  Verify thread safety of world modification and mesh regeneration.
4.  Test basic block placement/removal functionality thoroughly once the window issue is resolved.

- [ ] **3.1: Chunk Modification Tracking**
  - ⚠️ `markChunkDirtyForPosition` exists. Verify it's called correctly for *both* placement and removal.
  - ⚠️ Verify that mesh regeneration actually happens after marking dirty (once window issue is fixed).
  - Handle cross-chunk boundary updates (blocks affecting adjacent chunks).

- [ ] **3.2: Real-time Mesh Regeneration**
  - ⚠️ Integration with `MeshJobSystem` needs verification.
  - ⚠️ Ensure mesh updates don't conflict with rendering thread.
  - Handle mesh updates without blocking game loop.
  - Ensure visual updates happen immediately after block changes.

- [ ] **3.3: Memory Management**
  - Implement efficient storage for chunk modifications.
  - Track dirty/clean state of chunks.
  - Consider memory limits and cleanup strategies.
  - Optimize data structures for fast lookup and modification

### Phase 4: Advanced Features & Polish ⏳ PENDING
**Goal:** Add sophisticated features and optimize the system.

- [ ] **4.1: Advanced Placement Rules**
  - Implement placement validation (can't place blocks inside player)
  - Add support for different block properties (solid vs. non-solid)
  - Handle special placement rules for different block types
  - Add undo/redo functionality for block operations

- [ ] **4.2: Performance Optimization**
  - Optimize ray casting for better performance
  - Batch mesh updates when multiple blocks change rapidly
  - Implement LOD considerations for distant chunks
  - Profile and optimize bottlenecks in the placement system

- [ ] **4.3: Enhanced User Experience**
  - Add crosshair/targeting reticle in center of screen
  - Implement distance limits for block placement/removal
  - Add keyboard shortcuts for common block types
  - Consider adding creative mode vs. survival mode logic

## Technical Considerations

### Integration Points
- **Input System**: Extend existing mouse and keyboard handlers
- **Rendering System**: Add UI rendering for block selection display
- **World Manager**: Extend with block modification capabilities
- **Mesh Generation**: Modify to support incremental updates
- **Camera System**: Use for ray casting calculations

### Dependencies
- Requires functional camera system (✅ Complete)
- Requires mesh generation system (✅ Complete)
- Requires input handling system (✅ Complete)
- May require UI framework extension

### Testing Strategy
- Test placement/removal in various world locations
- Verify mesh updates happen correctly and efficiently
- Test edge cases (world boundaries, player collision)
- Performance testing with rapid block modifications
- UI responsiveness and visual feedback testing
