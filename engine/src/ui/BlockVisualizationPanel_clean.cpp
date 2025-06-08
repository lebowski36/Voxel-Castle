#include "ui/BlockVisualizationPanel.h"
#include "rendering/TextureAtlas.h"
#include "utils/debug_logger_stub.h"
#include <glad/glad.h>
#include <iostream>
#include <algorithm>

namespace VoxelEngine {
namespace UI {

// Static member definitions
UIRenderer* BlockVisualizationPanel::g_currentRenderer = nullptr;
TextureAtlas* BlockVisualizationPanel::g_textureAtlas = nullptr;

BlockVisualizationPanel::BlockVisualizationPanel(float x, float y, float width, float height)
    : UIElement(x, y, width, height),
    selectedCategory_(0),
    selectedBlockType_(VoxelEngine::World::VoxelType::STONE),
    showGridView_(false),
    gridRows_(8),
    gridColumns_(8),
    autoRotation_(true),
    isDragging_(false),
    lastMouseX_(0.0f),
    lastMouseY_(0.0f),
    rotationX_(0.0f),
    rotationY_(0.0f),
    rotationZ_(0.0f),
    blockScale_(150.0f),
    rotationCycleTimer_(0.0f),
    currentRotationAxis_(1),
    rotationSpeed_(45.0f),
    scrollOffset_(0.0f),
    maxScrollOffset_(0.0f) {
    
    initializeBlockCategories();
}

void BlockVisualizationPanel::setCurrentRenderer(UIRenderer* renderer) {
    g_currentRenderer = renderer;
}

void BlockVisualizationPanel::setTextureAtlas(TextureAtlas* atlas) {
    g_textureAtlas = atlas;
}

void BlockVisualizationPanel::render() {
    if (!isVisible() || !g_currentRenderer) return;
    
    // Draw panel background
    g_currentRenderer->renderColoredQuad(
        getAbsolutePosition().x, 
        getAbsolutePosition().y, 
        getSize().x, 
        getSize().y, 
        glm::vec4(0.1f, 0.1f, 0.1f, 0.9f)
    );
    
    renderBlockList();
    renderBlockPreview();
    renderBlockInfo();
}

bool BlockVisualizationPanel::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!clicked) return false;
    
    // Check category clicks first
    float categoryY = getAbsolutePosition().y + 60;
    for (size_t i = 0; i < blockCategories_.size(); ++i) {
        if (mouseX >= getAbsolutePosition().x + 10 && mouseX <= getAbsolutePosition().x + 300 &&
            mouseY >= categoryY && mouseY <= categoryY + 25) {
            
            selectedCategory_ = static_cast<int>(i);
            scrollOffset_ = 0.0f;
            return true;
        }
        categoryY += 30;
    }
    
    // Get blocks for current category
    std::vector<VoxelEngine::World::VoxelType> displayBlocks;
    if (selectedCategory_ >= 0 && selectedCategory_ < static_cast<int>(blockCategories_.size())) {
        displayBlocks = blockCategories_[selectedCategory_].blocks;
    }
    
    // Check block list clicks with scroll offset fix
    float listStartY = getAbsolutePosition().y + 150;
    float listEndY = getAbsolutePosition().y + getSize().y - 50;
    
    if (mouseX >= getAbsolutePosition().x + 10 && mouseX <= getAbsolutePosition().x + 300 &&
        mouseY >= listStartY && mouseY <= listEndY) {
        
        float relativeY = mouseY - listStartY + scrollOffset_;
        int blockIndex = static_cast<int>(relativeY / 40);
        
        if (blockIndex >= 0 && blockIndex < static_cast<int>(displayBlocks.size())) {
            selectedBlockType_ = displayBlocks[blockIndex];
            return true;
        }
    }
    
    return false;
}

bool BlockVisualizationPanel::handleScroll(float x, float y, float scrollDelta) {
    float listStartY = getAbsolutePosition().y + 150;
    float listEndY = getAbsolutePosition().y + getSize().y - 50;
    
    if (x >= getAbsolutePosition().x + 10 && x <= getAbsolutePosition().x + 300 &&
        y >= listStartY && y <= listEndY) {
        
        scrollOffset_ -= scrollDelta * SCROLL_SPEED;
        scrollOffset_ = std::max(0.0f, std::min(scrollOffset_, maxScrollOffset_));
        return true;
    }
    
    return false;
}

void BlockVisualizationPanel::renderBlockList() {
    if (!g_currentRenderer) return;
    
    std::vector<VoxelEngine::World::VoxelType> displayBlocks;
    if (selectedCategory_ >= 0 && selectedCategory_ < static_cast<int>(blockCategories_.size())) {
        displayBlocks = blockCategories_[selectedCategory_].blocks;
    }
    
    // Calculate scroll bounds
    float listHeight = getSize().y - 200;
    float totalBlocksHeight = displayBlocks.size() * 40.0f;
    maxScrollOffset_ = std::max(0.0f, totalBlocksHeight - listHeight);
    scrollOffset_ = std::max(0.0f, std::min(scrollOffset_, maxScrollOffset_));
    
    // Render visible blocks only
    int firstVisibleBlock = static_cast<int>(scrollOffset_ / 40.0f);
    int lastVisibleBlock = std::min(static_cast<int>((scrollOffset_ + listHeight) / 40.0f) + 1, 
                                   static_cast<int>(displayBlocks.size()));
    
    float yPos = getAbsolutePosition().y + 150 - scrollOffset_ + (firstVisibleBlock * 40.0f);
    
    for (int i = firstVisibleBlock; i < lastVisibleBlock; ++i) {
        VoxelEngine::World::VoxelType blockType = displayBlocks[i];
        
        if (blockType == selectedBlockType_) {
            g_currentRenderer->renderColoredQuad(
                getAbsolutePosition().x + 5, yPos - 2, 290, 36,
                glm::vec4(0.3f, 0.3f, 0.8f, 0.3f)
            );
        }
        
        // Simple block representation
        g_currentRenderer->renderColoredQuad(
            getAbsolutePosition().x + 15, yPos + 5, 250, 25,
            glm::vec4(0.8f, 0.8f, 0.8f, 0.1f)
        );
        
        yPos += 40;
    }
    
    // Scroll indicator
    if (maxScrollOffset_ > 0) {
        float scrollbarHeight = listHeight * (listHeight / totalBlocksHeight);
        float scrollbarY = getAbsolutePosition().y + 150 + (scrollOffset_ / maxScrollOffset_) * (listHeight - scrollbarHeight);
        
        g_currentRenderer->renderColoredQuad(
            getAbsolutePosition().x + 305, scrollbarY, 5, scrollbarHeight,
            glm::vec4(0.6f, 0.6f, 0.6f, 0.8f)
        );
    }
}

void BlockVisualizationPanel::renderBlockPreview() {
    if (!g_currentRenderer) return;
    
    float previewX = getAbsolutePosition().x + 350;
    float previewY = getAbsolutePosition().y + 150;
    float previewSize = blockScale_ * 2.0f;
    
    g_currentRenderer->renderColoredQuad(
        previewX, previewY, previewSize, previewSize,
        glm::vec4(0.05f, 0.05f, 0.05f, 1.0f)
    );
    
    // Simple block preview
    glm::vec4 color = getBlockColor(selectedBlockType_);
    g_currentRenderer->renderColoredQuad(
        previewX + previewSize/4, previewY + previewSize/4, 
        previewSize/2, previewSize/2, color
    );
}

void BlockVisualizationPanel::renderBlockInfo() {
    if (!g_currentRenderer) return;
    
    g_currentRenderer->renderColoredQuad(
        getAbsolutePosition().x + 350, getAbsolutePosition().y + 500, 400, 150,
        glm::vec4(0.15f, 0.15f, 0.15f, 1.0f)
    );
}

glm::vec4 BlockVisualizationPanel::getBlockColor(VoxelEngine::World::VoxelType blockType) {
    switch (blockType) {
        case VoxelEngine::World::VoxelType::STONE: return glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        case VoxelEngine::World::VoxelType::DIRT: return glm::vec4(0.6f, 0.4f, 0.2f, 1.0f);
        case VoxelEngine::World::VoxelType::GRASS: return glm::vec4(0.2f, 0.8f, 0.2f, 1.0f);
        case VoxelEngine::World::VoxelType::SAND: return glm::vec4(0.9f, 0.9f, 0.6f, 1.0f);
        case VoxelEngine::World::VoxelType::GRAVEL: return glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
        case VoxelEngine::World::VoxelType::CLAY: return glm::vec4(0.7f, 0.5f, 0.3f, 1.0f);
        case VoxelEngine::World::VoxelType::BEDROCK: return glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        default: return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    }
}

std::string BlockVisualizationPanel::getBlockName(VoxelEngine::World::VoxelType blockType) const {
    const std::vector<std::string> basicNames = {
        "Air", "Stone", "Dirt", "Grass", "Sand", "Gravel", "Clay", "Bedrock", "Topsoil", "Subsoil"
    };
    
    uint8_t blockId = static_cast<uint8_t>(blockType);
    if (blockId < basicNames.size()) {
        return basicNames[blockId];
    }
    
    return "Block " + std::to_string(static_cast<int>(blockType));
}

std::string BlockVisualizationPanel::getFacePatternName(VoxelEngine::World::VoxelType blockType) const {
    return "UNIFORM";
}

void BlockVisualizationPanel::initializeBlockCategories() {
    blockCategories_.clear();
    
    BlockCategory basicTerrain;
    basicTerrain.name = "Basic Terrain (0-9)";
    for (int i = 0; i <= 9; ++i) {
        basicTerrain.blocks.push_back(static_cast<VoxelEngine::World::VoxelType>(i));
    }
    blockCategories_.push_back(basicTerrain);
    
    BlockCategory stoneVarieties;
    stoneVarieties.name = "Stone Varieties (10-19)";
    for (int i = 10; i <= 19; ++i) {
        stoneVarieties.blocks.push_back(static_cast<VoxelEngine::World::VoxelType>(i));
    }
    blockCategories_.push_back(stoneVarieties);
}

void BlockVisualizationPanel::update(float deltaTime) {
    // Update logic here
}

void BlockVisualizationPanel::renderBlockGrid() {
    // Grid view implementation
}

void BlockVisualizationPanel::updateBlockRotation(float deltaTime) {
    // Rotation logic here
}

void BlockVisualizationPanel::renderBlockWithTexture(VoxelEngine::World::VoxelType blockType, float centerX, float centerY, float scale) {
    // Texture rendering implementation
}

} // namespace UI
} // namespace VoxelEngine
