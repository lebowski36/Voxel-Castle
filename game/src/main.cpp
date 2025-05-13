#include "platform/Window.h" // Adjust path as necessary based on include directories
#include <iostream>
#include <vector>
#include <string>
#include <fstream> // Added
#include <sstream> // Added
#include <glad/glad.h>

// Flecs - ECS
#include <flecs.h>

// ECS Components
#include "ecs/components/position_component.h"
#include "ecs/components/velocity_component.h"
#include "ecs/components/renderable_component.h"

// ECS Systems
#include "ecs/systems/movement_system.h"

// Voxel World
#include "world/world_manager.h" // Defines VoxelCastle::World::WorldManager
#include "world/voxel_types.h" // Defines VoxelEngine::World::VoxelType
#include "world/voxel.h"       // Defines VoxelEngine::World::Voxel
#include "world/test_quadtree.h" // Added for Quadtree tests
#include "world/test_voxel_system.h" // Added for Voxel System tests

// Mesh Generation
#include "rendering/mesh_builder.h" // For VoxelEngine::Rendering::MeshBuilder
#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h" // Added for TextureAtlas
#include <chrono> // For std::chrono for timing
#include <thread> // For std::this_thread::sleep_for

// GLM Headers - Will be used by other engine systems, keep includes for now if generally useful
#define GLM_FORCE_SILENT_WARNINGS // Optional: To suppress GLM warnings if any
#define GLM_ENABLE_EXPERIMENTAL // Enable GLM experimental extensions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // Ensure this is included after enabling experimental extensions

// Variables for debug atlas quad
GLuint debugAtlasVAO = 0;
GLuint debugAtlasVBO = 0;
GLuint debugAtlasShaderProgram = 0;

// Variables for single tile debug quad
GLuint singleTileDebugVAO = 0;
GLuint singleTileDebugVBO = 0;
// Uses the same debugAtlasShaderProgram

const int SCREEN_WIDTH = 2400; // 3x original width
const int SCREEN_HEIGHT = 1800; // 3x original height

// Function to load and compile shaders (can be moved to a utility file later)
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
    GLuint frag = loadShader(fragPath, GL_FRAGMENT_SHADER); // Corrected this line
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
        std::cerr << "Debug Shader link error:\n" << infoLog << std::endl;
        glDeleteProgram(prog);
        prog = 0;
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return prog;
}

void setupDebugAtlasQuad() {
    const std::string projectRoot = "/home/system-x1/Projects/Voxel Castle/";
    debugAtlasShaderProgram = createShaderProgram(
        projectRoot + "assets/shaders/debug_atlas_display.vert",
        projectRoot + "assets/shaders/debug_atlas_display.frag"
    );
    if (debugAtlasShaderProgram == 0) {
        std::cerr << "Failed to create debug atlas shader program." << std::endl;
        return;
    }

    const float quadSize = 256.0f;
    float quadVertices[] = {
        // positions    // texCoords
        // TL
        0.0f,      0.0f,       0.0f, 1.0f,
        // BL
        0.0f,      quadSize,   0.0f, 0.0f,
        // TR
        quadSize,  0.0f,       1.0f, 1.0f,
        // BR
        quadSize,  quadSize,   1.0f, 0.0f
    };

    glGenVertexArrays(1, &debugAtlasVAO);
    glGenBuffers(1, &debugAtlasVBO);

    glBindVertexArray(debugAtlasVAO);

    glBindBuffer(GL_ARRAY_BUFFER, debugAtlasVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void setupSingleTileDebugQuad() {
    // Shader program is reused from the full atlas quad
    if (debugAtlasShaderProgram == 0) {
        std::cerr << "Cannot setup single tile debug quad: debugAtlasShaderProgram is not initialized." << std::endl;
        return;
    }

    const float quadScreenSize = 128.0f; // Half the size of the full atlas display
    const float atlasTotalSize = 256.0f;
    const float tileSizeInAtlas = 16.0f;

    // Stone tile: tile_x_idx = 1, tile_y_idx = 0
    // UVs are calculated based on (0,0) being bottom-left of the texture
    // and stbi_set_flip_vertically_on_load(true)
    float u_start = 1.0f / (atlasTotalSize / tileSizeInAtlas); // 1/16
    float u_end = 2.0f / (atlasTotalSize / tileSizeInAtlas);   // 2/16
    float v_start_flipped = 0.0f / (atlasTotalSize / tileSizeInAtlas); // Tile Y = 0, so starts at the "top" which is 0 after flip
    float v_end_flipped = 1.0f / (atlasTotalSize / tileSizeInAtlas);   // Ends one tile down

    // Correct V for OpenGL (0,0 is bottom-left) after stb_image flip (origin becomes top-left for calculation)
    // Tile Y = 0 means it's the *topmost* row of tiles in the image file.
    // After flipping, this topmost row's V coordinates are near the top of the texture (close to 1.0)
    float v_bottom = 1.0f - v_end_flipped;   // e.g. 1.0 - (1.0/16.0) = 15.0/16.0
    float v_top    = 1.0f - v_start_flipped; // e.g. 1.0 - (0.0/16.0) = 1.0

    // Vertex data: PosX, PosY, TexU, TexV
    float quadVertices[] = {
        // positions                                // texCoords
        // TL
        0.0f,            0.0f,                      u_start, v_top,
        // BL
        0.0f,            quadScreenSize,            u_start, v_bottom,
        // TR
        quadScreenSize,  0.0f,                      u_end,   v_top,
        // BR
        quadScreenSize,  quadScreenSize,            u_end,   v_bottom
    };

    glGenVertexArrays(1, &singleTileDebugVAO);
    glGenBuffers(1, &singleTileDebugVBO);

    glBindVertexArray(singleTileDebugVAO);

    glBindBuffer(GL_ARRAY_BUFFER, singleTileDebugVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    std::cout << "[Debug] Setup single stone tile debug quad (Top-Right). UVs: TL(" << u_start << "," << v_top << "), BR(" << u_end << "," << v_bottom << ")" << std::endl;
}

void drawDebugAtlasQuad(GLuint textureID) {
    if (debugAtlasShaderProgram == 0 || debugAtlasVAO == 0) return;

    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (depthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }

    glUseProgram(debugAtlasShaderProgram);

    glm::mat4 orthoProj = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, -1.0f, 1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(debugAtlasShaderProgram, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(debugAtlasShaderProgram, "model"), 1, GL_FALSE, &modelMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(debugAtlasShaderProgram, "screenTexture"), 0);

    glBindVertexArray(debugAtlasVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
}

void drawSingleTileDebugQuad(GLuint textureID) {
    if (debugAtlasShaderProgram == 0 || singleTileDebugVAO == 0) return;

    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (depthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }

    glUseProgram(debugAtlasShaderProgram); // Reuse the same shader

    // Position in top-right corner
    float quadScreenSize = 128.0f;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(SCREEN_WIDTH - quadScreenSize, 0.0f, 0.0f));
    glm::mat4 orthoProj = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, -1.0f, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(debugAtlasShaderProgram, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(debugAtlasShaderProgram, "model"), 1, GL_FALSE, &modelMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(debugAtlasShaderProgram, "screenTexture"), 0);

    glBindVertexArray(singleTileDebugVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
}

int main(int /*argc*/, char* /*argv*/[]) { // Suppress unused parameter warnings
    std::cout << "Starting VoxelFortress Game" << std::endl;

    // Initialize Flecs world
    flecs::world ecs;

    // Register systems
    VoxelEngine::ecs::MovementSystem movementSystem(ecs);
    // Future systems will be registered here

    VoxelCastle::World::WorldManager worldManager; // Keep a single instance for the game

    // Run all tests (excluding mesh generation tests that might rely on old direct building)
    std::cout << "--- Running Core Tests ---" << std::endl;
    VoxelCastle::World::runQuadtreeTests();     // Run Quadtree tests
    VoxelCastle::World::runVoxelSystemTests();  // Run Voxel System tests
    std::cout << "--- Core Tests Completed ---" << std::endl;

    // Use a larger window size for visibility
    Window gameWindow("Voxel Fortress - Alpha", SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!gameWindow.init()) {
        std::cerr << "Failed to initialize the game window!" << std::endl;
        return -1;
    }

    // --- OpenGL State Setup ---
    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Enable face culling for performance (render only front faces)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); 
    glFrontFace(GL_CCW); // Define front faces by counter-clockwise winding order
    // Set filled polygon mode for normal rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Debug OpenGL state
    std::cout << "OpenGL debug: Setting up rendering state" << std::endl;
    // Properly check if depth test is enabled
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    std::cout << "Depth testing enabled: " << (depthTestEnabled ? "Yes" : "No") << std::endl;
    GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    std::cout << "Face culling enabled: " << (cullFaceEnabled ? "Yes" : "No") << std::endl;
    

    // --- World and Rendering Setup ---
    VoxelEngine::Rendering::TextureAtlas atlas; // Create texture atlas
    VoxelEngine::Rendering::MeshBuilder meshBuilder; // Create mesh builder

    // Initialize a 1x1x1 chunk area (one ChunkColumn)
    worldManager.getOrCreateChunkColumn(0, 0); 

    // Initial mesh build for all dirty segments
    worldManager.updateDirtyMeshes(atlas, meshBuilder);

    // MeshRenderer setup - it will now manage multiple meshes
    VoxelEngine::Rendering::MeshRenderer meshRenderer;

    // Setup debug atlas quad
    setupDebugAtlasQuad();
    setupSingleTileDebugQuad(); // Setup the new quad

    // Camera setup
    glm::vec3 cameraPos   = glm::vec3(16.0f, 24.0f, 48.0f); // Adjusted for better view
    glm::vec3 cameraTarget = glm::vec3(8.0f, 8.0f, 8.0f);   // Look at the center of the first segment
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

    float fov = 70.0f; // Field of view

    float aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    glm::mat4 proj = glm::perspective(glm::radians(fov), aspect, 0.1f, 500.0f); // Increased far plane
    
    std::cout << "  Field of view: " << fov << " degrees" << std::endl;

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f); // A slightly more pleasant blue
    std::cout << "Clear color set to (0.1, 0.2, 0.3)" << std::endl;
    
    std::cout << "Starting main rendering loop" << std::endl;

    int frameCount = 0;
    glm::mat4 model = glm::mat4(1.0f); // Model matrix for meshes (identity, as positions are world coords)

    auto startTime = std::chrono::steady_clock::now();
    bool voxelChanged = false;

    // Main loop
    while (gameWindow.isRunning()) {
        gameWindow.handleEvents();
        ecs.progress();
        gameWindow.update();

        // --- Camera & View Matrix Update ---
        // Potentially update cameraPos, cameraTarget, cameraUp here if camera moves
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        // --- Game Logic & Mesh Updates ---
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        if (!voxelChanged && elapsedTime >= 3) {
            std::cout << "3 seconds elapsed. Changing a voxel..." << std::endl;
            worldManager.setVoxel(0, 0, 0, {static_cast<VoxelEngine::World::VoxelType>(3)}); // Use VoxelType 3
            voxelChanged = true;
            std::cout << "Voxel at (0,0,0) changed. WorldManager will mark segment dirty." << std::endl;
        }

        // Update meshes if any segments are dirty
        worldManager.updateDirtyMeshes(atlas, meshBuilder);

        // --- Rendering ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render all segment meshes
        auto meshesToRender = worldManager.getAllSegmentMeshes();
        for (const auto* vMesh : meshesToRender) {
            if (vMesh) { // Ensure mesh is not null
                 meshRenderer.uploadMesh(*vMesh); // Re-upload mesh data (can be optimized)
                 meshRenderer.draw(model, view, proj);
            }
        }
        
        // Debug Atlas Rendering (optional)
        // drawDebugAtlasQuad(atlas.getTextureID()); 
        // drawSingleTileDebugQuad(atlas.getTextureID());

        gameWindow.render();
        frameCount++;

        // Optional: Cap framerate or yield
        // std::this_thread::sleep_for(std::chrono::milliseconds(16)); 
    }

    if (debugAtlasVAO != 0) glDeleteVertexArrays(1, &debugAtlasVAO);
    if (debugAtlasVBO != 0) glDeleteBuffers(1, &debugAtlasVBO);
    if (debugAtlasShaderProgram != 0) glDeleteProgram(debugAtlasShaderProgram); // This shader is shared

    if (singleTileDebugVAO != 0) glDeleteVertexArrays(1, &singleTileDebugVAO);
    if (singleTileDebugVBO != 0) glDeleteBuffers(1, &singleTileDebugVBO);
    // Shader program is deleted with the full atlas quad resources

    return 0;
}
