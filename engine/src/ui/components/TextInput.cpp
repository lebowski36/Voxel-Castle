#include "ui/components/TextInput.h"
#include "ui/core/UISystem.h"
#include "ui/core/UITheme.h"
#include "ui/UIRenderer.h"
#include <algorithm>
#include <iostream>

// GLFW key codes
#define GLFW_KEY_BACKSPACE    259
#define GLFW_KEY_DELETE       261
#define GLFW_KEY_ENTER        257
#define GLFW_KEY_LEFT         263
#define GLFW_KEY_RIGHT        262
#define GLFW_KEY_HOME         268
#define GLFW_KEY_END          269
#define GLFW_KEY_A            65
#define GLFW_MOD_CONTROL      0x0002

namespace VoxelEngine {
namespace UI {

TextInput::TextInput(const std::string& placeholder)
    : text_("")
    , placeholder_(placeholder)
    , cursorPosition_(0)
    , selectionStart_(0)
    , selectionEnd_(0)
    , hasSelection_(false)
    , focused_(false)
    , hovered_(false)
    , cursorBlinkTime_(0.0f)
    , cursorVisible_(true)
    , backgroundColor_(0.15f, 0.15f, 0.2f, 1.0f)
    , textColor_(1.0f, 1.0f, 1.0f, 1.0f)
    , borderColor_(0.4f, 0.4f, 0.5f, 1.0f)
    , focusColor_(0.3f, 0.6f, 1.0f, 1.0f)
    , placeholderColor_(0.6f, 0.6f, 0.6f, 1.0f)
    , borderWidth_(2.0f)
    , cornerRadius_(6.0f)
    , padding_(12.0f, 8.0f, 12.0f, 8.0f)
    , maxLength_(1000)
    , readOnly_(false)
    , passwordMode_(false)
    , focusAnimation_(0.0f) {
}

void TextInput::Initialize() {
    UIComponent::Initialize();
    
    // Set default size
    SetSize(glm::vec2(200.0f, 40.0f));
    
    std::cout << "[TextInput] Initialized with placeholder: '" << placeholder_ << "'" << std::endl;
}

glm::vec2 TextInput::CalculatePreferredSize() const {
    auto uiSystem = GetUISystem();
    if (!uiSystem || !uiSystem->GetRenderer()) {
        return glm::vec2(200, 40); // Fallback size
    }
    
    auto theme = uiSystem->GetTheme();
    float fontSize = theme->typography.body;
    float height = fontSize * 1.5f + padding_.y + padding_.w + borderWidth_ * 2;
    
    return glm::vec2(200, height);
}

bool TextInput::HandleMouseButton(int button, int action, int mods, const glm::vec2& localPos) {
    if (button == 0 && action == 1) { // Left mouse button press
        // Request focus
        auto uiSystem = GetUISystem();
        if (uiSystem) {
            uiSystem->SetFocus(shared_from_this());
        }
        
        // Position cursor at click location
        int charIndex = GetCharacterAtPosition(localPos);
        MoveCursor(charIndex);
        
        return true;
    }
    
    return false;
}

bool TextInput::HandleKey(int key, int scancode, int action, int mods) {
    if (!focused_ || readOnly_) return false;
    
    if (action == 1 || action == 2) { // Key press or repeat
        switch (key) {
            case GLFW_KEY_BACKSPACE:
                if (hasSelection_) {
                    DeleteSelection();
                } else if (cursorPosition_ > 0) {
                    DeleteCharacter();
                }
                return true;
                
            case GLFW_KEY_DELETE:
                if (hasSelection_) {
                    DeleteSelection();
                } else if (cursorPosition_ < text_.length()) {
                    text_.erase(cursorPosition_, 1);
                    if (onTextChanged_) {
                        onTextChanged_(text_);
                    }
                }
                return true;
                
            case GLFW_KEY_ENTER:
                if (onEnter_) {
                    onEnter_();
                }
                return true;
                
            case GLFW_KEY_LEFT:
                if (cursorPosition_ > 0) {
                    MoveCursor(cursorPosition_ - 1);
                }
                return true;
                
            case GLFW_KEY_RIGHT:
                if (cursorPosition_ < text_.length()) {
                    MoveCursor(cursorPosition_ + 1);
                }
                return true;
                
            case GLFW_KEY_HOME:
                MoveCursor(0);
                return true;
                
            case GLFW_KEY_END:
                MoveCursorToEnd();
                return true;
                
            case GLFW_KEY_A:
                if (mods & GLFW_MOD_CONTROL) {
                    // Select all
                    selectionStart_ = 0;
                    selectionEnd_ = text_.length();
                    hasSelection_ = true;
                }
                return true;
        }
    }
    
    return false;
}

bool TextInput::HandleChar(unsigned int codepoint) {
    if (!focused_ || readOnly_) return false;
    
    // Only accept printable ASCII characters for now
    if (codepoint >= 32 && codepoint <= 126) {
        if (text_.length() < maxLength_) {
            InsertCharacter(static_cast<char>(codepoint));
        }
        return true;
    }
    
    return false;
}

void TextInput::OnFocusGained() {
    focused_ = true;
    cursorBlinkTime_ = 0.0f;
    cursorVisible_ = true;
    
    if (onFocusChanged_) {
        onFocusChanged_(true);
    }
    
    std::cout << "[TextInput] Focus gained" << std::endl;
}

void TextInput::OnFocusLost() {
    focused_ = false;
    hasSelection_ = false;
    
    if (onFocusChanged_) {
        onFocusChanged_(false);
    }
    
    std::cout << "[TextInput] Focus lost" << std::endl;
}

void TextInput::SetText(const std::string& text) {
    text_ = text.substr(0, maxLength_);
    cursorPosition_ = std::min(cursorPosition_, static_cast<int>(text_.length()));
    hasSelection_ = false;
    
    if (onTextChanged_) {
        onTextChanged_(text_);
    }
}

void TextInput::SetPlaceholder(const std::string& placeholder) {
    placeholder_ = placeholder;
}

void TextInput::SetBackgroundColor(const glm::vec4& color) {
    backgroundColor_ = color;
}

void TextInput::SetTextColor(const glm::vec4& color) {
    textColor_ = color;
}

void TextInput::SetBorderColor(const glm::vec4& color) {
    borderColor_ = color;
}

void TextInput::SetFocusColor(const glm::vec4& color) {
    focusColor_ = color;
}

void TextInput::SetMaxLength(int maxLength) {
    maxLength_ = std::max(0, maxLength);
    if (text_.length() > maxLength_) {
        SetText(text_.substr(0, maxLength_));
    }
}

void TextInput::SetReadOnly(bool readOnly) {
    readOnly_ = readOnly;
}

void TextInput::SetPasswordMode(bool passwordMode) {
    passwordMode_ = passwordMode;
}

void TextInput::SetOnTextChanged(std::function<void(const std::string&)> callback) {
    onTextChanged_ = callback;
}

void TextInput::SetOnEnter(std::function<void()> callback) {
    onEnter_ = callback;
}

void TextInput::SetOnFocusChanged(std::function<void(bool)> callback) {
    onFocusChanged_ = callback;
}

void TextInput::OnRender() {
    RenderBackground();
    RenderText();
    
    if (focused_) {
        if (hasSelection_) {
            RenderSelection();
        }
        RenderCursor();
    }
}

void TextInput::OnUpdate(float deltaTime) {
    // Update focus animation
    float targetFocus = focused_ ? 1.0f : 0.0f;
    focusAnimation_ += (targetFocus - focusAnimation_) * FOCUS_ANIMATION_SPEED * deltaTime;
    focusAnimation_ = std::clamp(focusAnimation_, 0.0f, 1.0f);
    
    // Update cursor blink
    if (focused_) {
        cursorBlinkTime_ += deltaTime;
        if (cursorBlinkTime_ >= 1.0f / CURSOR_BLINK_RATE) {
            cursorVisible_ = !cursorVisible_;
            cursorBlinkTime_ = 0.0f;
        }
    }
}

void TextInput::RenderBackground() {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto renderer = uiSystem->GetRenderer();
    if (!renderer) return;
    
    glm::vec2 pos = GetAbsolutePosition();
    glm::vec2 size = GetSize();
    float scale = uiSystem->GetTotalScale();
    
    // Apply scaling
    pos *= scale;
    size *= scale;
    float scaledBorderWidth = borderWidth_ * scale;
    
    // Interpolate border color with focus color
    glm::vec4 currentBorderColor = glm::mix(borderColor_, focusColor_, focusAnimation_);
    
    // Render border
    renderer->renderColoredQuad(pos.x - scaledBorderWidth, pos.y - scaledBorderWidth, 
                               size.x + 2 * scaledBorderWidth, size.y + 2 * scaledBorderWidth, 
                               currentBorderColor);
    
    // Render background
    renderer->renderColoredQuad(pos.x, pos.y, size.x, size.y, backgroundColor_);
}

void TextInput::RenderText() {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto renderer = uiSystem->GetRenderer();
    if (!renderer) return;
    
    std::string displayText;
    glm::vec4 textColor;
    
    if (text_.empty() && !placeholder_.empty() && !focused_) {
        displayText = placeholder_;
        textColor = placeholderColor_;
    } else {
        displayText = passwordMode_ ? std::string(text_.length(), '*') : text_;
        textColor = textColor_;
    }
    
    if (displayText.empty()) return;
    
    glm::vec2 pos = GetAbsolutePosition();
    glm::vec2 size = GetSize();
    float scale = uiSystem->GetTotalScale();
    
    // Calculate text position with padding
    float textX = pos.x + padding_.x;
    float textY = pos.y + (size.y - 16.0f) * 0.5f; // Center vertically
    
    // Render text
    renderer->drawText(displayText, textX, textY, 1.0f, glm::vec3(textColor.r, textColor.g, textColor.b));
}

void TextInput::RenderCursor() {
    if (!cursorVisible_ || readOnly_) return;
    
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto renderer = uiSystem->GetRenderer();
    if (!renderer) return;
    
    glm::vec2 cursorPos = GetCursorPosition();
    glm::vec2 pos = GetAbsolutePosition();
    float scale = uiSystem->GetTotalScale();
    
    // Render cursor as thin line
    float cursorWidth = 1.0f * scale;
    float cursorHeight = 20.0f * scale;
    
    renderer->renderColoredQuad(
        pos.x + cursorPos.x,
        pos.y + (GetSize().y - cursorHeight / scale) * 0.5f,
        cursorWidth,
        cursorHeight,
        textColor_
    );
}

void TextInput::RenderSelection() {
    // TODO: Implement selection rendering
}

void TextInput::MoveCursor(int position) {
    cursorPosition_ = std::clamp(position, 0, static_cast<int>(text_.length()));
    hasSelection_ = false;
    cursorBlinkTime_ = 0.0f;
    cursorVisible_ = true;
}

void TextInput::MoveCursorToEnd() {
    MoveCursor(text_.length());
}

void TextInput::InsertCharacter(char c) {
    if (hasSelection_) {
        DeleteSelection();
    }
    
    text_.insert(cursorPosition_, 1, c);
    cursorPosition_++;
    
    if (onTextChanged_) {
        onTextChanged_(text_);
    }
}

void TextInput::DeleteCharacter() {
    if (cursorPosition_ > 0) {
        text_.erase(cursorPosition_ - 1, 1);
        cursorPosition_--;
        
        if (onTextChanged_) {
            onTextChanged_(text_);
        }
    }
}

void TextInput::DeleteSelection() {
    if (!hasSelection_) return;
    
    int start = std::min(selectionStart_, selectionEnd_);
    int end = std::max(selectionStart_, selectionEnd_);
    
    text_.erase(start, end - start);
    cursorPosition_ = start;
    hasSelection_ = false;
    
    if (onTextChanged_) {
        onTextChanged_(text_);
    }
}

glm::vec2 TextInput::GetCursorPosition() const {
    auto uiSystem = GetUISystem();
    if (!uiSystem || !uiSystem->GetRenderer()) {
        return glm::vec2(padding_.x, 0);
    }
    
    auto renderer = uiSystem->GetRenderer();
    
    // Get text up to cursor position
    std::string textToCursor = text_.substr(0, cursorPosition_);
    if (passwordMode_) {
        textToCursor = std::string(textToCursor.length(), '*');
    }
    
    float textWidth = renderer->getTextWidth(textToCursor, 1.0f);
    
    return glm::vec2(padding_.x + textWidth, 0);
}

int TextInput::GetCharacterAtPosition(const glm::vec2& position) const {
    auto uiSystem = GetUISystem();
    if (!uiSystem || !uiSystem->GetRenderer()) {
        return 0;
    }
    
    auto renderer = uiSystem->GetRenderer();
    
    float targetX = position.x - padding_.x;
    
    // Find character closest to the x position
    for (int i = 0; i <= text_.length(); ++i) {
        std::string textToPos = text_.substr(0, i);
        if (passwordMode_) {
            textToPos = std::string(textToPos.length(), '*');
        }
        
        float textWidth = renderer->getTextWidth(textToPos, 1.0f);
        if (textWidth >= targetX) {
            return i;
        }
    }
    
    return text_.length();
}

} // namespace UI
} // namespace VoxelEngine
