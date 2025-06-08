#include "ui/WorldGenerationUI.h"
#include "ui/MenuSystem.h"
#include "world/biome/biome_registry.h"
#include "world/biome/biome_types.h"
#include "utils/debug_logger_stub.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace VoxelCastle::UI {

WorldGenerationUI::WorldGenerationUI(VoxelEngine::UI::UIRenderer* renderer) 
    : BaseMenu(renderer, "World Generation") {
    // Ensure title is set and visible
    setTitle("World Generation");
}

WorldGenerationUI::~WorldGenerationUI() {
    if (isGenerating_) {
        CancelGeneration();
    }
}

bool WorldGenerationUI::initialize(VoxelEngine::UI::MenuSystem* menuSystem) {
    if (!initializeBase(menuSystem)) {
        return false;
    }
    
    menuSystem_ = menuSystem;
    
    std::cout << "[WorldGenerationUI] Initializing with size: " << getSize().x << "x" << getSize().y << std::endl;
    std::cout << "[WorldGenerationUI] Position: " << getPosition().x << ", " << getPosition().y << std::endl;
    
    createUIElements();
    
    std::cout << "[WorldGenerationUI] After button creation, size: " << getSize().x << "x" << getSize().y << std::endl;
    std::cout << "[WorldGenerationUI] Visibility: " << (isVisible() ? "VISIBLE" : "HIDDEN") << std::endl;
    
    // Ensure the menu is visible
    setVisible(true);
    
    std::cout << "[WorldGenerationUI] Initialization complete, visibility: " << (isVisible() ? "VISIBLE" : "HIDDEN") << std::endl;
    
    return true;
}

void WorldGenerationUI::createUIElements() {
    // Clear any existing elements
    removeAllChildren();
    currentY_ = TITLE_HEIGHT + ELEMENT_SPACING * 2;
    
    // Set proper size for the world generation UI (match main menu size)
    setSize(450.0f, 390.0f);
    
    // Add a small space at the top 
    currentY_ += 20.0f;
    
    // Create action buttons - createStyledButton already calls addChild()
    auto startButton = createStyledButton("Start World Generation", getNextElementY());
    startButton->setOnClick([this]() { 
        StartGeneration(); 
    });
    // Force very visible colors for debugging
    startButton->setBackgroundColor({1.0f, 0.0f, 0.0f, 1.0f}); // Bright red
    startButton->setHoverColor({1.0f, 0.5f, 0.0f, 1.0f}); // Orange
    startButton->setClickColor({1.0f, 1.0f, 0.0f, 1.0f}); // Yellow
    addElementSpacing();
    
    auto backButton = createStyledButton("Back to Main Menu", getNextElementY());
    backButton->setOnClick([this]() { 
        // Return to main menu
        if (menuSystem_) {
            menuSystem_->showMainMenu();
        }
    });
    // Force very visible colors for debugging
    backButton->setBackgroundColor({0.0f, 1.0f, 0.0f, 1.0f}); // Bright green
    backButton->setHoverColor({0.0f, 1.0f, 0.5f, 1.0f}); // Light green
    backButton->setClickColor({0.5f, 1.0f, 0.0f, 1.0f}); // Yellow-green
    addElementSpacing();
    
    // Debug output for children
    std::cout << "[WorldGenerationUI] Created " << children_.size() << " UI elements" << std::endl;
    for (size_t i = 0; i < children_.size(); i++) {
        auto& child = children_[i];
        if (child) {
            std::cout << "[WorldGenerationUI] Child " << i << " position: "
                      << child->getPosition().x << ", " << child->getPosition().y
                      << " size: " << child->getSize().x << "x" << child->getSize().y
                      << " visible: " << (child->isVisible() ? "YES" : "NO") << std::endl;
        }
    }
}

void WorldGenerationUI::update(float deltaTime) {
    BaseMenu::update(deltaTime);
    
    if (isGenerating_) {
        UpdateGeneration(deltaTime);
    }
}

void WorldGenerationUI::render() {
    // Only log once every 100 frames to reduce spam
    static int frameCounter = 0;
    if (frameCounter++ % 100 == 0) {
        std::cout << "[WorldGenerationUI] render() call " << frameCounter << " - Position: " << getPosition().x << ", " << getPosition().y
                  << " Size: " << getSize().x << "x" << getSize().y
                  << " Visible: " << (isVisible() ? "YES" : "NO")
                  << " Child count: " << children_.size() << std::endl;
    }
    
    // Call the parent render method
    BaseMenu::render();
}

bool WorldGenerationUI::IsGenerationComplete() const {
    return generationComplete_;
}

void WorldGenerationUI::StartGeneration() {
    if (isGenerating_) {
        return;
    }
    
    isGenerating_ = true;
    generationComplete_ = false;
    generationProgress_ = 0.0f;
    generationStartTime_ = std::chrono::steady_clock::now();
    
    // Initialize world generation components
    worldSeed_ = std::make_shared<VoxelCastle::World::WorldSeed>();
    worldParameters_ = std::make_shared<VoxelCastle::World::WorldParameters>();
    worldGenerator_ = std::make_shared<VoxelCastle::World::SeedWorldGenerator>(worldSeed_, worldParameters_);
    
    std::cout << "[WorldGenerationUI] Starting world generation..." << std::endl;
}

void WorldGenerationUI::CancelGeneration() {
    if (!isGenerating_) {
        return;
    }
    
    isGenerating_ = false;
    generationComplete_ = false;
    std::cout << "[WorldGenerationUI] World generation cancelled" << std::endl;
}

void WorldGenerationUI::SetCompletionCallback(std::function<void(std::shared_ptr<VoxelCastle::World::SeedWorldGenerator>)> callback) {
    completionCallback_ = callback;
}

void WorldGenerationUI::UpdateGeneration(float deltaTime) {
    // Simple progress simulation - complete in 3 seconds
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - generationStartTime_).count();
    float elapsedSeconds = elapsed / 1000.0f;
    
    // Update progress (3 second total generation time)
    generationProgress_ = std::min(1.0f, elapsedSeconds / 3.0f);
    
    if (generationProgress_ >= 1.0f) {
        CompleteGeneration();
    }
}

void WorldGenerationUI::CompleteGeneration() {
    isGenerating_ = false;
    generationComplete_ = true;
    
    std::cout << "[WorldGenerationUI] World generation complete!" << std::endl;
    
    // Call completion callback
    if (completionCallback_ && worldGenerator_) {
        completionCallback_(worldGenerator_);
    }
}

void WorldGenerationUI::OnStartClicked() {
    std::cout << "[WorldGenerationUI] Start button clicked" << std::endl;
    StartGeneration();
}

void WorldGenerationUI::OnCancelClicked() {
    std::cout << "[WorldGenerationUI] Cancel button clicked" << std::endl;
    CancelGeneration();
    
    // Return to main menu
    if (menuSystem_) {
        menuSystem_->showMainMenu();
    }
}

void WorldGenerationUI::removeAllChildren() {
    // Remove all child elements
    auto children = getChildren();
    for (auto& child : children) {
        removeChild(child);
    }
}

bool WorldGenerationUI::handleInput(float mouseX, float mouseY, bool clicked) {
    // Add debug output for mouse interactions
    static int debugCounter = 0;
    if (clicked && debugCounter++ % 10 == 0) {
        std::cout << "[WorldGenerationUI] Mouse click at (" << mouseX << ", " << mouseY << ")" << std::endl;
        std::cout << "[WorldGenerationUI] Menu position: (" << getPosition().x << ", " << getPosition().y << ")" << std::endl;
        std::cout << "[WorldGenerationUI] Menu size: (" << getSize().x << ", " << getSize().y << ")" << std::endl;
        
        // Check if click is within menu bounds
        if (mouseX >= getPosition().x && mouseX <= getPosition().x + getSize().x &&
            mouseY >= getPosition().y && mouseY <= getPosition().y + getSize().y) {
            std::cout << "[WorldGenerationUI] Click is within menu bounds!" << std::endl;
        } else {
            std::cout << "[WorldGenerationUI] Click is outside menu bounds!" << std::endl;
        }
    }
    
    return BaseMenu::handleInput(mouseX, mouseY, clicked);
}

} // namespace VoxelCastle::UI