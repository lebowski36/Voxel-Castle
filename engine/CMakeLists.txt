# engine/CMakeLists.txt

# Define the engine library
add_library(VoxelEngine STATIC # or SHARED
    # List all engine source files here
    src/core/engine_core.cpp
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
