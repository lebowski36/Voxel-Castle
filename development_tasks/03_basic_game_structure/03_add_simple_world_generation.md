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
