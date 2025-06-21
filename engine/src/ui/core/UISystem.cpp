#include "ui/core/UISystem.h"
#include "ui/core/UIComponent.h"
#include "ui/core/UIScaleManager.h"
#include "ui/core/UITheme.h"
#include <iostream>
#include <algorithm>

namespace VoxelEngine {
namespace UI {

UISystem* UISystem::instance_ = nullptr;

UISystem::UISystem() 
    : userScale_(1.0f)
    , debugMode_(false)
    , screenSize_(0, 0)
    , lastMousePos_(0, 0) {
    
    if (instance_ != nullptr) {
        std::cerr << "[UISystem] Warning: Multiple UISystem instances created!" << std::endl;
    }
    instance_ = this;
}

UISystem::~UISystem() {
    Shutdown();
    if (instance_ == this) {
        instance_ = nullptr;
    }
}

bool UISystem::Initialize(int screenWidth, int screenHeight, const std::string& projectRoot) {
    std::cout << "[UISystem] Initializing UI system..." << std::endl;
    
    // Initialize renderer
    renderer_ = std::make_unique<UIRenderer>();
    if (!renderer_->initialize(screenWidth, screenHeight, projectRoot)) {
        std::cerr << "[UISystem] Failed to initialize UIRenderer" << std::endl;
        return false;
    }
    
    // Initialize scale manager
    scaleManager_ = std::make_unique<UIScaleManager>();
    if (!scaleManager_->Initialize(screenWidth, screenHeight)) {
        std::cerr << "[UISystem] Failed to initialize UIScaleManager" << std::endl;
        return false;
    }
    
    // Set screen size
    screenSize_ = glm::vec2(screenWidth, screenHeight);
    
    // Initialize default theme
    theme_ = UITheme::GetDefault();
    
    std::cout << "[UISystem] UI system initialized successfully" << std::endl;
    std::cout << "[UISystem] Screen size: " << screenWidth << "x" << screenHeight << std::endl;
    std::cout << "[UISystem] DPI scale: " << GetDPIScale() << std::endl;
    std::cout << "[UISystem] Total scale: " << GetTotalScale() << std::endl;
    
    return true;
}

void UISystem::Shutdown() {
    std::cout << "[UISystem] Shutting down UI system..." << std::endl;
    
    if (rootComponent_) {
        rootComponent_->Shutdown();
        rootComponent_.reset();
    }
    
    theme_.reset();
    scaleManager_.reset();
    
    if (renderer_) {
        renderer_->shutdown();
        renderer_.reset();
    }
    
    std::cout << "[UISystem] UI system shutdown complete" << std::endl;
}

void UISystem::BeginFrame() {
    if (renderer_) {
        renderer_->beginFrame();
    }
}

void UISystem::Update(float deltaTime) {
    if (rootComponent_) {
        UpdateComponentHierarchy(deltaTime);
    }
}

void UISystem::Render() {
    if (rootComponent_ && renderer_) {
        RenderComponentHierarchy();
    }
    
    if (debugMode_) {
        // Render debug information
        // TODO: Implement debug overlay
    }
}

void UISystem::EndFrame() {
    if (renderer_) {
        renderer_->endFrame();
    }
}

void UISystem::SetScreenSize(int width, int height) {
    screenSize_ = glm::vec2(width, height);
    
    if (renderer_) {
        renderer_->setScreenSize(width, height);
    }
    
    if (scaleManager_) {
        scaleManager_->UpdateScreenSize(width, height);
    }
    
    // Invalidate layout for root component
    if (rootComponent_) {
        rootComponent_->InvalidateLayout();
    }
    
    std::cout << "[UISystem] Screen size changed to: " << width << "x" << height << std::endl;
}

float UISystem::GetAspectRatio() const {
    return screenSize_.y > 0 ? screenSize_.x / screenSize_.y : 1.0f;
}

float UISystem::GetDPIScale() const {
    return scaleManager_ ? scaleManager_->GetDPIScale() : 1.0f;
}

void UISystem::SetUserScale(float scale) {
    if (scaleManager_ && scaleManager_->IsValidUserScale(scale)) {
        userScale_ = scale;
        scaleManager_->SetUserScale(scale);
        
        // Invalidate layout for all components
        if (rootComponent_) {
            rootComponent_->InvalidateLayout();
        }
        
        std::cout << "[UISystem] User scale changed to: " << scale << std::endl;
    }
}

float UISystem::LogicalToScreen(float logicalPixels) const {
    return scaleManager_ ? scaleManager_->LogicalToScreen(logicalPixels) : logicalPixels;
}

glm::vec2 UISystem::LogicalToScreen(const glm::vec2& logicalPixels) const {
    return scaleManager_ ? scaleManager_->LogicalToScreen(logicalPixels) : logicalPixels;
}

float UISystem::ScreenToLogical(float screenPixels) const {
    return scaleManager_ ? scaleManager_->ScreenToLogical(screenPixels) : screenPixels;
}

glm::vec2 UISystem::ScreenToLogical(const glm::vec2& screenPixels) const {
    return scaleManager_ ? scaleManager_->ScreenToLogical(screenPixels) : screenPixels;
}

UISystem::ScreenSize UISystem::GetScreenSizeCategory() const {
    float width = screenSize_.x;
    
    if (width < 1366) return ScreenSize::SMALL;
    if (width < 1920) return ScreenSize::MEDIUM;
    if (width < 2560) return ScreenSize::LARGE;
    return ScreenSize::XLARGE;
}

void UISystem::SetRootComponent(std::shared_ptr<UIComponent> component) {
    if (rootComponent_) {
        rootComponent_->Shutdown();
    }
    
    rootComponent_ = component;
    
    if (rootComponent_) {
        rootComponent_->SetPosition(0, 0);
        rootComponent_->SetSize(screenSize_);
        rootComponent_->Initialize();
    }
}

void UISystem::HandleMouseButton(int button, int action, int mods, double xpos, double ypos) {
    glm::vec2 screenPos(xpos, ypos);
    glm::vec2 logicalPos = ScreenToLogical(screenPos);
    
    auto component = FindComponentAt(logicalPos);
    
    if (action == 1) { // Press
        // Set focus to clicked component
        if (component && component->CanReceiveFocus()) {
            SetFocusedComponent(component);
        } else {
            ClearFocus();
        }
    }
    
    // Send event to component hierarchy
    if (component) {
        glm::vec2 localPos = logicalPos - component->GetAbsolutePosition();
        component->HandleMouseButton(button, action, mods, localPos);
    }
}

void UISystem::HandleMouseMove(double xpos, double ypos) {
    glm::vec2 screenPos(xpos, ypos);
    glm::vec2 logicalPos = ScreenToLogical(screenPos);
    lastMousePos_ = logicalPos;
    
    auto component = FindComponentAt(logicalPos);
    auto previousHovered = hoveredComponent_.lock();
    
    // Handle mouse enter/leave events
    if (component != previousHovered) {
        if (previousHovered) {
            previousHovered->OnMouseLeave();
        }
        if (component) {
            component->OnMouseEnter();
        }
        hoveredComponent_ = component;
    }
    
    // Send move event to component
    if (component) {
        glm::vec2 localPos = logicalPos - component->GetAbsolutePosition();
        component->HandleMouseMove(localPos);
    }
}

void UISystem::HandleKey(int key, int scancode, int action, int mods) {
    auto focused = focusedComponent_.lock();
    if (focused) {
        focused->HandleKey(key, scancode, action, mods);
    }
}

void UISystem::HandleChar(unsigned int codepoint) {
    auto focused = focusedComponent_.lock();
    if (focused) {
        focused->HandleChar(codepoint);
    }
}

void UISystem::HandleScroll(double xoffset, double yoffset) {
    auto component = FindComponentAt(lastMousePos_);
    if (component) {
        glm::vec2 localPos = lastMousePos_ - component->GetAbsolutePosition();
        component->HandleScroll(xoffset, yoffset, localPos);
    }
}

void UISystem::SetFocusedComponent(std::shared_ptr<UIComponent> component) {
    auto previousFocus = focusedComponent_.lock();
    
    if (previousFocus == component) {
        return; // Already focused
    }
    
    if (previousFocus) {
        previousFocus->OnFocusLost();
    }
    
    focusedComponent_ = component;
    
    if (component) {
        component->OnFocusGained();
    }
}

void UISystem::ClearFocus() {
    SetFocusedComponent(nullptr);
}

void UISystem::SetTheme(std::shared_ptr<UITheme> theme) {
    theme_ = theme ? theme : UITheme::GetDefault();
}

void UISystem::UpdateComponentHierarchy(float deltaTime) {
    if (rootComponent_) {
        rootComponent_->Update(deltaTime);
    }
}

void UISystem::RenderComponentHierarchy() {
    if (rootComponent_) {
        rootComponent_->Render();
    }
}

std::shared_ptr<UIComponent> UISystem::FindComponentAt(const glm::vec2& screenPos) {
    if (!rootComponent_) {
        return nullptr;
    }
    
    // Recursively search component hierarchy
    std::function<std::shared_ptr<UIComponent>(std::shared_ptr<UIComponent>, const glm::vec2&)> searchComponent;
    searchComponent = [&](std::shared_ptr<UIComponent> component, const glm::vec2& pos) -> std::shared_ptr<UIComponent> {
        if (!component->IsVisible() || !component->ContainsPoint(pos)) {
            return nullptr;
        }
        
        // Check children first (front to back)
        for (auto it = component->GetChildren().rbegin(); it != component->GetChildren().rend(); ++it) {
            auto found = searchComponent(*it, pos);
            if (found) {
                return found;
            }
        }
        
        // If no child contains the point, return this component
        return component;
    };
    
    return searchComponent(rootComponent_, screenPos);
}

} // namespace UI
} // namespace VoxelEngine
