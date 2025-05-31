# CURRENT_TODO.md - Active Development Tracking

## 🚀 Current Focus: Building Custom C++ Engine Components

This file tracks our immediate high-level tasks and progress. Detailed sub-tasks for each item are located in corresponding files within the `/home/system-x1/Projects/Voxel Castle/development_tasks/` directory.

**IMPORTANT: This file is updated after each major development step is confirmed as complete by the user. The agent works from detailed sub-task files.**

---

## 🤖 AGENT OPERATIONAL PROTOCOL

This protocol outlines how the AI agent (GitHub Copilot) operates. Adherence to these guidelines is crucial for effective collaboration.

### I. Core Collaborative Workflow (The Primary Loop)
1.  **Task Identification:** Focus on a single, open sub-task from the relevant `development_tasks/*.md` file.
    *   If a detailed sub-task file is missing for a new high-level task, create it first, outlining steps.
    *   **If a sub-task involves multiple distinct steps or actions, break it down into clearly defined sub-sub-tasks within the `development_tasks/*.md` file before starting implementation.**
2.  **Implementation:** Perform coding, documentation, or other actions required for the sub-task.
    *   Consult design documents in `docs/design_specifications/` for new features/major changes.
    *   Maintain small, focused files.
3.  **Execution & Testing:**
    *   Run necessary commands (e.g., build, run application) using the `run_in_terminal` tool, providing an explanation.
    *   **CRITICAL COMMAND EXECUTION:** Always prepend `clear &&` to commands producing significant output (e.g., `clear && cmake --build ...`). State that the console is being cleared.
    *   Test thoroughly after each significant change to verify outcomes and check for regressions by running the application or specific tests.
4.  **User Confirmation & Decision Making:**
    *   Pause and explicitly await user confirmation of build/test results (success, failure, observations).
    *   **CRUCIAL DECISIONS:** Discuss and confirm ALL crucial decisions (e.g., library choices, major architectural changes), unrecoverable errors, or blocking questions with the user BEFORE proceeding.
    *   If command output is incomplete or unclear, await user clarification before proceeding.
    *   **TASK COMPLETION CONFIRMATION: The agent MUST NOT mark any task or sub-task as complete in ANY `.md` file (including `development_tasks` files or `CURRENT_TODO.md`) without first proposing the completion to the user and receiving an explicit affirmative response (e.g., "yes", "okay", "confirmed"). The agent should state what it believes is complete and ask "Is it okay to mark this as complete?".**
5.  **Task & Documentation Update (Post-User Confirmation):**
    *   **Only after explicit user confirmation (see point 4 above),** use `insert_edit_into_file` to mark the sub-task as complete in the `development_tasks/*.md` file.
    *   If all sub-tasks for a high-level task are done, propose updating this `CURRENT_TODO.md` to mark the high-level task complete (again, requiring explicit user confirmation).
    *   Document findings, solutions, and design decisions in relevant files (task files, `org/` docs).
6.  **Progression:** Proceed to the next open sub-task. If any part of the process or a specific task is unclear, ask for clarification.

### II. Problem Solving & Iteration
*   **Focused Approach:** Address one problem or feature at a time. If issues are tightly coupled, they can be addressed together, but this should be explicitly stated.
*   **Avoid Repetitive Fixes:** If a particular fix or approach is attempted multiple times without success, pause, re-evaluate the problem, and consider alternative diagnostic steps or solutions. State what has been tried and why it might not be working.
*   **Persistence:** If a problem persists after initial attempts, search for solutions online before repeatedly asking for help or trying the same fix.
*   **Integrate Changes:** After each logical fix or feature increment, ensure changes are metaphorically "submitted" (i.e., confirmed and integrated).

### III. Key Reminders & Tool Usage
*   **Direct Tool Usage:** Always execute necessary commands directly using the appropriate tool (e.g., `run_in_terminal`, `insert_edit_into_file`). Do not ask the user to run commands manually.
*   **File Modifications:** Use `insert_edit_into_file` for all file changes, providing clear explanations.
*   **Build and Run Instructions (Reference):**
    *   Project Root: `/home/system-x1/Projects/Voxel Castle/`
    *   Build: `clear && cmake --build "/home/system-x1/Projects/Voxel Castle/build"`
    *   Run: `clear && "/home/system-x1/Projects/Voxel Castle/build/bin/VoxelFortressGame"`

---

## 🔄 Active Tasks (New C++ Phase)

### 1. Core Engine Foundation (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/01_core_engine_foundation/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [x] Set up new C++ project structure
- [x] Implement window management
- [x] Set up rendering API
- [x] Select/Implement C++ Math Library


### 2. Implement Core Systems (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/02_core_systems/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [x] Choose and implement ECS
- [x] Create voxel data structures
- [x] Build mesh generation system  

### 3. Create Basic Game Structure (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/03_basic_game_structure/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [x] Implement camera system
- [x] Set up basic game loop
- [x] Add simple world generation
- [x] Implement basic lighting model

### 4. Essential Game Infrastructure (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/04_essential_game_infrastructure/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [x] Implement Physics System
- [ ] Develop UI Framework
- [ ] Implement Game State Management
- [ ] Implement Block Placement System
- [ ] Implement Modular UI System
- [ ] Implement Menu System

### 5. Advanced Engine Features & Game Systems (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/05_advanced_engine_features/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [ ] Implement Level of Detail (LOD) System
- [ ] Implement Save/Load System
- [ ] Performance Profiling and Optimization
- [ ] Implement Advanced World Generation System (see `/home/system-x1/Projects/Voxel Castle/development_tasks/05_advanced_engine_features/05_advanced_world_generation.md`)
- [ ] Implement Proper Game Versioning System
- [ ] (Optional) Implement Advanced Asynchronous Mesh Generation (see `/home/system-x1/Projects/Voxel Castle/development_tasks/05_advanced_engine_features/04_optional_async_mesh_generation.md`)

## 📝 Near-Term Priority Tasks
*This section should be updated regularly with the most immediate goals, typically pointing to the current high-level task from the list above.*

**Current Focus:** Implement Block Placement System - Currently debugging critical window disappearance bug that occurs during block modifications in loaded chunks

## 📊 Progress Tracking (C++ Phase)
- [x] Project structure and build system defined.
- [x] Window management implemented.
- [x] Basic rendering pipeline established.
- [x] ECS selected/designed.
- [x] Project successfully builds.

## Recently Completed:
*   **Camera System Implementation:** Progressing through sub-tasks for the flying spectator camera, as detailed in `development_tasks/03_basic_game_structure/01_implement_camera_system.md`. Several foundational items related to rendering setup and camera view/projection integration have been reviewed and confirmed as complete.
