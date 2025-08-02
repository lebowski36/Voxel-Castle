#include "ui/core/UISystem.h"
#include "ui/core/UIComponent.h"
#include "ui/core/UIScaleManager.h"
#include "ui/core/UITheme.h"
#include "ui/core/UIInputManager.h"
#include <iostream>
#include <algorithm>

namespace VoxelEngine {
namespace UI {

UISystem* UISystem::instance_ = nullptr;

UISystem::UISystem() 
    : screenSize_(0, 0)
    , userScale_(1.0f)
    , debugMode_(false)
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
    // Only update if screen size actually changed
    glm::vec2 newSize(width, height);
    if (screenSize_.x == newSize.x && screenSize_.y == newSize.y) {
        std::cout << "[UISystem] SetScreenSize called but no change: " << width << "x" << height << std::endl;
        return; // No change, skip update
    }
    
    std::cout << "[UISystem] *** SCREEN SIZE CHANGE *** " << width << "x" << height 
              << " (was " << screenSize_.x << "x" << screenSize_.y << ")" << std::endl;
    
    // Add debug info to trace what's calling this
    if (width == 1920 && height == 1080) {
        std::cout << "[UISystem] WARNING: Reverting to 1920x1080 - this might be wrong!" << std::endl;
    }
    
    screenSize_ = newSize;
    
    if (renderer_) {
        std::cout << "[UISystem] Calling renderer_->setScreenSize..." << std::endl;
        renderer_->setScreenSize(width, height);
    } else {
        std::cout << "[UISystem] WARNING: renderer_ is null!" << std::endl;
    }
    
    if (scaleManager_) {
        scaleManager_->UpdateScreenSize(width, height);
    }
    
    // Update root component layout and positioning for new screen size
    if (rootComponent_) {
        // Don't force root component to full screen size
        // Let it maintain its preferred content size and just re-center it
        rootComponent_->InvalidateLayout();
        
        // Re-center the root component on the new screen size
        CenterRootComponent();
        
        NotifyScreenSizeChanged(rootComponent_);
    }
    
    std::cout << "[UISystem] Screen size changed to " << width << "x" << height << std::endl;
}

void UISystem::setScreenSize(int width, int height) {
    SetScreenSize(width, height);
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
    // TEMPORARY: Bypass logical scaling to test if this is the issue
    return logicalPixels;
    // return scaleManager_ ? scaleManager_->LogicalToScreen(logicalPixels) : logicalPixels;
}

glm::vec2 UISystem::LogicalToScreen(const glm::vec2& logicalPixels) const {
    // TEMPORARY: Bypass logical scaling to test if this is the issue
    return logicalPixels;
    // return scaleManager_ ? scaleManager_->LogicalToScreen(logicalPixels) : logicalPixels;
}

float UISystem::ScreenToLogical(float screenPixels) const {
    // TEMPORARY: Bypass logical scaling to test if this is the issue
    return screenPixels;
    // return scaleManager_ ? scaleManager_->ScreenToLogical(screenPixels) : screenPixels;
}

glm::vec2 UISystem::ScreenToLogical(const glm::vec2& screenPixels) const {
    // TEMPORARY: Bypass logical scaling to test if this is the issue
    return screenPixels;
    // return scaleManager_ ? scaleManager_->ScreenToLogical(screenPixels) : screenPixels;
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
        // Let the component calculate its preferred size based on its layout and content
        glm::vec2 preferredSize = rootComponent_->CalculatePreferredSize();
        
        // If the component doesn't have a preferred size, use a reasonable default
        if (preferredSize.x <= 0 || preferredSize.y <= 0) {
            preferredSize = glm::vec2(400, 500); // Default menu size
        }
        
        rootComponent_->SetSize(preferredSize);
        
        // Center the root component on the screen
        CenterRootComponent();
        
        // Don't call Initialize() - component should already be initialized
        // Calling Initialize() again causes duplicate creation logs and potential issues
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

void UISystem::NotifyScreenSizeChanged(std::shared_ptr<UIComponent> component) {
    if (!component) return;
    
    // Notify this component
    component->OnScreenSizeChanged(screenSize_.x, screenSize_.y);
    
    // Recursively notify all children
    for (auto& child : component->GetChildren()) {
        NotifyScreenSizeChanged(child);
    }
}

void UISystem::CenterRootComponent() {
    if (!rootComponent_) return;
    
    // Get the current size of the root component
    glm::vec2 componentSize = rootComponent_->GetSize();
    
    // Convert screen size to logical pixels
    glm::vec2 logicalScreenSize = ScreenToLogical(screenSize_);
    
    // Calculate centered position
    glm::vec2 centeredPosition = (logicalScreenSize - componentSize) * 0.5f;
    
    // Ensure the component doesn't go off-screen (with 10px margin in logical pixels)
    float margin = 10.0f;
    centeredPosition.x = std::max(margin, std::min(centeredPosition.x, logicalScreenSize.x - componentSize.x - margin));
    centeredPosition.y = std::max(margin, std::min(centeredPosition.y, logicalScreenSize.y - componentSize.y - margin));
    
    // Set the centered position
    rootComponent_->SetPosition(centeredPosition);
    
    std::cout << "[UISystem] Root component centered at (" << centeredPosition.x << ", " << centeredPosition.y 
              << ") for size " << componentSize.x << "x" << componentSize.y 
              << " on screen " << logicalScreenSize.x << "x" << logicalScreenSize.y << std::endl;
}

void UISystem::DrawDebugRectangles() {
    if (!renderer_) return;
    
    // Draw test rectangles using the same coordinate system as UI components
    // This ensures they move correctly when the screen is resized
    
    float rectSize = 50.0f; // Logical pixels
    glm::vec4 colors[] = {
        {1.0f, 0.0f, 0.0f, 1.0f}, // Red
        {0.0f, 1.0f, 0.0f, 1.0f}, // Green  
        {0.0f, 0.0f, 1.0f, 1.0f}, // Blue
        {1.0f, 1.0f, 0.0f, 1.0f}, // Yellow
        {1.0f, 0.0f, 1.0f, 1.0f}  // Magenta
    };
    
    // Get logical screen size
    glm::vec2 logicalScreenSize = ScreenToLogical(screenSize_);
    
    // Define positions in logical coordinates
    glm::vec2 positions[] = {
        {logicalScreenSize.x * 0.5f - rectSize * 0.5f, logicalScreenSize.y * 0.5f - rectSize * 0.5f}, // Center
        {20.0f, 20.0f}, // Top-left
        {logicalScreenSize.x - rectSize - 20.0f, 20.0f}, // Top-right
        {20.0f, logicalScreenSize.y - rectSize - 20.0f}, // Bottom-left
        {logicalScreenSize.x - rectSize - 20.0f, logicalScreenSize.y - rectSize - 20.0f} // Bottom-right
    };
    
    const char* labels[] = {"center", "top-left", "top-right", "bottom-left", "bottom-right"};
    
    std::cout << "[UISystem] Screen size: " << screenSize_.x << "x" << screenSize_.y 
              << ", Logical size: " << logicalScreenSize.x << "x" << logicalScreenSize.y << std::endl;
    
    // Draw rectangles using logical coordinates (same as UI components)
    for (int i = 0; i < 5; i++) {
        // Convert logical coordinates to screen coordinates (same as UI components do)
        glm::vec2 screenPos = LogicalToScreen(positions[i]);
        glm::vec2 screenSize = LogicalToScreen(glm::vec2(rectSize, rectSize));
        
        std::cout << "[UISystem] Drawing " << labels[i] << " debug rect - Logical: (" 
                  << positions[i].x << "," << positions[i].y << "), Screen: (" 
                  << screenPos.x << "," << screenPos.y << "), Size: " << screenSize.x << "x" << screenSize.y << std::endl;
        
        renderer_->renderColoredQuad(screenPos.x, screenPos.y, screenSize.x, screenSize.y, colors[i]);
    }
}

void UISystem::PrimeUIRenderingState() {
    if (!renderer_) return;
    
    // This method replicates the essential OpenGL state setup that the test rectangles
    // were doing, but without drawing any visible output
    
    // The key insight is that something in the test rectangle drawing was setting up
    // OpenGL state that the regular UI rendering depends on, but doesn't set up itself
    
    static int frameCount = 0;
    frameCount++;
    
    // Only do this setup occasionally to avoid performance impact
    if (frameCount % 60 == 1) { // Every second at 60fps
        std::cout << "[UISystem] Priming UI rendering state..." << std::endl;
        
        // Use the same state setup as the test rectangles
        renderer_->drawTestRectangle();
        
        // Immediately clear the framebuffer to hide the test rectangles
        // This preserves the OpenGL state setup without visible artifacts
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

} // namespace UI
} // namespace VoxelEngine
