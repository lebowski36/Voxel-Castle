#include "ui/components/Label.h"
#include "ui/core/UISystem.h"
#include "ui/UIRenderer.h"
#include <algorithm>
#include <iostream>

namespace VoxelEngine {
namespace UI {

Label::Label(const std::string& text)
    : text_(text)
    , fontSize_(16.0f)
    , textColor_(1.0f, 1.0f, 1.0f, 1.0f) // White text
    , textAlign_(TextAlign::LEFT)
    , wordWrap_(false)
    , autoSize_(true)
    , measuredTextSize_(0, 0)
    , textSizeDirty_(true) {
}

void Label::SetText(const std::string& text) {
    if (text_ != text) {
        text_ = text;
        textSizeDirty_ = true;
        if (autoSize_) {
            InvalidateLayout();
        }
    }
}

void Label::SetFontSize(float fontSize) {
    if (fontSize_ != fontSize) {
        fontSize_ = fontSize;
        textSizeDirty_ = true;
        if (autoSize_) {
            InvalidateLayout();
        }
    }
}

void Label::SetTextColor(const glm::vec4& color) {
    textColor_ = color;
}

void Label::SetTextAlign(TextAlign align) {
    textAlign_ = align;
}

void Label::SetWordWrap(bool wordWrap) {
    if (wordWrap_ != wordWrap) {
        wordWrap_ = wordWrap;
        textSizeDirty_ = true;
        if (autoSize_) {
            InvalidateLayout();
        }
    }
}

void Label::SetAutoSize(bool autoSize) {
    autoSize_ = autoSize;
    if (autoSize) {
        InvalidateLayout();
    }
}

glm::vec2 Label::CalculatePreferredSize() const {
    if (autoSize_) {
        return MeasureText();
    }
    return GetSize();
}

void Label::OnRender() {
    if (text_.empty()) return;
    
    auto uiSystem = GetUISystem();
    if (!uiSystem) return;
    
    auto renderer = uiSystem->GetRenderer();
    if (!renderer) return;
    
    // Calculate text position based on alignment
    glm::vec2 textPos = CalculateTextPosition();
    glm::vec2 absolutePos = GetAbsolutePosition();
    
    // Apply UI scaling
    float scale = uiSystem->GetTotalScale();
    float scaledFontSize = fontSize_ * scale;
    
    // Debug output for title text positioning
    if (text_.find("Voxel Castle") != std::string::npos) {
        std::cout << "[Label::OnRender] Title \"" << text_ << "\" - AbsPos: (" 
                  << absolutePos.x << ", " << absolutePos.y << "), TextPos: (" 
                  << textPos.x << ", " << textPos.y << "), Final: (" 
                  << absolutePos.x + textPos.x << ", " << absolutePos.y + textPos.y 
                  << "), Scale: " << scale << ", FontSize: " << scaledFontSize << std::endl;
    }
    
    // Render the text
    renderer->drawText(
        text_,
        absolutePos.x + textPos.x,
        absolutePos.y + textPos.y,
        scaledFontSize / 32.0f, // Convert to renderer scale
        glm::vec3(textColor_.r, textColor_.g, textColor_.b)
    );
}

glm::vec2 Label::MeasureText() const {
    if (textSizeDirty_) {
        auto uiSystem = GetUISystem();
        if (uiSystem) {
            auto renderer = uiSystem->GetRenderer();
            if (renderer) {
                float scale = uiSystem->GetTotalScale();
                float scaledFontSize = fontSize_ * scale;
                
                // Measure text size
                float textWidth = renderer->getTextWidth(text_, scaledFontSize / 32.0f);
                float textHeight = renderer->getTextHeight(scaledFontSize / 32.0f);
                
                measuredTextSize_ = glm::vec2(textWidth, textHeight);
                textSizeDirty_ = false;
            }
        }
        
        if (measuredTextSize_.x == 0 && measuredTextSize_.y == 0) {
            // Fallback size estimation
            measuredTextSize_ = glm::vec2(text_.length() * fontSize_ * 0.6f, fontSize_ * 1.2f);
        }
    }
    
    return measuredTextSize_;
}

glm::vec2 Label::CalculateTextPosition() const {
    glm::vec2 size = GetSize();
    glm::vec2 textSize = MeasureText();
    
    glm::vec2 position(0, 0);
    
    // Horizontal alignment
    switch (textAlign_) {
        case TextAlign::LEFT:
            position.x = 0;
            break;
        case TextAlign::CENTER:
            position.x = (size.x - textSize.x) * 0.5f;
            break;
        case TextAlign::RIGHT:
            position.x = size.x - textSize.x;
            break;
    }
    
    // Vertical centering
    position.y = (size.y - textSize.y) * 0.5f;
    
    return position;
}

} // namespace UI
} // namespace VoxelEngine
