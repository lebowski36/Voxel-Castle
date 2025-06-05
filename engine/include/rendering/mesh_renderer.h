#pragma once
#include "rendering/voxel_mesh.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace VoxelEngine {
namespace Rendering {

class MeshRenderer {
public:
    MeshRenderer();
    ~MeshRenderer();

    void uploadMesh(const VoxelMesh& mesh);
    void draw(const VoxelMesh& mesh, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);
    bool isReady() const { return ready; }
    GLuint getTextureAtlasID() const { return textureAtlasID; } // Added getter
    void setTextureAtlasID(GLuint textureID) { textureAtlasID = textureID; } // Added setter
    GLuint getShaderProgram() const { return shaderProgram; }

private:
    // Removed shared VAO/VBO/EBO - now each mesh has its own buffers
    GLuint shaderProgram = 0;
    GLuint textureAtlasID = 0; // Added for texture atlas
    bool ready = false;

    GLuint loadShader(const std::string& path, GLenum type);
    GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath);
    bool loadTexture(const std::string& texturePath); // Added for loading texture
};

} // namespace Rendering
} // namespace VoxelEngine
