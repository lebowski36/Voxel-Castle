#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

#include "ui/TextRenderer.h"

namespace VoxelEngine {
namespace UI {

class UIElement;

class UIRenderer {
public:
    UIRenderer();
    ~UIRenderer();

    bool initialize(int screenWidth, int screenHeight, const std::string& projectRoot = "");
    void shutdown();
    
    void beginFrame();
    void endFrame();
    
    void setScreenSize(int width, int height);
    
    // Rendering primitives for UI elements
    void renderQuad(float x, float y, float width, float height, 
                   const glm::vec4& color = glm::vec4(1.0f));
    void renderTexturedQuad(float x, float y, float width, float height, 
                           GLuint textureID, const glm::vec4& texCoords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    void renderColoredQuad(float x, float y, float width, float height, 
                          const glm::vec4& color);

    // Text rendering
    bool loadUIFont(const std::string& fontPath, float fontSize = 32.0f);
    void drawText(const std::string& text, float x, float y, float scale = 1.0f, const glm::vec3& color = glm::vec3(1.0f));
    float getFontHeight() const;
    
    // Text measurement functions
    float getTextWidth(const std::string& text, float scale = 1.0f) const;
    float getTextHeight(float scale = 1.0f) const;

    // Test function to draw a visible rectangle regardless of other UI elements                      
    void drawTestRectangle();

    // Shader and projection access
    GLuint getShaderProgram() const { return shaderProgram_; }
    const glm::mat4& getProjectionMatrix() const { return projectionMatrix_; }
    int getScreenWidth() const { return screenWidth_; }
    int getScreenHeight() const { return screenHeight_; }

    // Check if text renderer is available
    bool isTextRendererAvailable() const { return textRenderer_ != nullptr; }

private:
    GLuint shaderProgram_;
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
    
    glm::mat4 projectionMatrix_;
    int screenWidth_;
    int screenHeight_;
    std::string projectRoot_;
    
    bool loadShaders();
    GLuint loadShader(const std::string& path, GLenum type);
    GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath);
    void setupQuadGeometry();

    std::unique_ptr<TextRenderer> textRenderer_;
};

} // namespace UI
} // namespace VoxelEngine
