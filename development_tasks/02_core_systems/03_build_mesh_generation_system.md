# Task: Build Mesh Generation System

**High-Level Goal:** Implement a system to convert voxel data from `ChunkSegment`s into renderable triangle meshes.

**Parent Task:** [2. Implement Core Systems (C++)](/home/system-x1/Projects/Voxel Castle/CURRENT_TODO.md#2-implement-core-systems-c)

**Design Document Reference:**
*   Consult `/home/system-x1/Projects/Voxel Castle/docs/design_specifications/Voxel Fortress Tech Stack (C++ Custom Engine Approach).md` for any relevant architectural guidelines.

---

## Sub-Tasks:

### 1. Basic Mesh Generation from Voxel Data
    - [x] 1.1. Define `VoxelMesh` (or similar) data structure to hold vertex attributes (position, normal, UVs) and indices.
    - [x] 1.2. Implement a function/method within `ChunkSegment` (or a dedicated `MeshBuilder` class) that iterates through its voxels.
    - [x] 1.3. For each solid voxel, generate faces (quads, then triangulate) for exposed sides (sides not adjacent to another solid voxel).
        - [x] 1.3.1. Start with a simple "naive" or "blocky" meshing approach where each voxel face is distinct.
        - [x] 1.3.2. Ensure correct vertex winding order for faces (e.g., counter-clockwise for front-facing).
        - [x] 1.3.3. Calculate basic face normals.
    - [x] 1.4. Store the generated mesh data.
    - [x] 1.5. **Testing:**
        - [x] 1.5.1. Create a test `ChunkSegment` with a simple pattern of voxels.
        - [x] 1.5.2. Generate the mesh.
        - [x] 1.5.3. (Placeholder for rendering) Log the number of vertices/triangles generated or visualize if a simple rendering path is available. For now, focus on data correctness.

### 2. Mesh Optimization (e.g., Greedy Meshing)
    - [ ] 2.1. Research and select a suitable mesh optimization algorithm (Greedy Meshing is a common choice for voxel engines).
        - [ ] 2.1.1. Understand the principles of Greedy Meshing: merging adjacent coplanar faces of the same voxel type.
    - [ ] 2.2. Implement the chosen optimization algorithm.
        - [ ] 2.2.1. Iterate through slices (e.g., XY, XZ, YZ planes) of the chunk.
        - [ ] 2.2.2. Identify contiguous runs of same-type voxels to form larger quads.
    - [ ] 2.3. Update the mesh generation process to use the optimization.
    - [ ] 2.4. **Testing:**
        - [ ] 2.4.1. Use the same or a more complex test `ChunkSegment`.
        - [ ] 2.4.2. Compare the vertex/triangle count before and after optimization.
        - [ ] 2.4.3. (Placeholder for rendering) Verify visually that the optimized mesh is correct.

### 3. Texture Mapping Support
    - [ ] 3.1. Define how voxel types map to texture coordinates (e.g., which part of a texture atlas corresponds to a stone voxel, grass voxel, etc.).
    - [ ] 3.2. Modify the mesh generation to include UV coordinates for each vertex.
        - [ ] 3.2.1. For naive meshing: Assign UVs based on the voxel face.
        - [ ] 3.2.2. For optimized meshing: Ensure UVs correctly map to the larger merged quads.
    - [ ] 3.3. **Testing:**
        - [ ] 3.3.1. (Placeholder for rendering) Verify UVs by applying a test texture/atlas.

### 4. Texture Atlas System
    - [ ] 4.1. Design a system for managing texture atlases.
        - [ ] 4.1.1. Define how individual voxel textures are packed into a larger atlas image.
        - [ ] 4.1.2. Create a mechanism to look up UV coordinates within the atlas for a given voxel type and face.
    - [ ] 4.2. Implement loading and management of the texture atlas.
    - [ ] 4.3. Integrate the texture atlas lookup into the UV generation step (Task 3.2).
    - [ ] 4.4. **Testing:**
        - [ ] 4.4.1. (Placeholder for rendering) Test with a sample texture atlas and verify correct texture application on generated meshes.

### 5. Integration and Refinement
    - [ ] 5.1. Integrate the mesh generation system with the `WorldManager` or `ChunkColumn` to trigger mesh updates when chunks change.
    - [ ] 5.2. Consider asynchronous mesh generation to avoid stalling the main thread.
    - [ ] 5.3. Profile and optimize mesh generation performance.
    - [ ] 5.4. Add Doxygen-style comments to new classes and functions.
    - [ ] 5.5. Update relevant design documents.

---
**Notes & Considerations:**
*   Start with the simplest approach that works (naive meshing) and iterate.
*   Rendering is not part of this task, but the generated mesh data should be suitable for a typical rendering pipeline (e.g., OpenGL).
*   Keep performance in mind, especially for large numbers of chunks.
*   Consider how different voxel types will affect meshing (e.g., transparent vs. opaque). This might be a later refinement.
*   Edge cases: chunk boundaries, transitions between different voxel types.

---
**Status:** Pending
