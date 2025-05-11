# CURRENT_TODO.md - Active Development Tracking

## 🚀 Current Focus: Building Custom C++ Engine Components

This file tracks our immediate high-level tasks and progress. Detailed sub-tasks for each item are located in corresponding files within the `/home/system-x1/Projects/Voxel Castle/development_tasks/` directory.

**IMPORTANT: This file is updated after each major development step is confirmed as complete by the user. The agent works from detailed sub-task files.**

---

## 🤖 Agent Workflow & Prompting Guidelines

**Core Principle:** The AI agent (GitHub Copilot) will assist in development by breaking down tasks, generating code/documentation, and executing commands. The user provides oversight, makes key decisions, and confirms completion of steps.

**Workflow:**

1.  **High-Level Tasks:** `CURRENT_TODO.md` lists the major development phases and tasks.
2.  **Detailed Sub-Tasks:** Each high-level task in this file will have a corresponding detailed Markdown file in the `development_tasks/` directory (e.g., `development_tasks/01_core_engine_foundation/01_set_up_project_structure.md`).
    *   If a detailed sub-task file does not exist when a high-level task is started, the agent will create it, outlining the specific steps, considerations, and self-prompting needed to complete that item.
3.  **Execution Focus:** The agent will primarily work from the detailed sub-task files.
4.  **User Confirmation:**
    *   The agent will confirm individual significant actions or outputs from the sub-task file with the user.
    *   Once all sub-steps within a detailed task file are completed and confirmed, the agent will propose marking the corresponding high-level task in *this* `CURRENT_TODO.md` as complete.
    *   The user must confirm before the high-level task is marked `[x]`.
5.  **File Modifications:** The agent will use the `insert_edit_into_file` tool for changes, providing clear explanations.
6.  **Command Execution:** The agent will use `run_in_terminal` for shell commands, with explanations.

**Bug Fixing & Iteration (Incorporating `MUST-READ-Prompt-Fixing-Bugs.txt` principles):**

*   **One Problem at a Time:** Address issues or implement features in focused, manageable chunks.
*   **Interconnected Problems:** If multiple issues are tightly coupled, they can be addressed together, but this should be explicitly stated.
*   **Submit Changes:** After each logical fix or feature increment, ensure changes are metaphorically "submitted" (i.e., confirmed and integrated).
*   **Document:** Record findings, solutions, and design decisions in relevant documentation (this includes the detailed sub-task files or other `org/` documents).
*   **Break Down Complexity:** Decompose complex problems into smaller, sequential steps (this is the primary purpose of the detailed sub-task files).
*   **Test Thoroughly:** After each significant change, verify (conceptually, or via actual tests if applicable) that the intended outcome is achieved and no regressions were introduced.

---

## 🔄 Active Tasks (New C++ Phase)

### 1. Core Engine Foundation (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/01_core_engine_foundation/`*
- [ ] Set up new C++ project structure
  - [ ] Define module/directory layout
  - [ ] Configure C++ build system (e.g., CMake, Meson)
  - [ ] Set up initial compiler/linker settings
- [ ] Implement window management (e.g., using SDL, SFML, or platform-specific APIs)
  - [ ] Create window with proper configuration
  - [ ] Set up event loop
  - [ ] Implement basic input handling
- [ ] Set up rendering API (e.g., OpenGL, Vulkan, DirectX)
  - [ ] Initialize graphics device and context
  - [ ] Create swap chain/surface
  - [ ] Set up initial render pipeline
  - [ ] Define strategy for shader management
- [ ] Select/Implement C++ Math Library (e.g., GLM, Eigen)

### 2. Implement Core Systems (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/02_core_systems/`*
- [ ] Choose and implement ECS
  - [ ] Research and select C++ ECS library (e.g., EnTT, Flecs) or design custom solution
  - [ ] Define entity and component structures
  - [ ] Set up system management/scheduler
- [ ] Create voxel data structures
  - [ ] Design chunk structure
  - [ ] Implement voxel storage
  - [ ] Add spatial partitioning (e.g., octrees)
- [ ] Build mesh generation system
  - [ ] Create mesh from voxel data
  - [ ] Implement greedy meshing or other optimization
  - [ ] Add texture mapping support
  - [ ] Implement texture atlas system

### 3. Create Basic Game Structure (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/03_basic_game_structure/`*
- [ ] Implement camera system
  - [ ] Create perspective/orthographic camera
  - [ ] Add camera controls (e.g., mouse look, movement)
  - [ ] Handle window resizing and aspect ratio
  - [ ] Implement frustum culling
- [ ] Set up basic game loop
  - [ ] Implement fixed timestep for updates, variable for rendering
  - [ ] Create update and render phases
- [ ] Add simple world generation
  - [ ] Implement noise-based terrain (e.g., Perlin, Simplex)
  - [ ] Generate basic chunk structure
- [ ] Implement basic lighting model

### 4. Essential Game Infrastructure (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/04_essential_game_infrastructure/`*
- [ ] Implement Physics System
  - [ ] Research and select/design C++ physics solution (e.g., Jolt, PhysX, Bullet, custom)
  - [ ] Integrate basic collision detection
  - [ ] Implement basic physics response
- [ ] Develop UI Framework
  - [ ] Research/select UI library or design custom immediate/retained mode UI system
  - [ ] Implement basic text rendering
  - [ ] Create foundational UI elements (buttons, panels)
- [ ] Implement Game State Management
  - [ ] Design state machine or manager
  - [ ] Handle transitions between states (e.g., menu, game, loading)

### 5. Advanced Engine Features & Game Systems (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/05_advanced_engine_features/`*
- [ ] Implement Level of Detail (LOD) System
  - [ ] Design distance-based LOD for terrain/objects
  - [ ] Implement mesh simplification techniques if needed
- [ ] Implement Save/Load System
  - [ ] Design save file format
  - [ ] Implement serialization/deserialization for game state
- [ ] Performance Profiling and Optimization
  - [ ] Integrate profiling tools
  - [ ] Identify and optimize bottlenecks (e.g., chunk generation, rendering)
  - [ ] Explore multithreading for heavy tasks
  - [ ] Implement occlusion culling (if not already part of initial rendering/camera)

## 📝 Near-Term Priority Tasks
*This section should be updated regularly with the most immediate goals, typically pointing to the current high-level task from the list above.*
1.  Focus on "Core Engine Foundation (C++)".

## 📊 Progress Tracking (C++ Phase)
*This section will track progress for the C++ engine development at a high level.*
- [ ] Project structure and build system defined.
- [ ] Window management implemented.
- [ ] Basic rendering pipeline established.
- [ ] ECS selected/designed.
- [ ] Initial voxel data structures implemented.
- [ ] Camera system functional.

## 🧠 Note to Self (Agent & User)
**WE MUST update this file and the relevant detailed task files after each development step!** This ensures we maintain focus on the most important tasks, have clear visibility into progress, and can adapt our plan as new information becomes available. The active task list should always reflect the current state of development.

**DOCUMENTATION IS CRUCIAL!** With every implementation step, we must update relevant documentation to explain design decisions, patterns used, and architectural considerations. This includes:
1. Updating existing documentation files (`org/` directory)
2. Creating/updating detailed task files in `development_tasks/`
3. Creating new documentation for new systems
4. Adding code comments for complex algorithms
5. Creating diagrams for system interactions when appropriate

**TEST REGULARLY!** We must compile and run the code after implementing each significant feature or component to ensure it works as expected. Never let untested code accumulate - this helps identify issues early when they're easier to fix. After implementing:
1. A new module or system → Compile and run to test it works
2. Changes to existing functionality → Verify nothing broke
3. Complex algorithms or critical systems → Add specific tests
4. Multiple components → Test their integration together

**MAINTAIN SMALL, FOCUSED FILES!** We must keep individual source files small, focused, and well-organized to facilitate better development. This approach:
1. Makes it easier to comprehend the codebase in smaller chunks
2. Reduces context overload when working on specific features
3. Allows us to pinpoint exactly what code I need to reference
4. Promotes better separation of concerns and modularity
5. Results in more maintainable and testable code

---
Last updated: <!-- Copilot will update this date -->
