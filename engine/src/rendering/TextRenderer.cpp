#include "rendering/TextRenderer.h"
#include <iostream> // For std::cout, std::cerr
#include <glm/gtc/type_ptr.hpp>

namespace VoxelEngine {
namespace Rendering {

TextRenderer::TextRenderer(FontManager* fontManager, const std::string& projectRootPath)
    : fontManager_(fontManager), 
      textShader_(nullptr), // Initialize unique_ptr to nullptr explicitly
      VAO_(0), 
      VBO_(0), 
      projectRootPath_(projectRootPath) {
    // textShader_ will be initialized in init()
}

TextRenderer::~TextRenderer() {
    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
    }
    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
    }
    // textShader_ unique_ptr will automatically clean up
}

bool TextRenderer::init() {
    if (!fontManager_) {
        std::cerr << "ERROR::TEXTRENDERER::INIT: FontManager is not set." << std::endl;
        return false;
    }
    if (fontManager_->getAtlasTextureID() == 0) {
        std::cerr << "ERROR::TEXTRENDERER::INIT: FontManager has not loaded a font or atlas is invalid." << std::endl;
        return false;
    }

    std::string textVertPath = projectRootPath_ + "/assets/shaders/text.vert";
    std::string textFragPath = projectRootPath_ + "/assets/shaders/text.frag";
    textShader_ = std::make_unique<Shader>(textVertPath.c_str(), textFragPath.c_str());

    if (!textShader_ || !textShader_->isSuccessfullyLoaded()) { // Check textShader_ pointer too
        std::cerr << "ERROR::TEXTRENDERER::SHADER_INIT_FAILED\n"
                  << "Vertex Shader Path: " << textVertPath << "\n"
                  << "Fragment Shader Path: " << textFragPath << std::endl;
        return false;
    }

    setupBuffers();
    std::cout << "TextRenderer initialized successfully." << std::endl;
    return true;
}

void TextRenderer::setupBuffers() { // Renamed from setupMesh
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    // Buffer will be filled dynamically, so just allocate for now or set data with nullptr
    // Using dynamic draw, so data will be provided per frame or per text block.
    // Example: glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * MAX_CHARS_PER_CALL * 6, nullptr, GL_DYNAMIC_DRAW);
    // For now, let's assume glBufferData is called in renderText functions with actual data.

    // Vertex attribute pointers
    // Position attribute (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, position));
    glEnableVertexAttribArray(0);
    // Texture coordinate attribute (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, texCoords));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::renderText2D(const std::string& text, float x, float y, float scale, glm::vec3 color, const glm::mat4& projection) {
    if (!fontManager_ || !textShader_ || !textShader_->isSuccessfullyLoaded()) {
        std::cerr << "TextRenderer not properly initialized or font/shader missing." << std::endl;
        return;
    }

    textShader_->use();
    textShader_->setMat4("projection", projection);
    textShader_->setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontManager_->getAtlasTextureID());
    textShader_->setInt("textAtlas", 0);

    glBindVertexArray(VAO_);

    float currentX = x;
    float currentY = y;

    std::vector<TextVertex> vertices;
    vertices.reserve(text.length() * 6); // Pre-allocate memory

    for (char c : text) {
        const CharacterInfo& ch = fontManager_->getCharacter(c);

        float xpos = currentX + ch.bearing.x * scale;
        float ypos = currentY - (ch.size.y - ch.bearing.y) * scale; // Adjust for baseline

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // Vertex data for the quad (counter-clockwise winding order)
        vertices.push_back({{xpos, ypos + h}, {ch.uv_y0.x, ch.uv_y0.y}});
        vertices.push_back({{xpos, ypos}, {ch.uv_x0.x, ch.uv_x0.y}});
        vertices.push_back({{xpos + w, ypos}, {ch.uv_y1.x, ch.uv_y1.y}});

        vertices.push_back({{xpos, ypos + h}, {ch.uv_y0.x, ch.uv_y0.y}});
        vertices.push_back({{xpos + w, ypos}, {ch.uv_y1.x, ch.uv_y1.y}});
        vertices.push_back({{xpos + w, ypos + h}, {ch.uv_x1.x, ch.uv_x1.y}});

        currentX += (ch.advance >> 6) * scale; // Advance is 1/64th of a pixel
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextVertex), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::renderText3D(const std::string& text, 
                                glm::vec3 position, 
                                float scale, 
                                glm::vec3 color, 
                                const glm::mat4& view, 
                                const glm::mat4& projection,
                                const glm::vec3& cameraRight,
                                const glm::vec3& cameraUp) {
    if (!fontManager_ || !textShader_ || !textShader_->isSuccessfullyLoaded()) {
        std::cerr << "TextRenderer not properly initialized or font/shader missing." << std::endl;
        return;
    }

    textShader_->use();
    textShader_->setMat4("projection", projection);
    textShader_->setMat4("view", view);
    textShader_->setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontManager_->getAtlasTextureID());
    textShader_->setInt("textAtlas", 0);

    glBindVertexArray(VAO_);

    float currentOffsetX = 0.0f;
    float fontScale = scale / fontManager_->getFontSize(); // Normalize scale based on font size

    std::vector<TextVertex> vertices;
    vertices.reserve(text.length() * 6);

    for (char c : text) {
        const CharacterInfo& ch = fontManager_->getCharacter(c);

        float x_offset = ch.bearing.x * fontScale;
        float y_offset = (ch.bearing.y - ch.size.y) * fontScale; // Adjust for baseline, characters drawn above baseline

        float w = ch.size.x * fontScale;
        float h = ch.size.y * fontScale;

        glm::vec3 v_bl = position + (currentOffsetX + x_offset) * cameraRight + y_offset * cameraUp; // Bottom-left
        glm::vec3 v_tl = position + (currentOffsetX + x_offset) * cameraRight + (y_offset + h) * cameraUp; // Top-left
        glm::vec3 v_br = position + (currentOffsetX + x_offset + w) * cameraRight + y_offset * cameraUp; // Bottom-right
        glm::vec3 v_tr = position + (currentOffsetX + x_offset + w) * cameraRight + (y_offset + h) * cameraUp; // Top-right

        vertices.push_back({glm::vec2(v_tl.x, v_tl.y), {ch.uv_y0.x, ch.uv_y0.y}});
        vertices.push_back({glm::vec2(v_bl.x, v_bl.y), {ch.uv_x0.x, ch.uv_x0.y}});
        vertices.push_back({glm::vec2(v_br.x, v_br.y), {ch.uv_y1.x, ch.uv_y1.y}});

        vertices.push_back({glm::vec2(v_tl.x, v_tl.y), {ch.uv_y0.x, ch.uv_y0.y}});
        vertices.push_back({glm::vec2(v_br.x, v_br.y), {ch.uv_y1.x, ch.uv_y1.y}});
        vertices.push_back({glm::vec2(v_tr.x, v_tr.y), {ch.uv_x1.x, ch.uv_x1.y}});

        currentOffsetX += (ch.advance >> 6) * fontScale; // Advance is 1/64th of a pixel
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextVertex), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace Rendering
} // namespace VoxelEngine
