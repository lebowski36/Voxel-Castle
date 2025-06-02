#include "ui/MenuSystem.h"
#include "ui/elements/MainMenu.h"
#include "ui/elements/SettingsMenu.h"
#include <iostream>
#include <typeinfo>  // For typeid
#include <algorithm> // For std::max
#include "../../../../game/include/utils/debug_logger.h"

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
    
    // Report UI element details
    std::cout << "[MenuSystem] Active UI Elements: " << elements_.size() << std::endl;
    for (size_t i = 0; i < elements_.size(); ++i) {
        const auto& element = elements_[i];
        std::cout << "[MenuSystem] Element " << i << ": " 
                  << "Type: " << typeid(*element.get()).name()
                  << ", Visible: " << (element->isVisible() ? "Yes" : "No") 
                  << ", Position: (" << element->getPosition().x << ", " << element->getPosition().y << ")"
                  << ", Size: (" << element->getSize().x << "x" << element->getSize().y << ")"
                  << std::endl;
    }
    
    std::cout << "[MenuSystem] === END DEBUG MENU STATE DUMP ===" << std::endl;
}

void MenuSystem::updateScreenSize(int width, int height) {
    // Cache the fixed menu dimensions before any updates
    static const float MAIN_MENU_WIDTH = 400.0f;
    static const float SETTINGS_MENU_WIDTH = 450.0f;
    
    // Store the heights the first time this is called
    static float mainMenuHeight = 0.0f;
    static float settingsMenuHeight = 0.0f;
    
    // Store the heights the first time this is called
    if (mainMenuHeight == 0.0f && mainMenu_) {
        mainMenuHeight = mainMenu_->getSize().y;
    }
    if (settingsMenuHeight == 0.0f && settingsMenu_) {
        settingsMenuHeight = settingsMenu_->getSize().y;
    }
    
    // Suppressed repetitive logging for menu system operations.
    
    // IMPORTANT: Store references to the menus before updating renderer to avoid brief size flashes
    std::shared_ptr<BaseMenu> mainMenuRef = mainMenu_;
    std::shared_ptr<BaseMenu> settingsMenuRef = settingsMenu_;
    
    // Temporarily remove menus from the UI system's element list to avoid rendering at wrong size
    std::vector<std::shared_ptr<UIElement>> tempMenus;
    for (auto it = elements_.begin(); it != elements_.end();) {
        if ((*it).get() == mainMenu_.get() || (*it).get() == settingsMenu_.get()) {
            tempMenus.push_back(*it);
            it = elements_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update the renderer's dimensions (viewport, projection matrix)
    getRenderer().setScreenSize(width, height);
    
    // Explicitly reset menu sizes to their original fixed dimensions BEFORE adding them back
    if (mainMenuRef) {
        mainMenuRef->setSize(MAIN_MENU_WIDTH, mainMenuHeight);
    }
    
    if (settingsMenuRef) {
        settingsMenuRef->setSize(SETTINGS_MENU_WIDTH, settingsMenuHeight);
    }
    
    // Re-add the menus to the element list
    for (auto& menu : tempMenus) {
        elements_.push_back(menu);
    }
    
    // Center the menus with the new screen dimensions
    centerMenus(width, height);
    
    // Verify menu sizes are preserved
    if (mainMenu_ && settingsMenu_) {
        glm::vec2 mainSize = mainMenu_->getSize();
        glm::vec2 settingsSize = settingsMenu_->getSize();
        
        DEBUG_LOG("MenuSystem", "Verification - Main menu size: " + std::to_string(static_cast<int>(mainSize.x)) + 
                  "x" + std::to_string(static_cast<int>(mainSize.y)) + ", Settings menu size: " + 
                  std::to_string(static_cast<int>(settingsSize.x)) + "x" + std::to_string(static_cast<int>(settingsSize.y)));
        
        // Correct sizes if somehow they got changed
        if (std::abs(mainSize.x - MAIN_MENU_WIDTH) > 0.1f || 
            std::abs(mainSize.y - mainMenuHeight) > 0.1f) {
            DEBUG_LOG("MenuSystem", "WARNING: Main menu size incorrect, resetting...");
            mainMenu_->setSize(MAIN_MENU_WIDTH, mainMenuHeight);
        }
        
        if (std::abs(settingsSize.x - SETTINGS_MENU_WIDTH) > 0.1f || 
            std::abs(settingsSize.y - settingsMenuHeight) > 0.1f) {
            DEBUG_LOG("MenuSystem", "WARNING: Settings menu size incorrect, resetting...");
            settingsMenu_->setSize(SETTINGS_MENU_WIDTH, settingsMenuHeight);
        }
    }
}

void MenuSystem::updateFullscreenState(bool isFullscreen) {
    if (settingsMenu_) {
        settingsMenu_->updateFullscreenCheckbox(isFullscreen);
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
    // Center main menu with bounds checks to prevent off-screen menus
    glm::vec2 mainSize = mainMenu_->getSize();
    
    // Default to centered position
    float idealMainX = screenWidth / 2.0f - mainSize.x / 2.0f;
    float idealMainY = screenHeight / 2.0f - mainSize.y / 2.0f;
    
    // Constrain to visible area with 10px margins
    float mainX = std::max(10.0f, std::min(idealMainX, screenWidth - mainSize.x - 10.0f));
    float mainY = std::max(10.0f, std::min(idealMainY, screenHeight - mainSize.y - 10.0f));
    
    // If screen is too small for the menu, prioritize showing top-left
    if (screenWidth < mainSize.x + 20.0f || screenHeight < mainSize.y + 20.0f) {
        mainX = 10.0f; // Pin to left with margin
        mainY = 10.0f; // Pin to top with margin
    }
    
    mainMenu_->setPosition(mainX, mainY);
    
    // Center settings menu with similar constraints
    glm::vec2 settingsSize = settingsMenu_->getSize();
    
    // Default to centered position
    float idealSettingsX = screenWidth / 2.0f - settingsSize.x / 2.0f;
    float idealSettingsY = screenHeight / 2.0f - settingsSize.y / 2.0f;
    
    // Constrain to visible area with 10px margins
    float settingsX = std::max(10.0f, std::min(idealSettingsX, screenWidth - settingsSize.x - 10.0f));
    float settingsY = std::max(10.0f, std::min(idealSettingsY, screenHeight - settingsSize.y - 10.0f));
    
    // If screen is too small for the menu, prioritize showing top-left
    if (screenWidth < settingsSize.x + 20.0f || screenHeight < settingsSize.y + 20.0f) {
        settingsX = 10.0f; // Pin to left with margin
        settingsY = 10.0f; // Pin to top with margin
    }
    
    settingsMenu_->setPosition(settingsX, settingsY);
}

glm::vec2 MenuSystem::getMainMenuSize() const {
    if (mainMenu_) {
        return mainMenu_->getSize();
    }
    return glm::vec2(0.0f, 0.0f);
}

glm::vec2 MenuSystem::getSettingsMenuSize() const {
    if (settingsMenu_) {
        return settingsMenu_->getSize();
    }
    return glm::vec2(0.0f, 0.0f);
}

} // namespace UI
} // namespace VoxelEngine
