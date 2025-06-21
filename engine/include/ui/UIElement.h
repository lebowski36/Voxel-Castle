#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace VoxelEngine {
namespace UI {

class UIElement {
public:
    UIElement();
    virtual ~UIElement() = default;

    // Core properties
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setVisible(bool visible);
    
    glm::vec2 getPosition() const { return position_; }
    glm::vec2 getSize() const { return size_; }
    bool isVisible() const { return visible_; }

    // Hierarchy management
    void addChild(std::shared_ptr<UIElement> child);
    void removeChild(std::shared_ptr<UIElement> child);
    void setParent(UIElement* parent);
    UIElement* getParent() const { return parent_; }
    const std::vector<std::shared_ptr<UIElement>>& getChildren() const { return children_; }

    // Virtual interface
    virtual void update(float deltaTime) { (void)deltaTime; }
    virtual void render() = 0;
    virtual bool handleInput(float mouseX, float mouseY, bool clicked) { (void)mouseX; (void)mouseY; (void)clicked; return false; }
    
    // New input handling methods for advanced UI components
    virtual void handleMouseButton(int button, int action, int mods, double xpos, double ypos) { (void)button; (void)action; (void)mods; (void)xpos; (void)ypos; }
    virtual void handleKey(int key, int scancode, int action, int mods) { (void)key; (void)scancode; (void)action; (void)mods; }
    virtual void handleChar(unsigned int codepoint) { (void)codepoint; }

    virtual bool isFocused() const { return false; }

    // Utility methods
    bool containsPoint(float x, float y) const;
    glm::vec2 getAbsolutePosition() const;

protected:
    glm::vec2 position_{0.0f, 0.0f};  // Position in pixels (screen space)
    glm::vec2 size_{100.0f, 100.0f}; // Size in pixels
    bool visible_{true};
    
    UIElement* parent_{nullptr};
    std::vector<std::shared_ptr<UIElement>> children_;
};

} // namespace UI
} // namespace VoxelEngine
