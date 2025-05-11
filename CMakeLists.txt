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

# Add subdirectories for engine and game
add_subdirectory(engine)
add_subdirectory(game)

# Enable CTest (optional, if you add tests)
# enable_testing()
# add_subdirectory(tests) # If you have a tests directory with CMakeLists.txt

message(STATUS "Project VoxelFortress configured. Build with 'cmake --build .'")
