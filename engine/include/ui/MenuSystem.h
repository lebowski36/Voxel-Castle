#pragma once

#include "ui/core/UISystem.h"
#include "world/world_seed.h"
#include <memory>
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

// Forward declarations
class MainMenu;
class CreateWorldMenu;
class SettingsMenu;

/**
 * @brief Modern menu system for Voxel Castle
 * 
 * Manages the main game menus with a clean, modern UI design.
 * Supports: Resume Game, New World, Load Game, Settings, Exit Game
 */
class MenuSystem : public UISystem {
public:
    enum class MenuState {
        NONE,              // No menu is active
        MAIN_MENU,         // Main menu is active
        SETTINGS,          // Settings menu is active
        CREATE_WORLD       // Create world menu is active
    };

    MenuSystem();
    ~MenuSystem();

    // Core initialization and rendering
    bool initialize(int screenWidth, int screenHeight, const std::string& projectRoot = "");
    void Render(); // Use correct method name (capital R to match base class)
    void update(float deltaTime);
    
    // Input handling
    bool handleInput(float mouseX, float mouseY, bool clicked);
    bool handleKeyboardInput(int key, bool pressed);
    
    // Menu navigation
    void showMainMenu();
    void showSettingsMenu();
    void showCreateWorldMenu();
    void closeMenus();
    
    // Game actions
    void resumeGame();        // Resume existing game (legacy world)
    void createNewWorld();    // Create new world (advanced generation)
    void loadGame();          // Load saved game
    void openSettings();      // Open settings menu
    void exitGame();          // Exit application
    
    // World creation methods
    void requestProceduralWorldCreation(const std::string& seed);  // Create world with procedural generation
    
    // Legacy method aliases for backward compatibility
    void requestWorldInitialization();
    void onNewGameClicked();
    void onLoadGameClicked();
    void onSettingsClicked();
    void requestExit();
    void closeAllMenus();  // Alias for closeMenus
    void closeTopMenu();
    
    // Additional callback methods
    void onQuitGameClicked();
    void onSettingsBackClicked();
    void onResolutionChanged(int width, int height);
    bool toggleFullscreen(bool enable);
    
    // Screen size management
    void setScreenSize(int width, int height);
    void updateScreenSize(int width, int height);
    
    // State queries
    MenuState getMenuState() const { return menuState_; }
    bool isMenuActive() const { return menuState_ != MenuState::NONE; }
    bool isMenuOpen() const;
    
    // Input handling
    void handleKey(int key, int scancode, int action, int mods);
    void handleChar(unsigned int codepoint);
    void handleMouseButton(int button, int action, int mods, double xpos, double ypos);
    
    // Fullscreen management
    void updateFullscreenState(bool isFullscreen);
    
    // Menu size helpers
    glm::vec2 getMainMenuSize() const;
    glm::vec2 getSettingsMenuSize() const;
    
    // Callbacks for game integration
    void setOnMenuClosed(std::function<void()> callback) { onMenuClosed_ = callback; }
    void setOnExitRequest(std::function<void()> callback) { onExitRequest_ = callback; }
    void setOnWorldInitRequest(std::function<void()> callback) { onWorldInitRequest_ = callback; }
    void setOnWorldCreateRequest(std::function<void(const VoxelCastle::World::WorldSeed&, int)> callback) { onWorldCreateRequest_ = callback; }
    void setOnFullscreenToggle(std::function<bool(bool)> callback) { onFullscreenToggle_ = callback; }

private:
    MenuState menuState_ = MenuState::NONE;
    
    // Menu components
    std::shared_ptr<MainMenu> mainMenu_;
    std::shared_ptr<SettingsMenu> settingsMenu_;
    std::shared_ptr<CreateWorldMenu> createWorldMenu_;
    
    // Callbacks for game integration
    std::function<void()> onMenuClosed_;
    std::function<bool(bool)> onFullscreenToggle_;
    std::function<void()> onExitRequest_;
    std::function<void()> onWorldInitRequest_;
    std::function<void(const VoxelCastle::World::WorldSeed&, int)> onWorldCreateRequest_;
};

} // namespace UI
} // namespace VoxelEngine
