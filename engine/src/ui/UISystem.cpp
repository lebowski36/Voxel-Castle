#include "ui/UISystem.h"
#include "ui/UILogger.h"
#include "../../game/include/utils/debug_logger.h"
#include <algorithm>
#include <iostream> // Add this header for std::cout and std::cerr

namespace VoxelEngine {
namespace UI {

UISystem::UISystem() : blockVisualizationPanel_(nullptr) {
}

UISystem::~UISystem() {
    shutdown();
}

bool UISystem::initialize(int screenWidth, int screenHeight, const std::string& projectRoot) {
    bool success = renderer_.initialize(screenWidth, screenHeight, projectRoot);
    
    // Set the static renderer pointer for BlockVisualizationPanel
    if (success) {
        BlockVisualizationPanel::setCurrentRenderer(&renderer_);
    }
    
    return success;
}

void UISystem::shutdown() {
    clearElements();
    renderer_.shutdown();
}

void UISystem::update(float deltaTime) {
    // Update all elements
    for (auto& element : elements_) {
        if (element && element->isVisible()) {
            element->update(deltaTime);
        }
    }
}

void UISystem::render() {
    // Only log once every 100 frames to reduce file spam
    static int frameCounter = 0;
    bool shouldLog = (frameCounter++ % 100 == 0);
    
    auto& logger = UILogger::getInstance();
    
    if (shouldLog) {
        logger.debug("UISystem", "Rendering " + std::to_string(elements_.size()) + " UI elements");
    }
    
    if (renderer_.getShaderProgram() == 0) {
        logger.error("UISystem", "UI Renderer has invalid shader program (0)");
        return;
    }
    
    // Debug: verify renderer state before starting
    if (shouldLog) {
        logger.debug("UISystem", "Renderer screen size: " + std::to_string(renderer_.getScreenWidth()) + "x" + std::to_string(renderer_.getScreenHeight()));
        
        // Verify OpenGL state
        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        logger.debug("UISystem", "Current OpenGL program before beginFrame: " + std::to_string(currentProgram));
    }
    
    renderer_.beginFrame();
    
    if (shouldLog) {
        // Verify OpenGL state after beginFrame
        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        logger.debug("UISystem", "Current OpenGL program after beginFrame: " + std::to_string(currentProgram));
        logger.debug("UISystem", "Expected program: " + std::to_string(renderer_.getShaderProgram()));
    }
    
    int visibleCount = 0;
    // Render all elements
    for (auto& element : elements_) {
        if (element && element->isVisible()) {
            // Verify element position is within screen bounds
            glm::vec2 pos = element->getAbsolutePosition();
            glm::vec2 size = element->getSize();
            
            if (pos.x < -size.x || pos.y < -size.y || 
                pos.x > renderer_.getScreenWidth() || pos.y > renderer_.getScreenHeight()) {
                if (shouldLog) {
                    logger.warning("UISystem", "Element at (" + std::to_string(pos.x) + "," + std::to_string(pos.y) 
                                   + ") with size (" + std::to_string(size.x) + "," + std::to_string(size.y) 
                                   + ") may be outside screen bounds " 
                                   + std::to_string(renderer_.getScreenWidth()) + "x" + std::to_string(renderer_.getScreenHeight()));
                }
            }
            
            if (shouldLog) {
                DEBUG_LOG("UISystem", "Rendering element at (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")");
            }
            
            renderElement(element.get());
            visibleCount++;
        } else if (shouldLog && element) {
            DEBUG_LOG("UISystem", "Skipping invisible element at (" + std::to_string(element->getPosition().x) + ", " + std::to_string(element->getPosition().y) + ")");
        }
    }
    
    if (shouldLog) {
        DEBUG_LOG("UISystem", "Rendered " + std::to_string(visibleCount) + " visible elements out of " + std::to_string(elements_.size()) + " total");
    }
    
    renderer_.endFrame();
}

void UISystem::setScreenSize(int width, int height) {
    renderer_.setScreenSize(width, height);
}

void UISystem::addElement(std::shared_ptr<UIElement> element) {
    if (element) {
        elements_.push_back(element);
    }
}

void UISystem::removeElement(std::shared_ptr<UIElement> element) {
    auto it = std::find(elements_.begin(), elements_.end(), element);
    if (it != elements_.end()) {
        elements_.erase(it);
    }
}

void UISystem::clearElements() {
    elements_.clear();
}

bool UISystem::handleInput(float mouseX, float mouseY, bool clicked) {
    // Process elements in reverse order (last added = on top)
    for (auto it = elements_.rbegin(); it != elements_.rend(); ++it) {
        auto& element = *it;
        if (element && element->isVisible()) {
            if (element->handleInput(mouseX, mouseY, clicked)) {
                return true; // Element handled the input
            }
        }
    }
    return false; // No element handled the input
}

void UISystem::renderElement(UIElement* element) {
    if (!element || !element->isVisible()) {
        return;
    }
    
    // Render this element
    element->render();
    
    // Render children
    for (const auto& child : element->getChildren()) {
        if (child) {
            renderElement(child.get());
        }
    }
}

void UISystem::toggleBlockVisualizationPanel() {
    if (!blockVisualizationPanel_) {
        // Lazy initialization - create the panel if it doesn't exist
        blockVisualizationPanel_ = std::make_shared<BlockVisualizationPanel>();
        
        // Set the static renderer pointer so the panel can render
        BlockVisualizationPanel::setCurrentRenderer(&renderer_);
        
        blockVisualizationPanel_->setPosition(100.0f, 100.0f);
        blockVisualizationPanel_->setSize(800.0f, 600.0f);
        addElement(blockVisualizationPanel_);
        blockVisualizationPanel_->setVisible(true);
    }
    else {
        // Toggle visibility
        blockVisualizationPanel_->setVisible(!blockVisualizationPanel_->isVisible());
    }
}

bool UISystem::isBlockVisualizationPanelVisible() const {
    return blockVisualizationPanel_ && blockVisualizationPanel_->isVisible();
}

} // namespace UI
} // namespace VoxelEngine
