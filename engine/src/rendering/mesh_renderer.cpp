#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h" // Added to access TILE_UV_WIDTH and TILE_UV_HEIGHT
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // Added for glm::scale, glm::translate, glm::rotate
#include <filesystem> // Required for std::filesystem::absolute

#include <SDL3/SDL.h> // Include SDL headers for types like Uint8, Uint16

#include "../../external/stb_image.h" // Include stb_image for texture loading without implementation

// Helper function to check and log OpenGL errors
void checkGlError(const char* operation) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "[OpenGL Error] After " << operation << ": 0x" << std::hex << err << std::dec << std::endl;
    }
}

namespace VoxelEngine {
namespace Rendering {

static bool textureSamplerWarningLogged = false;

// Define a consistent base directory for asset paths
const std::string BASE_DIRECTORY = "/home/system-x1/Projects/Voxel Castle/";

MeshRenderer::MeshRenderer() : vao(0), vbo(0), ebo(0), shaderProgram(0), textureAtlasID(0), ready(false) {
    std::cout << "[MeshRenderer] Constructor started." << std::endl;
    checkGlError("Constructor Start");

    try {
        std::cout << "[MeshRenderer] Current Working Directory: " << std::filesystem::current_path().string() << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[MeshRenderer] Error retrieving current working directory: " << e.what() << std::endl;
    }
    checkGlError("After getting CWD");

    // Correct paths for shaders and texture
    const std::string projectRoot = BASE_DIRECTORY;
    const std::string vertexShaderPath = projectRoot + "assets/shaders/voxel.vert";
    const std::string fragmentShaderPath = projectRoot + "assets/shaders/voxel.frag";
    const std::string texturePath = BASE_DIRECTORY + "assets/textures/atlas.png";

    std::cout << "[MeshRenderer] Creating shader program from:" << std::endl;
    std::cout << "  Vertex shader: " << vertexShaderPath << std::endl;
    std::cout << "  Fragment shader: " << fragmentShaderPath << std::endl;

    shaderProgram = createShaderProgram(vertexShaderPath, fragmentShaderPath);
    checkGlError("createShaderProgram");

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
    checkGlError("loadTexture");

    glGenVertexArrays(1, &vao);
    checkGlError("glGenVertexArrays");
    glGenBuffers(1, &vbo);
    checkGlError("glGenBuffers (VBO)");
    glGenBuffers(1, &ebo);
    checkGlError("glGenBuffers (EBO)");

    if (vao == 0 || vbo == 0 || ebo == 0) {
        std::cerr << "FATAL: [MeshRenderer] Failed to generate VAO/VBO/EBO." << std::endl;
    }

    // Set up VAO with vertex attributes
    glBindVertexArray(vao);
    checkGlError("glBindVertexArray (initial setup)");
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    checkGlError("glBindBuffer GL_ARRAY_BUFFER (initial setup)");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    checkGlError("glBindBuffer GL_ELEMENT_ARRAY_BUFFER (initial setup)");

    // Setup vertex attributes
    glEnableVertexAttribArray(0);
    checkGlError("glEnableVertexAttribArray(0)");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, position));
    checkGlError("glVertexAttribPointer(0)");

    glEnableVertexAttribArray(1);
    checkGlError("glEnableVertexAttribArray(1)");
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, normal));
    checkGlError("glVertexAttribPointer(1)");

    glEnableVertexAttribArray(2);
    checkGlError("glEnableVertexAttribArray(2)");
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, quad_uv));
    checkGlError("glVertexAttribPointer(2)");

    glEnableVertexAttribArray(3);
    checkGlError("glEnableVertexAttribArray(3)");
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, atlas_tile_origin_uv));
    checkGlError("glVertexAttribPointer(3)");
    
    glEnableVertexAttribArray(4);
    checkGlError("glEnableVertexAttribArray(4)");
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, light));
    checkGlError("glVertexAttribPointer(4)");

    glBindVertexArray(0); // Unbind VAO
    checkGlError("glBindVertexArray(0) (unbind after initial setup)");

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
    checkGlError("glDeleteVertexArrays (destructor)");
    glDeleteBuffers(1, &vbo);
    checkGlError("glDeleteBuffers VBO (destructor)");
    glDeleteBuffers(1, &ebo);
    checkGlError("glDeleteBuffers EBO (destructor)");
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        checkGlError("glDeleteProgram (destructor)");
    }
    if (textureAtlasID) {
        glDeleteTextures(1, &textureAtlasID);
        checkGlError("glDeleteTextures (destructor)");
    }
}

void MeshRenderer::uploadMesh(const VoxelMesh& mesh) {
    // if (shaderProgram == 0 || vao == 0 || vbo == 0 || ebo == 0) {
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
        // std::cout << "[MeshRenderer::uploadMesh] Uploading empty mesh (0 vertices, " << mesh.indices.size() << " indices)." << std::endl;
    }

    // std::cout << "[MeshRenderer::uploadMesh] Raw vertex buffer (first " << std::min<size_t>(4, mesh.vertices.size()) << " vertices):" << std::endl;
    // const float* raw = reinterpret_cast<const float*>(mesh.vertices.data());
    // size_t floatsPerVertex = sizeof(Vertex) / sizeof(float);
    // for (size_t i = 0; i < std::min<size_t>(4, mesh.vertices.size()); ++i) {
    //     std::cout << "  [" << i << "] ";
    //     for (size_t j = 0; j < floatsPerVertex; ++j) {
    //         std::cout << raw[i * floatsPerVertex + j] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    glBindVertexArray(vao);
    checkGlError("glBindVertexArray (uploadMesh initial)");
    
    // Clear any pre-existing OpenGL errors
    while (glGetError() != GL_NO_ERROR) {} 

    // Add debugging for the mesh data - only for large meshes or errors
    size_t expectedVertexSize = sizeof(VoxelEngine::Rendering::Vertex);
    size_t totalDataSize = mesh.vertices.size() * expectedVertexSize;
    
    static size_t upload_count = 0;
    upload_count++;
    bool should_log = (upload_count % 1000 == 0) || (mesh.vertices.size() > 8000);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    checkGlError("glBindBuffer GL_ARRAY_BUFFER (uploadMesh)");
    
    glBufferData(GL_ARRAY_BUFFER, totalDataSize, mesh.vertices.data(), GL_STATIC_DRAW);
    checkGlError("glBufferData GL_ARRAY_BUFFER (uploadMesh)");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    checkGlError("glBindBuffer GL_ELEMENT_ARRAY_BUFFER (uploadMesh)");
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);
    checkGlError("glBufferData GL_ELEMENT_ARRAY_BUFFER (uploadMesh)");

    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, position));
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, normal));
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, quad_uv));
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, atlas_tile_origin_uv));
    glEnableVertexAttribArray(4); 
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, light));
    checkGlError("Vertex attribute setup (uploadMesh)");

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
        // std::cout << "[MeshRenderer::uploadMesh] Mesh uploaded successfully. Index count: " << indexCount << ". Renderer is ready: " << std::boolalpha << ready << std::noboolalpha << std::endl;
    }

    glBindVertexArray(0);
    checkGlError("glBindVertexArray unbind (uploadMesh)");
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
    glUseProgram(shaderProgram); // This is the one we keep
    checkGlError("glUseProgram (draw)");

    // Set u_tile_uv_span uniform
    GLint tileUVSpanLoc = glGetUniformLocation(shaderProgram, "u_tile_uv_span");
    checkGlError("glGetUniformLocation u_tile_uv_span (draw)");
    if (tileUVSpanLoc != -1) {
        glUniform2f(tileUVSpanLoc, VoxelEngine::Rendering::TILE_UV_WIDTH, VoxelEngine::Rendering::TILE_UV_HEIGHT);
        checkGlError("glUniform2f u_tile_uv_span (draw)");
    } else {
        static bool tileUVSpanWarningLogged = false;
        if (!tileUVSpanWarningLogged) {
            std::cerr << "[MeshRenderer::draw] Warning (logged once): u_tile_uv_span uniform not found. Shader Program ID: " << shaderProgram << ". Tiling will not work correctly." << std::endl;
            tileUVSpanWarningLogged = true;
        }
    }

    // Activate texture unit 0 and bind our texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlasID);
    checkGlError("glBindTexture (draw)");

    // Set the sampler uniform
    GLint texSamplerLoc = glGetUniformLocation(shaderProgram, "uTextureSampler");
    checkGlError("glGetUniformLocation uTextureSampler (draw)");

    if (texSamplerLoc != -1) {
        glUniform1i(texSamplerLoc, 0);
        checkGlError("glUniform1i uTextureSampler (draw)");
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

    // Lighting uniforms
    GLint lightDirLoc = glGetUniformLocation(shaderProgram, "uLightDirection");
    GLint lightColorLoc = glGetUniformLocation(shaderProgram, "uLightColor");
    GLint ambientColorLoc = glGetUniformLocation(shaderProgram, "uAmbientColor");
    GLint ambientStrengthLoc = glGetUniformLocation(shaderProgram, "uAmbientStrength");

    if (modelLoc == -1 || viewLoc == -1 || projLoc == -1) {
        std::cerr << "[MeshRenderer::draw] Error: Could not get one or more uniform locations." << std::endl;
        std::cerr << "  uModel: " << modelLoc << ", uView: " << viewLoc << ", uProjection: " << projLoc << std::endl;
    }

    // Set lighting uniforms (basic sun-like lighting)
    if (lightDirLoc != -1) {
        glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f)); // Sun from above with slight angle
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirection));
    }
    if (lightColorLoc != -1) {
        glm::vec3 lightColor = glm::vec3(1.0f, 0.95f, 0.8f); // Warm white sun
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }
    if (ambientColorLoc != -1) {
        glm::vec3 ambientColor = glm::vec3(0.4f, 0.5f, 0.7f); // Slightly brighter cool blue ambient
        glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));
    }
    if (ambientStrengthLoc != -1) {
        float ambientStrength = 0.5f; // 50% ambient lighting for better visibility
        glUniform1f(ambientStrengthLoc, ambientStrength);
    }

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    checkGlError("glUniformMatrix4fv uModel (draw)");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    checkGlError("glUniformMatrix4fv uView (draw)");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    checkGlError("glUniformMatrix4fv uProjection (draw)");

    glBindVertexArray(vao);
    checkGlError("glBindVertexArray (draw)");

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
    checkGlError("glDrawElements (draw)");

    glBindVertexArray(0);
    checkGlError("glBindVertexArray unbind (draw)");
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
    checkGlError("glCreateShader");
    
    if (shader == 0) {
        std::cerr << "[MeshRenderer::loadShader] Error: glCreateShader returned 0." << std::endl;
        return 0;
    }
    
    const char* srcPtr = src.c_str();
    glShaderSource(shader, 1, &srcPtr, nullptr);
    checkGlError("glShaderSource");
    
    glCompileShader(shader);
    checkGlError("glCompileShader");
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    checkGlError("glGetShaderiv");
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
    checkGlError("glCreateProgram");
    
    if (prog == 0) {
        std::cerr << "[MeshRenderer::createShaderProgram] Error: glCreateProgram failed." << std::endl;
        glDeleteShader(vert);
        glDeleteShader(frag);
        return 0;
    }

    glAttachShader(prog, vert);
    checkGlError("glAttachShader vertex");
    
    glAttachShader(prog, frag);
    checkGlError("glAttachShader fragment");
    
    glLinkProgram(prog);
    checkGlError("glLinkProgram");

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
    
    checkGlError("Shader program creation cleanup");

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
    checkGlError("glGenTextures");
    
    if (textureAtlasID == 0) {
        std::cerr << "[MeshRenderer::loadTexture] Error: glGenTextures failed to generate texture ID." << std::endl;
        stbi_image_free(data);
        return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, textureAtlasID);
    checkGlError("glBindTexture (loadTexture)");

    // Set texture filtering and wrapping parameters for pixel art / texture atlases
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Changed from GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Changed from GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGlError("glTexParameteri calls (loadTexture)");

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    checkGlError("glTexImage2D (loadTexture)");
    // glGenerateMipmap(GL_TEXTURE_2D); // Temporarily disabled to simplify debugging

    stbi_image_free(data);
    return true;
}

} // namespace Rendering
} // namespace VoxelEngine
