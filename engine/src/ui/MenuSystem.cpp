#include "ui/MenuSystem.h"
#include "ui/elements/MainMenu.h"
#include "ui/elements/SettingsMenu.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

MenuSystem::MenuSystem()
    : UISystem() {
}

MenuSystem::~MenuSystem() {
    // UISystem destructor will handle cleanup
}

bool MenuSystem::initialize(int screenWidth, int screenHeight, const std::string& projectRoot) {
    if (!UISystem::initialize(screenWidth, screenHeight, projectRoot)) {
        std::cerr << "[MenuSystem] Failed to initialize UI system" << std::endl;
        return false;
    }
    
    UIRenderer* renderer = &getRenderer();
    
    // Create main menu
    mainMenu_ = std::make_shared<MainMenu>(renderer);
    if (!mainMenu_->initialize(this)) {
        std::cerr << "[MenuSystem] Failed to initialize main menu" << std::endl;
        return false;
    }
    
    // Create settings menu
    settingsMenu_ = std::make_shared<SettingsMenu>(renderer);
    if (!settingsMenu_->initialize(this)) {
        std::cerr << "[MenuSystem] Failed to initialize settings menu" << std::endl;
        return false;
    }
    
    // Set initial positions and sizes
    mainMenu_->setPosition(screenWidth / 2.0f - 150.0f, screenHeight / 2.0f - 150.0f);
    mainMenu_->setSize(300.0f, 300.0f);
    
    settingsMenu_->setPosition(screenWidth / 2.0f - 200.0f, screenHeight / 2.0f - 200.0f);
    settingsMenu_->setSize(400.0f, 400.0f);
    
    // Hide all menus initially
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(false);
    
    // Add menus to UI system
    addElement(mainMenu_);
    addElement(settingsMenu_);
    
    std::cout << "[MenuSystem] Initialized successfully" << std::endl;
    return true;
}

void MenuSystem::update(float deltaTime) {
    UISystem::update(deltaTime);
}

void MenuSystem::render() {
    UISystem::render();
}

void MenuSystem::showMainMenu() {
    mainMenu_->setVisible(true);
    settingsMenu_->setVisible(false);
    menuState_ = MenuState::MAIN_MENU;
    std::cout << "[MenuSystem] Showing main menu" << std::endl;
    
    // Ensure the block selection UI stays hidden during menu display
    // Block selection UI is identified by checking if it's not one of our menu elements
    for (const auto& element : elements_) {
        if (element.get() != mainMenu_.get() && element.get() != settingsMenu_.get()) {
            element->setVisible(false);
        }
    }
}

void MenuSystem::showSettingsMenu() {
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(true);
    menuState_ = MenuState::SETTINGS;
    std::cout << "[MenuSystem] Showing settings menu" << std::endl;
    
    // Ensure game UI elements stay hidden when switching to settings
    for (const auto& element : elements_) {
        if (element.get() != mainMenu_.get() && element.get() != settingsMenu_.get()) {
            element->setVisible(false);
        }
    }
}

void MenuSystem::closeMenus() {
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(false);
    menuState_ = MenuState::NONE;
    
    // Call the onMenuClosed callback if it exists
    if (onMenuClosed_) {
        onMenuClosed_();
    }
    
    std::cout << "[MenuSystem] All menus closed" << std::endl;
    
    // Note: We don't restore visibility of game UI elements here
    // That should be handled by the Game class since it knows which
    // elements should be visible in gameplay mode
}

} // namespace UI
} // namespace VoxelEngine
