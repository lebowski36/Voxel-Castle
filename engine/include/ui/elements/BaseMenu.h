#pragma once

#include "ui/elements/UIPanel.h"
#include "ui/elements/UIButton.h"
#include <memory>
#include <string>

namespace VoxelEngine {
namespace UI {

class MenuSystem;

/**
 * @brief Base class for all menu UI elements
 * 
 * Provides common styling, layout, and functionality for menus including
 * title bar, button positioning, and consistent visual design.
 */
class BaseMenu : public UIPanel {
public:
    BaseMenu(UIRenderer* renderer, const std::string& title = "");
    ~BaseMenu() override = default;

    /**
     * @brief Initialize the base menu with common elements
     * @param menuSystem Pointer to the menu system
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool initializeBase(MenuSystem* menuSystem);

    /**
     * @brief Update the menu
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime) override;

    /**
     * @brief Handle input events
     * @param mouseX X position of the mouse in screen coordinates
     * @param mouseY Y position of the mouse in screen coordinates
     * @param clicked Whether the mouse button is clicked
     * @return true if the input was handled, false otherwise
     */
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

    /**
     * @brief Render the menu including title text
     */
    void render() override;

    /**
     * @brief Set the menu title
     * @param title New title to display
     */
    void setTitle(const std::string& title);

protected:
    /**
     * @brief Create a styled button with consistent appearance
     * @param text Button text
     * @param yPosition Y position for the button
     * @return Shared pointer to the created button
     */
    std::shared_ptr<UIButton> createStyledButton(const std::string& text, float yPosition);

    /**
     * @brief Get the next available Y position for a new UI element
     * @return Y position for the next element
     */
    float getNextElementY();

    /**
     * @brief Add spacing for the next element
     * @param spacing Amount of spacing to add
     */
    void addElementSpacing(float spacing = 20.0f);

    // Common styling constants
    static constexpr float TITLE_HEIGHT = 80.0f; // Increased for better title visibility
    static constexpr float BUTTON_HEIGHT = 60.0f; // Increased to accommodate longer text
    static constexpr float BUTTON_WIDTH_RATIO = 0.7f; // Slightly narrower for better proportions
    static constexpr float ELEMENT_SPACING = 25.0f; // Increased spacing
    static constexpr float SIDE_PADDING = 40.0f;

    MenuSystem* menuSystem_;
    std::shared_ptr<UIPanel> titlePanel_;
    std::string title_;
    float currentY_; // Current Y position for next element

private:
    void createTitlePanel();
    void updateTitleText();
};

} // namespace UI
} // namespace VoxelEngine
