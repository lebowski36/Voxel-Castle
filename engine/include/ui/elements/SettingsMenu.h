#pragma once

#include "ui/UIElement.h"
#include "ui/elements/UIPanel.h"
#include "ui/elements/UIButton.h"
#include "ui/elements/UICheckbox.h"
#include <memory>
#include <string>

namespace VoxelEngine {
namespace UI {

class MenuSystem;

/**
 * @brief Settings menu UI element
 * 
 * Provides a container for game settings including fullscreen toggle and back button.
 */
class SettingsMenu : public UIPanel {
public:
    SettingsMenu(UIRenderer* renderer);
    ~SettingsMenu() override = default;

    /**
     * @brief Initialize the settings menu
     * @param menuSystem Pointer to the menu system that owns this menu
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(MenuSystem* menuSystem);

    /**
     * @brief Update the settings menu
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
    std::shared_ptr<UIButton> backButton_;
    std::shared_ptr<UICheckbox> fullscreenCheckbox_;
    std::shared_ptr<UIPanel> titlePanel_;

    void onBackClicked();
    void onFullscreenToggled(bool checked);
};

} // namespace UI
} // namespace VoxelEngine
