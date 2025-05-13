#pragma once

#include <memory> // For std::unique_ptr
#include <string> // For std::string (project root)
#include <chrono> // For std::chrono::steady_clock::time_point

// Forward declarations to minimize include dependencies in header
class Window;
namespace flecs { class world; }
namespace VoxelCastle { namespace World { class WorldManager; } }
namespace VoxelEngine { 
    namespace Rendering { 
        class TextureAtlas; 
        class MeshBuilder; 
        class MeshRenderer;
    }
}
class SpectatorCamera;

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

private:
    // Processes user input.
    void processInput();

    // Updates game state.
    void update(float deltaTime);

    // Renders the game world.
    void render();

    // Core game components - using unique_ptr for automatic memory management
    std::unique_ptr<Window> gameWindow_;
    std::unique_ptr<flecs::world> ecs_;
    std::unique_ptr<VoxelCastle::World::WorldManager> worldManager_;
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
