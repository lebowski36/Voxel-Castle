#pragma once

#include <memory> // For std::unique_ptr
#include <string> // For std::string (project root)
#include <chrono> // For std::chrono::steady_clock::time_point
#include <glm/glm.hpp> // For glm::vec3

#include "../SpectatorCamera.h" // Include the full definition of SpectatorCamera
#include "core/InputManager.h"        // Input handling module
#include "core/CameraMode.h"          // Camera mode enumeration
#include "world/voxel_types.h"        // For VoxelEngine::World::VoxelType

// Forward declarations to minimize include dependencies in header
class Window;
namespace flecs { class world; }
namespace VoxelCastle { namespace World { class WorldManager; } }
namespace VoxelCastle { namespace World { class WorldGenerator; } }
namespace VoxelEngine { 
    namespace Rendering { 
        class TextureAtlas; 
        class MeshBuilder; 
        class MeshRenderer;
        class FontManager; // Added
        class TextRenderer; // Added
        class DebugOverlay; // Forward declare DebugOverlay
    }
    namespace UI {
        class UISystem; // Forward declare UISystem
        class BlockSelectionUI; // Forward declare BlockSelectionUI
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

    // Runs the main game loop.
    void run();

    // Shuts down all game systems and cleans up resources.
    void shutdown();

    // --- Public Getters for Core Components and State ---
    Window* getWindow() { return gameWindow_.get(); }
    const Window* getWindow() const { return gameWindow_.get(); }

    flecs::world* getECS() { return ecs_.get(); }
    const flecs::world* getECS() const { return ecs_.get(); }

    VoxelCastle::World::WorldManager* getWorldManager() { return worldManager_.get(); }
    const VoxelCastle::World::WorldManager* getWorldManager() const { return worldManager_.get(); }

    VoxelEngine::Rendering::TextureAtlas* getTextureAtlas() { return textureAtlas_.get(); }
    const VoxelEngine::Rendering::TextureAtlas* getTextureAtlas() const { return textureAtlas_.get(); }

    VoxelEngine::Rendering::MeshBuilder* getMeshBuilder() { return meshBuilder_.get(); }
    const VoxelEngine::Rendering::MeshBuilder* getMeshBuilder() const { return meshBuilder_.get(); }    SpectatorCamera* getCamera() { return camera_.get(); }
    const SpectatorCamera* getCamera() const { return camera_.get(); }

    bool isMouseCaptured() const { return mouseCaptured_; }
    void setMouseCaptured(bool captured) { mouseCaptured_ = captured; }
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

private:
    // Helper method for world initialization
    void initializeWorldContent();

    // Delegate input processing to InputManager
    void processInput();
    // Allow InputManager to access private members for input handling
    friend void GameInput::processInput(Game& game);

    // Updates game state.
    void update(float deltaTime);

    // Renders the game world.
    void render();

    // Core game components - using unique_ptr for automatic memory management
    std::unique_ptr<Window> gameWindow_;
    std::unique_ptr<flecs::world> ecs_;
    std::unique_ptr<VoxelCastle::World::WorldManager> worldManager_;
    std::unique_ptr<VoxelCastle::World::WorldGenerator> worldGenerator_;
    std::unique_ptr<VoxelEngine::Rendering::TextureAtlas> textureAtlas_;
    std::unique_ptr<VoxelEngine::Rendering::MeshBuilder> meshBuilder_;
    std::unique_ptr<VoxelEngine::Rendering::MeshRenderer> meshRenderer_;
    std::unique_ptr<SpectatorCamera> camera_;
    std::unique_ptr<VoxelEngine::UI::UISystem> uiSystem_; // UI system for game interface
    std::shared_ptr<VoxelEngine::UI::BlockSelectionUI> blockSelectionUI_; // Block selection UI element
    
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
    const int screenWidth_ = 2400; 
    const int screenHeight_ = 1800;
    const std::string projectRoot_ = "/home/system-x1/Projects/Voxel Castle/"; // Needed for paths
};
