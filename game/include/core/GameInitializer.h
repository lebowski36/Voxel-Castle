#pragma once

#include <memory>
#include <chrono>
#include <string>
#include "rendering/debug_utils.h"  // Provides Debug utility functions

class Window;
namespace flecs { class world; }
namespace VoxelCastle { namespace World { class WorldManager; class WorldGenerator; } }
namespace VoxelEngine { namespace Rendering { class TextureAtlas; class MeshBuilder; class MeshRenderer; } }
class SpectatorCamera;
// Forward declare FontManager and TextRenderer
namespace VoxelEngine { namespace Rendering { class FontManager; class TextRenderer; class DebugOverlay; } }

class GameInitializer {
public:
    struct InitResult {
        std::unique_ptr<Window> gameWindow;
        std::unique_ptr<flecs::world> ecs;
        std::unique_ptr<VoxelCastle::World::WorldManager> worldManager;
        std::unique_ptr<VoxelCastle::World::WorldGenerator> worldGenerator;
        std::unique_ptr<VoxelEngine::Rendering::TextureAtlas> textureAtlas;
        std::unique_ptr<VoxelEngine::Rendering::MeshBuilder> meshBuilder;
        std::unique_ptr<VoxelEngine::Rendering::MeshRenderer> meshRenderer;
        std::unique_ptr<SpectatorCamera> camera;
        std::unique_ptr<VoxelEngine::Rendering::FontManager> fontManager; // Added
        std::unique_ptr<VoxelEngine::Rendering::TextRenderer> textRenderer; // Added
        std::unique_ptr<VoxelEngine::Rendering::DebugOverlay> debugOverlay; // Added
        std::chrono::steady_clock::time_point lastFrameTime;
        bool isRunning;
    };

    static InitResult initialize(int screenWidth, int screenHeight, const char* projectRoot);
    static void shutdown(InitResult& resources, int screenWidth, int screenHeight, const char* projectRoot);
};
