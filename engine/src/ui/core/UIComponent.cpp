#include "ui/core/UIComponent.h"
#include "ui/core/UISystem.h"
#include "ui/core/UITheme.h"
#include "ui/layout/LayoutManager.h"
#include <algorithm>
#include <iostream>

namespace VoxelEngine {
namespace UI {

UIComponent::UIComponent()
    : position_(0, 0)
    , size_(0, 0)
    , minSize_(0, 0)
    , maxSize_(10000, 10000) // Large default max size
    , visible_(true)
    , enabled_(true)
    , layoutDirty_(true) {
}

void UIComponent::Update(float deltaTime) {
    if (!visible_) return;
    
    // Update layout if needed
    if (layoutDirty_) {
        UpdateLayout();
        layoutDirty_ = false;
    }
    
    // Call custom update
    OnUpdate(deltaTime);
    
    // Update children
    UpdateHierarchy(deltaTime);
}

void UIComponent::Render() {
    if (!visible_) return;
    
    // Render this component
    OnRender();
    
    // Render children
    RenderHierarchy();
}

void UIComponent::AddChild(std::shared_ptr<UIComponent> child) {
    if (!child) return;
    
    // Remove from previous parent
    auto previousParent = child->GetParent();
    if (previousParent) {
        previousParent->RemoveChild(child);
    }
    
    // Add to this component
    children_.push_back(child);
    child->SetParent(weak_from_this());
    
    // Invalidate layout
    InvalidateLayout();
}

void UIComponent::RemoveChild(std::shared_ptr<UIComponent> child) {
    if (!child) return;
    
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->SetParent({});
        children_.erase(it);
        InvalidateLayout();
    }
}

void UIComponent::RemoveAllChildren() {
    for (auto& child : children_) {
        child->SetParent({});
    }
    children_.clear();
    InvalidateLayout();
}

std::shared_ptr<UIComponent> UIComponent::GetRoot() {
    auto current = shared_from_this();
    while (auto parent = current->GetParent()) {
        current = parent;
    }
    return current;
}

void UIComponent::SetPosition(const glm::vec2& position) {
    if (position_ != position) {
        position_ = position;
        InvalidateLayout();
    }
}

void UIComponent::SetSize(const glm::vec2& size) {
    glm::vec2 constrainedSize = ApplySizeConstraints(size);
    if (size_ != constrainedSize) {
        size_ = constrainedSize;
        InvalidateLayout();
    }
}

glm::vec2 UIComponent::GetAbsolutePosition() const {
    glm::vec2 absolutePos = position_;
    
    auto parent = parent_.lock();
    if (parent) {
        absolutePos += parent->GetAbsolutePosition();
    }
    
    return absolutePos;
}

glm::vec4 UIComponent::GetBounds() const {
    glm::vec2 absPos = GetAbsolutePosition();
    return glm::vec4(absPos.x, absPos.y, size_.x, size_.y);
}

bool UIComponent::ContainsPoint(const glm::vec2& point) const {
    glm::vec4 bounds = GetBounds();
    return point.x >= bounds.x && point.x <= bounds.x + bounds.z &&
           point.y >= bounds.y && point.y <= bounds.y + bounds.w;
}

void UIComponent::SetLayout(std::shared_ptr<LayoutManager> layout) {
    layout_ = layout;
    if (layout_) {
        layout_->SetContainer(shared_from_this());
    }
    InvalidateLayout();
}

glm::vec2 UIComponent::CalculatePreferredSize() const {
    // If we have a layout, use its preferred size calculation
    if (layout_) {
        return layout_->CalculatePreferredSize();
    }
    
    // Default implementation returns current size
    return size_;
}

bool UIComponent::HandleMouseButton(int button, int action, int mods, const glm::vec2& localPos) {
    // Try children first (front to back)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        if (!child->IsVisible() || !child->IsEnabled()) continue;
        
        glm::vec2 childLocalPos = localPos - child->GetPosition();
        if (child->ContainsPoint(GetAbsolutePosition() + localPos)) {
            if (child->HandleMouseButton(button, action, mods, childLocalPos)) {
                return true; // Child handled the event
            }
        }
    }
    
    // If no child handled it, this component can handle it
    return false;
}

bool UIComponent::HandleMouseMove(const glm::vec2& localPos) {
    // Try children first
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        if (!child->IsVisible()) continue;
        
        glm::vec2 childLocalPos = localPos - child->GetPosition();
        if (child->ContainsPoint(GetAbsolutePosition() + localPos)) {
            if (child->HandleMouseMove(childLocalPos)) {
                return true;
            }
        }
    }
    
    return false;
}

bool UIComponent::HandleKey(int key, int scancode, int action, int mods) {
    // Default implementation doesn't handle keys
    return false;
}

bool UIComponent::HandleChar(unsigned int codepoint) {
    // Default implementation doesn't handle character input
    return false;
}

bool UIComponent::HandleScroll(double xoffset, double yoffset, const glm::vec2& localPos) {
    // Try children first
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        if (!child->IsVisible()) continue;
        
        glm::vec2 childLocalPos = localPos - child->GetPosition();
        if (child->ContainsPoint(GetAbsolutePosition() + localPos)) {
            if (child->HandleScroll(xoffset, yoffset, childLocalPos)) {
                return true;
            }
        }
    }
    
    return false;
}

UISystem* UIComponent::GetUISystem() const {
    return UISystem::GetInstance();
}

void UIComponent::UpdateLayout() {
    if (layout_) {
        layout_->UpdateLayout();
    }
}

glm::vec2 UIComponent::ApplySizeConstraints(const glm::vec2& size) const {
    return glm::vec2(
        std::max(minSize_.x, std::min(maxSize_.x, size.x)),
        std::max(minSize_.y, std::min(maxSize_.y, size.y))
    );
}

void UIComponent::UpdateHierarchy(float deltaTime) {
    for (auto& child : children_) {
        if (child->IsVisible()) {
            child->Update(deltaTime);
        }
    }
}

void UIComponent::RenderHierarchy() {
    for (auto& child : children_) {
        if (child->IsVisible()) {
            child->Render();
        }
    }
}

std::shared_ptr<UIComponent> UIComponent::FindChildAt(const glm::vec2& point) {
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        if (child->IsVisible() && child->ContainsPoint(point)) {
            // Check if any grandchild contains the point
            auto grandchild = child->FindChildAt(point);
            return grandchild ? grandchild : child;
        }
    }
    return nullptr;
}

float UIComponent::GetResponsiveSpacing(float baseSpacing) const {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return baseSpacing;
    
    auto theme = uiSystem->GetTheme();
    if (!theme) return baseSpacing;
    
    auto screenSize = uiSystem->GetScreenSize();
    return theme->GetResponsiveSpacing(baseSpacing, screenSize.x);
}

float UIComponent::GetResponsiveFontSize(float baseFontSize) const {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return baseFontSize;
    
    auto theme = uiSystem->GetTheme();
    if (!theme) return baseFontSize;
    
    auto screenSize = uiSystem->GetScreenSize();
    return theme->GetResponsiveFontSize(baseFontSize, screenSize.x);
}

glm::vec2 UIComponent::GetResponsiveSize(const glm::vec2& baseSize) const {
    auto uiSystem = GetUISystem();
    if (!uiSystem) return baseSize;
    
    auto theme = uiSystem->GetTheme();
    if (!theme) return baseSize;
    
    auto screenSize = uiSystem->GetScreenSize();
    float multiplier = theme->breakpoints.GetSpacingMultiplier(screenSize.x);
    return baseSize * multiplier;
}

} // namespace UI
} // namespace VoxelEngine
