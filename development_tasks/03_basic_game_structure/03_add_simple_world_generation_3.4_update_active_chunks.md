# Sub-Sub-Task: Implement WorldManager::updateActiveChunks for Dynamic Chunk Loading

**Parent Task:** [03_add_simple_world_generation.md](03_add_simple_world_generation.md)
**Sub-Task:** 3.4. Implement `WorldManager::updateActiveChunks` for Dynamic Loading

---

## Overview
Implement a method in `WorldManager` to dynamically load and generate chunk segments in a configurable radius around a given world position (typically the camera/player). This enables seamless world streaming as the player moves.

## Steps

### 1. Method Declaration & Signature
- [ ] Add the following method to `engine/include/world/world_manager.h`:
  ```cpp
  void updateActiveChunks(const glm::vec3& centerWorldPosition, int loadRadiusInSegments, WorldGenerator& generator);
  ```

### 2. Implementation Logic
- [ ] Implement the method in `engine/src/world/world_manager.cpp`:
  - Calculate the center segment indices from `centerWorldPosition` using segment size constants.
  - Loop over X, Y, Z segment indices within the specified radius.
  - For each segment:
    - Ensure the corresponding `ChunkColumn` and `ChunkSegment` exist (create if needed).
    - If the segment is new or empty, call `generator.generateChunkSegment`.
    - Mark the segment as dirty so its mesh will be rebuilt.
  - Add debug logging to confirm which segments are being generated/updated.
  - **Unloading:**
    - Identify all chunk columns/segments that are outside the active radius.
    - Remove these from memory and ensure they are no longer rendered or simulated.
    - Free associated resources (meshes, voxel data, etc.).
    - Add debug logging to confirm which segments/columns are being unloaded.

### 3. Edge Cases & Consistency
- [ ] Ensure coordinate conversions (world <-> segment <-> column) are correct and consistent with the rest of the codebase.
- [ ] Handle vertical range (Y) appropriately (e.g., fixed range or based on camera height).
- [ ] Avoid redundant generation for already existing/filled segments.

### 4. Testing & Verification
- [ ] Add temporary logging to verify correct segments are loaded/generated and unloaded as the camera moves.
- [ ] (Optional) Add a debug command or toggle to print currently active segments.
- [ ] Prepare for integration: ensure the method can be called from the game loop without side effects.

---
---
## Acceptance Criteria (updated)

- The method `WorldManager::updateActiveChunks` exists and matches the specified signature.
- Chunks/segments within the given radius of the center position are created/generated as needed.
- Chunks/segments outside the radius are unloaded and removed from memory/rendering.
- No redundant generation for already existing/filled segments.
- Segments are marked dirty so their meshes are rebuilt and rendered.
- Debug logging confirms which segments/columns are loaded/generated and which are unloaded.
- The method can be safely called from the game loop and responds to camera movement.

**Last Updated:** 2025-05-14
