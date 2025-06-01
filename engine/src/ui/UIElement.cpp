#include "ui/UIElement.h"
#include "../../game/include/utils/debug_logger.h"
#include <algorithm>
#include <iostream> // Added for std::cerr

namespace VoxelEngine {
namespace UI {

UIElement::UIElement() = default;

void UIElement::setPosition(float x, float y) {
    // Check if position is likely to be off-screen (using common sense values)
    // This is just a warning, not a restriction - redirect to file logging
    static const float MAX_REASONABLE_COORDINATE = 5000.0f; // Arbitrary large value
    if (x < 0 || y < 0 || x > MAX_REASONABLE_COORDINATE || y > MAX_REASONABLE_COORDINATE) {
        DEBUG_LOG("UIElement", "UI Element positioned outside reasonable bounds: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    
    position_ = glm::vec2(x, y);
}

void UIElement::setSize(float width, float height) {
    size_ = glm::vec2(width, height);
}

void UIElement::setVisible(bool visible) {
    visible_ = visible;
}

void UIElement::addChild(std::shared_ptr<UIElement> child) {
    if (child) {
        child->setParent(this);
        children_.push_back(child);
    }
}

void UIElement::removeChild(std::shared_ptr<UIElement> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void UIElement::setParent(UIElement* parent) {
    parent_ = parent;
}

bool UIElement::containsPoint(float x, float y) const {
    glm::vec2 absPos = getAbsolutePosition();
    bool contains = x >= absPos.x && x <= absPos.x + size_.x &&
                    y >= absPos.y && y <= absPos.y + size_.y;
    // std::cout << "[UIElement::containsPoint] Mouse (" << x << "," << y 
    //           << ") vs Element AbsPos: (" << absPos.x << "," << absPos.y 
    //           << ") Size: (" << size_.x << "," << size_.y 
    //           << ") Contains: " << std::boolalpha << contains << std::endl;
    return contains;
}

glm::vec2 UIElement::getAbsolutePosition() const {
    if (parent_) {
        return parent_->getAbsolutePosition() + position_;
    }
    return position_;
}

} // namespace UI
} // namespace VoxelEngine
