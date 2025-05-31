#include "../../include/core/GameRenderCoordinator.h"
#include "../../include/SpectatorCamera.h"
#include "rendering/mesh_renderer.h"
#include "ui/UISystem.h"
#include "world/world_manager.h"
#include <GL/gl.h>
#include "core/GameRenderer.h"
#include "platform/Window.h"

void VoxelCastle::Core::GameRenderCoordinator::render(
    SpectatorCamera& camera,
    VoxelEngine::Rendering::MeshRenderer& meshRenderer,
    VoxelEngine::Rendering::TextureAtlas& textureAtlas,
    Window& gameWindow,
    VoxelCastle::World::WorldManager& worldManager,
    VoxelEngine::UI::UISystem* uiSystem,
    int screenWidth,
    int screenHeight
) {
    // Render the 3D world scene
    renderWorldScene(camera, meshRenderer, textureAtlas, gameWindow, worldManager, screenWidth, screenHeight);
    
    // Render the UI overlay
    renderUIOverlay(uiSystem);
    
    // Present the frame
    presentFrame(gameWindow);
}

void VoxelCastle::Core::GameRenderCoordinator::renderWorldScene(
    SpectatorCamera& camera,
    VoxelEngine::Rendering::MeshRenderer& meshRenderer,
    VoxelEngine::Rendering::TextureAtlas& textureAtlas,
    Window& gameWindow,
    VoxelCastle::World::WorldManager& worldManager,
    int screenWidth,
    int screenHeight
) {
    // Delegate to the existing GameRenderer
    GameRenderer::renderGame(
        camera,
        meshRenderer,
        textureAtlas,
        gameWindow,
        worldManager.getAllSegmentMeshes(),
        screenWidth,
        screenHeight
    );
}

void VoxelCastle::Core::GameRenderCoordinator::ensureViewportForUI(int screenWidth, int screenHeight) {
    // Set viewport for UI rendering
    glViewport(0, 0, screenWidth, screenHeight);
}

void VoxelCastle::Core::GameRenderCoordinator::renderUIOverlay(VoxelEngine::UI::UISystem* uiSystem) {
    if (uiSystem) {
        uiSystem->render();
    }
}

void VoxelCastle::Core::GameRenderCoordinator::presentFrame(Window& gameWindow) {
    gameWindow.render();
}
