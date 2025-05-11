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

private:
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLuint shaderProgram = 0;
    size_t indexCount = 0;
    bool ready = false;

    GLuint loadShader(const std::string& path, GLenum type);
    GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath);
};

} // namespace Rendering
} // namespace VoxelEngine
