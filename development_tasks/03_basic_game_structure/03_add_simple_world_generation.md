# Task: Add Simple World Generation

**Date Created:** 2025-05-12
**Status:** Not Started
**Depends On:** Voxel Data Structures, Chunk Management
**Leads To:** Procedural Content, Initial Game World

## 1. Overview
This task focuses on implementing a basic procedural world generation system to populate the game world with voxel terrain.

## 2. Goals
*   Implement a noise-based terrain generation algorithm (e.g., Perlin, Simplex).
*   Generate initial chunk structures based on this noise.

## 3. Sub-Tasks
### 3.2. Define Voxel Type Mapping (2D Heightmap based)

- [x] **3.2.1. Decide on a world generation scheme.**
    - **Decision:** For simple world generation, a 2D heightmap approach will be used. This involves generating a 2D noise pattern where the noise value at `(x, z)` determines the surface height `y`.
    - **Status:** Complete. Decision made and documented.
- [x] **3.2.2. Define thresholds for voxel types based on height or other criteria.**
    - **Current Logic (engine/src/world/world_generator.cpp):**
        - A 2D noise function (`Noise::generate`) creates a heightmap.
        - For each `(x, z)` column:
            - `surfaceY = static_cast<int>(noiseValue * CHUNK_HEIGHT * 0.5f + CHUNK_HEIGHT * 0.25f)` (This seems to scale and offset noise to world height).
            - If `y == surfaceY`: `GRASS`
            - If `y < surfaceY && y >= surfaceY - 2`: `DIRT` (Up to 2 layers of dirt)
            - If `y < surfaceY - 2`: `STONE`
            - Otherwise (above `surfaceY`): `AIR` (implicitly, as voxels are `AIR` by default)
    - **Status:** Complete. Existing logic investigated and documented.
- [x] **3.2.3. Document this mapping logic clearly.**
    - **Documentation:** The logic is now documented in sub-task 3.2.2 above.
    - **Status:** Complete.

### 3.3. Implement `WorldGenerator`

- [x] **3.3.1. Add a `WorldGenerator` class if it doesn't exist. This class will be responsible for generating chunk data.**
    - **Details:** The class `WorldGenerator` already exists in `engine/include/world/world_generator.h` and `engine/src/world/world_generator.cpp`. It uses a `Noise` utility (from `engine/include/util/noise.h`) for heightmap generation.
    - **Status:** Complete. Existing class fulfills this requirement.
- [x] **3.3.2. Implement a method within `WorldGenerator` that can fill a `ChunkSegment` (or equivalent structure) with voxel data based on the defined scheme.**
    - **Method:** `WorldGenerator::generateChunkSegment(ChunkSegment& segment, const glm::ivec3& segmentPosition)` in `world_generator.cpp` already implements this. It iterates `x, y, z` within the segment's local coordinates, calculates world coordinates, generates noise based on world `x, z`, and then sets voxel types based on the height logic described in 3.2.2.
    - **Status:** Complete. Existing method fulfills this requirement.
- [ ] **3.3.3. Ensure world generation is triggered for new chunks/segments as the player moves or as the world is initially created.**
    - [ ] **3.3.3.1. Verify and correct `WorldGenerator::generateChunkSegment` coordinate logic.**
        - **Goal:** Ensure that `worldX`, `worldY`, and `worldZ` are correctly calculated within `generateChunkSegment` based on its input parameters (e.g., `columnBaseX`, `segmentYIndexInColumn`, `columnBaseZ`) and the local voxel coordinates `(lx, ly, lz)` within the segment. The current call from `WorldManager` is `WorldGenerator::generateChunkSegment(*segment, column_coord.x, segment_y_idx, column_coord.z)`.
        - **Action:** Review `engine/src/world/world_generator.cpp` specifically the `generateChunkSegment` method. Confirm that `worldX = columnBaseX + lx`, `worldY = (segmentYIndexInColumn * SEGMENT_HEIGHT) + ly`, and `worldZ = columnBaseZ + lz` (or equivalent logic) is used when sampling noise or setting voxel types.
        - **Test:** Add temporary logging inside `generateChunkSegment` to print out the calculated `worldX, worldY, worldZ` for a few voxels (e.g., for local coords 0,0,0 and 15,15,15 in a segment) when a segment is generated. Verify these world coordinates make sense given the segment's intended position.
    - [ ] **3.3.3.2. Implement initial world area generation on game start.**
        - **Goal:** Generate a small, defined area of chunks when the game starts.
        - **Action:** In `game/src/main.cpp` (or a `Game::init()` method if it exists), after `WorldManager` and `WorldGenerator` are initialized:
            - Define a starting XZ center (e.g., `0, 0`).
            - Define a small radius (e.g., `initialLoadRadius = 1`, meaning a 3x3 area of chunk columns: -1 to +1 around the center).
            - Loop from `centerX - initialLoadRadius` to `centerX + initialLoadRadius` for X, and similarly for Z.
            - Inside the loop, call `worldManager.getOrCreateChunkColumn(loopX * CHUNK_WIDTH, loopZ * CHUNK_DEPTH);`. (Note: `getOrCreateChunkColumn` expects base world coordinates, not column indices).
        - **Test:** Run the application. Add logging in `WorldManager::getOrCreateChunkColumn` or `WorldGenerator::generateChunkSegment` to confirm that the expected number of columns and segments are being created and passed to the generator.
    - [ ] **3.3.3.3. Implement dynamic chunk generation based on player/camera position.**
        - **Goal:** Continuously generate new chunks around the player/camera as it moves.
        - **Action (Part 1 - Add Update Method):**
            - Add a new method to `WorldManager`: `void updateActiveChunks(const glm::vec3& centerPosition, int loadRadiusInChunks);`
            - In this method:
                - Calculate the current chunk column coordinates of `centerPosition`: `currentColX = static_cast<int_fast64_t>(std::floor(centerPosition.x / CHUNK_WIDTH));` (similarly for Z, using `CHUNK_DEPTH`).
                - Loop from `currentColX - loadRadiusInChunks` to `currentColX + loadRadiusInChunks` (and for Z).
                - For each `(targetColX, targetColZ)` in the loop, call `getOrCreateChunkColumn(targetColX * CHUNK_WIDTH, targetColZ * CHUNK_DEPTH);`.
        - **Action (Part 2 - Call from Game Loop):**
            - In the main game loop in `main.cpp` (or `Game::update()`):
                - Get the camera's current position.
                - Call `worldManager.updateActiveChunks(cameraPosition, desiredLoadRadius);` (e.g., `desiredLoadRadius = 4`).
        - **Test:** Run the application. Move the camera. Use logging (as in 3.3.3.2) to observe new chunk columns being generated as the camera enters new regions. Verify that chunks are generated in a radius around the camera.
    - [ ] **3.3.3.4. (Future Refinement) Optimize dynamic loading trigger.**
        - **Goal:** Avoid calling `updateActiveChunks` too frequently or when unnecessary.
        - **Considerations:** Only trigger the update if the player has moved to a new chunk column, or after a certain time interval. This is a later optimization.

### 3.4. Testing and Visualization
  - [ ] 3.4.1. Write a test to generate a single chunk and print a 2D slice of voxel types to the console.
  - [ ] 3.4.2. Visually inspect generated terrain in-game (after mesh build and render).
  - [ ] 3.4.3. Adjust noise parameters and mapping as needed for more interesting terrain.

## 4. Notes
*   The sub-tasks from `CURRENT_TODO.md` for world generation are to be detailed here.

---
*Note: This is an initial structure. For a detailed example of how this file should be fleshed out with more specific implementation details, considerations, and decisions, please refer to files in `development_tasks/01_core_engine_foundation/` or `development_tasks/02_core_systems/` (e.g., `02_create_voxel_data_structures.md`).*

---
**Last Updated:** 2025-05-12
