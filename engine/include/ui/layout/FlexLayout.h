#pragma once

#include "ui/layout/LayoutManager.h"
#include <vector>
#include <memory>

namespace VoxelEngine {
namespace UI {

/**
 * @brief Flexbox-style layout manager
 * 
 * Provides flexible box layout similar to CSS Flexbox with:
 * - Direction control (row/column)
 * - Justify content (start, center, end, space-between, space-around)
 * - Align items (start, center, end, stretch)
 * - Gap spacing between items
 * - Responsive behavior
 */
class FlexLayout : public LayoutManager {
public:
    enum class Direction {
        ROW,        // Horizontal layout
        COLUMN      // Vertical layout
    };
    
    enum class JustifyContent {
        START,          // Pack items to start
        CENTER,         // Pack items to center
        END,            // Pack items to end
        SPACE_BETWEEN,  // Distribute space between items
        SPACE_AROUND,   // Distribute space around items
        SPACE_EVENLY    // Distribute space evenly
    };
    
    enum class AlignItems {
        START,      // Align to start of cross axis
        CENTER,     // Align to center of cross axis
        END,        // Align to end of cross axis
        STRETCH     // Stretch to fill cross axis
    };
    
public:
    FlexLayout(Direction direction = Direction::COLUMN);
    ~FlexLayout() override = default;
    
    // Layout implementation
    void UpdateLayout() override;
    glm::vec2 CalculatePreferredSize() const override;
    
    // Configuration
    void SetDirection(Direction direction);
    Direction GetDirection() const { return direction_; }
    
    void SetJustifyContent(JustifyContent justify);
    JustifyContent GetJustifyContent() const { return justifyContent_; }
    
    void SetAlignItems(AlignItems align);
    AlignItems GetAlignItems() const { return alignItems_; }
    
    void SetGap(float gap);
    float GetGap() const { return gap_; }
    
    void SetPadding(float padding);
    void SetPadding(float top, float right, float bottom, float left);
    const glm::vec4& GetPadding() const { return padding_; } // top, right, bottom, left
    
    struct ChildInfo {
        std::shared_ptr<UIComponent> component;
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 preferredSize;
        float mainSize;
        float crossSize;
    };
    
    // Layout calculation methods
    std::vector<ChildInfo> GatherChildInfo() const;
    void PositionChildren(const std::vector<ChildInfo>& children);
    
private:
    Direction direction_;
    JustifyContent justifyContent_;
    AlignItems alignItems_;
    float gap_;
    glm::vec4 padding_; // top, right, bottom, left
    
    // Helper methods
    bool IsRow() const { return direction_ == Direction::ROW; }
    float GetMainAxisSize(const glm::vec2& size) const;
    float GetCrossAxisSize(const glm::vec2& size) const;
    glm::vec2 MakeSize(float main, float cross) const;
    float GetContentMainSize() const;
    float GetContentCrossSize() const;
};

} // namespace UI
} // namespace VoxelEngine
