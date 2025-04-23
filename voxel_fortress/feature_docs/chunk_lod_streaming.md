# Chunk LOD & Streaming System: Design & Implementation Plan

## Overview
This document outlines the design and implementation plan for a basic Level of Detail (LOD) and chunk streaming system in Voxel Fortress. The goal is to efficiently manage memory and performance by loading/generating only high-detail chunks near the player, while representing distant terrain with lower-detail meshes or heightmaps, and unloading chunks that are far away.

## Motivation
- Support vast, Earth-sized worlds with high performance
- Enable extraordinary view distances
- Lay the foundation for future simulation, AI, and gameplay systems

## Current State
- Chunks are generated and rendered at full detail in a fixed radius around the player
- No LOD or chunk streaming is implemented yet
- All loaded chunks are kept in memory and rendered at the same detail level

## Next Steps
1. **Design chunk states and LOD levels**
    - Define chunk states: Active (high detail), Inactive (low detail), Unloaded
    - Define LOD levels: Full-detail mesh, simplified mesh, heightmap, etc.
2. **Implement chunk loading/unloading logic**
    - Load/generate high-detail chunks near the player
    - Unload or deactivate chunks that are far away
3. **Implement basic LOD mesh for distant terrain**
    - Create a simple mesh for distant regions (e.g., one mesh per superchunk or region)
    - Use heightmap or averaged data for distant terrain
4. **Integrate with worldgen and rendering**
    - Ensure LOD meshes use the same worldgen data
    - Make the system modular for future expansion
5. **Test and iterate**
    - Run the game after each change
    - Document and troubleshoot any issues

## Implementation Plan
- Start with a simple two-level LOD: full-detail chunks (close), heightmap mesh (far)
- Add chunk state tracking to ChunkManager
- Add logic to update chunk states based on player position
- Add basic heightmap mesh generation for distant regions
- Update rendering to show correct LOD based on distance
- Document all changes in PROMPT.md and this feature doc

---
This document will be updated as the implementation progresses.
