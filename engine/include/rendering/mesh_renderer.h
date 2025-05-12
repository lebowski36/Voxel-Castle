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
    void draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);
    bool isReady() const { return ready; }
    GLuint getTextureAtlasID() const { return textureAtlasID; } // Added getter

private:
    GLuint vao = 0; // Changed order
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint shaderProgram = 0;
    GLuint textureAtlasID = 0; // Added for texture atlas
    size_t indexCount = 0;
    bool ready = false;

    GLuint loadShader(const std::string& path, GLenum type);
    GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath);
    bool loadTexture(const std::string& texturePath); // Added for loading texture
};

} // namespace Rendering
} // namespace VoxelEngine
