# Task: Set Up Rendering API (OpenGL with SDL3)

**High-Level Goal:** Integrate OpenGL with the existing SDL3 window to establish a basic 3D rendering pipeline.

**Parent Task:** [1. Core Engine Foundation (C++)](../../CURRENT_TODO.md)

## 📝 Sub-Tasks:

1.  **Configure SDL3 for OpenGL Context:**
    *   [x] Set SDL hints to specify OpenGL version (e.g., OpenGL 3.3 or higher, core profile).
        *   `SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);`
        *   `SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);`
        *   `SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);`
    *   [x] Modify `Window::init()` in `engine/src/platform/Window.cpp`.
    *   [x] Ensure the `SDL_WINDOW_OPENGL` flag is used when creating the SDL window in `Window::init()`.

2.  **Create OpenGL Context:**
    *   [x] Create an OpenGL context using `SDL_GL_CreateContext(sdlWindow)`.
    *   [x] Store the `SDL_GLContext` in the `Window` class.
    *   [x] Add error checking for context creation.
    *   [x] Modify `Window::init()` and `Window.h`.

3.  **Load OpenGL Functions:**
    *   [x] Choose and integrate an OpenGL loading library (e.g., GLAD, GLEW). GLAD is generally preferred for modern OpenGL.
    *   [x] **Decision Point:** We will start with GLAD. If issues arise, we can re-evaluate.
    *   [x] Generate GLAD files for the chosen OpenGL version (e.g., 3.3 core, with KHR_debug extension).
    *   [x] Add GLAD source/header files to the project (`external/glad/src/glad.c`, `external/glad/include/{glad/,KHR/}`).
    *   [x] Update CMake (`engine/CMakeLists.txt` and root `CMakeLists.txt`, plus `external/glad/CMakeLists.txt`) to include GLAD source files and necessary include directories.
    *   [x] Initialize GLAD after the OpenGL context is made current: `gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);`
    *   [x] Add error checking for GLAD initialization.

4.  **Initial OpenGL Setup:**
    *   [x] Set the viewport using `glViewport(0, 0, windowWidth, windowHeight);` in `Window::init()`.
    *   [x] Set a clear color using `glClearColor(R, G, B, A);` (e.g., a distinct color like blue or cornflower blue to verify OpenGL is working).
    *   [x] Modify the `Window::render()` method:
        *   [x] Remove `SDL_SetRenderDrawColor` and `SDL_RenderClear` for the `sdlRenderer`.
        *   [x] Clear the screen using `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);`.
        *   [x] Swap the window buffer using `SDL_GL_SwapWindow(sdlWindow);`.

5.  **Update CMake for OpenGL:**
    *   [x] Use `find_package(OpenGL REQUIRED)` in the main `CMakeLists.txt`.
    *   [x] Link against OpenGL libraries: `target_link_libraries(VoxelEngine PRIVATE OpenGL::GL)`.

6.  **Clean Up SDL Renderer (If no longer needed):**
    *   [x] If SDL's 2D rendering capabilities (`SDL_Renderer`) are not going to be used alongside OpenGL, remove its creation (`SDL_CreateRenderer`) and destruction (`SDL_DestroyRenderer`).
    *   [x] Remove the `sdlRenderer` member from the `Window` class.
    *   [x] **Decision:** For now, we will assume we are moving fully to OpenGL for rendering within the main window.

7.  **Testing:**
    *   [ ] Compile and run the application.
    *   [ ] Verify that the window appears and is cleared to the color set by `glClearColor`.
    *   [ ] Verify OpenGL version, GLSL version, Vendor, and Renderer are printed to console.

## ✅ Completion Criteria:
- The application compiles without errors.
- The application runs and displays a window.
- The window background is cleared to the color specified by `glClearColor` (e.g., dark blue).
- `SDL_GL_SwapWindow` is used for rendering.
- `SDL_Renderer` is no longer used for the main window rendering.
- GLAD is successfully loading OpenGL functions.
- OpenGL context is successfully created for the specified version (3.3 Core).
- Information about the OpenGL context (Version, GLSL Version, Vendor, Renderer) is printed to the console on startup.

## 🪵 Log / Notes:
- 2025-05-11: Configured SDL for OpenGL, created context, integrated GLAD, updated CMake. Window.cpp and Window.h modified. CMakeLists.txt files updated. GLAD files added to `external/glad`.
