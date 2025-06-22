#include "ui/UIRenderer.h"
#include "ui/TextRenderer.h"
#include "ui/UILogger.h"
#include "utils/debug_logger_stub.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

namespace VoxelEngine {
namespace UI {

UIRenderer::UIRenderer() : 
    shaderProgram_(0), vao_(0), vbo_(0), ebo_(0),
    screenWidth_(800), screenHeight_(600) {
    textRenderer_ = std::make_unique<TextRenderer>();
}

UIRenderer::~UIRenderer() {
    shutdown();
}

bool UIRenderer::initialize(int screenWidth, int screenHeight, const std::string& projectRoot) {
    auto& logger = UILogger::getInstance();
    logger.info("UIRenderer", "Starting initialization...");
    logger.info("UIRenderer", "Screen size: " + std::to_string(screenWidth) + "x" + std::to_string(screenHeight));
    logger.info("UIRenderer", "Project root: '" + projectRoot + "'");
    
    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;
    projectRoot_ = projectRoot;

    // Load default font
    std::string fontPath = projectRoot_ + "/assets/fonts/PressStart2P-Regular.ttf";
    if (!textRenderer_->initialize(fontPath, 16.0f)) {
        logger.error("UIRenderer", "Failed to load default font: " + fontPath);
        // Continue anyway as UI can work without text for now
    }

    // Create orthographic projection matrix (0,0 at top-left)
    projectionMatrix_ = glm::ortho(0.0f, static_cast<float>(screenWidth_), 
                                  static_cast<float>(screenHeight_), 0.0f, 
                                  -1.0f, 1.0f);
    
    // Set projection matrix for text renderer
    if (textRenderer_) {
        textRenderer_->setProjectionMatrix(projectionMatrix_);
    }
    
    if (!loadShaders()) {
        logger.error("UIRenderer", "Failed to load shaders");
        return false;
    }
    
    setupQuadGeometry();
    
    logger.info("UIRenderer", "Initialized with screen size " + std::to_string(screenWidth_) + "x" + std::to_string(screenHeight_));
    logger.info("UIRenderer", "VAO: " + std::to_string(vao_) + ", VBO: " + std::to_string(vbo_) + ", EBO: " + std::to_string(ebo_));
    return true;
}

void UIRenderer::shutdown() {
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (ebo_) glDeleteBuffers(1, &ebo_);
    if (shaderProgram_) glDeleteProgram(shaderProgram_);
    
    vao_ = vbo_ = ebo_ = shaderProgram_ = 0;
}

void UIRenderer::beginFrame() {
    // Clear any existing OpenGL errors first, but don't spam the console
    static int errorCount = 0;
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        if (errorCount++ % 100 == 0) {  // Only log occasionally
            DEBUG_LOG("UIRenderer", "Clearing OpenGL errors: 0x" + std::to_string(err));
        }
        while (glGetError() != GL_NO_ERROR) {} // Clear all errors
    }
    
    // Save the current OpenGL state - more complete
    GLint previousProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    
    GLint previousVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);
    
    GLboolean previousDepthTest;
    glGetBooleanv(GL_DEPTH_TEST, &previousDepthTest);
    
    GLboolean previousBlend;
    glGetBooleanv(GL_BLEND, &previousBlend);
    
    GLint previousBlendSrc, previousBlendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &previousBlendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &previousBlendDst);

    // Save viewport to restore later
    GLint previousViewport[4];
    glGetIntegerv(GL_VIEWPORT, previousViewport);
    
    // Make sure viewport is set correctly for UI rendering
    glViewport(0, 0, screenWidth_, screenHeight_);
    
    // Enable blending for UI elements - standard alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth testing for UI (always on top)
    glDisable(GL_DEPTH_TEST);
    
    // CRITICAL FIX: Also disable depth write for UI elements
    glDepthMask(GL_FALSE);
    
    if (shaderProgram_ == 0) {
        std::cerr << "[UIRenderer] ERROR: Cannot render UI - shader program is 0!" << std::endl;
        return;
    }
    
    glUseProgram(shaderProgram_);
    
    // Verify the program was activated
    GLint activeProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgram);
    if (static_cast<GLuint>(activeProgram) != shaderProgram_) {
        std::cerr << "[UIRenderer] ERROR: Failed to activate shader program! Expected: " 
                  << shaderProgram_ << ", got: " << activeProgram << std::endl;
    }
    
    // Set projection matrix
    GLint projLoc = glGetUniformLocation(shaderProgram_, "projection");
    if (projLoc != -1) {
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix_));
    } else {
        std::cerr << "[UIRenderer] ERROR: 'projection' uniform not found in shader!" << std::endl;
    }
    
    if (vao_ == 0) {
        std::cerr << "[UIRenderer] ERROR: VAO is 0, cannot render UI!" << std::endl;
        return;
    }
    
    glBindVertexArray(vao_);
}

void UIRenderer::endFrame() {
    // Only log once every 100 frames to reduce console spam
    static int frameCounter = 0;
    bool shouldLog = (frameCounter++ % 100 == 0);
    
    if (shouldLog) {
        DEBUG_LOG("UIRenderer", "Ending UI frame");
        
        // Check if we have any OpenGL errors before restoring state
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "[UIRenderer] OpenGL error before endFrame state restoration: 0x" 
                    << std::hex << err << std::dec << std::endl;
            while (glGetError() != GL_NO_ERROR) {} // Clear errors
        }
    }
    
    // Restore default OpenGL state
    glBindVertexArray(0);
    glUseProgram(0);
    
    // Restore depth testing and depth mask state
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);  // Re-enable depth writes
    
    // Disable blending (if the 3D renderer needs it enabled, it will turn it back on)
    glDisable(GL_BLEND);
    
    // Only check for errors occasionally to reduce spam
    if (shouldLog) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "[UIRenderer] OpenGL error after endFrame: 0x" 
                    << std::hex << err << std::dec << std::endl;
            while (glGetError() != GL_NO_ERROR) {} // Clear errors
        }
    }
}

void UIRenderer::setScreenSize(int width, int height) {
    screenWidth_ = width;
    screenHeight_ = height;
    projectionMatrix_ = glm::ortho(0.0f, static_cast<float>(screenWidth_), 
                                  static_cast<float>(screenHeight_), 0.0f, 
                                  -1.0f, 1.0f);
    
    // Update text renderer projection matrix
    if (textRenderer_) {
        textRenderer_->setProjectionMatrix(projectionMatrix_);
    }
}

bool UIRenderer::loadUIFont(const std::string& fontPath, float fontSize) {
    if (!textRenderer_) {
        std::cerr << "[UIRenderer] TextRenderer not initialized" << std::endl;
        return false;
    }
    
    bool result = textRenderer_->initialize(fontPath, fontSize);
    if (result) {
        textRenderer_->setProjectionMatrix(projectionMatrix_);
        std::cout << "[UIRenderer] Font loaded successfully: " << fontPath << std::endl;
    } else {
        std::cerr << "[UIRenderer] Failed to load font: " << fontPath << std::endl;
    }
    return result;
}

void UIRenderer::drawText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    if (textRenderer_) {
        textRenderer_->drawText(text, x, y, scale, color);
    }
}

float UIRenderer::getFontHeight() const {
    return textRenderer_ ? textRenderer_->getFontHeight() : 0.0f;
}

float UIRenderer::getTextWidth(const std::string& text, float scale) const {
    if (!textRenderer_) {
        return 0.0f;
    }
    
    // Calculate text width by summing up character advances
    float totalWidth = 0.0f;
    for (char c : text) {
        const auto* glyph = textRenderer_->getFontManager()->getGlyph(c);
        if (glyph) {
            totalWidth += glyph->advance * scale;
        }
    }
    return totalWidth;
}

float UIRenderer::getTextHeight(float scale) const {
    return getFontHeight() * scale;
}

void UIRenderer::renderColoredQuad(float x, float y, float width, float height, const glm::vec4& color) {
    // Add specific debug logging for bright magenta to trace the issue
    if (color.r > 0.9f && color.g < 0.1f && color.b > 0.9f) {
        std::cout << "[UIRenderer] BRIGHT MAGENTA renderColoredQuad called! Position(" << x << ", " << y << ") Size(" << width << ", " << height << ")" << std::endl;
    }
    
    // This is essentially the same as renderQuad, which already handles colored quads
    renderQuad(x, y, width, height, color);
}

void UIRenderer::renderQuad(float x, float y, float width, float height, const glm::vec4& color) {
    // Ensure blending is enabled for proper UI rendering with transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Clear any previous OpenGL errors before we start
    while (glGetError() != GL_NO_ERROR) {}
    
    // CRITICAL FIX: Ensure VAO is bound before rendering
    if (vao_ == 0) {
        std::cerr << "[UIRenderer] ERROR: VAO is 0, cannot render quad!" << std::endl;
        return;
    }
    
    // Bind VAO to ensure geometry is available
    glBindVertexArray(vao_);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] ERROR after glBindVertexArray: 0x" << std::hex << err << std::dec << std::endl;
        return;
    }
    
    // Ensure shader program is active
    if (shaderProgram_ == 0) {
        std::cerr << "[UIRenderer] ERROR: No shader program active!" << std::endl;
        return;
    }
    
    glUseProgram(shaderProgram_);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] ERROR after glUseProgram: 0x" << std::hex << err << std::dec << std::endl;
        return;
    }
    
    // Create model matrix for positioning and scaling
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    // Set uniforms with error checking
    GLint modelLoc = glGetUniformLocation(shaderProgram_, "model");
    if (modelLoc == -1) {
        std::cerr << "[UIRenderer] ERROR: 'model' uniform not found in shader!" << std::endl;
        return;
    }
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] ERROR after setting model uniform: 0x" << std::hex << err << std::dec << std::endl;
        return;
    }
    
    GLint colorLoc = glGetUniformLocation(shaderProgram_, "uColor");
    if (colorLoc == -1) {
        std::cerr << "[UIRenderer] ERROR: 'uColor' uniform not found in shader!" << std::endl;
        return;
    }
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] ERROR after setting color uniform: 0x" << std::hex << err << std::dec << std::endl;
        return;
    }
    
    GLint useTextureLoc = glGetUniformLocation(shaderProgram_, "uUseTexture");
    if (useTextureLoc == -1) {
        std::cerr << "[UIRenderer] ERROR: 'uUseTexture' uniform not found in shader!" << std::endl;
        return;
    }
    glUniform1i(useTextureLoc, 0); // Don't use texture, just color
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] ERROR after setting useTexture uniform: 0x" << std::hex << err << std::dec << std::endl;
        return;
    }
    
    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Check for OpenGL errors after drawing
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] OpenGL error after drawing quad: 0x" << std::hex << err << std::dec << std::endl;
    }
}

void UIRenderer::renderTexturedQuad(float x, float y, float width, float height, 
                                   GLuint textureID, const glm::vec4& texCoords) {
    // Suppressed repetitive logging for renderTexturedQuad calls.
    
    // Check for invalid texture
    if (textureID == 0) {
        std::cerr << "[UIRenderer] Error: Invalid texture ID (0)" << std::endl;
        return;
    }
    
    // Check for invalid position
    if (x < -width || x > screenWidth_ || y < -height || y > screenHeight_) {
        static int warnCounter = 0;
        if (warnCounter++ % 100 == 0) {
            std::cerr << "[UIRenderer] Warning: UI element position (" << x << "," << y 
                      << ") may be outside screen bounds " << screenWidth_ << "x" << screenHeight_ << std::endl;
        }
    }

    // CRITICAL FIX: Ensure VAO is bound before any buffer operations
    if (vao_ == 0) {
        std::cerr << "[UIRenderer] ERROR: VAO is 0, cannot render textured quad!" << std::endl;
        return;
    }
    
    glBindVertexArray(vao_);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] ERROR after glBindVertexArray: 0x" << std::hex << err << std::dec << std::endl;
        return;
    }
    
    // Ensure shader program is active
    if (shaderProgram_ == 0) {
        std::cerr << "[UIRenderer] ERROR: No shader program active!" << std::endl;
        return;
    }
    
    glUseProgram(shaderProgram_);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] ERROR after glUseProgram: 0x" << std::hex << err << std::dec << std::endl;
        return;
    }
    
    // Update vertex buffer with texture coordinates
    float vertices[] = {
        // positions        // texture coords
        0.0f, 1.0f,         texCoords.x, texCoords.w,  // bottom left
        1.0f, 1.0f,         texCoords.z, texCoords.w,  // bottom right
        1.0f, 0.0f,         texCoords.z, texCoords.y,  // top right
        0.0f, 0.0f,         texCoords.x, texCoords.y   // top left
    };
    
    // Save currently bound texture to restore later
    GLint prevTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    
    // Create model matrix for positioning and scaling
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    // Get shader uniforms and set them with proper error checking
    GLint modelLoc = glGetUniformLocation(shaderProgram_, "model");
    if (modelLoc == -1) {
        static bool warned = false;
        if (!warned) {
            std::cerr << "[UIRenderer] ERROR: 'model' uniform not found in shader" << std::endl;
            warned = true;
        }
    } else {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    }
    
    GLint colorLoc = glGetUniformLocation(shaderProgram_, "uColor");
    if (colorLoc != -1) {
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White tint
    }
    
    GLint useTextureLoc = glGetUniformLocation(shaderProgram_, "uUseTexture");
    if (useTextureLoc != -1) {
        glUniform1i(useTextureLoc, 1); // Use texture
    } else {
        static bool warned = false;
        if (!warned) {
            std::cerr << "[UIRenderer] ERROR: 'uUseTexture' uniform not found in shader" << std::endl;
            warned = true;
        }
    }
    
    // Bind texture with error checking
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Check texture binding worked
    GLint boundTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
    if (static_cast<GLuint>(boundTexture) != textureID) {
        std::cerr << "[UIRenderer] ERROR: Failed to bind texture! Expected: " 
                  << textureID << ", Got: " << boundTexture << std::endl;
    }
    
    GLint textureLoc = glGetUniformLocation(shaderProgram_, "uTexture");
    if (textureLoc != -1) {
        glUniform1i(textureLoc, 0);
    } else {
        static bool warned = false;
        if (!warned) {
            std::cerr << "[UIRenderer] ERROR: 'uTexture' uniform not found in shader" << std::endl;
            warned = true;
        }
    }
    
    // Check for OpenGL errors before drawing
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] OpenGL error before drawing: 0x" << std::hex << err << std::dec << std::endl;
        
        // Try to recover from error
        while (glGetError() != GL_NO_ERROR) {} // Clear all errors
    }
    
    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Check for OpenGL errors after drawing
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] OpenGL error after drawing: 0x" << std::hex << err << std::dec << std::endl;
    }
    
    // Restore previous texture binding
    glBindTexture(GL_TEXTURE_2D, prevTexture);
}

bool UIRenderer::loadShaders() {
    const std::string vertexShaderPath = projectRoot_ + "assets/shaders/ui.vert";
    const std::string fragmentShaderPath = projectRoot_ + "assets/shaders/ui.frag";
    
    // Redirected shader and texture loading details to file-based logging system.
    
    // Check if shader files exist
    std::ifstream vertFile(vertexShaderPath);
    if (!vertFile.good()) {
        std::cerr << "[UIRenderer] ERROR: Vertex shader file does not exist at: " << vertexShaderPath << std::endl;
        return false;
    }
    
    std::ifstream fragFile(fragmentShaderPath);
    if (!fragFile.good()) {
        std::cerr << "[UIRenderer] ERROR: Fragment shader file does not exist at: " << fragmentShaderPath << std::endl;
        return false;
    }
    
    DEBUG_LOG("UIRenderer", "Both shader files exist, continuing with compilation");

    // Reset any existing shader
    if (shaderProgram_) {
        glDeleteProgram(shaderProgram_);
        shaderProgram_ = 0;
    }
    
    shaderProgram_ = createShaderProgram(vertexShaderPath, fragmentShaderPath);
    
    if (shaderProgram_ != 0) {
        DEBUG_LOG("UIRenderer", "Shader program created successfully with ID: " + std::to_string(shaderProgram_));
        
        // Validate that we can find the required uniforms
        GLint projLoc = glGetUniformLocation(shaderProgram_, "projection");
        GLint modelLoc = glGetUniformLocation(shaderProgram_, "model");
        GLint colorLoc = glGetUniformLocation(shaderProgram_, "uColor");
        GLint useTexLoc = glGetUniformLocation(shaderProgram_, "uUseTexture");
        GLint texLoc = glGetUniformLocation(shaderProgram_, "uTexture");
        
        DEBUG_LOG("UIRenderer", "Shader uniform locations:");
        DEBUG_LOG("UIRenderer", "- projection: " + std::to_string(projLoc));
        DEBUG_LOG("UIRenderer", "- model: " + std::to_string(modelLoc));
        DEBUG_LOG("UIRenderer", "- uColor: " + std::to_string(colorLoc));
        DEBUG_LOG("UIRenderer", "- uUseTexture: " + std::to_string(useTexLoc));
        DEBUG_LOG("UIRenderer", "- uTexture: " + std::to_string(texLoc));
    } else {
        std::cerr << "[UIRenderer] Failed to create shader program!" << std::endl;
    }
    
    return shaderProgram_ != 0;
}

GLuint UIRenderer::loadShader(const std::string& path, GLenum type) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[UIRenderer] Failed to open shader file: " << path << std::endl;
        perror("[UIRenderer] System error");
        return 0;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    if (source.empty()) {
        std::cerr << "[UIRenderer] Shader file is empty: " << path << std::endl;
        return 0;
    }
    
    DEBUG_LOG("UIRenderer", "Shader file loaded: " + path + " (" + std::to_string(source.size()) + " bytes)");
    
    const char* sourceCStr = source.c_str();
    
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[UIRenderer] Shader compilation failed (" << path << "): " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    DEBUG_LOG("UIRenderer", "Shader compiled successfully: " + path);
    return shader;
}

GLuint UIRenderer::createShaderProgram(const std::string& vertPath, const std::string& fragPath) {
    // Check for OpenGL errors before we start
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] OpenGL error before shader creation: 0x" << std::hex << err << std::dec << std::endl;
    }

    GLuint vertexShader = loadShader(vertPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragPath, GL_FRAGMENT_SHADER);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        std::cerr << "[UIRenderer] Failed to load shaders: " 
                  << "vertex=" << vertexShader << ", fragment=" << fragmentShader << std::endl;
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "[UIRenderer] Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    } else {
        DEBUG_LOG("UIRenderer", "Shader program linked successfully: ID=" + std::to_string(program));
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Check for OpenGL errors after program creation
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[UIRenderer] OpenGL error after shader program creation: 0x" << std::hex << err << std::dec << std::endl;
    }
    
    return program;
}

void UIRenderer::setupQuadGeometry() {
    // Default quad vertices (will be updated for textured quads)
    float vertices[] = {
        // positions    // texture coords
        0.0f, 1.0f,     0.0f, 1.0f,  // bottom left
        1.0f, 1.0f,     1.0f, 1.0f,  // bottom right
        1.0f, 0.0f,     1.0f, 0.0f,  // top right
        0.0f, 0.0f,     0.0f, 0.0f   // top left
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    
    glBindVertexArray(vao_);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void UIRenderer::drawTestRectangle() {
    std::cout << "[UIRenderer] Attempting to draw test rectangle..." << std::endl;
    
    // Check shader program
    if (shaderProgram_ == 0) {
        std::cerr << "[UIRenderer] Cannot draw test rectangle: shader program is 0" << std::endl;
        return;
    }
    
    // Check VAO
    if (vao_ == 0) {
        std::cerr << "[UIRenderer] Cannot draw test rectangle: VAO is 0" << std::endl;
        return;
    }
    
    // Save current OpenGL state
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    
    GLint currentVAO = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    
    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    
    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    
    // Set up state for UI rendering
    glUseProgram(shaderProgram_);
    glBindVertexArray(vao_);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);  // Disable depth write
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Create model matrices for large rectangles in different screen locations
    // Try middle of screen with very vibrant colors that would be hard to miss
    constexpr float rectSize = 200.0f; // Make it very large - 200x200 pixels
    
    // Calculate positions for the center of the screen
    float centerX = (screenWidth_ - rectSize) / 2.0f;
    float centerY = (screenHeight_ - rectSize) / 2.0f;
    
    // Also try the corners
    const struct TestRect {
        float x, y;
        float r, g, b, a;  // Added alpha for testing
        const char* position;
    } testRects[] = {
        {centerX, centerY, 1.0f, 0.0f, 0.0f, 1.0f, "center"},                  // Center, red
        {50.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f, "top-left"},                    // Top-left, green
        {screenWidth_ - rectSize - 50.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f, "top-right"},  // Top-right, blue
        {50.0f, screenHeight_ - rectSize - 50.0f, 1.0f, 1.0f, 0.0f, 1.0f, "bottom-left"},  // Bottom-left, yellow
        {screenWidth_ - rectSize - 50.0f, screenHeight_ - rectSize - 50.0f, 1.0f, 0.0f, 1.0f, 1.0f, "bottom-right"}  // Bottom-right, magenta
    };
    
    // Draw all test rectangles to ensure at least one is visible
    for (const auto& rect : testRects) {
        std::cout << "[UIRenderer] Drawing " << rect.position << " rectangle at (" 
                  << rect.x << "," << rect.y << "), size: " << rectSize << "x" << rectSize
                  << ", color: RGBA(" << rect.r << "," << rect.g << "," << rect.b << "," << rect.a << ")" << std::endl;
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));
        model = glm::scale(model, glm::vec3(rectSize, rectSize, 1.0f));
        
        // Set uniforms
        GLint projLoc = glGetUniformLocation(shaderProgram_, "projection");
        if (projLoc != -1) {
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix_));
        }
        
        GLint modelLoc = glGetUniformLocation(shaderProgram_, "model");
        if (modelLoc != -1) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }
        
        GLint colorLoc = glGetUniformLocation(shaderProgram_, "uColor");
        if (colorLoc != -1) {
            glUniform4f(colorLoc, rect.r, rect.g, rect.b, rect.a);
        }
        
        GLint useTextureLoc = glGetUniformLocation(shaderProgram_, "uUseTexture");
        if (useTextureLoc != -1) {
            glUniform1i(useTextureLoc, 0); // No texture
        }
        
        // Draw using element buffer
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // Check for errors
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "[UIRenderer] OpenGL error after drawing " << rect.position << " test rectangle: 0x" 
                    << std::hex << err << std::dec << std::endl;
        }
    }
    
    std::cout << "[UIRenderer] Test rectangle drawn successfully" << std::endl;
    
    // Restore OpenGL state
    glUseProgram(currentProgram);
    glBindVertexArray(currentVAO);
    
    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    
    if (blendEnabled) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
    
    glDepthMask(GL_TRUE);  // Restore depth mask
}

} // namespace UI
} // namespace VoxelEngine
