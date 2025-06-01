#pragma once

#include "ui/elements/BaseMenu.h"
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
class MainMenu : public BaseMenu {
public:
    MainMenu(UIRenderer* renderer);
    ~MainMenu() override = default;

    /**
     * @brief Initialize the main menu
     * @param menuSystem Pointer to the menu system that owns this menu
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(MenuSystem* menuSystem);

private:
    std::shared_ptr<UIButton> closeButton_;
    std::shared_ptr<UIButton> settingsButton_;
    std::shared_ptr<UIButton> exitButton_;

    void onCloseClicked();
    void onSettingsClicked();
    void onExitClicked();
};

} // namespace UI
} // namespace VoxelEngine
