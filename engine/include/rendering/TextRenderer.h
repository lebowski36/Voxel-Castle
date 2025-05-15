#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include <vector> // Added for std::vector
#include <memory> // Required for std::unique_ptr
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "rendering/FontManager.h" // Needs FontManager
#include "rendering/shader.h"    // Assuming you have a Shader class

namespace VoxelEngine {
namespace Rendering {

// Define TextVertex struct
struct TextVertex {
    glm::vec2 position;
    glm::vec2 texCoords;
};

class TextRenderer {
public:
    TextRenderer(FontManager* fontManager, const std::string& projectRootPath);
    ~TextRenderer();

    bool init();
    bool isShaderReady() const { return textShader_ && textShader_->isSuccessfullyLoaded(); } // Added

    void renderText2D(const std::string& text, float x, float y, float scale, glm::vec3 color, const glm::mat4& projection);
    
    void renderText3D(const std::string& text, 
                      glm::vec3 position, 
                      float scale, 
                      glm::vec3 color, 
                      const glm::mat4& view, 
                      const glm::mat4& projection,
                      const glm::vec3& cameraRight, // For billboarding
                      const glm::vec3& cameraUp     // For billboarding
                      );

private:
    FontManager* fontManager_;
    std::unique_ptr<Shader> textShader_; // Changed to std::unique_ptr<Shader>
    GLuint VAO_, VBO_;
    std::string projectRootPath_;

    void setupBuffers(); // Renamed from setupMesh to setupBuffers for clarity
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // TEXT_RENDERER_H
