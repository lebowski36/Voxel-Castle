#pragma once
#include "ui/UIElement.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

class UIRenderer;

class UITexture : public UIElement {
public:
    UITexture(UIRenderer* renderer);
    virtual ~UITexture() = default;

    void setTexture(GLuint textureID);
    void setTextureCoords(const glm::vec4& texCoords); // x, y, width, height in texture coordinates (0-1)
    void setColor(const glm::vec4& color);
    
    GLuint getTexture() const { return textureID_; }
    const glm::vec4& getTextureCoords() const { return textureCoords_; }
    const glm::vec4& getColor() const { return color_; }

    // UIElement interface
    void render() override;

protected:
    UIRenderer* renderer_;
    GLuint textureID_;
    glm::vec4 textureCoords_; // UV coordinates in texture
    glm::vec4 color_;         // Tint color
};

} // namespace UI
} // namespace VoxelEngine
