#pragma once

#include "ui/UIElement.h"
#include "ui/UIRenderer.h"
#include <string>
#include <vector>

namespace VoxelEngine {
namespace UI {

/**
 * @brief A multi-line text panel that handles text wrapping and scrolling
 * 
 * This component automatically wraps text to fit within the panel width,
 * supports vertical scrolling for long content, and provides smooth text rendering.
 */
class UITextPanel : public UIElement {
public:
    UITextPanel(UIRenderer* renderer);
    ~UITextPanel() override = default;

    /**
     * @brief Set the text content of the panel
     * @param text The text to display (can contain newlines)
     */
    void setText(const std::string& text);

    /**
     * @brief Get the current text content
     * @return The text content
     */
    const std::string& getText() const { return rawText_; }

    /**
     * @brief Set the background color of the panel
     * @param color The background color (RGBA)
     */
    void setBackgroundColor(const glm::vec4& color);

    /**
     * @brief Set the text color
     * @param color The text color (RGB)
     */
    void setTextColor(const glm::vec3& color);

    /**
     * @brief Set the text scale/size
     * @param scale The text scale multiplier
     */
    void setTextScale(float scale);

    /**
     * @brief Set padding around the text
     * @param padding Padding in pixels
     */
    void setPadding(float padding);

    /**
     * @brief Enable or disable scrolling
     * @param enabled Whether scrolling is enabled
     */
    void setScrollEnabled(bool enabled);

    /**
     * @brief Set the maximum number of visible lines (0 = no limit)
     * @param maxLines Maximum visible lines
     */
    void setMaxVisibleLines(int maxLines);

    /**
     * @brief Scroll to the bottom of the text
     */
    void scrollToBottom();

    /**
     * @brief Scroll by a specific number of lines
     * @param lines Number of lines to scroll (positive = down, negative = up)
     */
    void scrollByLines(int lines);

    // UIElement interface
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

    /**
     * @brief Handle mouse wheel input for scrolling
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @param wheelDelta Mouse wheel delta
     * @return True if input was handled
     */
    bool handleMouseWheel(float mouseX, float mouseY, float wheelDelta);

private:
    /**
     * @brief Reprocess the text for wrapping and line breaks
     */
    void processText();

    /**
     * @brief Calculate optimal line breaks for the given text
     * @param text The text to process
     * @param maxWidth The maximum width in pixels
     * @return Vector of text lines
     */
    std::vector<std::string> wrapText(const std::string& text, float maxWidth);

    /**
     * @brief Get the width of a text string in pixels
     * @param text The text to measure
     * @return Width in pixels
     */
    float getTextWidth(const std::string& text);

    /**
     * @brief Get the height of a single line of text
     * @return Height in pixels
     */
    float getLineHeight();

    /**
     * @brief Update scroll position to keep it within valid bounds
     */
    void updateScrollBounds();

    /**
     * @brief Render the scrollbar if needed
     * @param panelPos Panel absolute position
     * @param visibleLines Number of visible lines
     */
    void renderScrollbar(const glm::vec2& panelPos, int visibleLines);

private:
    UIRenderer* renderer_;
    
    // Text content
    std::string rawText_;              // Original text with newlines
    std::vector<std::string> lines_;   // Processed lines after wrapping
    
    // Appearance
    glm::vec4 backgroundColor_;
    glm::vec3 textColor_;
    float textScale_;
    float padding_;
    
    // Scrolling
    bool scrollEnabled_;
    int scrollOffset_;                 // Lines scrolled from top
    int maxVisibleLines_;             // Maximum visible lines (0 = auto-calculate)
    int totalLines_;                  // Total number of lines in content
    
    // Layout
    bool textNeedsProcessing_;        // Flag to reprocess text when size changes
};

} // namespace UI
} // namespace VoxelEngine
