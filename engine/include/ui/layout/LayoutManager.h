#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

class UIComponent;

/**
 * @brief Base interface for layout managers
 * 
 * Layout managers automatically position and size child components
 * within their container, implementing different layout strategies.
 */
class LayoutManager {
public:
    virtual ~LayoutManager() = default;
    
    // Container management
    virtual void SetContainer(std::weak_ptr<UIComponent> container);
    std::shared_ptr<UIComponent> GetContainer() const { return container_.lock(); }
    
    // Layout operations
    virtual void UpdateLayout() = 0;
    virtual glm::vec2 CalculatePreferredSize() const = 0;
    
    // Layout invalidation
    void InvalidateLayout();
    
protected:
    std::weak_ptr<UIComponent> container_;
};

} // namespace UI
} // namespace VoxelEngine
