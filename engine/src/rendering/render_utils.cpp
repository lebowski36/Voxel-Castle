#include "rendering/render_utils.h"
#include <fstream>
#include <sstream>
#include <iostream> // For std::cerr

namespace VoxelEngine {
namespace Rendering {

GLuint loadShader(const std::string& path, GLenum type) {
    std::ifstream file(path);
    std::stringstream ss;
    if (file.is_open()) {
        ss << file.rdbuf();
        file.close();
    } else {
        std::cerr << "Error: Could not open shader file: " << path << std::endl;
        return 0;
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
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath) {
    GLuint vert = loadShader(vertPath, GL_VERTEX_SHADER);
    GLuint frag = loadShader(fragPath, GL_FRAGMENT_SHADER);
    if (vert == 0 || frag == 0) return 0;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cerr << "Shader link error:\n" << infoLog << std::endl;
        glDeleteProgram(prog);
        prog = 0;
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return prog;
}

} // namespace Rendering
} // namespace VoxelEngine
