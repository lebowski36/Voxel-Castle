#include "ui/BlockVisualizationPanel.h"
#include "ui/UIRenderer.h"
#include "ui/UILogger.h"
#include "../../game/include/utils/debug_logger.h"
#include <algorithm>
#include <sstream>
#include <glad/glad.h>

namespace VoxelEngine {
namespace UI {

// Static renderer pointer for accessing the current renderer
static UIRenderer* g_currentRenderer = nullptr;

void BlockVisualizationPanel::setCurrentRenderer(UIRenderer* renderer) {
    g_currentRenderer = renderer;
}

BlockVisualizationPanel::BlockVisualizationPanel() {
    // Initialize default position and size
    setPosition(50.0f, 50.0f);
    setSize(800.0f, 600.0f);
    
    // Initialize categories when first created
    initializeBlockCategories();
}

void BlockVisualizationPanel::update(float deltaTime) {
    if (!isVisible()) return;
    
    // Update block rotation if auto-rotate is enabled
    if (autoRotate_) {
        updateBlockRotation(deltaTime);
    }
}

void BlockVisualizationPanel::render() {
    if (!isVisible()) return;
    
    // Check if renderer is available
    if (!g_currentRenderer) {
        return; // Cannot render without renderer
    }
    
    // Draw panel background
    g_currentRenderer->renderColoredQuad(
        getAbsolutePosition().x, 
        getAbsolutePosition().y, 
        getSize().x, 
        getSize().y, 
        glm::vec4(0.1f, 0.1f, 0.1f, 0.9f)
    );
    
    // Draw title bar
    g_currentRenderer->renderColoredQuad(
        getAbsolutePosition().x, 
        getAbsolutePosition().y, 
        getSize().x, 
        40.0f, 
        glm::vec4(0.2f, 0.2f, 0.3f, 1.0f)
    );
    
    // Draw title text
    g_currentRenderer->drawText(
        "Block Visualization Debug Panel",
        getAbsolutePosition().x + 10.0f,
        getAbsolutePosition().y + 10.0f,
        1.0f,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
    
    // Draw UI components based on current view mode
    if (gridViewEnabled_) {
        renderBlockGrid();
    } else {
        if (showBlockList_) {
            renderBlockList();
        }
        renderBlockPreview();
        if (showBlockInfo_) {
            renderBlockInfo();
        }
    }
}

bool BlockVisualizationPanel::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!isVisible()) return false;
    
    // Check if mouse is inside panel
    if (!containsPoint(mouseX, mouseY)) {
        isDragging_ = false;
        return false;
    }
    
    // Handle mouse drag for block rotation
    glm::vec2 absPos = getAbsolutePosition();
    glm::vec2 previewPos = getBlockPreviewPosition();
    float previewSize = blockScale_ * 1.5f;
    
    bool inPreviewArea = 
        mouseX >= absPos.x + previewPos.x && 
        mouseX <= absPos.x + previewPos.x + previewSize &&
        mouseY >= absPos.y + previewPos.y && 
        mouseY <= absPos.y + previewPos.y + previewSize;
        
    if (clicked && inPreviewArea) {
        if (!isDragging_) {
            isDragging_ = true;
            lastMousePos_ = glm::vec2(mouseX, mouseY);
        }
    } else if (!clicked) {
        isDragging_ = false;
    }
    
    if (isDragging_) {
        glm::vec2 currentMousePos(mouseX, mouseY);
        glm::vec2 delta = currentMousePos - lastMousePos_;
        
        // Apply rotation based on mouse movement
        rotationAngleY_ += delta.x * 0.5f;
        rotationAngleX_ += delta.y * 0.5f;
        
        // Keep angles in reasonable range
        rotationAngleX_ = std::fmod(rotationAngleX_, 360.0f);
        rotationAngleY_ = std::fmod(rotationAngleY_, 360.0f);
        
        lastMousePos_ = currentMousePos;
        autoRotate_ = false; // Disable auto-rotation when manually rotating
    }
    
    // Handle list selection clicks
    if (clicked && showBlockList_) {
        glm::vec2 listPos = getBlockListPosition();
        if (mouseX >= absPos.x + listPos.x && 
            mouseX <= absPos.x + listPos.x + blockListWidth_ &&
            mouseY >= absPos.y + listPos.y + 30.0f) {
            
            float relY = mouseY - (absPos.y + listPos.y + 30.0f);
            int itemHeight = 24;
            
            // Calculate which category and item was clicked
            int itemIndex = static_cast<int>(relY / itemHeight);
            int categoryStartIndex = 0;
            
            for (size_t i = 0; i < blockCategories_.size(); i++) {
                // +1 for category header
                int categorySize = blockCategories_[i].blocks.size() + 1;
                
                if (itemIndex >= categoryStartIndex && itemIndex < categoryStartIndex + categorySize) {
                    int relativeIndex = itemIndex - categoryStartIndex;
                    
                    if (relativeIndex == 0) {
                        // Category header clicked - toggle filter
                        categoryFilter_ = (categoryFilter_ == static_cast<int>(i)) ? -1 : static_cast<int>(i);
                    } else if (relativeIndex - 1 < blockCategories_[i].blocks.size()) {
                        // Block item clicked
                        selectBlock(blockCategories_[i].blocks[relativeIndex - 1]);
                    }
                    break;
                }
                
                categoryStartIndex += categorySize;
            }
        }
    }
    
    return true;
}

void BlockVisualizationPanel::setVisible(bool visible) {
    // Call parent class implementation
    UIElement::setVisible(visible);
    
    if (visible && !initialized_) {
        initializeBlockCategories();
        initialized_ = true;
    }
}

void BlockVisualizationPanel::selectBlock(VoxelType blockType) {
    currentBlock_ = blockType;
    rotationAngleX_ = 0.0f;
    rotationAngleY_ = 0.0f;
}

void BlockVisualizationPanel::selectNextBlock() {
    // Convert to integer for easy increment, then back to VoxelType
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
            categoryFilter_ = categoryIndex;
        }
    }
}

void BlockVisualizationPanel::renderBlockList() {
    if (!g_currentRenderer) return;
    
    glm::vec2 absPos = getAbsolutePosition();
    glm::vec2 listPos = getBlockListPosition();
    
    // Draw list background
    g_currentRenderer->renderColoredQuad(
        absPos.x + listPos.x,
        absPos.y + listPos.y,
        blockListWidth_,
        getSize().y - listPos.y - 10.0f,
        glm::vec4(0.15f, 0.15f, 0.15f, 1.0f)
    );
    
    // Draw list title
    g_currentRenderer->renderColoredQuad(
        absPos.x + listPos.x,
        absPos.y + listPos.y,
        blockListWidth_,
        30.0f,
        glm::vec4(0.2f, 0.3f, 0.4f, 1.0f)
    );
    
    g_currentRenderer->drawText(
        "Block Types",
        absPos.x + listPos.x + 10.0f,
        absPos.y + listPos.y + 5.0f,
        0.8f,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
    
    // Draw block list items
    float yOffset = listPos.y + 30.0f;
    int itemHeight = 24;
    
    for (size_t i = 0; i < blockCategories_.size(); i++) {
        bool isCategoryFiltered = (categoryFilter_ >= 0 && categoryFilter_ != static_cast<int>(i));
        
        // Draw category header
        g_currentRenderer->renderColoredQuad(
            absPos.x + listPos.x,
            absPos.y + yOffset,
            blockListWidth_,
            static_cast<float>(itemHeight),
            glm::vec4(0.25f, 0.25f, 0.3f, 1.0f)
        );
        
        g_currentRenderer->drawText(
            blockCategories_[i].name,
            absPos.x + listPos.x + 5.0f,
            absPos.y + yOffset + 2.0f,
            0.7f,
            glm::vec3(1.0f, 1.0f, 0.8f)
        );
        
        yOffset += itemHeight;
        
        // Skip block items if category is filtered out
        if (isCategoryFiltered) {
            continue;
        }
        
        // Draw block items
        for (VoxelType block : blockCategories_[i].blocks) {
            bool isSelected = (block == currentBlock_);
            
            g_currentRenderer->renderColoredQuad(
                absPos.x + listPos.x,
                absPos.y + yOffset,
                blockListWidth_,
                static_cast<float>(itemHeight),
                isSelected ? glm::vec4(0.3f, 0.5f, 0.7f, 1.0f) : glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)
            );
            
            uint8_t blockId = static_cast<uint8_t>(block);
            std::string blockText = std::to_string(blockId) + ": " + getBlockName(block);
            
            g_currentRenderer->drawText(
                blockText,
                absPos.x + listPos.x + 15.0f,
                absPos.y + yOffset + 2.0f,
                0.7f,
                isSelected ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.8f, 0.8f, 0.8f)
            );
            
            yOffset += itemHeight;
        }
    }
}

void BlockVisualizationPanel::renderBlockPreview() {
    if (!g_currentRenderer) return;
    
    glm::vec2 absPos = getAbsolutePosition();
    glm::vec2 previewPos = getBlockPreviewPosition();
    
    // Draw preview background
    float previewAreaSize = blockScale_ * 2.0f;
    g_currentRenderer->renderColoredQuad(
        absPos.x + previewPos.x - blockScale_ * 0.25f,
        absPos.y + previewPos.y - blockScale_ * 0.25f,
        previewAreaSize,
        previewAreaSize,
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)
    );
    
    // Draw block using atlas textures
    uint8_t blockId = static_cast<uint8_t>(currentBlock_);
    
    // TODO: Replace with actual 3D block rendering using new texture atlas
    // For now, just draw a placeholder with the block ID
    float boxSize = blockScale_;
    
    // Draw a cube with different faces colored differently to simulate 3D
    glm::vec4 faceColors[6] = {
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Front
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // Back
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // Top
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // Bottom
        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), // Left
        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)  // Right
    };
    
    // Apply rotation
    // This is a very simplified simulation of 3D rotation
    float angleX = glm::radians(rotationAngleX_);
    float angleY = glm::radians(rotationAngleY_);
    
    float offsetX = std::sin(angleY) * boxSize * 0.3f;
    float offsetY = std::sin(angleX) * boxSize * 0.3f;
    
    // Draw a simple box with different colored sides
    g_currentRenderer->renderColoredQuad(
        absPos.x + previewPos.x + offsetX,
        absPos.y + previewPos.y + offsetY,
        boxSize - std::abs(offsetX) * 0.5f,
        boxSize - std::abs(offsetY) * 0.5f,
        faceColors[(static_cast<int>(rotationAngleX_ / 90) + static_cast<int>(rotationAngleY_ / 90)) % 6]
    );
    
    // Draw block ID in the center
    std::string blockIdText = std::to_string(blockId);
    g_currentRenderer->drawText(
        blockIdText,
        absPos.x + previewPos.x + boxSize * 0.5f - blockIdText.length() * 4,
        absPos.y + previewPos.y + boxSize * 0.5f - 10,
        1.0f,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
    
    // Draw rotation guidance
    g_currentRenderer->drawText(
        "Drag to rotate",
        absPos.x + previewPos.x,
        absPos.y + previewPos.y + boxSize + 20,
        0.7f,
        glm::vec3(0.8f, 0.8f, 0.8f)
    );
    
    // Draw auto-rotate status
    std::string rotateStatus = autoRotate_ ? "Auto-rotate: ON" : "Auto-rotate: OFF";
    g_currentRenderer->drawText(
        rotateStatus,
        absPos.x + previewPos.x,
        absPos.y + previewPos.y + boxSize + 40,
        0.7f,
        autoRotate_ ? glm::vec3(0.2f, 0.9f, 0.2f) : glm::vec3(0.9f, 0.2f, 0.2f)
    );
}

void BlockVisualizationPanel::renderBlockInfo() {
    if (!g_currentRenderer) return;
    
    glm::vec2 absPos = getAbsolutePosition();
    glm::vec2 infoPos = getBlockInfoPosition();
    
    // Draw info panel background
    g_currentRenderer->renderColoredQuad(
        absPos.x + infoPos.x,
        absPos.y + infoPos.y,
        getSize().x - infoPos.x - 10.0f,
        blockInfoHeight_,
        glm::vec4(0.15f, 0.15f, 0.15f, 1.0f)
    );
    
    // Get block information
    uint8_t blockId = static_cast<uint8_t>(currentBlock_);
    std::string blockName = getBlockName(currentBlock_);
    std::string patternName = getFacePatternName(currentBlock_);
    
    // Draw block information text
    float yPos = absPos.y + infoPos.y + 10.0f;
    float lineHeight = 25.0f;
    
    g_currentRenderer->drawText(
        "Block ID: " + std::to_string(blockId),
        absPos.x + infoPos.x + 10.0f,
        yPos,
        0.8f,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
    yPos += lineHeight;
    
    g_currentRenderer->drawText(
        "Name: " + blockName,
        absPos.x + infoPos.x + 10.0f,
        yPos,
        0.8f,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
    yPos += lineHeight;
    
    if (showFacePatternInfo_) {
        g_currentRenderer->drawText(
            "Face Pattern: " + patternName,
            absPos.x + infoPos.x + 10.0f,
            yPos,
            0.8f,
            glm::vec3(1.0f, 1.0f, 1.0f)
        );
        yPos += lineHeight;
    }
    
    // TODO: Add more block properties here as the BlockProperties system is implemented
}

void BlockVisualizationPanel::renderBlockGrid() {
    if (!g_currentRenderer) return;
    
    glm::vec2 absPos = getAbsolutePosition();
    float gridAreaWidth = getSize().x - 20.0f;
    float gridAreaHeight = getSize().y - 60.0f;
    
    // Calculate cell size and spacing
    float cellSize = std::min(
        (gridAreaWidth - (static_cast<float>(gridColumns_) - 1) * 10.0f) / static_cast<float>(gridColumns_),
        (gridAreaHeight - (static_cast<float>(gridRows_) - 1) * 10.0f) / static_cast<float>(gridRows_)
    );
    
    // Calculate start position to center the grid
    float startX = absPos.x + (getSize().x - (cellSize * static_cast<float>(gridColumns_) + 10.0f * (static_cast<float>(gridColumns_) - 1))) / 2.0f;
    float startY = absPos.y + 50.0f;
    
    // Draw grid title
    g_currentRenderer->drawText(
        "Block Grid View",
        absPos.x + 10.0f,
        absPos.y + 45.0f,
        1.0f,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
    
    // Draw grid cells with blocks
    int visibleBlockCount = 0;
    
    // Filter blocks by category if a filter is active
    std::vector<VoxelType> visibleBlocks;
    if (categoryFilter_ >= 0 && categoryFilter_ < static_cast<int>(blockCategories_.size())) {
        visibleBlocks = blockCategories_[categoryFilter_].blocks;
    } else {
        // Collect all blocks across categories
        for (const auto& category : blockCategories_) {
            visibleBlocks.insert(visibleBlocks.end(), category.blocks.begin(), category.blocks.end());
        }
    }
    
    // Limit blocks to what can fit in the grid
    int maxVisibleBlocks = gridRows_ * gridColumns_;
    
    for (int row = 0; row < gridRows_; row++) {
        for (int col = 0; col < gridColumns_; col++) {
            int index = row * gridColumns_ + col;
            
            if (index >= static_cast<int>(visibleBlocks.size())) {
                break;
            }
            
            VoxelType blockType = visibleBlocks[index];
            uint8_t blockId = static_cast<uint8_t>(blockType);
            
            // Calculate cell position
            float cellX = startX + static_cast<float>(col) * (cellSize + 10.0f);
            float cellY = startY + static_cast<float>(row) * (cellSize + 10.0f);
            
            // Draw cell background
            g_currentRenderer->renderColoredQuad(
                cellX,
                cellY,
                cellSize,
                cellSize,
                blockType == currentBlock_ ? 
                    glm::vec4(0.3f, 0.5f, 0.7f, 1.0f) : 
                    glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)
            );
            
            // Draw block ID
            g_currentRenderer->drawText(
                std::to_string(blockId),
                cellX + 5.0f,
                cellY + 5.0f,
                0.7f,
                glm::vec3(1.0f, 1.0f, 1.0f)
            );
            
            // Draw simplified block (colored square)
            float blockSize = cellSize * 0.6f;
            float blockX = cellX + (cellSize - blockSize) / 2.0f;
            float blockY = cellY + (cellSize - blockSize) / 2.0f;
            
            // Get a color based on block type
            uint8_t r = (blockId * 67) % 255;
            uint8_t g = (blockId * 97) % 255;
            uint8_t b = (blockId * 157) % 255;
            
            g_currentRenderer->renderColoredQuad(
                blockX,
                blockY,
                blockSize,
                blockSize,
                glm::vec4(r/255.0f, g/255.0f, b/255.0f, 1.0f)
            );
            
            visibleBlockCount++;
        }
    }
}

void BlockVisualizationPanel::updateBlockRotation(float deltaTime) {
    if (!autoRotate_) return;
    
    // Rotate the block slowly
    rotationAngleY_ += rotationSpeed_ * deltaTime;
    if (rotationAngleY_ >= 360.0f) {
        rotationAngleY_ -= 360.0f;
    }
}

std::string BlockVisualizationPanel::getBlockName(VoxelType blockType) const {
    // Get block name based on VoxelType enum
    // This should be expanded when the Block Properties system is implemented
    switch (blockType) {
        case VoxelType::AIR: return "Air";
        case VoxelType::STONE: return "Stone";
        case VoxelType::DIRT: return "Dirt";
        case VoxelType::GRASS: return "Grass";
        case VoxelType::WOOD: return "Wood";
        case VoxelType::LEAVES: return "Leaves";
        case VoxelType::WATER: return "Water";
        case VoxelType::SAND: return "Sand";
        default: {
            // For new extended block types
            uint8_t blockId = static_cast<uint8_t>(blockType);
            if (blockId >= 8 && blockId < 256) {
                // Try to find a matching block in categories
                for (const auto& category : blockCategories_) {
                    for (VoxelType block : category.blocks) {
                        if (block == blockType) {
                            // Extract name from category
                            std::string blockName;
                            uint8_t id = static_cast<uint8_t>(block);
                            
                            // Check different block type ranges
                            if (id <= 9) {
                                const char* basicNames[] = {"Air", "Stone", "Dirt", "Grass", "Sand", 
                                                          "Gravel", "Clay", "Bedrock", "Topsoil", "Subsoil"};
                                blockName = basicNames[id];
                            } else if (id >= 10 && id <= 19) {
                                const char* stoneNames[] = {"Granite", "Limestone", "Marble", "Sandstone", 
                                                          "Slate", "Basalt", "Quartzite", "Obsidian", "Pumice", "Shale"};
                                blockName = stoneNames[id - 10];
                            } else if (id >= 20 && id <= 29) {
                                if (id <= 25) {
                                    const char* oreNames[] = {"Coal Ore", "Iron Ore", "Copper Ore", 
                                                           "Tin Ore", "Silver Ore", "Gold Ore"};
                                    blockName = oreNames[id - 20];
                                } else {
                                    const char* gemNames[] = {"Ruby", "Sapphire", "Emerald", "Diamond"};
                                    blockName = gemNames[id - 26];
                                }
                            } else {
                                // Default name for other blocks
                                blockName = "Block";
                            }
                            
                            return blockName;
                        }
                    }
                }
                
                // If not found in categories
                return "Extended Block";
            }
            return "Unknown";
        }
    }
}

std::string BlockVisualizationPanel::getFacePatternName(VoxelType blockType) const {
    // This should use the actual FacePattern from voxel_face_patterns.h
    uint8_t blockId = static_cast<uint8_t>(blockType);
    
    // This is a placeholder implementation - should be replaced with actual face pattern lookup
    if (blockType == VoxelType::GRASS) {
        return "TOP_UNIQUE";
    } else if (blockType == VoxelType::WOOD) {
        return "END_UNIQUE";
    } else {
        return "UNIFORM";
    }
}

glm::vec2 BlockVisualizationPanel::getBlockListPosition() const {
    return glm::vec2(10.0f, 50.0f);
}

glm::vec2 BlockVisualizationPanel::getBlockPreviewPosition() const {
    float centerX = (getSize().x + blockListWidth_) * 0.5f;
    float centerY = (getSize().y - blockInfoHeight_) * 0.5f;
    return glm::vec2(centerX - blockScale_ * 0.5f, centerY - blockScale_ * 0.5f);
}

glm::vec2 BlockVisualizationPanel::getBlockInfoPosition() const {
    return glm::vec2(blockListWidth_ + 20.0f, getSize().y - blockInfoHeight_ - 10.0f);
}

void BlockVisualizationPanel::initializeBlockCategories() {
    blockCategories_.clear();
    
    // Basic Terrain (0-9)
    BlockCategory basicTerrain;
    basicTerrain.name = "Basic Terrain (0-9)";
    for (int i = 0; i <= 9; i++) {
        basicTerrain.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(basicTerrain);
    
    // Stone Varieties (10-19)
    BlockCategory stoneVarieties;
    stoneVarieties.name = "Stone Varieties (10-19)";
    for (int i = 10; i <= 19; i++) {
        stoneVarieties.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(stoneVarieties);
    
    // Ores & Minerals (20-29)
    BlockCategory oresMinerals;
    oresMinerals.name = "Ores & Minerals (20-29)";
    for (int i = 20; i <= 29; i++) {
        oresMinerals.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(oresMinerals);
    
    // Organic Natural (30-39)
    BlockCategory organicNatural;
    organicNatural.name = "Organic Natural (30-39)";
    for (int i = 30; i <= 39; i++) {
        organicNatural.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(organicNatural);
    
    // Biome Specific (40-49)
    BlockCategory biomeSpecific;
    biomeSpecific.name = "Biome Specific (40-49)";
    for (int i = 40; i <= 49; i++) {
        biomeSpecific.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(biomeSpecific);
    
    // Fluids & Gases (50-59)
    BlockCategory fluidsGases;
    fluidsGases.name = "Fluids & Gases (50-59)";
    for (int i = 50; i <= 59; i++) {
        fluidsGases.blocks.push_back(static_cast<VoxelType>(i));
    }
    blockCategories_.push_back(fluidsGases);
    
    // Initialize with the first block
    selectBlock(VoxelType::STONE);
}

} // namespace UI
} // namespace VoxelEngine
