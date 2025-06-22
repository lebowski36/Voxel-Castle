#pragma once

#include "ui/core/UIComponent.h"
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

class UITheme;

/**
 * @brief Enhanced button component with modern styling and interactions
 * 
 * Features:
 * - Multiple visual states (normal, hover, pressed, disabled)
 * - Smooth animations and transitions
 * - Customizable styling
 * - Accessibility support
 * - Auto-sizing based on content
 */
class Button : public UIComponent {
public:
    enum class Style {
        PRIMARY,    // Primary action button (blue)
        SECONDARY,  // Secondary action button (purple)
        SUCCESS,    // Success/confirm button (green)
        WARNING,    // Warning button (orange)
        DANGER,     // Dangerous action button (red)
        GHOST       // Transparent button with border
    };
    
    enum class Size {
        SMALL,      // 36px height
        MEDIUM,     // 44px height (default)
        LARGE       // 52px height
    };
    
public:
    Button(const std::string& text = "");
    ~Button() override = default;
    
    // UIComponent overrides
    void Initialize() override;
    glm::vec2 CalculatePreferredSize() const override;
    bool CanReceiveFocus() const override { return IsEnabled(); }
    
    // Input handling
    bool HandleMouseButton(int button, int action, int mods, const glm::vec2& localPos) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    
    // Focus events
    void OnFocusGained() override;
    void OnFocusLost() override;
    
    // Mouse events
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    
    // Configuration
    void SetText(const std::string& text);
    const std::string& GetText() const { return text_; }
    
    void SetStyle(Style style);
    Style GetStyle() const { return style_; }
    
    void SetSize(Size size);
    Size GetSize() const { return buttonSize_; }
    
    void SetOnClick(std::function<void()> onClick);
    
    // Custom styling
    void SetBackgroundColor(const glm::vec4& color);
    void SetTextColor(const glm::vec4& color);
    void SetBorderRadius(float radius);
    
    // State queries
    bool IsHovered() const { return hovered_; }
    bool IsPressed() const { return pressed_; }
    bool IsFocused() const override { return focused_; }
    
protected:
    void OnRender() override;
    void OnUpdate(float deltaTime) override;
    
private:
    std::string text_;
    Style style_;
    Size buttonSize_;
    std::function<void()> onClick_;
    
    // Visual state
    bool hovered_;
    bool pressed_;
    bool focused_;
    
    // Animation state
    float hoverAnimation_;
    float pressAnimation_;
    float focusAnimation_;
    
    // Custom styling
    bool hasCustomBackground_;
    bool hasCustomTextColor_;
    bool hasCustomBorderRadius_;
    glm::vec4 customBackgroundColor_;
    glm::vec4 customTextColor_;
    float customBorderRadius_;
    
    // Helper methods
    glm::vec4 GetCurrentBackgroundColor() const;
    glm::vec4 GetCurrentTextColor() const;
    float GetCurrentBorderRadius() const;
    float GetButtonHeight() const;
    glm::vec2 GetTextPadding() const;
    void UpdateAnimations(float deltaTime);
    void TriggerClick();
    
    // Modern rendering methods
    void RenderModernBackground(glm::vec2 absPos, glm::vec2 size, glm::vec4 baseColor) const;
};

} // namespace UI
} // namespace VoxelEngine
