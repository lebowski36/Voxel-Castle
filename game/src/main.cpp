#include "platform/Window.h" // Adjust path as necessary based on include directories
#include <iostream>

// GLM Headers
#define GLM_FORCE_SILENT_WARNINGS // Optional: To suppress GLM warnings if any
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/vector_float3.hpp> // For glm::vec3
#include <glm/ext/matrix_float4x4.hpp> // For glm::mat4
#include <glm/ext/matrix_clip_space.hpp> // For glm::perspective
#include <glm/ext/matrix_transform.hpp> // For glm::translate, glm::rotate, glm::scale

void printVec3(const glm::vec3& vec, const std::string& name) {
    std::cout << name << ": (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

void printMat4(const glm::mat4& mat, const std::string& name) {
    std::cout << name << ":" << std::endl;
    const float *pSource = (const float*)glm::value_ptr(mat);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << pSource[i * 4 + j] << "\t";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // --- GLM Test ---
    std::cout << "--- Running GLM Test ---" << std::endl;
    glm::vec3 myVector(1.0f, 2.0f, 3.0f);
    printVec3(myVector, "Original Vector");

    glm::mat4 identityMatrix = glm::mat4(1.0f); // Identity matrix
    printMat4(identityMatrix, "Identity Matrix");

    glm::mat4 translationMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, 0.0f, 0.0f));
    printMat4(translationMatrix, "Translation Matrix");

    glm::vec4 transformedVector = translationMatrix * glm::vec4(myVector, 1.0f); // Need w=1 for translation
    printVec3(glm::vec3(transformedVector), "Transformed Vector");
    
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    printMat4(proj, "Perspective Matrix");
    std::cout << "--- GLM Test End ---" << std::endl << std::endl;
    // --- End GLM Test ---

    Window gameWindow("Voxel Fortress - Alpha", 800, 600);

    if (!gameWindow.init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        return -1;
    }

    while (gameWindow.isRunning()) {
        gameWindow.handleEvents();
        gameWindow.update();
        gameWindow.render();
    }

    // cleanUp is called by Window destructor
    return 0;
}
