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
  - [ ] 3.2.1. Decide on a world generation scheme:
    - [x] 3.2.1.a. Briefly evaluate a 2D Heightmap approach (Pros: simpler for basic landscapes; Cons: no overhangs/caves). (Evaluated 2025-05-14: Current code in `WorldGenerator.cpp` uses this 2D heightmap approach, suitable for initial simple world. 3D features like overhangs/caves will require a 3D density field approach later.)
    - [ ] 3.2.1.b. Briefly evaluate a 3D Density Field approach (Pros: allows overhangs/caves; Cons: slightly more complex to map noise to voxels).
    - [ ] 3.2.1.c. User to decide on the scheme based on this evaluation.
    - [ ] 3.2.1.d. Document the chosen scheme and reasoning (Decision: [SCHEME_NAME] on YYYY-MM-DD).
  - [ ] 3.2.2. Define thresholds for voxel types (e.g., air, dirt, stone, grass) based on noise value and height.
  - [ ] 3.2.3. Document the mapping logic in this file for future reference.

### 3.3. Implement World Generation Logic
  - [ ] 3.3.1. Add a `WorldGenerator` class or static methods to `WorldManager` (document location/decision).
  - [ ] 3.3.2. Implement a method to fill a `ChunkSegment` with voxels using the noise function and mapping logic.
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
