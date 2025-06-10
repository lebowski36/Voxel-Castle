#include "ui/MenuSystem.h"
#include "ui/elements/MainMenu.h"
#include "ui/elements/SettingsMenu.h" 
#include "ui/WorldConfigurationUI.h"
#include "ui/WorldSimulationUI.h"
#include "world/world_seed.h"
#include <iostream>
#include <typeinfo>  // For typeid
#include <algorithm> // For std::max
#include "utils/debug_logger_stub.h"

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

    // Create new split world generation UIs
    worldConfigurationUI_ = std::make_shared<WorldConfigurationUI>(renderer);
    if (!worldConfigurationUI_->initialize(getRenderer().getScreenWidth(), getRenderer().getScreenHeight())) {
        std::cerr << "[MenuSystem] Failed to initialize world configuration UI" << std::endl;
        return false;
    }
    
    worldSimulationUI_ = std::make_shared<WorldSimulationUI>(renderer);
    if (!worldSimulationUI_->initialize(getRenderer().getScreenWidth(), getRenderer().getScreenHeight())) {
        std::cerr << "[MenuSystem] Failed to initialize world simulation UI" << std::endl;
        return false;
    }
    
    // Set up new split world generation UI callbacks
    worldConfigurationUI_->setOnConfigurationCompleteCallback([this](const WorldConfigurationUI::WorldConfig& config) {
        std::cout << "[MenuSystem] World configuration complete, switching to simulation" << std::endl;
        
        // Convert WorldConfigurationUI::WorldConfig to WorldSimulationUI::WorldConfig
        WorldSimulationUI::WorldConfig simConfig;
        simConfig.worldName = config.worldName;
        simConfig.worldSize = config.worldSize;
        simConfig.simulationDepth = config.simulationDepth;
        simConfig.climateType = config.climateType;
        simConfig.geologicalActivity = config.geologicalActivity;
        simConfig.hydrologyLevel = config.hydrologyLevel;
        simConfig.customSeed = config.customSeed;
        simConfig.enableCivilizations = config.enableCivilizations;
        simConfig.geologicalQuality = config.geologicalQuality;
        
        // Switch to simulation UI and start the simulation
        showWorldSimulationUI();
        worldSimulationUI_->startSimulation(simConfig, config.worldName);
    });
    
    worldConfigurationUI_->setOnBackCallback([this]() {
        std::cout << "[MenuSystem] World configuration cancelled, returning to main menu" << std::endl;
        showMainMenu();
    });
    
    worldSimulationUI_->setOnSimulationCompleteCallback([this](const WorldSimulationUI::WorldStats& stats) {
        std::cout << "[MenuSystem] World simulation complete!" << std::endl;
        std::cout << "[MenuSystem] Generated world statistics:" << std::endl;
        std::cout << "  - Highest Peak: " << stats.highestPeak << "m (" << stats.highestPeakName << ")" << std::endl;
        std::cout << "  - Deepest Valley: " << stats.deepestValley << "m (" << stats.deepestValleyName << ")" << std::endl;
        std::cout << "  - Largest Lake: " << stats.largestLakeSize << " (" << stats.largestLakeName << ")" << std::endl;
        std::cout << "  - Simulation Years: " << stats.simulationYears << std::endl;
        
        // TODO: Instead of calling legacy world creation, we should:
        // 1. Save the SeedWorldGenerator state from the simulation
        // 2. Pass it to the game system for immediate use
        // 3. Skip the legacy WorldGenerator entirely
        
        // For now, we'll stay in the simulation UI so the user can see completion
        // This prevents the GameState stack errors and OpenGL corruption
        std::cout << "[MenuSystem] World generation complete - simulation UI remains active" << std::endl;
        std::cout << "[MenuSystem] TODO: Add 'Play World' button to transition to gameplay with generated world" << std::endl;
        
        // NOTE: Do NOT call closeMenus() here - it causes:
        // 1. GameState stack underflow ("Cannot pop state: stack is empty")
        // 2. Mouse capture issues (cursor disappears)
        // 3. OpenGL state corruption (error 0x502 spam)
        
        // The user can use the "Back to Menu" button in WorldSimulationUI to navigate
        
        // NOTE: Commented out the problematic calls
        // closeMenus(); // This was causing GameState stack errors!
        
        // TODO: Convert simulation results to WorldSeed and trigger NEW world creation (not legacy)
        // VoxelCastle::World::WorldSeed seed; // Create with current timestamp  
        // if (onWorldCreateRequest_) {
        //     onWorldCreateRequest_(seed, 1); // This was calling the legacy system!
        // }
    });
    
    worldSimulationUI_->setOnBackCallback([this]() {
        std::cout << "[MenuSystem] World simulation cancelled, returning to configuration" << std::endl;
        showWorldConfigurationUI();
    });
    
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
    worldConfigurationUI_->setVisible(false);
    worldSimulationUI_->setVisible(false);

    // Add menus to UI system
    addElement(mainMenu_);
    addElement(settingsMenu_);
    addElement(worldConfigurationUI_);
    addElement(worldSimulationUI_);
    
    return true;
}

void MenuSystem::update(float deltaTime) {
    UISystem::update(deltaTime);
    
    // Update new split UIs if they're active
    if (menuState_ == MenuState::WORLD_CONFIGURATION && worldConfigurationUI_) {
        // WorldConfigurationUI is a UIElement, so it's updated by UISystem::update()
    }
    
    if (menuState_ == MenuState::WORLD_SIMULATION && worldSimulationUI_) {
        // WorldSimulationUI is a UIElement, so it's updated by UISystem::update()
        // But we can add simulation-specific update logic here if needed
    }
}

void MenuSystem::render() {
    // Logging disabled to reduce console spam
    if (menuState_ == MenuState::WORLD_CONFIGURATION || menuState_ == MenuState::WORLD_SIMULATION) {
        // New split UIs are UIElements, so they're rendered by UISystem::render()
        UISystem::render();
    } else {
        // Render normal UI system (menus)
        UISystem::render();
    }
}

bool MenuSystem::handleInput(float mouseX, float mouseY, bool clicked) {
    // Use default UISystem input handling for all menus (including new split UIs)
    // New split UIs are UIElements, so they're handled by UISystem::handleInput()
    return UISystem::handleInput(mouseX, mouseY, clicked);
}

bool MenuSystem::handleKeyboardInput(int key, bool pressed) {
    // Route keyboard input to appropriate UI components based on current menu state
    if (menuState_ == MenuState::WORLD_CONFIGURATION && worldConfigurationUI_) {
        // Route keyboard input to WorldConfigurationUI when it's active
        return worldConfigurationUI_->handleKeyboardInput(key, pressed);
    }
    
    // Other menu states don't currently need keyboard input
    // (Main menu and Settings menu use mouse-only interaction)
    return false;
}

bool MenuSystem::handleMouseWheel(float mouseX, float mouseY, float wheelDelta) {
    // Route mouse wheel events to the appropriate active menu
    switch (menuState_) {
        case MenuState::WORLD_SIMULATION:
            if (worldSimulationUI_ && worldSimulationUI_->isVisible()) {
                return worldSimulationUI_->handleExtendedInput(mouseX, mouseY, false, wheelDelta);
            }
            break;
        case MenuState::WORLD_CONFIGURATION:
            // World configuration UI doesn't need mouse wheel for now
            break;
        case MenuState::MAIN_MENU:
        case MenuState::SETTINGS:
        case MenuState::NONE:
        default:
            // Other menus don't handle mouse wheel events
            break;
    }
    return false;
}

void MenuSystem::showMainMenu() {
    // First make the menu visible to ensure its size is calculated correctly
    mainMenu_->setVisible(true);
    settingsMenu_->setVisible(false);
    menuState_ = MenuState::MAIN_MENU;
    
    // Now explicitly re-center menus after setting visibility
    // This ensures dimensions are correct when centering
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
    // Ensure the block selection UI stays hidden during menu display
    // Block selection UI is identified by checking if it's not one of our menu elements
    for (const auto& element : elements_) {
        if (element.get() != mainMenu_.get() && element.get() != settingsMenu_.get()) {
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
    menuState_ = MenuState::SETTINGS;
    
    // Now explicitly re-center menus after setting visibility
    // This ensures dimensions are correct when centering
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
    // Ensure game UI elements stay hidden when switching to settings
    for (const auto& element : elements_) {
        if (element.get() != mainMenu_.get() && element.get() != settingsMenu_.get()) {
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

void MenuSystem::showWorldConfigurationUI() {
    std::cout << "[MenuSystem] showWorldConfigurationUI() called" << std::endl;
    
    // Hide all other menus
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(false);
    worldSimulationUI_->setVisible(false);
    
    // Show world configuration UI
    worldConfigurationUI_->setVisible(true);
    menuState_ = MenuState::WORLD_CONFIGURATION;
    
    // Hide all other UI elements
    for (const auto& element : elements_) {
        if (element.get() != worldConfigurationUI_.get()) {
            element->setVisible(false);
        }
    }
    
    // Center the configuration UI
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
    std::cout << "[MenuSystem] WorldConfigurationUI is now active" << std::endl;
}

void MenuSystem::showWorldSimulationUI() {
    std::cout << "[MenuSystem] showWorldSimulationUI() called" << std::endl;
    
    // Hide all other menus
    mainMenu_->setVisible(false);
    settingsMenu_->setVisible(false);
    worldConfigurationUI_->setVisible(false);
    
    // Show world simulation UI
    worldSimulationUI_->setVisible(true);
    menuState_ = MenuState::WORLD_SIMULATION;
    
    // Hide all other UI elements  
    for (const auto& element : elements_) {
        if (element.get() != worldSimulationUI_.get()) {
            element->setVisible(false);
        }
    }
    
    // Center the simulation UI
    centerMenus(getRenderer().getScreenWidth(), getRenderer().getScreenHeight());
    
    std::cout << "[MenuSystem] WorldSimulationUI is now active" << std::endl;
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
                  "x" + std::to_string(static_cast<int>(mainSize.y)) + 
                  ", Settings menu size: " + std::to_string(static_cast<int>(settingsSize.x)) + 
                  "x" + std::to_string(static_cast<int>(settingsSize.y)));
        
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
