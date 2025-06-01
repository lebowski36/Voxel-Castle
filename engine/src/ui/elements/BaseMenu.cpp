#include "ui/elements/BaseMenu.h"
#include "ui/MenuSystem.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

BaseMenu::BaseMenu(UIRenderer* renderer, const std::string& title) 
    : UIPanel(renderer), menuSystem_(nullptr), title_(title), currentY_(TITLE_HEIGHT + ELEMENT_SPACING) {
    // Set consistent menu styling
    setColor({0.1f, 0.1f, 0.1f, 0.85f}); // Dark semi-transparent background
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
    // Call parent render to draw the panel and children
    UIPanel::render();
    
    // Render the title text on top of the title panel
    if (!title_.empty() && titlePanel_ && renderer_) {
        float titleX = titlePanel_->getPosition().x + getPosition().x + 20.0f;
        float titleY = titlePanel_->getPosition().y + getPosition().y + TITLE_HEIGHT / 2.0f + 8.0f;
        renderer_->drawText(title_, titleX, titleY, 1.2f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void BaseMenu::setTitle(const std::string& title) {
    title_ = title;
    updateTitleText();
}

std::shared_ptr<UIButton> BaseMenu::createStyledButton(const std::string& text, float yPosition) {
    auto button = std::make_shared<UIButton>(renderer_);
    
    // Calculate button size and position
    float buttonWidth = getSize().x * BUTTON_WIDTH_RATIO;
    float buttonX = (getSize().x - buttonWidth) / 2.0f; // Center horizontally
    
    button->setText(text);
    button->setPosition(buttonX, yPosition);
    button->setSize(buttonWidth, BUTTON_HEIGHT);
    
    // Set consistent button styling
    button->setBackgroundColor({0.2f, 0.3f, 0.4f, 0.9f}); // Slightly blue-tinted background
    button->setHoverColor({0.3f, 0.4f, 0.5f, 0.9f}); // Lighter on hover
    
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
    titlePanel_->setColor({0.15f, 0.2f, 0.3f, 0.95f}); // Darker blue-tinted background
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
