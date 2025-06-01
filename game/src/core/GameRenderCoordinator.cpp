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
    // ALWAYS use the actual window dimensions from the Window object
    // This ensures we properly handle fullscreen toggle and resizing
    int actualWidth = gameWindow.getWidth();
    int actualHeight = gameWindow.getHeight();
    
    // If dimensions don't match, this might indicate a window size change or fullscreen toggle
    if (actualWidth != screenWidth || actualHeight != screenHeight) {
        std::cout << "[GameRenderCoordinator] Dimension change detected: " 
                  << screenWidth << "x" << screenHeight << " -> "
                  << actualWidth << "x" << actualHeight << std::endl;
        
        // Force the dimensions to be correct
        screenWidth = actualWidth;
        screenHeight = actualHeight;
        
        // Update the camera aspect ratio if needed
        camera.updateAspect(static_cast<float>(actualWidth) / static_cast<float>(actualHeight));
    }
    
    // Log viewport dimensions occasionally to verify
    static int frameCounter = 0;
    if (frameCounter++ % 300 == 0) {  // Log every 5 seconds at 60fps
        std::cout << "[GameRenderCoordinator] Rendering with dimensions: " 
                  << screenWidth << "x" << screenHeight 
                  << " (Window reports: " << gameWindow.getWidth() << "x" << gameWindow.getHeight() << ")" 
                  << std::endl;
    }

    // Always set viewport explicitly before rendering anything
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Render the 3D world scene
    renderWorldScene(camera, meshRenderer, textureAtlas, gameWindow, worldManager, screenWidth, screenHeight);
    
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

    // Debug dimensions occasionally
    static int frameCounter = 0;
    if (frameCounter++ % 600 == 0) {  // Log every 10 seconds at 60fps
        std::cout << "[GameRenderCoordinator] World rendering dimensions: " << screenWidth << "x" << screenHeight << std::endl;
    }
    
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
        // Get the dimensions from both sources for comparison
        int rendererWidth = uiSystem->getRenderer().getScreenWidth();
        int rendererHeight = uiSystem->getRenderer().getScreenHeight();
        
        // Ensure the renderer has up-to-date dimensions
        // Sometimes the UISystem's internal dimensions might be outdated
        // This is particularly important after toggling fullscreen
        
        // Debug output to verify dimensions
        static int frameCounter = 0;
        if (frameCounter++ % 300 == 0) {  // Log every 5 seconds at 60fps
            std::cout << "[GameRenderCoordinator] UI Viewport dimensions: " << rendererWidth << "x" << rendererHeight << std::endl;
        }
        
        // Ensure the viewport is properly set for UI rendering
        ensureViewportForUI(rendererWidth, rendererHeight);
        
        // Now render the UI
        uiSystem->render();
    }
}

void VoxelCastle::Core::GameRenderCoordinator::presentFrame(Window& gameWindow) {
    gameWindow.render();
}
