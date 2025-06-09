#include "ui/WorldConfigurationUI.h"
#include "ui/elements/UIButton.h"
#include <iostream>
#include <random>

WorldConfigurationUI::WorldConfigurationUI(VoxelEngine::UI::UIRenderer* renderer) 
    : BaseMenu(renderer, "World Configuration")
    , currentY_(TITLE_HEIGHT + PANEL_MARGIN)
    , onConfigurationComplete_(nullptr)
    , onBack_(nullptr) {
    
    // Initialize default configuration
    config_.worldSize = 1024;
    config_.climateType = 1; // Temperate
    config_.simulationDepth = 2; // Normal
    config_.geologicalActivity = 1; // Moderate
    config_.hydrologyLevel = 1; // Normal
    config_.enableCivilizations = true;
    config_.customSeed = 0; // Random
}

bool WorldConfigurationUI::initialize(int screenWidth, int screenHeight) {
    // Use larger portion of screen for better usability
    float uiWidth = screenWidth * 0.85f;
    float uiHeight = screenHeight * 0.85f;
    
    setPosition((screenWidth - uiWidth) / 2, (screenHeight - uiHeight) / 2);
    setSize(uiWidth, uiHeight);
    
    std::cout << "[WorldConfigurationUI] Initializing with size: " << getSize().x << "x" << getSize().y 
              << " (screen: " << screenWidth << "x" << screenHeight << ")" << std::endl;
    
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
    createPreviewSection();
    createActionButtons();
}

void WorldConfigurationUI::createParameterControls() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float leftColumnWidth = panelWidth * 0.6f;
    
    // Calculate optimal layout dimensions
    float maxLabelWidth = calculateMaxParameterLabelWidth();
    float maxValueWidth = calculateMaxParameterValueWidth();
    float rowSpacing = calculateOptimalRowSpacing();
    
    float valueColumnX = PANEL_MARGIN + maxLabelWidth + ELEMENT_SPACING;
    float buttonColumnX = valueColumnX + maxValueWidth + ELEMENT_SPACING;
    
    float paramY = currentY_;
    
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
    
    std::vector<std::string> climateTypes = {"Arctic", "Temperate", "Tropical", "Desert"};
    auto climateValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateValueLabel->setText(climateTypes[config_.climateType]);
    climateValueLabel->setPosition(valueColumnX, paramY);
    climateValueLabel->autoSizeToText(8.0f);
    climateValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(climateValueLabel);
    
    auto climateDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateDecButton->setText("<");
    climateDecButton->setPosition(buttonColumnX, paramY);
    climateDecButton->setSize(25.0f, 25.0f);
    climateDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    climateDecButton->setOnClick([this, climateValueLabel, &climateTypes]() { 
        if (config_.climateType > 0) {
            config_.climateType--;
            climateValueLabel->setText(climateTypes[config_.climateType]);
            onParameterChanged();
        }
    });
    addChild(climateDecButton);
    
    auto climateIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateIncButton->setText(">");
    climateIncButton->setPosition(buttonColumnX + 30.0f, paramY);
    climateIncButton->setSize(25.0f, 25.0f);
    climateIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    climateIncButton->setOnClick([this, climateValueLabel, &climateTypes]() { 
        if (config_.climateType < 3) {
            config_.climateType++;
            climateValueLabel->setText(climateTypes[config_.climateType]);
            onParameterChanged();
        }
    });
    addChild(climateIncButton);
    paramY += rowSpacing;
    
    // Simulation Depth Parameter
    auto depthLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthLabel->setText("Simulation Depth:");
    depthLabel->setPosition(PANEL_MARGIN, paramY);
    depthLabel->autoSizeToText(8.0f);
    depthLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(depthLabel);
    
    std::vector<std::string> depthTypes = {"Fast", "Normal", "Epic"};
    auto depthValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthValueLabel->setText(depthTypes[config_.simulationDepth - 1]);
    depthValueLabel->setPosition(valueColumnX, paramY);
    depthValueLabel->autoSizeToText(8.0f);
    depthValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(depthValueLabel);
    
    auto depthDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthDecButton->setText("<");
    depthDecButton->setPosition(buttonColumnX, paramY);
    depthDecButton->setSize(25.0f, 25.0f);
    depthDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    depthDecButton->setOnClick([this, depthValueLabel, &depthTypes]() { 
        if (config_.simulationDepth > 1) {
            config_.simulationDepth--;
            depthValueLabel->setText(depthTypes[config_.simulationDepth - 1]);
            onParameterChanged();
        }
    });
    addChild(depthDecButton);
    
    auto depthIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthIncButton->setText(">");
    depthIncButton->setPosition(buttonColumnX + 30.0f, paramY);
    depthIncButton->setSize(25.0f, 25.0f);
    depthIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    depthIncButton->setOnClick([this, depthValueLabel, &depthTypes]() { 
        if (config_.simulationDepth < 3) {
            config_.simulationDepth++;
            depthValueLabel->setText(depthTypes[config_.simulationDepth - 1]);
            onParameterChanged();
        }
    });
    addChild(depthIncButton);
    paramY += rowSpacing;
    
    // Geological Activity Parameter
    auto geoLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoLabel->setText("Geological Activity:");
    geoLabel->setPosition(PANEL_MARGIN, paramY);
    geoLabel->autoSizeToText(8.0f);
    geoLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(geoLabel);
    
    std::vector<std::string> geoTypes = {"Stable", "Moderate", "Highly Active"};
    auto geoValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoValueLabel->setText(geoTypes[config_.geologicalActivity]);
    geoValueLabel->setPosition(valueColumnX, paramY);
    geoValueLabel->autoSizeToText(10.0f);
    geoValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(geoValueLabel);
    
    auto geoDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoDecButton->setText("<");
    geoDecButton->setPosition(buttonColumnX, paramY);
    geoDecButton->setSize(25.0f, 25.0f);
    geoDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    geoDecButton->setOnClick([this, geoValueLabel, &geoTypes]() { 
        if (config_.geologicalActivity > 0) {
            config_.geologicalActivity--;
            geoValueLabel->setText(geoTypes[config_.geologicalActivity]);
            onParameterChanged();
        }
    });
    addChild(geoDecButton);
    
    auto geoIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoIncButton->setText(">");
    geoIncButton->setPosition(buttonColumnX + 30.0f, paramY);
    geoIncButton->setSize(25.0f, 25.0f);
    geoIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    geoIncButton->setOnClick([this, geoValueLabel, &geoTypes]() { 
        if (config_.geologicalActivity < 2) {
            config_.geologicalActivity++;
            geoValueLabel->setText(geoTypes[config_.geologicalActivity]);
            onParameterChanged();
        }
    });
    addChild(geoIncButton);
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
        onParameterChanged();
    });
    addChild(civToggleButton);
    paramY += rowSpacing + ELEMENT_SPACING;
    
    currentY_ = paramY;
}

void WorldConfigurationUI::createPreviewSection() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Calculate right column position based on parameter controls layout
    // Need to account for: label + spacing + value + spacing + two buttons (- and +)
    float maxLabelWidth = calculateMaxParameterLabelWidth();
    float maxValueWidth = calculateMaxParameterValueWidth();
    float buttonColumnX = PANEL_MARGIN + maxLabelWidth + ELEMENT_SPACING + maxValueWidth + ELEMENT_SPACING;
    float rightmostButtonX = buttonColumnX + 30.0f + TEXT_HEIGHT; // Position of + button + its width
    
    // Start right column with adequate spacing after the rightmost button
    float rightColumnX = rightmostButtonX + ELEMENT_SPACING * 2; // Extra spacing for visual separation
    float rightColumnWidth = panelWidth - (rightColumnX - PANEL_MARGIN);
    
    // Make sure we don't exceed panel bounds
    if (rightColumnX + rightColumnWidth > PANEL_MARGIN + panelWidth) {
        rightColumnWidth = PANEL_MARGIN + panelWidth - rightColumnX;
    }
    
    // Preview label
    auto previewLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    previewLabel->setText("World Preview");
    previewLabel->setPosition(rightColumnX, TITLE_HEIGHT + PANEL_MARGIN);
    previewLabel->setSize(rightColumnWidth, TEXT_HEIGHT);
    previewLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(previewLabel);
    
    // Preview panel
    auto previewPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    previewPanel->setText("[Preview Map Area]");
    previewPanel->setPosition(rightColumnX, TITLE_HEIGHT + PANEL_MARGIN + TEXT_HEIGHT + 5.0f);
    previewPanel->setSize(rightColumnWidth, 180.0f);
    previewPanel->setBackgroundColor({0.15f, 0.2f, 0.25f, 0.7f});
    addChild(previewPanel);
    
    // Seed controls
    float seedY = TITLE_HEIGHT + PANEL_MARGIN + TEXT_HEIGHT + 190.0f;
    
    auto seedLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    seedLabel->setText("Random Seed:");
    seedLabel->setPosition(rightColumnX, seedY);
    seedLabel->autoSizeToText(8.0f);
    seedLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(seedLabel);
    
    auto seedValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    seedValueLabel->setText(config_.customSeed == 0 ? "Random" : std::to_string(config_.customSeed));
    seedValueLabel->setPosition(rightColumnX, seedY + 25.0f);
    seedValueLabel->autoSizeToText(10.0f);
    seedValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(seedValueLabel);
    
    auto newSeedButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    newSeedButton->setText("New Seed");
    newSeedButton->setPosition(rightColumnX + rightColumnWidth - 80.0f, seedY + 25.0f);
    newSeedButton->autoSizeToText(8.0f);
    newSeedButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    newSeedButton->setOnClick([this, seedValueLabel]() { 
        std::random_device rd;
        config_.customSeed = rd();
        seedValueLabel->setText(std::to_string(config_.customSeed));
        onParameterChanged();
    });
    addChild(newSeedButton);
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
    // Could trigger preview updates in the future
    std::cout << "[WorldConfigurationUI] Parameter changed" << std::endl;
}
