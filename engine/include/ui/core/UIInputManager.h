#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <memory>

namespace VoxelEngine {
namespace UI {

class UIComponent;

/**
 * @brief Handles input events and distributes them to UI components
 * 
 * The UIInputManager is responsible for:
 * - Processing mouse and keyboard events
 * - Hit testing to find which components should receive events
 * - Managing focus state and input capture
 * - Coordinating with the main UI system
 */
class UIInputManager {
public:
    UIInputManager();
    ~UIInputManager();

    // Input event processing
    void ProcessMouseMove(float x, float y);
    void ProcessMouseDown(int button, float x, float y);
    void ProcessMouseUp(int button, float x, float y);
    void ProcessKeyDown(int key);
    void ProcessKeyUp(int key);
    void ProcessTextInput(const char* text);

    // Component management
    void SetRootComponent(std::shared_ptr<UIComponent> root);
    void SetFocusedComponent(std::shared_ptr<UIComponent> component);
    std::shared_ptr<UIComponent> GetFocusedComponent() const;

    // Hit testing
    std::shared_ptr<UIComponent> GetComponentAt(float x, float y);

    // Mouse state
    glm::vec2 GetMousePosition() const { return mousePosition_; }
    bool IsMouseButtonDown(int button) const;

    // Focus management
    void ClearFocus();
    void FocusNext();
    void FocusPrevious();

private:
    glm::vec2 mousePosition_;
    std::vector<bool> mouseButtons_;
    std::weak_ptr<UIComponent> rootComponent_;
    std::weak_ptr<UIComponent> focusedComponent_;
    std::weak_ptr<UIComponent> hoveredComponent_;
    
    // Internal helper methods
    void UpdateHoveredComponent(float x, float y);
    std::shared_ptr<UIComponent> FindComponentAt(std::shared_ptr<UIComponent> parent, float x, float y);
};

} // namespace UI
} // namespace VoxelEngine
