#include "ui/elements/UIPanel.h"
#include "ui/UIRenderer.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

UIPanel::UIPanel(UIRenderer* renderer) : 
    renderer_(renderer),
    color_(1.0f, 1.0f, 1.0f, 1.0f) // Default to white
{
    std::cout << "[UIPanel] Created with default white color" << std::endl;
}

void UIPanel::setColor(const glm::vec4& color) {
    color_ = color;
    std::cout << "[UIPanel] Color set to RGBA(" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
}

void UIPanel::render() {
    static int logCounter = 0;
    bool shouldLog = (logCounter++ % 30 == 0); // Log every 30 frames
    
    if (!renderer_) {
        if (shouldLog) {
            std::cerr << "[UIPanel] ERROR: Cannot render: renderer is null!" << std::endl;
        }
        return;
    }
    
    if (!isVisible()) {
        if (shouldLog) {
            std::cout << "[UIPanel] Not rendering because element is not visible" << std::endl;
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
        std::cerr << "[UIPanel] WARNING: Element at (" << absPos.x << ", " << absPos.y 
                  << ") may be off-screen. Viewport: " << viewport[2] << "x" << viewport[3] << std::endl;
    }
    
    if (shouldLog) {
        std::cout << "[UIPanel] Rendering solid color panel at (" << absPos.x << ", " << absPos.y 
                  << "), size: (" << size_.x << ", " << size_.y 
                  << "), color: RGBA(" << color_.r << ", " << color_.g << ", " << color_.b << ", " << color_.a << ")" << std::endl;
                  
        // Check current OpenGL state
        GLint currentShader = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
        std::cout << "[UIPanel] Current shader program: " << currentShader << std::endl;
                  
        // Check depth testing state
        GLboolean depthTestEnabled;
        glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
        std::cout << "[UIPanel] Depth testing enabled: " << (depthTestEnabled ? "yes" : "no") << std::endl;
    }
              
    // Render using the solid color quad method
    renderer_->renderQuad(absPos.x, absPos.y, size_.x, size_.y, color_);
}

} // namespace UI
} // namespace VoxelEngine
