#include "rendering/mesh_renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // Added for glm::scale, glm::translate, glm::rotate
#include <filesystem> // Required for std::filesystem::absolute

#include <SDL3/SDL.h> // Include SDL headers for types like Uint8, Uint16

#include "../../external/stb_image.h" // Include stb_image for texture loading without implementation

namespace VoxelEngine {
namespace Rendering {

static bool textureSamplerWarningLogged = false;

// Define a consistent base directory for asset paths
const std::string BASE_DIRECTORY = "/home/system-x1/Projects/Voxel Castle/";

MeshRenderer::MeshRenderer() : vao(0), vbo(0), ebo(0), shaderProgram(0), textureAtlasID(0), ready(false) {
    std::cout << "[MeshRenderer] Constructor started." << std::endl;

    try {
        std::cout << "[MeshRenderer] Current Working Directory: " << std::filesystem::current_path().string() << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[MeshRenderer] Error retrieving current working directory: " << e.what() << std::endl;
    }

    // Correct paths for shaders and texture
    const std::string projectRoot = BASE_DIRECTORY;
    const std::string vertexShaderPath = projectRoot + "assets/shaders/voxel.vert";
    const std::string fragmentShaderPath = projectRoot + "assets/shaders/voxel.frag";
    const std::string texturePath = BASE_DIRECTORY + "assets/textures/atlas.png";

    std::cout << "[MeshRenderer] Creating shader program from:" << std::endl;
    std::cout << "  Vertex shader: " << vertexShaderPath << std::endl;
    std::cout << "  Fragment shader: " << fragmentShaderPath << std::endl;

    shaderProgram = createShaderProgram(vertexShaderPath, fragmentShaderPath);

    if (shaderProgram == 0) {
        std::cerr << "FATAL: [MeshRenderer] shaderProgram is 0. Shaders failed to load." << std::endl;
    } else {
        std::cout << "[MeshRenderer] Shader program created successfully. ID: " << shaderProgram << std::endl;
    }

    if (!loadTexture(texturePath)) {
        std::cerr << "FATAL: [MeshRenderer] Failed to load texture atlas." << std::endl;
    } else {
        std::cout << "[MeshRenderer] Texture atlas loaded successfully. ID: " << textureAtlasID << std::endl;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    if (vao == 0 || vbo == 0 || ebo == 0) {
        std::cerr << "FATAL: [MeshRenderer] Failed to generate VAO/VBO/EBO." << std::endl;
    }

    ready = (shaderProgram != 0 && vao != 0 && vbo != 0 && ebo != 0 && textureAtlasID != 0);
    if (!ready) {
        std::cerr << "[MeshRenderer] Constructor: Renderer not ready due to shader, buffer, or texture generation failure." << std::endl;
    } else {
        std::cout << "[MeshRenderer] Constructor: Renderer is ready." << std::endl;
    }
}

MeshRenderer::~MeshRenderer() {
    std::cout << "[MeshRenderer] Destructor called." << std::endl;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (textureAtlasID) glDeleteTextures(1, &textureAtlasID); 
}

void MeshRenderer::uploadMesh(const VoxelMesh& mesh) {
    std::cout << "[MeshRenderer::uploadMesh] Called with mesh (" << mesh.vertices.size() << " vertices, " << mesh.indices.size() << " indices)." << std::endl;
    if (shaderProgram == 0 || vao == 0 || vbo == 0 || ebo == 0) {
        std::cerr << "[MeshRenderer::uploadMesh] Renderer not properly initialized (shader/buffers missing). Cannot upload." << std::endl;
        std::cout << "  Shader Program ID: " << shaderProgram << ", VAO: " << vao << ", VBO: " << vbo << ", EBO: " << ebo << std::endl;
        ready = false; 
        indexCount = 0;
        return;
    }

    if (mesh.indices.size() > 0 && mesh.vertices.size() == 0) {
        std::cerr << "[MeshRenderer::uploadMesh] Error: Mesh has " << mesh.indices.size() << " indices but 0 vertices. Marking as not ready to render this mesh." << std::endl;
        glBindVertexArray(vao);
        std::cout << "[MeshRenderer::uploadMesh] Clearing VBO and EBO due to inconsistent mesh data." << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        indexCount = 0;
        ready = false; 
        glBindVertexArray(0);
        return;
    }
    
    if (mesh.vertices.size() == 0) { 
        std::cout << "[MeshRenderer::uploadMesh] Uploading empty mesh (0 vertices, " << mesh.indices.size() << " indices)." << std::endl;
    }

    std::cout << "[MeshRenderer::uploadMesh] Raw vertex buffer (first " << std::min<size_t>(4, mesh.vertices.size()) << " vertices):" << std::endl;
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

    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, position));
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, normal));
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, texCoords));
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, light));
    err = glGetError(); 
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::uploadMesh] OpenGL error 0x" << std::hex << err << std::dec << " after glVertexAttribPointer setup." << std::endl;
    }

    indexCount = mesh.indices.size();
    ready = (shaderProgram != 0 && vao != 0 && vbo != 0 && ebo != 0 && textureAtlasID != 0); 
                                
    if (!ready) {
         std::cerr << "[MeshRenderer::uploadMesh] MeshRenderer determined not ready after upload attempt." << std::endl;
         std::cout << "  Shader Program ID: " << shaderProgram << std::endl;
         std::cout << "  VAO ID: " << vao << std::endl;
         std::cout << "  VBO ID: " << vbo << std::endl;
         std::cout << "  EBO ID: " << ebo << std::endl;
         std::cout << "  Texture Atlas ID: " << textureAtlasID << std::endl;
    } else if (indexCount == 0 && mesh.vertices.size() > 0) {
        std::cout << "[MeshRenderer::uploadMesh] Mesh has " << mesh.vertices.size() << " vertices but 0 indices. Will draw 0 elements." << std::endl;
    } else {
        std::cout << "[MeshRenderer::uploadMesh] Mesh uploaded successfully. Index count: " << indexCount << ". Renderer is ready: " << std::boolalpha << ready << std::noboolalpha << std::endl;
    }

    glBindVertexArray(0);
}

// Draw function with improved debugging and logging
void MeshRenderer::draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {
    static int retryCount = 0;
    static bool retriesExhausted = false;
    static int frameCounter = 0;
    static bool initialDebugDone = false;
    const int maxRetries = 5;
    const int frameLogFrequency = 100; // Log every 100 frames

    if (retriesExhausted) {
        if (frameCounter % frameLogFrequency == 0) {
            std::cerr << "[MeshRenderer::draw] Renderer not ready. Skipping draw." << std::endl;
        }
        frameCounter++;
        return; // Stop further attempts
    }

    if (!ready) {
        if (retryCount < maxRetries) {
            std::cerr << "[MeshRenderer::draw] Renderer not ready. Attempt " << (retryCount + 1) << " of " << maxRetries << "." << std::endl;
            retryCount++;
        } else {
            std::cerr << "[MeshRenderer::draw] Renderer not ready. Max retries reached. No further attempts will be made." << std::endl;
            retriesExhausted = true;
        }
        return;
    }

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    retryCount = 0; // Reset retry count if ready
    retriesExhausted = false; // Reset flag if ready

    // Only do detailed debug on first successful frame - removed redundant frame logging
    if (!initialDebugDone) {
        std::cout << "\n==== MESH RENDERER DETAILED DEBUG ====" << std::endl;
        std::cout << "Texture Atlas ID: " << textureAtlasID << std::endl;
        std::cout << "Shader Program ID: " << shaderProgram << std::endl;
        std::cout << "VAO: " << vao << ", VBO: " << vbo << ", EBO: " << ebo << std::endl;
        std::cout << "Index Count: " << indexCount << std::endl;
        std::cout << "======================================\n" << std::endl;
        
        // Log the sampler warning summary here, only once
        if (textureSamplerWarningLogged) {
            std::cerr << "[MeshRenderer::draw] Summary (logged once): uTextureSampler uniform location was not found. This may indicate it is not used in the shader or an issue with shader compilation/linking." << std::endl;
        }
        initialDebugDone = true;
    }
    
    frameCounter++;

    // Use the shader program
    GLenum err;
    glUseProgram(shaderProgram); // This is the one we keep
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glUseProgram." << std::endl;
        return;
    }

    // Activate texture unit 0 and bind our texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlasID);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glBindTexture. Texture ID: " << textureAtlasID << std::endl;
    }

    // Set the sampler uniform
    GLint texSamplerLoc = glGetUniformLocation(shaderProgram, "uTextureSampler");
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glGetUniformLocation for uTextureSampler." << std::endl;
    }

    if (texSamplerLoc != -1) {
        glUniform1i(texSamplerLoc, 0);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glUniform1i for uTextureSampler." << std::endl;
        }
    } else {
        // If uTextureSampler is not found here, texturing will fail.
        if (!textureSamplerWarningLogged) { // Use the existing static flag to log only once.
            std::cerr << "[MeshRenderer::draw] Warning (logged once): uTextureSampler uniform not found after glUseProgram. Shader Program ID: " << shaderProgram << ". Texture rendering will likely fail." << std::endl;
            textureSamplerWarningLogged = true; // This ensures the summary in initialDebugDone also prints if it hasn't already.
        }
    }

    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");

    if (modelLoc == -1 || viewLoc == -1 || projLoc == -1) {
        std::cerr << "[MeshRenderer::draw] Error: Could not get one or more uniform locations." << std::endl;
        std::cerr << "  uModel: " << modelLoc << ", uView: " << viewLoc << ", uProjection: " << projLoc << std::endl;
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
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after glDrawElements." << std::endl;
    }

    glBindVertexArray(0);
    err = glGetError(); 
    if (err != GL_NO_ERROR) {
        std::cerr << "[MeshRenderer::draw] OpenGL error 0x" << std::hex << err << std::dec << " after unbinding VAO (final operation)." << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0); 

    // Disable blending after drawing if it was enabled here
    glDisable(GL_BLEND);
}

GLuint MeshRenderer::loadShader(const std::string& path, GLenum type) {
    std::string absolute_path_str = "Unknown (filesystem error)";
    try {
        std::filesystem::path p(path);
        if (p.is_relative()) {
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
        file.close(); 
    } else {
        std::cerr << "Error: Could not open shader file: " << path << " (Resolved absolute: " << absolute_path_str << ")" << std::endl;
        try {
            std::cerr << "Current Working Directory: " << std::filesystem::current_path().string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error when getting CWD: " << e.what() << std::endl;
        }
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
        glDeleteProgram(prog); 
        glDeleteShader(vert); 
        glDeleteShader(frag);
        return 0; 
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

bool MeshRenderer::loadTexture(const std::string& path) {
    std::string resolvedPath;
    if (std::filesystem::path(path).is_absolute()) {
        resolvedPath = path; // Use the provided absolute path
    } else {
        resolvedPath = BASE_DIRECTORY + "assets/textures/" + path; // Prepend the base directory
    }

    std::cout << "[MeshRenderer::loadTexture] Base directory: " << BASE_DIRECTORY << std::endl;
    std::cout << "[MeshRenderer::loadTexture] Resolved path: " << resolvedPath << std::endl;

    // Check if file exists and is accessible
    if (!std::filesystem::exists(resolvedPath)) {
        std::cerr << "[MeshRenderer::loadTexture] File not found: " << resolvedPath << std::endl;
        return false;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(resolvedPath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "[MeshRenderer::loadTexture] Error loading texture '" << resolvedPath << "': " << stbi_failure_reason() << std::endl;
        return false;
    }

    std::cout << "[MeshRenderer::loadTexture] Texture loaded successfully. Dimensions: " << width << "x" << height << ", Channels: " << channels << std::endl;

    GLenum format;
    if (channels == 1) { format = GL_RED; }
    else if (channels == 3) { format = GL_RGB; }
    else if (channels == 4) { format = GL_RGBA; }
    else {
        std::cerr << "[MeshRenderer::loadTexture] Error loading texture: Unknown number of channels: " << channels << " for texture " << resolvedPath << std::endl;
        stbi_image_free(data);
        return false;
    }

    glGenTextures(1, &textureAtlasID);
    glBindTexture(GL_TEXTURE_2D, textureAtlasID);

    // Set texture filtering and wrapping parameters for pixel art / texture atlases
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D); // Temporarily disabled to simplify debugging

    stbi_image_free(data);
    return true;
}

} // namespace Rendering
} // namespace VoxelEngine
