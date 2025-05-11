# Voxel Fortress Roadmap

**For a more detailed, phased development plan, please see the [Detailed Development Roadmap](./org/Development%20Roadmap.md).**

## Current State of the Project (Post C++ Transition)

The project is currently in the initial phase of transitioning to a custom C++ engine. Design documentation is being updated, and foundational C++ engine components are being planned.

### Vertical Slice Focus (C++ Engine)
- **Core Voxel Engine & Rendering**:
  - A small, procedurally generated voxel world rendered in 3D.
  - Basic camera controls for navigation.
- **Basic Colony Simulation Elements (Conceptual)**:
  - Initial design for colonist AI and task management within the C++ engine.
  - Design for a simple resource management system.

### Feature Prioritization (C++ Engine - Phase 1)
- **Core Engine Infrastructure**:
  - Establish C++ project structure and build system.
  - Implement windowing, input, and basic rendering (e.g., using OpenGL, Vulkan).
- **Voxel Systems**:
  - Develop core voxel data structures and chunk management.
  - Implement basic mesh generation for voxel terrain.
  - **Progress**: Conceptual planning and C++ transition underway.

### Feedback Incorporation
- Feedback will primarily come from internal testing and evaluation by the project owner during the C++ engine development.

## Completed Tasks (Conceptual & Transition)
- Defined the high-level development roadmap and initial C++ vertical slice focus.
- Established core engine architecture principles (data-oriented design, LOD techniques) for the C++ engine.
- Prioritized initial features for C++ implementation.
- Began transition of documentation and planning from Rust to C++.

## Planned Next Steps (C++ Engine)
1.  **Core C++ Engine Development**:
    *   Set up C++ project, build system (e.g., CMake), and core libraries.
    *   Implement window creation, input handling, and a basic 3D rendering pipeline (e.g., OpenGL/Vulkan).
    *   Develop foundational voxel data structures and chunk management.
    *   Implement initial voxel terrain generation and rendering.
2.  **Basic Interactivity**:
    *   Implement camera controls for 3D world navigation.
3.  **Early Simulation Design**:
    *   Refine designs for colonist AI, task execution, and resource management for the C++ context.
4.  **Feedback Process**:
    *   Set up a feedback tracker for C++ development milestones.
    *   Conduct regular reviews of C++ engine progress.

## Long-Term Goals (C++ Custom Engine)
- Develop a fully-featured custom C++ voxel engine.
- Expand the scope of the colony simulation mechanics with deep AI and complex interactions.
- Introduce diverse threats, challenges, and emergent narrative elements.
- Optimize performance for large-scale, persistent voxel worlds.
- Implement advanced rendering techniques and visual fidelity.

## Development Guidelines

### Decision-Making Process
- All major decisions during development must involve consulting the project owner to ensure alignment with the vision and priorities.

### Prototype Goal (C++ Engine)
- The immediate goal is to create a functional C++ prototype with the following features:
  - Basic 3D voxel terrain generation and rendering.
  - Camera controls for navigation.
  - Foundational structures for future simulation systems.

### Development Agent Guidelines
- The development agent should execute all commands and tasks autonomously, including managing C++ dependencies (as applicable through build systems like CMake/vcpkg or system package managers) and running the project, without requiring manual intervention from the project owner.

---
This roadmap will be updated regularly to reflect progress and changes in priorities for the C++ development effort.
