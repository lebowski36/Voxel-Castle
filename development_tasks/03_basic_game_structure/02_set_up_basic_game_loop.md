# Task: Set Up Basic Game Loop

**Date Created:** 2025-05-12
**Status:** In Progress
**Depends On:** Window Management, Basic Rendering Setup
**Leads To:** Game Logic Execution, Rendering Synchronization, Frame-Rate Independent Updates

## 1. Overview
This task involves implementing the fundamental game loop, which will drive game updates and rendering with proper timing mechanisms. The goal is to refactor the existing loop in `main.cpp` or encapsulate it within a new `Game` class to provide a more structured and robust foundation for future development.

## 2. Goals
*   Implement a game loop with distinct input, update, and render phases.
*   Implement delta time calculation for frame-rate independent updates and movements.
*   Consider and potentially implement a fixed timestep for physics-sensitive or critical game logic (though initial focus might be variable timestep for most things).
*   Improve the organization of the main application flow.

## 3. Sub-Tasks

*   [ ] **3.1. Design Decision: Game Loop Location and Structure**
    *   [ ] 3.1.1. Decide whether to refactor the game loop directly within `main.cpp` or create a new `Game` class (e.g., in `game/src/core/game.cpp` and `game/include/core/game.h`) to encapsulate the loop and manage core systems (WindowManager, WorldManager, Renderer, Camera, etc.).
    *   [ ] 3.1.2. Outline the high-level structure of the chosen approach (e.g., `Game::run()` method containing the loop, `Game::initialize()`, `Game::shutdown()`).
    *   [ ] 3.1.3. Decide on the timing strategy:
        *   Variable timestep with delta time for rendering and general updates.
        *   Consider if a fixed timestep loop for specific logic (e.g., physics, some game rules) is needed now or can be deferred. If needed now, outline its integration (e.g., accumulator pattern).

*   [ ] **3.2. Implement Core Timing Mechanism (Delta Time)**
    *   [ ] 3.2.1. Add variables to store `lastFrameTime` and `currentFrameTime`.
    *   [ ] 3.2.2. In each iteration of the game loop, calculate `deltaTime = currentFrameTime - lastFrameTime`.
    *   [ ] 3.2.3. Update `lastFrameTime = currentFrameTime` at the end of the loop or beginning of the next.
    *   [ ] 3.2.4. Ensure time units are consistent (e.g., seconds).

*   [ ] **3.3. Refactor/Implement Main Loop Phases**
    *   [ ] 3.3.1. **Input Processing Phase:** Ensure all SDL event processing is handled cleanly at the start of the loop.
    *   [ ] 3.3.2. **Update Phase:**
        *   Create a distinct `update(float deltaTime)` function/method.
        *   Move existing update logic (e.g., camera updates, `worldManager.updateDirtyMeshes()`) into this phase.
        *   Modify time-sensitive updates (e.g., camera movement speed) to use `deltaTime`.
    *   [ ] 3.3.3. **Render Phase:**
        *   Create a distinct `render()` function/method.
        *   Ensure all rendering calls are within this phase.

*   [ ] **3.4. (If creating `Game` class) Integrate `Game` Class into `main.cpp`**
    *   [ ] 3.4.1. In `main()`, create an instance of the `Game` class.
    *   [ ] 3.4.2. Call `game.initialize()`.
    *   [ ] 3.4.3. Call `game.run()` to start the game loop.
    *   [ ] 3.4.4. Call `game.shutdown()` after the loop exits.

*   [ ] **3.5. (Optional, if decided in 3.1.3) Implement Fixed Update Loop**
    *   [ ] 3.5.1. Implement an accumulator for `deltaTime`.
    *   [ ] 3.5.2. Define a `FIXED_TIMESTEP` (e.g., 1.0f / 60.0f for 60 updates per second).
    *   [ ] 3.5.3. In the main loop, while `accumulator >= FIXED_TIMESTEP`:
        *   Call a `fixedUpdate(FIXED_TIMESTEP)` function/method.
        *   Decrement `accumulator -= FIXED_TIMESTEP`.
    *   [ ] 3.5.4. Identify any current logic that should move to `fixedUpdate()`.

*   [ ] **3.6. Testing and Verification**
    *   [ ] 3.6.1. Verify that the application still runs, events are processed, and rendering occurs.
    *   [ ] 3.6.2. Test that camera movement speed is now consistent regardless of frame rate fluctuations (if possible to simulate, or by observing smoothness).
    *   [ ] 3.6.3. If fixed updates are implemented, verify they are occurring at the intended rate (e.g., via logging).

## 4. Notes & Considerations
*   **Initial Scope:** The primary goal is a functional loop with delta time. A full `Game` class structure is good practice but could be a larger refactor. We can start by improving `main.cpp` and then decide if a `Game` class is the immediate next step or part of a later refinement.
*   **System Access:** If a `Game` class is used, consider how it will provide access to core systems (e.g., renderer, input manager, world manager) for the `update` and `render` methods, or if those systems will be passed around.
*   The existing sub-tasks from `CURRENT_TODO.md` are incorporated and expanded here.

---
**Last Updated:** 2025-05-13
