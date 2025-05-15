# game/CMakeLists.txt

# Define the game executable
add_executable(VoxelFortressGame
    # List all game source files here
    src/main.cpp
    src/SpectatorCamera.cpp
    src/core/game.cpp # Added Game class implementation
    src/core/GameInitializer.cpp # Initialization and shutdown helper module
    src/core/InputManager.cpp # Added InputManager module
    src/core/WorldSetup.cpp # Added WorldSetup module
    src/core/WorldSetupGlobals.cpp # Ensure global variable is linked
    src/core/GameRenderer.cpp # Added GameRenderer module
    src/core/GameLogic.cpp # Added GameLogic module
    # Example:
    # src/gameplay/player.cpp
    # ...
)

# Link the game against the engine library
target_link_libraries(VoxelFortressGame PRIVATE 
    VoxelEngine
    flecs::flecs # Add Flecs here so the game target can find flecs.h
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
