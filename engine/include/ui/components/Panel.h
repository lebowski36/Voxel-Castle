#pragma once

#include "ui/core/UIComponent.h"
#include "ui/core/UITheme.h"

namespace VoxelEngine {
namespace UI {

/**
 * @brief Container component with background and border
 * 
 * A versatile container that can hold other UI components with:
 * - Customizable background color and opacity
 * - Border styling with color and thickness
 * - Rounded corners support
 * - Padding for child layout
 * - Clipping support for overflow content
 */
class Panel : public UIComponent {
public:
    Panel();
    ~Panel() override = default;
    
    // Background styling
    void SetBackgroundColor(const glm::vec4& color);
    const glm::vec4& GetBackgroundColor() const { return backgroundColor_; }
    
    void SetBackgroundVisible(bool visible);
    bool IsBackgroundVisible() const { return backgroundVisible_; }
    
    // Border styling
    void SetBorderColor(const glm::vec4& color);
    const glm::vec4& GetBorderColor() const { return borderColor_; }
    
    void SetBorderWidth(float width);
    float GetBorderWidth() const { return borderWidth_; }
    
    void SetBorderVisible(bool visible);
    bool IsBorderVisible() const { return borderVisible_; }
    
    // Corner rounding
    void SetCornerRadius(float radius);
    float GetCornerRadius() const { return cornerRadius_; }
    
    // Padding (affects child layout)
    void SetPadding(const glm::vec4& padding); // left, top, right, bottom
    void SetPadding(float padding) { SetPadding(glm::vec4(padding)); }
    void SetPadding(float horizontal, float vertical) { SetPadding(glm::vec4(horizontal, vertical, horizontal, vertical)); }
    const glm::vec4& GetPadding() const { return padding_; }
    
    // Content area (size minus padding)
    glm::vec2 GetContentSize() const;
    glm::vec2 GetContentPosition() const;
    
    // Clipping
    void SetClipContent(bool clip);
    bool IsContentClipped() const { return clipContent_; }
    
protected:
    void OnRender() override;
    
private:
    void RenderBackground();
    void RenderBorder();
    
    glm::vec4 backgroundColor_;
    bool backgroundVisible_;
    
    glm::vec4 borderColor_;
    float borderWidth_;
    bool borderVisible_;
    
    float cornerRadius_;
    glm::vec4 padding_; // left, top, right, bottom
    bool clipContent_;
};

} // namespace UI
} // namespace VoxelEngine
