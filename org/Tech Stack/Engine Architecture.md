# Engine Architecture

## Overview
The Voxel Fortress engine is designed from the ground up to handle our specific requirements: vast procedurally generated voxel worlds, complex simulation systems with hundreds of entities, and extreme view distances through advanced LOD techniques. This document outlines our engine architecture, detailing how different systems interact and the design principles guiding our implementation choices.

> **Related Documents:**
> - [Entity Component System](./Entity%20Component%20System.md)
> - [Rendering Pipeline](./Rendering%20Pipeline.md)
> - [Voxel Data Structures](./Voxel%20Data%20Structures.md)
> - [Technical Roadmap](./Technical%20Roadmap.md)

## Architectural Principles

### Data-Oriented Design
Rather than organizing code around objects and inheritance hierarchies, our engine embraces data-oriented design principles:
- Organizing data for optimal memory access patterns
- Prioritizing cache coherency through sensible memory layouts
- Separating data from operations that act upon it
- Designing for bulk operations on similar data
- Minimizing indirection and pointer-chasing

### Parallelism by Default
With modern hardware offering increasing core counts, our architecture assumes parallelism:
- Task-based job system for distributing work across available cores
- Lock-free algorithms where possible to minimize synchronization overhead
- Clear data ownership rules to prevent race conditions
- Work stealing for better load balancing across threads
- Pipeline design allowing different stages to execute concurrently

### Modularity with Clear Boundaries
While maintaining high performance, the engine is designed with modularity in mind:
- Well-defined interfaces between major systems
- Explicit dependencies to prevent unexpected coupling
- Compile-time configuration for different feature sets
- Plugin architecture for extensibility
- Comprehensive error handling at system boundaries

### Optimization-Friendly
The architecture anticipates the need for ongoing performance tuning:
- Instrumentation points throughout the engine for performance tracking
- Clear performance budgets for different subsystems
- Configurable detail levels for different hardware capabilities
- Hot-reloading support for faster iteration during development
- Profiling infrastructure built into the core systems

## Core Systems Architecture

### Central Engine Coordinator
At the highest level, the engine is orchestrated by a central coordinator that:
- Manages the main loop timing and synchronization
- Controls initialization and shutdown sequences
- Handles high-level error recovery
- Distributes frame time budgets across systems
- Maintains global engine state

### Layered Design
The engine follows a layered architecture with clear responsibilities:

1. **Foundation Layer**
   - Memory management
   - Thread pool and job system
   - Logging and diagnostics
   - Platform abstraction
   - Core utilities and math

2. **Systems Layer**
   - Entity Component System
   - Rendering system
   - Physics engine
   - Input management
   - Audio system

3. **Game Layer**
   - Voxel world management
   - Simulation systems
   - AI and pathfinding
   - Game logic and rules
   - User interface

4. **Application Layer**
   - Game states and flows
   - Asset management
   - Save/load systems
   - User settings
   - Platform-specific integration

### Cross-Cutting Concerns
Certain aspects span multiple layers:
- Performance monitoring and profiling
- Memory tracking and optimization
- Error handling and logging
- Configuration management
- Development tools integration

## System Interactions

### Main Loop Architecture
The engine uses a variable-rate update loop with fixed-rate simulation:

1. **Input Phase**
   - Poll for user input
   - Update input mapping states
   - Process platform events

2. **Update Phase**
   - Fixed timestep simulation updates (potentially multiple per frame)
   - Physics simulation
   - AI and entity behavior
   - World systems updates

3. **Render Phase**
   - Visibility determination
   - Scene graph updates
   - LOD system processing
   - Render command generation
   - Graphics API interactions

4. **Sync Phase**
   - Present rendered frame
   - Audio synchronization
   - Frame timing calculations
   - Performance metrics collection

### Data Flow Architecture
Data flows through the engine in well-defined paths:

1. **Input → Logic**
   - User inputs are mapped to game actions
   - Command pattern for reproducible actions
   - Input recording for replay and debugging

2. **Logic → Simulation State**
   - Game rules modify entity components
   - World state changes propagate through dependencies
   - Events trigger cascading effects

3. **Simulation State → Rendering**
   - Transformed into optimized visual representation
   - Multiple detail levels generated based on distance
   - Cached and reused when unchanged

4. **Simulation State → Persistence**
   - Serialization for saving/loading
   - Change tracking for efficient saves
   - Migration paths for version differences

## Memory Management

### Allocation Strategies
Different subsystems use appropriate allocation patterns:

1. **Arena Allocators**
   - For simulation data with similar lifetimes
   - Bulk creation/destruction of entities
   - Chunk data for world regions

2. **Pool Allocators**
   - Fixed-size, frequently allocated objects
   - Particle systems and effects
   - Temporary objects during rendering

3. **TLSF Allocators**
   - Dynamic allocations requiring fast response
   - Unpredictable sizes with real-time needs
   - Asset loading and management

4. **Custom Region-Based Management**
   - Voxel data organization
   - Spatial data structures
   - Streaming world data

### Memory Hierarchy Awareness
The engine design considers the realities of modern memory architecture:
- Cache-friendly data layouts
- Alignment for SIMD processing
- Prefetching hints where beneficial
- Separation of hot/cold data
- Thread-local storage for performance-critical paths

## Multithreading Architecture

### Job System
The engine uses a sophisticated job system to distribute work:
- Job graph creation with explicit dependencies
- Priority-based scheduling for critical path tasks
- Work stealing for load balancing
- Continuation-style job chaining
- Task batching for minimal overhead

### Thread Management
Threads are organized for optimal hardware utilization:
- Thread pool sizing based on available cores
- Core affinity control for performance-critical threads
- Background threads for non-time-critical work
- Main thread protection for platform API requirements
- Thread priority management

### Subsystem Threading Models

1. **Simulation Threading**
   - Entity updates parallelized by archetype
   - Spatial partitioning for independent updates
   - Multi-phase update for handling dependencies

2. **Physics Threading**
   - Parallel broadphase collision detection
   - Island-based solver parallelism
   - Spatial subdivision for independent simulation

3. **Rendering Threading**
   - Culling and LOD calculations in parallel
   - Mesh generation distributed across worker threads
   - Command buffer building in parallel
   - Background shader compilation

4. **IO Threading**
   - Asynchronous asset loading
   - Background chunk generation
   - Non-blocking save operations
   - Streaming data management

## System Details

### Entity Component System
The ECS forms the backbone of our simulation architecture:
- Component-focused data layout for cache optimization
- Systems operating on component streams
- Entity archetype organization
- Query-based component access
- Event propagation between systems

See: [Entity Component System](./Entity%20Component%20System.md) for implementation details

### Rendering System
Our rendering pipeline is specifically optimized for voxel worlds:
- Multi-level LOD system for extreme draw distances
- Specialized meshing algorithms
- Material system designed for voxel environments
- Advanced lighting with global illumination approximation
- Post-processing pipeline for visual enhancements

See: [Rendering Pipeline](./Rendering%20Pipeline.md) for implementation details

### Voxel World Management
The voxel system manages the massive world data:
- Chunk-based storage with efficient compression
- Multi-resolution representation
- Procedural generation pipeline
- Streaming system for dynamic loading
- Change tracking for efficient updates

See: [Voxel Data Structures](./Voxel%20Data%20Structures.md) for implementation details

### Physics System
The physics engine handles complex voxel-based interactions:
- Specialized collision detection for voxels
- Rigid body dynamics
- Fluid simulation
- Structural integrity calculations
- Physics proxies for performance at distance

See: [Physics System](./Physics%20System.md) for implementation details

### AI and Pathfinding
Our AI architecture balances depth with performance:
- Behavior tree implementation
- Hierarchical pathfinding in voxel environments
- Spatial awareness systems
- Need-based decision making
- Group coordination for entities

See: [AI Architecture](./AI%20Architecture.md) for implementation details

## Platform Abstraction

### Graphics Abstraction
The engine uses wgpu to provide cross-platform rendering:
- Vulkan, DirectX, Metal, and WebGPU support
- Feature detection and fallbacks
- Performance profiling integration
- Pipeline state caching
- Shader cross-compilation

### Input Abstraction
Input handling is normalized across platforms:
- Keyboard, mouse, controller support
- Input mapping system
- Customizable controls
- Input recording and playback
- Accessibility considerations

### File System Abstraction
All file operations go through a platform abstraction layer:
- Asynchronous file operations
- Platform-specific paths and conventions
- Asset package management
- Mod support infrastructure
- Save data handling

### Window and Display Management
Platform-specific windowing is abstracted:
- Resolution and display management
- Fullscreen and windowing options
- Monitor detection and setup
- DPI awareness and scaling
- Platform UI integration where needed

## Development Tools Integration

### Profiling Infrastructure
The engine includes comprehensive performance analysis tools:
- Frame timing breakdown
- CPU/GPU utilization tracking
- Memory allocation visualization
- System-specific metrics
- Integration with Tracy profiler

### Debug Visualization
Visual debugging tools assist development:
- Entity component visualization
- Physics debugging overlays
- Pathfinding visualization
- Memory usage heat maps
- Performance bottleneck highlighting

### Hot Reloading
Development velocity is improved through hot reloading:
- Shader recompilation without restart
- Asset updates in real-time
- Configuration changes on the fly
- Script reloading where applicable
- State preservation during reload

### Testing Framework
Automated testing is built into the engine:
- Unit testing framework
- Component system tests
- Performance benchmarking
- Regression testing
- Automated stress testing

## Implementation Progress

### Current Status
As outlined in our [Technical Roadmap](./Technical%20Roadmap.md), we are currently focused on:
- Core foundation layer implementation
- Basic ECS architecture
- Simple voxel rendering
- Initial world representation
- Fundamental physics integration

### Next Steps
Immediate development priorities include:
- Expanding the job system for better parallelism
- Implementing the initial LOD system
- Building the basic AI framework
- Developing world generation systems
- Creating the serialization framework

## Conclusion

The Voxel Fortress engine architecture represents a carefully considered balance between performance, flexibility, and development pragmatism. By building a custom engine specifically designed for our needs, we can deliver the ambitious vision of vast voxel worlds with deep simulation systems and impressive visual fidelity.

This architecture document serves as the blueprint for our implementation work, establishing clear patterns and expectations for how systems will interact and evolve over time.