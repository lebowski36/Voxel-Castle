#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <string>
#include <glad/glad.h> // For GLuint, GLenum

namespace VoxelEngine {
namespace Rendering {

/**
 * @brief Loads a shader from a file, compiles it, and returns its ID.
 * @param path Path to the shader file.
 * @param type Type of the shader (e.g., GL_VERTEX_SHADER, GL_FRAGMENT_SHADER).
 * @return Shader ID if successful, 0 otherwise.
 */
GLuint loadShader(const std::string& path, GLenum type);

/**
 * @brief Creates a shader program by linking vertex and fragment shaders.
 * @param vertPath Path to the vertex shader file.
 * @param fragPath Path to the fragment shader file.
 * @return Shader program ID if successful, 0 otherwise.
 */
GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath);

} // namespace Rendering
} // namespace VoxelEngine

#endif // RENDER_UTILS_H
