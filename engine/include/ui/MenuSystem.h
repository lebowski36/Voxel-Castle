#pragma once

#include "ui/UISystem.h"
#include <memory>
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

// Forward declarations
class MainMenu;
class SettingsMenu;

/**
 * @brief System for managing game menus
 * 
 * The MenuSystem extends the UISystem to provide specialized menu functionality.
 * It handles menu navigation, state, and organization of different menu screens.
 */
class MenuSystem : public UISystem {
public:
    enum class MenuState {
        NONE,       // No menu is active
        MAIN_MENU,  // Main menu is active
        SETTINGS    // Settings menu is active
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
    void render();

    /**
     * @brief Show the main menu
     */
    void showMainMenu();

    /**
     * @brief Show the settings menu
     */
    void showSettingsMenu();

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
     * @brief Update fullscreen checkbox state
     * @param isFullscreen Current fullscreen state
     */
    void updateFullscreenState(bool isFullscreen);

    /**
     * @brief Debug dump of menu state - to verify in console
     * Logs the current state of menu and visibility of elements
     */
    void debugDumpMenuState();

private:
    MenuState menuState_ = MenuState::NONE;
    std::shared_ptr<MainMenu> mainMenu_;
    std::shared_ptr<SettingsMenu> settingsMenu_;
    std::function<void()> onMenuClosed_;
    std::function<bool(bool)> onFullscreenToggle_;
    std::function<void()> onExitRequest_;
};

} // namespace UI
} // namespace VoxelEngine
