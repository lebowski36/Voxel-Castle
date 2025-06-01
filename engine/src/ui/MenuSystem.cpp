#include "ui/MenuSystem.h"
#include "ui/elements/MainMenu.h"
#include "ui/elements/SettingsMenu.h"
#include <iostream>
#include <typeinfo>  // For typeid

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
    
    // Set widths only - preserve auto-calculated heights
    glm::vec2 mainSize = mainMenu_->getSize();
    glm::vec2 settingsSize = settingsMenu_->getSize();
    mainMenu_->setSize(400.0f, mainSize.y); // Keep auto-calculated height
    settingsMenu_->setSize(450.0f, settingsSize.y); // Keep auto-calculated height
    
    // Position menus after they auto-calculate their heights
    // Note: This will be called again in showMainMenu/showSettingsMenu to ensure proper centering
    centerMenus(screenWidth, screenHeight);
    
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
    // Re-center menus in case they were auto-resized
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
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
    // Re-center menus in case they were auto-resized
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
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

bool MenuSystem::toggleFullscreen(bool enable) {
    if (onFullscreenToggle_) {
        return onFullscreenToggle_(enable);
    }
    
    // If no callback is set, just log a message and return false
    std::cout << "[MenuSystem] No fullscreen toggle callback set" << std::endl;
    return false;
}

void MenuSystem::debugDumpMenuState() {
    std::cout << "\n[MenuSystem] === DEBUG MENU STATE DUMP ===" << std::endl;
    
    // Report current menu state
    std::cout << "[MenuSystem] Current State: ";
    switch (menuState_) {
        case MenuState::NONE:      std::cout << "NONE (No menu active)"; break;
        case MenuState::MAIN_MENU: std::cout << "MAIN_MENU"; break;
        case MenuState::SETTINGS:  std::cout << "SETTINGS"; break;
        default:                   std::cout << "UNKNOWN (" << static_cast<int>(menuState_) << ")"; break;
    }
    std::cout << std::endl;
    
    // Report menu visibility
    std::cout << "[MenuSystem] Main Menu: " << (mainMenu_ && mainMenu_->isVisible() ? "Visible" : "Hidden") << std::endl;
    std::cout << "[MenuSystem] Settings Menu: " << (settingsMenu_ && settingsMenu_->isVisible() ? "Visible" : "Hidden") << std::endl;
    
    // Report callback states
    std::cout << "[MenuSystem] OnMenuClosed callback: " << (onMenuClosed_ ? "Set" : "Not set") << std::endl;
    std::cout << "[MenuSystem] OnFullscreenToggle callback: " << (onFullscreenToggle_ ? "Set" : "Not set") << std::endl;
    
    // List all UI elements
    std::cout << "[MenuSystem] All UI Elements (" << elements_.size() << " total):" << std::endl;
    for (size_t i = 0; i < elements_.size(); i++) {
        const auto& element = elements_[i];
        std::cout << "  [" << i << "] Type: " << typeid(*element).name()
                  << ", Visible: " << (element->isVisible() ? "Yes" : "No")
                  << ", Pos: (" << element->getPosition().x << "," << element->getPosition().y << ")"
                  << ", Size: (" << element->getSize().x << "x" << element->getSize().y << ")"
                  << std::endl;
    }
    
    std::cout << "[MenuSystem] === END DEBUG MENU STATE DUMP ===" << std::endl;
}

void MenuSystem::updateScreenSize(int width, int height) {
    // Double-check the dimensions we're working with to debug issues
    std::cout << "[MenuSystem] Updating screen size to " << width << "x" << height << std::endl;
    
    // Update both the MenuSystem and underlying UISystem screen size
    setScreenSize(width, height);
    
    // Explicitly update the renderer dimensions too - we need to use getRenderer() since we inherit from UISystem
    getRenderer().setScreenSize(width, height);
    
    // KEEP MENU SIZES COMPLETELY FIXED - do not resize based on screen dimensions
    // The menus should maintain their auto-calculated sizes regardless of screen size
    // Only reposition them to stay centered
    
    // Center menus without changing their sizes
    centerMenus(width, height);
    
    // Verify that the changes took effect
    int currentRendererWidth = getRenderer().getScreenWidth();
    int currentRendererHeight = getRenderer().getScreenHeight();
    glm::vec2 mainSize = mainMenu_->getSize();
    glm::vec2 settingsSize = settingsMenu_->getSize();
    
    std::cout << "[MenuSystem] Screen size updated. Renderer now: " << currentRendererWidth << "x" << currentRendererHeight 
              << ", menu sizes preserved: main(" << mainSize.x << "x" << mainSize.y << ") settings(" 
              << settingsSize.x << "x" << settingsSize.y << ")" << std::endl;
}

void MenuSystem::updateFullscreenState(bool isFullscreen) {
    if (settingsMenu_) {
        settingsMenu_->updateFullscreenCheckbox(isFullscreen);
        std::cout << "[MenuSystem] Fullscreen state updated to " << (isFullscreen ? "ON" : "OFF") << std::endl;
    }
}

void MenuSystem::requestExit() {
    if (onExitRequest_) {
        onExitRequest_();
    } else {
        std::cout << "[MenuSystem] No exit request callback set" << std::endl;
    }
}

void MenuSystem::centerMenus(int screenWidth, int screenHeight) {
    // Center main menu
    glm::vec2 mainSize = mainMenu_->getSize();
    float mainX = screenWidth / 2.0f - mainSize.x / 2.0f;
    float mainY = screenHeight / 2.0f - mainSize.y / 2.0f;
    mainMenu_->setPosition(mainX, mainY);
    
    // Center settings menu
    glm::vec2 settingsSize = settingsMenu_->getSize();
    float settingsX = screenWidth / 2.0f - settingsSize.x / 2.0f;
    float settingsY = screenHeight / 2.0f - settingsSize.y / 2.0f;
    settingsMenu_->setPosition(settingsX, settingsY);
}

} // namespace UI
} // namespace VoxelEngine
