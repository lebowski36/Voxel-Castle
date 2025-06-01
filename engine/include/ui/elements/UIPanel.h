#pragma once
#include "ui/UIElement.h"
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

class UIRenderer;

/**
 * A simple solid color rectangle UI element for testing and basic UI needs.
 * Renders without textures, using only solid colors.
 */
class UIPanel : public UIElement {
public:
    UIPanel(UIRenderer* renderer);
    virtual ~UIPanel() = default;

    void setColor(const glm::vec4& color);
    const glm::vec4& getColor() const { return color_; }

    // UIElement interface
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

protected:
    UIRenderer* renderer_;
    glm::vec4 color_; // RGBA color
};

} // namespace UI
} // namespace VoxelEngine
