#include "ui/BlockVisualizationPanel.h"
#include "ui/UIRenderer.h"
#include "ui/UILogger.h"
#include "rendering/texture_atlas.h"
#include "utils/debug_logger_stub.h"
#include <algorithm>
#include <cmath>

namespace VoxelEngine {
namespace UI {

static UIRenderer* g_currentRenderer = nullptr;
static VoxelEngine::Rendering::TextureAtlas* g_textureAtlas = nullptr;

void BlockVisualizationPanel::setCurrentRenderer(UIRenderer* renderer) {
    g_currentRenderer = renderer;
}

void BlockVisualizationPanel::setTextureAtlas(VoxelEngine::Rendering::TextureAtlas* atlas) {
    g_textureAtlas = atlas;
}

BlockVisualizationPanel::BlockVisualizationPanel() {
    setPosition(10.0f, 850.0f);
    setSize(300.0f, 200.0f);
    initializeBlockCategories();
}

void BlockVisualizationPanel::update(float deltaTime) {
    // No complex updates needed
    if (!isVisible()) return;
}

void BlockVisualizationPanel::render() {
    if (!isVisible() || !g_currentRenderer) return;
    
    glm::vec2 pos = getAbsolutePosition();
    glm::vec2 size = getSize();
    
    // Background
    g_currentRenderer->renderColoredQuad(pos.x, pos.y, size.x, size.y, 
                                         glm::vec4(0.05f, 0.05f, 0.05f, 0.95f));
    
    // Title bar
    g_currentRenderer->renderColoredQuad(pos.x, pos.y, size.x, 40.0f,
                                         glm::vec4(0.15f, 0.15f, 0.2f, 1.0f));
    
    // Title
    uint8_t blockId = static_cast<uint8_t>(currentBlock_);
    std::string title = "Block: " + std::to_string(blockId);
    g_currentRenderer->drawText(title, pos.x + 10.0f, pos.y + 8.0f, 1.2f,
                               glm::vec3(1.0f, 1.0f, 1.0f));
    
    // Content area
    float content_y = pos.y + 50.0f;
    float content_height = size.y - 80.0f;
    
    // Block preview box (left side)
    float preview_size = 80.0f;
    float preview_x = pos.x + 15.0f;
    float preview_y = content_y + 10.0f;
    
    g_currentRenderer->renderColoredQuad(preview_x, preview_y, preview_size, preview_size,
                                         glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
    
    // Color the box based on block type
    float hue = (blockId * 137.5f) / 360.0f;
    float r = 0.5f + 0.5f * std::sin(hue * 6.28f);
    float g = 0.5f + 0.5f * std::sin((hue + 0.33f) * 6.28f);
    float b = 0.5f + 0.5f * std::sin((hue + 0.66f) * 6.28f);
    
    g_currentRenderer->renderColoredQuad(preview_x + 5.0f, preview_y + 5.0f, 
                                         preview_size - 10.0f, preview_size - 10.0f,
                                         glm::vec4(r, g, b, 1.0f));
    
    // Block name (center area)
    float name_x = preview_x + preview_size + 20.0f;
    std::string blockName = getBlockName(currentBlock_);
    g_currentRenderer->drawText(blockName, name_x, content_y + 20.0f, 0.95f,
                               glm::vec3(0.9f, 0.9f, 0.9f));
    
    // Controls at bottom
    float button_y = pos.y + size.y - 35.0f;
    float button_width = 60.0f;
    float button_height = 25.0f;
    
    // Prev button
    g_currentRenderer->renderColoredQuad(pos.x + 20.0f, button_y, button_width, button_height,
                                        glm::vec4(0.2f, 0.5f, 0.2f, 1.0f));
    g_currentRenderer->drawText("Prev", pos.x + 25.0f, button_y + 3.0f, 1.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f));
    
    // Next button
    g_currentRenderer->renderColoredQuad(pos.x + size.x - 80.0f, button_y, button_width, button_height,
                                        glm::vec4(0.2f, 0.5f, 0.2f, 1.0f));
    g_currentRenderer->drawText("Next", pos.x + size.x - 75.0f, button_y + 3.0f, 1.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f));
}

bool BlockVisualizationPanel::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!isVisible() || !clicked) return false;
    
    glm::vec2 pos = getAbsolutePosition();
    glm::vec2 size = getSize();
    
    // Check if inside panel
    if (mouseX < pos.x || mouseX > pos.x + size.x ||
        mouseY < pos.y || mouseY > pos.y + size.y) {
        return false;
    }
    
    // Check buttons
    float button_y = pos.y + size.y - 35.0f;
    float button_width = 60.0f;
    float button_height = 25.0f;
    
    // Prev button
    if (mouseX >= pos.x + 20.0f && mouseX <= pos.x + 20.0f + button_width &&
        mouseY >= button_y && mouseY <= button_y + button_height) {
        selectPreviousBlock();
        return true;
    }
    
    // Next button
    if (mouseX >= pos.x + size.x - 80.0f && mouseX <= pos.x + size.x - 20.0f &&
        mouseY >= button_y && mouseY <= button_y + button_height) {
        selectNextBlock();
        return true;
    }
    
    return false;
}

void BlockVisualizationPanel::setVisible(bool visible) {
    UIElement::setVisible(visible);
}

void BlockVisualizationPanel::selectBlock(VoxelType blockType) {
    currentBlock_ = blockType;
}

void BlockVisualizationPanel::selectNextBlock() {
    uint8_t blockId = static_cast<uint8_t>(currentBlock_);
    blockId = (blockId + 1) % 256;
    currentBlock_ = static_cast<VoxelType>(blockId);
}

void BlockVisualizationPanel::selectPreviousBlock() {
    uint8_t blockId = static_cast<uint8_t>(currentBlock_);
    blockId = (blockId == 0) ? 255 : blockId - 1;
    currentBlock_ = static_cast<VoxelType>(blockId);
}

void BlockVisualizationPanel::selectBlockByCategory(int categoryIndex) {
    if (categoryIndex >= 0 && categoryIndex < static_cast<int>(blockCategories_.size())) {
        if (!blockCategories_[categoryIndex].blocks.empty()) {
            selectBlock(blockCategories_[categoryIndex].blocks[0]);
        }
    }
}

std::string BlockVisualizationPanel::getBlockName(VoxelType blockType) const {
    uint8_t id = static_cast<uint8_t>(blockType);
    
    // Simple list of block names
    static const char* blockNames[] = {
        "Air", "Stone", "Dirt", "Grass", "Sand", "Gravel", "Clay", "Bedrock", "Topsoil", "Subsoil",
        "Granite", "Limestone", "Marble", "Sandstone", "Slate", "Basalt", "Quartzite", "Obsidian", "Pumice", "Shale",
        "Coal Ore", "Iron Ore", "Copper Ore", "Tin Ore", "Silver Ore", "Gold Ore", "Ruby", "Sapphire", "Emerald", "Diamond",
        "Oak Wood", "Pine Wood", "Birch Wood", "Mahogany Wood", "Oak Leaves", "Pine Needles", "Birch Leaves", "Fronds", "Mushroom", "Fungi",
        "Snow", "Ice", "Packed Ice", "Cactus", "Vine", "Pink Coral", "Blue Coral", "Seaweed", "Moss", "Rock",
        "Water", "Lava", "Oil", "Acid", "Honey", "Steam", "Toxic Gas", "Natural Gas", "Mist", "Smoke"
    };
    
    if (id < 60) {
        return blockNames[id];
    }
    
    return "Block " + std::to_string(id);
}

void BlockVisualizationPanel::initializeBlockCategories() {
    blockCategories_.clear();
    
    BlockCategory basic;
    basic.name = "Basic";
    for (int i = 0; i < 10; i++) {
        basic.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(basic);
    
    BlockCategory stones;
    stones.name = "Stones";
    for (int i = 10; i < 20; i++) {
        stones.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(stones);
    
    BlockCategory ores;
    ores.name = "Ores";
    for (int i = 20; i < 30; i++) {
        ores.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(ores);
    
    BlockCategory organic;
    organic.name = "Organic";
    for (int i = 30; i < 40; i++) {
        organic.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(organic);
    
    BlockCategory biome;
    biome.name = "Biome";
    for (int i = 40; i < 50; i++) {
        biome.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(biome);
    
    BlockCategory fluids;
    fluids.name = "Fluids";
    for (int i = 50; i < 60; i++) {
        fluids.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(fluids);
    
    selectBlock(VoxelType::STONE);
}

} // namespace UI
} // namespace VoxelEngine
