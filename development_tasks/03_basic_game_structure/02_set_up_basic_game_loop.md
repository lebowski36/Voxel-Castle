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

*   [x] **3.1. Design Decision: Game Loop Location and Structure**
    *   [x] 3.1.1. Decide whether to refactor the game loop directly within `main.cpp` or create a new `Game` class (e.g., in `game/src/core/game.cpp` and `game/include/core/game.h`) to encapsulate the loop and manage core systems (WindowManager, WorldManager, Renderer, Camera, etc.).
    *   [x] 3.1.2. Outline the high-level structure of the chosen approach (e.g., `Game::run()` method containing the loop, `Game::initialize()`, `Game::shutdown()`).
    *   [x] 3.1.3. Decide on the timing strategy:
        *   [x] Variable timestep with delta time for rendering and general updates.
        *   [ ] Consider if a fixed timestep loop for specific logic (e.g., physics, some game rules) is needed now or can be deferred. If needed now, outline its integration (e.g., accumulator pattern).

*   [ ] **3.2. Implement Core Timing Mechanism (Delta Time)** (This will be integrated into `Game::run()`)
    *   [ ] 3.2.1. Add variables to store `lastFrameTime` and `currentFrameTime` (e.g., using `std::chrono::steady_clock::time_point`).
    *   [ ] 3.2.2. In each iteration of the game loop, calculate `deltaTime = currentFrameTime - lastFrameTime` (e.g., using `std::chrono::duration`).
    *   [ ] 3.2.3. Update `lastFrameTime = currentFrameTime` at the end of the loop or beginning of the next.
    *   [ ] 3.2.4. Ensure time units are consistent (e.g., seconds as a float).

*   [ ] **3.3. Create `Game` Class and Migrate Logic from `main.cpp`**
    *   [ ] 3.3.1. **Define `Game` Class Skeleton**
        *   [x] 3.3.1.1. Create `game/include/core/game.h` with the `Game` class structure:
            *   Include necessary headers (forward declare where possible).
            *   Declare member variables for core components (e.g., `std::unique_ptr<Window>`, `std::unique_ptr<flecs::world>`, `std::unique_ptr<SpectatorCamera>`, etc.) and game state (e.g., `isRunning_`, `mouseCaptured_`, input flags).
            *   Declare public methods: `Game()`, `~Game()`, `bool initialize()`, `void run()`, `void shutdown()`.
            *   Declare private helper methods: `void processInput()`, `void update(float deltaTime)`, `void render()`.
        *   [x] 3.3.1.2. Create `game/src/core/game.cpp` with stub implementations for all `Game` class methods. Include `core/game.h`.
    *   [ ] 3.3.2. **Implement `Game::initialize()`** by migrating setup code from `main.cpp`:
        *   [x] 3.3.2.1. Instantiate and initialize `gameWindow_`. If failed, return `false`.
        *   [x] 3.3.2.2. Migrate OpenGL state setup (e.g., `glEnable(GL_DEPTH_TEST)`, `glCullFace`, `glPolygonMode`).
        *   [x] 3.3.2.3. Migrate ECS world creation and `MovementSystem` registration.
        *   [x] 3.3.2.4. Instantiate `worldManager_`, `textureAtlas_`, `meshBuilder_`, `meshRenderer_`.
        *   [x] 3.3.2.5. Migrate initial world content setup (e.g., `worldManager_->getOrCreateChunkColumn(0, 0)`).
        *   [x] 3.3.2.6. Migrate initial mesh build call (e.g., `worldManager_->updateDirtyMeshes(*textureAtlas_, *meshBuilder_)`).
        *   [x] 3.3.2.7. Migrate debug utilities setup (e.g., `Debug::setupDebugAtlasQuad`).
        *   [x] 3.3.2.8. Instantiate and initialize `camera_` with appropriate parameters.
        *   [x] 3.3.2.9. Migrate `glClearColor` call.
        *   [x] 3.3.2.10. Initialize `Game` class members: `isRunning_ = true`, `mouseCaptured_ = true`, input booleans (e.g., `forward_ = false`), `lastFrameTime_` (using `std::chrono::steady_clock::now()`). Return `true`.
    *   [ ] 3.3.3. **Implement `Game::processInput()`** by migrating input handling from `main.cpp`'s loop:
        *   [x] 3.3.3.1. Implement SDL event polling loop (`while (SDL_PollEvent(&e))`).
        *   [x] 3.3.3.2. Migrate key down/up event handling to update `Game` member input flags (e.g., `forward_`, `backward_`, `speedMultiplier_`, `manualVoxelChangeRequested_`).
        *   [x] 3.3.3.3. Migrate mouse capture toggle logic (Escape key) updating `mouseCaptured_` and calling `SDL_SetWindowRelativeMouseMode`.
        *   [x] 3.3.3.4. Migrate mouse motion event handling (`e.motion.xrel`, `e.motion.yrel`), storing `mouseDeltaX_` and `mouseDeltaY_` in `Game` members.
        *   [x] 3.3.3.5. Migrate window resize event handling (update `camera_->updateAspect()`, `glViewport`).
        *   [x] 3.3.3.6. Set `isRunning_ = false` upon `SDL_EVENT_QUIT`.
    *   [ ] 3.3.4. **Implement `Game::update(float deltaTime)`** by migrating update logic from `main.cpp`'s loop:
        *   [x] 3.3.4.1. If `mouseCaptured_` and mouse deltas are non-zero, call `camera_->processMouse(mouseDeltaX_, mouseDeltaY_)` and reset deltas.
        *   [x] 3.3.4.2. Call `camera_->processKeyboard(deltaTime, forward_, backward_, ...)` using `Game` member input flags.
        *   [x] 3.3.4.3. Call `ecs_->progress()` (or `ecs_->progress(deltaTime)` if applicable).
        *   [x] 3.3.4.4. Call `gameWindow_->update()` (if this is a per-frame update distinct from buffer swapping).
        *   [x] 3.3.4.5. Migrate game-specific update logic:
            *   [x] 3.3.4.5.1. Handle `manualVoxelChangeRequested_` flag: if true, perform voxel modifications using `worldManager_` and reset flag.
            *   [x] 3.3.4.5.2. Call `worldManager_->updateDirtyMeshes(*textureAtlas_, *meshBuilder_)`.
    *   [ ] 3.3.5. **Implement `Game::render()`** by migrating rendering calls from `main.cpp`'s loop:
        *   [x] 3.3.5.1. Call `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)`.
        *   [x] 3.3.5.2. Get view matrix (`camera_->getViewMatrix()`) and projection matrix (`camera_->getProjectionMatrix()`).
        *   [x] 3.3.5.3. Migrate loop for rendering segment meshes: get meshes from `worldManager_`, call `meshRenderer_->uploadMesh()` and `meshRenderer_->draw()`.
        *   [x] 3.3.5.4. Migrate debug rendering calls (e.g., `Debug::drawDebugAtlasQuad`) (commented out for now).
        *   [x] 3.3.5.5. Call `gameWindow_->render()` (typically swaps OpenGL buffers).
    *   [ ] 3.3.6. **Implement `Game::run()`**:
        *   [x] 3.3.6.1. Implement the main game loop: `while (isRunning_ && gameWindow_->isRunning())`.
        *   [x] 3.3.6.2. Calculate `currentFrameTime` using `std::chrono::steady_clock::now()`.
        *   [x] 3.3.6.3. Calculate `deltaTime` as float seconds: `std::chrono::duration<float>(currentFrameTime - lastFrameTime_).count()`.
        *   [x] 3.3.6.4. Update `lastFrameTime_ = currentFrameTime;`.
        *   [x] 3.3.6.5. Call `processInput()`.
        *   [x] 3.3.6.6. Call `update(deltaTime)`.
        *   [x] 3.3.6.7. Call `render()`.
        *   [x] 3.3.6.8. (Optional) Add frame capping or `std::this_thread::sleep_for` if needed (added a deltaTime cap and noted sleep option).
    *   [ ] 3.3.7. **Implement `Game::shutdown()`** by migrating cleanup code from `main.cpp`:
        *   [ ] 3.3.7.1. Call `Debug::cleanupDebugQuads()`.
        *   [ ] 3.3.7.2. Call `gameWindow_->cleanUp()`.
        *   [ ] 3.3.7.3. Other cleanup if necessary (most resources managed by `std::unique_ptr` will auto-cleanup).

*   [ ] **3.4. Integrate `Game` Class into `main.cpp`**
    *   [ ] 3.4.1. Modify `main.cpp` to include `core/game.h`.
    *   [ ] 3.4.2. In `main()`, remove all migrated initialization, game loop, and cleanup logic.
    *   [ ] 3.4.3. In `main()`, create an instance of the `Game` class: `Game game;`.
    *   [ ] 3.4.4. Call `if (!game.initialize()) { return -1; }`.
    *   [ ] 3.4.5. Call `game.run();`.
    *   [ ] 3.4.6. Call `game.shutdown();`.
    *   [ ] 3.4.7. Return `0`.

*   [ ] **3.5. (Optional, if decided in 3.1.3) Implement Fixed Update Loop**
    *   [ ] 3.5.1. Implement an accumulator for `deltaTime` in `Game::run()`.
    *   [ ] 3.5.2. Define a `FIXED_TIMESTEP` constant in `Game` class (e.g., 1.0f / 60.0f).
    *   [ ] 3.5.3. In `Game::run()`, before the variable `update()`, add a loop: `while (accumulator >= FIXED_TIMESTEP)`:
        *   Call a new `fixedUpdate(FIXED_TIMESTEP)` method.
        *   Decrement `accumulator -= FIXED_TIMESTEP`.
    *   [ ] 3.5.4. Create `Game::fixedUpdate(float fixedDeltaTime)` method and identify any current logic that should move to it.

*   [ ] **3.6. Testing and Verification**
    *   [ ] 3.6.1. Build the project successfully.
    *   [ ] 3.6.2. Verify that the application still runs, events are processed, camera works, and rendering occurs as before.
    *   [ ] 3.6.3. Test that camera movement speed is now consistent regardless of frame rate fluctuations (due to `deltaTime` usage).
    *   [ ] 3.6.4. If fixed updates are implemented, verify they are occurring at the intended rate (e.g., via logging).

## 4. Notes & Considerations
*   **Initial Scope:** The primary goal is a functional loop with delta time. A full `Game` class structure is good practice but could be a larger refactor. We can start by improving `main.cpp` and then decide if a `Game` class is the immediate next step or part of a later refinement.
*   **System Access:** If a `Game` class is used, consider how it will provide access to core systems (e.g., renderer, input manager, world manager) for the `update` and `render` methods, or if those systems will be passed around.
*   The existing sub-tasks from `CURRENT_TODO.md` are incorporated and expanded here.

---
**Last Updated:** 2025-05-13
