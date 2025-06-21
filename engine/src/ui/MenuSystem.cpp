#include "ui/MenuSystem.h"
#include "ui/menus/MainMenu.h"
#include "ui/menus/CreateWorldMenu.h"
// #include "ui/menus/SettingsMenu.h" // TODO: Create SettingsMenu
#include <iostream>

namespace VoxelEngine {
namespace UI {

MenuSystem::MenuSystem() : UISystem() {
    std::cout << "[MenuSystem] Creating minimal MenuSystem stub" << std::endl;
}

MenuSystem::~MenuSystem() = default;

bool MenuSystem::initialize(int screenWidth, int screenHeight, const std::string& projectRoot) {
    std::cout << "[MenuSystem] Initializing minimal stub" << std::endl;
    
    if (!UISystem::Initialize(screenWidth, screenHeight, projectRoot)) {
        return false;
    }
    
    // Create new UI menus
    mainMenu_ = std::make_shared<MainMenu>();
    mainMenu_->Initialize();
    
    createWorldMenu_ = std::make_shared<CreateWorldMenu>();
    createWorldMenu_->Initialize();
    
    // TODO: Create SettingsMenu when implemented
    // settingsMenu_ = std::make_shared<SettingsMenu>();
    
    return true;
}

bool MenuSystem::handleInput(float mouseX, float mouseY, bool clicked) {
    // Basic input forwarding to UISystem
    if (clicked) {
        HandleMouseButton(0, 1, 0, mouseX, mouseY);
    }
    return false;
}

void MenuSystem::Render() {
    // Use UISystem's render method
    UISystem::Render();
}

void MenuSystem::update(float deltaTime) {
    // Use UISystem's update method
    UISystem::Update(deltaTime);
}

// Menu navigation methods - minimal stubs
void MenuSystem::showMainMenu() {
    std::cout << "[MenuSystem] showMainMenu() called" << std::endl;
    menuState_ = MenuState::MAIN_MENU;
}

void MenuSystem::showSettingsMenu() {
    std::cout << "[MenuSystem] showSettingsMenu() called" << std::endl;
    menuState_ = MenuState::SETTINGS;
}

void MenuSystem::showCreateWorldMenu() {
    std::cout << "[MenuSystem] showCreateWorldMenu() called" << std::endl;
    menuState_ = MenuState::CREATE_WORLD;
}

void MenuSystem::closeMenus() {
    std::cout << "[MenuSystem] closeMenus() called" << std::endl;
    menuState_ = MenuState::NONE;
}

void MenuSystem::closeAllMenus() {
    closeMenus();
}

void MenuSystem::closeTopMenu() {
    closeMenus();
}

bool MenuSystem::isMenuOpen() const {
    return menuState_ != MenuState::NONE;
}

void MenuSystem::setScreenSize(int width, int height) {
    SetScreenSize(width, height);
}

// Callback methods - basic stubs
void MenuSystem::onNewGameClicked() {
    std::cout << "[MenuSystem] onNewGameClicked()" << std::endl;
    showCreateWorldMenu();
}

void MenuSystem::onLoadGameClicked() {
    std::cout << "[MenuSystem] onLoadGameClicked()" << std::endl;
}

void MenuSystem::onSettingsClicked() {
    std::cout << "[MenuSystem] onSettingsClicked()" << std::endl;
    showSettingsMenu();
}

void MenuSystem::onQuitGameClicked() {
    std::cout << "[MenuSystem] onQuitGameClicked()" << std::endl;
    requestExit();
}

void MenuSystem::onSettingsBackClicked() {
    std::cout << "[MenuSystem] onSettingsBackClicked()" << std::endl;
    showMainMenu();
}

void MenuSystem::onResolutionChanged(int width, int height) {
    std::cout << "[MenuSystem] onResolutionChanged(" << width << ", " << height << ")" << std::endl;
    setScreenSize(width, height);
}

bool MenuSystem::toggleFullscreen(bool enable) {
    std::cout << "[MenuSystem] toggleFullscreen(" << enable << ")" << std::endl;
    return true; // Stub implementation
}

void MenuSystem::requestExit() {
    std::cout << "[MenuSystem] Exit requested!" << std::endl;
    // TODO: Implement actual exit logic
}

void MenuSystem::requestWorldInitialization() {
    std::cout << "[MenuSystem] World initialization requested!" << std::endl;
    // TODO: Implement world initialization
}

void MenuSystem::requestProceduralWorldCreation(const std::string& seed) {
    std::cout << "[MenuSystem] requestProceduralWorldCreation() called with seed: '" << seed << "'" << std::endl;
    
    // Create world seed object
    VoxelCastle::World::WorldSeed worldSeed;
    worldSeed.setSeedFromString(seed);
    
    // Use the advanced world creation callback
    if (onWorldCreateRequest_) {
        onWorldCreateRequest_(worldSeed, 0); // 0 = default world size
    }
    
    // Close menus after world creation request
    closeMenus();
}

// Input handling stubs
void MenuSystem::handleKey(int key, int scancode, int action, int mods) {
    HandleKey(key, scancode, action, mods);
}

void MenuSystem::handleChar(unsigned int codepoint) {
    HandleChar(codepoint);
}

void MenuSystem::handleMouseButton(int button, int action, int mods, double xpos, double ypos) {
    HandleMouseButton(button, action, mods, xpos, ypos);
}

void MenuSystem::updateFullscreenState(bool isFullscreen) {
    std::cout << "[MenuSystem] updateFullscreenState(" << isFullscreen << ")" << std::endl;
    // TODO: Update fullscreen checkbox state in settings menu when implemented
}

glm::vec2 MenuSystem::getMainMenuSize() const {
    std::cout << "[MenuSystem] getMainMenuSize() called" << std::endl;
    // Return default size for now
    return glm::vec2(400.0f, 600.0f);
}

glm::vec2 MenuSystem::getSettingsMenuSize() const {
    std::cout << "[MenuSystem] getSettingsMenuSize() called" << std::endl;
    // Return default size for now
    return glm::vec2(500.0f, 700.0f);
}

void MenuSystem::updateScreenSize(int width, int height) {
    std::cout << "[MenuSystem] updateScreenSize(" << width << ", " << height << ")" << std::endl;
    setScreenSize(width, height);
    // TODO: Update menu positioning when menus are properly implemented
}

// New game action methods
void MenuSystem::resumeGame() {
    std::cout << "[MenuSystem] resumeGame() called" << std::endl;
    requestWorldInitialization();
}

void MenuSystem::createNewWorld() {
    std::cout << "[MenuSystem] createNewWorld() called" << std::endl;
    showCreateWorldMenu();
}

void MenuSystem::loadGame() {
    std::cout << "[MenuSystem] loadGame() called" << std::endl;
    // TODO: Implement load game functionality
}

void MenuSystem::openSettings() {
    std::cout << "[MenuSystem] openSettings() called" << std::endl;
    showSettingsMenu();
}

void MenuSystem::exitGame() {
    std::cout << "[MenuSystem] exitGame() called" << std::endl;
    requestExit();
}

} // namespace UI
} // namespace VoxelEngine
