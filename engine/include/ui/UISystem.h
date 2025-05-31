#pragma once
#include "ui/UIRenderer.h"
#include "ui/UIElement.h"
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
    void render();
    
    void setScreenSize(int width, int height);
    
    // Element management
    void addElement(std::shared_ptr<UIElement> element);
    void removeElement(std::shared_ptr<UIElement> element);
    void clearElements();
    
    // Input handling
    bool handleInput(float mouseX, float mouseY, bool clicked);
    
    // Access to renderer for custom UI elements
    UIRenderer& getRenderer() { return renderer_; }

protected:
    std::vector<std::shared_ptr<UIElement>> elements_;

private:
    UIRenderer renderer_;
    
    void renderElement(UIElement* element);
};

} // namespace UI
} // namespace VoxelEngine
