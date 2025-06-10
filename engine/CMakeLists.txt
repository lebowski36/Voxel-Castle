# engine/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(VoxelEngine LANGUAGES CXX)

# Define the VoxelEngine library
add_library(VoxelEngine STATIC
    src/core/engine_core.cpp
    src/platform/Window.cpp
    src/platform/fullscreen_debug.cpp # Added fullscreen_debug.cpp
    src/ecs/systems/movement_system.cpp
    src/world/chunk_segment.cpp # Added ChunkSegment source
    src/world/chunk_column.cpp # Added ChunkColumn source
    src/world/world_manager.cpp # Added WorldManager source
    src/world/world_generator.cpp # Added WorldGenerator source
    src/world/world_seed.cpp # Added WorldSeed source
    src/world/world_parameters.cpp # Added WorldParameters source
    src/world/world_coordinates.cpp # Added WorldCoordinates source (±128 chunk range support)
    src/world/seed_world_generator.cpp # Added SeedWorldGenerator source
    src/world/world_metadata.cpp # World metadata and persistence system
    src/world/world_persistence_manager.cpp # World creation, loading, and directory management
    src/world/voxel_face_patterns.cpp # Added VoxelFacePatterns source
    src/world/block_properties.cpp # Added BlockProperties system source
    src/world/block_properties_generated.cpp # Generated unified block properties from JSON
    # Biome system sources
    src/world/biome/biome_types.cpp # Biome type definitions and utilities
    src/world/biome/biome_data.cpp # Biome data structures and climate matching
    src/world/biome/biome_registry.cpp # Central biome registry and management
    src/world/regional_data.cpp # Regional environmental data system
    src/world/geological_data.cpp # Geological data structures and simulation
    src/world/hydrological_data.cpp # Hydrological data structures and simulation
    src/world/climate_data.cpp # Climate data structures and simulation
    src/world/tectonic_plate.cpp # Tectonic plate structures and operations
    src/world/tectonic_simulator.cpp # Tectonic simulation system
    src/world/quadtree.cpp      # Added Quadtree source
    # src/world/test_quadtree.cpp # Removed Quadtree test source
    src/world/test_voxel_system.cpp # Added Voxel System test source
    src/world/test_mesh_generation.cpp # Added Mesh Generation test source
    src/rendering/mesh_builder.cpp # Added MeshBuilder source
    src/rendering/meshing/two_phase_greedy_meshing_algorithm.cpp # Two-phase greedy meshing algorithm
    src/rendering/mesh_job_system.cpp # Added MeshJobSystem source
    src/rendering/mesh_renderer.cpp # Added MeshRenderer source
    src/rendering/block_outline_renderer.cpp # Added BlockOutlineRenderer source
    src/rendering/render_utils.cpp # Added RenderUtils source
    src/rendering/debug_utils.cpp # Added DebugUtils source
    src/rendering/texture_atlas.cpp # Added TextureAtlas source
    src/external/stb_image_impl.cpp # Added stb_image implementation
    src/rendering/Shader.cpp # Added Shader source
    # Enhanced noise system
    src/util/enhanced_noise.cpp # Added EnhancedNoise source
    # Utils system sources
    src/utils/debug_logger_stub.cpp # Stub implementation for VoxelCastle::Utils logging (temporary until dual logging systems are merged)
    # UI System sources
    src/ui/UIElement.cpp # UI base element
    src/ui/UIRenderer.cpp # UI rendering system
    src/ui/UISystem.cpp # Main UI system manager
    src/ui/UILogger.cpp # UI logging system
    src/ui/TextRenderer.cpp # Text rendering system
    src/ui/FontManager.cpp # Font management system
    src/ui/elements/UITexture.cpp # Basic texture UI element
    src/ui/elements/UIPanel.cpp # Basic solid color UI element
    src/ui/elements/HUD.cpp # Game HUD element
    src/ui/elements/Crosshair.cpp # Crosshair targeting reticle
    src/ui/MenuSystem.cpp # Menu system manager
    src/ui/elements/UIButton.cpp # Button UI element
    src/ui/elements/UICheckbox.cpp # Checkbox UI element
    src/ui/elements/BaseMenu.cpp # Base menu class for consistent styling
    src/ui/elements/MainMenu.cpp # Main menu UI
    src/ui/elements/SettingsMenu.cpp # Settings menu UI
    src/ui/WorldConfigurationUI.cpp # World generation setup UI
    src/ui/WorldSimulationUI.cpp # World generation simulation UI
    src/ui/BlockVisualizationPanel.cpp # Block visualization debug panel
    # Input system sources
    src/input/MouseCaptureManager.cpp # Mouse capture management
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
        # Use the fetched GLM path instead of the local one
        $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external> # For stb_image.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Explicitly include SDL3 directories - use the fetched path
target_include_directories(VoxelEngine PRIVATE 
    ${SDL3_SOURCE_DIR}/include
)

# Link against SDL3 (static) and OpenGL
target_link_libraries(VoxelEngine PRIVATE 
    SDL3::SDL3
    OpenGL::GL # Standard CMake target for OpenGL
    glad
    flecs::flecs # Link against the Flecs library
)

# Link VoxelEngine against its dependencies
target_link_libraries(VoxelEngine PUBLIC
    SDL3::SDL3 # Link against SDL3 
    glad            # Link against GLAD
    OpenGL::GL      # Link against OpenGL
    Threads::Threads # Link against pthreads
    flecs::flecs    # Link against Flecs
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
