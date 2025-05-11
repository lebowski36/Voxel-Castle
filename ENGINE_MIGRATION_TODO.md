# Voxel Fortress TODO: Bevy to Custom Engine Migration

This checklist tracks our progress as we transition from Bevy to a more custom engine approach. **Important: Update this checklist regularly as tasks are completed to maintain clear progress tracking.**

## Phase 1: Foundation Setup

- [ ] **Research and Selection**
  - [ ] Evaluate ECS alternatives (Legion, hecs, shipyard)
  - [ ] Test wgpu direct implementation 
  - [ ] Choose math library (glam vs nalgebra)
  - [ ] Select serialization approach

- [ ] **Core Project Structure**
  - [ ] Create new project structure for custom engine
  - [ ] Set up build system and dependencies
  - [ ] Establish module organization

- [ ] **Window and Input System**
  - [ ] Implement window creation with winit
  - [ ] Create basic event loop
  - [ ] Set up input handling system

- [ ] **Basic Renderer**
  - [ ] Set up wgpu device and queue
  - [ ] Implement render pipeline creation
  - [ ] Create shader management system
  - [ ] Add basic camera functionality

## Phase 2: Core Systems

- [ ] **ECS Implementation**
  - [ ] Set up entity component system framework
  - [ ] Create system scheduler
  - [ ] Implement resource management

- [ ] **Voxel Data Structures**
  - [ ] Implement chunk storage system
  - [ ] Create voxel type definitions
  - [ ] Set up spatial partitioning

- [ ] **Rendering Pipeline**
  - [ ] Implement mesh generation for chunks
  - [ ] Add texture atlas system
  - [ ] Create basic lighting system
  - [ ] Implement frustum culling

- [ ] **Basic UI Framework**
  - [ ] Design simple immediate-mode UI system
  - [ ] Implement text rendering
  - [ ] Add basic UI containers and layouts
  - [ ] Create widget system

## Phase 3: Game Systems Migration

- [ ] **World Generation**
  - [ ] Port terrain generation algorithms
  - [ ] Implement biome system
  - [ ] Add structure placement

- [ ] **Physics Integration**
  - [ ] Set up Rapier integration
  - [ ] Implement collision detection
  - [ ] Add physics response system

- [ ] **Player Systems**
  - [ ] Implement camera controls
  - [ ] Add player movement
  - [ ] Create interaction systems

- [ ] **Game State Management**
  - [ ] Implement state system
  - [ ] Add transition logic
  - [ ] Create loading screens

## Phase 4: Advanced Features

- [ ] **LOD System**
  - [ ] Implement distance-based LOD
  - [ ] Create mesh simplification
  - [ ] Add LOD transition system

- [ ] **Colony Simulation**
  - [ ] Port AI behavior systems
  - [ ] Implement needs and skills
  - [ ] Add task allocation

- [ ] **Performance Optimization**
  - [ ] Set up profiling tools
  - [ ] Implement multithreading for chunk generation
  - [ ] Add occlusion culling
  - [ ] Optimize memory usage

- [ ] **Save System**
  - [ ] Design save file format
  - [ ] Implement serialization
  - [ ] Add save/load functionality

## Current Focus Items
- [ ] Evaluate ECS alternatives to replace Bevy ECS
- [ ] Set up basic window and rendering with winit and wgpu
- [ ] Create a minimal prototype showing a voxel chunk

## Completed Items
- [x] Document transition plan
- [x] Create initial TODO list

**Reminder: This checklist must be updated with every development step to maintain clear progress tracking. Add new tasks as they become apparent and mark completed items to ensure continuous progress.**
