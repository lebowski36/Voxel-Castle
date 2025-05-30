#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

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

    // Test function to draw a visible rectangle regardless of other UI elements                      
    void drawTestRectangle();

    // Shader and projection access
    GLuint getShaderProgram() const { return shaderProgram_; }
    const glm::mat4& getProjectionMatrix() const { return projectionMatrix_; }
    int getScreenWidth() const { return screenWidth_; }
    int getScreenHeight() const { return screenHeight_; }

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
};

} // namespace UI
} // namespace VoxelEngine
