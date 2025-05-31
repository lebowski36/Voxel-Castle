#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <memory>

// Forward declarations
class SpectatorCamera; // Global namespace
class Window; // Global namespace

namespace VoxelEngine::Rendering {
    class MeshRenderer;
    class TextureAtlas;
}

namespace VoxelCastle::World {
    class WorldManager;
}

namespace VoxelEngine::UI {
    class UISystem;
}

namespace VoxelCastle::Core {

/**
 * @brief Coordinates the rendering pipeline for the game
 * 
 * This class manages the complete rendering pipeline including:
 * - Scene rendering coordination
 * - OpenGL state management
 * - Viewport management
 * - UI rendering coordination
 * - Buffer swapping
 */
class GameRenderCoordinator {
public:
    GameRenderCoordinator() = default;
    ~GameRenderCoordinator() = default;

    /**
     * @brief Execute the complete rendering pipeline
     * 
     * @param camera The camera for view/projection matrices
     * @param meshRenderer The mesh rendering system
     * @param textureAtlas The texture atlas for materials
     * @param gameWindow The game window for buffer swapping
     * @param worldManager The world manager for segment meshes
     * @param uiSystem The UI system for overlay rendering
     * @param screenWidth Current screen width
     * @param screenHeight Current screen height
     */
    void render(
        SpectatorCamera& camera,
        VoxelEngine::Rendering::MeshRenderer& meshRenderer,
        VoxelEngine::Rendering::TextureAtlas& textureAtlas,
        Window& gameWindow,
        VoxelCastle::World::WorldManager& worldManager,
        VoxelEngine::UI::UISystem* uiSystem,
        int screenWidth,
        int screenHeight
    );

private:
    /**
     * @brief Clear color and depth buffers
     */
    void clearBuffers();

    /**
     * @brief Render the 3D world scene
     */
    void renderWorldScene(
        SpectatorCamera& camera,
        VoxelEngine::Rendering::MeshRenderer& meshRenderer,
        VoxelEngine::Rendering::TextureAtlas& textureAtlas,
        Window& gameWindow,
        VoxelCastle::World::WorldManager& worldManager,
        int screenWidth,
        int screenHeight
    );

    /**
     * @brief Ensure viewport is correctly set for UI rendering
     */
    void ensureViewportForUI(int screenWidth, int screenHeight);

    /**
     * @brief Render the UI overlay
     */
    void renderUIOverlay(VoxelEngine::UI::UISystem* uiSystem);

    /**
     * @brief Swap buffers to present the frame
     */
    void presentFrame(Window& gameWindow);
};

} // namespace VoxelCastle::Core
