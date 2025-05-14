#pragma once

#include <memory> // For std::unique_ptr
#include <string> // For std::string (project root)
#include <chrono> // For std::chrono::steady_clock::time_point
#include <glm/glm.hpp> // For glm::vec3

#include "../SpectatorCamera.h" // Include the full definition of SpectatorCamera
#include "core/InputManager.h"        // Input handling module

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
    const VoxelEngine::Rendering::MeshBuilder* getMeshBuilder() const { return meshBuilder_.get(); }

    SpectatorCamera* getCamera() { return camera_.get(); }
    const SpectatorCamera* getCamera() const { return camera_.get(); }

    bool isMouseCaptured() const { return mouseCaptured_; }
    float getMouseDeltaX() const { return mouseDeltaX_; }
    float getMouseDeltaY() const { return mouseDeltaY_; }

    bool isForward() const { return forward_; }
    bool isBackward() const { return backward_; }
    bool isLeft() const { return left_; }
    bool isRight() const { return right_; }
    bool isUp() const { return up_; }
    bool isDown() const { return down_; }
    float getSpeedMultiplier() const { return speedMultiplier_; }

    bool isManualVoxelChangeRequested() const { return manualVoxelChangeRequested_; }
    void setManualVoxelChangeRequested(bool v) { manualVoxelChangeRequested_ = v; }

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


    // Configuration
    const int screenWidth_ = 2400; 
    const int screenHeight_ = 1800;
    const std::string projectRoot_ = "/home/system-x1/Projects/Voxel Castle/"; // Needed for paths
};
