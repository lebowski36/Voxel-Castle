#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h" // Added to access TILE_UV_WIDTH and TILE_UV_HEIGHT
#include "../../game/include/utils/debug_logger.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // Added for glm::scale, glm::translate, glm::rotate
#include <filesystem> // Required for std::filesystem::absolute

#include <SDL3/SDL.h> // Include SDL headers for types like Uint8, Uint16

#include "../../external/stb_image.h" // Include stb_image for texture loading without implementation

// Timestamp helper for mesh renderer
static std::string getTimestampMR() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// Helper function to check and log OpenGL errors with throttling
void checkGlError(const char* operation) {
    static std::unordered_map<std::string, int> errorCounts;
    static std::unordered_map<std::string, std::chrono::steady_clock::time_point> lastErrorTime;
    
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::string errorKey = std::string(operation) + "_" + std::to_string(err);
        auto now = std::chrono::steady_clock::now();
        
        errorCounts[errorKey]++;
        
        // Only log the first 3 occurrences of each error type, then every 1000th occurrence
        int count = errorCounts[errorKey];
        bool shouldLog = (count <= 3) || (count % 1000 == 0);
        
        // Also throttle by time - don't spam more than once per second for the same error
        if (shouldLog && (lastErrorTime[errorKey] + std::chrono::seconds(1) < now || count <= 3)) {
            std::cerr << "[" << getTimestampMR() << "] [OpenGL Error] After " << operation << ": 0x" << std::hex << err << std::dec;
            if (count > 3) {
                std::cerr << " (occurrence #" << count << ")";
            }
            std::cerr << std::endl;
            lastErrorTime[errorKey] = now;
            
            if (count == 3) {
                std::cerr << "[" << getTimestampMR() << "] [OpenGL Error] Further '" << operation << "' errors will be logged every 1000 occurrences." << std::endl;
            }
        }
    }
}

namespace VoxelEngine {
namespace Rendering {

static bool textureSamplerWarningLogged = false;

// Define a consistent base directory for asset paths
const std::string BASE_DIRECTORY = "/home/system-x1/Projects/Voxel Castle/";

MeshRenderer::MeshRenderer() : shaderProgram(0), textureAtlasID(0), ready(false) {
    // std::cout << "[" << getTimestampMR() << "] [MeshRenderer] Constructor started." << std::endl;
    checkGlError("Constructor Start");

    try {
        // std::cout << "[MeshRenderer] Current Working Directory: " << std::filesystem::current_path().string() << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[MeshRenderer] Error retrieving current working directory: " << e.what() << std::endl;
    }
    checkGlError("After getting CWD");

    // Correct paths for shaders
    const std::string projectRoot = BASE_DIRECTORY;
    const std::string vertexShaderPath = projectRoot + "assets/shaders/voxel.vert";
    const std::string fragmentShaderPath = projectRoot + "assets/shaders/voxel.frag";

    // std::cout << "[MeshRenderer] Creating shader program from:" << std::endl;
    DEBUG_LOG("MeshRenderer", "Vertex shader: " + vertexShaderPath);
    DEBUG_LOG("MeshRenderer", "Fragment shader: " + fragmentShaderPath);

    shaderProgram = createShaderProgram(vertexShaderPath, fragmentShaderPath);
    checkGlError("createShaderProgram");

    if (shaderProgram == 0) {
        std::cerr << "FATAL: [MeshRenderer] shaderProgram is 0. Shaders failed to load." << std::endl;
    } else {
        // std::cout << "[MeshRendaerer] Shader program created successfully. ID: " << shaderProgram << std::endl;
    }
    
    // Note: Texture is now loaded by TextureAtlas class
    DEBUG_LOG("MeshRenderer", "Texture atlas loading is now handled by TextureAtlas class");
    checkGlError("Constructor texture setup");

    // No longer creating shared VAO/VBO/EBO - each mesh has its own buffers
    ready = (shaderProgram != 0);
    if (!ready) {
        std::cerr << "[MeshRenderer] Constructor: Renderer not ready due to shader generation failure." << std::endl;
    } else {
        // std::cout << "[MeshRenderer] Constructor: Renderer is ready." << std::endl;
    }
}

MeshRenderer::~MeshRenderer() {
    // std::cout << "[MeshRenderer] Destructor called." << std::endl;
    // No longer need to clean up shared VAO/VBO/EBO - each mesh manages its own
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        checkGlError("glDeleteProgram (destructor)");
    }
    if (textureAtlasID) {
        glDeleteTextures(1, &textureAtlasID);
        checkGlError("glDeleteTextures (destructor)");
    }
}

// Upload mesh data to GPU buffers - now uses per-mesh buffers
void MeshRenderer::uploadMesh(const VoxelMesh& mesh) {
    // DIAGNOSTIC: Log buffer creation for first few uploads
    static int uploadCount = 0;
    uploadCount++;
    
    if (uploadCount <= 10) { // Log first 10 uploads
        DEBUG_LOG("MeshRenderer", "FIXED: Upload #" + std::to_string(uploadCount) + 
                 " creating unique buffers for mesh at (" + 
                 std::to_string(mesh.getWorldPosition().x) + ", " + 
                 std::to_string(mesh.getWorldPosition().z) + ")");
    }

    if (shaderProgram == 0) {
        std::cerr << "[MeshRenderer::uploadMesh] Shader program not initialized. Cannot upload." << std::endl;
        return;
    }

    if (mesh.vertices.size() == 0) { 
        // std::cout << "[MeshRenderer::uploadMesh] Uploading empty mesh (0 vertices, " << mesh.indices.size() << " indices)." << std::endl;
        return; // Nothing to upload for empty mesh
    }

    // Create OpenGL buffers for this mesh if not already created
    if (mesh.vao == 0) {
        glGenVertexArrays(1, &mesh.vao);
        checkGlError("glGenVertexArrays (per-mesh)");
        glGenBuffers(1, &mesh.vbo);
        checkGlError("glGenBuffers VBO (per-mesh)");
        glGenBuffers(1, &mesh.ebo);
        checkGlError("glGenBuffers EBO (per-mesh)");

        if (mesh.vao == 0 || mesh.vbo == 0 || mesh.ebo == 0) {
            std::cerr << "[MeshRenderer::uploadMesh] Failed to generate buffers for mesh." << std::endl;
            return;
        }
    }

    // Bind and upload mesh data
    glBindVertexArray(mesh.vao);
    checkGlError("glBindVertexArray (per-mesh upload)");
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    checkGlError("glBindBuffer GL_ARRAY_BUFFER (per-mesh upload)");
    size_t totalDataSize = mesh.vertices.size() * sizeof(VoxelEngine::Rendering::Vertex);
    glBufferData(GL_ARRAY_BUFFER, totalDataSize, mesh.vertices.data(), GL_STATIC_DRAW);
    checkGlError("glBufferData GL_ARRAY_BUFFER (per-mesh upload)");
    
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    checkGlError("glBindBuffer GL_ELEMENT_ARRAY_BUFFER (per-mesh upload)");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);
    checkGlError("glBufferData GL_ELEMENT_ARRAY_BUFFER (per-mesh upload)");

    // Setup vertex attributes for this VAO
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
    glEnableVertexAttribArray(5); 
    glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(VoxelEngine::Rendering::Vertex), (void*)offsetof(VoxelEngine::Rendering::Vertex, atlas_id));
    checkGlError("Vertex attribute setup (per-mesh upload)");

    mesh.buffersUploaded = true;
    
    glBindVertexArray(0);
    checkGlError("glBindVertexArray unbind (per-mesh upload)");
}

// Draw function - now uses per-mesh buffers
void MeshRenderer::draw(const VoxelMesh& mesh, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {
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

    if (!ready || shaderProgram == 0 || textureAtlasID == 0) {
        if (retryCount < maxRetries) {
            std::cerr << "[MeshRenderer::draw] Renderer not ready (shader: " << shaderProgram << ", texture: " << textureAtlasID << "). Attempt " << (retryCount + 1) << " of " << maxRetries << "." << std::endl;
            retryCount++;
        } else {
            std::cerr << "[MeshRenderer::draw] Renderer not ready. Max retries reached. No further attempts will be made." << std::endl;
            retriesExhausted = true;
        }
        return;
    }

    // Check if mesh has valid buffers and data
    if (!mesh.buffersUploaded || mesh.vao == 0 || mesh.indices.size() == 0) {
        // Silently skip empty meshes - they're common during world generation
        return;
    }

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    retryCount = 0; // Reset retry count if ready
    retriesExhausted = false; // Reset flag if ready

    // Only do detailed debug on first successful frame - redirect to file
    if (!initialDebugDone) {
        DEBUG_LOG("MeshRenderer", "==== MESH RENDERER DETAILED DEBUG (FIXED VERSION) ====");
        DEBUG_LOG("MeshRenderer", "Texture Atlas ID: " + std::to_string(textureAtlasID));
        DEBUG_LOG("MeshRenderer", "Shader Program ID: " + std::to_string(shaderProgram));
        DEBUG_LOG("MeshRenderer", "Using per-mesh buffers - no more shared VAO/VBO/EBO");
        DEBUG_LOG("MeshRenderer", "=======================================================");
        
        // Log the sampler warning summary here, only once
        if (textureSamplerWarningLogged) {
            DEBUG_LOG("MeshRenderer", "Summary (logged once): uTextureSampler uniform location was not found. This may indicate it is not used in the shader or an issue with shader compilation/linking.");
        }
        initialDebugDone = true;
    }
    
    frameCounter++;

    // Use the shader program
    glUseProgram(shaderProgram);
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

    // Activate texture units and bind all three atlases
    // Debug: Log atlas binding information
    static int atlas_debug_count = 0;
    if (atlas_debug_count < 3) {
        printf("ATLAS_DEBUG: Binding atlases - Main: %u, Side: %u, Bottom: %u\n", 
               mainAtlasID ? mainAtlasID : textureAtlasID, sideAtlasID, bottomAtlasID);
        atlas_debug_count++;
    }
    
    // Texture unit 0: Main atlas
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainAtlasID ? mainAtlasID : textureAtlasID);
    checkGlError("glBindTexture main atlas (draw)");
    
    // Texture unit 1: Side atlas
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sideAtlasID);
    checkGlError("glBindTexture side atlas (draw)");
    
    // Texture unit 2: Bottom atlas
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, bottomAtlasID);
    checkGlError("glBindTexture bottom atlas (draw)");

    // Set the sampler uniforms for each atlas
    GLint mainAtlasSamplerLoc = glGetUniformLocation(shaderProgram, "uMainAtlasSampler");
    GLint sideAtlasSamplerLoc = glGetUniformLocation(shaderProgram, "uSideAtlasSampler");
    GLint bottomAtlasSamplerLoc = glGetUniformLocation(shaderProgram, "uBottomAtlasSampler");
    
    // Debug: Log uniform locations
    if (atlas_debug_count <= 3) {
        printf("ATLAS_DEBUG: Uniform locations - Main: %d, Side: %d, Bottom: %d\n", 
               mainAtlasSamplerLoc, sideAtlasSamplerLoc, bottomAtlasSamplerLoc);
    }
    
    if (mainAtlasSamplerLoc != -1) {
        glUniform1i(mainAtlasSamplerLoc, 0); // Texture unit 0
        checkGlError("glUniform1i uMainAtlasSampler (draw)");
    }
    if (sideAtlasSamplerLoc != -1) {
        glUniform1i(sideAtlasSamplerLoc, 1); // Texture unit 1
        checkGlError("glUniform1i uSideAtlasSampler (draw)");
    }
    if (bottomAtlasSamplerLoc != -1) {
        glUniform1i(bottomAtlasSamplerLoc, 2); // Texture unit 2
        checkGlError("glUniform1i uBottomAtlasSampler (draw)");
    }

    // Legacy texture sampler (fallback for old shaders)
    GLint texSamplerLoc = glGetUniformLocation(shaderProgram, "uTextureSampler");
    checkGlError("glGetUniformLocation uTextureSampler (draw)");

    if (texSamplerLoc != -1) {
        glUniform1i(texSamplerLoc, 0); // Use main atlas as default
        checkGlError("glUniform1i uTextureSampler (draw)");
    } else {
        // If uTextureSampler is not found here, texturing will fail.
        if (!textureSamplerWarningLogged) { // Use the existing static flag to log only once.
            std::cerr << "[MeshRenderer::draw] Warning (logged once): uTextureSampler uniform not found after glUseProgram. Shader Program ID: " << shaderProgram << ". Texture rendering will likely fail." << std::endl;
            textureSamplerWarningLogged = true;
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

    // Bind this mesh's unique VAO and draw
    glBindVertexArray(mesh.vao);
    checkGlError("glBindVertexArray mesh-specific (draw)");

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    checkGlError("glDrawElements (draw)");

    glBindVertexArray(0);
    checkGlError("glBindVertexArray unbind (draw)");
    glBindTexture(GL_TEXTURE_2D, 0); 

    // Disable blending after drawing if it was enabled here
    glDisable(GL_BLEND);
}

// Multi-atlas support
void MeshRenderer::setAtlasTextures(GLuint mainAtlas, GLuint sideAtlas, GLuint bottomAtlas) {
    mainAtlasID = mainAtlas;
    sideAtlasID = sideAtlas;
    bottomAtlasID = bottomAtlas;
    
    // For backward compatibility, set the main atlas as the legacy textureAtlasID
    textureAtlasID = mainAtlas;
    
    DEBUG_LOG("MeshRenderer", "Multi-atlas textures set - Main: " + std::to_string(mainAtlas) + 
             ", Side: " + std::to_string(sideAtlas) + ", Bottom: " + std::to_string(bottomAtlas));
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

    DEBUG_LOG("MeshRenderer", "Attempting to open shader: " + path + " (Absolute: " + absolute_path_str + ")");

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

    DEBUG_LOG("MeshRenderer", "Loading texture from: " + resolvedPath);

    // Check if file exists and is accessible
    if (!std::filesystem::exists(resolvedPath)) {
        std::cerr << "[ERROR] Texture file not found: " << resolvedPath << std::endl;
        return false;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(resolvedPath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "[ERROR] Error loading texture '" << resolvedPath << "': " << stbi_failure_reason() << std::endl;
        return false;
    }

    std::cout << "[INFO] Loading texture atlas..." << std::endl;
    DEBUG_LOG("MeshRenderer", "Texture loaded successfully. Dimensions: " + std::to_string(width) + "x" + std::to_string(height) + ", Channels: " + std::to_string(channels));

    GLenum format;
    if (channels == 1) { format = GL_RED; }
    else if (channels == 3) { format = GL_RGB; }
    else if (channels == 4) { format = GL_RGBA; }
    else {
        std::cerr << "[ERROR] Unknown number of channels: " << channels << " for texture " << resolvedPath << std::endl;
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
