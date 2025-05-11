# engine/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(VoxelEngine LANGUAGES CXX)

# Define the VoxelEngine library
add_library(VoxelEngine STATIC
    src/core/engine_core.cpp
    src/platform/Window.cpp
    # Add other engine source files here
)

target_include_directories(VoxelEngine
    PUBLIC
        # For headers intended to be used by the game from engine/include (if any)
        # e.g., if you move Window.h to engine/include/platform/Window.h,
        # then game/src/main.cpp could use #include <platform/Window.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>

        # To allow game to include "platform/Window.h" directly from engine/src
        # This makes 'engine/src' an include path for consumers.
        # So, game/src/main.cpp can use #include "platform/Window.h"
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    PRIVATE
        # For the engine's own internal includes.
        # This makes 'engine/src' an include path for VoxelEngine's own .cpp files.
        # e.g., engine/src/platform/Window.cpp can use #include "platform/Window.h"
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Explicitly include SDL3 directories 
target_include_directories(VoxelEngine PRIVATE 
    ${CMAKE_BINARY_DIR}/external/SDL3/include-revision
    ${CMAKE_SOURCE_DIR}/external/SDL3/include
)

# Link against SDL3 (static)
# The target name SDL3::SDL3-static is common when SDL3 is built as a static library via CMake.
target_link_libraries(VoxelEngine PRIVATE SDL3::SDL3-static)

# Set C++ standard for the engine
target_compile_features(VoxelEngine PRIVATE cxx_std_17)

# Ensure the engine's public include directory exists, even if empty for now
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
