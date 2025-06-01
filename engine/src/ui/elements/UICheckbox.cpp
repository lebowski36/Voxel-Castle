#include "ui/elements/UICheckbox.h"
#include "ui/UIRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "../../../../game/include/utils/debug_logger.h"

namespace VoxelEngine {
namespace UI {

UICheckbox::UICheckbox(UIRenderer* renderer) : UIElement(), renderer_(renderer) {
    // Default size for checkbox
    setSize(200.0f, 24.0f);
}

void UICheckbox::setLabel(const std::string& label) {
    label_ = label;
}

void UICheckbox::setChecked(bool checked) {
    checked_ = checked;
}

void UICheckbox::setOnToggle(std::function<void(bool)> callback) {
    onToggle_ = callback;
}

void UICheckbox::update(float deltaTime) {
    // No animation updates for now
}

void UICheckbox::render() {
    if (!isVisible() || !renderer_) {
        return;
    }

    glm::vec2 pos = getAbsolutePosition();
    
    // Box size (square)
    float boxSize = 20.0f;
    
    // Draw checkbox border - dark gray
    renderer_->renderColoredQuad(pos.x, pos.y, boxSize, boxSize, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    
    // Draw checkbox inner area - slightly lighter gray
    renderer_->renderColoredQuad(pos.x + 2, pos.y + 2, boxSize - 4, boxSize - 4, 
                               glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
    
    // If checked, draw a checkmark (simple colored box for now)
    if (checked_) {
        renderer_->renderColoredQuad(pos.x + 4, pos.y + 4, boxSize - 8, boxSize - 8,
                                   glm::vec4(0.2f, 0.6f, 0.9f, 1.0f)); // Blue check
    }

    // Render checkbox label
    float textScale = 1.0f; // Adjust scale as needed
    glm::vec3 textColor = glm::vec3(1.0f); // White text
    renderer_->drawText(label_, pos.x + boxSize + 10.0f, pos.y + boxSize / 2.0f, textScale, textColor);
    
    // Debug output - keep for now
    DEBUG_LOG("UICheckbox", "Rendering checkbox: \"" + label_ + "\" at position: " + std::to_string(pos.x) + ", " + std::to_string(pos.y));
}

bool UICheckbox::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!isVisible()) {
        return false;
    }
    
    // Check if mouse is over checkbox
    bool wasHovered = isHovered_;
    isHovered_ = containsPoint(mouseX, mouseY);
    
    // Handle hover state change
    if (isHovered_ != wasHovered) {
        // Could trigger hover effect here
    }
    
    // Handle click to toggle checkbox
    if (isHovered_ && clicked) {
        checked_ = !checked_;
        
        // Call onToggle callback if provided
        if (onToggle_) {
            onToggle_(checked_);
        }
        
        return true;
    }
    
    return isHovered_;
}

} // namespace UI
} // namespace VoxelEngine
