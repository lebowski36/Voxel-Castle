#include "core/GameRenderer.h" // Corrected include
#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h"
#include "rendering/voxel_mesh.h"
#include "SpectatorCamera.h" // Assuming this includes glm a
#include "platform/Window.h"   // For gameWindow->render() and potentially dimensions
#include "rendering/debug_render_mode.h" // Added for ::g_debugRenderMode
#include "rendering/debug_utils.h"
#include "utils/debug_logger.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream> // For std::cout, std::ostringstream
#include <set>      // For std::set in frame summary logging and mesh tracking
#include <sstream>  // For std::ostringstream

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

    // Store original polygon mode
    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);

    if (::g_debugRenderMode == DebugRenderMode::WIREFRAME) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();
    
    // Frame summary logging - every 1000 frames
    if (frameCounter % 1000 == 0) {
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

            // Only log brief summary without position details - redirect to file
            DEBUG_LOG("GameRenderer", "Frame: " + std::to_string(frameCounter) + ", Meshes: " + std::to_string(meshCount) + ", Vertices: " + std::to_string(totalVertices) + ", Unique Positions: " + std::to_string(uniquePositions.size()));
        }
    }
    frameCounter++;

    // Set debug render mode uniform for the shader
    GLint debugModeLoc = glGetUniformLocation(meshRenderer.getShaderProgram(), "uDebugRenderMode");
    if (debugModeLoc != -1) {
        glUseProgram(meshRenderer.getShaderProgram());
        glUniform1i(debugModeLoc, static_cast<int>(::g_debugRenderMode));
    }
    // CRITICAL FIX: Don't upload meshes every frame!
    // This was causing VAO corruption and crashes.
    // Use a set to track which meshes have been uploaded to avoid re-uploading
    static std::set<const VoxelEngine::Rendering::VoxelMesh*> uploadedMeshes;
    
    for (const auto* vMesh : worldMeshes) {
        if (vMesh && vMesh->isInitialized()) {
            // Only upload if this mesh hasn't been uploaded yet
            if (uploadedMeshes.find(vMesh) == uploadedMeshes.end()) {
                meshRenderer.uploadMesh(*vMesh);
                uploadedMeshes.insert(vMesh);
            }
            
            glm::mat4 model = glm::translate(glm::mat4(1.0f), vMesh->getWorldPosition());
            meshRenderer.draw(model, view, proj);
        }
    }

    // Restore original polygon mode
    glPolygonMode(GL_FRONT, polygonMode[0]);
    glPolygonMode(GL_BACK, polygonMode[1]);

    // Optional: Debug Atlas Rendering (can be called from here or separately)
    // renderDebugInfo(textureAtlas, screenWidth, screenHeight);

    // NOTE: Buffer swap moved to Game::render() after UI rendering is complete
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
