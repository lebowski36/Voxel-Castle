#include "ui/components/Button.h"
#include "ui/core/UISystem.h"
#include "ui/core/UITheme.h"
#include "ui/UIRenderer.h"
#include <algorithm>
#include <iostream>

namespace VoxelEngine {
namespace UI {

Button::Button(const std::string& text)
    : text_(text)
    , style_(Style::PRIMARY)
    , buttonSize_(Size::MEDIUM)
    , hovered_(false)
    , pressed_(false)
    , focused_(false)
    , hoverAnimation_(0.0f)
    , pressAnimation_(0.0f)
    , focusAnimation_(0.0f)
    , hasCustomBackground_(false)
    , hasCustomTextColor_(false)
    , hasCustomBorderRadius_(false)
    , customBackgroundColor_(0)
    , customTextColor_(0)
    , customBorderRadius_(0) {
}

void Button::Initialize() {
    UIComponent::Initialize();
    
    // Set initial size based on content
    glm::vec2 preferredSize = CalculatePreferredSize();
    UIComponent::SetSize(preferredSize);
    
    std::cout << "[Button] Initialized button: '" << text_ << "' size: " 
              << preferredSize.x << "x" << preferredSize.y << std::endl;
}

glm::vec2 Button::CalculatePreferredSize() const {
    auto uiSystem = GetUISystem();
    if (!uiSystem || !uiSystem->GetRenderer()) {
        return glm::vec2(100, GetButtonHeight()); // Fallback size
    }
    
    auto theme = uiSystem->GetTheme();
    auto renderer = uiSystem->GetRenderer();
    
    // Calculate text size
    float textWidth = renderer->getTextWidth(text_, 1.0f); // Scale will be applied later
    
    // Add padding
    glm::vec2 padding = GetTextPadding();
    
    // Calculate final size
    float width = textWidth + padding.x * 2;
    float height = GetButtonHeight();
    
    // Apply minimum width
    float minWidth = 100.0f; // Minimum button width
    width = std::max(width, minWidth);
    
    // Convert to screen pixels for accurate sizing
    return uiSystem->LogicalToScreen(glm::vec2(width, height));
}

bool Button::HandleMouseButton(int button, int action, int mods, const glm::vec2& localPos) {
    if (!IsEnabled()) return false;
    
    if (button == 0) { // Left mouse button
        if (action == 1) { // Press
            pressed_ = true;
            std::cout << "[Button] '" << text_ << "' pressed" << std::endl;
            return true;
        } else if (action == 0) { // Release
            if (pressed_ && hovered_) {
                TriggerClick();
            }
            pressed_ = false;
            return true;
        }
    }
    
    return false;
}

bool Button::HandleKey(int key, int scancode, int action, int mods) {
    if (!IsEnabled() || !focused_) return false;
    
    if ((key == 257 || key == 32) && action == 1) { // Enter or Space
        TriggerClick();
        return true;
    }
    
    return false;
}

void Button::OnFocusGained() {
    focused_ = true;
    std::cout << "[Button] '" << text_ << "' focused" << std::endl;
}

void Button::OnFocusLost() {
    focused_ = false;
    pressed_ = false; // Cancel any press state
}

void Button::OnMouseEnter() {
    hovered_ = true;
}

void Button::OnMouseLeave() {
    hovered_ = false;
    pressed_ = false; // Cancel press state when mouse leaves
}

void Button::SetText(const std::string& text) {
    if (text_ != text) {
        text_ = text;
        InvalidateLayout(); // Text change may affect size
    }
}

void Button::SetStyle(Style style) {
    if (style_ != style) {
        style_ = style;
        // Visual change only, no layout invalidation needed
    }
}

void Button::SetSize(Size size) {
    if (buttonSize_ != size) {
        buttonSize_ = size;
        InvalidateLayout();
    }
}

void Button::SetOnClick(std::function<void()> onClick) {
    onClick_ = onClick;
}

void Button::SetBackgroundColor(const glm::vec4& color) {
    customBackgroundColor_ = color;
    hasCustomBackground_ = true;
}

void Button::SetTextColor(const glm::vec4& color) {
    customTextColor_ = color;
    hasCustomTextColor_ = true;
}

void Button::SetBorderRadius(float radius) {
    customBorderRadius_ = radius;
    hasCustomBorderRadius_ = true;
}

void Button::OnRender() {
    auto uiSystem = GetUISystem();
    if (!uiSystem || !uiSystem->GetRenderer()) return;
    
    auto renderer = uiSystem->GetRenderer();
    auto theme = uiSystem->GetTheme();
    
    // Get current colors based on state
    glm::vec4 backgroundColor = GetCurrentBackgroundColor();
    glm::vec4 textColor = GetCurrentTextColor();
    // float borderRadius = GetCurrentBorderRadius(); // TODO: Implement rounded corners
    
    // Get absolute position and size in screen coordinates
    glm::vec2 absPos = uiSystem->LogicalToScreen(GetAbsolutePosition());
    glm::vec2 size = uiSystem->LogicalToScreen(UIComponent::GetSize());
    
    // Draw button background
    renderer->renderColoredQuad(absPos.x, absPos.y, size.x, size.y, backgroundColor);
    
    // Draw focus outline if focused
    if (focused_ && focusAnimation_ > 0.01f) {
        glm::vec4 focusColor = theme->colors.focus;
        focusColor.a *= focusAnimation_;
        float outlineWidth = 2.0f;
        
        // Draw outline as border
        renderer->renderColoredQuad(
            absPos.x - outlineWidth, absPos.y - outlineWidth,
            size.x + outlineWidth * 2, size.y + outlineWidth * 2,
            focusColor
        );
    }
    
    // Draw text
    if (!text_.empty()) {
        float fontSize = theme->typography.button;
        float scaledFontSize = uiSystem->LogicalToScreen(fontSize);
        
        // Calculate text position (centered)
        float textWidth = renderer->getTextWidth(text_, scaledFontSize / renderer->getFontHeight());
        float textHeight = renderer->getTextHeight(scaledFontSize / renderer->getFontHeight());
        
        float textX = absPos.x + (size.x - textWidth) * 0.5f;
        float textY = absPos.y + (size.y - textHeight) * 0.5f;
        
        renderer->drawText(text_, textX, textY, scaledFontSize / renderer->getFontHeight(), 
                          glm::vec3(textColor.r, textColor.g, textColor.b));
    }
}

void Button::OnUpdate(float deltaTime) {
    UpdateAnimations(deltaTime);
}

glm::vec4 Button::GetCurrentBackgroundColor() const {
    if (hasCustomBackground_) {
        return customBackgroundColor_;
    }
    
    auto theme = GetUISystem()->GetTheme();
    glm::vec4 baseColor;
    
    switch (style_) {
        case Style::PRIMARY:
            baseColor = theme->colors.primaryAccent;
            break;
        case Style::SECONDARY:
            baseColor = theme->colors.secondaryAccent;
            break;
        case Style::SUCCESS:
            baseColor = theme->colors.success;
            break;
        case Style::WARNING:
            baseColor = theme->colors.warning;
            break;
        case Style::DANGER:
            baseColor = theme->colors.error;
            break;
        case Style::GHOST:
            baseColor = glm::vec4(0, 0, 0, 0); // Transparent
            break;
    }
    
    if (!IsEnabled()) {
        baseColor = theme->colors.textDisabled;
    } else {
        // Apply hover and press effects
        if (pressed_ && pressAnimation_ > 0) {
            // Darken for press effect
            baseColor.r *= (1.0f - pressAnimation_ * 0.2f);
            baseColor.g *= (1.0f - pressAnimation_ * 0.2f);
            baseColor.b *= (1.0f - pressAnimation_ * 0.2f);
        } else if (hovered_ && hoverAnimation_ > 0) {
            // Lighten for hover effect
            baseColor.r = std::min(1.0f, baseColor.r * (1.0f + hoverAnimation_ * 0.1f));
            baseColor.g = std::min(1.0f, baseColor.g * (1.0f + hoverAnimation_ * 0.1f));
            baseColor.b = std::min(1.0f, baseColor.b * (1.0f + hoverAnimation_ * 0.1f));
        }
    }
    
    return baseColor;
}

glm::vec4 Button::GetCurrentTextColor() const {
    if (hasCustomTextColor_) {
        return customTextColor_;
    }
    
    auto theme = GetUISystem()->GetTheme();
    
    if (!IsEnabled()) {
        return theme->colors.textDisabled;
    }
    
    return theme->colors.textPrimary;
}

float Button::GetCurrentBorderRadius() const {
    if (hasCustomBorderRadius_) {
        return customBorderRadius_;
    }
    
    auto theme = GetUISystem()->GetTheme();
    return theme->borderRadius.large;
}

float Button::GetButtonHeight() const {
    auto theme = GetUISystem()->GetTheme();
    
    switch (buttonSize_) {
        case Size::SMALL:
            return theme->buttonHeightSmall;
        case Size::MEDIUM:
            return theme->buttonHeightMedium;
        case Size::LARGE:
            return theme->buttonHeightLarge;
    }
    
    return theme->buttonHeightMedium; // Fallback
}

glm::vec2 Button::GetTextPadding() const {
    auto theme = GetUISystem()->GetTheme();
    
    switch (buttonSize_) {
        case Size::SMALL:
            return glm::vec2(theme->spacing.md, theme->spacing.sm);
        case Size::MEDIUM:
            return glm::vec2(theme->spacing.lg, theme->spacing.md);
        case Size::LARGE:
            return glm::vec2(theme->spacing.xl, theme->spacing.lg);
    }
    
    return glm::vec2(theme->spacing.lg, theme->spacing.md); // Fallback
}

void Button::UpdateAnimations(float deltaTime) {
    auto theme = GetUISystem()->GetTheme();
    float animSpeed = 1.0f / theme->animationFast; // Animation speed
    
    // Update hover animation
    float targetHover = hovered_ ? 1.0f : 0.0f;
    hoverAnimation_ += (targetHover - hoverAnimation_) * animSpeed * deltaTime;
    hoverAnimation_ = std::clamp(hoverAnimation_, 0.0f, 1.0f);
    
    // Update press animation
    float targetPress = pressed_ ? 1.0f : 0.0f;
    pressAnimation_ += (targetPress - pressAnimation_) * animSpeed * deltaTime * 2.0f; // Faster press animation
    pressAnimation_ = std::clamp(pressAnimation_, 0.0f, 1.0f);
    
    // Update focus animation
    float targetFocus = focused_ ? 1.0f : 0.0f;
    focusAnimation_ += (targetFocus - focusAnimation_) * animSpeed * deltaTime;
    focusAnimation_ = std::clamp(focusAnimation_, 0.0f, 1.0f);
}

void Button::TriggerClick() {
    std::cout << "[Button] '" << text_ << "' clicked!" << std::endl;
    
    if (onClick_) {
        onClick_();
    }
}

} // namespace UI
} // namespace VoxelEngine
