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
#include "world/test_mesh_generation.h" // Added for Mesh Generation tests

// Mesh Generation
#include "rendering/mesh_builder.h" // For VoxelEngine::Rendering::MeshBuilder
#include "world/chunk_segment.h"    // For VoxelCastle::World::ChunkSegment
#include "rendering/mesh_renderer.h"
#include "rendering/texture_atlas.h" // Added for TextureAtlas

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

const int SCREEN_WIDTH = 800; // Assuming these are defined or accessible
const int SCREEN_HEIGHT = 600;

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

int main(int /*argc*/, char* /*argv*/[]) { // Suppress unused parameter warnings
    std::cout << "Starting VoxelFortress Game" << std::endl;

    // Initialize Flecs world
    flecs::world ecs;

    // Register systems
    VoxelEngine::ecs::MovementSystem movementSystem(ecs);
    // Future systems will be registered here

    VoxelCastle::World::WorldManager worldManager; // Keep a single instance for the game

    // Run all tests
    std::cout << "--- Running All Tests ---" << std::endl;
    VoxelCastle::World::runQuadtreeTests();     // Run Quadtree tests
    VoxelCastle::World::runVoxelSystemTests();  // Run Voxel System tests
    VoxelCastle::World::runMeshGenerationTests(); // Run Mesh Generation tests
    std::cout << "--- All Tests Completed ---" << std::endl;

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
    // Disable face culling for debug (see all faces)
    glDisable(GL_CULL_FACE);
    // Set filled polygon mode for normal rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Debug OpenGL state
    std::cout << "OpenGL debug: Setting up rendering state" << std::endl;
    // Properly check if depth test is enabled
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    std::cout << "Depth testing enabled: " << (depthTestEnabled ? "Yes" : "No") << std::endl;
    
    // Force enable depth test again to be sure
    glEnable(GL_DEPTH_TEST);
    depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    std::cout << "Depth testing enabled after forcing: " << (depthTestEnabled ? "Yes" : "No") << std::endl;

    // --- Mesh Rendering Setup ---
    // Create a visible ground plane (16x16, y=8) alternating between DIRT and STONE
    VoxelCastle::World::ChunkSegment groundSegment;
    int groundY = 0;
    for (int x = 0; x < VoxelCastle::World::SEGMENT_WIDTH; ++x) {
        for (int z = 0; z < VoxelCastle::World::SEGMENT_DEPTH; ++z) {
            uint8_t type = ((x + z) % 2 == 0)
                ? static_cast<uint8_t>(VoxelEngine::World::VoxelType::DIRT)
                : static_cast<uint8_t>(VoxelEngine::World::VoxelType::STONE);
            groundSegment.setVoxel(x, groundY, z, VoxelEngine::World::Voxel(type));
        }
    }

    VoxelEngine::Rendering::TextureAtlas atlas;
    VoxelEngine::Rendering::VoxelMesh groundMesh = VoxelEngine::Rendering::MeshBuilder::buildGreedyMesh(groundSegment, atlas);

    // MeshRenderer setup
    VoxelEngine::Rendering::MeshRenderer meshRenderer;
    meshRenderer.uploadMesh(groundMesh);

    // Setup debug atlas quad
    setupDebugAtlasQuad();

    // Camera setup
    // New camera settings for better view of a potential 32x32x32 chunk centered around (16,0,16)
    glm::vec3 cameraPos   = glm::vec3(48.0f, 30.0f, 48.0f); 
    glm::vec3 cameraTarget = glm::vec3(16.0f, 0.0f, 16.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    float fov = 60.0f; // Field of view

    float aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    glm::mat4 proj = glm::perspective(glm::radians(fov), aspect, 0.1f, 300.0f); // Increased far plane to 300
    
    std::cout << "  Field of view: " << fov << " degrees" << std::endl;

    std::cout << "First 8 mesh vertex positions:" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(8, groundMesh.vertices.size()); ++i) {
        const auto& v = groundMesh.vertices[i];
        std::cout << "  [" << i << "] (" << v.position.x << ", " << v.position.y << ", " << v.position.z << ")" << std::endl;
    }

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    std::cout << "Clear color set to dark blue (0.0, 0.0, 0.2)" << std::endl;
    
    std::cout << "Starting main rendering loop" << std::endl;

    int frameCount = 0; // DEBUG: Frame counter
    glm::mat4 model = glm::mat4(1.0f); // Define model matrix for the main mesh

    while (gameWindow.isRunning() || frameCount < 100) { // DEBUG: Ensure at least 100 frames
        gameWindow.handleEvents();

        ecs.progress();

        gameWindow.update();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (frameCount % 100 == 0) {
            std::cout << "Frame " << frameCount << ": Rendering scene" << std::endl;
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "OpenGL error before draw: 0x" << std::hex << err << std::dec << std::endl;
            }
        }
        
        meshRenderer.draw(model, view, proj);

        drawDebugAtlasQuad(meshRenderer.getTextureAtlasID());

        gameWindow.render();
        frameCount++;
    }

    if (debugAtlasVAO != 0) glDeleteVertexArrays(1, &debugAtlasVAO);
    if (debugAtlasVBO != 0) glDeleteBuffers(1, &debugAtlasVBO);
    if (debugAtlasShaderProgram != 0) glDeleteProgram(debugAtlasShaderProgram);

    return 0;
}
