#include "ui/elements/UIPanel.h"
#include "ui/UIRenderer.h"
#include <iostream>
#include "../../../../game/include/utils/debug_logger.h"

namespace VoxelEngine {
namespace UI {

UIPanel::UIPanel(UIRenderer* renderer) : 
    renderer_(renderer),
    color_(1.0f, 1.0f, 1.0f, 1.0f) // Default to white
{
}

void UIPanel::setColor(const glm::vec4& color) {
    color_ = color;
}

void UIPanel::render() {
    static int logCounter = 0;
    bool shouldLog = (logCounter++ % 30 == 0); // Log every 30 frames
    
    if (!renderer_) {
        if (shouldLog) {
            std::cerr << "[UIPanel] ERROR: Cannot render: renderer is null!" << std::endl;
        }
        return;
    }
    
    if (!isVisible()) {
        return;
    }
    
    glm::vec2 absPos = getAbsolutePosition();
    
    // Check if element is off-screen
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    bool isOffScreen = (absPos.x + size_.x < 0 || absPos.y + size_.y < 0 || 
                       absPos.x > viewport[2] || absPos.y > viewport[3]);
    
    if (isOffScreen && shouldLog) {
        std::cerr << "[UIPanel] WARNING: Element at (" << absPos.x << ", " << absPos.y 
                  << ") may be off-screen. Viewport: " << viewport[2] << "x" << viewport[3] << std::endl;
    }
    
    // Render using the solid color quad method
    renderer_->renderQuad(absPos.x, absPos.y, size_.x, size_.y, color_);
}

bool UIPanel::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!isVisible() || !containsPoint(mouseX, mouseY)) {
        return false;
    }

    // If the panel itself is clicked and contains the point, 
    // it has technically handled the input for its area.
    // Now, iterate through children to see if they handle it more specifically.
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        if (child && child->isVisible()) {
            if (child->handleInput(mouseX, mouseY, clicked)) {
                return true; // Child handled the input
            }
        }
    }
    
    // If no child handled it, but the click was within the panel, 
    // the panel itself can be considered to have handled it (e.g., to prevent clicks passing through).
    // However, for simple panels that are just containers, we might want to return false 
    // if no children handled it, unless the panel itself has specific click behavior.
    // For now, let's assume if a click is on a panel and no child takes it, the panel 'absorbs' it.
    return true; 
}

} // namespace UI
} // namespace VoxelEngine
