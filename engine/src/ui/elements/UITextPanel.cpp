#include "ui/elements/UITextPanel.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace VoxelEngine {
namespace UI {

UITextPanel::UITextPanel(UIRenderer* renderer)
    : UIElement()
    , renderer_(renderer)
    , backgroundColor_(0.1f, 0.1f, 0.1f, 0.8f)
    , textColor_(1.0f, 1.0f, 1.0f)
    , textScale_(1.0f)
    , padding_(8.0f)
    , scrollEnabled_(true)
    , scrollOffset_(0)
    , maxVisibleLines_(0)
    , totalLines_(0)
    , textNeedsProcessing_(true) {
    
    // Set default size
    setSize(300.0f, 200.0f);
}

void UITextPanel::setText(const std::string& text) {
    if (rawText_ != text) {
        rawText_ = text;
        textNeedsProcessing_ = true;
    }
}

void UITextPanel::setBackgroundColor(const glm::vec4& color) {
    backgroundColor_ = color;
}

void UITextPanel::setTextColor(const glm::vec3& color) {
    textColor_ = color;
}

void UITextPanel::setTextScale(float scale) {
    if (textScale_ != scale) {
        textScale_ = scale;
        textNeedsProcessing_ = true;
    }
}

void UITextPanel::setPadding(float padding) {
    if (padding_ != padding) {
        padding_ = padding;
        textNeedsProcessing_ = true;
    }
}

void UITextPanel::setScrollEnabled(bool enabled) {
    scrollEnabled_ = enabled;
    if (!enabled) {
        scrollOffset_ = 0;
    }
}

void UITextPanel::setMaxVisibleLines(int maxLines) {
    maxVisibleLines_ = maxLines;
}

void UITextPanel::scrollToBottom() {
    if (scrollEnabled_ && totalLines_ > 0) {
        int visibleLines = maxVisibleLines_;
        if (visibleLines <= 0) {
            // Auto-calculate visible lines based on panel height
            float availableHeight = size_.y - (padding_ * 2.0f);
            visibleLines = static_cast<int>(availableHeight / getLineHeight());
        }
        
        scrollOffset_ = std::max(0, totalLines_ - visibleLines);
    }
}

void UITextPanel::scrollByLines(int lines) {
    if (scrollEnabled_) {
        scrollOffset_ += lines;
        updateScrollBounds();
    }
}

void UITextPanel::render() {
    if (!isVisible() || !renderer_) {
        return;
    }
    
    // Process text if needed
    if (textNeedsProcessing_) {
        processText();
        textNeedsProcessing_ = false;
    }
    
    glm::vec2 pos = getAbsolutePosition();
    
    // Render background
    renderer_->renderColoredQuad(pos.x, pos.y, size_.x, size_.y, backgroundColor_);
    
    // Calculate text rendering area
    float textX = pos.x + padding_;
    float textY = pos.y + padding_;
    float textWidth = size_.x - (padding_ * 2.0f);
    float textHeight = size_.y - (padding_ * 2.0f);
    
    if (textWidth <= 0 || textHeight <= 0) {
        return; // Not enough space to render text
    }
    
    // Calculate how many lines we can display
    float lineHeight = getLineHeight();
    int maxDisplayLines = static_cast<int>(textHeight / lineHeight);
    
    if (maxVisibleLines_ > 0) {
        maxDisplayLines = std::min(maxDisplayLines, maxVisibleLines_);
    }
    
    // Render visible lines
    int startLine = scrollOffset_;
    int endLine = std::min(startLine + maxDisplayLines, static_cast<int>(lines_.size()));
    
    float currentY = textY;
    for (int i = startLine; i < endLine; ++i) {
        if (i >= 0 && i < static_cast<int>(lines_.size())) {
            renderer_->drawText(lines_[i], textX, currentY, textScale_, textColor_);
            currentY += lineHeight;
        }
    }
    
    // Render scrollbar if needed
    if (scrollEnabled_ && totalLines_ > maxDisplayLines) {
        renderScrollbar(pos, maxDisplayLines);
    }
}

bool UITextPanel::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!isVisible()) {
        return false;
    }
    
    // Check if mouse is within the panel
    if (!containsPoint(mouseX, mouseY)) {
        return false;
    }
    
    // Handle scrollbar interaction if enabled
    if (scrollEnabled_ && clicked) {
        // Simple click-to-scroll functionality
        glm::vec2 pos = getAbsolutePosition();
        float relativeY = mouseY - pos.y;
        float centerY = size_.y / 2.0f;
        
        if (relativeY < centerY) {
            // Clicked in upper half - scroll up
            scrollByLines(-1);
        } else {
            // Clicked in lower half - scroll down
            scrollByLines(1);
        }
        
        return true;
    }
    
    return false;
}

bool UITextPanel::handleMouseWheel(float mouseX, float mouseY, float wheelDelta) {
    if (!isVisible() || !scrollEnabled_) {
        return false;
    }
    
    // Check if mouse is within the panel
    if (!containsPoint(mouseX, mouseY)) {
        return false;
    }
    
    // Scroll based on wheel delta
    int scrollLines = static_cast<int>(-wheelDelta); // Negative for natural scrolling
    scrollByLines(scrollLines);
    
    return true;
}

void UITextPanel::processText() {
    lines_.clear();
    
    if (rawText_.empty()) {
        totalLines_ = 0;
        return;
    }
    
    // Calculate available width for text
    float availableWidth = size_.x - (padding_ * 2.0f);
    if (availableWidth <= 0) {
        totalLines_ = 0;
        return;
    }
    
    // Split text by existing newlines first
    std::vector<std::string> paragraphs;
    std::stringstream ss(rawText_);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (line.empty()) {
            paragraphs.push_back(""); // Preserve empty lines
        } else {
            paragraphs.push_back(line);
        }
    }
    
    // Wrap each paragraph
    for (const auto& paragraph : paragraphs) {
        if (paragraph.empty()) {
            lines_.push_back(""); // Preserve empty lines
        } else {
            auto wrappedLines = wrapText(paragraph, availableWidth);
            lines_.insert(lines_.end(), wrappedLines.begin(), wrappedLines.end());
        }
    }
    
    totalLines_ = static_cast<int>(lines_.size());
    updateScrollBounds();
}

std::vector<std::string> UITextPanel::wrapText(const std::string& text, float maxWidth) {
    std::vector<std::string> result;
    
    if (text.empty()) {
        return result;
    }
    
    std::string remainingText = text;
    
    while (!remainingText.empty()) {
        // Find the maximum number of characters that fit
        std::string testLine;
        size_t pos = 0;
        
        // Try to fit as many characters as possible
        while (pos < remainingText.length()) {
            testLine = remainingText.substr(0, pos + 1);
            if (getTextWidth(testLine) > maxWidth) {
                break;
            }
            pos++;
        }
        
        if (pos == 0) {
            // Even a single character doesn't fit - force it anyway
            result.push_back(remainingText.substr(0, 1));
            remainingText = remainingText.substr(1);
            continue;
        }
        
        // Try to break at a word boundary
        size_t breakPos = pos;
        if (pos < remainingText.length()) {
            // Look for a space to break at
            size_t spacePos = remainingText.find_last_of(' ', pos);
            if (spacePos != std::string::npos && spacePos > pos / 2) {
                // Found a good break point
                breakPos = spacePos;
            }
        }
        
        // Extract the line and add it to results
        std::string line = remainingText.substr(0, breakPos);
        result.push_back(line);
        
        // Remove the processed part from remaining text
        remainingText = remainingText.substr(breakPos);
        
        // Skip leading whitespace on the next line
        while (!remainingText.empty() && remainingText[0] == ' ') {
            remainingText = remainingText.substr(1);
        }
    }
    
    return result;
}

float UITextPanel::getTextWidth(const std::string& text) {
    if (!renderer_ || text.empty()) {
        return 0.0f;
    }
    
    return renderer_->getTextWidth(text, textScale_);
}

float UITextPanel::getLineHeight() {
    if (!renderer_) {
        return 16.0f; // Fallback height
    }
    
    // Get base font height and scale it
    float baseHeight = renderer_->getFontHeight();
    return baseHeight * textScale_ * 1.2f; // Add some line spacing
}

void UITextPanel::updateScrollBounds() {
    if (!scrollEnabled_) {
        scrollOffset_ = 0;
        return;
    }
    
    // Calculate maximum visible lines
    int maxDisplayLines = maxVisibleLines_;
    if (maxDisplayLines <= 0) {
        float availableHeight = size_.y - (padding_ * 2.0f);
        maxDisplayLines = static_cast<int>(availableHeight / getLineHeight());
    }
    
    // Clamp scroll offset to valid range
    int maxScrollOffset = std::max(0, totalLines_ - maxDisplayLines);
    scrollOffset_ = std::clamp(scrollOffset_, 0, maxScrollOffset);
}

void UITextPanel::renderScrollbar(const glm::vec2& panelPos, int visibleLines) {
    if (totalLines_ <= visibleLines) {
        return; // No need for scrollbar
    }
    
    // Scrollbar dimensions
    float scrollbarWidth = 8.0f;
    float scrollbarX = panelPos.x + size_.x - scrollbarWidth - 2.0f;
    float scrollbarY = panelPos.y + padding_;
    float scrollbarHeight = size_.y - (padding_ * 2.0f);
    
    // Scrollbar background
    glm::vec4 scrollbarBg(0.2f, 0.2f, 0.2f, 0.8f);
    renderer_->renderColoredQuad(scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight, scrollbarBg);
    
    // Scrollbar thumb
    float thumbHeight = (static_cast<float>(visibleLines) / totalLines_) * scrollbarHeight;
    float thumbY = scrollbarY + (static_cast<float>(scrollOffset_) / totalLines_) * scrollbarHeight;
    
    glm::vec4 thumbColor(0.6f, 0.6f, 0.6f, 0.9f);
    renderer_->renderColoredQuad(scrollbarX + 1.0f, thumbY, scrollbarWidth - 2.0f, thumbHeight, thumbColor);
}

} // namespace UI
} // namespace VoxelEngine
