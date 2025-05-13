#include "rendering/debug_utils.h"
#include "rendering/render_utils.h" // For createShaderProgram
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream> // For std::cerr, std::cout

namespace VoxelEngine {
namespace Rendering {
namespace Debug {

// Define global variables
GLuint debugAtlasVAO = 0;
GLuint debugAtlasVBO = 0;
GLuint debugAtlasShaderProgram = 0;
GLuint singleTileDebugVAO = 0;
GLuint singleTileDebugVBO = 0;

// projectRoot is currently unused as shader paths are hardcoded relative to it.
// screenWidth and screenHeight are for future use if quad setup becomes dynamic with screen size.
void setupDebugAtlasQuad(const std::string& projectRoot, int /*screenWidth*/, int /*screenHeight*/) {
    debugAtlasShaderProgram = VoxelEngine::Rendering::createShaderProgram(
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// screenWidth and screenHeight are for future use if quad setup becomes dynamic with screen size.
void setupSingleTileDebugQuad(int /*screenWidth*/, int /*screenHeight*/) {
    if (debugAtlasShaderProgram == 0) {
        std::cerr << "Cannot setup single tile debug quad: debugAtlasShaderProgram is not initialized." << std::endl;
        return;
    }

    const float quadScreenSize = 128.0f;
    const float atlasTotalSize = 256.0f;
    const float tileSizeInAtlas = 16.0f;

    float u_start = 1.0f / (atlasTotalSize / tileSizeInAtlas);
    float u_end = 2.0f / (atlasTotalSize / tileSizeInAtlas);
    float v_start_flipped = 0.0f / (atlasTotalSize / tileSizeInAtlas);
    float v_end_flipped = 1.0f / (atlasTotalSize / tileSizeInAtlas);

    float v_bottom = 1.0f - v_end_flipped;
    float v_top    = 1.0f - v_start_flipped;

    float quadVertices[] = {
        0.0f,            0.0f,                      u_start, v_top,
        0.0f,            quadScreenSize,            u_start, v_bottom,
        quadScreenSize,  0.0f,                      u_end,   v_top,
        quadScreenSize,  quadScreenSize,            u_end,   v_bottom
    };

    glGenVertexArrays(1, &singleTileDebugVAO);
    glGenBuffers(1, &singleTileDebugVBO);

    glBindVertexArray(singleTileDebugVAO);

    glBindBuffer(GL_ARRAY_BUFFER, singleTileDebugVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    std::cout << "[Debug] Setup single stone tile debug quad (Top-Right). UVs: TL(" << u_start << "," << v_top << "), BR(" << u_end << "," << v_bottom << ")" << std::endl;
}

void drawDebugAtlasQuad(GLuint textureID, int screenWidth, int screenHeight) {
    if (debugAtlasShaderProgram == 0 || debugAtlasVAO == 0) return;

    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (depthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }

    glUseProgram(debugAtlasShaderProgram);

    glm::mat4 orthoProj = glm::ortho(0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f, -1.0f, 1.0f);
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

void drawSingleTileDebugQuad(GLuint textureID, int screenWidth, int screenHeight) {
    if (debugAtlasShaderProgram == 0 || singleTileDebugVAO == 0) return;

    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (depthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }

    glUseProgram(debugAtlasShaderProgram);

    float quadScreenSize = 128.0f;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth - quadScreenSize, 0.0f, 0.0f));
    glm::mat4 orthoProj = glm::ortho(0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f, -1.0f, 1.0f);

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

void cleanupDebugQuads() {
    if (debugAtlasVAO != 0) glDeleteVertexArrays(1, &debugAtlasVAO);
    if (debugAtlasVBO != 0) glDeleteBuffers(1, &debugAtlasVBO);
    if (debugAtlasShaderProgram != 0) glDeleteProgram(debugAtlasShaderProgram); 

    if (singleTileDebugVAO != 0) glDeleteVertexArrays(1, &singleTileDebugVAO);
    if (singleTileDebugVBO != 0) glDeleteBuffers(1, &singleTileDebugVBO);
    
    debugAtlasVAO = 0;
    debugAtlasVBO = 0;
    debugAtlasShaderProgram = 0;
    singleTileDebugVAO = 0;
    singleTileDebugVBO = 0;
}

} // namespace Debug
} // namespace Rendering
} // namespace VoxelEngine
