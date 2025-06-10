#include "ui/WorldConfigurationUI.h"
#include "ui/elements/UIButton.h"
#include "world/world_persistence_manager.h"
#include "world/world_seed.h"
#include "world/world_parameters.h"
#include <iostream>
#include <random>
#include <cstring>

WorldConfigurationUI::WorldConfigurationUI(VoxelEngine::UI::UIRenderer* renderer) 
    : BaseMenu(renderer, "World Configuration")
    , worldNameExists_(false)
    , isEditingWorldName_(false)
    , onConfigurationComplete_(nullptr)
    , onBack_(nullptr)
    , currentY_(TITLE_HEIGHT + PANEL_MARGIN)
    , isRecreatingUI_(false) {
    
    // Initialize default configuration
    config_.worldName = "New World";
    config_.worldSize = 1024;
    config_.climateType = 1; // Temperate
    config_.simulationDepth = 2; // Normal
    config_.geologicalActivity = 1; // Moderate
    config_.hydrologyLevel = 1; // Normal
    config_.enableCivilizations = true;
    config_.customSeed = 0; // Random
    
    // Initialize world name buffer
    strncpy(worldNameBuffer_, config_.worldName.c_str(), sizeof(worldNameBuffer_) - 1);
    worldNameBuffer_[sizeof(worldNameBuffer_) - 1] = '\0';
}

WorldConfigurationUI::~WorldConfigurationUI() {
    // Destructor implementation - unique_ptr will handle cleanup automatically
}

bool WorldConfigurationUI::initialize(int screenWidth, int screenHeight) {
    // Use larger portion of screen for better usability
    float uiWidth = screenWidth * 0.85f;
    float uiHeight = screenHeight * 0.85f;
    
    setPosition((screenWidth - uiWidth) / 2, (screenHeight - uiHeight) / 2);
    setSize(uiWidth, uiHeight);
    
    std::cout << "[WorldConfigurationUI] Initializing with size: " << getSize().x << "x" << getSize().y 
              << " (screen: " << screenWidth << "x" << screenHeight << ")" << std::endl;
    
    // Initialize world generation objects
    initializeWorldGenerationObjects();
    
    createUIElements();
    setVisible(true);
    
    return true;
}

void WorldConfigurationUI::render() {
    BaseMenu::render();
}

bool WorldConfigurationUI::handleInput(float mouseX, float mouseY, bool clicked) {
    return BaseMenu::handleInput(mouseX, mouseY, clicked);
}

bool WorldConfigurationUI::handleKeyboardInput(int key, bool pressed) {
    if (!isEditingWorldName_ || !pressed) {
        return false;
    }
    
    // Handle text input when editing world name
    size_t currentLength = strlen(worldNameBuffer_);
    
    // Handle backspace
    if (key == 8) { // Backspace key
        if (currentLength > 0) {
            worldNameBuffer_[currentLength - 1] = '\0';
            onWorldNameChanged();
        }
        return true;
    }
    
    // Handle Enter key to finish editing
    if (key == 13) { // Enter key
        isEditingWorldName_ = false;
        onWorldNameChanged();
        createUIElements(); // Refresh UI to show new state
        return true;
    }
    
    // Handle Escape key to cancel editing
    if (key == 27) { // Escape key
        isEditingWorldName_ = false;
        // Restore original name
        strncpy(worldNameBuffer_, config_.worldName.c_str(), sizeof(worldNameBuffer_) - 1);
        worldNameBuffer_[sizeof(worldNameBuffer_) - 1] = '\0';
        createUIElements(); // Refresh UI to show original state
        return true;
    }
    
    // Handle printable characters
    if (key >= 32 && key <= 126 && currentLength < sizeof(worldNameBuffer_) - 1) {
        char c = static_cast<char>(key);
        
        // Convert to uppercase/lowercase as appropriate
        if (key >= 97 && key <= 122) { // lowercase letters
            c = static_cast<char>(key - 32); // Convert to uppercase
        }
        
        worldNameBuffer_[currentLength] = c;
        worldNameBuffer_[currentLength + 1] = '\0';
        onWorldNameChanged();
        return true;
    }
    
    return false;
}

void WorldConfigurationUI::createUIElements() {
    // Clear existing children manually since there's no removeAllChildren()
    children_.clear();
    currentY_ = TITLE_HEIGHT + PANEL_MARGIN;
    
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Title
    auto titleLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    titleLabel->setText("World Generation Setup");
    titleLabel->setPosition(PANEL_MARGIN, PANEL_MARGIN);
    titleLabel->setSize(panelWidth, TITLE_HEIGHT);
    titleLabel->setBackgroundColor({0.2f, 0.3f, 0.4f, 0.9f});
    addChild(titleLabel);
    
    createParameterControls();
    createActionButtons();
}

void WorldConfigurationUI::createParameterControls() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Calculate optimal layout dimensions
    float maxLabelWidth = calculateMaxParameterLabelWidth();
    float maxValueWidth = calculateMaxParameterValueWidth();
    float rowSpacing = calculateOptimalRowSpacing();
    
    float valueColumnX = PANEL_MARGIN + maxLabelWidth + ELEMENT_SPACING;
    float buttonColumnX = valueColumnX + maxValueWidth + ELEMENT_SPACING;
    
    float paramY = currentY_;
    
    // World Name Parameter (first parameter)
    auto nameLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    nameLabel->setText("World Name:");
    nameLabel->setPosition(PANEL_MARGIN, paramY);
    nameLabel->autoSizeToText(8.0f);
    nameLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(nameLabel);
    
    // World name value with validation status
    auto nameValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    std::string nameText = config_.worldName;
    if (config_.worldName.empty()) {
        nameText = "[Enter world name]";
    }
    
    // Show typing indicator when editing
    if (isEditingWorldName_) {
        nameText = std::string(worldNameBuffer_) + "_"; // Blinking cursor effect
    }
    
    nameValueLabel->setText(nameText);
    nameValueLabel->setPosition(valueColumnX, paramY);
    nameValueLabel->autoSizeToText(8.0f);
    
    // Color coding based on validation and editing state
    if (isEditingWorldName_) {
        nameValueLabel->setBackgroundColor({0.3f, 0.3f, 0.6f, 0.8f}); // Blue for editing
    } else if (!worldNameError_.empty()) {
        nameValueLabel->setBackgroundColor({0.6f, 0.2f, 0.2f, 0.8f}); // Red for error
    } else if (worldNameExists_) {
        nameValueLabel->setBackgroundColor({0.6f, 0.4f, 0.2f, 0.8f}); // Orange for existing world
    } else {
        nameValueLabel->setBackgroundColor({0.2f, 0.4f, 0.2f, 0.8f}); // Green for valid
    }
    addChild(nameValueLabel);
    
    // Edit button for world name
    auto nameEditButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    nameEditButton->setText(isEditingWorldName_ ? "Done" : "Edit");
    nameEditButton->setPosition(buttonColumnX, paramY);
    nameEditButton->setSize(60, TEXT_HEIGHT);
    nameEditButton->setBackgroundColor({0.3f, 0.3f, 0.5f, 0.8f});
    nameEditButton->setOnClick([this]() { 
        if (isEditingWorldName_) {
            // Finish editing
            isEditingWorldName_ = false;
            onWorldNameChanged();
        } else {
            // Start editing
            isEditingWorldName_ = true;
            // Clear the buffer to start fresh
            worldNameBuffer_[0] = '\0';
        }
        createUIElements(); // Refresh UI
    });
    addChild(nameEditButton);
    
    paramY += TEXT_HEIGHT + ELEMENT_SPACING;
    
    // Show error message if there's a validation error
    if (!worldNameError_.empty()) {
        auto errorLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        errorLabel->setText("⚠ " + worldNameError_);
        errorLabel->setPosition(PANEL_MARGIN, paramY);
        errorLabel->autoSizeToText(7.0f);
        errorLabel->setBackgroundColor({0.6f, 0.1f, 0.1f, 0.8f}); // Red background for error
        addChild(errorLabel);
        paramY += TEXT_HEIGHT + ELEMENT_SPACING;
    }
    
    // Show editing instructions when in edit mode
    if (isEditingWorldName_) {
        auto instructionLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        instructionLabel->setText("💡 Type world name. Press Enter to save, Escape to cancel.");
        instructionLabel->setPosition(PANEL_MARGIN, paramY);
        instructionLabel->autoSizeToText(7.0f);
        instructionLabel->setBackgroundColor({0.2f, 0.3f, 0.5f, 0.8f}); // Blue background for instructions
        addChild(instructionLabel);
        paramY += TEXT_HEIGHT + ELEMENT_SPACING;
    }
    
    paramY += ELEMENT_SPACING; // Extra spacing after world name
    
    // World Size Parameter
    auto sizeLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeLabel->setText("World Size:");
    sizeLabel->setPosition(PANEL_MARGIN, paramY);
    sizeLabel->autoSizeToText(8.0f);
    sizeLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(sizeLabel);
    
    auto sizeValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeValueLabel->setText(std::to_string(config_.worldSize) + "x" + std::to_string(config_.worldSize) + " regions");
    sizeValueLabel->setPosition(valueColumnX, paramY);
    sizeValueLabel->autoSizeToText(8.0f);
    sizeValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(sizeValueLabel);
    
    auto sizeDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeDecButton->setText("-");
    sizeDecButton->setPosition(buttonColumnX, paramY);
    sizeDecButton->setSize(TEXT_HEIGHT, TEXT_HEIGHT);
    sizeDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    sizeDecButton->setOnClick([this, sizeValueLabel]() { 
        if (config_.worldSize > 256) {
            if (config_.worldSize == 512) config_.worldSize = 256;
            else if (config_.worldSize == 1024) config_.worldSize = 512;
            else if (config_.worldSize == 2048) config_.worldSize = 1024;
            
            sizeValueLabel->setText(std::to_string(config_.worldSize) + "x" + std::to_string(config_.worldSize) + " regions");
            std::cout << "[DEBUG] World size decreased to: " << config_.worldSize << std::endl;
            onParameterChanged();
        }
    });
    addChild(sizeDecButton);
    
    auto sizeIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeIncButton->setText("+");
    sizeIncButton->setPosition(buttonColumnX + 30.0f, paramY);
    sizeIncButton->setSize(TEXT_HEIGHT, TEXT_HEIGHT);
    sizeIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    sizeIncButton->setOnClick([this, sizeValueLabel]() { 
        if (config_.worldSize < 2048) {
            if (config_.worldSize == 256) config_.worldSize = 512;
            else if (config_.worldSize == 512) config_.worldSize = 1024;
            else if (config_.worldSize == 1024) config_.worldSize = 2048;
            
            sizeValueLabel->setText(std::to_string(config_.worldSize) + "x" + std::to_string(config_.worldSize) + " regions");
            std::cout << "[DEBUG] World size increased to: " << config_.worldSize << std::endl;
            onParameterChanged();
        }
    });
    addChild(sizeIncButton);
    paramY += rowSpacing;
    
    // Climate Parameter
    auto climateLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateLabel->setText("Climate Type:");
    climateLabel->setPosition(PANEL_MARGIN, paramY);
    climateLabel->autoSizeToText(8.0f);
    climateLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(climateLabel);
    
    auto climateValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateValueLabel->setPosition(valueColumnX, paramY);
    climateValueLabel->autoSizeToText(8.0f);
    climateValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(climateValueLabel);
    
    auto climateDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateDecButton->setText("<");
    climateDecButton->setPosition(buttonColumnX, paramY);
    climateDecButton->setSize(25.0f, 25.0f);
    climateDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    climateDecButton->setOnClick([this, climateValueLabel]() { 
        if (config_.climateType > 0) {
            config_.climateType--;
            std::vector<std::string> climateTypes = {"Arctic", "Temperate", "Tropical", "Desert"};
            climateValueLabel->setText(climateTypes[config_.climateType]);
            std::cout << "[DEBUG] Climate decreased to: " << config_.climateType << " (" << climateTypes[config_.climateType] << ")" << std::endl;
            onParameterChanged();
        }
    });
    addChild(climateDecButton);
    
    auto climateIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateIncButton->setText(">");
    climateIncButton->setPosition(buttonColumnX + 30.0f, paramY);
    climateIncButton->setSize(25.0f, 25.0f);
    climateIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    climateIncButton->setOnClick([this, climateValueLabel]() { 
        if (config_.climateType < 3) {
            config_.climateType++;
            std::vector<std::string> climateTypes = {"Arctic", "Temperate", "Tropical", "Desert"};
            climateValueLabel->setText(climateTypes[config_.climateType]);
            std::cout << "[DEBUG] Climate increased to: " << config_.climateType << " (" << climateTypes[config_.climateType] << ")" << std::endl;
            onParameterChanged();
        }
    });
    addChild(climateIncButton);
    
    // Update climate value text after creating buttons
    std::vector<std::string> climateTypes = {"Arctic", "Temperate", "Tropical", "Desert"};
    climateValueLabel->setText(climateTypes[config_.climateType]);
    
    paramY += rowSpacing;
    
    // Simulation Depth Parameter
    auto depthLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthLabel->setText("Simulation Depth:");
    depthLabel->setPosition(PANEL_MARGIN, paramY);
    depthLabel->autoSizeToText(8.0f);
    depthLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(depthLabel);
    
    auto depthValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthValueLabel->setPosition(valueColumnX, paramY);
    depthValueLabel->autoSizeToText(8.0f);
    depthValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(depthValueLabel);
    
    auto depthDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthDecButton->setText("<");
    depthDecButton->setPosition(buttonColumnX, paramY);
    depthDecButton->setSize(25.0f, 25.0f);
    depthDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    depthDecButton->setOnClick([this, depthValueLabel]() { 
        if (config_.simulationDepth > 1) {
            config_.simulationDepth--;
            std::vector<std::string> depthTypes = {"Fast", "Normal", "Epic"};
            depthValueLabel->setText(depthTypes[config_.simulationDepth - 1]);
            std::cout << "[DEBUG] Simulation depth decreased to: " << config_.simulationDepth << " (" << depthTypes[config_.simulationDepth - 1] << ")" << std::endl;
            onParameterChanged();
        }
    });
    addChild(depthDecButton);
    
    auto depthIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthIncButton->setText(">");
    depthIncButton->setPosition(buttonColumnX + 30.0f, paramY);
    depthIncButton->setSize(25.0f, 25.0f);
    depthIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    depthIncButton->setOnClick([this, depthValueLabel]() { 
        if (config_.simulationDepth < 3) {
            config_.simulationDepth++;
            std::vector<std::string> depthTypes = {"Fast", "Normal", "Epic"};
            depthValueLabel->setText(depthTypes[config_.simulationDepth - 1]);
            std::cout << "[DEBUG] Simulation depth increased to: " << config_.simulationDepth << " (" << depthTypes[config_.simulationDepth - 1] << ")" << std::endl;
            onParameterChanged();
        }
    });
    addChild(depthIncButton);
    
    // Update depth value text after creating buttons
    std::vector<std::string> depthTypes = {"Fast", "Normal", "Epic"};
    depthValueLabel->setText(depthTypes[config_.simulationDepth - 1]);
    
    paramY += rowSpacing;
    
    // Geological Activity Parameter
    auto geoLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoLabel->setText("Geological Activity:");
    geoLabel->setPosition(PANEL_MARGIN, paramY);
    geoLabel->autoSizeToText(8.0f);
    geoLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(geoLabel);
    
    auto geoValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoValueLabel->setPosition(valueColumnX, paramY);
    geoValueLabel->autoSizeToText(10.0f);
    geoValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(geoValueLabel);
    
    auto geoDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoDecButton->setText("<");
    geoDecButton->setPosition(buttonColumnX, paramY);
    geoDecButton->setSize(25.0f, 25.0f);
    geoDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    geoDecButton->setOnClick([this, geoValueLabel]() { 
        if (config_.geologicalActivity > 0) {
            config_.geologicalActivity--;
            std::vector<std::string> geoTypes = {"Stable", "Moderate", "Highly Active"};
            geoValueLabel->setText(geoTypes[config_.geologicalActivity]);
            std::cout << "[DEBUG] Geological activity decreased to: " << config_.geologicalActivity << " (" << geoTypes[config_.geologicalActivity] << ")" << std::endl;
            onParameterChanged();
        }
    });
    addChild(geoDecButton);
    
    auto geoIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoIncButton->setText(">");
    geoIncButton->setPosition(buttonColumnX + 30.0f, paramY);
    geoIncButton->setSize(25.0f, 25.0f);
    geoIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    geoIncButton->setOnClick([this, geoValueLabel]() { 
        if (config_.geologicalActivity < 2) {
            config_.geologicalActivity++;
            std::vector<std::string> geoTypes = {"Stable", "Moderate", "Highly Active"};
            geoValueLabel->setText(geoTypes[config_.geologicalActivity]);
            std::cout << "[DEBUG] Geological activity increased to: " << config_.geologicalActivity << " (" << geoTypes[config_.geologicalActivity] << ")" << std::endl;
            onParameterChanged();
        }
    });
    addChild(geoIncButton);
    
    // Update geological activity value text after creating buttons
    std::vector<std::string> geoTypes = {"Stable", "Moderate", "Highly Active"};
    geoValueLabel->setText(geoTypes[config_.geologicalActivity]);
    
    paramY += rowSpacing;
    
    // Enable Civilizations Parameter
    auto civLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    civLabel->setText("Enable Civilizations:");
    civLabel->setPosition(PANEL_MARGIN, paramY);
    civLabel->autoSizeToText(8.0f);
    civLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(civLabel);
    
    auto civToggleButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    civToggleButton->setText(config_.enableCivilizations ? "Enabled" : "Disabled");
    civToggleButton->setPosition(valueColumnX, paramY);
    civToggleButton->autoSizeToText(10.0f);
    civToggleButton->setBackgroundColor(config_.enableCivilizations ? 
        glm::vec4{0.2f, 0.4f, 0.2f, 0.8f} : glm::vec4{0.4f, 0.2f, 0.2f, 0.8f});
    civToggleButton->setOnClick([this, civToggleButton]() { 
        config_.enableCivilizations = !config_.enableCivilizations;
        civToggleButton->setText(config_.enableCivilizations ? "Enabled" : "Disabled");
        civToggleButton->setBackgroundColor(config_.enableCivilizations ? 
            glm::vec4{0.2f, 0.4f, 0.2f, 0.8f} : glm::vec4{0.4f, 0.2f, 0.2f, 0.8f});
        std::cout << "[DEBUG] Civilizations toggled to: " << (config_.enableCivilizations ? "Enabled" : "Disabled") << std::endl;
        onParameterChanged();
    });
    addChild(civToggleButton);
    paramY += rowSpacing + ELEMENT_SPACING;
    
    currentY_ = paramY;
}

void WorldConfigurationUI::createActionButtons() {
    float buttonY = getSize().y - BUTTON_HEIGHT - PANEL_MARGIN;
    
    auto startButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    startButton->setText("Start Generation");
    startButton->setPosition(getSize().x - 280.0f, buttonY);
    startButton->autoSizeToText(16.0f);
    startButton->setBackgroundColor({0.2f, 0.4f, 0.2f, 0.8f});
    startButton->setOnClick([this]() { onStartGenerationClicked(); });
    addChild(startButton);
    
    auto cancelButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    cancelButton->setText("Back to Menu");
    cancelButton->setPosition(getSize().x - 140.0f, buttonY);
    cancelButton->autoSizeToText(16.0f);
    cancelButton->setBackgroundColor({0.4f, 0.2f, 0.2f, 0.8f});
    cancelButton->setOnClick([this]() { onBackClicked(); });
    addChild(cancelButton);
}

float WorldConfigurationUI::calculateMaxParameterLabelWidth() {
    std::vector<std::string> parameterLabels = {
        "World Size:",
        "Climate Type:",
        "Simulation Depth:",
        "Geological Activity:",
        "Enable Civilizations:"
    };
    
    float maxWidth = 0.0f;
    for (const std::string& label : parameterLabels) {
        float textWidth = renderer_->getTextWidth(label);
        float buttonWidth = textWidth + 16.0f; // 8px padding on each side
        maxWidth = std::max(maxWidth, buttonWidth);
    }
    
    return maxWidth;
}

float WorldConfigurationUI::calculateMaxParameterValueWidth() {
    std::vector<std::string> parameterValues = {
        "256x256 regions", "512x512 regions", "1024x1024 regions", "2048x2048 regions",
        "Arctic", "Temperate", "Tropical", "Desert",
        "Fast", "Normal", "Epic",
        "Stable", "Moderate", "Highly Active",
        "Enabled", "Disabled"
    };
    
    float maxWidth = 0.0f;
    for (const std::string& value : parameterValues) {
        float textWidth = renderer_->getTextWidth(value);
        float buttonWidth = textWidth + 20.0f; // 10px padding on each side
        maxWidth = std::max(maxWidth, buttonWidth);
    }
    
    return maxWidth;
}

float WorldConfigurationUI::calculateOptimalRowSpacing() {
    // Base spacing plus consideration for different screen sizes
    float baseSpacing = VERTICAL_SPACING;
    float availableHeight = getSize().y - (TITLE_HEIGHT + PANEL_MARGIN * 3 + BUTTON_HEIGHT);
    float requiredHeight = 5 * baseSpacing; // 5 parameter rows
    
    if (availableHeight > requiredHeight * 1.2f) {
        return baseSpacing * 1.2f; // More generous spacing if we have room
    }
    
    return baseSpacing;
}

void WorldConfigurationUI::onStartGenerationClicked() {
    // Validate world name before proceeding
    validateWorldName();
    
    if (!isWorldNameValid()) {
        std::cout << "[WorldConfigurationUI] Cannot start generation: " << worldNameError_ << std::endl;
        createUIElements(); // Refresh UI to show error
        return;
    }
    
    if (onConfigurationComplete_) {
        onConfigurationComplete_(config_);
    }
}

void WorldConfigurationUI::onBackClicked() {
    if (onBack_) {
        onBack_();
    }
}

void WorldConfigurationUI::onParameterChanged() {
    std::cout << "[WorldConfigurationUI] Parameter changed - world size: " << config_.worldSize 
              << ", climate: " << config_.climateType << ", seed: " << config_.customSeed << std::endl;
    
    // Update world generation objects with new parameters
    initializeWorldGenerationObjects();
}

void WorldConfigurationUI::onWorldNameChanged() {
    // Update config with new world name
    config_.worldName = std::string(worldNameBuffer_);
    
    // Validate the world name
    validateWorldName();
    
    // Recreate UI elements to show/hide error messages
    createUIElements();
}

void WorldConfigurationUI::validateWorldName() {
    worldNameError_.clear();
    worldNameExists_ = false;
    
    std::string trimmedName = config_.worldName;
    
    // Trim whitespace
    size_t start = trimmedName.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        worldNameError_ = "World name cannot be empty";
        return;
    }
    size_t end = trimmedName.find_last_not_of(" \t\n\r");
    trimmedName = trimmedName.substr(start, end - start + 1);
    
    // Check if name is empty after trimming
    if (trimmedName.empty()) {
        worldNameError_ = "World name cannot be empty";
        return;
    }
    
    // Check for invalid characters (basic validation)
    for (char c : trimmedName) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            worldNameError_ = "World name contains invalid characters";
            return;
        }
    }
    
    // Check if world already exists
    try {
        VoxelCastle::World::WorldPersistenceManager worldManager;
        if (worldManager.WorldExists(trimmedName)) {
            worldNameExists_ = true;
            worldNameError_ = "A world with this name already exists";
            return;
        }
    } catch (const std::exception& e) {
        std::cerr << "[WorldConfigurationUI] Error checking world existence: " << e.what() << std::endl;
        // Continue anyway - this is not a critical error
    }
    
    // Update config with trimmed name
    config_.worldName = trimmedName;
    strncpy(worldNameBuffer_, trimmedName.c_str(), sizeof(worldNameBuffer_) - 1);
    worldNameBuffer_[sizeof(worldNameBuffer_) - 1] = '\0';
}

bool WorldConfigurationUI::isWorldNameValid() const {
    return worldNameError_.empty() && !config_.worldName.empty();
}

void WorldConfigurationUI::initializeWorldGenerationObjects() {
    // Generate random seed if none specified
    if (config_.customSeed == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        config_.customSeed = gen();
    }
    
    std::cout << "[WorldConfigurationUI] Initialized world generation with seed: " << config_.customSeed << std::endl;
}
