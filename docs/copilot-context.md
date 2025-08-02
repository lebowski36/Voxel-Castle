# Copilot Context File

## Current Development Phase
Core Engine Development - Building foundational systems for colony simulation

## Active Systems
- Voxel chunk management (32³ chunks)
- Greedy meshing algorithm
- OpenGL rendering pipeline
- Flecs ECS integration
- Basic world generation
- Block placement/removal system

## Key Files & Responsibilities
- `src/engine/VoxelChunk.hpp` - 32x32x32 voxel chunk management
- `src/engine/MeshGenerator.hpp` - Greedy meshing implementation
- `src/engine/WorldManager.hpp` - Chunk loading/unloading
- `src/rendering/` - OpenGL rendering systems
- `data/blocks/` - JSON block definitions
- `scripts/generators/` - Code generation tools

## Never Change These Values
- CHUNK_SIZE = 32 (in any dimension)
- VOXEL_SIZE = 0.25f meters
- TARGET_FPS = 60
- OpenGL version minimum 3.3 core
