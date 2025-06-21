#include "ui/elements/CreateWorldMenu.h"
#include "ui/MenuSystem.h"
#include "ui/elements/UITextInput.h"
#include <iostream>

namespace VoxelEngine {
namespace UI {

CreateWorldMenu::CreateWorldMenu(UIRenderer* renderer) : BaseMenu(renderer, "Create New World") {
}

bool CreateWorldMenu::initialize(MenuSystem* menuSystem) {
    if (!initializeBase(menuSystem)) {
        return false;
    }

    std::cout << "[CreateWorldMenu] Starting initialization..." << std::endl;
    std::cout << "[CreateWorldMenu] Initial menu size: (" << getSize().x << ", " << getSize().y << ")" << std::endl;
    
    // CRITICAL FIX: Set proper menu size BEFORE creating elements
    float menuWidth = 400.0f;  // Standard menu width like MainMenu
    setSize(menuWidth, 100.0f); // Height will be auto-adjusted later
    
    std::cout << "[CreateWorldMenu] Set menu size to: (" << getSize().x << ", " << getSize().y << ")" << std::endl;

    // Create and style the seed input field
    seedInput_ = std::make_shared<UITextInput>(renderer_);
    seedInput_->setPlaceholder("Enter world seed (optional)");
    
    // Calculate centered position for the input field (use same logic as buttons)
    float inputWidth = 250.0f; // Use a width similar to buttons (220) 
    float inputHeight = 50.0f;
    
    // Use EXACTLY the same centering logic as buttons
    float centerX = (getSize().x - inputWidth) / 2.0f;
    
    std::cout << "[CreateWorldMenu] Input field positioning:" << std::endl;
    std::cout << "  Input width: " << inputWidth << std::endl;
    std::cout << "  Menu width: " << getSize().x << std::endl;
    std::cout << "  Calculated centerX: " << centerX << std::endl;
    std::cout << "  Y position: " << getNextElementY() << std::endl;
    
    seedInput_->setPosition(centerX, getNextElementY()); 
    seedInput_->setSize(inputWidth, inputHeight);
    
    // Style the input field to match menu theme
    seedInput_->setBackgroundColor({0.15f, 0.15f, 0.2f, 1.0f}); // Dark background
    seedInput_->setTextColor({1.0f, 1.0f, 1.0f, 1.0f}); // White text
    seedInput_->setBorderColor({0.4f, 0.4f, 0.5f, 1.0f}); // Light border
    
    seedInput_->setOnEnter([this]() { onCreateClicked(); });
    seedInput_->setVisible(true); // Explicitly ensure visibility
    addChild(seedInput_);
    std::cout << "[CreateWorldMenu] Added input field as child" << std::endl;
    addElementSpacing(20.0f); // Add extra spacing after input

    std::cout << "[CreateWorldMenu] Creating 'Create World' button..." << std::endl;
    createButton_ = createStyledButton("Create World", getNextElementY());
    createButton_->setOnClick([this]() { onCreateClicked(); });
    addElementSpacing();

    std::cout << "[CreateWorldMenu] Creating 'Cancel' button..." << std::endl;
    cancelButton_ = createStyledButton("Cancel", getNextElementY());
    cancelButton_->setOnClick([this]() { onCancelClicked(); });

    // Auto-resize height to fit content, but keep the width we set
    autoResizeHeight();
    
    std::cout << "[CreateWorldMenu] Final menu size: (" << getSize().x << ", " << getSize().y << ")" << std::endl;
    std::cout << "[CreateWorldMenu] Initialization complete. Total children: " << getChildren().size() << std::endl;

    return true;
}

void CreateWorldMenu::onCreateClicked() {
    if (menuSystem_) {
        std::string seed = seedInput_->getText();
        menuSystem_->requestProceduralWorldCreation(seed);
        menuSystem_->closeMenus();
    }
}

void CreateWorldMenu::onCancelClicked() {
    if (menuSystem_) {
        menuSystem_->showMainMenu();
    }
}

void CreateWorldMenu::render() {
    // Call parent render first
    BaseMenu::render();
    
    if (!isVisible() || !renderer_) {
        return;
    }
    
    // Debug: Let's temporarily skip the custom text rendering to see if that's the issue
    // The BaseMenu should handle rendering all children (input field and buttons)
    
    // TODO: Re-enable custom label text once we fix the main rendering issue
    /*
    // Draw descriptive text above the input field
    glm::vec2 pos = getAbsolutePosition();
    float labelY = pos.y + getNextElementY() - 80.0f; // Position above the input field
    float centerX = pos.x + getSize().x / 2.0f;
    
    std::string labelText = "World Seed (leave empty for random)";
    float textWidth = renderer_->getTextWidth(labelText, 1.0f);
    float labelX = centerX - textWidth / 2.0f;
    
    renderer_->drawText(labelText, labelX, labelY, 1.0f, glm::vec3(0.9f, 0.9f, 0.9f));
    */
}

} // namespace UI
} // namespace VoxelEngine
