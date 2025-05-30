#include "ui/elements/BlockSelectionUI.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

BlockSelectionUI::BlockSelectionUI(UIRenderer* renderer, 
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
    
    std::cout << "[BlockSelectionUI] Created with default block type: STONE" << std::endl;
}

void BlockSelectionUI::setCurrentBlockType(VoxelEngine::World::VoxelType blockType) {
    if (currentBlockType_ != blockType) {
        currentBlockType_ = blockType;
        updateTextureCoordinates();
        
        std::cout << "[BlockSelectionUI] Block type changed to: " << static_cast<int>(blockType) << std::endl;
    }
}

void BlockSelectionUI::centerBottomOfScreen(int screenWidth, int screenHeight, int margin) {
    // Center horizontally
    float x = (screenWidth - size_.x) / 2.0f;
    
    // Position at bottom with margin
    float y = screenHeight - size_.y - margin;
    
    setPosition(x, y);
    
    std::cout << "[BlockSelectionUI] Positioned at (" << x << ", " << y << ") on screen " 
              << screenWidth << "x" << screenHeight << std::endl;
}

void BlockSelectionUI::updateTextureCoordinates() {
    if (!textureAtlas_) {
        std::cerr << "[BlockSelectionUI] TextureAtlas is null!" << std::endl;
        return;
    }
    
    // Get texture coordinates for the current block type
    auto texCoords = textureAtlas_->getTextureCoordinates(currentBlockType_);
    
    // Convert to the format expected by UITexture (x, y, width, height)
    glm::vec4 uvRect = texCoords.getUVRect();
    setTextureCoords(uvRect);
    
    std::cout << "[BlockSelectionUI] Updated texture coordinates for block type " 
              << static_cast<int>(currentBlockType_) 
              << " to UV(" << uvRect.x << ", " << uvRect.y << ", " << uvRect.z << ", " << uvRect.w << ")" << std::endl;
}

} // namespace UI
} // namespace VoxelEngine
