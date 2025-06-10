#pragma once

#include "ui/UISystem.h"
#include "world/world_seed.h"
#include <memory>
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

// Forward declarations
class MainMenu;
class SettingsMenu;

} // namespace UI
} // namespace VoxelEngine



// Forward declarations for new split UIs
class WorldConfigurationUI;
class WorldSimulationUI;

namespace VoxelEngine {
namespace UI {

/**
 * @brief System for managing game menus
 * 
 * The MenuSystem extends the UISystem to provide specialized menu functionality.
 * It handles menu navigation, state, and organization of different menu screens.
 */
class MenuSystem : public UISystem {
public:
    enum class MenuState {
        NONE,              // No menu is active
        MAIN_MENU,         // Main menu is active
        SETTINGS,          // Settings menu is active
        WORLD_CREATION,    // World creation dialog is active
        WORLD_GENERATION,  // World generation UI is active (original)
        WORLD_CONFIGURATION, // World configuration UI is active (new)
        WORLD_SIMULATION   // World simulation UI is active (new)
    };

    MenuSystem();
    ~MenuSystem();

    /**
     * @brief Initialize the menu system
     * @param screenWidth Width of the screen in pixels
     * @param screenHeight Height of the screen in pixels
     * @param projectRoot Root directory of the project (for resources)
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(int screenWidth, int screenHeight, const std::string& projectRoot = "");

    /**
     * @brief Update the menu system
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime);

    /**
     * @brief Render the active menu
     */
    void render() override;

    /**
     * @brief Handle input events (mouse clicks, etc.)
     * @param mouseX Mouse X coordinate
     * @param mouseY Mouse Y coordinate  
     * @param clicked Whether mouse was clicked
     * @return true if input was handled, false otherwise
     */
    bool handleInput(float mouseX, float mouseY, bool clicked);

    /**
     * @brief Handle keyboard input events (key presses, text input, etc.)
     * @param key SDL key code or character
     * @param pressed Whether key was pressed (true) or released (false)
     * @return true if input was handled, false otherwise
     */
    bool handleKeyboardInput(int key, bool pressed);

    /**
     * @brief Handle mouse wheel input for menus
     * @param mouseX Mouse X coordinate  
     * @param mouseY Mouse Y coordinate
     * @param wheelDelta Mouse wheel delta (positive = up, negative = down)
     * @return true if input was handled
     */
    bool handleMouseWheel(float mouseX, float mouseY, float wheelDelta);

    /**
     * @brief Show the main menu
     */
    void showMainMenu();

    /**
     * @brief Show the settings menu
     */
    void showSettingsMenu();

    /**
     * @brief Show the world configuration UI (new split UI)
     */
    void showWorldConfigurationUI();

    /**
     * @brief Show the world simulation UI (new split UI)
     */
    void showWorldSimulationUI();

    /**
     * @brief Close all menus
     */
    void closeMenus();

    /**
     * @brief Toggle fullscreen mode
     * @param enable True to enable fullscreen, false to disable
     * @return True if successful, false otherwise
     */
    bool toggleFullscreen(bool enable);

    /**
     * @brief Request application exit
     */
    void requestExit();

    /**
     * @brief Request world initialization when resuming game
     */
    void requestWorldInitialization();

    /**
     * @brief Get the current menu state
     * @return The current menu state
     */
    MenuState getMenuState() const { return menuState_; }

    /**
     * @brief Check if any menu is currently active
     * @return true if a menu is active, false otherwise
     */
    bool isMenuActive() const { return menuState_ != MenuState::NONE; }

    /**
     * @brief Set the callback for when the menu is closed
     * @param callback Function to call when menu is closed
     */
    void setOnMenuClosed(std::function<void()> callback) { onMenuClosed_ = callback; }
    
    /**
     * @brief Set the callback for fullscreen toggle
     * @param callback Function to call when fullscreen is toggled
     */
    void setOnFullscreenToggle(std::function<bool(bool)> callback) { onFullscreenToggle_ = callback; }
    
    /**
     * @brief Set the callback for exit request
     * @param callback Function to call when exit is requested
     */
    void setOnExitRequest(std::function<void()> callback) { onExitRequest_ = callback; }
    
    /**
     * @brief Set the callback for requesting world initialization
     * @param callback Function to call when world initialization is requested
     */
    void setOnWorldInitRequest(std::function<void()> callback) { onWorldInitRequest_ = callback; }

    /**
     * @brief Set the callback for world creation
     * @param callback Function to call when a new world is created
     */
    void setOnWorldCreateRequest(std::function<void(const VoxelCastle::World::WorldSeed&, int)> callback) { onWorldCreateRequest_ = callback; }

    /**
     * @brief Alias for setOnFullscreenToggle for backward compatibility
     * @param callback Function to call when fullscreen is toggled
     */
    void setOnFullscreenToggled(std::function<bool(bool)> callback) { onFullscreenToggle_ = callback; }

    /**
     * @brief Update screen size and reposition menus
     * @param width New screen width
     * @param height New screen height
     */
    void updateScreenSize(int width, int height);
    
    /**
     * @brief Request that menus be re-centered (called after auto-resize)
     */
    void requestMenuRecentering();
    
    /**
     * @brief Update fullscreen checkbox state
     * @param isFullscreen Current fullscreen state
     */
    void updateFullscreenState(bool isFullscreen);

    /**
     * @brief Get the size of the main menu
     * @return Size of the main menu as a glm::vec2
     */
    glm::vec2 getMainMenuSize() const;

    /**
     * @brief Get the size of the settings menu
     * @return Size of the settings menu as a glm::vec2
     */
    glm::vec2 getSettingsMenuSize() const;

    /**
     * @brief Debug dump of menu state - to verify in console
     * Logs the current state of menu and visibility of elements
     */
    void debugDumpMenuState();

private:
    /**
     * @brief Center menus on screen based on their current sizes
     * @param screenWidth Current screen width
     * @param screenHeight Current screen height
     */
    void centerMenus(int screenWidth, int screenHeight);

    MenuState menuState_ = MenuState::NONE;
    std::shared_ptr<MainMenu> mainMenu_;
    std::shared_ptr<SettingsMenu> settingsMenu_;
    
    // New split world generation UIs
    std::shared_ptr<WorldConfigurationUI> worldConfigurationUI_;
    std::shared_ptr<WorldSimulationUI> worldSimulationUI_;
    
    std::function<void()> onMenuClosed_;
    std::function<bool(bool)> onFullscreenToggle_;
    std::function<void()> onExitRequest_;
    std::function<void()> onWorldInitRequest_;
    std::function<void(const VoxelCastle::World::WorldSeed&, int)> onWorldCreateRequest_;
};

} // namespace UI
} // namespace VoxelEngine
