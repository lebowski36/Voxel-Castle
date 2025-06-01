#include "ui/TextRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace VoxelEngine {
namespace UI {

TextRenderer::TextRenderer() : 
    shaderProgram_(0), vao_(0), vbo_(0), ebo_(0) {}

TextRenderer::~TextRenderer() {
    cleanup();
}

bool TextRenderer::initialize(const std::string& fontPath, float fontSize) {
    if (!fontManager_.loadFont(fontPath, fontSize)) {
        return false;
    }
    
    if (!initializeGL()) {
        std::cerr << "[TextRenderer] Failed to initialize OpenGL resources" << std::endl;
        return false;
    }
    
    return true;
}

void TextRenderer::cleanup() {
    if (vao_) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (ebo_) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (shaderProgram_) {
        glDeleteProgram(shaderProgram_);
        shaderProgram_ = 0;
    }
}

bool TextRenderer::initializeGL() {
    // Create text shader program
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
    TexCoord = aTexCoord;
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec3 textColor;

void main() {
    float alpha = texture(textTexture, TexCoord).r;
    FragColor = vec4(textColor, alpha);
}
)";

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "[TextRenderer] Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "[TextRenderer] Fragment shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return false;
    }

    // Create shader program
    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader);
    glAttachShader(shaderProgram_, fragmentShader);
    glLinkProgram(shaderProgram_);
    
    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram_, 512, NULL, infoLog);
        std::cerr << "[TextRenderer] Shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Setup VAO, VBO, EBO for text quads
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    // Setup VBO (dynamic buffer for text vertices)
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, NULL, GL_DYNAMIC_DRAW); // 4 vertices * 4 floats

    // Setup EBO (indices for quad)
    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    
    return true;
}

float TextRenderer::getFontHeight() const {
    return fontManager_.getFontHeight();
}

void TextRenderer::setProjectionMatrix(const glm::mat4& projection) {
    projectionMatrix_ = projection;
}

void TextRenderer::drawText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    if (shaderProgram_ == 0 || fontManager_.getAtlasTexture() == 0) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(shaderProgram_);
    glBindVertexArray(vao_);
    
    // Set uniforms
    GLint projLoc = glGetUniformLocation(shaderProgram_, "projection");
    if (projLoc != -1) {
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix_));
    }
    
    GLint colorLoc = glGetUniformLocation(shaderProgram_, "textColor");
    if (colorLoc != -1) {
        glUniform3f(colorLoc, color.r, color.g, color.b);
    }
    
    GLint textureLoc = glGetUniformLocation(shaderProgram_, "textTexture");
    if (textureLoc != -1) {
        glUniform1i(textureLoc, 0);
    }
    
    // Bind font atlas texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontManager_.getAtlasTexture());
    
    float xpos = x;
    float ypos = y;
    
    for (char c : text) {
        const GlyphInfo* glyph = fontManager_.getGlyph(c);
        if (!glyph) continue;
        
        float w = glyph->size.x * scale;
        float h = glyph->size.y * scale;
        float xoff = glyph->offset.x * scale;
        float yoff = glyph->offset.y * scale;
        
        // Calculate quad vertices
        float vertices[] = {
            // positions        // texture coords
            xpos + xoff,     ypos + yoff + h, glyph->uv0.x, glyph->uv1.y, // bottom left
            xpos + xoff + w, ypos + yoff + h, glyph->uv1.x, glyph->uv1.y, // bottom right  
            xpos + xoff + w, ypos + yoff,     glyph->uv1.x, glyph->uv0.y, // top right
            xpos + xoff,     ypos + yoff,     glyph->uv0.x, glyph->uv0.y  // top left
        };
        
        // Update VBO with glyph vertices
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        // Set model matrix for this glyph
        glm::mat4 model = glm::mat4(1.0f);
        GLint modelLoc = glGetUniformLocation(shaderProgram_, "model");
        if (modelLoc != -1) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }
        
        // Draw glyph
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        xpos += glyph->advance * scale;
    }
    
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);
}

} // namespace UI
} // namespace VoxelEngine
