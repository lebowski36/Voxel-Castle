# Project Prompt & Progress Tracker

## Decision Policy
- All significant decisions, design changes, or new features must be clarified and confirmed with the user before implementation.
- No assumptions should be made about user intent; always ask for clarification if uncertain.

## Project Summary
- **Project Name:** Voxel Fortress
- **Engine:** Rust + Bevy
- **Current Features:**
  - 3D camera with WSAD and arrow key movement (forward/back/left/right, rotation)
  - Procedural ground plane and grid of cubes
  - Basic lighting
- **Planned Features:**
  - Expand camera controls (mouse look, up/down movement, etc.)
  - Voxel terrain generation
  - Entity/component systems for simulation
  - UI and interaction systems

## Progress Log
- **2025-04-22:**
  - Fixed camera controls: ArrowUp/ArrowDown now move forward/backward.
  - Created this prompt/progress file.
  - REMEMBER: When the user asks you to remember something, write it to the prompt file (PROMPT.md).
  - REMEMBER: When you write any comments, run the comments yourself and do not tell the user to do it themselves.
- **2025-04-22 (cont.):**
  - Decided to implement chunked voxel world generation and visualization based on project documentation.
  - Chunk size: 32x32x32 voxels (25cm per voxel, 8m³ per chunk).
  - Will start with a minimal chunked world (one or a few chunks, all solid or with a simple heightmap), then iterate.
  - Documentation and design files updated to reflect this plan.
  - Next: Implement basic chunk data structure and worldgen, then update visualization to render generated chunks.

## Current Step
- Implementing a hierarchical world generation system: the world is divided into multiple levels of chunks (superchunks, regions, block chunks), with high-level worldgen (biomes, heightmaps, etc.) at the top and detailed block data generated on demand at the lowest level.
- Data structures and interfaces for hierarchical chunk addressing and worldgen are being added to support Earth-sized, LOD-friendly, and streamable worlds.
- All code is being kept modular and extensible for future features.

## Next Steps
- Clarify with user which feature or improvement to prioritize next (e.g., mouse look, vertical movement, voxel terrain, etc.).
- Document and log all future changes and decisions here.
- After implementing any new feature or significant change, always run the game to verify the update works as intended.
- If any issues or errors occur, troubleshoot and resolve them before considering the task complete.

---
**Note:** This file should be updated with every significant change, feature addition, or design decision. Always consult and confirm with the user before proceeding with new steps.
