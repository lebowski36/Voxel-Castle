# Voxel Fortress Roadmap

## Current State of the App

### Vertical Slice Focus
- **Colony Simulation**:
  - A small procedurally generated world.
  - Basic colonist AI with task assignment (e.g., gathering resources, building structures).
  - A simple resource management system.

### Feature Prioritization
- **World Generation**:
  - Build the foundation for the game world.
  - Focus on terrain generation and rendering.
  - **Progress**: Basic terrain generation implemented using Perlin noise.
- **Resource Management**:
  - Create a system for gathering, storing, and using resources.

### Feedback Incorporation
- Feedback will primarily come from internal testing and evaluation by the project owner.

## Completed Tasks
- Defined the development roadmap and vertical slice focus.
- Established the engine architecture principles (data-oriented design, LOD techniques).
- Prioritized initial features for implementation.
- Implemented basic terrain generation using Perlin noise.

## Planned Next Steps
1. **World Generation**:
   - Optimize rendering for small-scale worlds.
2. **Colony Simulation**:
   - Develop basic colonist AI for task execution.
   - Create a simple resource management system.
3. **Feedback Process**:
   - Set up a feedback tracker to log and prioritize issues or suggestions.
   - Conduct regular playtesting sessions to evaluate progress.

## Long-Term Goals
- Expand the scope of the colony simulation mechanics.
- Introduce threats and challenges to enhance gameplay depth.
- Optimize performance for large-scale voxel worlds.
- Develop advanced AI systems for colonists and enemies.

## Development Guidelines

### Decision-Making Process
- All major decisions during development must involve consulting the project owner to ensure alignment with the vision and priorities.

### Prototype Goal
- The immediate goal is to create a functional prototype with the following features:
  - Basic terrain generation and rendering.
  - Simple colonist AI for task execution.
  - A basic resource management system.

### Transition to 3D Visualization
- Replace ASCII visualization with a 3D rendering system.
- Integrate a rendering library (e.g., `wgpu` or `bevy`) to render the voxel world in 3D.
- Implement basic camera controls and lighting for the 3D environment.

### Prototype Goal Update
- The prototype will now include a 3D visualization of the terrain as part of the functional prototype.

### Development Agent Guidelines
- The development agent should execute all commands and tasks autonomously, including installing dependencies and running the project, without requiring manual intervention from the project owner.

---
This roadmap will be updated regularly to reflect progress and changes in priorities.
