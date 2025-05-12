#include "rendering/mesh_renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem> // Required for std::filesystem::absolute

namespace VoxelEngine {
namespace Rendering {

MeshRenderer::MeshRenderer() : shaderProgram(0), vao(0), vbo(0), ebo(0), ready(false) {
    shaderProgram = createShaderProgram(
        "../assets/shaders/voxel.vert", // Adjusted path
        "../assets/shaders/voxel.frag"  // Adjusted path
    );
    if (shaderProgram == 0) {
        std::cerr << "FATAL: [MeshRenderer] shaderProgram is 0. Shaders failed to load." << std::endl;
        // 'ready' will remain false due to later checks or default initialization
    }
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    if (vao == 0 || vbo == 0 || ebo == 0) {
        std::cerr << "FATAL: [MeshRenderer] Failed to generate VAO/VBO/EBO." << std::endl;
        std::cerr << "  VAO: " << vao << ", VBO: " << vbo << ", EBO: " << ebo << std::endl;
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error during glGen*: 0x" << std::hex << err << std::dec << std::endl;
        }
        // 'ready' will remain false
    }
    // Initialize ready state based on fundamental setup
    ready = (shaderProgram != 0 && vao != 0 && vbo != 0 && ebo != 0);
    if (!ready) {
        std::cerr << "[MeshRenderer] Constructor: Renderer not ready due to shader or buffer generation failure." << std::endl;
    }
}

MeshRenderer::~MeshRenderer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

void MeshRenderer::uploadMesh(const VoxelMesh& mesh) {
    if (shaderProgram == 0 || vao == 0 || vbo == 0 || ebo == 0) {
        std::cerr << "[MeshRenderer::uploadMesh] Renderer not properly initialized (shader/buffers missing). Cannot upload." << std::endl;
        ready = false; // Ensure it's marked not ready
        indexCount = 0;
        return;
    }

    if (mesh.indices.size() > 0 && mesh.vertices.size() == 0) {
        std::cerr << "[MeshRenderer::uploadMesh] Error: Mesh has " << mesh.indices.size() << " indices but 0 vertices. Marking as not ready to render this mesh." << std::endl;
        // Clear potentially old data from buffers to avoid rendering stale data with new (zero) indexCount
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        indexCount = 0;
        ready = false; // Not ready to render *this* mesh configuration
        glBindVertexArray(0);
        return;
    }
    
    if (mesh.vertices.size() == 0) { // Covers (vertices=0, indices=0) and (vertices=0, indices>0 was caught above)
        std::cout << "[MeshRenderer::uploadMesh] Uploading empty mesh (0 vertices, " << mesh.indices.size() << " indices)." << std::endl;
    }

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
    GLenum err; 

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::uploadMesh] OpenGL error 0x" << std::hex << err << std::dec << " after VBO glBufferData." << std::endl;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::uploadMesh] OpenGL error 0x" << std::hex << err << std::dec << " after EBO glBufferData." << std::endl;
    }

    // Enable position, normal, and light attributes
    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, position));
    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, normal));
    // glEnableVertexAttribArray(2); // texCoord
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, texCoords));
    glEnableVertexAttribArray(3); // light
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, light));
    err = glGetError(); // Check after last AttribPointer
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::uploadMesh] OpenGL error 0x" << std::hex << err << std::dec << " after glVertexAttribPointer setup." << std::endl;
    }

    indexCount = mesh.indices.size();
    // Re-affirm ready state. If any GL error occurred above, this might be too optimistic.
    // The crucial part is that shaderProgram, vao, vbo, ebo must be valid.
    // And the current mesh data must not be inconsistent (like indices w/o vertices).
    ready = (shaderProgram != 0 && vao != 0 && vbo != 0 && ebo != 0); 
                                // The indices w/o vertices case sets ready=false and returns.
    
    if (!ready) {
         std::cerr << "[MeshRenderer::uploadMesh] MeshRenderer determined not ready after upload attempt." << std::endl;
    } else if (indexCount == 0 && mesh.vertices.size() > 0) {
        std::cout << "[MeshRenderer::uploadMesh] Mesh has " << mesh.vertices.size() << " vertices but 0 indices. Will draw 0 elements." << std::endl;
    }

    glBindVertexArray(0);
}

void MeshRenderer::draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {
    if (!ready) {
        // std::cout << "[MeshRenderer::draw] Not ready, skipping draw." << std::endl;
        return;
    }
    // shaderProgram == 0 should be covered by !ready, but double check for safety.
    if (shaderProgram == 0) {
        std::cerr << "[MeshRenderer::draw] Critical error: shaderProgram is 0 but renderer was marked ready. Skipping draw." << std::endl;
        return;
    }
    if (indexCount == 0) {
        // std::cout << "[MeshRenderer::draw] indexCount is 0, skipping glDrawElements." << std::endl;
        return; // Nothing to draw
    }

    GLenum err;

    glUseProgram(shaderProgram);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glUseProgram." << std::endl;
        // If glUseProgram fails, subsequent calls are likely to fail too.
    }

    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");

    if (modelLoc == -1 || viewLoc == -1 || projLoc == -1) {
        std::cerr << "[MeshRenderer::draw] Error: Could not get one or more uniform locations." << std::endl;
        std::cerr << "  uModel: " << modelLoc << ", uView: " << viewLoc << ", uProjection: " << projLoc << std::endl;
        // This will cause GL_INVALID_OPERATION on glUniformMatrix4fv if location is -1.
    }

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after uModel uniform." << std::endl;
    }

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after uView uniform." << std::endl;
    }

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after uProjection uniform." << std::endl;
    }

    glBindVertexArray(vao);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glBindVertexArray." << std::endl;
    }

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
    err = glGetError(); 
    if (err != GL_NO_ERROR) {
        // This is the most likely place for the original 0x502 if other checks pass
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glDrawElements." << std::endl;
    }

    glBindVertexArray(0);
    err = glGetError(); // Check error from unbinding VAO
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after unbinding VAO (final operation)." << std::endl;
    }
}

GLuint MeshRenderer::loadShader(const std::string& path, GLenum type) {
    // Attempt to get the absolute path for debugging
    std::string absolute_path_str = "Unknown (filesystem error)";
    try {
        std::filesystem::path p(path);
        if (p.is_relative()) {
            // If the path is relative, make it absolute based on the current working directory
            absolute_path_str = std::filesystem::absolute(p).string();
        } else {
            absolute_path_str = p.string();
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error when resolving path '" << path << "': " << e.what() << std::endl;
    }

    std::cout << "[MeshRenderer::loadShader] Attempting to open shader: " << path << " (Absolute: " << absolute_path_str << ")" << std::endl;

    std::ifstream file(path);
    std::stringstream ss;
    if (file.is_open()) {
        ss << file.rdbuf();
        file.close(); // Ensure the file is closed before proceeding
    } else {
        std::cerr << "Error: Could not open shader file: " << path << " (Resolved absolute: " << absolute_path_str << ")" << std::endl;
        // Also print current working directory
        try {
            std::cerr << "Current Working Directory: " << std::filesystem::current_path().string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error when getting CWD: " << e.what() << std::endl;
        }
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

    if (vert == 0 || frag == 0) {
        std::cerr << "[MeshRenderer::createShaderProgram] Error: Vertex or Fragment shader failed to load/compile." << std::endl;
        if (vert != 0) glDeleteShader(vert);
        if (frag != 0) glDeleteShader(frag);
        return 0;
    }

    GLuint prog = glCreateProgram();
    if (prog == 0) {
        std::cerr << "[MeshRenderer::createShaderProgram] Error: glCreateProgram failed." << std::endl;
        glDeleteShader(vert);
        glDeleteShader(frag);
        return 0;
    }

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cerr << "[MeshRenderer::createShaderProgram] Shader link error:\n" << infoLog << std::endl;
        glDeleteProgram(prog); // Clean up the failed program
        glDeleteShader(vert); 
        glDeleteShader(frag);
        return 0; // Return 0 to indicate failure
    }

    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::createShaderProgram] OpenGL error 0x" << std::hex << err << std::dec << " after successful link and shader cleanup." << std::endl;
    }

    return prog;
}

} // namespace Rendering
} // namespace VoxelEngine
