#pragma once
#include "ui/UIElement.h"
#include "world/voxel_types.h"
#include "world/voxel_face_patterns.h"

// Forward declarations
namespace VoxelEngine { 
    namespace Rendering { 
        class TextureAtlas; 
    } 
}

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
    BlockVisualizationPanel(float x, float y, float width, float height);
    virtual ~BlockVisualizationPanel() = default;

    // UIElement interface
    void update(float deltaTime) override;
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;        bool handleScroll(float x, float y, float scrollDelta);

    // Add scroll wheel support


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
    
    // Texture atlas access
    static void setTextureAtlas(class VoxelEngine::Rendering::TextureAtlas* atlas);    private:
        void initializeBlockCategories();
    // Helper methods
    void renderBlockList();
    void renderBlockPreview();
    void renderBlockInfo();
    void renderBlockGrid();
    void updateBlockRotation(float deltaTime);
    glm::vec4 getBlockColor(VoxelEngine::World::VoxelType blockType);
    std::string getBlockName(VoxelEngine::World::VoxelType blockType) const;
    std::string getFacePatternName(VoxelEngine::World::VoxelType blockType) const;
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
    float rotationAngleZ_{0.0f};
    float rotationSpeed_{50.0f}; // degrees per second
    
    // Multi-axis rotation cycle state
    enum class RotationAxis { X, Y, Z, COMBINED };
    RotationAxis currentRotationAxis_{RotationAxis::Y};
    float axisRotationTime_{0.0f};
    float timePerAxis_{3.0f}; // seconds per axis
    int gridColumns_{4};
    int gridRows_{4};
    
    // Visualization settings
    float blockScale_{150.0f};  // Increased from 100.0f for bigger cube rendering
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
    
    // Scrolling support for block list
    float scrollOffset_;
    float maxScrollOffset_;
    static constexpr float SCROLL_SPEED = 40.0f;
    
    // Core state
    int selectedCategory_;
    VoxelEngine::World::VoxelType selectedBlockType_;
    
    // Visual properties
    bool showGridView_;

    
    // Initialize block categories

};

} // namespace UI
} // namespace VoxelEngine
