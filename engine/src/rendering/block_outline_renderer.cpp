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
    : vao_(0), vbo_(0), ebo_(0), faceVao_(0), faceVbo_(0), faceEbo_(0), 
      previewVao_(0), previewVbo_(0), previewEbo_(0), 
      shaderProgram_(0), previewShaderProgram_(0), ready_(false) {
}

BlockOutlineRenderer::~BlockOutlineRenderer() {
    if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
    if (ebo_ != 0) glDeleteBuffers(1, &ebo_);
    if (faceVao_ != 0) glDeleteVertexArrays(1, &faceVao_);
    if (faceVbo_ != 0) glDeleteBuffers(1, &faceVbo_);
    if (faceEbo_ != 0) glDeleteBuffers(1, &faceEbo_);
    if (previewVao_ != 0) glDeleteVertexArrays(1, &previewVao_);
    if (previewVbo_ != 0) glDeleteBuffers(1, &previewVbo_);
    if (previewEbo_ != 0) glDeleteBuffers(1, &previewEbo_);
    if (shaderProgram_ != 0) glDeleteProgram(shaderProgram_);
    if (previewShaderProgram_ != 0) glDeleteProgram(previewShaderProgram_);
}

bool BlockOutlineRenderer::initialize() {
    if (!loadShaders()) {
        std::cerr << "Failed to load shaders for block outline renderer" << std::endl;
        return false;
    }

    if (!loadPreviewShaders()) {
        std::cerr << "Failed to load preview shaders for block outline renderer" << std::endl;
        return false;
    }

    createWireframeCube();
    createFaceHighlightMesh();
    createBlockPreviewMesh();
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

void BlockOutlineRenderer::renderFaceHighlight(const glm::ivec3& blockPosition,
                                             const glm::vec3& faceNormal,
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
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(blockPosition));
    
    // Add small offset based on face normal to prevent z-fighting
    glm::vec3 offset = faceNormal * 0.002f;
    model = glm::translate(model, offset);

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram_, "outlineColor"), 1, glm::value_ptr(color));

    // Determine which face to render based on the normal
    glBindVertexArray(faceVao_);
    
    // Render the appropriate face outline based on normal
    if (std::abs(faceNormal.y) > 0.9f) {
        // Top or bottom face
        if (faceNormal.y > 0) {
            // Top face (indices 16-23)
            glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(16 * sizeof(unsigned int)));
        } else {
            // Bottom face (indices 0-7)
            glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)0);
        }
    } else if (std::abs(faceNormal.x) > 0.9f) {
        // Left or right face
        if (faceNormal.x > 0) {
            // Right face (indices 8-15)
            glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(8 * sizeof(unsigned int)));
        } else {
            // Left face (indices 24-31)
            glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));
        }
    } else if (std::abs(faceNormal.z) > 0.9f) {
        // Front or back face
        if (faceNormal.z > 0) {
            // Front face (indices 32-39)
            glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(32 * sizeof(unsigned int)));
        } else {
            // Back face (indices 40-47)
            glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(40 * sizeof(unsigned int)));
        }
    }
    
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

void BlockOutlineRenderer::createFaceHighlightMesh() {
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

    // Face outline indices - each face gets its own set of 4 lines (8 indices)
    unsigned int indices[] = {
        // Bottom face outline (Y = 0) - indices 0-7
        0, 1,  1, 2,  2, 3,  3, 0,
        
        // Right face outline (X = 1) - indices 8-15
        1, 5,  5, 6,  6, 2,  2, 1,
        
        // Top face outline (Y = 1) - indices 16-23
        4, 5,  5, 6,  6, 7,  7, 4,
        
        // Left face outline (X = 0) - indices 24-31
        0, 4,  4, 7,  7, 3,  3, 0,
        
        // Front face outline (Z = 1) - indices 32-39
        2, 6,  6, 7,  7, 3,  3, 2,
        
        // Back face outline (Z = 0) - indices 40-47
        0, 4,  4, 5,  5, 1,  1, 0
    };

    // Generate and bind vertex array object for faces
    glGenVertexArrays(1, &faceVao_);
    glBindVertexArray(faceVao_);

    // Generate and bind vertex buffer object for faces
    glGenBuffers(1, &faceVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, faceVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind element buffer object for faces
    glGenBuffers(1, &faceEbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEbo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindVertexArray(0);
}

void BlockOutlineRenderer::createBlockPreviewMesh() {
    // Cube vertices (0 to 1 range, will be translated)
    float vertices[] = {
        // Bottom face (Y = 0)
        0.0f, 0.0f, 0.0f,  // 0
        1.0f, 0.0f, 0.0f,  // 1
        1.0f, 0.0f, 1.0f,  // 2
        0.0f, 0.0f, 1.0f,  // 3
        
        // Top face (Y = 1)
        0.0f, 1.0f, 0.0f,  // 4
        1.0f, 1.0f, 0.0f,  // 5
        1.0f, 1.0f, 1.0f,  // 6
        0.0f, 1.0f, 1.0f   // 7
    };

    // Indices for solid cube (36 indices for 6 faces * 2 triangles per face)
    unsigned int indices[] = {
        // Bottom face
        0, 1, 2,  0, 2, 3,
        // Top face
        4, 6, 5,  4, 7, 6,
        // Front face (Z = 1)
        3, 2, 6,  3, 6, 7,
        // Back face (Z = 0)
        0, 4, 5,  0, 5, 1,
        // Right face (X = 1)
        1, 5, 6,  1, 6, 2,
        // Left face (X = 0)
        0, 3, 7,  0, 7, 4
    };

    // Generate and bind vertex array object for preview
    glGenVertexArrays(1, &previewVao_);
    glBindVertexArray(previewVao_);

    // Generate and bind vertex buffer object for preview
    glGenBuffers(1, &previewVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, previewVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind element buffer object for preview
    glGenBuffers(1, &previewEbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previewEbo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindVertexArray(0);
}

bool BlockOutlineRenderer::loadPreviewShaders() {
    // Vertex shader for transparent block preview
    static const char* previewVertexShaderSource = R"(
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

    // Fragment shader for transparent block preview
    static const char* previewFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 blockColor;
uniform float alpha;

void main()
{
    FragColor = vec4(blockColor, alpha);
}
)";

    GLuint vertexShader = loadShader(previewVertexShaderSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) return false;

    GLuint fragmentShader = loadShader(previewFragmentShaderSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Create shader program for preview
    previewShaderProgram_ = glCreateProgram();
    glAttachShader(previewShaderProgram_, vertexShader);
    glAttachShader(previewShaderProgram_, fragmentShader);
    glLinkProgram(previewShaderProgram_);

    // Check for linking errors
    GLint success;
    glGetProgramiv(previewShaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(previewShaderProgram_, 512, NULL, infoLog);
        std::cerr << "Preview shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Clean up individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
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

void BlockOutlineRenderer::renderBlockPreview(const glm::ivec3& blockPosition,
                                            const glm::mat4& view, 
                                            const glm::mat4& projection,
                                            const glm::vec3& color,
                                            float alpha) {
    if (!ready_) return;

    // Save current OpenGL state
    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    GLboolean depthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    GLboolean cullFaceEnabled;
    glGetBooleanv(GL_CULL_FACE, &cullFaceEnabled);

    // Set up OpenGL state for transparent rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // Don't write to depth buffer for transparent objects
    glDisable(GL_CULL_FACE); // Render both front and back faces

    // Use our preview shader program
    glUseProgram(previewShaderProgram_);

    // Create model matrix - translate to block position
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(blockPosition));

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(previewShaderProgram_, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(previewShaderProgram_, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(previewShaderProgram_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(previewShaderProgram_, "blockColor"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocation(previewShaderProgram_, "alpha"), alpha);

    // Render the preview block
    glBindVertexArray(previewVao_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 6 faces * 2 triangles * 3 vertices = 36
    glBindVertexArray(0);

    // Restore OpenGL state
    if (!blendEnabled) glDisable(GL_BLEND);
    if (depthMask) glDepthMask(GL_TRUE);
    if (cullFaceEnabled) glEnable(GL_CULL_FACE);
}

} // namespace Rendering
} // namespace VoxelEngine
