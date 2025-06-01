#include "ui/UISystem.h"
#include "ui/UILogger.h"
#include <algorithm>
#include <iostream> // Add this header for std::cout and std::cerr

namespace VoxelEngine {
namespace UI {

UISystem::UISystem() = default;

UISystem::~UISystem() {
    shutdown();
}

bool UISystem::initialize(int screenWidth, int screenHeight, const std::string& projectRoot) {
    return renderer_.initialize(screenWidth, screenHeight, projectRoot);
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
                    std::cerr << "[UISystem] Warning: Element at (" << pos.x << "," << pos.y 
                              << ") with size (" << size.x << "," << size.y 
                              << ") may be outside screen bounds " 
                              << renderer_.getScreenWidth() << "x" << renderer_.getScreenHeight() << std::endl;
                }
            }
            
            if (shouldLog) {
                std::cout << "[UISystem] Rendering element at (" << pos.x << ", " << pos.y 
                          << "), size: (" << size.x << ", " << size.y << ")" << std::endl;
            }
            
            renderElement(element.get());
            visibleCount++;
        } else if (shouldLog && element) {
            std::cout << "[UISystem] Skipping invisible element at (" 
                      << element->getPosition().x << ", " << element->getPosition().y << ")" << std::endl;
        }
    }
    
    if (shouldLog) {
        std::cout << "[UISystem] Rendered " << visibleCount << " visible elements out of " 
                  << elements_.size() << " total" << std::endl;
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

} // namespace UI
} // namespace VoxelEngine
