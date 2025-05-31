#include "ui/elements/SettingsMenu.h"
#include "ui/MenuSystem.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

SettingsMenu::SettingsMenu(UIRenderer* renderer) : UIPanel(renderer) {
    // Set panel properties (dark semi-transparent background)
    setColor({0.1f, 0.1f, 0.1f, 0.8f});
}

bool SettingsMenu::initialize(MenuSystem* menuSystem) {
    if (!menuSystem) {
        std::cerr << "[SettingsMenu] MenuSystem pointer is null" << std::endl;
        return false;
    }

    menuSystem_ = menuSystem;

    // Create title panel
    titlePanel_ = std::make_shared<UIPanel>(renderer_);
    titlePanel_->setColor({0.2f, 0.2f, 0.3f, 0.9f});
    titlePanel_->setPosition(0.0f, 0.0f);
    titlePanel_->setSize(getSize().x, 40.0f);
    addChild(titlePanel_);

    // Create settings controls
    float controlWidth = getSize().x - 40.0f;
    float controlHeight = 40.0f;
    float controlX = 20.0f;
    float controlYStart = 60.0f;
    float controlSpacing = 20.0f;

    // Fullscreen checkbox
    fullscreenCheckbox_ = std::make_shared<UICheckbox>(renderer_);
    fullscreenCheckbox_->setLabel("Fullscreen");
    fullscreenCheckbox_->setPosition(controlX, controlYStart);
    fullscreenCheckbox_->setSize(controlWidth, controlHeight);
    fullscreenCheckbox_->setOnToggle([this](bool checked) { onFullscreenToggled(checked); });
    addChild(fullscreenCheckbox_);

    // Back button
    backButton_ = std::make_shared<UIButton>(renderer_);
    backButton_->setText("Back");
    backButton_->setPosition(controlX, getSize().y - controlHeight - 20.0f);
    backButton_->setSize(controlWidth, controlHeight);
    backButton_->setOnClick([this]() { onBackClicked(); });
    addChild(backButton_);

    return true;
}

void SettingsMenu::update(float deltaTime) {
    UIPanel::update(deltaTime);
}

bool SettingsMenu::handleInput(float mouseX, float mouseY, bool clicked) {
    return UIPanel::handleInput(mouseX, mouseY, clicked);
}

void SettingsMenu::onBackClicked() {
    if (menuSystem_) {
        menuSystem_->showMainMenu();
    }
}

void SettingsMenu::onFullscreenToggled(bool checked) {
    // This would connect to the window management system to toggle fullscreen
    std::cout << "[SettingsMenu] Fullscreen toggled: " << (checked ? "ON" : "OFF") << std::endl;
    
    // TODO: Implement fullscreen toggle logic
    // This will be implemented in Phase 4 of the menu system implementation
}

} // namespace UI
} // namespace VoxelEngine
