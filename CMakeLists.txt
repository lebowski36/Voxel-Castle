cmake_minimum_required(VERSION 3.20) # Match user's CMake version or a reasonable minimum
project(VoxelFortress VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard for the whole project (can be overridden per target)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings (optional but good practice)
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic) # Removed -Werror
endif()

# Output directories for executables and libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- SDL Configuration ---
# Build SDL as a static library
set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library")
set(SDL_SHARED OFF CACHE BOOL "Build SDL as a shared library")
# You might need to set other SDL options here if defaults are not suitable
# e.g., disabling certain features if not needed:
# set(SDL_VIDEO OFF CACHE BOOL "") # Example: if you only need SDL_timer or SDL_audio

# Add subdirectory for SDL (vendored)
add_subdirectory(external/SDL3)
# --- End SDL Configuration ---

# --- GLAD Configuration ---
add_subdirectory(external/glad)
# --- End GLAD Configuration ---

# --- Flecs Configuration ---
# Options:
#   FLECS_STATIC_ONLY (OFF): Build Flecs only as a static library
#   FLECS_SHARED_ONLY (OFF): Build Flecs only as a shared library
#   FLECS_PIC (ON): Enable position independent code (recommended for shared libs, good for static too)
#   FLECS_DEBUG_FLAGS (OFF): Enable debug flags
#   FLECS_STRIP_SYMBOLS (OFF): Strip symbols from binary (only for release)
#   FLECS_BUILD_EXAMPLES (OFF): Build Flecs examples
#   FLECS_BUILD_TESTS (OFF): Build Flecs tests
#   FLECS_COVERAGE (OFF): Enable coverage reporting
#   FLECS_SANITIZE (OFF): Enable sanitizers (asan, ubsan)
#   FLECS_INSTALL_LIBS (ON): Install Flecs libraries
#   FLECS_INSTALL_CMAKE (ON): Install Flecs CMake config files
#   FLECS_C99 (OFF): Enforce C99 standard (Flecs is C99 but can be compiled as C++)
# We will use the defaults for now, which should build a static library.
add_subdirectory(external/flecs)
# --- End Flecs Configuration ---

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find Threads (for pthread)
find_package(Threads REQUIRED)

# Add subdirectories for engine and game
add_subdirectory(engine)
add_subdirectory(game)

# Add seed system test executable
add_executable(seed_system_test tests/seed_system_test.cpp)
target_link_libraries(seed_system_test PRIVATE VoxelEngine)
target_include_directories(seed_system_test PRIVATE engine/include)

# Add world parameters test executable
add_executable(world_parameters_test tests/world_parameters_test.cpp)
target_link_libraries(world_parameters_test PRIVATE VoxelEngine)
target_include_directories(world_parameters_test PRIVATE engine/include)

# Enable CTest (optional, if you add tests)
# enable_testing()
# add_subdirectory(tests) # If you have a tests directory with CMakeLists.txt

message(STATUS "Project VoxelFortress configured. Build with 'cmake --build .'")
