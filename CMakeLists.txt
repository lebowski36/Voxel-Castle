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
