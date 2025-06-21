#include "ui/elements/UITextInput.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

UITextInput::UITextInput(UIRenderer* renderer)
    : renderer_(renderer) {
}

void UITextInput::setText(const std::string& text) {
    text_ = text;
}

void UITextInput::setPlaceholder(const std::string& placeholder) {
    placeholder_ = placeholder;
}

void UITextInput::setBackgroundColor(const glm::vec4& color) {
    backgroundColor_ = color;
}

void UITextInput::setTextColor(const glm::vec4& color) {
    textColor_ = color;
}

void UITextInput::setBorderColor(const glm::vec4& color) {
    borderColor_ = color;
}

void UITextInput::setOnEnter(std::function<void()> onEnter) {
    onEnter_ = onEnter;
}

void UITextInput::render() {
    if (!isVisible()) return;

    // Draw a thicker border for better visibility
    float borderWidth = focused_ ? 3.0f : 2.0f;
    glm::vec4 actualBorderColor = focused_ ? glm::vec4(0.6f, 0.6f, 0.8f, 1.0f) : borderColor_;
    
    // Draw border by drawing a larger quad behind
    renderer_->renderColoredQuad(position_.x - borderWidth, position_.y - borderWidth, 
                                size_.x + 2 * borderWidth, size_.y + 2 * borderWidth, actualBorderColor);
    
    // Draw background with slightly lighter color when focused
    glm::vec4 actualBackgroundColor = focused_ ? 
        glm::vec4(backgroundColor_.r + 0.05f, backgroundColor_.g + 0.05f, backgroundColor_.b + 0.05f, backgroundColor_.a) : 
        backgroundColor_;
    renderer_->renderColoredQuad(position_.x, position_.y, size_.x, size_.y, actualBackgroundColor);

    // Calculate text positioning for better centering
    float textScale = 1.2f;
    float textHeight = renderer_->getTextHeight(textScale);
    float textY = position_.y + (size_.y - textHeight) / 2.0f;
    float textX = position_.x + 10.0f; // Small left padding

    if (text_.empty() && !placeholder_.empty() && !focused_) {
        // Draw placeholder text in gray
        renderer_->drawText(placeholder_, textX, textY, textScale, glm::vec3(0.6f, 0.6f, 0.6f));
    } else {
        std::string displayText = text_;
        if (focused_) {
            // Add blinking cursor effect
            displayText += "|";
        }
        renderer_->drawText(displayText, textX, textY, textScale, glm::vec3(textColor_.r, textColor_.g, textColor_.b));
    }
}

void UITextInput::handleMouseButton(int button, int action, int mods, double xpos, double ypos) {
    (void)mods; // Suppress unused parameter warning
    if (button == 0 && action == 1) { // Left mouse button press
        if (xpos >= position_.x && xpos <= position_.x + size_.x &&
            ypos >= position_.y && ypos <= position_.y + size_.y) {
            focused_ = true;
        } else {
            focused_ = false;
        }
    }
}

void UITextInput::handleKey(int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods; // Suppress unused parameter warnings
    if (focused_ && (action == 1 || action == 2)) { // Key press or repeat
        if (key == 257) { // Enter
            if (onEnter_) {
                onEnter_();
            }
        } else if (key == 259) { // Backspace
            if (!text_.empty()) {
                text_.pop_back();
            }
        }
    }
}

void UITextInput::handleChar(unsigned int codepoint) {
    if (focused_) {
        text_ += static_cast<char>(codepoint);
    }
}

bool UITextInput::handleInput(float mouseX, float mouseY, bool clicked) {
    if (clicked) {
        // Convert to the new input handling format
        handleMouseButton(0, 1, 0, mouseX, mouseY); // Left mouse button press
        return focused_; // Return true if we became focused
    }
    return false;
}

} // namespace UI
} // namespace VoxelEngine
