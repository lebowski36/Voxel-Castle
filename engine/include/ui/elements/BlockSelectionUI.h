#pragma once
#include "ui/elements/UITexture.h"
#include "world/voxel_types.h"
#include "rendering/texture_atlas.h"

namespace VoxelEngine {
namespace UI {

class BlockSelectionUI : public UITexture {
public:
    BlockSelectionUI(UIRenderer* renderer, 
                    VoxelEngine::Rendering::TextureAtlas* textureAtlas,
                    GLuint atlasTextureID);
    virtual ~BlockSelectionUI() = default;

    // Update the displayed block type
    void setCurrentBlockType(VoxelEngine::World::VoxelType blockType);
    VoxelEngine::World::VoxelType getCurrentBlockType() const { return currentBlockType_; }

    // Positioning helpers for centering at bottom of screen
    void centerBottomOfScreen(int screenWidth, int screenHeight, int margin = 50);

private:
    VoxelEngine::Rendering::TextureAtlas* textureAtlas_;
    VoxelEngine::World::VoxelType currentBlockType_;
    
    void updateTextureCoordinates();
};

} // namespace UI
} // namespace VoxelEngine
