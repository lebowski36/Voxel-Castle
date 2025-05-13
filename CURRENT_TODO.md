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

**Note on Task Completion:** A main task item (e.g., `- [ ] Implement camera system`) in the lists below is marked as complete (`[x]`) if and only if all its constituent sub-tasks, as detailed in its corresponding markdown file within the `development_tasks/` directory, have been completed and verified.

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
- [ ] Build mesh generation system  
    *Paused: Integration with WorldManager deferred while camera system is implemented.*

### 3. Create Basic Game Structure (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/03_basic_game_structure/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [ ] Implement camera system *(In Progress: Flying spectator camera implementation)*
- [ ] Set up basic game loop
- [ ] Add simple world generation
- [ ] Implement basic lighting model

### 4. Essential Game Infrastructure (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/04_essential_game_infrastructure/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [ ] Implement Physics System
- [ ] Develop UI Framework
- [ ] Implement Game State Management

### 5. Advanced Engine Features & Game Systems (C++)
*Detailed sub-tasks in `/home/system-x1/Projects/Voxel Castle/development_tasks/05_advanced_engine_features/`. A main task in this section is marked `[x]` ONLY if ALL its detailed sub-tasks in the corresponding file are complete.*
- [ ] Implement Level of Detail (LOD) System
- [ ] Implement Save/Load System
- [ ] Performance Profiling and Optimization


## 📝 Near-Term Priority Tasks
*This section should be updated regularly with the most immediate goals, typically pointing to the current high-level task from the list above.*

### 🚧 Implement Flying Spectator Camera (May 13, 2025)

- [ ] **Design and implement a `SpectatorCamera` class/struct** for free-flying camera movement and mouse look.
- [ ] **Integrate camera with rendering pipeline** (view/projection matrices to shaders).
- [ ] **Test camera controls and verify mesh rendering responds to camera movement.**

**Summary:**
Paused mesh generation system integration to focus on implementing a reusable flying spectator camera. This will enable flexible navigation and testing of voxel worlds. Once the camera system is complete and tested, we will resume mesh system integration and chunk update logic.

## 📊 Progress Tracking (C++ Phase)
*This section will track progress for the C++ engine development at a high level.*
- [x] Project structure and build system defined.
- [x] Window management implemented.
- [x] Basic rendering pipeline established.
- [x] ECS selected/designed.
- [x] Project successfully builds.

## Recently Completed:
*   **Camera System Implementation:** Progressing through sub-tasks for the flying spectator camera, as detailed in `development_tasks/03_basic_game_structure/01_implement_camera_system.md`. Several foundational items related to rendering setup and camera view/projection integration have been reviewed and confirmed as complete.

## 🧠 Note to Self (Agent)

**Core Collaborative Workflow (User-Confirmed Steps):**

My primary directive is to follow a strict, user-confirmed, step-by-step process for every sub-task:

1.  **Focus on One Sub-Task:** Address only the current open sub-task from the detailed `development_tasks/*.md` file.
2.  **Implement/Code:** Perform the necessary coding or action for the sub-task.
3.  **Test Changes:** After implementation, initiate a build and/or relevant tests (including running the executable if applicable to observe behavior).
4.  **Await User Confirmation:** Pause and explicitly await the users confirmation of the build/test results (success, failure, or observations). I will not proceed if a test fails or if the outcome is unclear without your feedback.
5.  **Mark Complete (Post-Confirmation):** Only after you confirm the successful completion and satisfactory outcome of the sub-task, will I mark it as complete in the corresponding detailed task file.
6.  **Proceed to Next Sub-Task:** Once confirmed and marked, move to the next open sub-task.

If any part of this process or a specific task is unclear, I will ask for clarification before proceeding. This iterative, confirmation-driven approach is key to our collaboration.

---

**Additional Reminders & Guidelines (Preserving Previous Instructions):**

*   **Agent Self-Correction/Reminder:** Per user instruction, I will always execute necessary commands directly using the appropriate tool. I will not ask you to run commands manually.
*   **Avoid Repetitive Fixes & Ensure Test-Driven Iteration:** If a particular fix or approach is attempted multiple times without success (e.g., repeatedly adjusting camera parameters in small increments), I must pause, re-evaluate the problem from a different angle, and consider alternative diagnostic steps or solutions. I will avoid making similar changes repeatedly without clear evidence from testing that the new change is likely to succeed or provides new information. Each distinct change should be tested, and its outcome analyzed, before layering on subsequent changes to the same system. If I find myself in a loop, I will explicitly state what has been tried and why it might not be working, and then propose a different strategy or seek your guidance.
*   **Autonomous Operation Mandate & Testing Protocol (Reinforced by Core Workflow):** My primary mode of operation is to work autonomously through the defined sub-tasks, following the Core Collaborative Workflow above. I will continue coding, implementing features, and resolving issues independently. However, I will pause and await user interaction under the following circumstances (in addition to step 4 of the Core Workflow):
    1.  A critical design decision with multiple viable options requires user input.
    2.  A question arises that genuinely blocks progress and cannot be resolved through available context or research.
    3.  An unrecoverable error or significant unexpected behavior is encountered during development or testing.

*   **Testing Best Practices (To be followed diligently):**
    *   **Test Regularly:** Compile and **run the application** after implementing each significant feature or component to ensure it works as expected, as per the Core Workflow. Never let untested code accumulate.
        *   After a new module or system: Compile and run to test it works.
        *   After changes to existing functionality: Verify nothing broke by running the application.
        *   After complex algorithms or critical systems: Add specific tests if feasible, or at least perform thorough manual checks by running and observing the application.
        *   After integrating multiple components: Test their integration together by running the application.
    *   **Awaiting Test Output:** If I'm unable to see the complete output from a command I've run (especially build outputs or error messages), I **must wait for you to provide the results** before proceeding.

*   **General Operational Guidelines:**
    *   **ABSOLUTELY CRITICAL: Clear Console with Command!** Before executing any new command in the terminal that produces significant output (especially builds or tests), I **MUST** prepend the `clear` command to the main command, joined by `&&` (e.g., `clear && cmake --build build`). This ensures the console is cleared immediately before the intended command runs, preventing output from previous commands from obscuring the new results. I will explicitly state that the command includes clearing the console and ensure this is followed for every command execution.
    *   **CRITICAL - Update Task Files:** I **MUST ALWAYS** update this file (`CURRENT_TODO.md`) and, most importantly, the relevant detailed task files in `development_tasks/` (by checking off completed sub-tasks and updating their status) after each development step is confirmed complete by you (as per Core Workflow step 5). **Sub-tasks themselves should be maintained within their specific detailed markdown files, not duplicated or expanded in this `CURRENT_TODO.md` file.** This `CURRENT_TODO.md` file should only list the main high-level tasks. This is paramount for accurate progress tracking and ensuring I don't repeat or miss steps.
    *   **Consult Design Documents:** Before implementing new features or making significant design choices, I will consult the relevant documents in the `/home/system-x1/Projects/Voxel Castle/docs/design_specifications/` directory (including `Voxel Fortress Tech Stack (C++ Custom Engine Approach).md`, `Voxel Fortress Game Concept Details.md`, and other related files) to ensure my work aligns with the established project vision, architecture, and technical guidelines.
    *   **Crucial Decisions:** ALWAYS CONFIRM CRUCIAL DECISIONS WITH YOU. Before making significant choices (e.g., selecting a library, deciding on a core architectural pattern), present the options, my recommendation (if any), and ask for your input and confirmation.
    *   **Problem Persistence:** IF A PROBLEM PERSISTS after initial attempts, I will search for solutions online before asking you for help or trying the same fix repeatedly.
    *   **Documentation:** DOCUMENTATION IS CRUCIAL! With every implementation step, update relevant documentation.
    *   **Small, Focused Files:** MAINTAIN SMALL, FOCUSED FILES for better organization and maintainability.
    *   **Build and Run Instructions:**
        *   The project root directory is: `/home/system-x1/Projects/Voxel Castle/`.
        *   To clear console and build: `clear && cmake --build "/home/system-x1/Projects/Voxel Castle/build"`
        *   To clear console and run: `clear && "/home/system-x1/Projects/Voxel Castle/build/bin/VoxelFortressGame"`
