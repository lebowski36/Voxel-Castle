# game/CMakeLists.txt

# Define the game executable
add_executable(VoxelFortressGame
    # List all game source files here
    src/main.cpp
    src/SpectatorCamera.cpp
    src/core/game.cpp # Added Game class implementation
    src/core/GameLoop.cpp # Added GameLoop class implementation
    src/core/GameRenderCoordinator.cpp # Added GameRenderCoordinator class implementation
    src/core/GameInitializer.cpp # Initialization and shutdown helper module
    src/core/InputManager.cpp # Added InputManager module
    src/core/WorldSetup.cpp # Added WorldSetup module
    src/core/WorldSetupGlobals.cpp # Ensure global variable is linked
    src/core/GameRenderer.cpp # Added GameRenderer module
    src/core/GameLogic.cpp # Added GameLogic module
    src/physics/PlayerPhysics.cpp # Added player physics system
    src/interaction/BlockPlacement.cpp # Added block placement system
    src/utils/debug_logger.cpp # Added debug logger system
    src/core/GameStateManager.cpp # State management system
    src/core/GameStateDelegate.cpp # State delegate methods for Game class
    src/core/TimeManager.cpp # Time control system
    src/core/GameTimeControl.cpp # Game time control methods
    src/core/BinaryReader.cpp # Binary data reader for save files
    src/core/BinaryWriter.cpp # Binary data writer for save files
    src/core/SerializationTest.cpp # Serialization testing implementation
    src/core/GameSerializationTest.cpp # Game class serialization test methods
    src/core/JsonUtils.cpp # JSON utility functions for save metadata
    src/core/SaveManager.cpp # Save/load manager implementation
    src/core/SaveManagerTest.cpp # SaveManager testing implementation
    src/core/GameSaveManagerTest.cpp # Game class SaveManager test methods
    # Example:
    # src/gameplay/player.cpp
    # ...
)

# Link the game against the engine library
target_link_libraries(VoxelFortressGame PRIVATE 
    VoxelEngine
    flecs::flecs # Add Flecs here so the game target can find flecs.h
    X11          # Add X11 for UI system DPI detection
)

# Explicitly include SDL3 directories for the game module
target_include_directories(VoxelFortressGame PRIVATE 
    ${CMAKE_BINARY_DIR}/external/SDL3/include-revision
    ${CMAKE_SOURCE_DIR}/external/SDL3/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link other dependencies if any (e.g., external libraries used directly by the game)
# target_link_libraries(VoxelFortressGame PRIVATE SomeExternalLibForGame)

# Public include directories for the game (if any, usually not needed for executable)
# target_include_directories(VoxelFortressGame PUBLIC
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#    $<INSTALL_INTERFACE:include>
# )

message(STATUS "Configuring VoxelFortressGame executable...")
