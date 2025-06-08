#include "ui/elements/SettingsMenu.h"
#include "ui/MenuSystem.h"
#include "utils/debug_logger_stub.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

SettingsMenu::SettingsMenu(UIRenderer* renderer) : BaseMenu(renderer, "Settings") {
}

bool SettingsMenu::initialize(MenuSystem* menuSystem) {
    if (!initializeBase(menuSystem)) {
        return false;
    }

    // Create fullscreen checkbox
    float checkboxWidth = getSize().x * BUTTON_WIDTH_RATIO;
    float checkboxX = (getSize().x - checkboxWidth) / 2.0f;
    
    fullscreenCheckbox_ = std::make_shared<UICheckbox>(renderer_);
    fullscreenCheckbox_->setLabel("Fullscreen");
    fullscreenCheckbox_->setPosition(checkboxX, getNextElementY());
    fullscreenCheckbox_->setSize(checkboxWidth, BUTTON_HEIGHT);
    fullscreenCheckbox_->setOnToggle([this](bool checked) { onFullscreenToggled(checked); });
    addChild(fullscreenCheckbox_);
    addElementSpacing();

    // Add some extra spacing before the back button
    addElementSpacing(40.0f);

    // Back button
    backButton_ = createStyledButton("Back", getNextElementY());
    backButton_->setOnClick([this]() { onBackClicked(); });
    
    // Request menu centering after all components are initialized
    // This ensures proper positioning in both windowed and fullscreen modes
    if (menuSystem_) {
        menuSystem_->requestMenuRecentering();
    }

    return true;
}

void SettingsMenu::updateFullscreenCheckbox(bool isFullscreen) {
    if (fullscreenCheckbox_) {
        fullscreenCheckbox_->setChecked(isFullscreen);
    }
}

void SettingsMenu::onBackClicked() {
    if (menuSystem_) {
        menuSystem_->showMainMenu();
    }
}

void SettingsMenu::onFullscreenToggled(bool checked) {
    if (menuSystem_) {
        bool success = menuSystem_->toggleFullscreen(checked);
        DEBUG_LOG("SettingsMenu", "Fullscreen toggled " + std::string(checked ? "ON" : "OFF") + 
                  " - result: " + std::string(success ? "SUCCESS" : "FAILED"));
        
        // If toggle failed, update checkbox to match actual state
        if (!success) {
            fullscreenCheckbox_->setChecked(!checked);
        }
    }
}

} // namespace UI
} // namespace VoxelEngine