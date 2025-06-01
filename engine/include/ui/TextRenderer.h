#pragma once
#include "ui/FontManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <glad/glad.h>

namespace VoxelEngine {
namespace UI {

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool initialize(const std::string& fontPath, float fontSize = 32.0f);
    void cleanup();

    // Draws text at (x, y) in screen space, with color and scale
    void drawText(const std::string& text, float x, float y, float scale, const glm::vec3& color);

    float getFontHeight() const;
    
    // Set projection matrix for rendering
    void setProjectionMatrix(const glm::mat4& projection);

    FontManager* getFontManager() { return &fontManager_; }

private:
    bool initializeGL();
    
    FontManager fontManager_;
    GLuint shaderProgram_;
    GLuint vao_, vbo_, ebo_;
    glm::mat4 projectionMatrix_;
};

} // namespace UI
} // namespace VoxelEngine
