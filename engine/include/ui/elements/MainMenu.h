#pragma once

#include "ui/UIElement.h"
#include "ui/elements/UIPanel.h"
#include "ui/elements/UIButton.h"
#include <memory>
#include <string>

namespace VoxelEngine {
namespace UI {

class MenuSystem;

/**
 * @brief Main menu UI element
 * 
 * Provides a container for the main menu options including Close and Settings buttons.
 */
class MainMenu : public UIPanel {
public:
    MainMenu(UIRenderer* renderer);
    ~MainMenu() override = default;

    /**
     * @brief Initialize the main menu
     * @param menuSystem Pointer to the menu system that owns this menu
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(MenuSystem* menuSystem);

    /**
     * @brief Update the main menu
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

private:
    MenuSystem* menuSystem_;
    std::shared_ptr<UIButton> closeButton_;
    std::shared_ptr<UIButton> settingsButton_;
    std::shared_ptr<UIPanel> titlePanel_;

    void onCloseClicked();
    void onSettingsClicked();
};

} // namespace UI
} // namespace VoxelEngine
