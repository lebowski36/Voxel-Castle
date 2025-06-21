#include "ui/layout/FlexLayout.h"
#include "ui/core/UIComponent.h"
#include <algorithm>
#include <numeric>

namespace VoxelEngine {
namespace UI {

FlexLayout::FlexLayout(Direction direction)
    : direction_(direction)
    , justifyContent_(JustifyContent::START)
    , alignItems_(AlignItems::START)
    , gap_(0.0f)
    , padding_(0.0f) {
}

void FlexLayout::UpdateLayout() {
    auto container = GetContainer();
    if (!container) return;
    
    auto children = GatherChildInfo();
    if (children.empty()) return;
    
    PositionChildren(children);
}

glm::vec2 FlexLayout::CalculatePreferredSize() const {
    auto container = GetContainer();
    if (!container) return glm::vec2(0);
    
    auto children = GatherChildInfo();
    if (children.empty()) {
        return glm::vec2(padding_.y + padding_.w, padding_.x + padding_.z); // right+left, top+bottom
    }
    
    float totalMainSize = 0;
    float maxCrossSize = 0;
    
    for (size_t i = 0; i < children.size(); ++i) {
        const auto& child = children[i];
        totalMainSize += child.mainSize;
        maxCrossSize = std::max(maxCrossSize, child.crossSize);
        
        if (i > 0) {
            totalMainSize += gap_;
        }
    }
    
    // Add padding
    totalMainSize += IsRow() ? (padding_.y + padding_.w) : (padding_.x + padding_.z);
    maxCrossSize += IsRow() ? (padding_.x + padding_.z) : (padding_.y + padding_.w);
    
    return MakeSize(totalMainSize, maxCrossSize);
}

void FlexLayout::SetDirection(Direction direction) {
    if (direction_ != direction) {
        direction_ = direction;
        InvalidateLayout();
    }
}

void FlexLayout::SetJustifyContent(JustifyContent justify) {
    if (justifyContent_ != justify) {
        justifyContent_ = justify;
        InvalidateLayout();
    }
}

void FlexLayout::SetAlignItems(AlignItems align) {
    if (alignItems_ != align) {
        alignItems_ = align;
        InvalidateLayout();
    }
}

void FlexLayout::SetGap(float gap) {
    if (gap_ != gap) {
        gap_ = gap;
        InvalidateLayout();
    }
}

void FlexLayout::SetPadding(float padding) {
    SetPadding(padding, padding, padding, padding);
}

void FlexLayout::SetPadding(float top, float right, float bottom, float left) {
    glm::vec4 newPadding(top, right, bottom, left);
    if (padding_ != newPadding) {
        padding_ = newPadding;
        InvalidateLayout();
    }
}

float FlexLayout::GetMainAxisSize(const glm::vec2& size) const {
    return IsRow() ? size.x : size.y;
}

float FlexLayout::GetCrossAxisSize(const glm::vec2& size) const {
    return IsRow() ? size.y : size.x;
}

glm::vec2 FlexLayout::MakeSize(float main, float cross) const {
    return IsRow() ? glm::vec2(main, cross) : glm::vec2(cross, main);
}

float FlexLayout::GetContentMainSize() const {
    auto container = GetContainer();
    if (!container) return 0;
    
    glm::vec2 containerSize = container->GetSize();
    float mainSize = GetMainAxisSize(containerSize);
    float paddingMain = IsRow() ? (padding_.y + padding_.w) : (padding_.x + padding_.z);
    
    return std::max(0.0f, mainSize - paddingMain);
}

float FlexLayout::GetContentCrossSize() const {
    auto container = GetContainer();
    if (!container) return 0;
    
    glm::vec2 containerSize = container->GetSize();
    float crossSize = GetCrossAxisSize(containerSize);
    float paddingCross = IsRow() ? (padding_.x + padding_.z) : (padding_.y + padding_.w);
    
    return std::max(0.0f, crossSize - paddingCross);
}

std::vector<FlexLayout::ChildInfo> FlexLayout::GatherChildInfo() const {
    auto container = GetContainer();
    if (!container) return {};
    
    std::vector<ChildInfo> children;
    children.reserve(container->GetChildren().size());
    
    for (auto& child : container->GetChildren()) {
        if (!child->IsVisible()) continue;
        
        ChildInfo info;
        info.component = child;
        info.preferredSize = child->CalculatePreferredSize();
        info.mainSize = GetMainAxisSize(info.preferredSize);
        info.crossSize = GetCrossAxisSize(info.preferredSize);
        
        children.push_back(info);
    }
    
    return children;
}

void FlexLayout::PositionChildren(const std::vector<ChildInfo>& children) {
    if (children.empty()) return;
    
    float contentMainSize = GetContentMainSize();
    float contentCrossSize = GetContentCrossSize();
    
    // Calculate total required main axis space
    float totalChildrenMainSize = 0;
    for (size_t i = 0; i < children.size(); ++i) {
        totalChildrenMainSize += children[i].mainSize;
        if (i > 0) totalChildrenMainSize += gap_;
    }
    
    // Calculate starting position and spacing for justify-content
    float mainStart = IsRow() ? padding_.w : padding_.x; // left or top padding
    float itemSpacing = gap_;
    float extraSpace = contentMainSize - totalChildrenMainSize;
    
    switch (justifyContent_) {
        case JustifyContent::START:
            // mainStart already set correctly
            break;
            
        case JustifyContent::CENTER:
            mainStart += extraSpace * 0.5f;
            break;
            
        case JustifyContent::END:
            mainStart += extraSpace;
            break;
            
        case JustifyContent::SPACE_BETWEEN:
            if (children.size() > 1) {
                itemSpacing = gap_ + extraSpace / (children.size() - 1);
            }
            break;
            
        case JustifyContent::SPACE_AROUND:
            if (!children.empty()) {
                float spacePerItem = extraSpace / children.size();
                mainStart += spacePerItem * 0.5f;
                itemSpacing = gap_ + spacePerItem;
            }
            break;
            
        case JustifyContent::SPACE_EVENLY:
            if (!children.empty()) {
                float spacePerGap = extraSpace / (children.size() + 1);
                mainStart += spacePerGap;
                itemSpacing = gap_ + spacePerGap;
            }
            break;
    }
    
    // Position each child
    float currentMainPos = mainStart;
    
    for (const auto& child : children) {
        // Calculate cross axis position
        float crossStart = IsRow() ? padding_.x : padding_.y; // top or left padding
        float childCrossSize = child.crossSize;
        
        switch (alignItems_) {
            case AlignItems::START:
                // crossStart already set correctly
                break;
                
            case AlignItems::CENTER:
                crossStart += (contentCrossSize - childCrossSize) * 0.5f;
                break;
                
            case AlignItems::END:
                crossStart += contentCrossSize - childCrossSize;
                break;
                
            case AlignItems::STRETCH:
                childCrossSize = contentCrossSize;
                break;
        }
        
        // Set child position and size
        glm::vec2 childPos = MakeSize(currentMainPos, crossStart);
        glm::vec2 childSize = MakeSize(child.mainSize, childCrossSize);
        
        child.component->SetPosition(childPos);
        child.component->SetSize(childSize);
        
        // Move to next position
        currentMainPos += child.mainSize + itemSpacing;
    }
}

} // namespace UI
} // namespace VoxelEngine
