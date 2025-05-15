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

    // OpenGL state for text rendering - Save current state
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLint last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha;
    glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
    GLboolean last_depth_mask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &last_depth_mask);

    // Apply text rendering states
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (last_enable_cull_face) glDisable(GL_CULL_FACE); 
    // if (last_enable_depth_test) glDisable(GL_DEPTH_TEST); // Keep depth test enabled for 3D
    glDepthMask(GL_FALSE); // Don't write to depth buffer, but still test against it

    glBindVertexArray(VAO_);

    std::vector<TextVertex> vertices;
    vertices.reserve(text.length() * 6);
    float currentOffsetX = 0.0f;

    for (char c : text) {
        const CharacterInfo& ch = fontManager_->getCharacter(c);

        // Local quad coordinates (before billboarding rotation and world translation)
        float x_local = currentOffsetX + ch.bearing.x * scale;
        float y_local = (ch.bearing.y - ch.size.y) * scale; // Y position of the bottom-left corner
        float w_local = ch.size.x * scale;
        float h_local = ch.size.y * scale;

        // Define 2D local quad vertices
        glm::vec2 bl_local = glm::vec2(x_local, y_local);
        glm::vec2 tl_local = glm::vec2(x_local, y_local + h_local);
        glm::vec2 br_local = glm::vec2(x_local + w_local, y_local);
        glm::vec2 tr_local = glm::vec2(x_local + w_local, y_local + h_local);

        // Add to vertex buffer - these are local 2D coordinates for the shader
        vertices.push_back({tl_local, {ch.uv_y0.x, ch.uv_y0.y}}); // Top-left
        vertices.push_back({bl_local, {ch.uv_x0.x, ch.uv_x0.y}}); // Bottom-left
        vertices.push_back({br_local, {ch.uv_y1.x, ch.uv_y1.y}}); // Bottom-right

        vertices.push_back({tl_local, {ch.uv_y0.x, ch.uv_y0.y}}); // Top-left
        vertices.push_back({br_local, {ch.uv_y1.x, ch.uv_y1.y}}); // Bottom-right
        vertices.push_back({tr_local, {ch.uv_x1.x, ch.uv_x1.y}}); // Top-right
        
        currentOffsetX += (ch.advance >> 6) * scale;
    }
    
    // Construct the model matrix for billboarding and positioning
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    // Set rotation part for billboarding
    modelMatrix[0] = glm::vec4(cameraRight, 0.0f);
    modelMatrix[1] = glm::vec4(cameraUp, 0.0f);
    modelMatrix[2] = glm::vec4(glm::normalize(glm::cross(cameraUp, cameraRight)), 0.0f); // Ensure Z is normalized and correct direction
    // Set translation part
    modelMatrix[3] = glm::vec4(position, 1.0f);
    
    // Apply overall scale to the model matrix if text is too small/large globally
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(overallTextScale, overallTextScale, overallTextScale));

    textShader_->setMat4("model", modelMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextVertex), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore OpenGL state
    if (last_depth_mask) { glDepthMask(GL_TRUE); } else { glDepthMask(GL_FALSE); }
    if (!last_enable_blend) glDisable(GL_BLEND);
    else glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    
    if (last_enable_cull_face) glEnable(GL_CULL_FACE);
    // if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); // Depth test was not disabled
}

} // namespace Rendering
} // namespace VoxelEngine
