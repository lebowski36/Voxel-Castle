\
# Task: Create Voxel Data Structures

**Date Created:** 2025-05-12
**Status:** In Progress
**Depends On:** ECS Implementation
**Leads To:** Mesh Generation System

## 1. Overview
This task focuses on designing and implementing the core data structures for storing and managing voxel data within the Voxel Castle engine. Efficient voxel data structures are critical for performance, especially concerning world generation, rendering, physics, and game logic. The chosen approach involves `Chunk Columns` which are vertical stacks of `Chunk Segments`.

## 2. Goals
*   Define a clear and efficient representation for individual voxels.
*   Design a system based on `Chunk Columns` and `Chunk Segments` for managing large voxel worlds. Each `Chunk Segment` will be a `32x32x32` block of voxels.
*   Implement basic management for `Chunk Columns` and `Chunk Segments` (creation, storage, retrieval).
*   Consider future needs like voxel types, lighting data, and metadata.

## 3. Sub-Tasks

### 3.1. Define Voxel Structure
*   **Status:** Completed
*   **Description:** Define the data structure for a single voxel.
    *   [x] **3.1.1.** Determine the information each voxel needs to store (e.g., type ID, active state, potentially lighting, health, etc.).
        *   **Decision:** The voxel now stores a `type_id` and a packed `light_level` byte (4 bits sunlight, 4 bits block light). This supports both basic rendering and future lighting features. See helpers in `Voxel` struct for manipulating light values.
    *   [x] **3.1.2.** Create `Voxel.h` (e.g., in `engine/include/world/voxel.h` or similar). *(Completed)*
    *   [x] **3.1.3.** Implement the `Voxel` struct/class. Keep it lightweight. *(Completed with type_id and light_level)*
        *   Example: `struct Voxel { uint8_t type_id; uint8_t light_level; /* helpers */ };`

### 3.2. Design Chunk Segment and Chunk Column Structures
*   **Status:** Completed
*   **Description:** Design the data structure for a `Chunk Segment` (a 3D array of voxels) and a `Chunk Column` (a vertical stack of segments).
    *   [x] **3.2.1.** Define `Chunk Segment` dimensions as `32x32x32` voxels (8m x 8m x 8m). A `Chunk Column` will have a `32x32` voxel footprint (8m x 8m XZ). *(Decision made)*
    *   [x] **3.2.2.** Define storage for voxels within a `Chunk Segment` (e.g., `std::array<Voxel, 32*32*32>`). *(Completed in ChunkSegment.h)*
    *   [x] **3.2.3.** Create `ChunkSegment.h` (e.g., in `engine/include/world/chunk_segment.h`). *(Completed)*
    *   [x] **3.2.4.** Implement the `ChunkSegment` class/struct. *(Completed, including basic get/set methods. Segment position is managed by ChunkColumn.)*
        *   Include methods for getting/setting voxels at local segment coordinates.
        *   Store segment's position/coordinates within the column (e.g., Y-index or absolute Y base) - *This will be handled by `ChunkColumn` which owns/manages segments.*
    *   [x] **3.2.5.** Create `ChunkColumn.h` (e.g., in `engine/include/world/chunk_column.h`). *(Completed)*
    *   [x] **3.2.6.** Implement the `ChunkColumn` class/struct. *(Completed)*
        *   Store its world XZ coordinates.
        *   Manage a collection of `ChunkSegment`s (e.g., `std::map<int, std::unique_ptr<ChunkSegment>>` where key is segment Y-index).
        *   Provide methods to get/create segments at a given Y-index/world Y coordinate.
    *   [x] **3.2.7.** Implement `getVoxel(worldX, worldY, worldZ)` and `setVoxel(worldX, worldY, worldZ, voxel)` which delegate to the correct segment (creating if necessary for `setVoxel`).
    *   [x] **3.2.8.** Implement `getSegment(segmentYIndex)` and `getOrCreateSegment(segmentYIndex)`.
    *   [x] **3.2.9.** Implement helpers: `worldYToSegmentYIndex(worldY)` and `worldToLocalSegmentCoords(...)`.
    *   [x] **3.2.10.** Add `chunk_column.cpp` to `engine/CMakeLists.txt`.
    *   [x] **3.2.11.** Test build after `ChunkColumn` implementation.

### 3.3. Implement World/Chunk Management
*   **Status:** Completed
*   **Description:** Implement a basic system for managing `ChunkColumn`s and `ChunkSegment`s.
    *   [x] **3.3.1.** Design a `World` or `ChunkManager` class (e.g., `engine/include/world/world_manager.h` and `engine/src/world/world_manager.cpp`). *(Completed with WorldManager.h/cpp)*
    *   [x] **3.3.2.** Implement a simple storage mechanism for active `ChunkColumn`s (e.g., `std::unordered_map<WorldCoordXZ, std::unique_ptr<ChunkColumn>>`). *(Completed with `std::map` in WorldManager)*
    *   [x] **3.3.3.** Implement methods to load/generate a new `ChunkColumn` (and its initial `ChunkSegment`s) at given XZ world coordinates. *(Completed with `getOrCreateChunkColumn` in WorldManager)*
        *   For now, this can just allocate new `ChunkColumn` and `ChunkSegment` objects, perhaps filled with air or a solid material.
    *   [x] **3.3.4.** Implement methods to get a `ChunkColumn` by its XZ world coordinates. *(Completed with `getOrCreateChunkColumn` in WorldManager)*
    *   [x] **3.3.5.** Implement methods to get/set a voxel at world coordinates (delegating to the appropriate `ChunkColumn` and then `ChunkSegment`). *(Completed with `getVoxel` and `setVoxel` in WorldManager)*

### 3.4. Initial Voxel Types
*   **Status:** Completed
*   **Description:** Define a few basic voxel types.
    *   [x] **3.4.1.** Create an enum or constants for basic voxel types (e.g., `AIR`, `STONE`, `DIRT`, `GRASS`) in a suitable header (e.g., `engine/include/world/voxel_types.h`). *(Completed)*

### 3.5. Integration and Testing (Basic)
*   **Status:** Completed
*   **Description:** Perform basic tests of the voxel, segment, and column structures.
    *   [x] **3.5.1.** In `main.cpp` or a test file, create a `WorldManager` instance. *(Implemented in main.cpp)*
    *   [x] **3.5.2.** Request a few `ChunkColumn`s (and implicitly `ChunkSegment`s) to be generated. *(Implemented in main.cpp via setVoxel)*
    *   [x] **3.5.3.** Set and get some voxel types within these segments using world coordinates. *(Implemented in main.cpp)*
    *   [x] **3.5.4.** Print or log the results to verify correctness. *(Implemented in main.cpp)*
    *   [x] **3.5.5.** Build and run to confirm basic functionality. *(Completed)*
    *   [ ] **3.5.6.** **When testing future features (e.g., lighting), ensure that tests include visible results in the rendered output, not just wireframe or console output. Visual feedback is required for lighting and mesh features.**

### 3.6. Documentation
*   **Status:** Completed
*   **Description:** Add Doxygen-style comments to the new data structure headers.
    *   [x] **3.6.1.** Add comments to `voxel.h`. *(Completed)*
    *   [x] **3.6.2.** Add comments to `chunk_segment.h`. *(Completed)*
    *   [x] **3.6.3.** Add comments to `chunk_column.h`. *(Completed)*
    *   [x] **3.6.4.** Add comments to `world_manager.h`. *(Completed)*
    *   [x] **3.6.5.** Add comments to `voxel_types.h`. *(Completed)*

### 3.7. Review and Refactor (Optional)
*   **Status:** Not Started
*   **Description:** Review the implemented voxel data structures for any immediate refactoring opportunities or improvements.
    *   [ ] **3.7.1.** Check for code clarity, consistency, and potential optimizations.
    *   [ ] **3.7.2.** Ensure naming conventions are followed.

### 3.8. Add Spatial Partitioning (e.g., Octrees)

*   **Status:** In Progress
*   **Description:** Implement a spatial partitioning system to efficiently manage and query `ChunkColumn`s, especially for very large worlds. This will likely involve a structure within `WorldManager`.

#### 3.8.1. Research and Recommendation
After reviewing common spatial partitioning structures:
  - **Grid:** Simple but not memory efficient for sparse worlds.
  - **Quadtree:** 2D tree for XZ plane, ideal for vertical chunk columns (as in Minecraft-style worlds).
  - **Octree:** 3D tree, best if you need to partition in all three axes (X, Y, Z), but more complex and often unnecessary for columnar voxel worlds.

**Decision:** For Voxel Castle, where `ChunkColumn`s are defined by XZ and extend vertically, a **Quadtree** is the best fit. It allows fast queries for all columns in a region, is memory efficient for sparse worlds, and is simpler to implement and maintain than a full Octree.

#### 3.8.2. Implementation Plan
  - [x] **3.8.2.1.** Design Quadtree API:
      - `insert(x, z, ChunkColumn*)`
      - `remove(x, z)`
      - `find(x, z)`
      - `queryRegion(xMin, zMin, xMax, zMax)` (returns all columns in a region)
  - [x] **3.8.2.2.** Implement Quadtree in `engine/include/world/quadtree.h` and `engine/src/world/quadtree.cpp`.
  - [x] **3.8.2.3.** Integrate Quadtree with `WorldManager` for chunk management (replace or supplement current `std::map`).
  - [x] **3.8.2.4.** Add tests for insertion, removal, and region queries.
  - [ ] **3.8.2.5.** Document and benchmark region queries vs. the old map-based approach.

---
**Next:** Design and implement the Quadtree data structure and update WorldManager integration.

## 4. Considerations
*   **Memory Layout:** Optimize for cache coherency. A flat array for voxels within a `Chunk Segment` is generally good.
*   **`Chunk Segment` Size:** Balance between too many small segments (overhead) and too few large segments (granularity issues for loading/meshing). `32x32x32` is chosen.
*   **Coordinate Systems:** Clearly define local (`Chunk Segment`), `Chunk Column` relative, and world coordinates, and how conversions are handled.
*   **Extensibility:** Design with future needs in mind (e.g., storing more data per voxel, different segment states).
*   **Performance:** Voxel access (get/set) should be as fast as possible.

## 5. Decisions to be Made
*   Exact data to be stored per voxel initially (beyond `type_id`).
*   `Chunk Column` vertical extent (dynamic or fixed range?).
*   Initial set of voxel types.

---
**Last Updated:** 2025-05-13
