#include "ui/elements/WorldCreationDialog.h"
#include "ui/MenuSystem.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace VoxelEngine {
namespace UI {

WorldCreationDialog::WorldCreationDialog(UIRenderer* renderer) 
    : BaseMenu(renderer, "Create New World")
    , currentWorldSize_(WorldSize::REGIONAL)
{
    // Set initial dialog width to accommodate longer button text
    setSize(700.0f, 100.0f); // Width for long buttons, height will auto-resize
    
    // Generate an initial random seed
    currentSeed_ = VoxelCastle::World::WorldSeed(); // Default constructor generates random seed
    updateSeedDisplay();
}

bool WorldCreationDialog::initialize(MenuSystem* menuSystem) {
    if (!initializeBase(menuSystem)) {
        return false;
    }

    // Add seed section
    addSeedSection();
    
    // Add world size section
    addWorldSizeSection();
    
    // Add action buttons
    addActionButtons();

    return true;
}

void WorldCreationDialog::update(float deltaTime) {
    BaseMenu::update(deltaTime);
    
    // Update any dynamic content here
    // For now, just ensure the size buttons reflect current state
    updateSizeButtons();
}

void WorldCreationDialog::addSeedSection() {
    // Add spacing and section title
    addElementSpacing();
    
    // Show current seed (as text display)
    // Note: Since we don't have text input yet, we'll show the seed and provide a random button
    addElementSpacing();
    
    // Random seed button
    randomSeedButton_ = createStyledButton("Generate Random Seed", getNextElementY());
    randomSeedButton_->setOnClick([this]() { onRandomSeedClicked(); });
    addElementSpacing();
}

void WorldCreationDialog::addWorldSizeSection() {
    // Add spacing and section
    addElementSpacing();
    
    // World size buttons
    starterSizeButton_ = createStyledButton("Starter (10km²) - ~5 seconds", getNextElementY());
    starterSizeButton_->setOnClick([this]() { onSizeButtonClicked(WorldSize::STARTER); });
    addElementSpacing();
    
    regionalSizeButton_ = createStyledButton("Regional (1,000km²) - ~30 seconds", getNextElementY());
    regionalSizeButton_->setOnClick([this]() { onSizeButtonClicked(WorldSize::REGIONAL); });
    addElementSpacing();
    
    continentalSizeButton_ = createStyledButton("Continental (100,000km²) - ~10 minutes", getNextElementY());
    continentalSizeButton_->setOnClick([this]() { onSizeButtonClicked(WorldSize::CONTINENTAL); });
    addElementSpacing();
    
    // Massive size button (disabled)
    massiveSizeButton_ = createStyledButton("Massive (25M km²) - Coming Soon", getNextElementY());
    // TODO: Add setEnabled method to UIButton
    // massiveSizeButton_->setEnabled(false);  // Disabled until optimization complete
    addElementSpacing();
}

void WorldCreationDialog::addActionButtons() {
    // Add spacing before action buttons
    addElementSpacing();
    addElementSpacing();
    
    // Create World button
    createWorldButton_ = createStyledButton("Create World", getNextElementY());
    createWorldButton_->setOnClick([this]() { onCreateWorldClicked(); });
    addElementSpacing();
    
    // Cancel button
    cancelButton_ = createStyledButton("Cancel", getNextElementY());
    cancelButton_->setOnClick([this]() { onCancelClicked(); });
}

void WorldCreationDialog::onRandomSeedClicked() {
    currentSeed_ = VoxelCastle::World::WorldSeed(); // Default constructor generates random seed
    updateSeedDisplay();
    
    std::cout << "[WorldCreationDialog] Generated new random seed: " << currentSeed_.getMasterSeed() << std::endl;
}

void WorldCreationDialog::onSizeButtonClicked(WorldSize size) {
    if (!isWorldSizeAvailable(size)) {
        std::cout << "[WorldCreationDialog] World size not yet available: " << static_cast<int>(size) << std::endl;
        return;
    }
    
    currentWorldSize_ = size;
    updateSizeButtons();
    
    std::cout << "[WorldCreationDialog] Selected world size: " << getWorldSizeDescription(size) 
              << " (estimate: " << getGenerationTimeEstimate(size) << ")" << std::endl;
}

void WorldCreationDialog::onCreateWorldClicked() {
    if (onWorldCreate_) {
        std::cout << "[WorldCreationDialog] Creating world with seed: " << currentSeed_.getMasterSeed() 
                  << ", size: " << getWorldSizeDescription(currentWorldSize_) << std::endl;
        onWorldCreate_(currentSeed_, currentWorldSize_);
    }
}

void WorldCreationDialog::onCancelClicked() {
    if (onCancel_) {
        std::cout << "[WorldCreationDialog] World creation cancelled" << std::endl;
        onCancel_();
    }
}

void WorldCreationDialog::updateSeedDisplay() {
    std::ostringstream oss;
    oss << "Seed: " << currentSeed_.getMasterSeed();
    seedDisplayText_ = oss.str();
    
    // Update the random button text to show the current seed
    if (randomSeedButton_) {
        std::ostringstream buttonText;
        buttonText << "Random Seed: " << currentSeed_.getMasterSeed();
        randomSeedButton_->setText(buttonText.str());
    }
}

void WorldCreationDialog::updateSizeButtons() {
    // Update button appearance based on current selection
    if (starterSizeButton_) {
        bool isSelected = (currentWorldSize_ == WorldSize::STARTER);
        starterSizeButton_->setText(isSelected ? "> Starter (10km²) - ~5 seconds <" : "Starter (10km²) - ~5 seconds");
    }
    
    if (regionalSizeButton_) {
        bool isSelected = (currentWorldSize_ == WorldSize::REGIONAL);
        regionalSizeButton_->setText(isSelected ? "> Regional (1,000km²) - ~30 seconds <" : "Regional (1,000km²) - ~30 seconds");
    }
    
    if (continentalSizeButton_) {
        bool isSelected = (currentWorldSize_ == WorldSize::CONTINENTAL);
        continentalSizeButton_->setText(isSelected ? "> Continental (100,000km²) - ~10 minutes <" : "Continental (100,000km²) - ~10 minutes");
    }
}

std::string WorldCreationDialog::getWorldSizeDescription(WorldSize size) const {
    switch (size) {
        case WorldSize::STARTER: return "Starter";
        case WorldSize::REGIONAL: return "Regional";
        case WorldSize::CONTINENTAL: return "Continental";
        case WorldSize::MASSIVE: return "Massive";
        default: return "Unknown";
    }
}

std::string WorldCreationDialog::getGenerationTimeEstimate(WorldSize size) const {
    switch (size) {
        case WorldSize::STARTER: return "~5 seconds";
        case WorldSize::REGIONAL: return "~30 seconds";
        case WorldSize::CONTINENTAL: return "~10 minutes";
        case WorldSize::MASSIVE: return "~2 hours";
        default: return "Unknown";
    }
}

bool WorldCreationDialog::isWorldSizeAvailable(WorldSize size) const {
    // For now, only Starter, Regional, and Continental are available
    // Massive is disabled until performance optimization is complete
    return size != WorldSize::MASSIVE;
}

} // namespace UI
} // namespace VoxelEngine
