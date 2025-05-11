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
    *   Within these detailed task files, sub-tasks can be further broken down using a hierarchical numbering scheme (e.g., 2.1, 2.1.1, 2.1.2, 2.2) for clarity if needed.
3.  **Execution Focus:** The agent will primarily work from the detailed sub-task files.
4.  **User Confirmation & Involvement:**
    *   The agent will confirm individual significant actions or outputs from the sub-task file with the user.
    *   **Crucial decisions (e.g., library choices, major architectural changes) MUST be discussed and confirmed with the user before proceeding.**
    *   Once all sub-steps within a detailed task file are completed and confirmed, the agent will propose marking the corresponding high-level task in *this* `CURRENT_TODO.md` as complete.
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
- [x] Set up new C++ project structure
  - [x] Define module/directory layout
  - [x] Configure C++ build system (e.g., CMake, Meson)
  - [x] Set up initial compiler/linker settings
- [x] Implement window management (e.g., using SDL, SFML, or platform-specific APIs)
  - [x] Create window with proper configuration
  - [x] Set up event loop
  - [x] Implement basic input handling
- [x] Set up rendering API (e.g., OpenGL, Vulkan, DirectX)
  - [x] Initialize graphics device and context
  - [x] Create swap chain/surface
  - [x] Set up initial render pipeline
  - [x] Define strategy for shader management
- [x] Select/Implement C++ Math Library (e.g., GLM, Eigen)

### 2. Implement Core Systems (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/02_core_systems/`*
- [x] Choose and implement ECS
  - [x] Research and select C++ ECS library (e.g., EnTT, Flecs) or design custom solution
  - [x] Define entity and component structures
  - [x] Set up system management/scheduler
- [ ] Create voxel data structures
  - [x] Design chunk structure *(Covers Voxel, ChunkSegment, ChunkColumn design)*
  - [x] Implement voxel storage *(Covers Voxel, ChunkSegment, ChunkColumn, WorldManager implementation)*
  - [ ] Add spatial partitioning (e.g., octrees)
- [ ] Build mesh generation system
  - [x] Create mesh from voxel data // Naive meshing implemented and tested, but part of the larger item
  - [x] Implement greedy meshing or other optimization
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
1.  Focus on "Implement Core Systems (C++)" - specifically, "Build mesh generation system".

## 📊 Progress Tracking (C++ Phase)
*This section will track progress for the C++ engine development at a high level.*
- [x] Project structure and build system defined.
- [x] Window management implemented.
- [x] Basic rendering pipeline established.
- [x] ECS selected/designed.

## 🧠 Note to Self (Agent)
**Agent Self-Correction/Reminder:** Per user instruction, I will always execute necessary commands directly using the appropriate tool. I will not ask the user to run commands manually.

**Autonomous Operation Mandate & Testing Protocol:** My primary mode of operation is to work autonomously through the defined sub-tasks. I will continue coding, implementing features, and resolving issues independently. However, I will pause and await user interaction under the following circumstances:
1.  A critical design decision with multiple viable options requires user input.
2.  Specific feedback on a completed segment of work is needed before proceeding.
3.  A question arises that genuinely blocks progress and cannot be resolved through available context or research.
4.  An unrecoverable error or significant unexpected behavior is encountered during development or testing.
5.  **After implementing a significant feature, component, or a set of related changes, I will initiate a build and/or relevant tests (which includes running the main executable if applicable to observe behavior). I will then pause and explicitly await user confirmation of the build/test results (success, failure, or observations) before proceeding to the next task. I will not move on if a test fails or if the outcome is unclear without user feedback.**

**Testing Best Practices (To be followed diligently):**
*   **Test Regularly:** Compile and **run the application** after implementing each significant feature or component to ensure it works as expected. Never let untested code accumulate.
    *   After a new module or system: Compile and run to test it works.
    *   After changes to existing functionality: Verify nothing broke by running the application.
    *   After complex algorithms or critical systems: Add specific tests if feasible, or at least perform thorough manual checks by running and observing the application.
    *   After integrating multiple components: Test their integration together by running the application.
*   **Awaiting Test Output:** If I'm unable to see the complete output from a command I've run (especially build outputs or error messages), I **must wait for the user to provide the results** before proceeding.

**General Operational Guidelines:**
*   **ABSOLUTELY CRITICAL: Clear Console with Command!** Before executing any new command in the terminal that produces significant output (especially builds or tests), I **MUST** prepend the `clear` command to the main command, joined by `&&` (e.g., `clear && cmake --build build`). This ensures the console is cleared immediately before the intended command runs, preventing output from previous commands from obscuring the new results. I will explicitly state that the command includes clearing the console.
*   **CRITICAL - Update Task Files:** I **MUST ALWAYS** update this file (`CURRENT_TODO.md`) and, most importantly, the relevant detailed task files in `development_tasks/` (by checking off completed sub-tasks and updating their status) after each development step is confirmed complete. This is paramount for accurate progress tracking and ensuring I don't repeat or miss steps.
*   **Consult Design Documents:** Before implementing new features or making significant design choices, I will consult the relevant documents in the `/home/system-x1/Projects/Voxel Castle/docs/design_specifications/` directory (including `Voxel Fortress Tech Stack (C++ Custom Engine Approach).md`, `Voxel Fortress Game Concept Details.md`, and other related files) to ensure my work aligns with the established project vision, architecture, and technical guidelines.
*   **Crucial Decisions:** ALWAYS CONFIRM CRUCIAL DECISIONS WITH THE USER. Before making significant choices (e.g., selecting a library, deciding on a core architectural pattern), present the options, your recommendation (if any), and ask for the user's input and confirmation.
*   **Problem Persistence:** IF A PROBLEM PERSISTS after initial attempts, I will search for solutions online before asking the user for help or trying the same fix repeatedly.
*   **Documentation:** DOCUMENTATION IS CRUCIAL! With every implementation step, update relevant documentation.
*   **Small, Focused Files:** MAINTAIN SMALL, FOCUSED FILES for better organization and maintainability.

---
Last updated: May 11, 2025 // Updated agent guidelines for console clearing and command execution.
