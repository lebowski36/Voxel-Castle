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

# --- Use FetchContent for external libraries ---
include(FetchContent)

# --- SDL Configuration ---
# Build SDL as a static library
set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library")
set(SDL_SHARED OFF CACHE BOOL "Build SDL as a shared library")
# Fetch SDL3 from repository
FetchContent_Declare(
  SDL3
  URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.16/SDL3-3.2.16.zip
)
FetchContent_MakeAvailable(SDL3)
# --- End SDL Configuration ---

# --- GLM Configuration ---
FetchContent_Declare(
  glm
  URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)
FetchContent_MakeAvailable(glm)
# --- End GLM Configuration ---

# --- GLAD Configuration ---
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glad)
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
FetchContent_Declare(
  flecs
  GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
  GIT_TAG master
)
FetchContent_MakeAvailable(flecs)
# --- End Flecs Configuration ---

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find Threads (for pthread)
find_package(Threads REQUIRED)

# Add subdirectories for engine and game
add_subdirectory(engine)
add_subdirectory(game)

message(STATUS "Project VoxelFortress configured. Build with 'cmake --build .'")
