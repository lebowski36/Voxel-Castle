#pragma once

#include "ui/core/UIComponent.h"
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

/**
 * @brief Text input field with cursor, selection and validation
 * 
 * Features:
 * - Text editing with cursor positioning
 * - Text selection support
 * - Placeholder text
 * - Input validation
 * - Focus states and styling
 * - Character limits
 */
class TextInput : public UIComponent {
public:
    TextInput(const std::string& placeholder = "");
    ~TextInput() override = default;
    
    // UIComponent overrides
    void Initialize() override;
    glm::vec2 CalculatePreferredSize() const override;
    bool CanReceiveFocus() const override { return IsEnabled(); }
    bool IsFocused() const override { return focused_; }
    
    // Input handling
    bool HandleMouseButton(int button, int action, int mods, const glm::vec2& localPos) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    bool HandleChar(unsigned int codepoint) override;
    
    // Focus events
    void OnFocusGained() override;
    void OnFocusLost() override;
    
    // Content
    void SetText(const std::string& text);
    const std::string& GetText() const { return text_; }
    
    void SetPlaceholder(const std::string& placeholder);
    const std::string& GetPlaceholder() const { return placeholder_; }
    
    // Styling
    void SetBackgroundColor(const glm::vec4& color);
    const glm::vec4& GetBackgroundColor() const { return backgroundColor_; }
    
    void SetTextColor(const glm::vec4& color);
    const glm::vec4& GetTextColor() const { return textColor_; }
    
    void SetBorderColor(const glm::vec4& color);
    const glm::vec4& GetBorderColor() const { return borderColor_; }
    
    void SetFocusColor(const glm::vec4& color);
    const glm::vec4& GetFocusColor() const { return focusColor_; }
    
    // Input constraints
    void SetMaxLength(int maxLength);
    int GetMaxLength() const { return maxLength_; }
    
    void SetReadOnly(bool readOnly);
    bool IsReadOnly() const { return readOnly_; }
    
    void SetPasswordMode(bool passwordMode);
    bool IsPasswordMode() const { return passwordMode_; }
    
    // Events
    void SetOnTextChanged(std::function<void(const std::string&)> callback);
    void SetOnEnter(std::function<void()> callback);
    void SetOnFocusChanged(std::function<void(bool)> callback);
    
protected:
    void OnRender() override;
    void OnUpdate(float deltaTime) override;
    
private:
    void RenderBackground();
    void RenderText();
    void RenderCursor();
    void RenderSelection();
    
    void MoveCursor(int position);
    void MoveCursorToEnd();
    void InsertCharacter(char c);
    void DeleteCharacter();
    void DeleteSelection();
    
    glm::vec2 GetCursorPosition() const;
    int GetCharacterAtPosition(const glm::vec2& position) const;
    
    // Text content
    std::string text_;
    std::string placeholder_;
    
    // Cursor and selection
    int cursorPosition_;
    int selectionStart_;
    int selectionEnd_;
    bool hasSelection_;
    
    // Visual state
    bool focused_;
    bool hovered_;
    float cursorBlinkTime_;
    bool cursorVisible_;
    
    // Styling
    glm::vec4 backgroundColor_;
    glm::vec4 textColor_;
    glm::vec4 borderColor_;
    glm::vec4 focusColor_;
    glm::vec4 placeholderColor_;
    float borderWidth_;
    float cornerRadius_;
    glm::vec4 padding_;
    
    // Input constraints
    int maxLength_;
    bool readOnly_;
    bool passwordMode_;
    
    // Events
    std::function<void(const std::string&)> onTextChanged_;
    std::function<void()> onEnter_;
    std::function<void(bool)> onFocusChanged_;
    
    // Animation
    float focusAnimation_;
    
    // Constants
    static constexpr float CURSOR_BLINK_RATE = 1.0f; // Blinks per second
    static constexpr float FOCUS_ANIMATION_SPEED = 8.0f;
};

} // namespace UI
} // namespace VoxelEngine
