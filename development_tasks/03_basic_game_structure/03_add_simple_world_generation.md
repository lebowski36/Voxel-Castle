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
### 3.1. Choose and Integrate Noise Function
  - [x] 3.1.1. Research and select a simple noise algorithm (e.g., Perlin, Simplex, Value noise) or a lightweight C++ noise library.
  - [x] 3.1.2. If using a library, add it to the project and CMake. If implementing, create a utility header/source (e.g., `engine/include/util/noise.h`). (Decision: Implemented custom value noise in `engine/include/util/noise.h` on 2025-05-14)
  - [x] 3.1.3. Implement or integrate a function: `float getNoise(float x, float y, float z)`. (Fulfilled by `smoothValueNoise` in `engine/include/util/noise.h` as of 2025-05-14)

### 3.2. Map Noise to Voxel Types
  - [x] 3.2.1. Decide on a world generation scheme:
    - [x] 3.2.1.a. Briefly evaluate a 2D Heightmap approach (Pros: simpler for basic landscapes; Cons: no overhangs/caves). (Evaluated 2025-05-14: Current code in `WorldGenerator.cpp` uses this 2D heightmap approach, suitable for initial simple world. 3D features like overhangs/caves will require a 3D density field approach later.)
    - [x] 3.2.1.b. Briefly evaluate a 3D Density Field approach (Pros: allows overhangs/caves; Cons: slightly more complex to map noise to voxels). (Evaluated 2025-05-14: Decided against for initial simple world generation due to complexity. Reserved for advanced world generation.)
    - [x] 3.2.1.c. User to decide on the scheme based on this evaluation. (Decision 2025-05-14: Proceed with 2D Heightmap for simple world generation.)
    - [x] 3.2.1.d. Document the chosen scheme and reasoning (Decision: 2D Heightmap on 2025-05-14. Rationale: Simplicity for initial implementation, aligns with existing `WorldGenerator.cpp` structure. Provides a foundation for basic terrain. Advanced features like caves, overhangs, and more complex geological formations will be addressed in a dedicated "Advanced World Generation" phase using more sophisticated techniques, potentially including 3D density fields or advanced 2D-to-3D methods.) (Documented 2025-05-14)
  - [x] 3.2.2. Define thresholds for voxel types (e.g., air, dirt, stone, grass) based on noise value and height. (Defined in `engine/src/world/world_generator.cpp` and documented in 3.2.3 as of 2025-05-14)
  - [x] 3.2.3. Document the mapping logic in this file for future reference. (Documented 2025-05-14)
    ```markdown
    The current voxel type mapping logic is implemented in `engine/src/world/world_generator.cpp` within the `generateChunkSegment` method. It functions as follows:

    1.  **Noise-based Height Calculation:**
        *   A 2D noise value (`smoothValueNoise` using world X and Z coordinates, with Y fixed at 0.0f) is generated for each (x,z) column. The noise input coordinates are scaled by `0.05f`.
        *   This noise value (range [0,1]) is then scaled and offset to determine the surface height (`columnHeight`) for that column:
            `columnHeight = static_cast<int>(noise * (ChunkSegment::CHUNK_HEIGHT * 0.75f)) + (ChunkSegment::CHUNK_HEIGHT / 4);`
            This results in a surface height that varies between 25% and 100% of the chunk height.

    2.  **Voxel Type Layering (based on `globalY` vs `columnHeight`):**
        *   **Above Surface (`globalY > columnHeight`):** `VoxelType::AIR`
        *   **At Surface (`globalY == columnHeight`):** `VoxelType::GRASS`
        *   **Immediately Below Surface (`columnHeight - 3 < globalY < columnHeight`):** `VoxelType::DIRT`. This creates two layers of dirt directly beneath the grass layer (at `columnHeight - 1` and `columnHeight - 2`).
        *   **Deep Below Surface (`globalY <= columnHeight - 3`):** `VoxelType::STONE`. This forms the base layer of the terrain.

    This layering provides a simple but effective differentiation of basic terrain materials for the initial world generation.
    ```

### 3.3. Implement World Generation Logic
  - [x] 3.3.1. Add a `WorldGenerator` class or static methods to `WorldManager` (or similar central place) to encapsulate generation logic. (Completed 2025-05-14: Fulfilled by the existing `WorldGenerator` class located in `engine/include/world/world_generator.h` and `engine/src/world/world_generator.cpp`, which uses a static method `generateChunkSegment`.)
  - [ ] 3.3.2. Implement a method to fill a `ChunkSegment` with voxel data based on its world coordinates (e.g., `void fillChunkSegment(ChunkSegment& segment, int segmentWorldX, int segmentWorldY, int segmentWorldZ)`).
  - [ ] 3.3.3. Ensure world generation is triggered for new chunks/segments (e.g., in `WorldManager::getOrCreateChunkColumn`).

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
