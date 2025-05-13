#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace VoxelEngine {
namespace Rendering {
namespace Debug {

// Global variables for debug rendering
// These will be defined in debug_utils.cpp
extern GLuint debugAtlasVAO;
extern GLuint debugAtlasVBO;
extern GLuint debugAtlasShaderProgram;
extern GLuint singleTileDebugVAO;
extern GLuint singleTileDebugVBO;

// Screen dimensions are now passed as parameters to relevant functions.

void setupDebugAtlasQuad(const std::string& projectRoot, int screenWidth, int screenHeight);
void setupSingleTileDebugQuad(int screenWidth, int screenHeight); 
void drawDebugAtlasQuad(GLuint textureID, int screenWidth, int screenHeight);
void drawSingleTileDebugQuad(GLuint textureID, int screenWidth, int screenHeight);
void cleanupDebugQuads(); // For releasing OpenGL resources

} // namespace Debug
} // namespace Rendering
} // namespace VoxelEngine

#endif // DEBUG_UTILS_H
