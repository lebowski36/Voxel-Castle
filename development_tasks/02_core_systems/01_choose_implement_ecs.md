# Task: Choose and Implement Entity Component System (ECS)

**Date Created:** 2025-05-11
**Status:** Not Started
**Depends On:** Core Engine Foundation (Math Library Integrated)
**Leads To:** Voxel Data Structures, Mesh Generation

## 1. Overview
This task covers the research, selection, integration, and initial implementation of an Entity Component System (ECS) for the Voxel Castle engine. The ECS will be a fundamental part of the engine's architecture, managing game entities, their data (components), and their behavior (systems).

## 2. Why an ECS?
An ECS architecture is chosen for its benefits in:
*   **Performance:** Data-oriented design leads to better cache utilization.
*   **Flexibility:** Easy to add/remove/modify aspects of entities without deep inheritance hierarchies.
*   **Decoupling:** Separates data (components) from logic (systems), improving modularity and testability.
*   **Parallelism:** Often makes it easier to design systems that can run in parallel.

## 3. Sub-Tasks

### 3.1. Task: Research and Select ECS Solution
- **Status:** In Progress
- **Sub-Tasks:**
    - [x] 3.1.1. Research prominent C++ ECS libraries (e.g., EnTT, Flecs).
        - **Status:** COMPLETE
        - **Notes:** Research completed for EnTT and Flecs. Findings documented above.
    - [ ] 3.1.2. Evaluate the feasibility and trade-offs of a custom ECS solution for Voxel Castle.
        - **Status:** ADDRESSED
        - **Notes:** While a custom ECS could be tailored, the benefits of using a well-tested, feature-rich, and performant library like Flecs outweigh the development effort and potential pitfalls of a custom solution for this project. The focus should be on game development rather than re-inventing core engine components unless strictly necessary.
    - [x] 3.1.3. Define clear selection criteria (e.g., performance, ease of use, features, community support, licensing).
        - **Status:** COMPLETE
        - **Notes:** Criteria implicitly defined and used during the comparison (see table above). Key factors included performance, ease of integration, feature set (especially for potential future needs like reflection or editor integration), and C++ standard compatibility.
    - [x] 3.1.4. Make a decision on the ECS library or custom approach.
        - **Status:** COMPLETE
        - **Decision:** Flecs has been chosen.
    - [x] 3.1.5. Document the final decision and reasoning.
        - **Status:** COMPLETE
        - **Documentation:** See Section 5: Decisions Made.

### 3.1.1.A. Library Comparison Summary: EnTT vs. Flecs

To aid in the decision-making process (Sub-task 3.1.4), here's a summary of key aspects:

| Feature             | EnTT (skypjack/entt)                                  | Flecs (SanderMertens/flecs)                               | Voxel Castle Considerations                                                                                                |
| :------------------ | :---------------------------------------------------- | :-------------------------------------------------------- | :------------------------------------------------------------------------------------------------------------------------- |
| **Core Philosophy** | Modern C++17, header-only, performance-centric, broad utility set beyond ECS. | C99 core with C++11 API, lightweight, speed, strong entity relationships. | Both are excellent. C++17 is already our target. Flecs' C99 core means high portability.                               | 
| **Primary API**     | C++17                                                 | C++11 (over C99 core), also offers C API.                 | EnTT's API is often praised for its expressiveness. Flecs' C++ API is modern but avoids STL, which can be a pro or con.        |
| **Key Strengths**   | Raw speed, extensive feature set (signals, resource cache, reflection, scheduler), mature, used in Minecraft. | Entity relationships (hierarchies, prefabs), modularity (addons for systems), web explorer, used in Hytale. | Flecs' relationship model could be very beneficial for complex voxel structures or entities composed of multiple parts. EnTT's rich feature set might reduce the need for other small utility libraries. |
| **Performance**     | Extremely high, often a benchmark leader.             | Extremely high, also a benchmark leader.                  | Both are likely more than performant enough. Specific micro-benchmarks vary.                                               |
| **Ease of Use**     | Generally good API design, well-documented.           | Well-documented, C++ API is user-friendly.                | Learning curve should be manageable for either.                                                                            |
| **Build/Integration**| Header-only, excellent CMake & package manager support. | Can be header-only or compiled, good CMake/Meson support. | Both are easy to integrate. EnTT might be slightly simpler due to being purely header-only.                                |
| **Dependencies**    | No external dependencies for the core ECS.            | No external dependencies for the core ECS.                | Clean integration for both.                                                                                                |
| **Community**       | Large, active, good support (Discord, Gitter).        | Growing, active, good support (Discord).                  | Both have healthy communities.                                                                                             |
| **Licensing**       | MIT                                                   | MIT                                                       | Permissive and suitable for our project.                                                                                   |

**Recommendation Leaning:**

*   Choose **EnTT** if:
    *   The absolute rawest speed in typical ECS operations is the top priority.
    *   You value a rich, integrated set of utilities (signals, resource cache, etc.) alongside the ECS.
    *   A pure C++17 header-only library is preferred.
*   Choose **Flecs** if:
    *   Advanced entity relationships (hierarchies, prefabs, `IsA` relationships) are a high priority for your game's design (e.g., complex multi-part entities, structured game objects).
    *   You prefer its modular addon system or find its C API or non-STL C++ API appealing.
    *   The web-based "Flecs Explorer" for debugging and visualization is a significant draw.

For "Voxel Castle," which will likely involve complex structures and potentially hierarchical entities (e.g., a castle made of many distinct, but related, voxel chunks or objects), **Flecs' strong support for entity relationships might offer a more direct and powerful way to model these scenarios.** However, EnTT's performance and broader utility set are also very compelling.

### 3.2. Integration
*Now that Flecs has been selected, we will integrate it into the Voxel Castle project.*

*   **Priority:** High
*   **Assigned To:** Agent
*   **Description:** Integrate the chosen ECS into the project.
    *   [x] **3.2.1.** Add the Flecs library to the project (e.g., `external/flecs`, likely via git submodule).
    *   [x] **3.2.2.** Update CMake build system (`engine/CMakeLists.txt` and potentially root `CMakeLists.txt`) to include Flecs.
    *   [x] **3.2.3.** Perform a test compile to ensure Flecs headers can be found and used.

### 3.3. Initial Implementation
*   **Priority:** Medium
*   **Assigned To:** Agent
*   **Description:** Implement basic ECS structures to verify functionality.
    *   [x] **3.3.1.** Define basic components.
        *   Create header files for a few simple components (e.g., `PositionComponent.h`, `VelocityComponent.h`, `RenderableComponent.h`) within the engine's module structure (e.g., `engine/include/ecs/components/`).
        *   Example `PositionComponent`: `struct PositionComponent { glm::vec3 position; };`
    *   [x] **3.3.2.** Define basic systems.
        *   Create header/source files for a few simple systems (e.g., `MovementSystem.h/.cpp`) within the engine's module structure (e.g., `engine/src/ecs/systems/`).
        *   Example `MovementSystem`: A system that iterates over entities with `PositionComponent` and `VelocityComponent` and updates positions.
    *   [x] **3.3.3.** Set up ECS registry/world.
        *   Instantiate the ECS registry (e.g., `flecs::world ecs;`) in an appropriate place (e.g., a future `Engine` class or temporarily in `main.cpp`).
    *   [x] **3.3.4.** Implement system execution logic.
        *   Create a basic mechanism to update systems in the game loop (e.g., `ecs.progress();`).

### 3.4. Testing and Verification
*   **Priority:** High
*   **Assigned To:** Agent
*   **Description:** Test the integrated ECS with the initial components and systems.
    *   [x] **3.4.1.** Add temporary test code.
        *   In `game/src/main.cpp` (or a dedicated test file):
            *   Create a few entities.
            *   Add components to these entities with initial values.
            *   Run the systems (e.g., call the `MovementSystem::update()` method via `ecs.progress()`).
    *   [x] **3.4.2.** Verify system effects.
        *   Print component values (e.g., positions) before and after system execution to confirm they are being modified as expected.
        *   Log messages from systems.
    *   [x] **3.4.3.** Confirm basic ECS functionality.
        *   Ensure entities can be created, components added/removed/retrieved, and systems can query and modify components.

### 3.5. Documentation and Cleanup
*   **Priority:** Medium
*   **Assigned To:** Agent
*   **Description:** Document the ECS choice and clean up temporary test code.
    *   [x] **3.5.1.** Update project documentation.
        *   Add a section to `docs/design_specifications/Voxel Fortress Tech Stack (C++ Custom Engine Approach).md` detailing the chosen ECS, its rationale, and basic usage patterns within the engine.
    *   [x] **3.5.2.** Remove temporary test code from `main.cpp` or test files.
        *   Plan for how ECS initialization and system updates will be handled in the main engine loop or dedicated game state managers.
    *   [x] **3.5.3.** Mark the high-level ECS task as complete in `CURRENT_TODO.md`.
    *   [x] **3.5.4.** Update "Last updated" date in `CURRENT_TODO.md`.

## 4. Considerations
*   **Performance:** Pay close attention to the performance implications of the chosen ECS, especially regarding iteration over components and entity creation/destruction.
*   **Data-Oriented Design:** Strive to design components and systems following data-oriented principles.
*   **Scalability:** Consider how the ECS will scale as the number of entities and systems grows.
*   **Debugging:** Investigate debugging capabilities provided by the ECS library or plan for how to debug ECS-related issues.

## 5. Decisions Made

*   **ECS Library Choice:** **Flecs (SanderMertens/flecs)**
    *   **Reasoning:**
        *   **Performance:** Flecs is designed for high performance and low overhead, which is crucial for a voxel game that might manage a very large number of entities and components.
        *   **Ease of Use & Integration:** Flecs offers a C-based core with a C++ API, making it relatively straightforward to integrate. Its build system support (CMake) aligns with the project's existing setup. The API is generally considered intuitive.
        *   **Feature Set:** Flecs provides a comprehensive set of ECS features, including archetypes, queries, systems, reflection, and optional modules for things like hierarchies, snapshots, and REST APIs for debugging/inspection. This rich feature set provides flexibility for future development needs.
        *   **C++ Standard & Compatibility:** The C++ API is modern (C++11 and newer), fitting well with the project's C++17 standard.
        *   **Community & Documentation:** Flecs has an active community, good documentation, and numerous examples, which will be beneficial for learning and troubleshooting.
        *   **Modularity:** The core of Flecs is lightweight, and additional features can be opted into, allowing for a tailored approach.
        *   **Suitability for Voxel Game:** While both EnTT and Flecs are strong contenders, Flecs's emphasis on runtime flexibility, its query language, and potential for editor integration (via reflection and REST API) seem particularly well-suited for a complex game like Voxel Castle. The ability to add/remove components and systems dynamically with relative ease is a plus.
    *   **Alternative Considered:** EnTT was a strong alternative, known for its raw performance and header-only nature. However, Flecs's broader feature set out-of-the-box (like reflection, optional modules) and its slightly more declarative API style were considered more advantageous for this project's long-term goals, despite EnTT's excellent performance benchmarks. The potential for easier debugging and inspection tools with Flecs was also a factor.
*   **Custom ECS Solution:** Decided against. The development overhead and risk of introducing bugs or performance issues are too high compared to leveraging a mature, well-tested library.

---
**Last Updated:** 2025-05-12
