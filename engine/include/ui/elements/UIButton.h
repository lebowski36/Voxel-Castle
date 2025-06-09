#pragma once

#include "ui/UIElement.h"
#include "ui/UIRenderer.h"
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

/**
 * @brief A clickable button UI element
 */
class UIButton : public UIElement {
public:
    UIButton(UIRenderer* renderer);
    ~UIButton() override = default;

    /**
     * @brief Set the button's text
     * @param text The text to display on the button
     */
    void setText(const std::string& text);

    /**
     * @brief Get the button's text
     * @return The text displayed on the button
     */
    const std::string& getText() const { return text_; }

    /**
     * @brief Set the button's background color
     * @param color The color to use for the button background
     */
    void setBackgroundColor(const glm::vec4& color);

    /**
     * @brief Set the button's text color
     * @param color The color to use for the button text
     */
    void setTextColor(const glm::vec4& color);

    /**
     * @brief Set the button's hover color
     * @param color The color to use when the mouse is hovering over the button
     */
    void setHoverColor(const glm::vec4& color);

    /**
     * @brief Set the button's click color
     * @param color The color to use when the button is clicked
     */
    void setClickColor(const glm::vec4& color);

    /**
     * @brief Set the callback to be called when the button is clicked
     * @param callback The function to call
     */
    void setOnClick(std::function<void()> callback);

    /**
     * @brief Auto-size the button to fit its text content
     * @param padding Extra padding around the text (default: 10px on each side)
     * @param scale Text scale factor (default: 1.0f)
     */
    void autoSizeToText(float padding = 10.0f, float scale = 1.0f);

    // UIElement interface
    void update(float deltaTime) override;
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

private:
    UIRenderer* renderer_;
    std::string text_;
    glm::vec4 backgroundColor_{0.3f, 0.3f, 0.3f, 1.0f};
    glm::vec4 textColor_{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 hoverColor_{0.4f, 0.4f, 0.4f, 1.0f};
    glm::vec4 clickColor_{0.2f, 0.2f, 0.2f, 1.0f};
    std::function<void()> onClick_;
    bool isHovered_{false};
    bool isPressed_{false};
};

} // namespace UI
} // namespace VoxelEngine
