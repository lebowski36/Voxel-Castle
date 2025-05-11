# Task: Set Up C++ Project Structure

**Objective:** Establish the foundational directory structure and build system configuration for the Voxel Fortress game and engine.

## 1. Define Directory Structure

Create the following top-level directories in the project root (`/home/system-x1/Projects/Voxel Castle/`):

*   `assets/`: For game assets (models, textures, sounds, fonts, etc.).
    *   `assets/models/`
    *   `assets/textures/`
    *   `assets/audio/`
    *   `assets/fonts/`
    *   `assets/shaders/`
*   `build/`: For CMake generated build files and compiled binaries. (This will be added to `.gitignore`).
*   `docs/`: For general project documentation (design documents are in `org/`).
*   `engine/`: For the custom C++ game engine source code.
    *   `engine/src/`
        *   `engine/src/core/`
        *   `engine/src/rendering/`
        *   `engine/src/physics/`
        *   `engine/src/audio/`
        *   `engine/src/input/`
        *   `engine/src/platform/`
    *   `engine/include/` (for public engine headers)
    *   `engine/tests/`
*   `external/`: For third-party libraries and dependencies (e.g., SDL, GLM, stb_image).
    *   Each library should have its own subdirectory (e.g., `external/glm/`, `external/spdlog/`).
*   `game/`: For the Voxel Fortress game-specific logic.
    *   `game/src/`
        *   `game/src/main.cpp` (entry point)
        *   `game/src/gameplay/`
        *   `game/src/world/`
        *   `game/src/ui/`
    *   `game/include/` (for public game headers)
    *   `game/tests/`
*   `scripts/`: For utility scripts (e.g., build scripts, asset processing).
*   `tests/`: For integration tests or tests spanning multiple modules (unit tests are within `engine/tests/` and `game/tests/`).

## 2. Configure CMake Build System

### 2.1. Root `CMakeLists.txt`

Create `/home/system-x1/Projects/Voxel Castle/CMakeLists.txt` with the following initial content:

```cmake
cmake_minimum_required(VERSION 3.16) # Or a newer version if needed
project(VoxelFortress VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # Avoid compiler-specific extensions

# Compiler warnings (adjust as needed for your compiler)
if(MSVC)
    add_compile_options(/W4 /WX) # Treat warnings as errors
else() # GCC/Clang
    add_compile_options(-Wall -Wextra -Wpedantic -Werror) # Treat warnings as errors
endif()

# Output directories for executables and libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/engine/include
    ${PROJECT_SOURCE_DIR}/game/include
    # Add paths to external library include directories here
    # e.g., ${PROJECT_SOURCE_DIR}/external/glm
)

# Add subdirectories for engine and game
add_subdirectory(engine)
add_subdirectory(game)

# Add top-level tests if any
# add_subdirectory(tests) # If you have tests in the root tests/ directory

# Enable CTest for testing
enable_testing()

message(STATUS "CMake Project: VoxelFortress")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}") # Will be empty if not set, e.g. "Debug", "Release"
message(STATUS "Binary Dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Source Dir: ${PROJECT_SOURCE_DIR}")
```

### 2.2. Engine `CMakeLists.txt`

Create `/home/system-x1/Projects/Voxel Castle/engine/CMakeLists.txt`:

```cmake
# engine/CMakeLists.txt

# Define the engine library
add_library(VoxelEngine STATIC # or SHARED
    # List all engine source files here
    # Example:
    # src/core/application.cpp
    # src/rendering/renderer.cpp
    # ...
)

# Public include directories for the engine library
target_include_directories(VoxelEngine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include> # For installing headers
)

# Link dependencies if any (e.g., external libraries used by the engine)
# target_link_libraries(VoxelEngine PRIVATE SomeExternalLib)

# Add engine tests
# if(BUILD_TESTING) # Check if CTest is enabled
#   add_executable(EngineTests
#       tests/main_test.cpp
#       # ... other test files
#   )
#   target_link_libraries(EngineTests PRIVATE VoxelEngine gtest gtest_main) # Example with GTest
#   add_test(NAME EngineTests COMMAND EngineTests)
# endif()

message(STATUS "Configuring VoxelEngine library...")
```

### 2.3. Game `CMakeLists.txt`

Create `/home/system-x1/Projects/Voxel Castle/game/CMakeLists.txt`:

```cmake
# game/CMakeLists.txt

# Define the game executable
add_executable(VoxelFortressGame
    # List all game source files here
    src/main.cpp
    # Example:
    # src/gameplay/player.cpp
    # ...
)

# Link the game against the engine library
target_link_libraries(VoxelFortressGame PRIVATE VoxelEngine)

# Link other dependencies if any (e.g., external libraries used directly by the game)
# target_link_libraries(VoxelFortressGame PRIVATE SomeExternalLibForGame)

# Public include directories for the game (if any, usually not needed for executable)
# target_include_directories(VoxelFortressGame PUBLIC
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#    $<INSTALL_INTERFACE:include>
# )

# Add game tests
# if(BUILD_TESTING)
#   add_executable(GameTests
#       tests/main_test.cpp
#       # ... other test files
#   )
#   target_link_libraries(GameTests PRIVATE VoxelFortressGame VoxelEngine gtest gtest_main) # Example
#   add_test(NAME GameTests COMMAND GameTests)
# endif()

message(STATUS "Configuring VoxelFortressGame executable...")

```

## 3. Initial Compiler/Linker Settings

*   **C++ Standard:** C++17 is set in the root `CMakeLists.txt`. This can be updated to C++20 or C++23 if desired and supported by the chosen compiler.
*   **Warnings:** Basic warning flags (`-Wall -Wextra -Wpedantic -Werror` for GCC/Clang, `/W4 /WX` for MSVC) are enabled to catch common issues and enforce code quality. These can be customized.
*   **Build Types:** CMake supports different build types (e.g., `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`). These can be specified during the CMake configuration step (e.g., `cmake -DCMAKE_BUILD_TYPE=Debug ..`).
    *   **Debug:** Include debugging symbols, no optimizations.
    *   **Release:** Enable optimizations, no debugging symbols.
*   **Output Directories:** Executables will be placed in `build/bin/`, and libraries in `build/lib/`.

## 4. Create Placeholder `game/src/main.cpp`

Create `/home/system-x1/Projects/Voxel Castle/game/src/main.cpp` with a minimal entry point:

```cpp
#include <iostream>

int main() {
    std::cout << "Voxel Fortress - C++ Custom Engine" << std::endl;
    return 0;
}

```

## 5. Initial `.gitignore` Update

Ensure `/home/system-x1/Projects/Voxel Castle/.gitignore` includes:

```gitignore
# CMake / Build files
build/
**/compile_commands.json
**/CMakeCache.txt
**/CMakeFiles/
**/cmake_install.cmake
**/CTestTestfile.cmake
Makefile
*.make

# Executables and libraries
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# IDE specific files (add more as needed)
.vscode/
*.suo
*.user
*.sln
*.vcxproj
*.vcxproj.filters

# Other
*.log
*.tmp
*.bak
```
(Many of these might already be present from the previous `.gitignore` update, but ensure `build/` is definitely there).

## 6. Next Steps (To be performed after this setup)

1.  Create the directory structure as defined in section 1.
2.  Populate the `CMakeLists.txt` files as defined in section 2.
3.  Create the placeholder `main.cpp` as in section 4.
4.  Run CMake to configure the project:
    *   `mkdir build`
    *   `cd build`
    *   `cmake ..` (or `cmake -G "Your Generator" ..` e.g., `cmake -G "Visual Studio 17 2022" ..` or `cmake -G "Ninja" ..`)
    *   `cmake --build .` (or use your IDE's build command)
5.  Run the `VoxelFortressGame` executable from `build/bin/`.
6.  Commit the initial project structure and build system files to Git.

This task lays the groundwork for all subsequent C++ development.
