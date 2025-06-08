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
    
    // Create simple UI elements for now
    auto startButton = createStyledButton("Start World Generation", getNextElementY());
    startButton->setOnClick([this]() { OnStartClicked(); });
    addElementSpacing();
    
    auto cancelButton = createStyledButton("Cancel", getNextElementY());
    cancelButton->setOnClick([this]() { OnCancelClicked(); });
    addElementSpacing();
    
    std::cout << "[WorldGenerationUI] After button creation, size: " << getSize().x << "x" << getSize().y << std::endl;
    std::cout << "[WorldGenerationUI] Visibility: " << (isVisible() ? "VISIBLE" : "HIDDEN") << std::endl;
    
    // Ensure the menu is visible
    setVisible(true);
    
    std::cout << "[WorldGenerationUI] Initialization complete, visibility: " << (isVisible() ? "VISIBLE" : "HIDDEN") << std::endl;
    
    return true;
}

void WorldGenerationUI::update(float deltaTime) {
    BaseMenu::update(deltaTime);
    
    if (isGenerating_) {
        UpdateGeneration(deltaTime);
    }
}

void WorldGenerationUI::render() {
    std::cout << "[WorldGenerationUI] render() called - Position: " << getPosition().x << ", " << getPosition().y 
              << " Size: " << getSize().x << "x" << getSize().y << " Visible: " << (isVisible() ? "YES" : "NO") << std::endl;
    
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
    if (!isGenerating_) {
        StartGeneration();
    }
}

void WorldGenerationUI::OnCancelClicked() {
    if (menuSystem_) {
        menuSystem_->showMainMenu();
    }
}

} // namespace VoxelCastle::UI