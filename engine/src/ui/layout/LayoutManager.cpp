#include "ui/layout/LayoutManager.h"
#include "ui/core/UIComponent.h"

namespace VoxelEngine {
namespace UI {

void LayoutManager::SetContainer(std::weak_ptr<UIComponent> container) {
    container_ = container;
}

void LayoutManager::InvalidateLayout() {
    auto container = container_.lock();
    if (container) {
        container->InvalidateLayout();
    }
}

} // namespace UI
} // namespace VoxelEngine
