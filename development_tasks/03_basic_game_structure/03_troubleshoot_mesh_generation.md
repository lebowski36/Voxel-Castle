# Troubleshooting: Missing Vertical Faces in Outer Chunks (Voxel Engine)

**Date Started:** 2025-05-14

---

## Overview

This document tracks the troubleshooting and analysis process for the issue of missing vertical faces (+X, -X, +Z, -Z) in the mesh generation of outer chunks in the voxel engine. The center chunk renders correctly, but outer chunks have missing vertical faces, both at chunk boundaries and within chunk interiors. Top and bottom faces are always rendered correctly.

The goal is to identify the root cause (suspected to be in the greedy meshing logic, neighbor checks, or mask indexing) and document the troubleshooting process separately from the main world generation task documentation.

---

## Problem Summary

**Symptoms:**
    - Only some vertical faces (+X, -X, +Z, -Z) are missing in outer chunks.
    - The center chunk renders all faces correctly.
    - The issue is not limited to chunk/world borders; it also occurs for voxels inside the chunk boundaries of outer chunks.
    - Top and bottom faces are always present, so Y face logic is likely correct.
    - **Key new observation:** Whenever a vertical face is missing, there is always another face parallel to it (on the same axis and height), possibly many blocks away, and never a missing face unless there is a parallel face behind it. This suggests the greedy meshing mask or expansion logic is incorrectly merging or marking faces across air blocks, not just adjacent solids.
**Suspected Areas:**
    - Greedy meshing logic (mask usage, especially mask marking and expansion logic)
    - Mask indexing (incorrect marking or usage)
    - Neighbor voxel checks (local vs. world coordinates)

---

## Findings and Hypotheses

- The pattern of missing faces always having a parallel face behind theMask Indexing: If the mask is marked for a larger area than intended, or if the mask index calculation is off, it could cause faces to be skipped in the interior of a chunk.
Neighbor Check: If the neighbor check uses local coordinates but should use world coordinates (or vice versa), it may incorrectly think a neighbor is solid/air.
Off-by-One in Loops: If the greedy expansion or main loop is off by one, it could skip faces at the start or end of a strip.
m (even many blocks away) strongly suggests a mask marking or greedy expansion bug: the mask is being set for a region that includes both the visible face and the parallel face, even if there is air in between. This causes the second face to be skipped, as the mask is already set from the first, distant face.
- The mask or expansion logic is likely not properly constrained to contiguous, adjacent, same-type, visible faces, and may be skipping over air blocks when merging faces or marking the mask.
- This is not a neighbor check or chunk boundary issue, but a classic greedy meshing mask overlap/expansion bug.

---

---

## Step-by-Step Analysis Plan

### 1. Neighbor Check Logic
- **Goal:** Ensure that the neighbor check for face visibility is correct.
    - Confirm whether the neighbor check uses local or world coordinates as appropriate.
    - Check if the logic correctly determines when a face should be rendered (i.e., when the neighbor is air/empty).
    - Pay special attention to chunk boundaries: is the neighbor in another chunk/segment handled correctly?
- **Actions:**
    - Review the code where neighbor voxels are checked for face visibility.
    - Verify coordinate system used (local vs. world).
    - Check for any early returns or logic that could skip faces incorrectly.

### 2. Off-by-One Errors in Loops
- **Goal:** Ensure that the greedy meshing loops and main iteration do not skip the first or last voxels in any direction.
    - Check the main iteration bounds for each axis.
    - Check the greedy expansion logic for off-by-one errors (e.g., does it expand one voxel too far or not far enough?).
    - Confirm that all relevant voxels are considered for face generation.
- **Actions:**
    - Review all for-loops and while-loops in the greedy meshing code.
    - Pay attention to loop start/end conditions and increments.
    - Check if any faces at the start or end of a strip are skipped.

### 3. Mask Indexing Logic
- **Goal:** Ensure that the mask used for greedy meshing is marked and indexed correctly.
    - Check if the mask is being marked for a larger area than intended, or if the mask index calculation is off.
    - Confirm that the mask is reset and used correctly for each slice and direction.
    - Incorrect mask usage could cause faces to be skipped in the interior of a chunk.
- **Actions:**
    - Review mask initialization and reset logic.
    - Check how mask indices are calculated and used during face generation.
    - Look for any mismatches between mask size and the area being meshed.

---

## Next Steps

1. **Neighbor Check Logic:**
    - Confirmed that `ChunkSegment::getVoxel` returns AIR for out-of-bounds, so neighbor checks at chunk boundaries are correct for single-chunk meshing. No issues found here.
2. **Off-by-One Errors in Loops:**
    - **Next step:** Review all for-loops and greedy expansion logic for off-by-one errors, especially in the greedy expansion and mask marking. Focus on whether the expansion or mask marking is skipping over air or merging non-contiguous faces.
3. **Mask Indexing:**
    - To be reviewed after loop/expansion logic if needed.

---

**Next action:** Begin detailed review of greedy expansion and mask marking logic for off-by-one errors and improper merging across air blocks.

---

## References
- Main world generation task: `/development_tasks/03_basic_game_structure/03_add_simple_world_generation.md`
- Greedy meshing logic: `engine/src/rendering/mesh_builder.cpp`

---

**This file will be updated as the troubleshooting proceeds.**
