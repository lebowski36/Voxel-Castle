#pragma once

#include <memory> // For std::unique_ptr
#include <string> // For std::string (project root)
#include <chrono> // For std::chrono::steady_clock::time_point
#include <glm/glm.hpp> // For glm::vec3

#include "../SpectatorCamera.h" // Include the full definition of SpectatorCamera
#include "core/InputManager.h"        // Input handling module
#include "core/CameraMode.h"          // Camera mode enumeration
#include "core/GameStateManager.h"    // Enhanced state management
#include "core/TimeManager.h"         // Time control system
#include "world/voxel_types.h"        // For VoxelEngine::World::VoxelType
#include "input/MouseCaptureManager.h" // Mouse capture management
#include "core/GameLoop.h"            // Game loop management
#include "ui/elements/HUD.h"
#include "ui/elements/Crosshair.h"

// Forward declaration of GameLoop
class GameLoop;

// Game state enumeration
enum class GameState {
    // Basic states
    MAIN_MENU,            // Main menu (world selection/creation)
    WORLD_SELECT,         // World selection screen
    CREATE_WORLD,         // Create new world screen
    PLAYING,              // Normal gameplay (general playing state)
    MENU,                 // Menu is open (game paused)
    
    // Control mode specific states
    FIRST_PERSON_MODE,    // First-person avatar control mode
    STRATEGIC_MODE,       // Strategic overseer mode (free camera + colony management)
    HYBRID_MODE,          // Hybrid mode allowing quick switching
    
    // Transition states
    TRANSITIONING,        // Transitioning between control modes
    LOADING,              // Loading game state or save file
    SAVING,               // Saving game state
    
    // Specialized states
    PAUSED,               // Game explicitly paused (distinct from menu)
    AUTO_SAVING           // Auto-save in progress
};

// Forward declarations to minimize include dependencies in header
class Window;
namespace flecs { class world; }
namespace VoxelCastle { namespace World { class WorldManager; } }
namespace VoxelCastle { namespace World { class WorldGenerator; } }
namespace VoxelCastle { namespace Core { class GameRenderCoordinator; } } // Render coordination
namespace VoxelCastle { namespace Core { class SaveManager; } } // Save/load management
namespace VoxelEngine { 
    namespace Rendering { 
        class TextureAtlas; 
        class MeshBuilder; 
        class MeshRenderer;
        class BlockOutlineRenderer;
        class FontManager; // Added
        class TextRenderer; // Added
        class DebugOverlay; // Forward declare DebugOverlay
    }
    namespace UI {
        class UISystem; // Forward declare UISystem
        class MenuSystem; // Forward declare MenuSystem
        class HUD; // Forward declare HUD
    }
}

// Custom comparator for glm::vec3 to be used in std::set
// Placed here to be available to the Game class and its methods if needed elsewhere
struct Vec3Comparator {
    bool operator()(const glm::vec3& a, const glm::vec3& b) const {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }
};

class Game {
public:
    Game();
    ~Game();

    // Initializes all game systems. Returns true on success, false otherwise.
    bool initialize();
    
    // Initialize world systems (called when a world is selected/created or game is resumed)
    bool initializeWorldSystems(const std::string& worldSeed = "");

    // Runs the main game loop.
    void run();

    // Shuts down all game systems and cleans up resources.
    void shutdown();

    // Check if game is running
    bool isRunning() const { return isRunning_; }
    
    // Set game running state
    void setRunning(bool running) { isRunning_ = running; }
    
    // Check if window is running
    bool isWindowRunning() const;
    
    // Check if window exists
    bool hasWindow() const;

    // --- Public Getters for Core Components and State ---
    Window* getWindow() { return gameWindow_.get(); }
    const Window* getWindow() const { return gameWindow_.get(); }

    flecs::world* getECS() { return ecs_.get(); }
    const flecs::world* getECS() const { return ecs_.get(); }

    VoxelCastle::World::WorldManager* getWorldManager() { return worldManager_.get(); }
    const VoxelCastle::World::WorldManager* getWorldManager() const { return worldManager_.get(); }

    VoxelCastle::Core::SaveManager* getSaveManager() { return saveManager_.get(); }
    const VoxelCastle::Core::SaveManager* getSaveManager() const { return saveManager_.get(); }

    VoxelEngine::Rendering::TextureAtlas* getTextureAtlas() { return textureAtlas_.get(); }
    const VoxelEngine::Rendering::TextureAtlas* getTextureAtlas() const { return textureAtlas_.get(); }

    VoxelEngine::Rendering::MeshBuilder* getMeshBuilder() { return meshBuilder_.get(); }
    const VoxelEngine::Rendering::MeshBuilder* getMeshBuilder() const { return meshBuilder_.get(); }
    
    VoxelEngine::UI::MenuSystem* getMenuSystem() { return menuSystem_.get(); }
    const VoxelEngine::UI::MenuSystem* getMenuSystem() const { return menuSystem_.get(); }
    
    SpectatorCamera* getCamera() { return camera_.get(); }
    const SpectatorCamera* getCamera() const { return camera_.get(); }

    bool isMouseCaptured() const;
    void setMouseCaptured(bool captured);
    float getMouseDeltaX() const { return mouseDeltaX_; }
    float getMouseDeltaY() const { return mouseDeltaY_; }
    void setMouseDeltaX(float delta) { mouseDeltaX_ = delta; }
    void setMouseDeltaY(float delta) { mouseDeltaY_ = delta; }

    bool isForward() const { return forward_; }
    bool isBackward() const { return backward_; }
    bool isLeft() const { return left_; }
    bool isRight() const { return right_; }
    bool isUp() const { return up_; }
    bool isDown() const { return down_; }
    float getSpeedMultiplier() const { return speedMultiplier_; }

    bool isManualVoxelChangeRequested() const { return manualVoxelChangeRequested_; }
    void setManualVoxelChangeRequested(bool v) { manualVoxelChangeRequested_ = v; }

    // Camera mode management
    CameraMode getCameraMode() const { return cameraMode_; }
    void setCameraMode(CameraMode mode) { cameraMode_ = mode; }
    void toggleCameraMode();

    // Game state management
    GameState getGameState() const;
    void setGameState(GameState state);
    bool isMenuOpen() const;
    void toggleMenu();
    
    // GameStateManager access for special state checks
    VoxelCastle::Core::GameStateManager* getStateManager() const { return stateManager_.get(); }
    
    // Enhanced state management helper functions (delegate to GameStateManager)
    bool isPlaying() const;
    bool isPaused() const;
    bool isInTransition() const;
    bool canAcceptInput() const;
    
    // Fullscreen toggle for menu system
    bool toggleFullscreen();
    bool isFullscreen() const;

    // First-person mode input state
    bool isSprinting() const { return sprinting_; }
    bool isCrouching() const { return crouching_; }
    bool isJumping() const { return jumping_; }
    
    // Block placement state accessors
    VoxelEngine::World::VoxelType getCurrentBlockType() const { return currentBlockType_; }
    void setCurrentBlockType(VoxelEngine::World::VoxelType blockType) { currentBlockType_ = blockType; }
    bool isLeftMousePressed() const { return leftMousePressed_; }
    bool isRightMousePressed() const { return rightMousePressed_; }

    // Player physics state accessors
    glm::vec3& getPlayerPosition() { return playerPosition_; }
    glm::vec3& getPlayerVelocity() { return playerVelocity_; }
    bool& isPlayerOnGround() { return isOnGround_; }
    float& getPlayerFallVelocity() { return fallVelocity_; }

    // Block action accessors
    bool hasPendingBlockAction() const { return pendingBlockAction_; }
    bool isBlockPlacement() const { return isBlockPlacement_; }
    void clearPendingBlockAction() { pendingBlockAction_ = false; }
    
    // World readiness accessors
    bool isWorldReadyForBlockOperations() const;
    void markWorldAsFullyLoaded() { isWorldFullyLoaded_ = true; }

    // Block targeting (for outline rendering)
    struct RaycastResult getTargetedBlock() const;
    VoxelEngine::Rendering::BlockOutlineRenderer* getBlockOutlineRenderer() const { 
        return blockOutlineRenderer_.get(); 
    }
    
    // Time control methods
    bool isPauseSimulation() const;
    bool togglePauseSimulation();
    void setTimeScale(VoxelCastle::Core::TimeManager::TimeScale scale);
    void increaseTimeScale();
    void decreaseTimeScale();
    float getTimeScaleFactor() const;
    std::string getTimeScaleString() const;
    VoxelCastle::Core::TimeManager* getTimeManager() const { return timeManager_.get(); }

    // Game loop methods (public for GameLoop access)
    void processInput();
    void update(float deltaTime);
    void render();

    // UI Input Handling
    void handleMenuInput(float mouseX, float mouseY, bool clicked);
    void handleMenuKeyboardInput(int key, bool pressed);

    /**
     * @brief Request the game to exit
     * This will safely shut down the game on the next update
     */
    void requestExit();

    // Save/Load operations
    /**
     * @brief Save the current game state with the given name
     * @param saveName The name for the save file
     * @return True if save was successful
     */
    bool saveGame(const std::string& saveName);

    /**
     * @brief Load a game state from the given save name
     * @param saveName The name of the save file to load
     * @return True if load was successful
     */
    bool loadGame(const std::string& saveName);

    /**
     * @brief Perform a quick save operation
     * @return True if quick save was successful
     */
    bool quickSave();

    /**
     * @brief Perform a quick load operation  
     * @return True if quick load was successful
     */
    bool quickLoad();

    // Testing functions
    void runSerializationTest();
    void runSaveManagerTest(); // T key is always replaced with the current test

private:
    // Helper method for world initialization
    void initializeWorldContent();

    // State transition handler
    void onStateChanged(GameState from, GameState to);

    // Allow InputManager to access private members for input handling
    friend void GameInput::processInput(Game& game);

    // Core game components - using unique_ptr for automatic memory management
    std::unique_ptr<Window> gameWindow_;
    std::unique_ptr<flecs::world> ecs_;
    std::unique_ptr<VoxelCastle::Core::GameStateManager> stateManager_;
    std::unique_ptr<VoxelCastle::Core::TimeManager> timeManager_;
    std::unique_ptr<VoxelCastle::World::WorldManager> worldManager_;
    std::unique_ptr<VoxelCastle::World::WorldGenerator> worldGenerator_;
    std::unique_ptr<VoxelEngine::Rendering::TextureAtlas> textureAtlas_;
    std::unique_ptr<VoxelEngine::Rendering::MeshBuilder> meshBuilder_;
    std::unique_ptr<VoxelEngine::Rendering::MeshRenderer> meshRenderer_;
    std::unique_ptr<VoxelEngine::Rendering::BlockOutlineRenderer> blockOutlineRenderer_;
    std::unique_ptr<SpectatorCamera> camera_;
    std::unique_ptr<VoxelEngine::UI::MenuSystem> menuSystem_; // Menu system for game interface
    std::shared_ptr<VoxelEngine::UI::HUD> hudSystem_; // HUD (Heads Up Display) for in-game UI elements
    std::shared_ptr<VoxelEngine::UI::Crosshair> crosshairSystem_; // Crosshair for targeting/aiming
    std::unique_ptr<VoxelEngine::Input::MouseCaptureManager> mouseCaptureManager_; // Mouse capture management
    std::unique_ptr<GameLoop> gameLoop_; // Game loop management
    std::unique_ptr<VoxelCastle::Core::GameRenderCoordinator> renderCoordinator_; // Render coordination
    std::unique_ptr<VoxelCastle::Core::SaveManager> saveManager_; // Save/load management
    
    // Game loop state
    bool isRunning_ = false;
    std::chrono::steady_clock::time_point lastFrameTime_;

    // Input state variables (migrated from main.cpp)
    bool mouseCaptured_ = true;
    float speedMultiplier_ = 1.0f;
    bool forward_ = false, backward_ = false, left_ = false, right_ = false, up_ = false, down_ = false;
    bool manualVoxelChangeRequested_ = false; // For the 'M' key test functionality
    float mouseDeltaX_ = 0.0f;
    float mouseDeltaY_ = 0.0f;

    // Camera mode and physics input state
    CameraMode cameraMode_ = CameraMode::FREE_FLYING; // Default to free-flying mode
    GameState gameState_ = GameState::STRATEGIC_MODE; // Default to strategic mode (matches FREE_FLYING camera)
    GameState previousPlayingState_ = GameState::STRATEGIC_MODE; // Store previous state when entering menu
    bool sprinting_ = false;   // Shift key for faster movement in first-person
    bool crouching_ = false;   // Ctrl key for crouching in first-person
    bool jumping_ = false;     // Space key for jumping in first-person

    // Player physics state (for first-person mode)
    glm::vec3 playerPosition_ = glm::vec3(16.0f, 24.0f, 48.0f); // Player world position
    glm::vec3 playerVelocity_ = glm::vec3(0.0f); // Player velocity
    bool isOnGround_ = false;  // Ground detection for jumping
    float fallVelocity_ = 0.0f; // For fall damage calculation

    // Block placement state
    VoxelEngine::World::VoxelType currentBlockType_ = VoxelEngine::World::VoxelType::STONE; // Currently selected block type
    bool leftMousePressed_ = false;   // Left mouse button state
    bool rightMousePressed_ = false;  // Right mouse button state
    bool pendingBlockAction_ = false; // Flag to handle block placement/removal on next frame
    bool isBlockPlacement_ = true;    // Whether action is placement (true) or removal (false)
    
    // World readiness state
    std::chrono::steady_clock::time_point worldInitTime_; // When world was initialized
    bool isWorldFullyLoaded_ = false; // Whether world is ready for block operations

    // Configuration
    const int screenWidth_ = 1280; 
    const int screenHeight_ = 720;
    const std::string projectRoot_ = "/home/system-x1/Projects/Voxel Castle/"; // Needed for paths
};
