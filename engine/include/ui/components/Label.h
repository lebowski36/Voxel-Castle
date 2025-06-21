#pragma once

#include "ui/core/UIComponent.h"
#include "ui/core/UITheme.h"
#include <string>

namespace VoxelEngine {
namespace UI {

/**
 * @brief Text display component
 * 
 * Displays text with customizable styling including:
 * - Font size and color
 * - Text alignment
 * - Auto-sizing based on content
 * - Text wrapping support
 */
class Label : public UIComponent {
public:
    enum class TextAlign {
        LEFT,
        CENTER,
        RIGHT
    };
    
    explicit Label(const std::string& text = "");
    ~Label() override = default;
    
    // Text content
    void SetText(const std::string& text);
    const std::string& GetText() const { return text_; }
    
    // Text styling
    void SetFontSize(float fontSize);
    float GetFontSize() const { return fontSize_; }
    
    void SetTextColor(const glm::vec4& color);
    const glm::vec4& GetTextColor() const { return textColor_; }
    
    void SetTextAlign(TextAlign align);
    TextAlign GetTextAlign() const { return textAlign_; }
    
    void SetWordWrap(bool wordWrap);
    bool GetWordWrap() const { return wordWrap_; }
    
    // Auto-sizing
    void SetAutoSize(bool autoSize);
    bool GetAutoSize() const { return autoSize_; }
    
    // UIComponent overrides
    glm::vec2 CalculatePreferredSize() const override;
    
protected:
    void OnRender() override;
    
private:
    void RecalculateSize();
    glm::vec2 MeasureText() const;
    glm::vec2 CalculateTextPosition() const;
    
    std::string text_;
    float fontSize_;
    glm::vec4 textColor_;
    TextAlign textAlign_;
    bool wordWrap_;
    bool autoSize_;
    
    // Cached values
    mutable glm::vec2 measuredTextSize_;
    mutable bool textSizeDirty_;
};

} // namespace UI
} // namespace VoxelEngine
