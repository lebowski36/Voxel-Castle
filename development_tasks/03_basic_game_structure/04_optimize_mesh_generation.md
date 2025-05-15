# Optimize Mesh Generation System

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

- [x] **Review Current Mesh Generation System:**
  - Analyzed the code structure:
    - `ChunkSegment` stores voxels and provides local access; its `getVoxel` returns AIR for out-of-bounds.
    - `ChunkColumn` manages a stack of segments and provides access by world coordinates.
    - `WorldManager` manages all columns and provides `getVoxel(worldX, worldY, worldZ)`, returning AIR for missing/out-of-bounds.
    - Mesh generation (`rebuildMesh`) currently only checks local segment neighbors, so faces at chunk borders may be rendered unnecessarily, and faces at world edges may be missed.
  - Identified that the mesh builder’s voxel accessor must be cross-chunk aware, using world coordinates and `WorldManager::getVoxel` for correct face visibility.
  - Next step: Update `rebuildMesh` to use a cross-chunk-aware accessor via `WorldManager`.

- [ ] **Implement Cross-Chunk Visibility Checks:**
  - Update mesh generation to query neighboring chunks for voxel data when at chunk borders.
  - Only render a face if the neighbor (even in another chunk) is air or out-of-bounds.
  - Add logic to detect if a chunk is at the edge of the rendered world and treat out-of-bounds as air.
  - Continue to test and refine until all edge cases are handled.

- [ ] **Handle World/Chunk Edge Cases:**
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
