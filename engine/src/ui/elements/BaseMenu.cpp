#include "ui/elements/BaseMenu.h"
#include "ui/MenuSystem.h"
#include "ui/UIRenderer.h"
#include "ui/UILogger.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>

namespace VoxelEngine {
namespace UI {

BaseMenu::BaseMenu(UIRenderer* renderer, const std::string& title) 
    : UIPanel(renderer), menuSystem_(nullptr), title_(title), currentY_(TITLE_HEIGHT + ELEMENT_SPACING * 2) {
    // Set consistent menu styling with darker background for better button contrast
    setColor({0.05f, 0.05f, 0.1f, 0.9f}); // Very dark blue-black background
    
    // DEBUG: Log initial size
    auto& logger = UILogger::getInstance();
    logger.debug("BaseMenu", "Constructor - Initial size: (" + std::to_string(getSize().x) + ", " + std::to_string(getSize().y) + ")");
}

bool BaseMenu::initializeBase(MenuSystem* menuSystem) {
    if (!menuSystem) {
        std::cerr << "[BaseMenu] MenuSystem pointer is null" << std::endl;
        return false;
    }

    menuSystem_ = menuSystem;
    // No title panel creation - we'll render title text directly
    return true;
}

void BaseMenu::update(float deltaTime) {
    UIPanel::update(deltaTime);
}

bool BaseMenu::handleInput(float mouseX, float mouseY, bool clicked) {
    return UIPanel::handleInput(mouseX, mouseY, clicked);
}

void BaseMenu::render() {
    // Only render if the menu is visible
    if (!isVisible() || !renderer_) {
        return;
    }

    // First render the main panel background
    glm::vec2 pos = getAbsolutePosition();

    // Check for OpenGL errors before drawing - limit logging to avoid spam
    static int errorCount = 0;
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        if (errorCount++ < 5) { // Show first 5 errors instead of every 100th
            std::cerr << "[BaseMenu] OpenGL error before rendering quad: 0x" << std::hex << err << std::dec 
                      << " (error #" << errorCount << ")" << std::endl;
        }
        // Clear any further errors
        while (glGetError() != GL_NO_ERROR) {}
    }

    // Draw main background
    renderer_->renderColoredQuad(pos.x, pos.y, size_.x, size_.y, color_);

    // Create title bar if there is a title
    if (!title_.empty()) {
        // Draw title background
        glm::vec4 titleBgColor = {0.1f, 0.1f, 0.2f, 1.0f}; // Slightly lighter than menu background
        renderer_->renderColoredQuad(pos.x, pos.y, size_.x, TITLE_HEIGHT, titleBgColor);
        
        // Draw title text if text renderer is available
        if (renderer_->isTextRendererAvailable()) {
            // Calculate title text centering using actual text measurements
            float titleScale = 1.5f;
            float actualTitleWidth = renderer_->getTextWidth(title_, titleScale);
            float titleX = pos.x + (getSize().x - actualTitleWidth) / 2.0f; // Center horizontally
            float titleY = pos.y + TITLE_HEIGHT / 2.0f + 12.0f; // Center in title area
            
            renderer_->drawText(title_, titleX, titleY, titleScale, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }

    // Render all children (buttons, etc.)
    // Debug logging disabled to reduce console spam
    
    for (auto& child : children_) {
        if (child && child->isVisible()) {
            // Force children to be visible and update their absolute positions before rendering
            child->setVisible(true);
            child->render();
        }
    }
}

void BaseMenu::setTitle(const std::string& title) {
    title_ = title;
    // Title text will be rendered directly in the render() method
}

void BaseMenu::setSize(float width, float height) {
    // DEBUG: Log size changes
    auto& logger = UILogger::getInstance();
    logger.debug("BaseMenu", "setSize called - Old size: (" + std::to_string(getSize().x) + ", " + std::to_string(getSize().y) + ")");
    logger.debug("BaseMenu", "setSize called - New size: (" + std::to_string(width) + ", " + std::to_string(height) + ")");
    
    // Store fixed menu dimensions for specific menu types
    static std::map<const void*, glm::vec2> fixedMenuSizes;
    
    // For main menu and settings menu, we want to preserve their width (400px and 450px)
    // Store the fixed sizes the first time we see a successful size setting
    glm::vec2 currentSize = getSize();
    
    // First-time setting a proper size (after auto-calculating height)
    if (currentSize.x == 100.0f && (width == 400.0f || width == 450.0f)) {
        // This is an initialization to the proper menu width 
        // Save this as the fixed size for this menu instance
        glm::vec2 fixedSize(width, height);
        fixedMenuSizes[this] = fixedSize;
        logger.info("BaseMenu", "Established fixed menu size: (" + std::to_string(width) + "x" + std::to_string(height) + ")");
    }
    // Check if this menu has a fixed size we should enforce
    else if (fixedMenuSizes.find(this) != fixedMenuSizes.end()) {
        // We know the correct size for this menu
        glm::vec2 correctSize = fixedMenuSizes[this];
        
        // If dimensions changed dramatically, it's likely a fullscreen toggle resize
        // Ignore the resize and keep our fixed dimensions
        if (std::abs(width - correctSize.x) > 50.0f) {  // Allow small adjustments
            logger.warning("BaseMenu", "Preventing menu resize from " + std::to_string(correctSize.x) + "x" + 
                          std::to_string(correctSize.y) + " to " + std::to_string(width) + "x" + std::to_string(height));
            
            // If forced to be too large, explicitly restore our known good size
            if (width > 800.0f) {
                // Call parent setSize with the correct size
                UIPanel::setSize(correctSize.x, correctSize.y);
                logger.debug("BaseMenu", "Restored fixed menu size: (" + std::to_string(correctSize.x) + 
                            "x" + std::to_string(correctSize.y) + ")");
                
                // Ensure buttons are centered correctly
                repositionButtons();
            }
            return;  // Ignore this resize request entirely
        }
    }
    
    // Call parent setSize for acceptable size changes
    UIPanel::setSize(width, height);
    
    logger.debug("BaseMenu", "setSize completed - Actual size: (" + std::to_string(getSize().x) + ", " + std::to_string(getSize().y) + ")");
    
    // Reposition all existing buttons to be centered in the new menu size
    repositionButtons();
}

void BaseMenu::repositionButtons() {
    auto& logger = UILogger::getInstance();
    logger.debug("BaseMenu", "Repositioning buttons after size change - Menu size: (" + std::to_string(getSize().x) + ", " + std::to_string(getSize().y) + ")");
    
    // Find all UIButton children and reposition them
    for (auto& child : children_) {
        // Check if this child is a UIButton
        auto button = std::dynamic_pointer_cast<UIButton>(child);
        if (button) {
            // Get current button properties
            glm::vec2 currentPos = button->getPosition();
            glm::vec2 currentSize = button->getSize();
            
            // Recalculate centered X position with current menu width
            float newButtonX = (getSize().x - currentSize.x) / 2.0f;
            
            // Keep the same Y position
            button->setPosition(newButtonX, currentPos.y);
            
            logger.debug("BaseMenu", "Repositioned button - Old pos: (" + std::to_string(currentPos.x) + ", " + std::to_string(currentPos.y) + 
                        ") New pos: (" + std::to_string(newButtonX) + ", " + std::to_string(currentPos.y) + ")");
        }
    }
}

void BaseMenu::autoResizeHeight() {
    auto& logger = UILogger::getInstance();
    
    // Calculate required height based on current content
    // The currentY_ represents the Y position of the last button
    // We need to add the button height and bottom margin
    float bottomMargin = 40.0f; // Add some breathing room at the bottom
    float requiredHeight = currentY_ + BUTTON_HEIGHT + bottomMargin;
    
    // Only resize if the calculated height is different from current height
    if (std::abs(getSize().y - requiredHeight) > 1.0f) {
        float currentWidth = getSize().x;
        
        logger.debug("BaseMenu", "Auto-resizing height - Current: " + std::to_string(getSize().y) + 
                    " Required: " + std::to_string(requiredHeight) + " (currentY_: " + std::to_string(currentY_) + 
                    ", buttonHeight: " + std::to_string(BUTTON_HEIGHT) + ", bottomMargin: " + std::to_string(bottomMargin) + ")");
        
        // Set new size (this will trigger repositionButtons via setSize override)
        UIPanel::setSize(currentWidth, requiredHeight);
        
        logger.debug("BaseMenu", "Auto-resize completed - New height: " + std::to_string(getSize().y));
        
        // NOTE: We've removed the call to requestMenuRecentering() that was causing crashes
    }
}



std::shared_ptr<UIButton> BaseMenu::createStyledButton(const std::string& text, float yPosition) {
    auto button = std::make_shared<UIButton>(renderer_);
    
    // Calculate dynamic button width based on actual text measurements
    float textScale = 1.0f;
    float actualTextWidth = renderer_->getTextWidth(text, textScale);
    float textPadding = 80.0f; // Increased padding to ensure text fits comfortably
    float buttonWidth = actualTextWidth + textPadding;
    
    // Ensure minimum width and don't exceed maximum width
    float minWidth = 220.0f; // Increased minimum width for better appearance
    float maxWidth = getSize().x * 0.75f; // Slightly smaller max to ensure centering
    buttonWidth = std::max(minWidth, std::min(buttonWidth, maxWidth));
    
    // DEBUG: Add extra logging for button sizing
    auto& logger = UILogger::getInstance();
    logger.debug("BaseMenu", "Button '" + text + "' sizing:");
    logger.debug("BaseMenu", "  Actual text width: " + std::to_string(actualTextWidth));
    logger.debug("BaseMenu", "  Text padding: " + std::to_string(textPadding));
    logger.debug("BaseMenu", "  Calculated width: " + std::to_string(actualTextWidth + textPadding));
    logger.debug("BaseMenu", "  Min width: " + std::to_string(minWidth));
    logger.debug("BaseMenu", "  Max width: " + std::to_string(maxWidth));
    logger.debug("BaseMenu", "  Final button width: " + std::to_string(buttonWidth));
    
    // TEMP: Console debug output to help diagnose button sizing issue
    std::cout << "[BaseMenu] Button '" << text << "' sizing:" << std::endl;
    std::cout << "  Actual text width: " << actualTextWidth << std::endl;
    std::cout << "  Text padding: " << textPadding << std::endl;
    std::cout << "  Calculated width: " << (actualTextWidth + textPadding) << std::endl;
    std::cout << "  Min width: " << minWidth << std::endl;
    std::cout << "  Max width: " << maxWidth << std::endl;
    std::cout << "  Final button width: " << buttonWidth << std::endl;
    
    // Center the button horizontally in the menu
    float buttonX = (getSize().x - buttonWidth) / 2.0f;
    
    // TEMP: Add console debug for button positioning to compare with input field
    std::cout << "[BaseMenu] Button '" << text << "' positioning:" << std::endl;
    std::cout << "  Menu size: (" << getSize().x << ", " << getSize().y << ")" << std::endl;
    std::cout << "  Button width: " << buttonWidth << std::endl;
    std::cout << "  Calculated buttonX: " << buttonX << std::endl;
    std::cout << "  yPosition: " << yPosition << std::endl;
    
    // DEBUG: Log positioning information to UI log
    glm::vec2 menuPos = getAbsolutePosition();
    logger.debug("BaseMenu", "Creating button '" + text + "':");
    logger.debug("BaseMenu", "  Menu absolute position: (" + std::to_string(menuPos.x) + ", " + std::to_string(menuPos.y) + ")");
    logger.debug("BaseMenu", "  Menu size: (" + std::to_string(getSize().x) + ", " + std::to_string(getSize().y) + ")");
    logger.debug("BaseMenu", "  Button relative position: (" + std::to_string(buttonX) + ", " + std::to_string(yPosition) + ")");
    logger.debug("BaseMenu", "  Button size: (" + std::to_string(buttonWidth) + ", " + std::to_string(BUTTON_HEIGHT) + ")");
    
    button->setText(text);
    button->setPosition(buttonX, yPosition);
    button->setSize(buttonWidth, BUTTON_HEIGHT);
    
    // Set consistent button styling with better contrast
    button->setBackgroundColor({0.4f, 0.5f, 0.6f, 0.95f}); // Medium blue-gray background
    button->setHoverColor({0.5f, 0.6f, 0.7f, 0.95f}); // Lighter on hover
    button->setClickColor({0.6f, 0.7f, 0.8f, 0.95f}); // Even lighter when clicked
    
    addChild(button);
    
    // DEBUG: Log final button position after adding as child
    glm::vec2 buttonAbsPos = button->getAbsolutePosition();
    logger.debug("BaseMenu", "  Button absolute position after addChild: (" + std::to_string(buttonAbsPos.x) + ", " + std::to_string(buttonAbsPos.y) + ")");
    
    // Auto-resize menu height to fit all content with bottom margin
    autoResizeHeight();
    
    return button;
}

float BaseMenu::getNextElementY() {
    return currentY_;
}

void BaseMenu::addElementSpacing(float spacing) {
    currentY_ += BUTTON_HEIGHT + spacing;
}

} // namespace UI
} // namespace VoxelEngine
