#include "ui/elements/BaseMenu.h"
#include "ui/MenuSystem.h"
#include "ui/UIRenderer.h"
#include <iostream>
#include <algorithm>

namespace VoxelEngine {
namespace UI {

BaseMenu::BaseMenu(UIRenderer* renderer, const std::string& title) 
    : UIPanel(renderer), menuSystem_(nullptr), title_(title), currentY_(TITLE_HEIGHT + ELEMENT_SPACING * 2) {
    // Set consistent menu styling with darker background for better button contrast
    setColor({0.05f, 0.05f, 0.1f, 0.9f}); // Very dark blue-black background
}

bool BaseMenu::initializeBase(MenuSystem* menuSystem) {
    if (!menuSystem) {
        std::cerr << "[BaseMenu] MenuSystem pointer is null" << std::endl;
        return false;
    }

    menuSystem_ = menuSystem;
    createTitlePanel();
    return true;
}

void BaseMenu::update(float deltaTime) {
    UIPanel::update(deltaTime);
}

bool BaseMenu::handleInput(float mouseX, float mouseY, bool clicked) {
    return UIPanel::handleInput(mouseX, mouseY, clicked);
}

void BaseMenu::render() {
    // First render the main panel background
    if (renderer_ && isVisible()) {
        glm::vec2 pos = getAbsolutePosition();
        renderer_->renderColoredQuad(pos.x, pos.y, size_.x, size_.y, color_);
    }
    
    // Render title panel first (so it's behind the text)
    if (titlePanel_ && titlePanel_->isVisible()) {
        titlePanel_->render();
    }
    
    // Render the title text on top of the title panel
    if (!title_.empty() && titlePanel_ && renderer_) {
        float titleX = getPosition().x + 20.0f; // Left padding
        float titleY = getPosition().y + TITLE_HEIGHT / 2.0f + 12.0f; // Better vertical centering
        renderer_->drawText(title_, titleX, titleY, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f)); // Larger text scale
    }
    
    // Render all other children (buttons, etc.) - skip the title panel since we already rendered it
    for (auto& child : children_) {
        if (child && child->isVisible() && child != titlePanel_) {
            child->render();
        }
    }
}

void BaseMenu::setTitle(const std::string& title) {
    title_ = title;
    updateTitleText();
}

std::shared_ptr<UIButton> BaseMenu::createStyledButton(const std::string& text, float yPosition) {
    auto button = std::make_shared<UIButton>(renderer_);
    
    // Calculate dynamic button width based on text length
    float textScale = 1.0f;
    float estimatedTextWidth = text.length() * 12.0f * textScale; // Character width estimate
    float textPadding = 40.0f; // Padding on both sides
    float buttonWidth = estimatedTextWidth + textPadding;
    
    // Ensure minimum width and don't exceed maximum width
    float minWidth = 150.0f;
    float maxWidth = getSize().x * 0.8f;
    buttonWidth = std::max(minWidth, std::min(buttonWidth, maxWidth));
    
    // Center the button horizontally in the menu
    float buttonX = (getSize().x - buttonWidth) / 2.0f;
    
    button->setText(text);
    button->setPosition(buttonX, yPosition);
    button->setSize(buttonWidth, BUTTON_HEIGHT);
    
    // Set consistent button styling with better contrast
    button->setBackgroundColor({0.4f, 0.5f, 0.6f, 0.95f}); // Medium blue-gray background
    button->setHoverColor({0.5f, 0.6f, 0.7f, 0.95f}); // Lighter on hover
    button->setClickColor({0.6f, 0.7f, 0.8f, 0.95f}); // Even lighter when clicked
    
    addChild(button);
    return button;
}

float BaseMenu::getNextElementY() {
    return currentY_;
}

void BaseMenu::addElementSpacing(float spacing) {
    currentY_ += BUTTON_HEIGHT + spacing;
}

void BaseMenu::createTitlePanel() {
    titlePanel_ = std::make_shared<UIPanel>(renderer_);
    titlePanel_->setColor({0.15f, 0.2f, 0.3f, 0.98f}); // More distinct blue-tinted background for title
    titlePanel_->setPosition(0.0f, 0.0f);
    titlePanel_->setSize(getSize().x, TITLE_HEIGHT);
    addChild(titlePanel_);
    
    updateTitleText();
}

void BaseMenu::updateTitleText() {
    if (!titlePanel_ || !renderer_ || title_.empty()) {
        return;
    }
    
    // Title text will be rendered by the renderer during draw calls
    // The actual text rendering happens in the render phase
}

} // namespace UI
} // namespace VoxelEngine
