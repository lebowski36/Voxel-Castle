#include "../../include/core/GameRenderCoordinator.h"
#include "../../include/core/game.h"
#include "../../include/SpectatorCamera.h"
#include "rendering/mesh_renderer.h"
#include "rendering/block_outline_renderer.h"
#include "interaction/BlockPlacement.h"
#include "ui/UISystem.h"
#include "world/world_manager.h"
#include <GL/gl.h>
#include "core/GameRenderer.h"
#include "platform/Window.h"

void VoxelCastle::Core::GameRenderCoordinator::render(
    Game& game,
    SpectatorCamera& camera,
    VoxelEngine::Rendering::MeshRenderer& meshRenderer,
    VoxelEngine::Rendering::TextureAtlas& textureAtlas,
    Window& gameWindow,
    VoxelCastle::World::WorldManager& worldManager,
    VoxelEngine::UI::UISystem* uiSystem,
    int screenWidth,
    int screenHeight
) {
    // ALWAYS use the actual window dimensions from the Window object
    // This ensures we properly handle fullscreen toggle and resizing
    int actualWidth = gameWindow.getWidth();
    int actualHeight = gameWindow.getHeight();
    
    // If dimensions don't match, this might indicate a window size change or fullscreen toggle
    if (actualWidth != screenWidth || actualHeight != screenHeight) {
        // Force the dimensions to be correct
        screenWidth = actualWidth;
        screenHeight = actualHeight;
        
        // Update the camera aspect ratio if needed
        camera.updateAspect(static_cast<float>(actualWidth) / static_cast<float>(actualHeight));
    }
    
    // Always set viewport explicitly before rendering anything
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Render the 3D world scene
    renderWorldScene(game, camera, meshRenderer, textureAtlas, gameWindow, worldManager, screenWidth, screenHeight);
    
    // If UI system exists, update its dimensions to match current window size before rendering
    if (uiSystem) {
        uiSystem->setScreenSize(screenWidth, screenHeight);
    }
    
    // Render the UI overlay
    renderUIOverlay(uiSystem);
    
    // Present the frame
    presentFrame(gameWindow);
}

void VoxelCastle::Core::GameRenderCoordinator::renderWorldScene(
    Game& game,
    SpectatorCamera& camera,
    VoxelEngine::Rendering::MeshRenderer& meshRenderer,
    VoxelEngine::Rendering::TextureAtlas& textureAtlas,
    Window& gameWindow,
    VoxelCastle::World::WorldManager& worldManager,
    int screenWidth,
    int screenHeight
) {
    // Explicitly set viewport for 3D rendering
    glViewport(0, 0, screenWidth, screenHeight);

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

    // Render block outline if we have a targeted block
    auto targetedBlock = game.getTargetedBlock();
    auto* outlineRenderer = game.getBlockOutlineRenderer();
    if (targetedBlock.hit && outlineRenderer && outlineRenderer->isReady()) {
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        outlineRenderer->renderOutline(targetedBlock.blockPosition, view, projection);
    }
}

void VoxelCastle::Core::GameRenderCoordinator::ensureViewportForUI(int screenWidth, int screenHeight) {
    // Set viewport for UI rendering
    glViewport(0, 0, screenWidth, screenHeight);
}

void VoxelCastle::Core::GameRenderCoordinator::renderUIOverlay(VoxelEngine::UI::UISystem* uiSystem) {
    if (uiSystem) {
        // Get the dimensions from both sources for comparison
        int rendererWidth = uiSystem->getRenderer().getScreenWidth();
        int rendererHeight = uiSystem->getRenderer().getScreenHeight();
        
        // Ensure the renderer has up-to-date dimensions
        // Sometimes the UISystem's internal dimensions might be outdated
        // This is particularly important after toggling fullscreen
        
        // Ensure the viewport is properly set for UI rendering
        ensureViewportForUI(rendererWidth, rendererHeight);
        
        // Now render the UI
        uiSystem->render();
    }
}

void VoxelCastle::Core::GameRenderCoordinator::presentFrame(Window& gameWindow) {
    gameWindow.render();
}
