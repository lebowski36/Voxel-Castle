#include "ui/elements/UIButton.h"
#include "ui/UIRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace VoxelEngine {
namespace UI {

UIButton::UIButton(UIRenderer* renderer) : UIElement(), renderer_(renderer) {
    // Default size for buttons
    setSize(120.0f, 40.0f);
}

void UIButton::setText(const std::string& text) {
    text_ = text;
}

void UIButton::setBackgroundColor(const glm::vec4& color) {
    backgroundColor_ = color;
}

void UIButton::setTextColor(const glm::vec4& color) {
    textColor_ = color;
}

void UIButton::setHoverColor(const glm::vec4& color) {
    hoverColor_ = color;
}

void UIButton::setClickColor(const glm::vec4& color) {
    clickColor_ = color;
}

void UIButton::setOnClick(std::function<void()> callback) {
    onClick_ = callback;
}

void UIButton::update(float deltaTime) {
    // No animation updates for now
}

void UIButton::render() {
    if (!isVisible()) {
        return;
    }

    // Get access to the renderer from the UISystem
    // For now, we'll implement a simplified version
    
    glm::vec2 pos = getAbsolutePosition();
    
    // Determine which color to use based on button state
    glm::vec4 bgColor;
    if (isPressed_) {
        bgColor = clickColor_;
    } else if (isHovered_) {
        bgColor = hoverColor_;
    } else {
        bgColor = backgroundColor_;
    }
    
    // The actual implementation would use the UIRenderer to draw the button
    // This is a placeholder that will be implemented later
    // For now, this just means the button won't be visible
    
    // TODO: Implement proper rendering using UIRenderer methods
    std::cout << "[UIButton] render() called for button: " << text_ << " at position: " 
              << pos.x << ", " << pos.y << " with color: " 
              << bgColor.r << ", " << bgColor.g << ", " << bgColor.b << std::endl;
}

bool UIButton::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!isVisible()) {
        return false;
    }
    
    // Check if mouse is over button
    bool wasHovered = isHovered_;
    isHovered_ = containsPoint(mouseX, mouseY);
    
    // Handle hover state change
    if (isHovered_ != wasHovered) {
        // Could trigger hover sound or animation here
    }
    
    // Handle click
    if (isHovered_ && clicked) {
        isPressed_ = true;
        
        // Call onClick callback if provided
        if (onClick_) {
            onClick_();
        }
        
        return true;
    } else {
        isPressed_ = false;
    }
    
    return isHovered_;
}

} // namespace UI
} // namespace VoxelEngine
