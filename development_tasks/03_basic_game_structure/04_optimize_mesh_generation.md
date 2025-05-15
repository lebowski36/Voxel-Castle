# Optimize Mesh Generation System
#
---
## Findings (May 2025)

- The mesh rebuild cap (`maxMeshRebuildsPerCall`) was originally used to prevent the main thread from freezing when many chunk segments are dirty. This is only necessary for main-thread mesh generation. With async/parallel mesh generation, the cap is not needed for responsiveness, but a job queue/thread pool limit is still important for resource management.
- "Segments" refer to 32x32x32 voxel blocks (ChunkSegment). A chunk column is a vertical stack of these segments, each with its own mesh.
- Logging the number of dirty segments per frame is useful for diagnosing performance and correctness issues.
- The correct long-term solution is to move mesh generation off the main thread (asynchronous/parallel mesh generation), which is standard in modern voxel engines.

---
## Ongoing and Upcoming Steps

- Lowered the mesh rebuild cap to 10 for immediate responsiveness.
- Added logging to warn if the number of dirty segments is not decreasing.
- Next: Implement asynchronous (parallel) mesh generation using a thread pool/job queue so that mesh building does not block the main thread.
    - Main thread will only upload finished meshes.
    - Cap will be replaced by a job queue/thread pool limit.
- After async meshing is in place, continue with:
    - Systematic testing for missing/unnecessary faces at chunk borders, overhangs, and world edges.
    - Profiling performance and memory usage.
    - Optimizing data structures for efficient visibility checks if needed.
    - Documenting changes and updating design docs.


## Objective
Optimize the current mesh generation system to reduce the number of unnecessary faces being rendered. This will improve performance and ensure only visible faces are meshed and textured.

## Background
The current system renders all faces of solid blocks that touch chunk borders, regardless of whether the adjacent block is also solid. Additionally, faces on the outermost chunk border of the rendered world are being rendered unnecessarily. These inefficiencies lead to a higher computational load and reduced performance.

## Workflow
This task will follow the collaborative workflow outlined in `CURRENT_TODO.md`:
1. **Task Identification:** Focus on the sub-tasks below, breaking them into smaller steps if needed.
2. **Implementation:** Perform coding and documentation for each sub-task.
3. **Execution & Testing:** Test thoroughly after each significant change.
4. **User Confirmation:** Confirm results and decisions with the user before marking tasks as complete.
5. **Documentation Update:** Update this file and related documents after user confirmation.
6. **Single Task Focus:** Work on one sub-task at a time. After completing a sub-task, return to the user for testing and confirmation before proceeding to the next sub-task or marking any items as complete.

## Subtasks
## Subtasks

- [x] **Review Current Mesh Generation System:**
  - Analyzed the code structure:
    - `ChunkSegment` stores voxels and provides local access; its `getVoxel` returns AIR for out-of-bounds.
    - `ChunkColumn` manages a stack of segments and provides access by world coordinates.
    - `WorldManager` manages all columns and provides `getVoxel(worldX, worldY, worldZ)`, returning AIR for missing/out-of-bounds.
    - Mesh generation (`rebuildMesh`) currently only checks local segment neighbors, so faces at chunk borders may be rendered unnecessarily, and faces at world edges may be missed.
  - Identified that the mesh builder’s voxel accessor must be cross-chunk aware, using world coordinates and `WorldManager::getVoxel` for correct face visibility.
  - Next step: Update `rebuildMesh` to use a cross-chunk-aware accessor via `WorldManager`.

- [x] **Implement Cross-Chunk Visibility Checks:**
  - Update mesh generation to query neighboring chunks for voxel data when at chunk borders.
  - Only render a face if the neighbor (even in another chunk) is air or out-of-bounds.
  - Add logic to detect if a chunk is at the edge of the rendered world and treat out-of-bounds as air.
  - Continue to test and refine until all edge cases are handled.

- [x] **Handle World/Chunk Edge Cases:**
  - Ensure faces at the edge of the rendered world are only rendered if truly exposed.

- [ ] **Implement Asynchronous/Parallel Mesh Generation:**
  - [x] Design thread pool/job queue system for mesh generation jobs.
  - [x] Refactor mesh generation to enqueue jobs instead of running on the main thread.
  - [x] Ensure mesh building runs in worker threads, with only mesh upload on the main thread.
  - [x] Cap the number of concurrent jobs to avoid resource exhaustion.
  - [x] Add synchronization and safe handoff of finished meshes to the main thread.
  - [x] Add logging and debugging for job queue and thread pool activity.
  - [ ] Test for correct and efficient mesh updates under heavy load.

- [ ] **Test and Validate (Expanded):**
  - Systematically test for missing and unnecessary faces at chunk borders, overhangs, and world edges.
  - Profile performance and memory usage.

- [ ] **Optimize Data Structures:**
  - Ensure data structures used for storing chunk and block information allow for efficient visibility checks.

- [ ] **Test and Validate:**
  - Test the updated system to ensure it correctly skips unnecessary faces while maintaining proper rendering for visible faces.
  - Validate performance improvements by measuring frame rates and computational load.

- [ ] **Document Changes:**
  - Update relevant design documents and task files to reflect the new system.
- [x] **Review Current Mesh Generation System:**
  - Analyzed the code structure:
    - `ChunkSegment` stores voxels and provides local access; its `getVoxel` returns AIR for out-of-bounds.
    - `ChunkColumn` manages a stack of segments and provides access by world coordinates.
    - `WorldManager` manages all columns and provides `getVoxel(worldX, worldY, worldZ)`, returning AIR for missing/out-of-bounds.
    - Mesh generation (`rebuildMesh`) currently only checks local segment neighbors, so faces at chunk borders may be rendered unnecessarily, and faces at world edges may be missed.
  - Identified that the mesh builder’s voxel accessor must be cross-chunk aware, using world coordinates and `WorldManager::getVoxel` for correct face visibility.
  - Next step: Update `rebuildMesh` to use a cross-chunk-aware accessor via `WorldManager`.

- [x] **Implement Cross-Chunk Visibility Checks:**
  - Update mesh generation to query neighboring chunks for voxel data when at chunk borders.
  - Only render a face if the neighbor (even in another chunk) is air or out-of-bounds.
  - Add logic to detect if a chunk is at the edge of the rendered world and treat out-of-bounds as air.
  - Continue to test and refine until all edge cases are handled.

- [x] **Handle World/Chunk Edge Cases:**
  - Ensure faces at the edge of the rendered world are only rendered if truly exposed.

- [ ] **Test and Validate (Expanded):**
  - Systematically test for missing and unnecessary faces at chunk borders, overhangs, and world edges.
  - Profile performance and memory usage.

- [ ] **Optimize Data Structures:**
  - Ensure data structures used for storing chunk and block information allow for efficient visibility checks.

- [ ] **Test and Validate:**
  - Test the updated system to ensure it correctly skips unnecessary faces while maintaining proper rendering for visible faces.
  - Validate performance improvements by measuring frame rates and computational load.

- [ ] **Document Changes:**
  - Update relevant design documents and task files to reflect the new system.

## Related Tasks
- Document findings and decisions in this file as the optimization progresses.

## Notes
- This task is critical for improving the performance of the rendering system.
- Ensure thorough testing to avoid introducing visual artifacts or rendering issues.
