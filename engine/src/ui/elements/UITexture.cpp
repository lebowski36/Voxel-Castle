#include "ui/elements/UITexture.h"
#include "ui/UIRenderer.h"
#include <iostream> // Add this header for std::cout and std::cerr

namespace VoxelEngine {
namespace UI {

UITexture::UITexture(UIRenderer* renderer) : 
    renderer_(renderer),
    textureID_(0),
    textureCoords_(0.0f, 0.0f, 1.0f, 1.0f), // Full texture by default
    color_(1.0f, 1.0f, 1.0f, 1.0f) // White tint by default
{
}

void UITexture::setTexture(GLuint textureID) {
    textureID_ = textureID;
}

void UITexture::setTextureCoords(const glm::vec4& texCoords) {
    textureCoords_ = texCoords;
}

void UITexture::setColor(const glm::vec4& color) {
    color_ = color;
}

void UITexture::render() {
    // DEBUGGING: Check texture rendering issues - increase logging frequency
    static int logCounter = 0;
    bool shouldLog = (logCounter++ % 30 == 0); // Log more frequently (every 30 frames)
    
    if (!renderer_) {
        if (shouldLog) {
            std::cerr << "[UITexture] ERROR: Cannot render: renderer is null!" << std::endl;
        }
        return;
    }
    
    if (textureID_ == 0) {
        if (shouldLog) {
            std::cerr << "[UITexture] ERROR: Cannot render: texture ID is 0!" << std::endl;
        }
        return;
    }
    
    if (!isVisible()) {
        if (shouldLog) {
            std::cout << "[UITexture] Not rendering because element is not visible" << std::endl;
        }
        return;
    }
    
    glm::vec2 absPos = getAbsolutePosition();
    
    // Check if element is off-screen
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    bool isOffScreen = (absPos.x + size_.x < 0 || absPos.y + size_.y < 0 || 
                       absPos.x > viewport[2] || absPos.y > viewport[3]);
    
    if (isOffScreen && shouldLog) {
        std::cerr << "[UITexture] WARNING: Element at (" << absPos.x << ", " << absPos.y 
                  << ") may be off-screen. Viewport: " << viewport[2] << "x" << viewport[3] << std::endl;
    }
    
    if (shouldLog) {
        std::cout << "[UITexture] Rendering at (" << absPos.x << ", " << absPos.y 
                  << "), size: (" << size_.x << ", " << size_.y 
                  << "), texture: " << textureID_ << std::endl;
        
        std::cout << "[UITexture] UV coordinates: (" 
                  << textureCoords_.x << ", " << textureCoords_.y << ", " 
                  << textureCoords_.z << ", " << textureCoords_.w << ")" << std::endl;
                  
        // Check current OpenGL state
        GLint currentShader = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
        std::cout << "[UITexture] Current shader program: " << currentShader << std::endl;
                  
        // Check if texture is valid (basic check)
        GLint prevTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
        glBindTexture(GL_TEXTURE_2D, textureID_);
        
        GLint width = 0, height = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "[UITexture] OpenGL error checking texture: 0x" << std::hex << err << std::dec << std::endl;
        } else {
            std::cout << "[UITexture] Texture dimensions: " << width << "x" << height << std::endl;
        }
        
        // Restore previous texture binding
        glBindTexture(GL_TEXTURE_2D, prevTexture);
        
        // Check depth testing state
        GLboolean depthTestEnabled;
        glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
        std::cout << "[UITexture] Depth testing enabled: " << (depthTestEnabled ? "yes" : "no") << std::endl;
    }
              
    // Attempt to render with the renderer
    renderer_->renderTexturedQuad(absPos.x, absPos.y, size_.x, size_.y, textureID_, textureCoords_);
}

} // namespace UI
} // namespace VoxelEngine
