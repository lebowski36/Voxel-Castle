#include "ui/elements/UICheckbox.h"
#include "ui/UIRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

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
    if (!isVisible()) {
        return;
    }

    glm::vec2 pos = getAbsolutePosition();
    
    // Box size (square)
    float boxSize = 20.0f;
    
    // The actual implementation would use the UIRenderer to draw the checkbox
    // This is a placeholder that will be implemented later
    // For now, this just means the checkbox won't be visible
    
    // TODO: Implement proper rendering using UIRenderer methods
    std::cout << "[UICheckbox] render() called for checkbox: " << label_ 
              << " at position: " << pos.x << ", " << pos.y
              << " checked: " << (checked_ ? "true" : "false") << std::endl;
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
