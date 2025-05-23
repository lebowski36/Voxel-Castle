#include "core/GameRenderer.h"
#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h"
#include "rendering/voxel_mesh.h"
#include "SpectatorCamera.h" // Assuming this includes glm a
#include "platform/Window.h"   // For gameWindow->render() and potentially dimensions
#include "rendering/debug_render_mode.h" // Added for ::g_debugRenderMode

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream> // For std::cout, std::ostringstream
#include <set>      // For std::set in frame summary logging
#include <sstream>  // For std::ostringstream

// If Debug Utils are specific to GameRenderer, include them here.
// Otherwise, they might be part of a general rendering utility.
#include "rendering/debug_utils.h" 

// Helper for logging, if needed
struct Vec3Comparator {
    bool operator()(const glm::vec3& a, const glm::vec3& b) const {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }
};

namespace GameRenderer {

void renderGame(
    SpectatorCamera& camera,
    VoxelEngine::Rendering::MeshRenderer& meshRenderer,
    VoxelEngine::Rendering::TextureAtlas& textureAtlas,
    Window& gameWindow,
    const std::vector<const VoxelEngine::Rendering::VoxelMesh*>& worldMeshes,
    int screenWidth,
    int screenHeight
) {
    static int frameCounter = 0; // Static frame counter for logging

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (::g_debugRenderMode == DebugRenderMode::WIREFRAME) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();
    
    // Frame summary logging - every 100 frames
    if (frameCounter % 100 == 0) {
        if (!worldMeshes.empty()) {
            std::set<glm::vec3, Vec3Comparator> uniquePositions;
            unsigned int meshCount = 0;
            unsigned int totalVertices = 0;
            unsigned int totalIndices = 0;

            for (const auto* vMesh : worldMeshes) {
                if (vMesh && vMesh->isInitialized()) {
                    meshCount++;
                    totalVertices += vMesh->getVertexCount();
                    totalIndices += vMesh->getIndexCount();
                    uniquePositions.insert(vMesh->getWorldPosition());
                }
            }

            std::ostringstream positionsStream;
            for (const auto& pos : uniquePositions) {
                positionsStream << "(" << pos.x << "," << pos.y << "," << pos.z << ") ";
            }

            std::cout << "[GameRenderer::renderGame Frame Summary] Frame: " << frameCounter 
                      << ", Meshes: " << meshCount 
                      << ", Vertices: " << totalVertices 
                      << ", Indices: " << totalIndices
                      << ", Unique Positions: " << uniquePositions.size() << " [" << positionsStream.str() << "]" << std::endl;
        }
    }
    frameCounter++;

    // Set debug render mode uniform for the shader
    GLint debugModeLoc = glGetUniformLocation(meshRenderer.getShaderProgram(), "uDebugRenderMode");
    if (debugModeLoc != -1) {
        glUseProgram(meshRenderer.getShaderProgram());
        glUniform1i(debugModeLoc, static_cast<int>(::g_debugRenderMode));
    }
    for (const auto* vMesh : worldMeshes) {
        if (vMesh && vMesh->isInitialized()) { 
             meshRenderer.uploadMesh(*vMesh); 
             glm::mat4 model = glm::translate(glm::mat4(1.0f), vMesh->getWorldPosition());
             meshRenderer.draw(model, view, proj);
        }
    }

    // No need to restore fill mode; already set to GL_FILL for textured rendering

    // Optional: Debug Atlas Rendering (can be called from here or separately)
    // renderDebugInfo(textureAtlas, screenWidth, screenHeight);

    gameWindow.render(); // Swaps buffers
}

void renderDebugInfo(
    const VoxelEngine::Rendering::TextureAtlas& textureAtlas,
    int screenWidth,
    int screenHeight
) {
    // Example: Draw the entire atlas
    // VoxelEngine::Rendering::Debug::drawDebugAtlasQuad(textureAtlas.getTextureID(), screenWidth, screenHeight);
    
    // Example: Draw a single tile from the atlas
    // VoxelEngine::Rendering::Debug::drawSingleTileDebugQuad(textureAtlas.getTextureID(), screenWidth, screenHeight);
    
    // Add more debug rendering calls as needed
}

} // namespace GameRenderer
