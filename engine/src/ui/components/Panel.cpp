#include "ui/components/Panel.h"
#include "ui/core/UISystem.h"
#include "ui/UIRenderer.h"

namespace VoxelEngine {
namespace UI {

Panel::Panel()
    : backgroundColor_(0.2f, 0.2f, 0.3f, 0.95f) // Semi-transparent dark background
    , backgroundVisible_(true)
    , borderColor_(0.5f, 0.5f, 0.6f, 1.0f) // Light gray border
    , borderWidth_(1.0f)
    , borderVisible_(false)
    , cornerRadius_(8.0f)
    , padding_(12.0f, 12.0f, 12.0f, 12.0f) // 12px padding all around
    , clipContent_(false) {
}

void Panel::SetBackgroundColor(const glm::vec4& color) {
    backgroundColor_ = color;
}

void Panel::SetBackgroundVisible(bool visible) {
    backgroundVisible_ = visible;
}

void Panel::SetBorderColor(const glm::vec4& color) {
    borderColor_ = color;
}

void Panel::SetBorderWidth(float width) {
    borderWidth_ = std::max(0.0f, width);
}

void Panel::SetBorderVisible(bool visible) {
    borderVisible_ = visible;
}

void Panel::SetCornerRadius(float radius) {
    cornerRadius_ = std::max(0.0f, radius);
}

void Panel::SetPadding(const glm::vec4& padding) {
    padding_ = padding;
    InvalidateLayout();
}

glm::vec2 Panel::GetContentSize() const {
    glm::vec2 size = GetSize();
    return glm::vec2(
        std::max(0.0f, size.x - padding_.x - padding_.z), // width - left - right
        std::max(0.0f, size.y - padding_.y - padding_.w)  // height - top - bottom
    );
}

glm::vec2 Panel::GetContentPosition() const {
    return glm::vec2(padding_.x, padding_.y); // left, top
}

void Panel::SetClipContent(bool clip) {
    clipContent_ = clip;
}

void Panel::OnRender() {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto renderer = uiSystem->GetRenderer();
    if (!renderer) return;
    
    // Render background
    if (backgroundVisible_) {
        RenderBackground();
    }
    
    // Render border
    if (borderVisible_ && borderWidth_ > 0) {
        RenderBorder();
    }
}

void Panel::RenderBackground() {
    auto uiSystem = GetUISystem();
    auto renderer = uiSystem->GetRenderer();
    
    glm::vec2 pos = GetAbsolutePosition();
    glm::vec2 size = GetSize();
    float scale = uiSystem->GetTotalScale();
    
    // Apply scaling
    pos *= scale;
    size *= scale;
    
    // For now, render as a simple colored quad
    // TODO: Add rounded corners support when renderer supports it
    renderer->renderColoredQuad(pos.x, pos.y, size.x, size.y, backgroundColor_);
}

void Panel::RenderBorder() {
    auto uiSystem = GetUISystem();
    auto renderer = uiSystem->GetRenderer();
    
    glm::vec2 pos = GetAbsolutePosition();
    glm::vec2 size = GetSize();
    float scale = uiSystem->GetTotalScale();
    float scaledBorderWidth = borderWidth_ * scale;
    
    // Apply scaling
    pos *= scale;
    size *= scale;
    
    // Render border as four rectangles (top, right, bottom, left)
    // Top border
    renderer->renderColoredQuad(pos.x, pos.y, size.x, scaledBorderWidth, borderColor_);
    
    // Right border
    renderer->renderColoredQuad(pos.x + size.x - scaledBorderWidth, pos.y, scaledBorderWidth, size.y, borderColor_);
    
    // Bottom border
    renderer->renderColoredQuad(pos.x, pos.y + size.y - scaledBorderWidth, size.x, scaledBorderWidth, borderColor_);
    
    // Left border
    renderer->renderColoredQuad(pos.x, pos.y, scaledBorderWidth, size.y, borderColor_);
}

} // namespace UI
} // namespace VoxelEngine
