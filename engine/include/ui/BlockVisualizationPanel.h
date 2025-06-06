#pragma once
#include "ui/UIElement.h"
#include "world/voxel_types.h"
#include "world/voxel_face_patterns.h"

// Use these namespaces for clarity
using VoxelEngine::World::VoxelType;
using VoxelEngine::World::FacePattern;
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

/**
 * Block Visualization Debug UI Panel
 * 
 * A developer tool for visualizing and inspecting all available block types
 * in the game. Allows rotating blocks to view different faces and showing
 * detailed information about each block type.
 */
class BlockVisualizationPanel : public UIElement {
public:
    BlockVisualizationPanel();
    virtual ~BlockVisualizationPanel() = default;

    // UIElement interface
    void update(float deltaTime) override;
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

    // Configuration
    void setVisible(bool visible);
    void toggleVisibility() { setVisible(!isVisible()); }
    void setBlockRotationSpeed(float speed) { rotationSpeed_ = speed; }
    
    // Block selection
    void selectBlock(VoxelType blockType);
    void selectNextBlock();
    void selectPreviousBlock();
    void selectBlockByCategory(int categoryIndex);

    // Display options
    void setShowBlockInfo(bool show) { showBlockInfo_ = show; }
    void setShowBlockList(bool show) { showBlockList_ = show; }
    void setShowFacePatternInfo(bool show) { showFacePatternInfo_ = show; }
    void setAutoRotate(bool autoRotate) { autoRotate_ = autoRotate; }
    
    // Grid view options
    void toggleGridView() { gridViewEnabled_ = !gridViewEnabled_; }
    void setGridSize(int columns, int rows) { gridColumns_ = columns; gridRows_ = rows; }
    
    // Static method to set current renderer (called by UISystem)
    static void setCurrentRenderer(class UIRenderer* renderer);

private:
    // Helper methods
    void renderBlockList();
    void renderBlockPreview();
    void renderBlockInfo();
    void renderBlockGrid();
    void updateBlockRotation(float deltaTime);
    std::string getBlockName(VoxelType blockType) const;
    std::string getFacePatternName(VoxelType blockType) const;
    glm::vec2 getBlockListPosition() const;
    glm::vec2 getBlockPreviewPosition() const;
    glm::vec2 getBlockInfoPosition() const;
    
    // UI state
    bool initialized_{false};
    bool showBlockInfo_{true};
    bool showBlockList_{true};
    bool showFacePatternInfo_{true};
    bool gridViewEnabled_{false};
    bool autoRotate_{true};
    
    // Block rendering state
    VoxelType currentBlock_{VoxelType::STONE};
    int categoryFilter_{-1}; // -1 = all categories
    float rotationAngleX_{0.0f};
    float rotationAngleY_{0.0f};
    float rotationSpeed_{50.0f}; // degrees per second
    int gridColumns_{4};
    int gridRows_{4};
    
    // Visualization settings
    float blockScale_{100.0f};
    float blockListWidth_{200.0f};
    float blockInfoHeight_{150.0f};
    
    // Categorized block lists for organization
    struct BlockCategory {
        std::string name;
        std::vector<VoxelType> blocks;
    };
    std::vector<BlockCategory> blockCategories_;
    
    // Mouse interaction
    bool isDragging_{false};
    glm::vec2 lastMousePos_{0.0f, 0.0f};
    
    // Initialize block categories
    void initializeBlockCategories();
};

} // namespace UI
} // namespace VoxelEngine
