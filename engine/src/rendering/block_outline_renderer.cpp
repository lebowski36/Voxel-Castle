#include "rendering/block_outline_renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VoxelEngine {
namespace Rendering {

// Simple vertex shader for wireframe outline
static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// Simple fragment shader for wireframe outline
static const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 outlineColor;

void main()
{
    FragColor = vec4(outlineColor, 1.0);
}
)";

BlockOutlineRenderer::BlockOutlineRenderer()
    : vao_(0), vbo_(0), ebo_(0), shaderProgram_(0), ready_(false) {
}

BlockOutlineRenderer::~BlockOutlineRenderer() {
    if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
    if (ebo_ != 0) glDeleteBuffers(1, &ebo_);
    if (shaderProgram_ != 0) glDeleteProgram(shaderProgram_);
}

bool BlockOutlineRenderer::initialize() {
    if (!loadShaders()) {
        std::cerr << "Failed to load shaders for block outline renderer" << std::endl;
        return false;
    }

    createWireframeCube();
    ready_ = true;
    return true;
}

void BlockOutlineRenderer::renderOutline(const glm::ivec3& blockPosition, 
                                        const glm::mat4& view, 
                                        const glm::mat4& projection,
                                        const glm::vec3& color,
                                        float lineWidth) {
    if (!ready_) return;

    // Save current OpenGL state
    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    GLfloat currentLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);

    // Set up OpenGL state for wireframe rendering
    glDisable(GL_DEPTH_TEST); // Render outline on top
    glLineWidth(lineWidth);

    // Use our shader program
    glUseProgram(shaderProgram_);

    // Create model matrix - translate to block position
    // Add small offset to prevent z-fighting with the block faces
    glm::mat4 model = glm::translate(glm::mat4(1.0f), 
                                   glm::vec3(blockPosition) - glm::vec3(0.001f));
    model = glm::scale(model, glm::vec3(1.002f)); // Slightly larger than block

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram_, "outlineColor"), 1, glm::value_ptr(color));

    // Render the wireframe cube
    glBindVertexArray(vao_);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0); // 12 edges * 2 vertices = 24
    glBindVertexArray(0);

    // Restore OpenGL state
    if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
    glLineWidth(currentLineWidth);
}

void BlockOutlineRenderer::createWireframeCube() {
    // Cube vertices (0 to 1 range, will be translated and scaled)
    float vertices[] = {
        // Bottom face
        0.0f, 0.0f, 0.0f,  // 0
        1.0f, 0.0f, 0.0f,  // 1
        1.0f, 0.0f, 1.0f,  // 2
        0.0f, 0.0f, 1.0f,  // 3
        // Top face
        0.0f, 1.0f, 0.0f,  // 4
        1.0f, 1.0f, 0.0f,  // 5
        1.0f, 1.0f, 1.0f,  // 6
        0.0f, 1.0f, 1.0f   // 7
    };

    // Wireframe indices (pairs of vertices for each edge)
    unsigned int indices[] = {
        // Bottom face edges
        0, 1,  1, 2,  2, 3,  3, 0,
        // Top face edges
        4, 5,  5, 6,  6, 7,  7, 4,
        // Vertical edges
        0, 4,  1, 5,  2, 6,  3, 7
    };

    // Generate and bind vertex array object
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Generate and bind vertex buffer object
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind element buffer object
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindVertexArray(0);
}

bool BlockOutlineRenderer::loadShaders() {
    GLuint vertexShader = loadShader(vertexShaderSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) return false;

    GLuint fragmentShader = loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Create shader program
    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader);
    glAttachShader(shaderProgram_, fragmentShader);
    glLinkProgram(shaderProgram_);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram_, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Clean up individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

GLuint BlockOutlineRenderer::loadShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

} // namespace Rendering
} // namespace VoxelEngine
