#pragma once

#include "ui/elements/BaseMenu.h"
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
class SettingsMenu : public BaseMenu {
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
     * @brief Update the fullscreen checkbox state
     * @param isFullscreen Current fullscreen state
     */
    void updateFullscreenCheckbox(bool isFullscreen);

private:
    std::shared_ptr<UIButton> backButton_;
    std::shared_ptr<UICheckbox> fullscreenCheckbox_;

    void onBackClicked();
    void onFullscreenToggled(bool checked);
};

} // namespace UI
} // namespace VoxelEngine
