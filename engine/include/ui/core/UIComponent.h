#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

class UIComponent;
class LayoutManager;
class UISystem;

/**
 * @brief Base class for all UI components
 * 
 * Provides the foundation for all UI elements with proper:
 * - Hierarchical structure (parent/child relationships)
 * - Layout management and positioning
 * - Event handling and input processing
 * - Styling and appearance
 * - Lifecycle management
 */
class UIComponent : public std::enable_shared_from_this<UIComponent> {
public:
    UIComponent();
    virtual ~UIComponent() = default;

    // Lifecycle
    virtual void Initialize() {}
    virtual void Update(float deltaTime);
    virtual void Render();
    virtual void Shutdown() {}
    
    // Hierarchy management
    void AddChild(std::shared_ptr<UIComponent> child);
    void RemoveChild(std::shared_ptr<UIComponent> child);
    void RemoveAllChildren();
    const std::vector<std::shared_ptr<UIComponent>>& GetChildren() const { return children_; }
    
    void SetParent(std::weak_ptr<UIComponent> parent) { parent_ = parent; }
    std::shared_ptr<UIComponent> GetParent() const { return parent_.lock(); }
    std::shared_ptr<UIComponent> GetRoot();
    
    // Position and size (in logical pixels)
    void SetPosition(const glm::vec2& position);
    void SetPosition(float x, float y) { SetPosition(glm::vec2(x, y)); }
    const glm::vec2& GetPosition() const { return position_; }
    
    void SetSize(const glm::vec2& size);
    void SetSize(float width, float height) { SetSize(glm::vec2(width, height)); }
    const glm::vec2& GetSize() const { return size_; }
    
    // Computed properties
    glm::vec2 GetAbsolutePosition() const;
    glm::vec4 GetBounds() const; // x, y, width, height in absolute coordinates
    bool ContainsPoint(const glm::vec2& point) const;
    
    // Visibility and state
    void SetVisible(bool visible) { visible_ = visible; }
    bool IsVisible() const { return visible_; }
    
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }
    
    virtual bool IsFocused() const { return false; }
    virtual bool CanReceiveFocus() const { return false; }
    
    // Layout management
    void SetLayout(std::shared_ptr<LayoutManager> layout);
    std::shared_ptr<LayoutManager> GetLayout() const { return layout_; }
    void InvalidateLayout() { layoutDirty_ = true; }
    
    // Sizing constraints
    void SetMinSize(const glm::vec2& minSize) { minSize_ = minSize; InvalidateLayout(); }
    void SetMaxSize(const glm::vec2& maxSize) { maxSize_ = maxSize; InvalidateLayout(); }
    const glm::vec2& GetMinSize() const { return minSize_; }
    const glm::vec2& GetMaxSize() const { return maxSize_; }
    
    // Preferred size calculation
    virtual glm::vec2 CalculatePreferredSize() const;
    
    // Input handling (return true if event was handled)
    virtual bool HandleMouseButton(int button, int action, int mods, const glm::vec2& localPos);
    virtual bool HandleMouseMove(const glm::vec2& localPos);
    virtual bool HandleKey(int key, int scancode, int action, int mods);
    virtual bool HandleChar(unsigned int codepoint);
    virtual bool HandleScroll(double xoffset, double yoffset, const glm::vec2& localPos);
    
    // Focus events
    virtual void OnFocusGained() {}
    virtual void OnFocusLost() {}
    
    // Mouse events
    virtual void OnMouseEnter() {}
    virtual void OnMouseLeave() {}
    
    // Utility methods
    UISystem* GetUISystem() const;
    
    // Component creation helper
    template<typename T, typename... Args>
    std::shared_ptr<T> CreateComponent(Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        component->Initialize();
        return component;
    }
    
    // Responsive design helpers
    float GetResponsiveSpacing(float baseSpacing) const;
    float GetResponsiveFontSize(float baseFontSize) const;
    glm::vec2 GetResponsiveSize(const glm::vec2& baseSize) const;
    
    // Override this for responsive layout changes
    virtual void OnScreenSizeChanged(float screenWidth, float screenHeight) {}
    
protected:
    // Override these for custom rendering
    virtual void OnRender() {}
    virtual void OnUpdate(float deltaTime) {}
    
    // Input events - override these for custom input handling
    virtual void OnMouseDown(int button, float x, float y) {}
    virtual void OnMouseUp(int button, float x, float y) {}
    virtual void OnKeyDown(int key) {}
    virtual void OnKeyUp(int key) {}
    virtual void OnTextInput(const char* /*text*/) {}
    
    // Layout helpers
    void UpdateLayout();
    glm::vec2 ApplySizeConstraints(const glm::vec2& size) const;
    
private:
    std::weak_ptr<UIComponent> parent_;
    std::vector<std::shared_ptr<UIComponent>> children_;
    std::shared_ptr<LayoutManager> layout_;
    
    glm::vec2 position_;
    glm::vec2 size_;
    glm::vec2 minSize_;
    glm::vec2 maxSize_;
    
    bool visible_;
    bool enabled_;
    bool layoutDirty_;
    
    // Internal methods
    void UpdateHierarchy(float deltaTime);
    void RenderHierarchy();
    std::shared_ptr<UIComponent> FindChildAt(const glm::vec2& point);
};

} // namespace UI
} // namespace VoxelEngine
