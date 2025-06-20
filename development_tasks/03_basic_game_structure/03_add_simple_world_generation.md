# Task: Add Simple World Generation

> **⚠️ LEGACY/OBSOLETE FILE ⚠️**
> 
> This file is part of the legacy world generation system and is being replaced by the new 08c world generation system.
> **For new development, see:** `04_essential_game_infrastructure/08c_world_generation_system.md`
> 
> This file is kept for historical reference and understanding the current legacy implementation.

**Date Created:** 2025-05-12
**Status:** Complete (Legacy - Being Replaced)
**Depends On:** Voxel Data Structures, Chunk Management, Basic Game Loop, Camera System
**Leads To:** ~~Procedural Content~~ → **Superseded by 08c World Generation System**

## 1. Overview
This task focuses on implementing and integrating a basic procedural world generation system to populate the game world with voxel terrain, building upon the existing `WorldGenerator` and moving from any static test displays to a dynamic system.

## 2. Goals
*   Integrate the existing noise-based `WorldGenerator` to create an initial viewable world.
*   Transition from a static, pre-defined test chunk (if any is currently rendered by default) to dynamically generated chunks.
*   Implement dynamic loading of chunks around the camera.
*   Ensure correct coordinate mapping between world generation, chunk management, and rendering.

## 3. Sub-Tasks

### Phase 1: Integrate Existing `WorldGenerator` for Initial Static World Display

- [x] **3.1. Verify `WorldGenerator::generateChunkSegment` Logic and Coordinates**
    - **Status:** Complete
    - **Goal:** Confirm the existing `WorldGenerator::generateChunkSegment` correctly uses its input parameters (`worldX`, `worldY`, `worldZ` which represent segment-level world coordinates/indices) to calculate per-voxel global coordinates for noise sampling and voxel type determination.
    - **Current `WorldGenerator` Parameters:** `generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ)` where `worldX`, `worldY`, `worldZ` are segment indices in the world.
    - **Noise Calculation:** `float nx = (worldX * width + x) * 0.05f;` (similar for `nz`). This uses `worldX` (segment index) and `x` (local voxel X in segment) to derive a global X for noise. This is correct for continuous noise.
    - **Global Y Calculation:** `int globalY = baseY + y;` where `baseY = worldY * height;`. This is correct for calculating global Y voxel positions based on segment Y index.
    - **Action:**
        - Review `engine/src/world/world_generator.cpp` - `generateChunkSegment` method.
        - Add temporary logging inside `generateChunkSegment` to print:
            - Input `worldX, worldY, worldZ` (segment indices).
            - Calculated `nx, nz` for noise sampling for a few local `x, z` (e.g., 0,0 and 15,15).
            - Calculated `globalY` and resulting `VoxelType` for a few local `x,y,z` (e.g., 0,0,0 and 15,15,15).
    - **Verification:** Ensure the logged global coordinates and noise inputs make sense for the segment being generated. For example, if generating segment (0,0,0), the noise inputs should be based on global voxel coordinates within that segment's bounds (e.g., x from 0 to 31, z from 0 to 31 for noise, global Y from 0 to 31 for voxel placement).

- [x] **3.2. Implement Initial Static World Area Generation in `Game::initialize`**
    - **Status:** Complete
    - **Goal:** Modify the game's initialization to generate a fixed-size area of chunks using the `WorldManager` and `WorldGenerator`, replacing any current hardcoded test chunk display.
    - **Action:**
        - In `game/src/core/game.cpp` (within `Game::initialize` or a new dedicated method called from it, after `worldManager_` and `worldGenerator_` are ready):
            - Define a starting center for generation (e.g., segment indices `0,0,0`).
            - Define a small radius for initial generation, e.g., `initialLoadRadiusInSegments = 1` (this would mean a 3x3 area of chunk columns, and a few segments vertically, e.g., Y segment indices 0 and 1).
            - Loop for X segment indices from `centerSegX - initialLoadRadiusInSegments` to `centerSegX + initialLoadRadiusInSegments`.
            - Loop for Z segment indices from `centerSegZ - initialLoadRadiusInSegments` to `centerSegZ + initialLoadRadiusInSegments`.
            - Loop for Y segment indices (e.g., from `0` to `1`).
                - Inside the innermost loop, for `loopSegX, loopSegY, loopSegZ`:
                    - Call `worldManager_->getOrCreateChunkColumn(loopSegX, loopSegZ)` to ensure the column exists. (Note: `getOrCreateChunkColumn` might take world coordinates or column indices; ensure consistency. If it takes world coordinates, convert `loopSegX * CHUNK_WIDTH`, etc. For now, assume it takes segment/column indices if that's simpler, or adjust as per its actual signature). Let's assume `getOrCreateChunkColumn(int colX, int colZ)` takes column indices, which are the same as segment X and Z indices.
                    - Retrieve or create the specific segment: `ChunkSegment* segment = worldManager_->getOrCreateChunkColumn(loopSegX, loopSegZ)->getOrCreateSegment(loopSegY);`
                    - If the segment was newly created (or if we want to force regeneration for this test by checking a flag or if `segment->isEmpty()` for example):
                        - Call `worldGenerator_->generateChunkSegment(*segment, loopSegX, loopSegY, loopSegZ);`
                        - Mark the segment's mesh as dirty: `segment->markDirty(true);`
    - **Build System Note:** Ensure `WorldGenerator` is accessible to `Game` (it likely is via `WorldManager` or direct instantiation if `WorldManager` holds a `WorldGenerator` instance).
    - **Test:**
        - Run the application.
        - Add logging in `WorldManager::getOrCreateChunkColumn`, `ChunkColumn::getOrCreateSegment`, and `WorldGenerator::generateChunkSegment` to confirm that the expected number of columns and segments are being created/retrieved and passed to the generator.
        - **Crucially, ensure any previous code that was displaying a single hardcoded test chunk (e.g., in `main.cpp` or `Game::run` or `Game::initialize`) is disabled or removed so we only see the newly generated world.**
    - **Verification:** The game should now display a small, static area of procedurally generated terrain. The camera should be positioned to see this generated area.

- [x] **3.3. Ensure Generated Chunks are Meshed and Rendered**
    - **Status:** Complete
    - **Goal:** Verify that the newly generated chunks have their meshes built and are rendered.
    - **Action:**
        - In `Game::update` (or wherever chunk meshing is handled, likely `worldManager_->rebuildDirtySegmentMeshes(...)`), ensure that dirty segments (marked in 3.2) have their meshes rebuilt.
        - Ensure `worldManager_->renderVisibleMeshes(camera, mainShader);` (or equivalent) is called in `Game::render`.
    - **Verification:** The static world generated in 3.2 should be visible and match the appearance of the screenshot provided (grass, dirt, stone layers based on noise).




### Phase 2: Transition to Dynamic Chunk Loading

- [x] **3.4. Implement `WorldManager::updateActiveChunks` for Dynamic Loading**
    - **Status:** In Progress
    - **Goal:** Create a mechanism in `WorldManager` to load/generate chunks in a radius around a given center point (e.g., camera position).
    - **Action:**
        - Add a new method to `engine/include/world/world_manager.h` and implement it in `engine/src/world/world_manager.cpp`:
          `void updateActiveChunks(const glm::vec3& centerWorldPosition, int loadRadiusInSegments, WorldGenerator& generator);` (passing generator if not a member)
        - Inside this method:
            - Calculate the segment indices of the `centerWorldPosition`:
                - `centerSegX = static_cast<int>(std::floor(centerWorldPosition.x / ChunkSegment::CHUNK_WIDTH));`
                - `centerSegY = static_cast<int>(std::floor(centerWorldPosition.y / ChunkSegment::CHUNK_HEIGHT));`
                - `centerSegZ = static_cast<int>(std::floor(centerWorldPosition.z / ChunkSegment::CHUNK_DEPTH));`
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
    - **Verification:** This method itself doesn't produce visual output yet but is foundational. Add logging to confirm it calculates the correct segment ranges and attempts to create/generate them.

- [x] **3.5. Call `updateActiveChunks` from the Game Loop**
    - **Status:** TODO
    - **Goal:** Regularly update active chunks based on camera movement.
    - **Action:**
        - In `game/src/core/game.cpp` within `Game::update`:
            - Get the camera's current world position: `glm::vec3 cameraPos = camera_->getPosition();`
            - Define a `desiredLoadRadiusInSegments` (e.g., 2 or 3 for testing).
            - Call `worldManager_->updateActiveChunks(cameraPos, desiredLoadRadiusInSegments, *worldGenerator_);` (assuming `worldGenerator_` is a member of `Game` or accessible).
    - **Optimization Note (for later):** This call could be made less frequently, e.g., only when the camera moves to a new segment, to save processing. For now, calling it every frame is acceptable for testing.
    - **Test:**
        - Run the application.
        - Move the camera around.
        - Use logging from 3.4 (and potentially in `generateChunkSegment`) to observe new segments being generated as the camera moves into new areas.
    - **Verification:** As the camera moves, new terrain should appear in the direction of movement, and the world should seem to expand dynamically.

### Phase 3: Refinement and Testing

- [ ] **3.6. Test World Generation and Visualization Thoroughly**
    - **Status:** TODO
    - **Goal:** Ensure the generated world looks as expected and there are no obvious gaps or errors in generation or rendering.
    - **Action:**
        - Fly around the generated world extensively.
        - Look for:
            - Gaps between chunks/segments.
            - Incorrect voxel types based on the heightmap logic.
            - Chunks not loading/rendering.
            - Abrupt changes or visual artifacts at segment boundaries.
    - **Verification:** The world generation should be continuous and visually consistent.

- [ ] **3.7. (Optional) Console Output for Single Chunk Slice**
    - **Status:** TODO
    - **Goal:** Create a debug utility to print a 2D slice of a generated chunk to the console for non-visual verification.
    - **Action:**
        - Create a temporary function or test case.
        - Instantiate a `ChunkSegment` and `WorldGenerator`.
        - Call `worldGenerator.generateChunkSegment(segment, 0, 0, 0);` (for segment at 0,0,0).
        - Iterate `x` and `z` from `0` to `ChunkSegment::CHUNK_WIDTH-1` and `0` to `ChunkSegment::CHUNK_DEPTH-1`. For a fixed `y` (e.g., `ChunkSegment::CHUNK_HEIGHT/2`), print the `VoxelType` ID.
    - **Verification:** The console output should match the expected pattern from the generation logic.

- [ ] **3.8. Adjust Noise Parameters and Mapping (Iterative)**
    - **Status:** TODO
    - **Goal:** Fine-tune the world generation for more visually appealing or game-appropriate terrain.
    - **Action:**
        - Experiment with the noise parameters in `engine/src/world/world_generator.cpp`:
            - `0.05f` (scale factor for noise input coordinates).
            - `height * 0.75f` and `height / 4` (scaling and offset for `columnHeight`).
        - Modify the thresholds for `GRASS`, `DIRT`, `STONE`.
        - Rebuild and run to observe changes.
    - **Verification:** Terrain features (mountains, valleys, flat areas) can be controlled by these parameters.

## 4. Notes
*   This plan assumes `WorldManager`, `ChunkColumn`, `ChunkSegment`, `MeshBuilder`, `TextureAtlas`, and the rendering pipeline are sufficiently functional from previous tasks.
*   The focus is on integrating and visualizing the *simple* heightmap-based generation. More advanced generation (caves, biomes) is for later tasks.
*   Ensure all world coordinates (voxel global, segment global indices, chunk column global indices) are handled consistently.

---
**Last Updated:** 2025-05-14
