#include "ui/elements/Crosshair.h"
#include "../../../game/include/utils/debug_logger.h"
#include <glad/glad.h>
#include <iostream>

namespace VoxelEngine {
namespace UI {

Crosshair::Crosshair(UIRenderer* renderer) :
    UITexture(renderer),
    crosshairTextureID_(0)
{
    // Set size for the crosshair
    setSize(CROSSHAIR_SIZE, CROSSHAIR_SIZE);
    
    // Generate the crosshair texture
    generateCrosshairTexture();
    
    // Set the texture and make it fully white
    setTexture(crosshairTextureID_);
    setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // White crosshair
    
    DEBUG_LOG("Crosshair", "Created crosshair UI element");
}

void Crosshair::centerOnScreen(int screenWidth, int screenHeight) {
    // Position crosshair at exact center of screen
    float centerX = (screenWidth - size_.x) / 2.0f;
    float centerY = (screenHeight - size_.y) / 2.0f;
    
    setPosition(centerX, centerY);
    
    DEBUG_LOG("Crosshair", "Centered crosshair at (" + 
              std::to_string(centerX) + ", " + std::to_string(centerY) + 
              ") on screen " + std::to_string(screenWidth) + "x" + std::to_string(screenHeight));
}

void Crosshair::generateCrosshairTexture() {
    // Create a simple crosshair texture
    // 24x24 pixels with white crosshair lines and transparent background
    const int size = static_cast<int>(CROSSHAIR_SIZE);
    std::vector<unsigned char> textureData(size * size * 4, 0); // RGBA, start with transparent
    
    // Draw horizontal line
    int centerY = size / 2;
    for (int x = 0; x < size; x++) {
        // Skip center gap
        if (x >= size/2 - 2 && x <= size/2 + 2) continue;
        
        int index = (centerY * size + x) * 4;
        textureData[index + 0] = 255; // R
        textureData[index + 1] = 255; // G  
        textureData[index + 2] = 255; // B
        textureData[index + 3] = 255; // A
    }
    
    // Draw vertical line
    int centerX = size / 2;
    for (int y = 0; y < size; y++) {
        // Skip center gap
        if (y >= size/2 - 2 && y <= size/2 + 2) continue;
        
        int index = (y * size + centerX) * 4;
        textureData[index + 0] = 255; // R
        textureData[index + 1] = 255; // G
        textureData[index + 2] = 255; // B
        textureData[index + 3] = 255; // A
    }
    
    // Create OpenGL texture
    glGenTextures(1, &crosshairTextureID_);
    glBindTexture(GL_TEXTURE_2D, crosshairTextureID_);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.data());
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    DEBUG_LOG("Crosshair", "Generated crosshair texture with ID: " + std::to_string(crosshairTextureID_));
}

} // namespace UI
} // namespace VoxelEngine
