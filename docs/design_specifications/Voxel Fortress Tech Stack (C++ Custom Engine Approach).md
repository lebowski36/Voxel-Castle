# Voxel Fortress Tech Stack (C++ Custom Engine Approach)

## 0. Overview
This document provides a high-level overview of the technology stack for Voxel Fortress, which is centered around a custom game engine built primarily with C++. For more detailed technical breakdowns of specific engine components and systems, please refer to the specialized documents located in the [`org/Tech Stack/`](./Tech%20Stack/) subdirectory.

## 1. Core Philosophy

This document outlines the proposed technology stack for Voxel Fortress, centered around a custom game engine built primarily with C++. The choice of C++ is driven by the need for maximum performance, low-level system control, and access to a mature ecosystem of libraries and tools essential for a project of this scale and complexity. We aim to build a highly optimized, multithreaded engine capable of handling vast voxel worlds, complex AI simulations, and advanced rendering techniques.

## 1.A. Guiding Principles for Technology Selection

The selection of specific technologies, libraries, and architectural patterns for the C++ engine is guided by the following core principles, ensuring alignment with the project's ambitious goals:

*   **Performance Priority:** Technologies must enable the highest possible performance, especially for rendering vast environments, simulating numerous entities, and managing complex physics. Low-level control and optimization capabilities are paramount.
*   **Integration & Interoperability:** Chosen components should integrate well within a custom C++ architecture. APIs should be clear, and the effort required to combine different systems should be manageable.
*   **Maintainability & Scalability:** The tech stack must support long-term development. This includes considering the learning curve, code clarity, ease of debugging, and the ability to scale systems as game complexity grows.
*   **Ecosystem & Community Support:** Preference is given to technologies with active development, robust documentation, and strong community support (e.g., forums, readily available expertise) to aid in problem-solving and future development.
*   **Flexibility & Extensibility:** The engine needs to be adaptable. Technologies should allow for customization and extension to meet the unique requirements of Voxel Fortress, rather than imposing rigid constraints.

## 2. Core Language & Build System

*   **Programming Language:** C++ (targeting C++17 or C++20 for modern features and compatibility).
    *   **Rationale:** Offers unparalleled performance, direct memory management, and a vast ecosystem. Modern C++ features enhance safety and developer productivity.
*   **Build System:** CMake
    *   **Rationale:** Cross-platform, widely adopted, and well-supported by IDEs and C++ communities. Manages complex dependencies and build configurations effectively.
*   **Compiler:** Clang or GCC on Linux/macOS, MSVC on Windows.
    *   **Rationale:** Adherence to C++ standards, excellent optimization capabilities, and good tooling support.

## 3. Rendering

*   **Graphics API:** Vulkan
    *   **Rationale:** Provides explicit, low-level control over the GPU, essential for achieving high performance with vast view distances and complex voxel rendering. Enables advanced techniques like multi-threaded command buffer generation. Offers better performance characteristics on modern hardware compared to OpenGL for demanding applications.
*   **Shader Language:** GLSL (compiled to SPIR-V for Vulkan)
*   **Windowing & Input:** SDL2 (Simple DirectMedia Layer) or GLFW
*   **Mesh Generation:** Custom algorithms for voxel meshing.
*   **LOD System:** Custom implementation.

> For a detailed breakdown of the rendering pipeline, including specific algorithms, data flows, and optimization strategies, see:
> - [**Rendering Pipeline Details**](./Tech%20Stack/Rendering%20Pipeline.md)

## 4. Physics

*   **Physics Engine:** Potential use of libraries like Jolt Physics, Bullet Physics SDK, or PhysX, or a custom-tailored solution.
    *   **Rationale:** Chosen for maturity, feature set, and performance characteristics suitable for 3D rigid body dynamics and collision detection.
*   **Voxel Physics:** Custom solutions for block stability, cave-ins, and basic fluid dynamics.

> For a detailed breakdown of the physics system architecture, integration with voxel data, and specific challenges, see:
> - [**Physics System Details**](./Tech%20Stack/Physics%20System.md)

## 5. Entity Component System (ECS)

*   **ECS Framework:** Flecs (https://github.com/SanderMertens/flecs)
    *   **Rationale:** Flecs is a fast and lightweight C++11 Entity Component System. It was chosen for its high performance, small footprint, extensive feature set (including hierarchies, prefabs, systems, queries, and a reflection module), excellent documentation, and active development. Its data-oriented design aligns well with the performance goals of Voxel Fortress, maximizing cache efficiency and enabling parallel processing for managing game entities and their behaviors.
    *   **Integration:** Added as a git submodule in `external/flecs` and integrated into the CMake build system. Core game logic will leverage Flecs for entity management, component definition, and system processing.

> For a detailed discussion of the ECS architecture, design patterns, and integration with other engine systems, see:
> - [**Entity Component System Details**](./Tech%20Stack/Entity%20Component%20System.md)

## 6. Artificial Intelligence

*   **Core AI Systems:** Behavior Trees, Utility AI, Pathfinding (A*, HPA*, JPS variants) adapted for 3D voxel environments.
*   **Advanced Features:** Potential for GOAP, group AI, and sophisticated NPC decision-making.

> For an in-depth look at the AI architecture, including behavior systems, pathfinding algorithms, knowledge representation, and C++ implementation strategies, see:
> - [**AI Architecture Details**](./Tech%20Stack/AI%20Architecture.md)

## 7. World Generation & Management

*   **Procedural Generation:** Noise algorithms (Perlin, Simplex, etc.) for terrain, biomes, and resource placement.
*   **Voxel Data Structures:** Optimized structures for storing and querying vast voxel worlds (e.g., chunk-based systems, octrees, spatial hashing).
    *   **Rationale:** Essential for managing the immense scale of the game world, supporting efficient modifications, and enabling fast spatial queries.
*   **Streaming:** Asynchronous loading/unloading of world chunks.

> For detailed information on how voxel data is structured, stored, accessed, and managed, including compression techniques and spatial indexing, see:
> - [**Voxel Data Structures Details**](./Tech%20Stack/Voxel%20Data%20Structures.md)

## 8. Mathematics Library

*   **Library:** GLM (OpenGL Mathematics)
    *   **Rationale:** GLM is a header-only C++ mathematics library based on the OpenGL Shading Language (GLSL) specifications. It was chosen for its ease of integration (header-only), its compatibility with OpenGL and Vulkan workflows, its GLSL-like syntax (which is beneficial for shader development), and its comprehensive set of mathematical functions and types (vectors, matrices, quaternions, transformations).
    *   **Integration:** Added to the project under `external/glm` and included in the CMake build process for the engine.

## Engine Architecture (C++ Custom Engine)

The Voxel Fortress engine will be custom-built in C++ to achieve the specific performance and feature requirements of the game. This includes managing vast procedurally generated voxel worlds, simulating a large number of entities, and supporting advanced rendering techniques.

> For a comprehensive overview of the engine's architecture, including its core C++ principles, system layers, and key C++ considerations for development, see:
> - [**Engine Architecture Details**](./Tech%20Stack/Engine%20Architecture.md)

### Architectural Principles (C++ Focus)

*   **Data-Oriented Design (DOD):** This will be a core principle. C++ allows for fine-grained memory layout control (structs, arrays of structs vs. structs of arrays). We will prioritize cache coherency by organizing data for optimal access patterns, separating data (in components) from behavior (in systems).
*   **Parallelism by Default:** Leveraging C++11/14/17/20+ threading capabilities (`std::thread`, `std::async`, `std::mutex`, `std::atomic`) and potentially a task-based job system (e.g., custom-built, or libraries like Intel TBB, EnTT's scheduler, or inspiration from existing engine job systems). The goal is to distribute work (physics, AI, rendering sub-tasks, world generation) across available CPU cores efficiently.
*   **Modularity with Clear Boundaries:** Systems (Rendering, Physics, AI, UI, etc.) will be designed as distinct modules with well-defined C++ APIs/interfaces. This promotes maintainability and allows for easier testing and potential future replacement of modules.
*   **Optimization-Friendly:** The C++ implementation will facilitate profiling (e.g., integrating with tools like Tracy, Optick, or custom instrumentation). Performance budgets will be considered for critical systems. Hot-reloading for assets (shaders, scripts if used) will be aimed for to speed up iteration.

### Core Systems Architecture (C++ Implementation)

*   **Central Engine Coordinator/Loop:** A main C++ class will manage the game loop, timing (fixed update for simulation, variable for rendering), system initialization/shutdown, and overall state.
*   **Layered Design:**
    1.  **Foundation Layer (C++):** Custom memory allocators (arena, pool, stack for specific use cases), core math libraries (e.g., GLM or custom SIMD-optimized math), logging, platform abstraction (windowing, input via libraries like SDL2 or GLFW, or direct platform APIs).
    2.  **Systems Layer (C++):**
        *   **Entity Component System (ECS):** Flecs, a high-performance C++ ECS (integrated via `external/flecs`), will be the backbone for game objects and their behaviors.
        *   **Rendering System:** See dedicated Rendering Pipeline section.
        *   **Physics Engine:** See dedicated Physics System section.
        *   **Input Management:** Mapping raw input to game actions.
        *   **Audio System:** Using libraries like OpenAL, FMOD, or Wwise (if budget allows) for 3D audio, effects, and music.
    3.  **Game Layer (C++):** Voxel world management (chunking, streaming, modification), simulation systems (colony management, AI), game logic, UI implementation.
    4.  **Application Layer (C++):** Game states (menus, gameplay, loading screens), asset management pipeline, save/load functionality.

### Key C++ Considerations for Engine Development

*   **Memory Management:**
    *   **Allocators:** Beyond standard `new`/`delete`, custom allocators (e.g., linear/arena for frame-local data, pool for frequently allocated/deallocated objects of the same size) will be crucial for performance and reducing fragmentation.
    *   **Ownership & Lifecycles:** Smart pointers (`std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr`) will be used judiciously. Clear ownership models for dynamically allocated resources are essential.
    *   **Data Structures:** Choosing appropriate standard library containers (`std::vector`, `std::unordered_map`, `std::map`, etc.) or implementing custom cache-friendly data structures where necessary.
*   **Multithreading & Concurrency:**
    *   **Job System:** A task-based job system is highly recommended for distributing work like voxel mesh generation, pathfinding, AI updates, and some rendering tasks.
    *   **Synchronization:** Careful use of mutexes, condition variables, atomics, and potentially lock-free data structures to manage shared data access between threads.
    *   **Thread Pools:** Managing a pool of worker threads to execute jobs.
*   **Platform Abstraction:** Abstracting platform-specific functionalities (windowing, input, file I/O) to allow for potential future portability and cleaner code. Libraries like SDL2 or GLFW can assist here for windowing and input.
*   **Build System & Dependencies:** CMake is a standard choice for C++ projects. Managing external dependencies (libraries) effectively (e.g., via Conan, vcpkg, or git submodules).
*   **Compilation & Toolchain:** Choosing a modern C++ compiler (GCC, Clang, MSVC) and leveraging its optimization capabilities. Utilizing static analysis tools and sanitizers (address, thread, undefined behavior) during development.
*   **Debugging & Profiling Tools:** Essential for C++ development. Integrated debuggers (GDB, LLDB, Visual Studio Debugger), and profilers (Tracy, Perf, VTune, Optick) will be used.

This C++ engine architecture will provide a robust and performant foundation for Voxel Fortress.

## Key Technical Challenges

Developing Voxel Fortress with a custom C++ engine presents several significant technical hurdles that will require careful planning and innovative solutions:

*   **Rendering Performance at Scale:** Achieving and maintaining high frame rates while rendering vast voxel landscapes with extreme view distances and seamless Level of Detail (LOD) transitions is a primary challenge. This involves optimizing mesh generation, culling, batching, and shader performance.
*   **Memory Management:** The sheer volume of data for a large, dynamic voxel world (terrain, entities, game state) necessitates sophisticated memory management strategies to minimize footprint and prevent fragmentation, especially with C++'s manual memory handling.
*   **Efficient Parallelism:** Effectively utilizing multi-core processors for simulation (AI, physics), rendering (command buffer generation), and world generation/streaming is crucial. Designing thread-safe systems and avoiding contention will be complex.
*   **Complex AI in Dynamic Environments:** Implementing intelligent and performant AI for hundreds of colonists and other entities, including robust pathfinding in a fully destructible 3D voxel world, is a major undertaking.
*   **Custom UI System:** Developing a flexible, performant, and aesthetically pleasing UI system that can handle both complex game management screens and immersive in-game elements requires significant effort.
*   **Data Persistence and Streaming:** Efficiently saving, loading, and streaming massive world states and game data without causing hitches or excessive load times.
*   **Cross-Platform Consistency:** Ensuring stable performance and behavior across different target platforms (e.g., Windows, Linux) if pursued, while managing platform-specific APIs and compiler differences.

## High-Level Technical Implementation Phases

The development of the C++ engine and game is envisioned to proceed through several high-level technical phases, allowing for iterative development and milestone tracking:

1.  **Phase 1: Core Engine Foundation**
    *   Establishment of the C++ project structure, build system (CMake), and version control.
    *   Basic window creation, input handling (keyboard, mouse).
    *   Initial Vulkan rendering pipeline setup: clear screen, basic primitive rendering.
    *   Simple voxel data representation and rendering (e.g., a single chunk of solid blocks).
    *   Foundation for the Entity Component System (ECS).
    *   Basic physics integration (e.g., simple collision shapes, gravity).

2.  **Phase 2: Core Simulation Systems**
    *   Advanced voxel data structures (chunking, storage optimization).
    *   Procedural world generation basics (heightmaps, simple biomes).
    *   Chunk loading, unloading, and streaming.
    *   Expansion of the ECS with core components for colonists/entities.
    *   Initial AI framework: basic pathfinding (e.g., A* on a 2D projection or simplified 3D grid), simple behavior logic (e.g., idle, move).
    *   Basic resource management concepts.

3.  **Phase 3: Advanced Rendering & Interaction**
    *   Implementation of the multi-tiered Level of Detail (LOD) system for terrain.
    *   Optimized voxel meshing algorithms (e.g., greedy meshing).
    *   Lighting and shading models (e.g., ambient occlusion, shadows).
    *   Player character representation and direct control mechanics.
    *   Indirect colony management interface basics (UI placeholders).
    *   More complex physics interactions (e.g., block destruction, basic fluid stubs).

4.  **Phase 4: Gameplay Systems & Polish**
    *   Detailed implementation of core gameplay loops: needs/moods, skills/professions, building/logistics, threats/challenges.
    *   Advanced AI behaviors, social interactions, and faction logic.
    *   Full UI implementation with data visualization.
    *   Audio system integration.
    *   Comprehensive performance optimization across all systems (CPU, GPU, memory).
    *   Persistence (saving/loading game states).
    *   Extensive testing, bug fixing, and balancing.

These phases are iterative and may overlap. The focus will be on building functional vertical slices to test and refine systems progressively.

## 14. Development Tools & Practices

*   **Version Control:** Git
*   **IDE:** Visual Studio Code, CLion, or Visual Studio (on Windows).
*   **Debugging:** GDB, LLDB, Visual Studio Debugger. Profiling tools like Valgrind, Perf, Tracy Profiler, or platform-specific profilers.
*   **Testing:** Unit testing frameworks (e.g., Google Test, Catch2) and integration testing.
*   **Continuous Integration (CI):** GitHub Actions, GitLab CI, or Jenkins for automated builds and tests.

This tech stack provides a robust foundation for developing Voxel Fortress, balancing performance, control, and access to a rich ecosystem of tools and libraries. The custom engine approach allows for deep optimization tailored to the unique demands of the game.
