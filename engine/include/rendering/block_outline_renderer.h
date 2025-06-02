#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace Rendering {

/**
 * Renders a wireframe outline around a block to highlight it
 */
class BlockOutlineRenderer {
public:
    BlockOutlineRenderer();
    ~BlockOutlineRenderer();

    /**
     * Initialize the outline renderer (creates shaders and mesh)
     * @return True if initialization succeeded
     */
    bool initialize();

    /**
     * Render a wireframe outline around a block at the specified position
     * @param blockPosition Position of the block to outline (world coordinates)
     * @param view View matrix from camera
     * @param projection Projection matrix from camera
     * @param color Color of the outline (defaults to white)
     * @param lineWidth Width of the outline lines (defaults to 2.0f)
     */
    void renderOutline(const glm::ivec3& blockPosition, 
                      const glm::mat4& view, 
                      const glm::mat4& projection,
                      const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f),
                      float lineWidth = 2.0f);

    /**
     * Check if the renderer is ready to use
     * @return True if initialized properly
     */
    bool isReady() const { return ready_; }

private:
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
    GLuint shaderProgram_;
    bool ready_;

    /**
     * Create the wireframe cube mesh
     */
    void createWireframeCube();

    /**
     * Load and compile shaders for the outline
     * @return True if shaders compiled successfully
     */
    bool loadShaders();

    /**
     * Load a shader from source code
     * @param source Shader source code
     * @param type Shader type (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)
     * @return Shader ID or 0 if failed
     */
    GLuint loadShader(const char* source, GLenum type);
};

} // namespace Rendering
} // namespace VoxelEngine
