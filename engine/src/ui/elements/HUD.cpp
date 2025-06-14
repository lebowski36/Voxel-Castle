#include "ui/elements/HUD.h"
#include "utils/debug_logger_stub.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

HUD::HUD(UIRenderer* renderer, 
         VoxelEngine::Rendering::TextureAtlas* textureAtlas,
         GLuint atlasTextureID) :
    UITexture(renderer),
    textureAtlas_(textureAtlas),
    currentBlockType_(VoxelEngine::World::VoxelType::STONE) // Default to STONE
{
    // Set the atlas texture
    setTexture(atlasTextureID);
    
    // Set default size (64x64 pixels looks good for block selection)
    setSize(64.0f, 64.0f);
    
    // Update texture coordinates for the default block type
    updateTextureCoordinates();
    
    std::cout << "[HUD] Created with default block type: STONE" << std::endl;
}

void HUD::setCurrentBlockType(VoxelEngine::World::VoxelType blockType) {
    if (currentBlockType_ != blockType) {
        currentBlockType_ = blockType;
        updateTextureCoordinates();
        
        DEBUG_LOG("HUD", "Block type changed to: " + std::to_string(static_cast<int>(blockType)));
    }
}

void HUD::centerBottomOfScreen(int screenWidth, int screenHeight, int margin) {
    // Center horizontally
    float x = (screenWidth - size_.x) / 2.0f;
    
    // Position at bottom with margin
    float y = screenHeight - size_.y - margin;
    
    setPosition(x, y);
    
    std::cout << "[HUD] Positioned at (" << x << ", " << y << ") on screen " 
              << screenWidth << "x" << screenHeight << std::endl;
}

void HUD::updateTextureCoordinates() {
    if (!textureAtlas_) {
        std::cerr << "[HUD] TextureAtlas is null!" << std::endl;
        return;
    }
    
    // Get texture coordinates for the current block type
    auto texCoords = textureAtlas_->getTextureCoordinates(currentBlockType_);
    
    // Convert to the format expected by UITexture (x, y, width, height)
    glm::vec4 uvRect = texCoords.getUVRect();
    setTextureCoords(uvRect);
    
    DEBUG_LOG("HUD", "Updated texture coordinates for block type " + std::to_string(static_cast<int>(currentBlockType_)) + " to UV(" + std::to_string(uvRect.x) + ", " + std::to_string(uvRect.y) + ", " + std::to_string(uvRect.z) + ", " + std::to_string(uvRect.w) + ")");
}

} // namespace UI
} // namespace VoxelEngine
