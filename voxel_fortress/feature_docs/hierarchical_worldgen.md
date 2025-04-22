# Hierarchical World Generation: Design & Implementation Plan

## Overview
This document details the design and implementation plan for a modular, hierarchical world generation system for Voxel Fortress. The goal is to support Earth-sized, realistic, and highly extensible procedural worlds, with efficient streaming, LOD, and simulation.

## Key Concepts
- **Hierarchical Chunks:** The world is divided into multiple levels of chunks (e.g., superchunk → region → chunk → block chunk), with each level refining the data from the level above.
- **High-Level Worldgen:** At the top level, generate global features (continents, mountain ranges, rivers, biomes, climate) using multi-octave noise, domain warping, and climate simulation.
- **Erosion & Rivers:** Apply hydraulic erosion and river pathfinding to the heightmap for realistic valleys and riverbeds.
- **Biome Map:** Generate a climate map (temperature, rainfall) and assign biomes based on climate and altitude, with smooth blending between biomes.
- **Cave System:** Use 3D noise and/or cellular automata for caves, with special rules for underground rivers/lakes.
- **Feature Placement:** Place special features (cliffs, arches, ruins) using additional noise and rules.
- **Chunk Streaming:** Only generate and load block chunks as needed, using high-level data for LOD and distant terrain.
- **Simulation:** Unloaded chunks are simulated at a high level, with fast-forwarding/interpolation when loaded.

## Implementation Steps
1. **Data Structures:**
   - Hierarchical chunk addressing (level, x, y, z)
   - ChunkNode and ChunkSummary for each level
   - WorldGen struct for global worldgen state and parameters
2. **High-Level Worldgen:**
   - Generate and store high-level maps (height, biome, river, cave) for any chunk address
   - Use deterministic algorithms (seeded noise, climate simulation)
3. **Chunk Generation:**
   - Generate block chunks on demand using high-level data from all parent levels
   - Ensure seamless transitions and consistency across chunk boundaries
4. **Erosion & Rivers:**
   - Apply hydraulic erosion to heightmap
   - Carve river paths using flow algorithms
5. **Biome Assignment:**
   - Assign biomes based on climate, altitude, and proximity to water
   - Blend biomes smoothly
6. **Cave & Feature Placement:**
   - Use 3D noise and rules for caves, cliffs, ruins, etc.
7. **Streaming & LOD:**
   - Load/generate only needed chunks around the player
   - Use LOD meshes for distant terrain
8. **Simulation:**
   - Simulate unloaded chunks at a high level
   - Fast-forward/interpolate when loading

## Best Practices & References
- Multi-octave noise, domain warping, and ridged noise for natural terrain
- Hydraulic and thermal erosion for realistic landforms
- River pathfinding and flow maps
- Climate simulation for biomes
- Modular, extensible code for future features
- Inspiration: Minecraft modding (TerraForged, OpenTerrainGenerator), Dwarf Fortress, No Man's Sky, real-world geology

---
This document will be updated as the implementation progresses and new features are added.
