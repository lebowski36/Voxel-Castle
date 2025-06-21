#include "ui/core/UIInputManager.h"
#include "ui/core/UIComponent.h"
#include <algorithm>

namespace VoxelEngine {
namespace UI {

UIInputManager::UIInputManager()
    : mousePosition_(0.0f, 0.0f)
    , mouseButtons_(8, false) // Support up to 8 mouse buttons
{
}

UIInputManager::~UIInputManager() = default;

void UIInputManager::ProcessMouseMove(float x, float y) {
    mousePosition_ = glm::vec2(x, y);
    UpdateHoveredComponent(x, y);
}

void UIInputManager::ProcessMouseDown(int button, float x, float y) {
    if (button >= 0 && button < static_cast<int>(mouseButtons_.size())) {
        mouseButtons_[button] = true;
    }
    
    mousePosition_ = glm::vec2(x, y);
    
    // Find component at click position and set focus
    auto component = GetComponentAt(x, y);
    if (component) {
        SetFocusedComponent(component);
        glm::vec2 localPos = glm::vec2(x, y) - component->GetAbsolutePosition();
        component->HandleMouseButton(button, 1, 0, localPos); // action=press
    } else {
        ClearFocus();
    }
}

void UIInputManager::ProcessMouseUp(int button, float x, float y) {
    if (button >= 0 && button < static_cast<int>(mouseButtons_.size())) {
        mouseButtons_[button] = false;
    }
    
    mousePosition_ = glm::vec2(x, y);
    
    auto component = GetComponentAt(x, y);
    if (component) {
        glm::vec2 localPos = glm::vec2(x, y) - component->GetAbsolutePosition();
        component->HandleMouseButton(button, 0, 0, localPos); // action=release
    }
}

void UIInputManager::ProcessKeyDown(int key) {
    auto focused = focusedComponent_.lock();
    if (focused) {
        focused->HandleKey(key, 0, 1, 0); // key, scancode, action=press, mods
    }
}

void UIInputManager::ProcessKeyUp(int key) {
    auto focused = focusedComponent_.lock();
    if (focused) {
        focused->HandleKey(key, 0, 0, 0); // key, scancode, action=release, mods
    }
}

void UIInputManager::ProcessTextInput(const char* text) {
    auto focused = focusedComponent_.lock();
    if (focused && text && text[0]) {
        focused->HandleChar(static_cast<unsigned int>(text[0]));
    }
}

void UIInputManager::SetRootComponent(std::shared_ptr<UIComponent> root) {
    rootComponent_ = root;
}

void UIInputManager::SetFocusedComponent(std::shared_ptr<UIComponent> component) {
    auto currentFocused = focusedComponent_.lock();
    if (currentFocused) {
        currentFocused->OnFocusLost();
    }
    
    focusedComponent_ = component;
    
    if (component) {
        component->OnFocusGained();
    }
}

std::shared_ptr<UIComponent> UIInputManager::GetFocusedComponent() const {
    return focusedComponent_.lock();
}

std::shared_ptr<UIComponent> UIInputManager::GetComponentAt(float x, float y) {
    auto root = rootComponent_.lock();
    if (!root) {
        return nullptr;
    }
    
    return FindComponentAt(root, x, y);
}

bool UIInputManager::IsMouseButtonDown(int button) const {
    if (button >= 0 && button < static_cast<int>(mouseButtons_.size())) {
        return mouseButtons_[button];
    }
    return false;
}

void UIInputManager::ClearFocus() {
    SetFocusedComponent(nullptr);
}

void UIInputManager::FocusNext() {
    // TODO: Implement tab order navigation
    // This would require maintaining a list of focusable components
}

void UIInputManager::FocusPrevious() {
    // TODO: Implement reverse tab order navigation
}

void UIInputManager::UpdateHoveredComponent(float x, float y) {
    auto newHovered = GetComponentAt(x, y);
    auto currentHovered = hoveredComponent_.lock();
    
    if (currentHovered != newHovered) {
        if (currentHovered) {
            currentHovered->OnMouseLeave();
        }
        
        hoveredComponent_ = newHovered;
        
        if (newHovered) {
            newHovered->OnMouseEnter();
        }
    }
}

std::shared_ptr<UIComponent> UIInputManager::FindComponentAt(std::shared_ptr<UIComponent> parent, float x, float y) {
    if (!parent || !parent->IsVisible()) {
        return nullptr;
    }
    
    // Check children first (front to back)
    auto& children = parent->GetChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto result = FindComponentAt(*it, x, y);
        if (result) {
            return result;
        }
    }
    
    // Check if point is within this component's bounds
    glm::vec2 pos = parent->GetAbsolutePosition();
    glm::vec2 size = parent->GetSize();
    
    if (x >= pos.x && x < pos.x + size.x && 
        y >= pos.y && y < pos.y + size.y) {
        return parent;
    }
    
    return nullptr;
}

} // namespace UI
} // namespace VoxelEngine
