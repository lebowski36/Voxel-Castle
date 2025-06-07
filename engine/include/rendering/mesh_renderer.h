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
    GLuint getTextureAtlasID() const { return textureAtlasID; } // Added getter - legacy main atlas
    void setTextureAtlasID(GLuint textureID) { textureAtlasID = textureID; } // Added setter - legacy main atlas
    
    // Multi-atlas support
    void setAtlasTextures(GLuint mainAtlas, GLuint sideAtlas, GLuint bottomAtlas);
    GLuint getShaderProgram() const { return shaderProgram; }

private:
    // Removed shared VAO/VBO/EBO - now each mesh has its own buffers
    GLuint shaderProgram = 0;
    GLuint textureAtlasID = 0; // Added for texture atlas - legacy main atlas
    
    // Multi-atlas support
    GLuint mainAtlasID = 0;
    GLuint sideAtlasID = 0;
    GLuint bottomAtlasID = 0;
    
    bool ready = false;

    GLuint loadShader(const std::string& path, GLenum type);
    GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath);
    bool loadTexture(const std::string& texturePath); // Added for loading texture
};

} // namespace Rendering
} // namespace VoxelEngine
