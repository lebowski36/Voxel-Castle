# Task: Implement Window Management

**Objective:** Integrate a library for window creation, event handling, and input, and set up the basic window for Voxel Fortress.

**Parent Task:** [Core Engine Foundation (C++)](../CURRENT_TODO.md)

## 1. Library Selection (Decision Required)

We need to choose a cross-platform library to handle windowing, OpenGL/Vulkan context creation, and input. Common choices for C++ game development include:

*   **SDL (Simple DirectMedia Layer):**
    *   **Pros:** Widely used, mature, good community support, handles windowing, input, audio, and more. Excellent for OpenGL context creation. Generally considered robust.
    *   **Cons:** Can be a bit C-style in its API, though it has C++ wrappers/idioms.
*   **SFML (Simple and Fast Multimedia Library):**
    *   **Pros:** Object-oriented C++ API, easier to get started with for some, handles windowing, graphics (its own drawing API, but also good for OpenGL), audio, and networking.
    *   **Cons:** Might be slightly less performant for very high-end graphics needs compared to a raw SDL+OpenGL/Vulkan setup by some accounts, but generally very capable.
*   **GLFW:**
    *   **Pros:** More lightweight, focused specifically on OpenGL/Vulkan context creation, windowing, and input. Good if you want a minimal layer.
    *   **Cons:** Less feature-rich than SDL or SFML (e.g., no built-in audio). You'd need other libraries for other functionalities.
*   **Platform-Specific APIs (e.g., Win32 API, X11, Cocoa):**
    *   **Pros:** Maximum control, potentially highest performance if optimized correctly.
    *   **Cons:** Not cross-platform (requires separate implementations for each OS), significantly more complex and time-consuming to implement and maintain.

**Recommendation:** For a custom engine aiming for cross-platform support and good integration with graphics APIs like OpenGL (which we'll likely set up next), **SDL** or **SFML** are strong contenders. SDL is often favored for custom engines due to its robustness and control, while SFML offers a more modern C++ API.

**ACTION REQUIRED (User): Please choose which library you'd prefer to use (e.g., SDL, SFML, GLFW).**

**Note on SDL Versions:** We are currently attempting to use SDL3. If significant issues persist that cannot be resolved in a timely manner, we will consider falling back to the more established SDL2 library.

## 2. Library Integration

Once a library is chosen, we will:

*   **Download/Install:**
    *   Fetch the development libraries (headers and binaries) for the chosen library.
    *   Decide on a method for inclusion: system-wide install, or preferably, vendoring it within the `external/` directory of our project for better portability and consistent builds.
    *   If vendoring, we'll add it as a Git submodule or copy the necessary files into `external/[library_name]/`.
*   **CMake Configuration:**
    *   Update the root `CMakeLists.txt` and/or `engine/CMakeLists.txt` to:
        *   Find the chosen library (e.g., `find_package(SDL2 REQUIRED)`).
        *   Link the `VoxelEngine` library against it (`target_link_libraries(VoxelEngine PRIVATE SDL2::SDL2)` or similar).
        *   Include necessary directories.

## 3. Window Class/Module Implementation (in `engine/src/platform/` or `engine/src/core/`)

We will create a basic window management system, likely encapsulated in a class (e.g., `Window.h`, `Window.cpp`).

*   **Header (`engine/include/core/Window.h` or `engine/include/platform/Window.h`):**
    *   Define the `Window` class interface.
    *   Member variables for window dimensions, title, window handle (e.g., `SDL_Window*`).
    *   Methods for:
        *   `create(title, width, height, flags)`
        *   `destroy()`
        *   `handleEvents()` (or a way to poll/process events)
        *   `swapBuffers()` (if using double buffering with OpenGL)
        *   `isOpen()`
        *   Getters for width, height, native handle.
*   **Source (`engine/src/core/Window.cpp` or `engine/src/platform/Window.cpp`):**
    *   Implement the `Window` class methods using the chosen library's API.
    *   Initialization of the library (e.g., `SDL_Init(SDL_INIT_VIDEO)`).
    *   Window creation (e.g., `SDL_CreateWindow`).
    *   Error handling.
    *   Cleanup (e.g., `SDL_DestroyWindow`, `SDL_Quit`).

## 4. Basic Event Loop (in `game/src/main.cpp` initially, later in an `Application` class)

*   Modify `game/src/main.cpp` to:
    *   Include the new `Window` header.
    *   Create a `Window` object.
    *   Implement a simple game loop:
        ```cpp
        // Example structure
        Window window;
        if (window.create("Voxel Fortress", 1280, 720)) {
            while (window.isOpen()) {
                window.handleEvents(); // Process input, window events
                // Update game logic (placeholder)
                // Render scene (placeholder)
                window.swapBuffers(); // If applicable
            }
            window.destroy();
        }
        ```

## 5. Basic Input Handling

*   Within the `window.handleEvents()` method or a dedicated input manager:
    *   Poll for events (e.g., `SDL_PollEvent`).
    *   Handle basic events like:
        *   Window close event (e.g., `SDL_QUIT`, clicking the X button).
        *   Keyboard events (e.g., `SDL_KEYDOWN`, `SDL_KEYUP` for Esc to close).
        *   Mouse events (placeholder for now, but structure to handle them).
    *   Update the `isOpen()` state of the window based on these events.

## 6. Testing

*   Compile and run the `VoxelFortressGame` executable.
*   Verify:
    *   A window appears with the correct title and dimensions.
    *   The window remains open.
    *   The window can be closed by clicking the X button.
    *   The window can be closed by pressing the Escape key (if implemented).

## Considerations:

*   **Error Handling:** Robust error checking at each step (library initialization, window creation).
*   **Configuration:** Allow window title, dimensions, and flags (fullscreen, resizable, VSync) to be configurable, perhaps later via a config file or settings object.
*   **Abstraction:** The `Window` class should abstract the underlying library as much as possible, so if we ever needed to switch libraries, the changes would be mostly contained within this class.
*   **Graphics Context:** This task focuses on windowing. The next task will deal with creating an OpenGL/Vulkan context within this window.

**Next Steps (after user decision on library):**
1.  Proceed with Library Integration (Step 2).
2.  Implement Window Class (Step 3).
3.  Set up Basic Event Loop (Step 4).
4.  Implement Basic Input Handling (Step 5).
5.  Test (Step 6).
