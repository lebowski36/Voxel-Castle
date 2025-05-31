#include "ui/elements/MainMenu.h"
#include "ui/MenuSystem.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

MainMenu::MainMenu(UIRenderer* renderer) : UIPanel(renderer) {
    // Set panel properties (dark semi-transparent background)
    setColor({0.1f, 0.1f, 0.1f, 0.8f});
}

bool MainMenu::initialize(MenuSystem* menuSystem) {
    if (!menuSystem) {
        std::cerr << "[MainMenu] MenuSystem pointer is null" << std::endl;
        return false;
    }

    menuSystem_ = menuSystem;

    // Create title panel
    titlePanel_ = std::make_shared<UIPanel>(renderer_);
    titlePanel_->setColor({0.2f, 0.2f, 0.3f, 0.9f});
    titlePanel_->setPosition(0.0f, 0.0f);
    titlePanel_->setSize(getSize().x, 40.0f);
    addChild(titlePanel_);

    // Create buttons
    float buttonWidth = getSize().x - 40.0f;
    float buttonHeight = 40.0f;
    float buttonX = 20.0f;
    float buttonYStart = 60.0f;
    float buttonSpacing = 20.0f;

    // Close button (returns to game)
    closeButton_ = std::make_shared<UIButton>(renderer_);
    closeButton_->setText("Resume Game");
    closeButton_->setPosition(buttonX, buttonYStart);
    closeButton_->setSize(buttonWidth, buttonHeight);
    closeButton_->setOnClick([this]() { onCloseClicked(); });
    addChild(closeButton_);

    // Settings button
    settingsButton_ = std::make_shared<UIButton>(renderer_);
    settingsButton_->setText("Settings");
    settingsButton_->setPosition(buttonX, buttonYStart + buttonHeight + buttonSpacing);
    settingsButton_->setSize(buttonWidth, buttonHeight);
    settingsButton_->setOnClick([this]() { onSettingsClicked(); });
    addChild(settingsButton_);

    return true;
}

void MainMenu::update(float deltaTime) {
    UIPanel::update(deltaTime);
}

bool MainMenu::handleInput(float mouseX, float mouseY, bool clicked) {
    return UIPanel::handleInput(mouseX, mouseY, clicked);
}

void MainMenu::onCloseClicked() {
    if (menuSystem_) {
        menuSystem_->closeMenus();
    }
}

void MainMenu::onSettingsClicked() {
    if (menuSystem_) {
        menuSystem_->showSettingsMenu();
    }
}

} // namespace UI
} // namespace VoxelEngine
