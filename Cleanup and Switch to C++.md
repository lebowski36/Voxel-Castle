# Project Cleanup and Transition to C++ Plan

**Date:** May 11, 2025

**Objective:** To comprehensively update the Voxel Fortress project documentation and structure to reflect the transition from a Rust-based development approach to a C++ custom engine. This involves removing obsolete Rust-specific files and artifacts, updating conceptual documents to be C++-relevant, and organizing technical information.

**Important:** This document outlines the steps for cleaning up the project and transitioning to C++.
*   **Processing Workflow & Confirmations:**
    *   **General Tasks:** For most tasks outside the `org/Game Concept/` and `org/Tech Stack/` directories, the AI agent (GitHub Copilot) will perform the actions, then ask for your confirmation before marking the item as complete (`- [x]`).
    *   **`org/Game Concept/` Directory:** For files within this directory, the AI will operate with increased autonomy. It will independently review documents, update them to reflect the C++ development approach (e.g., adding C++ specific technical considerations, removing Rust-isms, suggesting relevant C++ libraries). These update tasks will be marked as complete (`- [x]`) by the AI in this plan upon their execution. The AI will only pause and request your input if it encounters ambiguities, requires a significant decision beyond standard C++ contextualization (e.g., a major conceptual rewrite), or if a file is a candidate for deletion.
    *   **`org/Tech Stack/` Directory:** For files within this directory, the AI will independently review documents and update them *in place* to reflect the C++ development approach (e.g., adding C++ specific technical considerations, removing Rust-isms, suggesting relevant C++ libraries). These files will be kept as separate, detailed documents. The main `/home/system-x1/Projects/Voxel Castle/org/Voxel Fortress Tech Stack (C++ Custom Engine Approach).md` will serve as a high-level overview, potentially linking to them. This approach helps keep documents focused and manage context size. These update tasks will be marked as complete (`- [x]`) by the AI in this plan upon their execution. The AI will only pause and request your input if it encounters ambiguities or requires a significant decision beyond standard C++ contextualization.
*   **Human Approval for Deletions:** CRITICALLY, for any action involving the **DELETION OF FILES OR DIRECTORIES** (regardless of location), the AI agent will first propose the deletion. **NO FILES OR DIRECTORIES WILL BE DELETED WITHOUT YOUR EXPLICIT, INDIVIDUAL CONFIRMATION FOR EACH PROPOSED DELETION.** The agent will await your direct approval before generating any removal commands.
Please address each item individually and provide confirmation as required.

---

## I. Workspace Root Cleanup

The following files at the project root (`/home/system-x1/Projects/Voxel Castle/`) need review and action:

- [x] **`bevy_error.log`**:
    *   Assessment: Likely related to the previous Bevy/Rust implementation.
    *   Action: Remove.
- [x] **`compile_errors.log`**:
    *   Assessment: Likely related to Rust compilation.
    *   Action: Remove.
- [x] **`CURRENT_TODO.md`**:
    *   Assessment: May contain general TODOs or Rust-specific TODOs.
    *   Action: Review. Preserve general game development TODOs. Remove or update Rust-specific TODOs to be C++ relevant. Consider migrating relevant TODOs to a new C++ project management system or a revised `ROADMAP.md`.
- [x] **`DEVELOPMENT.md`**:
    *   Assessment: May contain Rust-specific development guidelines.
    *   Action: Review. Rewrite to reflect C++ development practices, build environment (e.g., CMake), coding standards, and C++-specific tooling.
- [x] **`ENGINE_MIGRATION_TODO.md`**:
    *   Assessment: This might be a precursor to the current transition effort.
    *   Action: Review. Incorporate any still-relevant, non-redundant tasks into this plan or the main C++ tech stack document. Then, remove this file to avoid confusion.
- [x] **`MUST-READ-Prompt-Fixing-Bugs.txt`**:
    *   Assessment: General advice.
    *   Action: Review. Keep if still relevant and general.
- [x] **`README.md`** (Root project README):
    *   Assessment: The main project README. Likely needs significant updates.
    *   Action: Rewrite to reflect the C++ custom engine approach. Update project description, build instructions (once established for C++), and contribution guidelines for a C++ context.
- [x] **`ROADMAP.md`** (Root project ROADMAP):
    *   Assessment: High-level project roadmap.
    *   Action: Review and update all sections to align with the C++ development plan and timelines. Ensure consistency with `org/Development Roadmap.md` (if kept). Remove Rust-specific milestones.
- [x] **`TRANSITION_PLAN.md`**:
    *   Assessment: If an older file with this name exists and is different from this current comprehensive plan, it\'s obsolete.
    *   Action: Remove any older, superseded version. This current document (`Cleanup and Switch to C++.md`) will be the guide.

---

## II. `org` Directory Cleanup and Update

Path: `/home/system-x1/Projects/Voxel Castle/org/`

- [x] **`Development Roadmap.md`**: (Processed: Moved from `org/Game Concept/`, reviewed, C++ context added, internal links updated)
    *   Assessment: Detailed development roadmap. May be redundant or not align with the new C++ direction.
    *   Action: Review its content and purpose. **Decide if it should be updated for C++ or deleted.** If kept, ensure all phases, tasks, and technical considerations align with the C++ custom engine strategy and the root `ROADMAP.md`. Remove any Rust-specific milestones or technologies. Execute the decision (update or delete).
- [ ] **`Voxel Fortress Game Concept Details.md`**:
    *   Assessment: Main game concept document. Already partially updated for C++.
    *   Action: Perform a full review. Ensure all sections are language-agnostic or explicitly updated with C++ considerations where technical details are implied (e.g., performance characteristics, library availability). Verify links to other documents are correct, especially the tech stack link to the C++ version.
- [ ] **`Voxel Fortress Tech Stack (C++ Custom Engine Approach).md`**:
    *   Assessment: The new central technical document for the C++ engine.
    *   Action: Ensure it serves as a high-level overview. Add links to the specialized documents in `org/Tech Stack/` as appropriate.
- [ ] **`Voxel Fortress Tech Stack (Rust Custom Engine Approach).md`**:
    *   Assessment: Obsolete Rust-specific tech stack.
    *   Action: Review carefully. Identify any core concepts, architectural ideas, or problem-solving strategies that are valuable regardless of the programming language and are NOT YET documented in the `Voxel Fortress Tech Stack (C++ Custom Engine Approach).md` or other relevant `org/Game Concept/` documents. If such unique, valuable information exists, extract and adapt it into the appropriate C++ document(s). Once all valuable, non-Rust-specific, and non-redundant information has been migrated, **confirm for deletion.** If the document solely contains Rust-specific implementation details or its conceptual information is already fully covered elsewhere, it can be confirmed for deletion directly.

### A. `org/Game Concept/` Sub-directory

Path: `/home/system-x1/Projects/Voxel Castle/org/Game Concept/`

- [ ] **Process all `.md` files within `org/Game Concept/`**:
    *   **Files to process (ensure correct filenames for `Physics Systems.md` and `Player Interactions & Controls.md` during their turn):**
        - [x] `AI Systems.md` (Processed: C++ context added)
        - [x] `Building & Logistics.md` (Processed: Reviewed, no C++ specific changes needed)
        - [x] `Colony Simulation.md` (Processed: C++ context added)
        - [x] `Control Systems.md` (Processed: C++ context added)
        - [x] `Data Structures.md` (Processed: C++ context added)
        - [x] `Development Roadmap.md` (Processed: Moved to `org/Development Roadmap.md`, C++ context added, internal links updated, original in this sub-directory deleted)
        - [x] `Emergent Narrative.md` (Processed: C++ context added)
        - [x] `Needs & Moods.md` (Processed: C++ context added)
        - [x] `Performance Optimization.md` (Processed: C++ context added, content is for Rendering Systems)
        - [x] `Physics Systems.md` (Processed: C++ context added)
        - [x] `Player Interactions & Controls.md` (Processed: C++ context added)
        - [x] `Rendering Systems.md` (Processed: C++ context added)
        - [x] `Resource Management.md` (Processed: C++ context added)
        - [x] `Skills & Professions.md` (Processed: C++ context added)
        - [x] `Threats & Challenges.md` (Processed: C++ context added)
        - [x] `UI Design.md` (Processed: C++ context added)
        - [x] `Visual Design.md` (Processed: C++ context added)
        - [x] `World Generation.md` (Processed: C++ context added)
    *   **Note on `org/Game Concept/Development Roadmap.md`**: Assess if this is a duplicate of `org/Development Roadmap.md` or the root `ROADMAP.md`, or if it serves a unique, valuable purpose as a granular plan for game concepts. If redundant or outdated, mark for removal. If distinct and valuable, update per the actions below.
    *   **Action for each file identified for update (apply to unchecked files above):**
        1.  Review Content: Understand its core concepts.
        2.  Remove Rust-isms: Eliminate any language, library (e.g., Bevy, specific Rust crates), or tool references specific to Rust.
        3.  Add C++ Context/Rewrite: Where technical implementation is discussed or implied:
            *   Rewrite sections to reflect how the system would be designed and implemented in C++.
            *   Mention relevant C++ libraries (e.g., Bullet Physics, EnTT for ECS, Dear ImGui for UI tools, SDL2/GLFW for windowing, Vulkan for rendering).
            *   Discuss C++ specific design patterns or architectural choices.
            *   If the original design was heavily tied to Rust\'s unique features (e.g., borrow checker influencing data structure design), fundamentally rethink and rewrite those parts for an idiomatic C++ approach.
        4.  Ensure Conceptual Integrity: The core gameplay or system *concept* should remain intact unless the switch to C++ fundamentally enables a better approach that still aligns with the game\'s vision.
        5.  Mark files for removal if they become entirely obsolete after review.

### B. `org/Tech Stack/` Sub-directory

Path: `/home/system-x1/Projects/Voxel Castle/org/Tech Stack/`

- [x] **Process all `.md` files within `org/Tech Stack/`**:
    *   Files include: `AI Architecture.md`, `Engine Architecture.md`, `Entity Component System.md`, `Physics System.md`, `README.md` (within `org/Tech Stack/`), `Rendering Pipeline.md`, `Voxel Data Structures.md`.
    *   **Strategy:** These files will be updated individually *in place* to reflect a C++ context, removing Rust-specific information. They will be kept as separate, detailed documents. The main `/home/system-x1/Projects/Voxel Castle/org/Voxel Fortress Tech Stack (C++ Custom Engine Approach).md` will serve as a high-level overview, potentially linking to these specialized files. This approach helps keep documents focused and manage context size.
    *   **Action for each file:**
        1.  Review Content: Understand its core technical area.
        2.  Remove Rust-isms: Eliminate any language, library, or tool references specific to Rust.
        3.  Update for C++ Context: Rewrite sections to reflect C++ design, implementation, relevant C++ libraries (e.g., for AI, specific physics libraries if different from main, rendering APIs if detailed here), C++ patterns, and terminology.
        4.  Ensure it serves as a detailed, C++-focused document for its specific domain (e.g., `AI Architecture.md` should be the go-to for AI tech details).
        5.  Mark as processed in this plan once updated.
        - [x] Review and update `Voxel Data Structures.md` for C++ context.
    - [x] **Final Review of `org/Tech Stack/` Directory:** All files reviewed, updated for C++ context in place, and are now standalone detailed documents. The main `Voxel Fortress Tech Stack (C++ Custom Engine Approach).md` has been updated to be a high-level overview linking to these.

---

## III. `voxel_engine` Directory Cleanup

Path: `/home/system-x1/Projects/Voxel Castle/voxel_engine/`

- [x] **Review `voxel_engine/ENGINE_ARCHITECTURE.md`**:
    *   Action: If it contains unique, valuable information not present in `org/Tech Stack/Engine Architecture.md` (and thus not yet merged into the main C++ tech stack doc), merge that information into `/home/system-x1/Projects/Voxel Castle/org/Voxel Fortress Tech Stack (C++ Custom Engine Approach).md`. (Self-assessment: No unique, valuable, non-Rust-specific information found that isn't already better covered in C++ docs. No merge needed.)
- [x] **Remove the entire `voxel_engine` directory**:
    *   Assessment: This entire directory is the former Rust-based game engine. It includes `Cargo.toml`, `Cargo.lock`, Rust source files (`.rs`), the (now reviewed) `ENGINE_ARCHITECTURE.md`, and potentially build artifacts.
    *   Action: Delete the directory.

---

## IV. `voxel_fortress` Directory Cleanup

Path: `/home/system-x1/Projects/Voxel Castle/voxel_fortress/`

- [x] **Review and Process `voxel_fortress/feature_docs/`**: (Processed: Content reviewed, relevant concepts were confirmed to be integrated into `org/Game Concept/Rendering Systems.md` and `org/Game Concept/World Generation.md`. Original files `chunk_lod_streaming.md` and `hierarchical_worldgen.md` are redundant.)
    *   Files: `chunk_lod_streaming.md`, `hierarchical_worldgen.md`.
    *   Action: These likely contain valuable conceptual information. Adapt them to be C++ relevant (removing Rust specifics). Decide whether to move them to `org/Game Concept/` as new files or integrate their content into relevant existing documents there (e.g., `Rendering Systems.md`, `World Generation.md`). Execute the decision.
- [x] **Review and Process other Markdown files in `voxel_fortress/`**: (Processed: Files reviewed. No unique, valuable, non-Rust-specific information found for merging. Files are obsolete.)
    *   Files: `PROJECT_OVERVIEW.md`, `PROMPT.md`, `ui_fix_docs.md`, `ui.md`, `ui.todo.md`.
    *   Action: Extract any non-Rust-specific conceptual or TODO information that is still valuable. Merge this into relevant `org/` documents or the main `ROADMAP.md`. Mark these files for removal after processing.
- [x] **Remove the entire `voxel_fortress` directory**:
    *   Assessment: This entire directory is the former Rust-based main game project. It includes `Cargo.toml`, `Cargo.lock`, Rust source files (`.rs`), the (now processed) `feature_docs/` and other Markdown files, and the `target/` build output directory.
    *   Action: Delete the directory. This will remove Rust-specific dependencies and build artifacts.

---

## V. Post-Cleanup Next Steps

(These are for after the cleanup is complete and are not individual checkboxes for the AI to wait on at this stage)

1.  **Establish C++ Project Structure:** Create a new directory structure for the C++ engine and game (e.g., `engine/`, `game/`, or a monolithic `src/` within a single C++ project).
2.  **Initialize C++ Build System:** Set up CMake (or chosen C++ build system like Meson or Bazel) with a root `CMakeLists.txt`.
3.  **Version Control:** Commit all cleanup changes and the new C++ project structure to Git in a clear, staged manner.
4.  **Begin C++ Development:** Start implementing the core systems based on the updated design documents and the C++ tech stack, beginning with basic windowing, input, and a rendering loop.

---
This plan provides a structured approach to refactoring the project for C++ development.
