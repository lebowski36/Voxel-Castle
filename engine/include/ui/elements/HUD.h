#pragma once
#include "ui/elements/UITexture.h"
#include "world/voxel_types.h"
#include "rendering/texture_atlas.h"

namespace VoxelEngine {
namespace UI {

/**
 * HUD (Heads Up Display) class
 * 
 * This class manages game HUD elements including block selection UI
 * and other in-game UI elements that appear during gameplay.
 */
class HUD : public UITexture {
public:
    HUD(UIRenderer* renderer, 
        VoxelEngine::Rendering::TextureAtlas* textureAtlas,
        GLuint atlasTextureID);
    virtual ~HUD() = default;

    // Update the displayed block type
    void setCurrentBlockType(VoxelEngine::World::VoxelType blockType);
    VoxelEngine::World::VoxelType getCurrentBlockType() const { return currentBlockType_; }

    // Positioning helpers for centering at bottom of screen
    void centerBottomOfScreen(int screenWidth, int screenHeight, int margin = 50);

private:
    VoxelEngine::Rendering::TextureAtlas* textureAtlas_;
    VoxelEngine::World::VoxelType currentBlockType_;
    
    // Update texture coordinates for block display
    void updateTextureCoordinates();
};

} // namespace UI
} // namespace VoxelEngine
