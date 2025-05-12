# engine/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(VoxelEngine LANGUAGES CXX)

# Define the VoxelEngine library
add_library(VoxelEngine STATIC
    src/core/engine_core.cpp
    src/platform/Window.cpp
    src/ecs/systems/movement_system.cpp
    src/world/chunk_segment.cpp # Added ChunkSegment source
    src/world/chunk_column.cpp # Added ChunkColumn source
    src/world/world_manager.cpp # Added WorldManager source
    src/world/quadtree.cpp      # Added Quadtree source
    src/world/test_quadtree.cpp # Added Quadtree test source
    src/world/test_voxel_system.cpp # Added Voxel System test source
    src/world/test_mesh_generation.cpp # Added Mesh Generation test source
    src/rendering/mesh_builder.cpp # Added MeshBuilder source
    src/rendering/mesh_renderer.cpp # Added MeshRenderer source
    src/external/stb_image_impl.cpp # Added stb_image implementation
    # Add GLAD source file if compiling it directly
    # Or link against a pre-built GLAD library later
    # For example, if GLAD source is in external/glad/src/glad.c relative to root:
    # ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c 
)

target_include_directories(VoxelEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/glad/include> 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/glm> 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external> # For stb_image.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Explicitly include SDL3 directories 
target_include_directories(VoxelEngine PRIVATE 
    ${CMAKE_BINARY_DIR}/external/SDL3/include-revision
    ${CMAKE_SOURCE_DIR}/external/SDL3/include
)

# Link against SDL3 (static) and OpenGL
target_link_libraries(VoxelEngine PRIVATE 
    SDL3::SDL3-static
    OpenGL::GL # Standard CMake target for OpenGL
    glad
    flecs::flecs # Link against the Flecs library
)

# Set C++ standard for the engine
target_compile_features(VoxelEngine PRIVATE cxx_std_17)

# Ensure the engine's public include directory exists, even if empty for now
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- GLAD --- 
# If you are building GLAD from source directly within this target (not recommended for modularity)
# you would add its .c file to add_library and its include path to target_include_directories.

# If GLAD is built as a separate library (e.g., in external/glad with its own CMakeLists.txt)
# you would link against it: target_link_libraries(VoxelEngine PRIVATE glad)
# And ensure its include directory is propagated: target_include_directories(VoxelEngine PUBLIC/PRIVATE path/to/glad/include)

# For now, assuming GLAD headers will be made available globally or handled by the user.
# If you have downloaded GLAD and put it in, for example, 'external/glad_src_and_include':
# target_sources(VoxelEngine PRIVATE ${CMAKE_SOURCE_DIR}/external/glad_src_and_include/src/glad.c)
# target_include_directories(VoxelEngine PUBLIC ${CMAKE_SOURCE_DIR}/external/glad_src_and_include/include)
