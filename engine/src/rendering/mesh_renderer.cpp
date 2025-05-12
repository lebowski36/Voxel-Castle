#include "rendering/mesh_renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace VoxelEngine {
namespace Rendering {

MeshRenderer::MeshRenderer() {
    shaderProgram = createShaderProgram(
        "../assets/shaders/voxel.vert",
        "../assets/shaders/voxel.frag"
    );
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
}

MeshRenderer::~MeshRenderer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

void MeshRenderer::uploadMesh(const VoxelMesh& mesh) {
    // Print first 4 vertices as raw floats for debug
    std::cout << "Raw vertex buffer (first 4 vertices):" << std::endl;
    const float* raw = reinterpret_cast<const float*>(mesh.vertices.data());
    size_t floatsPerVertex = sizeof(Vertex) / sizeof(float);
    for (size_t i = 0; i < std::min<size_t>(4, mesh.vertices.size()); ++i) {
        std::cout << "  [" << i << "] ";
        for (size_t j = 0; j < floatsPerVertex; ++j) {
            std::cout << raw[i * floatsPerVertex + j] << " ";
        }
        std::cout << std::endl;
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

    // Only enable position attribute for debug
    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // glEnableVertexAttribArray(1); // normal
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // glEnableVertexAttribArray(2); // texCoord
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    indexCount = mesh.indices.size();
    ready = true;
    glBindVertexArray(0);
}

void MeshRenderer::draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {
    if (!ready) return;
    // Print debug info
    std::cout << "[MeshRenderer::draw] indexCount: " << indexCount
              << ", vao: " << vao << ", vbo: " << vbo << ", ebo: " << ebo
              << ", shaderProgram: " << shaderProgram << std::endl;

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error: 0x" << std::hex << err << std::dec << std::endl;
    }
}

GLuint MeshRenderer::loadShader(const std::string& path, GLenum type) {
    std::ifstream file(path);
    std::stringstream ss;
    if (file.is_open()) {
        ss << file.rdbuf();
        file.close(); // Ensure the file is closed before proceeding
    } else {
        std::cerr << "Error: Could not open shader file: " << path << std::endl;
        return 0; // Return 0 or handle error appropriately
    }
    std::string src = ss.str();
    GLuint shader = glCreateShader(type);
    const char* srcPtr = src.c_str();
    glShaderSource(shader, 1, &srcPtr, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error (" << path << "):\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint MeshRenderer::createShaderProgram(const std::string& vertPath, const std::string& fragPath) {
    GLuint vert = loadShader(vertPath, GL_VERTEX_SHADER);
    GLuint frag = loadShader(fragPath, GL_FRAGMENT_SHADER);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glDeleteShader(vert);
    glDeleteShader(frag);
    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cerr << "Shader link error:\n" << infoLog << std::endl;
    }
    return prog;
}

} // namespace Rendering
} // namespace VoxelEngine
