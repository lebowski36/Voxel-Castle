#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include "ui/UIRenderer.h"
#include "ui/core/UIInputManager.h"

namespace VoxelEngine {
namespace UI {

class UIComponent;
class UIScaleManager;
class UIInputManager;
class UITheme;

/**
 * @brief Main UI system manager - handles all UI globally
 * 
 * This is the core of the new UI system that manages:
 * - Screen size and DPI scaling
 * - Component lifecycle and hierarchy
 * - Input handling and event distribution
 * - Theme management and styling
 */
class UISystem {
public:
    UISystem();
    ~UISystem();

    // System lifecycle
    bool Initialize(int screenWidth, int screenHeight, const std::string& projectRoot = "");
    void Shutdown();
    
    // Frame management
    void BeginFrame();
    void Update(float deltaTime);
    void Render();
    void EndFrame();
    
    // Screen management
    void SetScreenSize(int width, int height);
    void setScreenSize(int width, int height); // Compatibility wrapper
    glm::vec2 GetScreenSize() const { return screenSize_; }
    float GetAspectRatio() const;
    
    // DPI and scaling
    float GetDPIScale() const;
    float GetUserScale() const { return userScale_; }
    void SetUserScale(float scale);
    float GetTotalScale() const { return GetDPIScale() * userScale_; }
    
    // Logical pixels to screen pixels conversion
    float LogicalToScreen(float logicalPixels) const;
    glm::vec2 LogicalToScreen(const glm::vec2& logicalPixels) const;
    float ScreenToLogical(float screenPixels) const;
    glm::vec2 ScreenToLogical(const glm::vec2& screenPixels) const;
    
    // Screen size categories for responsive design
    enum class ScreenSize {
        SMALL,    // < 1366px width
        MEDIUM,   // 1366-1920px
        LARGE,    // 1920-2560px
        XLARGE    // > 2560px
    };
    ScreenSize GetScreenSizeCategory() const;
    
    // Component management
    void SetRootComponent(std::shared_ptr<UIComponent> component);
    std::shared_ptr<UIComponent> GetRootComponent() const { return rootComponent_; }
    
    // Input handling
    void HandleMouseButton(int button, int action, int mods, double xpos, double ypos);
    void HandleMouseMove(double xpos, double ypos);
    void HandleKey(int key, int scancode, int action, int mods);
    void HandleChar(unsigned int codepoint);
    void HandleScroll(double xoffset, double yoffset);
    
    // Focus management
    void SetFocusedComponent(std::shared_ptr<UIComponent> component);
    std::shared_ptr<UIComponent> GetFocusedComponent() const { 
        return focusedComponent_.lock(); 
    }
    void ClearFocus();
    
    // Theme management
    void SetTheme(std::shared_ptr<UITheme> theme);
    std::shared_ptr<UITheme> GetTheme() const { return theme_; }
    
    // Renderer access
    UIRenderer* GetRenderer() const { return renderer_.get(); }
    
    // Singleton access
    static UISystem* GetInstance() { return instance_; }
    
    // Debug and utilities
    void SetDebugMode(bool enabled) { debugMode_ = enabled; }
    bool IsDebugMode() const { return debugMode_; }
    
private:
    static UISystem* instance_;
    
    std::unique_ptr<UIRenderer> renderer_;
    std::unique_ptr<UIScaleManager> scaleManager_;
    std::unique_ptr<UIInputManager> inputManager_;
    
    std::shared_ptr<UIComponent> rootComponent_;
    std::weak_ptr<UIComponent> focusedComponent_;
    std::shared_ptr<UITheme> theme_;
    
    glm::vec2 screenSize_;
    float userScale_;
    bool debugMode_;
    
    // Input state
    glm::vec2 lastMousePos_;
    std::weak_ptr<UIComponent> hoveredComponent_;
    
    // Internal methods
    void UpdateComponentHierarchy(float deltaTime);
    void RenderComponentHierarchy();
    std::shared_ptr<UIComponent> FindComponentAt(const glm::vec2& screenPos);
    void DistributeInputEvent(const std::function<bool(UIComponent*)>& eventHandler);
};

} // namespace UI
} // namespace VoxelEngine
