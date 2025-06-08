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

# --- GoogleTest Configuration ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# --- End GoogleTest Configuration ---

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

# Add simple feature parity test executable
add_executable(feature_parity_test 
    tests/simple_feature_parity_test.cpp
    game/src/utils/debug_logger.cpp  # Include debug utilities
)
target_link_libraries(feature_parity_test PRIVATE VoxelEngine)
target_include_directories(feature_parity_test PRIVATE 
    engine/include
    game/include  # Include game headers for debug utilities
)

# Add feature parity test executable (requires GoogleTest)
# TODO: Add GoogleTest dependency for this test
# add_executable(feature_parity_test tests/feature_parity_test.cpp)
# target_link_libraries(feature_parity_test PRIVATE VoxelEngine gtest gtest_main)
# target_include_directories(feature_parity_test PRIVATE engine/include)

# Add voxel face patterns test executable
add_executable(test_voxel_face_patterns tests/world/test_voxel_face_patterns.cpp)
target_link_libraries(test_voxel_face_patterns PRIVATE VoxelEngine gtest gtest_main)
target_include_directories(test_voxel_face_patterns PRIVATE engine/include)

# Add regional data test executable
add_executable(test_regional_data tests/test_regional_data.cpp)
target_link_libraries(test_regional_data PRIVATE VoxelEngine gtest gtest_main)
target_include_directories(test_regional_data PRIVATE engine/include)

# Add seed world generator regional integration test
add_executable(test_seed_world_generator_regional 
    tests/test_seed_world_generator_regional.cpp
    game/src/utils/debug_logger.cpp  # Include debug utilities
)
target_link_libraries(test_seed_world_generator_regional PRIVATE VoxelEngine gtest gtest_main)
target_include_directories(test_seed_world_generator_regional PRIVATE 
    engine/include
    game/include  # Include game headers for debug utilities
)

# Add tectonic simulation test executable
add_executable(test_tectonic_simulation tests/test_tectonic_simulation.cpp)
target_link_libraries(test_tectonic_simulation PRIVATE VoxelEngine gtest gtest_main)
target_include_directories(test_tectonic_simulation PRIVATE engine/include)

# Add tectonic integration test executable
add_executable(test_tectonic_integration test_tectonic_integration.cpp)
target_link_libraries(test_tectonic_integration PRIVATE VoxelEngine glm::glm)
target_include_directories(test_tectonic_integration PRIVATE engine/include)

# Enable CTest (optional, if you add tests)
enable_testing()
# add_subdirectory(tests) # If you have a tests directory with CMakeLists.txt

message(STATUS "Project VoxelFortress configured. Build with 'cmake --build .'")
