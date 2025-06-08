#pragma once
#include "ui/UIRenderer.h"
#include "ui/UIElement.h"
#include "ui/BlockVisualizationPanel.h"
#include <memory>
#include <vector>

namespace VoxelEngine {
namespace UI {

class UISystem {
public:
    UISystem();
    ~UISystem();

    bool initialize(int screenWidth, int screenHeight, const std::string& projectRoot = "");
    void shutdown();
    
    void update(float deltaTime);
    virtual void render();
    
    void setScreenSize(int width, int height);
    
    // Element management
    void addElement(std::shared_ptr<UIElement> element);
    void removeElement(std::shared_ptr<UIElement> element);
    void clearElements();
    
    // Input handling
    bool handleInput(float mouseX, float mouseY, bool clicked);
    
    // Add method to handle scroll wheel events for UI elements
    void handleScrollWheel(float x, float y, float scrollDelta) {
        if (blockVisualizationPanel_ && blockVisualizationPanel_->isVisible()) {
            blockVisualizationPanel_->handleScroll(x, y, scrollDelta);
        }
    }
    
    // Access to renderer for custom UI elements
    UIRenderer& getRenderer() { return renderer_; }
    
    // Debug UI panels
    void toggleBlockVisualizationPanel();
    bool isBlockVisualizationPanelVisible() const;
    std::shared_ptr<BlockVisualizationPanel> getBlockVisualizationPanel() { return blockVisualizationPanel_; }

protected:
    std::vector<std::shared_ptr<UIElement>> elements_;

private:
    UIRenderer renderer_;
    std::shared_ptr<BlockVisualizationPanel> blockVisualizationPanel_;
    
    void renderElement(UIElement* element);
};

} // namespace UI
} // namespace VoxelEngine
