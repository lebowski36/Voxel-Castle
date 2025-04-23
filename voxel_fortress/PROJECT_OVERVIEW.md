# Voxel Fortress Project Overview

## Project Description
Voxel Fortress is a colony simulation game combining Dwarf Fortress-like depth with voxel-based creative freedom. The game features procedurally generated worlds, colony management, and emergent storytelling through complex simulation systems.

## Technical Stack
- **Language**: Rust
- **Game Engine**: Bevy (0.15.3)
- **Key Libraries**: noise (0.8) for procedural generation

## Core Architecture

### Current Structure
- **main.rs**: App initialization, camera controls, setup functions
- **chunk.rs**: Chunk management and LOD system
- **systems.rs**: UI, materials, and chunk entity management
- **terrain.rs**: Voxel and terrain generation
- **mesh_gen.rs**: (Currently empty/unused)
- **renderer.rs**: (Currently empty/unused)

### Key Issues
1. Resource initialization issue causing panic in `update_chunk_entities_system`
2. Overly cluttered main.rs with mixed responsibilities
3. Several unused/dead code elements
4. Lack of clear modular organization

### Refactoring Plan
1. Create a more modular architecture with Bevy plugins
2. Move camera and input handling to dedicated modules
3. Separate core systems into logical domains
4. Fix resource initialization and dependency issues
5. Implement a cleaner main entry point with organized plugin registration

## Key Game Features
- Voxel-based terrain with LOD (Level of Detail) rendering
- Chunk-based world management and streaming
- Camera controls and navigation
- Procedural world generation

## Priorities
1. Fix existing bugs and ensure the game runs properly
2. Implement a clean modular architecture
3. Separate concerns into appropriate modules
4. Establish maintainable code organization for future development
