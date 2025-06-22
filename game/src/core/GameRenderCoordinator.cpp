#include "../../include/core/GameRenderCoordinator.h"
#include "../../include/core/game.h"
#include "../../include/SpectatorCamera.h"
#include "rendering/mesh_renderer.h"
#include "rendering/block_outline_renderer.h"
#include "rendering/texture_atlas.h"
#include "interaction/BlockPlacement.h"
#include "ui/core/UISystem.h"
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
    
    // First, update the window's internal size tracking to match actual SDL window
    gameWindow.updateWindowSize();
    
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
    
    // CRITICAL FIX: Only render 3D world scene when we're actually in the game, not in menus
    // Check if we're in a menu state where 3D world rendering should be skipped
    bool shouldRender3DWorld = game.shouldRender3DWorld();
    
    if (shouldRender3DWorld) {
        // Render the 3D world scene
        renderWorldScene(game, camera, meshRenderer, textureAtlas, gameWindow, worldManager, screenWidth, screenHeight);
    } else {
        // When in menu states, just clear the screen with a dark background
        // This prevents the 3D renderer from clearing UI elements drawn in the same frame
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Dark blue-gray background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
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
        
        // Render only the face being looked at instead of the whole block
        outlineRenderer->renderFaceHighlight(targetedBlock.blockPosition, 
                                            targetedBlock.normal, 
                                            view, 
                                            projection);
        
        // Render placement preview (ghost block) at the adjacent position
        // Always show preview when targeting a valid block (for better UX)
        
        // Get the current block type and its texture coordinates
        auto currentBlockType = game.getCurrentBlockType();
        auto textureCoords = textureAtlas.getTextureCoordinates(currentBlockType);
        
        // Get the texture atlas ID from the mesh renderer
        GLuint textureAtlasID = meshRenderer.getTextureAtlasID();
        
        // Calculate tile UV span (same for all tiles)
        glm::vec2 tileUVSpan(VoxelEngine::Rendering::TILE_UV_WIDTH, 
                           VoxelEngine::Rendering::TILE_UV_HEIGHT);
        
        float previewAlpha = 0.3f; // Semi-transparent
        
        outlineRenderer->renderBlockPreview(targetedBlock.adjacentPosition,
                                           view,
                                           projection,
                                           textureAtlasID,
                                           textureCoords.getBottomLeft(),
                                           tileUVSpan,
                                           previewAlpha);
    }
}

void VoxelCastle::Core::GameRenderCoordinator::ensureViewportForUI(int screenWidth, int screenHeight) {
    // Set viewport for UI rendering
    glViewport(0, 0, screenWidth, screenHeight);
}

void VoxelCastle::Core::GameRenderCoordinator::renderUIOverlay(VoxelEngine::UI::UISystem* uiSystem) {
    if (!uiSystem) {
        return;
    }
    
    try {
        // Get the renderer pointer (using new UI system API)
        auto* renderer = uiSystem->GetRenderer();
        if (!renderer) {
            std::cerr << "[ERROR] UISystem renderer is null" << std::endl;
            return;
        }
        
        // Get the dimensions from the renderer
        int rendererWidth = renderer->getScreenWidth();
        int rendererHeight = renderer->getScreenHeight();
        
        // Ensure the viewport is properly set for UI rendering
        ensureViewportForUI(rendererWidth, rendererHeight);
        
        // Now render the UI
        uiSystem->Render();
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in renderUIOverlay: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "[ERROR] Unknown exception in renderUIOverlay" << std::endl;
    }
}

void VoxelCastle::Core::GameRenderCoordinator::presentFrame(Window& gameWindow) {
    gameWindow.render();
}
