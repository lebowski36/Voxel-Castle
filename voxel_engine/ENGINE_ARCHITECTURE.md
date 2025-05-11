# Voxel Engine Architecture

## Overview

The Voxel Engine is a custom-built game engine designed specifically for voxel-based worlds with a focus on performance and scalability. This document outlines the architecture of the engine and how the different components interact.

## Core Components

### Engine Structure

```
voxel_engine/
├── core/          # Core engine functionality
│   ├── mod.rs     # Main Engine struct and loop
│   └── window.rs  # Window management with winit
├── graphics/      # Rendering systems
│   ├── mod.rs     # Graphics module exports
│   └── renderer.rs # wgpu rendering implementation
├── ecs/           # Entity Component System
├── voxel/         # Voxel data structures and operations
└── utils/         # Utility functions and helpers
```

### Component Relationships

The engine follows a layered architecture:

1. **Core Layer** - Provides the foundation for the engine
   - Engine: Coordinates all systems and runs the main loop
   - Window: Handles window creation and event processing

2. **Graphics Layer** - Manages rendering
   - Renderer: Initializes wgpu and handles the rendering pipeline
   - (Future) Camera: Manages view and projection matrices
   - (Future) Shaders: Handles shader compilation and management

3. **Voxel Layer** - Manages voxel data
   - (Future) Chunk: Stores voxel data in fixed-size chunks
   - (Future) World: Manages multiple chunks and coordinates loading/unloading

4. **ECS Layer** - Handles game logic
   - (Future) Will implement entity-component-system pattern

## Data Flow

1. Engine initialization:
   - Window is created
   - Renderer is initialized with the window
   - Other systems are set up

2. Main loop:
   - Window events are processed (input, resize, close, etc.)
   - Game state is updated based on elapsed time
   - Renderer draws the current frame
   - Loop repeats until termination signal

3. Rendering process:
   - Get current texture from surface
   - Create command encoder
   - Begin render pass with clear color
   - Execute draw calls (will be expanded later)
   - Submit commands to queue
   - Present frame to display

## Design Decisions

### wgpu over direct graphics APIs

We chose wgpu because it provides a modern, safe abstraction over various graphics APIs (Vulkan, Metal, DirectX, WebGPU) while still allowing high performance.

### Engine vs ECS management

The Engine struct provides high-level coordination, while the ECS (to be implemented) will handle game-specific logic and data. This separation allows us to maintain a clear distinction between engine systems and game systems.

### Asynchronous rendering initialization

Graphics device initialization in wgpu is asynchronous, so we use pollster to block on this initialization during engine startup. This simplifies our API while still leveraging the async capabilities of wgpu.

## Future Extensions

- Shader system for managing WGSL shaders
- Mesh generation system for voxel data
- Texture atlas for efficient texture mapping
- Camera controls for navigation
- Frustum culling for performance optimization
- Multi-threaded chunk loading
- Physics integration

## Performance Considerations

- Chunk data will be structured for efficient access patterns
- LOD system will be implemented for distance-based detail reduction
- Occlusion culling will reduce unnecessary rendering
- Greedy meshing will optimize triangle count
