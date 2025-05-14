#pragma once

#include <vector>
#include <string>

// Forward declarations
namespace VoxelEngine {
    namespace Rendering {
        class VoxelMesh;
        class MeshRenderer;
        class TextureAtlas;
    }
}
class SpectatorCamera;
class Window; // Assuming gameWindow_ is of type Window or similar

namespace GameRenderer {

/**
 * @brief Handles the main rendering loop for the game.
 * 
 * This includes clearing the screen, setting up view and projection matrices,
 * rendering world meshes, and potentially debug information.
 */
void renderGame(
    SpectatorCamera& camera,
    VoxelEngine::Rendering::MeshRenderer& meshRenderer,
    VoxelEngine::Rendering::TextureAtlas& textureAtlas, // For debug drawing
    Window& gameWindow, // For swapping buffers and getting screen dimensions
    const std::vector<const VoxelEngine::Rendering::VoxelMesh*>& worldMeshes,
    int screenWidth, // For debug drawing, consider getting from Window if possible
    int screenHeight // For debug drawing, consider getting from Window if possible
);

/**
 * @brief Renders debug information, such as the texture atlas.
 * 
 * @param textureAtlas The texture atlas to render.
 * @param screenWidth The width of the screen.
 * @param screenHeight The height of the screen.
 */
void renderDebugInfo(
    const VoxelEngine::Rendering::TextureAtlas& textureAtlas,
    int screenWidth,
    int screenHeight
);

} // namespace GameRenderer
