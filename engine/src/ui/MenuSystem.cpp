#include "ui/MenuSystem.h"
#include "ui/elements/MainMenu.h"
#include "ui/elements/SettingsMenu.h"
#include "ui/elements/WorldCreationDialog.h"
#include "world/world_seed.h"
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
    
    // Create world creation dialog
    worldCreationDialog_ = std::make_shared<WorldCreationDialog>(renderer);
    if (!worldCreationDialog_->initialize(this)) {
        std::cerr << "[MenuSystem] Failed to initialize world creation dialog" << std::endl;
        return false;
    }
    
    // Set up world creation dialog callbacks
    worldCreationDialog_->setOnWorldCreate([this](const VoxelCastle::World::WorldSeed& seed, WorldCreationDialog::WorldSize size) {
        if (onWorldCreateRequest_) {
            onWorldCreateRequest_(seed, static_cast<int>(size));
        }
        closeMenus(); // Close dialog after world creation
    });
    
    worldCreationDialog_->setOnCancel([this]() {
        showMainMenu(); // Return to main menu
    });
    
    // Set widths only - preserve auto-calculated heights
    glm::vec2 mainSize = mainMenu_->getSize();
    glm::vec2 settingsSize = settingsMenu_->getSize();
    glm::vec2 worldCreationSize = worldCreationDialog_->getSize();
    mainMenu_->setSize(400.0f, mainSize.y); // Keep auto-calculated height
    settingsMenu_->setSize(450.0f, settingsSize.y); // Keep auto-calculated height
    worldCreationDialog_->setSize(600.0f, worldCreationSize.y); // Wider to accommodate longer text
    
    // Position menus after they auto-calculate their heights
    // Note: This will be called again in showMainMenu/showSettingsMenu to ensure proper centering
    centerMenus(screenWidth, screenHeight);
    
    // Hide all menus initially
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(false);
    worldCreationDialog_->setVisible(false);
    
    // Add menus to UI system
    addElement(mainMenu_);
    addElement(settingsMenu_);
    addElement(worldCreationDialog_);
    
    return true;
}

void MenuSystem::update(float deltaTime) {
    UISystem::update(deltaTime);
}

void MenuSystem::render() {
    UISystem::render();
}

void MenuSystem::showMainMenu() {
    // First make the menu visible to ensure its size is calculated correctly
    mainMenu_->setVisible(true);
    settingsMenu_->setVisible(false);
    worldCreationDialog_->setVisible(false);
    menuState_ = MenuState::MAIN_MENU;
    
    // Now explicitly re-center menus after setting visibility
    // This ensures dimensions are correct when centering
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
    // Ensure the block selection UI stays hidden during menu display
    // Block selection UI is identified by checking if it's not one of our menu elements
    for (const auto& element : elements_) {
        if (element.get() != mainMenu_.get() && element.get() != settingsMenu_.get() && element.get() != worldCreationDialog_.get()) {
            element->setVisible(false);
        }
    }
    
    // Double-check the position to ensure it's properly centered
    float screenWidth = getRenderer().getScreenWidth();
    float screenHeight = getRenderer().getScreenHeight();
    glm::vec2 menuSize = mainMenu_->getSize();
    mainMenu_->setPosition(
        screenWidth / 2.0f - menuSize.x / 2.0f,
        screenHeight / 2.0f - menuSize.y / 2.0f
    );
}

void MenuSystem::showSettingsMenu() {
    // First make the menu visible to ensure its size is calculated correctly
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(true);
    worldCreationDialog_->setVisible(false);
    menuState_ = MenuState::SETTINGS;
    
    // Now explicitly re-center menus after setting visibility
    // This ensures dimensions are correct when centering
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
    // Ensure game UI elements stay hidden when switching to settings
    for (const auto& element : elements_) {
        if (element.get() != mainMenu_.get() && element.get() != settingsMenu_.get() && element.get() != worldCreationDialog_.get()) {
            element->setVisible(false);
        }
    }
    
    // Double-check the position to ensure it's properly centered
    float screenWidth = getRenderer().getScreenWidth();
    float screenHeight = getRenderer().getScreenHeight();
    glm::vec2 menuSize = settingsMenu_->getSize();
    settingsMenu_->setPosition(
        screenWidth / 2.0f - menuSize.x / 2.0f,
        screenHeight / 2.0f - menuSize.y / 2.0f
    );
}

void MenuSystem::showWorldCreationDialog() {
    // First make the dialog visible to ensure its size is calculated correctly
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(false);
    worldCreationDialog_->setVisible(true);
    menuState_ = MenuState::WORLD_CREATION;
    
    // Now explicitly re-center menus after setting visibility
    // This ensures dimensions are correct when centering
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
    // Ensure game UI elements stay hidden when showing world creation dialog
    for (const auto& element : elements_) {
        if (element.get() != mainMenu_.get() && element.get() != settingsMenu_.get() && element.get() != worldCreationDialog_.get()) {
            element->setVisible(false);
        }
    }
    
    // Double-check the position to ensure it's properly centered
    float screenWidth = getRenderer().getScreenWidth();
    float screenHeight = getRenderer().getScreenHeight();
    glm::vec2 dialogSize = worldCreationDialog_->getSize();
    worldCreationDialog_->setPosition(
        screenWidth / 2.0f - dialogSize.x / 2.0f,
        screenHeight / 2.0f - dialogSize.y / 2.0f
    );
}

void MenuSystem::closeMenus() {
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(false);
    worldCreationDialog_->setVisible(false);
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
    static const float WORLD_CREATION_WIDTH = 700.0f;
    
    // Store the heights the first time this is called
    static float mainMenuHeight = 0.0f;
    static float settingsMenuHeight = 0.0f;
    static float worldCreationHeight = 0.0f;
    
    // Store the heights the first time this is called
    if (mainMenuHeight == 0.0f && mainMenu_) {
        mainMenuHeight = mainMenu_->getSize().y;
    }
    if (settingsMenuHeight == 0.0f && settingsMenu_) {
        settingsMenuHeight = settingsMenu_->getSize().y;
    }
    if (worldCreationHeight == 0.0f && worldCreationDialog_) {
        worldCreationHeight = worldCreationDialog_->getSize().y;
    }
    
    // Suppressed repetitive logging for menu system operations.
    
    // IMPORTANT: Store references to the menus before updating renderer to avoid brief size flashes
    std::shared_ptr<BaseMenu> mainMenuRef = mainMenu_;
    std::shared_ptr<BaseMenu> settingsMenuRef = settingsMenu_;
    std::shared_ptr<BaseMenu> worldCreationRef = worldCreationDialog_;
    
    // Temporarily remove menus from the UI system's element list to avoid rendering at wrong size
    std::vector<std::shared_ptr<UIElement>> tempMenus;
    for (auto it = elements_.begin(); it != elements_.end();) {
        if ((*it).get() == mainMenu_.get() || (*it).get() == settingsMenu_.get() || (*it).get() == worldCreationDialog_.get()) {
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
    
    if (worldCreationRef) {
        worldCreationRef->setSize(WORLD_CREATION_WIDTH, worldCreationHeight);
    }
    
    // Re-add the menus to the element list
    for (auto& menu : tempMenus) {
        elements_.push_back(menu);
    }
    
    // Center the menus with the new screen dimensions
    centerMenus(width, height);
    
    // Verify menu sizes are preserved
    if (mainMenu_ && settingsMenu_ && worldCreationDialog_) {
        glm::vec2 mainSize = mainMenu_->getSize();
        glm::vec2 settingsSize = settingsMenu_->getSize();
        glm::vec2 worldCreationSize = worldCreationDialog_->getSize();
        
        DEBUG_LOG("MenuSystem", "Verification - Main menu size: " + std::to_string(static_cast<int>(mainSize.x)) + 
                  "x" + std::to_string(static_cast<int>(mainSize.y)) + 
                  ", Settings menu size: " + std::to_string(static_cast<int>(settingsSize.x)) + 
                  "x" + std::to_string(static_cast<int>(settingsSize.y)) +
                  ", World Creation dialog size: " + std::to_string(static_cast<int>(worldCreationSize.x)) +
                  "x" + std::to_string(static_cast<int>(worldCreationSize.y)));
        
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
        
        if (std::abs(worldCreationSize.x - WORLD_CREATION_WIDTH) > 0.1f ||
            std::abs(worldCreationSize.y - worldCreationHeight) > 0.1f) {
            DEBUG_LOG("MenuSystem", "WARNING: World Creation dialog size incorrect, resetting...");
            worldCreationDialog_->setSize(WORLD_CREATION_WIDTH, worldCreationHeight);
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

void MenuSystem::requestWorldInitialization() {
    if (onWorldInitRequest_) {
        onWorldInitRequest_();
    } else {
        std::cout << "[MenuSystem] No world initialization request callback set" << std::endl;
    }
}

void MenuSystem::requestMenuRecentering() {
    // Simply use the current screen dimensions from the renderer
    // We've removed this call from BaseMenu::autoResizeHeight to avoid crashes during initialization
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
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
    
    // Center world creation dialog with similar constraints
    if (worldCreationDialog_) {
        glm::vec2 worldCreationSize = worldCreationDialog_->getSize();
        
        // Default to centered position
        float idealWorldCreationX = screenWidth / 2.0f - worldCreationSize.x / 2.0f;
        float idealWorldCreationY = screenHeight / 2.0f - worldCreationSize.y / 2.0f;
        
        // Constrain to visible area with 10px margins
        float worldCreationX = std::max(10.0f, std::min(idealWorldCreationX, screenWidth - worldCreationSize.x - 10.0f));
        float worldCreationY = std::max(10.0f, std::min(idealWorldCreationY, screenHeight - worldCreationSize.y - 10.0f));
        
        // If screen is too small for the menu, prioritize showing top-left
        if (screenWidth < worldCreationSize.x + 20.0f || screenHeight < worldCreationSize.y + 20.0f) {
            worldCreationX = 10.0f; // Pin to left with margin
            worldCreationY = 10.0f; // Pin to top with margin
        }
        
        worldCreationDialog_->setPosition(worldCreationX, worldCreationY);
    }
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

glm::vec2 MenuSystem::getWorldCreationDialogSize() const {
    if (worldCreationDialog_) {
        return worldCreationDialog_->getSize();
    }
    return glm::vec2(0.0f, 0.0f);
}

} // namespace UI
} // namespace VoxelEngine
