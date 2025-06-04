#include "ui/elements/MainMenu.h"
#include "ui/MenuSystem.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

MainMenu::MainMenu(UIRenderer* renderer) : BaseMenu(renderer, "Voxel Castle") {
}

bool MainMenu::initialize(MenuSystem* menuSystem) {
    if (!initializeBase(menuSystem)) {
        return false;
    }

    // Create buttons using the styled button helper
    closeButton_ = createStyledButton("Resume Game", getNextElementY());
    closeButton_->setOnClick([this]() { onCloseClicked(); });
    addElementSpacing();

    worldSelectionButton_ = createStyledButton("Load World", getNextElementY());
    worldSelectionButton_->setOnClick([this]() { onWorldSelectionClicked(); });
    addElementSpacing();

    createWorldButton_ = createStyledButton("Create New World", getNextElementY());
    createWorldButton_->setOnClick([this]() { onCreateWorldClicked(); });
    addElementSpacing();

    settingsButton_ = createStyledButton("Settings", getNextElementY());
    settingsButton_->setOnClick([this]() { onSettingsClicked(); });
    addElementSpacing();

    exitButton_ = createStyledButton("Exit Game", getNextElementY());
    exitButton_->setOnClick([this]() { onExitClicked(); });

    return true;
}

void MainMenu::onCloseClicked() {
    if (menuSystem_) {
        // Request world initialization before closing the menu
        menuSystem_->requestWorldInitialization();
        menuSystem_->closeMenus();
    }
}

void MainMenu::onSettingsClicked() {
    if (menuSystem_) {
        menuSystem_->showSettingsMenu();
    }
}

void MainMenu::onExitClicked() {
    if (menuSystem_) {
        menuSystem_->requestExit();
    }
}

void MainMenu::onWorldSelectionClicked() {
    // TODO: Show world selection dialog
    std::cout << "[MainMenu] World Selection clicked - TODO: Implement world selection dialog" << std::endl;
}

void MainMenu::onCreateWorldClicked() {
    // TODO: Show create world dialog
    std::cout << "[MainMenu] Create World clicked - TODO: Implement create world dialog" << std::endl;
}

} // namespace UI
} // namespace VoxelEngine