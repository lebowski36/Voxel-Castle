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
    virtual ~BlockVisualizationPanel() = default;

    // UIElement interface
    void update(float deltaTime) override;
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

    // Configuration
    void setVisible(bool visible);
    void toggleVisibility() { setVisible(!isVisible()); }
    
    // Block selection
    void selectBlock(VoxelType blockType);
    void selectNextBlock();
    void selectPreviousBlock();
    
    // Static method to set current renderer (called by UISystem)
    static void setCurrentRenderer(class UIRenderer* renderer);
    
    // Texture atlas access
    static void setTextureAtlas(class VoxelEngine::Rendering::TextureAtlas* atlas);

private:
    void initializeBlockCategories();
    std::string getBlockName(uint8_t blockId) const;
    
    // UI state
    VoxelType currentBlock_{VoxelType::STONE};
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
