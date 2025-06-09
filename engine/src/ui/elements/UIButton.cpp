#include "ui/elements/UIButton.h"
#include "ui/UIRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace VoxelEngine {
namespace UI {

UIButton::UIButton(UIRenderer* renderer) : UIElement(), renderer_(renderer) {
    // Default size for buttons
    setSize(120.0f, 40.0f);
    
    // Set default colors
    backgroundColor_ = glm::vec4(0.2f, 0.3f, 0.4f, 1.0f);  // Dark blue-gray
    textColor_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);        // White
    hoverColor_ = glm::vec4(0.3f, 0.4f, 0.5f, 1.0f);       // Lighter blue-gray
    clickColor_ = glm::vec4(0.4f, 0.5f, 0.6f, 1.0f);       // Even lighter blue-gray
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
    if (!isVisible() || !renderer_) {
        return;
    }

    // Debug output for WorldGenerationUI buttons
    static int debugCounter = 0;
    if (text_.find("Start World") != std::string::npos || text_.find("Back to Main") != std::string::npos) {
        if (debugCounter++ % 100 == 0) {
            glm::vec2 pos = getAbsolutePosition();
            std::cout << "[UIButton] Rendering button '" << text_ << "' at (" << pos.x << ", " << pos.y 
                      << ") size: (" << size_.x << ", " << size_.y << ")" << std::endl;
        }
    }

    glm::vec2 pos = getAbsolutePosition();
    glm::vec4 bgColor = isPressed_ ? clickColor_ : (isHovered_ ? hoverColor_ : backgroundColor_);

    renderer_->renderColoredQuad(pos.x, pos.y, size_.x, size_.y, bgColor);

    if (!text_.empty()) {
        float textScale = 1.0f; // Adjust scale as needed
        glm::vec3 textColor = glm::vec3(1.0f); // White text
        
        // Calculate actual text width for proper centering
        float actualTextWidth = renderer_->getTextWidth(text_, textScale);
        float textX = pos.x + (size_.x - actualTextWidth) / 2.0f; // Center horizontally
        float textY = pos.y + size_.y / 2.0f + 8.0f; // Center vertically with slight offset
        
        renderer_->drawText(text_, textX, textY, textScale, textColor);
    }
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
        
        return true; // Indicate that the input was handled
    }
    
    // If not clicked, or not hovered, check if we were pressed and now released
    if (isPressed_ && !clicked && isHovered_) {
        // This is a click release event on the button
        // We already handled the action on press, so just reset pressed state
        isPressed_ = false;
        // Optionally, could have a separate onRelease callback here
    }
    
    // If mouse is not over the button, or if it's not a click event, reset pressed state
    if (!isHovered_ || (!clicked && isPressed_ && !isHovered_)) { 
        // If mouse moves off while pressed, or if released outside
        isPressed_ = false;
    }
    
    return false; // Input not handled by this button (or handled on press)
}

void UIButton::autoSizeToText(float padding, float scale) {
    if (!renderer_ || text_.empty()) {
        return;
    }
    
    // Get text dimensions
    float textWidth = renderer_->getTextWidth(text_, scale);
    float textHeight = renderer_->getTextHeight(scale);
    
    // Set button size with padding
    setSize(textWidth + (padding * 2.0f), textHeight + (padding * 2.0f));
}

} // namespace UI
} // namespace VoxelEngine
