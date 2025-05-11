# Task: Select and Implement C++ Math Library

**High-Level Goal:** Integrate a C++ mathematics library to handle common graphics and game development calculations (vectors, matrices, quaternions, transformations).

**Parent Task:** [1. Core Engine Foundation (C++)](../../CURRENT_TODO.md)

## 📝 Sub-Tasks:

1.  **Research and Decision:**
    *   [x] Briefly review common C++ math libraries suitable for game development with OpenGL.
        *   **GLM (OpenGL Mathematics):** Header-only, GLSL-like syntax, designed for OpenGL.
        *   **Eigen:** Powerful general linear algebra, can be more complex.
        *   Other potential candidates (though GLM is a strong default for OpenGL projects).
    *   [x] **Decision Point (User Confirmation):** Confirm the choice of math library.
        *   *Initial Recommendation:* GLM due to its direct compatibility and ease of use with OpenGL.
    *   [x] Document the final decision and reasoning.
        *   *Decision: GLM selected on 2025-05-11 due to its OpenGL compatibility, header-only nature, and GLSL-like syntax.*

2.  **Integration (Assuming GLM):**
    *   [ ] Download the latest stable version of GLM (it's header-only).
        *   Typically, this involves cloning the repository or downloading a release zip from [https://github.com/g-truc/glm](https://github.com/g-truc/glm).
    *   [ ] Add GLM to the project's `external/` directory (e.g., `external/glm`).
    *   [ ] Update CMake (`engine/CMakeLists.txt` and potentially root `CMakeLists.txt`) to include GLM's include directory.
        *   For a header-only library like GLM, this usually means adding its path to `target_include_directories`.
        *   Example for engine library: `target_include_directories(VoxelEngine PRIVATE ../external/glm)` (adjust path as needed).
    *   [ ] Ensure GLM headers can be included in engine code (e.g., `#include <glm/glm.hpp>`).

3.  **Basic Usage and Testing:**
    *   [ ] Create a simple test case or example within the engine or a temporary test file:
        *   Include necessary GLM headers (e.g., `glm/vec3.hpp`, `glm/mat4x4.hpp`, `glm/gtc/matrix_transform.hpp`).
        *   Declare and initialize a `glm::vec3` (vector).
        *   Declare and initialize a `glm::mat4` (matrix), perhaps an identity matrix or a simple translation matrix.
        *   Perform a basic operation (e.g., multiply vector by matrix, or create a view/projection matrix).
        *   Print the results to the console to verify.
    *   [ ] Compile the project to ensure GLM is correctly integrated and there are no compilation errors related to its inclusion or usage.
    *   [ ] Run the test case and verify the output.

4.  **Documentation and Cleanup:**
    *   [ ] Add a brief note to project documentation about the chosen math library and its location/integration method.
    *   [ ] Remove any temporary test code if it's not part of a formal test suite.

## ✅ Completion Criteria:
- A C++ math library (e.g., GLM) is selected and the decision is documented.
- The chosen library is successfully integrated into the project's build system (CMake).
- Engine code can include and use the library's features (vectors, matrices) without compilation errors.
- A basic test demonstrates correct functionality of core math operations (e.g., matrix creation, vector transformation).
- Project documentation is updated to reflect the new dependency.

## 🪵 Log / Notes:
- 2025-05-11: Initial setup of this task file. GLM selected as the math library.
