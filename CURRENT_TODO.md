# CURRENT_TODO.md - Active Development Tracking

## 🚀 Current Focus: Building Custom Engine Components (No Bevy)

This file tracks our immediate tasks and progress. **IMPORTANT: I must update this file after each development step to maintain accurate progress tracking and keep focus on the most important next actions.**

## 🔄 Active Tasks (April 29, 2025)

### 1. Create Core Engine Foundation
- [x] Set up new project structure
  - [x] Create new crate with appropriate dependencies
  - [x] Set up module structure
  - [x] Configure build system
- [x] Implement window management with winit
  - [x] Create window with proper configuration
  - [x] Set up event loop
  - [x] Implement basic input handling
- [x] Set up rendering with wgpu
  - [x] Initialize device and adapter
  - [x] Create swap chain
  - [x] Set up render pipeline

### 2. Implement Core Systems
- [ ] Choose and implement ECS
  - [ ] Research and select ECS library (Legion, hecs, or shipyard)
  - [ ] Create entity and component definitions
  - [ ] Set up system scheduler
- [ ] Create voxel data structures
  - [ ] Design chunk structure
  - [ ] Implement voxel storage
  - [ ] Add spatial partitioning
- [ ] Build mesh generation system
  - [ ] Create mesh from voxel data
  - [ ] Implement greedy meshing optimization
  - [ ] Add texture mapping

### 3. Create Basic Game Structure
- [ ] Implement camera system
  - [ ] Create perspective camera
  - [ ] Add camera controls
  - [ ] Handle window resizing
- [ ] Set up basic game loop
  - [ ] Implement time management
  - [ ] Create update and render systems
- [ ] Add simple world generation
  - [ ] Implement noise-based terrain
  - [ ] Generate basic chunk structure

## 📝 Today's Priority Tasks
1. Research and select ECS library for our specific needs
2. Design basic voxel data structures for chunk representation
3. Begin implementing camera system for navigating the 3D world

## 📊 Progress Tracking
- 🟢 Decision made to build custom engine without Bevy
- 🟢 Project plan and TODO list created
- 🟢 Project structure set up with appropriate dependencies
- 🟢 Window management implemented with winit
- 🟢 Basic rendering pipeline established with wgpu
- 🟢 Engine architecture documentation created
- 🔴 ECS research in progress
- 🔴 Voxel data structures not started
- 🔴 Camera system not started

## 🧠 Note to Self
**I MUST update this file after each development step!** This ensures we maintain focus on the most important tasks, have clear visibility into progress, and can adapt our plan as new information becomes available. The active task list should always reflect the current state of development.

**DOCUMENTATION IS CRUCIAL!** With every implementation step, I must update relevant documentation to explain design decisions, patterns used, and architectural considerations. This includes:
1. Updating existing documentation files
2. Creating new documentation for new systems
3. Adding code comments for complex algorithms
4. Creating diagrams for system interactions when appropriate

**TEST REGULARLY!** I must compile and run the code after implementing each significant feature or component to ensure it works as expected. Never let untested code accumulate - this helps identify issues early when they're easier to fix. After implementing:
1. A new module or system → Compile and run to test it works
2. Changes to existing functionality → Verify nothing broke
3. Complex algorithms or critical systems → Add specific tests
4. Multiple components → Test their integration together

**MAINTAIN SMALL, FOCUSED FILES!** I must keep individual source files small, focused, and well-organized to facilitate better development. This approach:
1. Makes it easier to comprehend the codebase in smaller chunks
2. Reduces context overload when working on specific features
3. Allows me to pinpoint exactly what code I need to reference
4. Promotes better separation of concerns and modularity
5. Results in more maintainable and testable code

---
Last updated: April 29, 2025
