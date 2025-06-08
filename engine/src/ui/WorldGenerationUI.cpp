#include "ui/WorldGenerationUI.h"
#include "ui/MenuSystem.h"
#include "world/biome/biome_registry.h"
#include "world/biome/biome_types.h"
#include "utils/debug_logger_stub.h"
#include "ui/elements/UIButton.h"
#include "ui/elements/UIPanel.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

namespace VoxelCastle::UI {

WorldGenerationUI::WorldGenerationUI(VoxelEngine::UI::UIRenderer* renderer) 
    : BaseMenu(renderer, "World Generation") {
    // Initialize default configuration
    config_.worldSize = 1024;
    config_.simulationDepth = 2;
    config_.climateType = 1;
    config_.geologicalActivity = 1;
    config_.hydrologyLevel = 1;
    config_.customSeed = 0;
    config_.enableCivilizations = true;
    
    // Initialize log with welcome message
    AddLogEntry("Welcome to World Generation", 0);
    AddLogEntry("Configure your world parameters and click 'Start Generation'", 0);
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
    
    // Set large size for comprehensive UI
    setSize(900.0f, 700.0f);
    
    std::cout << "[WorldGenerationUI] Initializing with size: " << getSize().x << "x" << getSize().y << std::endl;
    
    createUIElements();
    setVisible(true);
    
    return true;
}

void WorldGenerationUI::createUIElements() {
    removeAllChildren();
    currentY_ = TITLE_HEIGHT + PANEL_MARGIN;
    
    switch (currentPhase_) {
        case GenerationPhase::CONFIGURATION:
            createConfigurationUI();
            break;
        case GenerationPhase::COMPLETE:
            createWorldSummaryUI();
            break;
        default:
            createGenerationUI();
            break;
    }
}

void WorldGenerationUI::createConfigurationUI() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float leftColumnWidth = panelWidth * 0.6f;
    float rightColumnWidth = panelWidth * 0.35f;
    float rightColumnX = PANEL_MARGIN + leftColumnWidth + PANEL_MARGIN;
    
    // Title
    auto titleLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    titleLabel->setText("World Configuration");
    titleLabel->setPosition(PANEL_MARGIN, currentY_);
    titleLabel->setSize(panelWidth, 30.0f);
    titleLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(titleLabel);
    currentY_ += 40.0f;
    
    // Left column - World Parameters with interactive controls
    float paramY = currentY_;
    
    // World Size with controls
    auto sizeLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeLabel->setText("World Size:");
    sizeLabel->setPosition(PANEL_MARGIN, paramY);
    sizeLabel->setSize(200.0f, 25.0f);
    sizeLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(sizeLabel);
    
    auto sizeValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeValueLabel->setText(std::to_string(config_.worldSize) + "x" + std::to_string(config_.worldSize) + " regions");
    sizeValueLabel->setPosition(PANEL_MARGIN + 220.0f, paramY);
    sizeValueLabel->setSize(140.0f, 25.0f);
    sizeValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(sizeValueLabel);
    
    // Size adjustment buttons
    auto sizeDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeDecButton->setText("-");
    sizeDecButton->setPosition(PANEL_MARGIN + 370.0f, paramY);
    sizeDecButton->setSize(25.0f, 25.0f);
    sizeDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    sizeDecButton->setOnClick([this]() { 
        if (config_.worldSize > 256) {
            config_.worldSize = config_.worldSize == 512 ? 256 : (config_.worldSize == 1024 ? 512 : config_.worldSize);
            createUIElements();
        }
    });
    addChild(sizeDecButton);
    
    auto sizeIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    sizeIncButton->setText("+");
    sizeIncButton->setPosition(PANEL_MARGIN + 400.0f, paramY);
    sizeIncButton->setSize(25.0f, 25.0f);
    sizeIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    sizeIncButton->setOnClick([this]() { 
        if (config_.worldSize < 2048) {
            config_.worldSize = config_.worldSize == 256 ? 512 : (config_.worldSize == 512 ? 1024 : (config_.worldSize == 1024 ? 2048 : config_.worldSize));
            createUIElements();
        }
    });
    addChild(sizeIncButton);
    paramY += 35.0f;
    
    // Climate Type with controls
    auto climateLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateLabel->setText("Climate:");
    climateLabel->setPosition(PANEL_MARGIN, paramY);
    climateLabel->setSize(200.0f, 25.0f);
    climateLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(climateLabel);
    
    std::vector<std::string> climateTypes = {"Arctic", "Temperate", "Tropical", "Desert"};
    auto climateValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateValueLabel->setText(climateTypes[config_.climateType]);
    climateValueLabel->setPosition(PANEL_MARGIN + 220.0f, paramY);
    climateValueLabel->setSize(140.0f, 25.0f);
    climateValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(climateValueLabel);
    
    // Climate adjustment buttons
    auto climateDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateDecButton->setText("<");
    climateDecButton->setPosition(PANEL_MARGIN + 370.0f, paramY);
    climateDecButton->setSize(25.0f, 25.0f);
    climateDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    climateDecButton->setOnClick([this]() { 
        if (config_.climateType > 0) {
            config_.climateType--;
            createUIElements();
        }
    });
    addChild(climateDecButton);
    
    auto climateIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    climateIncButton->setText(">");
    climateIncButton->setPosition(PANEL_MARGIN + 400.0f, paramY);
    climateIncButton->setSize(25.0f, 25.0f);
    climateIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    climateIncButton->setOnClick([this]() { 
        if (config_.climateType < 3) {
            config_.climateType++;
            createUIElements();
        }
    });
    addChild(climateIncButton);
    paramY += 35.0f;
    
    // Simulation Depth with controls
    auto depthLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthLabel->setText("Simulation Depth:");
    depthLabel->setPosition(PANEL_MARGIN, paramY);
    depthLabel->setSize(200.0f, 25.0f);
    depthLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(depthLabel);
    
    std::vector<std::string> depthTypes = {"Fast", "Normal", "Epic"};
    auto depthValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthValueLabel->setText(depthTypes[config_.simulationDepth - 1]);
    depthValueLabel->setPosition(PANEL_MARGIN + 220.0f, paramY);
    depthValueLabel->setSize(140.0f, 25.0f);
    depthValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(depthValueLabel);
    
    // Depth adjustment buttons
    auto depthDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthDecButton->setText("<");
    depthDecButton->setPosition(PANEL_MARGIN + 370.0f, paramY);
    depthDecButton->setSize(25.0f, 25.0f);
    depthDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    depthDecButton->setOnClick([this]() { 
        if (config_.simulationDepth > 1) {
            config_.simulationDepth--;
            createUIElements();
        }
    });
    addChild(depthDecButton);
    
    auto depthIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    depthIncButton->setText(">");
    depthIncButton->setPosition(PANEL_MARGIN + 400.0f, paramY);
    depthIncButton->setSize(25.0f, 25.0f);
    depthIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    depthIncButton->setOnClick([this]() { 
        if (config_.simulationDepth < 3) {
            config_.simulationDepth++;
            createUIElements();
        }
    });
    addChild(depthIncButton);
    paramY += 35.0f;
    
    // Geological Activity with controls
    auto geoLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoLabel->setText("Geological Activity:");
    geoLabel->setPosition(PANEL_MARGIN, paramY);
    geoLabel->setSize(200.0f, 25.0f);
    geoLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(geoLabel);
    
    std::vector<std::string> geoTypes = {"Stable", "Moderate", "Highly Active"};
    auto geoValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoValueLabel->setText(geoTypes[config_.geologicalActivity]);
    geoValueLabel->setPosition(PANEL_MARGIN + 220.0f, paramY);
    geoValueLabel->setSize(140.0f, 25.0f);
    geoValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(geoValueLabel);
    
    // Geological adjustment buttons
    auto geoDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoDecButton->setText("<");
    geoDecButton->setPosition(PANEL_MARGIN + 370.0f, paramY);
    geoDecButton->setSize(25.0f, 25.0f);
    geoDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    geoDecButton->setOnClick([this]() { 
        if (config_.geologicalActivity > 0) {
            config_.geologicalActivity--;
            createUIElements();
        }
    });
    addChild(geoDecButton);
    
    auto geoIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    geoIncButton->setText(">");
    geoIncButton->setPosition(PANEL_MARGIN + 400.0f, paramY);
    geoIncButton->setSize(25.0f, 25.0f);
    geoIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    geoIncButton->setOnClick([this]() { 
        if (config_.geologicalActivity < 2) {
            config_.geologicalActivity++;
            createUIElements();
        }
    });
    addChild(geoIncButton);
    paramY += 35.0f;
    
    // Civilization toggle
    auto civLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    civLabel->setText("Enable Civilizations:");
    civLabel->setPosition(PANEL_MARGIN, paramY);
    civLabel->setSize(200.0f, 25.0f);
    civLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(civLabel);
    
    auto civToggleButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    civToggleButton->setText(config_.enableCivilizations ? "Enabled" : "Disabled");
    civToggleButton->setPosition(PANEL_MARGIN + 220.0f, paramY);
    civToggleButton->setSize(140.0f, 25.0f);
    civToggleButton->setBackgroundColor(config_.enableCivilizations ? 
        glm::vec4{0.2f, 0.4f, 0.2f, 0.8f} : glm::vec4{0.4f, 0.2f, 0.2f, 0.8f});
    civToggleButton->setOnClick([this]() { 
        config_.enableCivilizations = !config_.enableCivilizations;
        createUIElements();
    });
    addChild(civToggleButton);
    paramY += 50.0f;
    
    // Right column - Preview and Random Seed
    auto previewLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    previewLabel->setText("World Preview");
    previewLabel->setPosition(rightColumnX, currentY_);
    previewLabel->setSize(rightColumnWidth, 25.0f);
    previewLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(previewLabel);
    
    // Placeholder for world preview visualization
    auto previewPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    previewPanel->setText("[Preview Map Area]");
    previewPanel->setPosition(rightColumnX, currentY_ + 35.0f);
    previewPanel->setSize(rightColumnWidth, 150.0f);
    previewPanel->setBackgroundColor({0.15f, 0.2f, 0.25f, 0.7f});
    addChild(previewPanel);
    
    // Seed controls
    auto seedLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    seedLabel->setText("Random Seed:");
    seedLabel->setPosition(rightColumnX, currentY_ + 200.0f);
    seedLabel->setSize(rightColumnWidth, 20.0f);
    seedLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(seedLabel);
    
    auto seedValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    seedValueLabel->setText(config_.customSeed == 0 ? "Random" : std::to_string(config_.customSeed));
    seedValueLabel->setPosition(rightColumnX, currentY_ + 225.0f);
    seedValueLabel->setSize(rightColumnWidth - 60.0f, 25.0f);
    seedValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(seedValueLabel);
    
    auto newSeedButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    newSeedButton->setText("New");
    newSeedButton->setPosition(rightColumnX + rightColumnWidth - 55.0f, currentY_ + 225.0f);
    newSeedButton->setSize(50.0f, 25.0f);
    newSeedButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    newSeedButton->setOnClick([this]() { 
        std::random_device rd;
        config_.customSeed = rd();
        createUIElements();
    });
    addChild(newSeedButton);
    
    // Action buttons at bottom
    float buttonY = getSize().y - 80.0f;
    float buttonWidth = 120.0f;
    
    auto startButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    startButton->setText("Start Generation");
    startButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN, buttonY);
    startButton->setSize(buttonWidth, BUTTON_HEIGHT);
    startButton->setBackgroundColor({0.2f, 0.4f, 0.2f, 0.8f});
    startButton->setOnClick([this]() { OnStartClicked(); });
    addChild(startButton);
    
    auto cancelButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    cancelButton->setText("Back");
    cancelButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN, buttonY);
    cancelButton->setSize(buttonWidth, BUTTON_HEIGHT);
    cancelButton->setBackgroundColor({0.4f, 0.2f, 0.2f, 0.8f});
    cancelButton->setOnClick([this]() { OnBackToMainMenuClicked(); });
    addChild(cancelButton);
}

void WorldGenerationUI::createGenerationUI() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Visualization controls at top
    auto vizControlsLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    vizControlsLabel->setText("Visualization Mode:");
    vizControlsLabel->setPosition(PANEL_MARGIN, currentY_);
    vizControlsLabel->setSize(150.0f, 20.0f);
    vizControlsLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(vizControlsLabel);
    
    // Visualization mode buttons
    float vizButtonX = PANEL_MARGIN + 160.0f;
    std::vector<std::pair<std::string, VisualizationMode>> vizModes = {
        {"Elevation", VisualizationMode::ELEVATION},
        {"Temperature", VisualizationMode::TEMPERATURE},
        {"Precipitation", VisualizationMode::PRECIPITATION},
        {"Biomes", VisualizationMode::BIOMES},
        {"Hydrology", VisualizationMode::HYDROLOGY}
    };
    
    for (const auto& mode : vizModes) {
        auto modeButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        modeButton->setText(mode.first);
        modeButton->setPosition(vizButtonX, currentY_);
        modeButton->setSize(80.0f, 20.0f);
        bool isActive = (visualizationMode_ == mode.second);
        modeButton->setBackgroundColor(isActive ? 
            glm::vec4{0.3f, 0.5f, 0.3f, 0.8f} : glm::vec4{0.2f, 0.2f, 0.2f, 0.6f});
        modeButton->setOnClick([this, mode]() { 
            OnVisualizationModeChanged(mode.second); 
            createUIElements(); 
        });
        addChild(modeButton);
        vizButtonX += 85.0f;
    }
    currentY_ += 30.0f;
    
    // Speed controls
    auto speedLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    speedLabel->setText("Generation Speed:");
    speedLabel->setPosition(PANEL_MARGIN, currentY_);
    speedLabel->setSize(150.0f, 20.0f);
    speedLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(speedLabel);
    
    auto speedValueLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    speedValueLabel->setText(std::to_string((int)(generationSpeed_ * 100)) + "%");
    speedValueLabel->setPosition(PANEL_MARGIN + 160.0f, currentY_);
    speedValueLabel->setSize(60.0f, 20.0f);
    speedValueLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(speedValueLabel);
    
    // Speed adjustment buttons
    auto speedDecButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    speedDecButton->setText("-");
    speedDecButton->setPosition(PANEL_MARGIN + 230.0f, currentY_);
    speedDecButton->setSize(20.0f, 20.0f);
    speedDecButton->setBackgroundColor({0.3f, 0.2f, 0.2f, 0.8f});
    speedDecButton->setOnClick([this]() { 
        generationSpeed_ = std::max(0.25f, generationSpeed_ - 0.25f);
        createUIElements();
    });
    addChild(speedDecButton);
    
    auto speedIncButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    speedIncButton->setText("+");
    speedIncButton->setPosition(PANEL_MARGIN + 255.0f, currentY_);
    speedIncButton->setSize(20.0f, 20.0f);
    speedIncButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    speedIncButton->setOnClick([this]() { 
        generationSpeed_ = std::min(4.0f, generationSpeed_ + 0.25f);
        createUIElements();
    });
    addChild(speedIncButton);
    currentY_ += 35.0f;
    
    // Main visualization area (large central panel)
    float vizWidth = panelWidth * 0.55f;
    float vizHeight = 300.0f;
    
    auto vizLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    vizLabel->setText("World Visualization - " + GetVisualizationModeDisplayName(visualizationMode_));
    vizLabel->setPosition(PANEL_MARGIN, currentY_);
    vizLabel->setSize(vizWidth, 25.0f);
    vizLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(vizLabel);
    
    auto vizPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    std::string vizText = "[" + GetPhaseDisplayName(currentPhase_) + " - " + 
                         GetVisualizationModeDisplayName(visualizationMode_) + " View]";
    vizPanel->setText(vizText);
    vizPanel->setPosition(PANEL_MARGIN, currentY_ + 30.0f);
    vizPanel->setSize(vizWidth, vizHeight);
    vizPanel->setBackgroundColor({0.15f, 0.2f, 0.25f, 0.8f});
    addChild(vizPanel);
    
    // Progress panel (right side)
    float progressX = PANEL_MARGIN + vizWidth + PANEL_MARGIN;
    float progressWidth = panelWidth - vizWidth - PANEL_MARGIN;
    float progressY = currentY_;
    
    auto progressLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    progressLabel->setText("Generation Progress");
    progressLabel->setPosition(progressX, progressY);
    progressLabel->setSize(progressWidth, 25.0f);
    progressLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(progressLabel);
    progressY += 35.0f;
    
    // Current phase with time estimation
    auto phaseLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    phaseLabel->setText("Phase: " + GetPhaseDisplayName(currentPhase_));
    phaseLabel->setPosition(progressX, progressY);
    phaseLabel->setSize(progressWidth, 20.0f);
    phaseLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(phaseLabel);
    progressY += 25.0f;
    
    // Time remaining estimate
    float timeRemaining = CalculateTimeRemaining();
    auto timeLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    timeLabel->setText("Est. Time: " + FormatTimeRemaining(timeRemaining));
    timeLabel->setPosition(progressX, progressY);
    timeLabel->setSize(progressWidth, 18.0f);
    timeLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.5f});
    addChild(timeLabel);
    progressY += 25.0f;
    
    // Progress bars with visual indicators
    auto overallProgressLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    overallProgressLabel->setText("Overall: " + std::to_string((int)(generationProgress_ * 100)) + "%");
    overallProgressLabel->setPosition(progressX, progressY);
    overallProgressLabel->setSize(progressWidth, 20.0f);
    float overallHue = generationProgress_ * 0.3f; // Green when complete
    overallProgressLabel->setBackgroundColor({0.1f + overallHue, 0.1f + overallHue * 2, 0.1f, 0.6f});
    addChild(overallProgressLabel);
    progressY += 25.0f;
    
    auto phaseProgressLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    phaseProgressLabel->setText("Current Phase: " + std::to_string((int)(phaseProgress_ * 100)) + "%");
    phaseProgressLabel->setPosition(progressX, progressY);
    phaseProgressLabel->setSize(progressWidth, 20.0f);
    float phaseHue = phaseProgress_ * 0.3f;
    phaseProgressLabel->setBackgroundColor({0.1f + phaseHue, 0.1f + phaseHue * 2, 0.1f, 0.6f});
    addChild(phaseProgressLabel);
    progressY += 35.0f;
    
    // Enhanced Statistics
    auto statsLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    statsLabel->setText("Live Statistics:");
    statsLabel->setPosition(progressX, progressY);
    statsLabel->setSize(progressWidth, 20.0f);
    statsLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.7f});
    addChild(statsLabel);
    progressY += 25.0f;
    
    auto mountainsLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    mountainsLabel->setText("Mountains: " + std::to_string(stats_.mountainRanges));
    mountainsLabel->setPosition(progressX, progressY);
    mountainsLabel->setSize(progressWidth, 18.0f);
    mountainsLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.5f});
    addChild(mountainsLabel);
    progressY += 20.0f;
    
    auto riversLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    riversLabel->setText("Rivers: " + std::to_string(stats_.majorRivers));
    riversLabel->setPosition(progressX, progressY);
    riversLabel->setSize(progressWidth, 18.0f);
    riversLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.5f});
    addChild(riversLabel);
    progressY += 20.0f;
    
    auto biomesLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    biomesLabel->setText("Biomes: " + std::to_string(stats_.biomesIdentified));
    biomesLabel->setPosition(progressX, progressY);
    biomesLabel->setSize(progressWidth, 18.0f);
    biomesLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.5f});
    addChild(biomesLabel);
    progressY += 20.0f;
    
    auto yearsLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    yearsLabel->setText("Sim Years: " + std::to_string(stats_.simulationYears));
    yearsLabel->setPosition(progressX, progressY);
    yearsLabel->setSize(progressWidth, 18.0f);
    yearsLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.5f});
    addChild(yearsLabel);
    
    // Generation log (bottom panel)
    float logY = currentY_ + vizHeight + 50.0f;
    auto logLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    logLabel->setText("Generation Log (Recent):");
    logLabel->setPosition(PANEL_MARGIN, logY);
    logLabel->setSize(panelWidth, 20.0f);
    logLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(logLabel);
    logY += 25.0f;
    
    // Show recent log entries with better formatting
    int entriesToShow = std::min(6, (int)generationLog_.size());
    for (int i = generationLog_.size() - entriesToShow; i < (int)generationLog_.size(); i++) {
        if (i >= 0) {
            const auto& entry = generationLog_[i];
            std::string logText = "[" + std::to_string(entry.simulationYear) + "] " + entry.message;
            
            auto logEntryLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
            logEntryLabel->setText(logText);
            logEntryLabel->setPosition(PANEL_MARGIN + 10.0f, logY);
            logEntryLabel->setSize(panelWidth - 20.0f, 18.0f);
            // Color code log entries by recency
            float recency = (float)(i - (generationLog_.size() - entriesToShow)) / entriesToShow;
            logEntryLabel->setBackgroundColor({0.05f + recency * 0.1f, 0.1f + recency * 0.1f, 0.05f, 0.4f + recency * 0.2f});
            addChild(logEntryLabel);
            logY += 20.0f;
        }
    }
    
    // Enhanced control buttons
    float buttonY = getSize().y - 50.0f;
    float buttonX = PANEL_MARGIN;
    
    auto pauseButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    pauseButton->setText(isPaused_ ? "Resume" : "Pause");
    pauseButton->setPosition(buttonX, buttonY);
    pauseButton->setSize(80.0f, BUTTON_HEIGHT);
    pauseButton->setBackgroundColor(isPaused_ ? 
        glm::vec4{0.2f, 0.4f, 0.2f, 0.8f} : glm::vec4{0.4f, 0.3f, 0.2f, 0.8f});
    pauseButton->setOnClick([this]() { OnPauseResumeClicked(); });
    addChild(pauseButton);
    buttonX += 90.0f;
    
    auto cancelButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    cancelButton->setText("Cancel");
    cancelButton->setPosition(buttonX, buttonY);
    cancelButton->setSize(80.0f, BUTTON_HEIGHT);
    cancelButton->setBackgroundColor({0.4f, 0.2f, 0.2f, 0.8f});
    cancelButton->setOnClick([this]() { OnCancelClicked(); });
    addChild(cancelButton);
    buttonX += 90.0f;
    
    // Add export button for current state
    auto exportButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    exportButton->setText("Export");
    exportButton->setPosition(buttonX, buttonY);
    exportButton->setSize(80.0f, BUTTON_HEIGHT);
    exportButton->setBackgroundColor({0.2f, 0.2f, 0.4f, 0.8f});
    exportButton->setOnClick([this]() { OnExportCurrentStateClicked(); });
    addChild(exportButton);
}

void WorldGenerationUI::createWorldSummaryUI() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Title
    auto titleLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    titleLabel->setText("World Generation Complete!");
    titleLabel->setPosition(PANEL_MARGIN, currentY_);
    titleLabel->setSize(panelWidth, 30.0f);
    titleLabel->setBackgroundColor({0.2f, 0.4f, 0.2f, 0.8f});
    addChild(titleLabel);
    currentY_ += 50.0f;
    
    // World overview map
    float mapWidth = panelWidth * 0.6f;
    float mapHeight = 300.0f;
    
    auto mapLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    mapLabel->setText("Final World Map");
    mapLabel->setPosition(PANEL_MARGIN, currentY_);
    mapLabel->setSize(mapWidth, 25.0f);
    mapLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(mapLabel);
    
    auto mapPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    mapPanel->setText("[Interactive World Map]");
    mapPanel->setPosition(PANEL_MARGIN, currentY_ + 30.0f);
    mapPanel->setSize(mapWidth, mapHeight);
    mapPanel->setBackgroundColor({0.15f, 0.25f, 0.2f, 0.8f});
    addChild(mapPanel);
    
    // World statistics (right side)
    float statsX = PANEL_MARGIN + mapWidth + PANEL_MARGIN;
    float statsWidth = panelWidth - mapWidth - PANEL_MARGIN;
    float statsY = currentY_;
    
    auto statsLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    statsLabel->setText("World Statistics");
    statsLabel->setPosition(statsX, statsY);
    statsLabel->setSize(statsWidth, 25.0f);
    statsLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(statsLabel);
    statsY += 35.0f;
    
    // Notable features
    if (!stats_.highestPeakName.empty()) {
        auto peakLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        peakLabel->setText("Highest Peak: " + stats_.highestPeakName);
        peakLabel->setPosition(statsX, statsY);
        peakLabel->setSize(statsWidth, 20.0f);
        peakLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
        addChild(peakLabel);
        statsY += 25.0f;
    }
    
    if (!stats_.longestRiverName.empty()) {
        auto riverLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        riverLabel->setText("Longest River: " + stats_.longestRiverName);
        riverLabel->setPosition(statsX, statsY);
        riverLabel->setSize(statsWidth, 20.0f);
        riverLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
        addChild(riverLabel);
        statsY += 25.0f;
    }
    
    // Final statistics
    auto finalStatsLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    finalStatsLabel->setText("Total Generation Time: " + FormatTimeRemaining(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - generationStartTime_).count()));
    finalStatsLabel->setPosition(statsX, statsY);
    finalStatsLabel->setSize(statsWidth, 20.0f);
    finalStatsLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(finalStatsLabel);
    
    // Action buttons
    float buttonY = getSize().y - 60.0f;
    float buttonWidth = 120.0f;
    
    auto beginGameButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    beginGameButton->setText("Begin Game");
    beginGameButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN, buttonY);
    beginGameButton->setSize(buttonWidth, BUTTON_HEIGHT);
    beginGameButton->setOnClick([this]() { OnBeginGameClicked(); });
    addChild(beginGameButton);
    
    auto regenerateButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    regenerateButton->setText("Regenerate");
    regenerateButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN, buttonY);
    regenerateButton->setSize(buttonWidth, BUTTON_HEIGHT);
    regenerateButton->setOnClick([this]() { 
        currentPhase_ = GenerationPhase::CONFIGURATION;
        createUIElements();
    });
    addChild(regenerateButton);
    
    auto backButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    backButton->setText("Main Menu");
    backButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN, buttonY);
    backButton->setSize(buttonWidth, BUTTON_HEIGHT);
    backButton->setOnClick([this]() { OnBackToMainMenuClicked(); });
    addChild(backButton);
}

void WorldGenerationUI::update(float deltaTime) {
    BaseMenu::update(deltaTime);
    
    if (isGenerating_ && !isPaused_) {
        UpdateGeneration(deltaTime * generationSpeed_);
    }
}

void WorldGenerationUI::render() {
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
    phaseProgress_ = 0.0f;
    generationStartTime_ = std::chrono::steady_clock::now();
    phaseStartTime_ = std::chrono::steady_clock::now();
    currentPhase_ = GenerationPhase::TECTONICS;
    isPaused_ = false;
    
    // Initialize world generation components
    worldSeed_ = std::make_shared<VoxelCastle::World::WorldSeed>();
    worldParameters_ = std::make_shared<VoxelCastle::World::WorldParameters>();
    worldGenerator_ = std::make_shared<VoxelCastle::World::SeedWorldGenerator>(worldSeed_, worldParameters_);
    
    // Reset statistics
    stats_ = WorldStats();
    
    // Clear log and add initial entries
    generationLog_.clear();
    AddLogEntry("World generation started with seed: " + std::to_string(config_.customSeed), 0);
    AddLogEntry("Beginning tectonic simulation...", 0);
    
    // Recreate UI for generation phase
    createUIElements();
    
    std::cout << "[WorldGenerationUI] Starting world generation..." << std::endl;
}

void WorldGenerationUI::CancelGeneration() {
    if (!isGenerating_) {
        return;
    }
    
    isGenerating_ = false;
    generationComplete_ = false;
    currentPhase_ = GenerationPhase::CONFIGURATION;
    createUIElements();
    
    std::cout << "[WorldGenerationUI] World generation cancelled" << std::endl;
}

void WorldGenerationUI::SetCompletionCallback(std::function<void(std::shared_ptr<VoxelCastle::World::SeedWorldGenerator>)> callback) {
    completionCallback_ = callback;
}

void WorldGenerationUI::UpdateGeneration(float deltaTime) {
    if (currentPhase_ == GenerationPhase::COMPLETE) {
        return;
    }
    
    // Simulate current phase
    SimulatePhase(currentPhase_, deltaTime);
    
    // Update phase progress
    float phaseExpectedDuration = GetPhaseExpectedDuration(currentPhase_);
    auto phaseElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - phaseStartTime_).count() / 1000.0f;
    
    phaseProgress_ = std::min(1.0f, phaseElapsed / phaseExpectedDuration);
    
    // Check if phase is complete
    if (phaseProgress_ >= 1.0f) {
        AdvancePhase();
    }
    
    // Update overall progress
    int totalPhases = 6; // TECTONICS through CIVILIZATION
    int currentPhaseIndex = static_cast<int>(currentPhase_) - 1; // -1 because CONFIGURATION is 0
    
    if (currentPhaseIndex >= 0) {
        generationProgress_ = (currentPhaseIndex + phaseProgress_) / totalPhases;
    }
    
    // Recreate UI to update progress displays
    static float lastUIUpdate = 0.0f;
    lastUIUpdate += deltaTime;
    if (lastUIUpdate >= 0.5f) { // Update UI every 0.5 seconds
        createUIElements();
        lastUIUpdate = 0.0f;
    }
}

void WorldGenerationUI::AdvancePhase() {
    switch (currentPhase_) {
        case GenerationPhase::TECTONICS:
            currentPhase_ = GenerationPhase::EROSION;
            AddLogEntry("Tectonic simulation complete. Beginning erosion simulation...", stats_.simulationYears);
            break;
        case GenerationPhase::EROSION:
            currentPhase_ = GenerationPhase::HYDROLOGY;
            AddLogEntry("Erosion simulation complete. Generating river networks...", stats_.simulationYears);
            break;
        case GenerationPhase::HYDROLOGY:
            currentPhase_ = GenerationPhase::CLIMATE;
            AddLogEntry("Hydrology complete. Calculating climate patterns...", stats_.simulationYears);
            break;
        case GenerationPhase::CLIMATE:
            currentPhase_ = GenerationPhase::BIOMES;
            AddLogEntry("Climate simulation complete. Assigning biomes...", stats_.simulationYears);
            break;
        case GenerationPhase::BIOMES:
            if (config_.enableCivilizations) {
                currentPhase_ = GenerationPhase::CIVILIZATION;
                AddLogEntry("Biome assignment complete. Simulating ancient civilizations...", stats_.simulationYears);
            } else {
                CompleteGeneration();
                return;
            }
            break;
        case GenerationPhase::CIVILIZATION:
            CompleteGeneration();
            return;
        default:
            break;
    }
    
    phaseProgress_ = 0.0f;
    phaseStartTime_ = std::chrono::steady_clock::now();
}

void WorldGenerationUI::CompleteGeneration() {
    isGenerating_ = false;
    generationComplete_ = true;
    currentPhase_ = GenerationPhase::COMPLETE;
    generationProgress_ = 1.0f;
    
    AddLogEntry("World generation complete! Your world awaits exploration.", stats_.simulationYears);
    
    // Generate final world features
    stats_.highestPeakName = "Mount Skyreach";
    stats_.longestRiverName = "The Lifeflow";
    stats_.largestLakeName = "Mirrormere";
    stats_.deepestValleyName = "Shadowrift Gorge";
    
    createUIElements();
    
    std::cout << "[WorldGenerationUI] World generation complete!" << std::endl;
    
    // Call completion callback
    if (completionCallback_ && worldGenerator_) {
        completionCallback_(worldGenerator_);
    }
}

void WorldGenerationUI::SimulatePhase(GenerationPhase phase, float deltaTime) {
    // Simulate years passing
    float yearsPerSecond = 1000.0f; // 1000 simulation years per real second
    int yearsPassed = static_cast<int>(deltaTime * yearsPerSecond);
    stats_.simulationYears += yearsPassed;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    switch (phase) {
        case GenerationPhase::TECTONICS:
            if (gen() % 100 == 0) { // Random chance for mountain range creation
                stats_.mountainRanges++;
                std::vector<std::string> mountainNames = {
                    "Dragonspine Mountains", "Ironhold Peaks", "Mistral Heights", 
                    "Shadowcrest Range", "Goldmine Hills"
                };
                std::string name = mountainNames[gen() % mountainNames.size()];
                AddLogEntry("The " + name + " rise from tectonic collision", stats_.simulationYears);
                stats_.highestPeak = std::max(stats_.highestPeak, (float)(1000 + gen() % 2000));
            }
            break;
            
        case GenerationPhase::EROSION:
            if (gen() % 150 == 0) { // Random valley formation
                std::vector<std::string> valleyNames = {
                    "Whisperwind Valley", "Deepholm Gorge", "Sunlit Dale",
                    "Shadowrift Canyon", "Peaceful Glen"
                };
                std::string name = valleyNames[gen() % valleyNames.size()];
                AddLogEntry("Erosion carves the " + name, stats_.simulationYears);
            }
            break;
            
        case GenerationPhase::HYDROLOGY:
            if (gen() % 120 == 0) { // Random river formation
                stats_.majorRivers++;
                std::vector<std::string> riverNames = {
                    "Whisperflow River", "Goldstream", "Mistral Waters",
                    "Lifespring River", "Serpentine Creek"
                };
                std::string name = riverNames[gen() % riverNames.size()];
                AddLogEntry("The " + name + " begins flowing through the valleys", stats_.simulationYears);
                stats_.longestRiverLength = std::max(stats_.longestRiverLength, (float)(50 + gen() % 400));
            }
            if (gen() % 200 == 0) { // Random lake formation
                std::vector<std::string> lakeNames = {
                    "Mirrormere", "Crystal Lake", "Deepwater Basin",
                    "Moonlit Pond", "Sacred Waters"
                };
                std::string name = lakeNames[gen() % lakeNames.size()];
                AddLogEntry("Lake " + name + " forms in the central depression", stats_.simulationYears);
                stats_.largestLakeSize = std::max(stats_.largestLakeSize, (float)(1 + gen() % 50));
            }
            break;
            
        case GenerationPhase::CLIMATE:
            if (gen() % 180 == 0) { // Random climate events
                std::vector<std::string> climateEvents = {
                    "Temperature patterns stabilize across the continent",
                    "Precipitation zones establish in mountain regions",
                    "Desert conditions spread in rain shadow areas",
                    "Tropical conditions develop near the equator"
                };
                std::string event = climateEvents[gen() % climateEvents.size()];
                AddLogEntry(event, stats_.simulationYears);
            }
            break;
            
        case GenerationPhase::BIOMES:
            if (gen() % 140 == 0) { // Random biome formation
                stats_.biomesIdentified = std::min(12, stats_.biomesIdentified + 1);
                std::vector<std::string> biomeNames = {
                    "Dense forest spreads across humid northern regions",
                    "Grasslands establish in the temperate zones",
                    "Desert ecosystem develops in arid areas",
                    "Tundra forms in the coldest regions",
                    "Wetlands appear near major rivers",
                    "Alpine meadows grow on mountain slopes"
                };
                if (!biomeNames.empty()) {
                    std::string event = biomeNames[gen() % biomeNames.size()];
                    AddLogEntry(event, stats_.simulationYears);
                }
            }
            break;
            
        case GenerationPhase::CIVILIZATION:
            if (gen() % 300 == 0) { // Random civilization events
                std::vector<std::string> civEvents = {
                    "Ancient civilization 'The Stone Builders' establishes settlements",
                    "The Bronze Age kingdom of Valdris rises in the fertile valleys",
                    "Nomadic tribes cross the great plains, leaving mysterious stone circles",
                    "The lost city of Aethermoor is founded on the floating islands",
                    "Ancient trade routes connect distant settlements",
                    "Mysterious ruins are left by an unknown civilization"
                };
                std::string event = civEvents[gen() % civEvents.size()];
                AddLogEntry(event, stats_.simulationYears);
            }
            break;
            
        default:
            break;
    }
}

void WorldGenerationUI::AddLogEntry(const std::string& message, int year) {
    LogEntry entry;
    entry.message = message;
    entry.timestamp = std::chrono::steady_clock::now();
    entry.simulationYear = (year == -1) ? stats_.simulationYears : year;
    
    generationLog_.push_back(entry);
    
    // Keep only the most recent entries
    while (generationLog_.size() > MAX_LOG_ENTRIES) {
        generationLog_.pop_front();
    }
}

// Event handlers
void WorldGenerationUI::OnStartClicked() {
    std::cout << "[WorldGenerationUI] Start button clicked" << std::endl;
    StartGeneration();
}

void WorldGenerationUI::OnCancelClicked() {
    std::cout << "[WorldGenerationUI] Cancel button clicked" << std::endl;
    CancelGeneration();
}

void WorldGenerationUI::OnBackToMainMenuClicked() {
    std::cout << "[WorldGenerationUI] Back to main menu clicked" << std::endl;
    if (menuSystem_) {
        menuSystem_->showMainMenu();
    }
}

void WorldGenerationUI::OnBeginGameClicked() {
    std::cout << "[WorldGenerationUI] Begin game clicked" << std::endl;
    // TODO: Start the actual game with generated world
    if (completionCallback_ && worldGenerator_) {
        completionCallback_(worldGenerator_);
    }
}

void WorldGenerationUI::OnPauseResumeClicked() {
    isPaused_ = !isPaused_;
    std::cout << "[WorldGenerationUI] Generation " << (isPaused_ ? "paused" : "resumed") << std::endl;
    createUIElements(); // Refresh UI to update button text
}

void WorldGenerationUI::OnVisualizationModeChanged(VisualizationMode mode) {
    visualizationMode_ = mode;
    // TODO: Update visualization display
}

void WorldGenerationUI::OnSpeedChanged(float speed) {
    generationSpeed_ = speed;
}

void WorldGenerationUI::OnWorldSizeChanged(int size) {
    config_.worldSize = size;
}

void WorldGenerationUI::OnClimateTypeChanged(int type) {
    config_.climateType = type;
}

// Helper methods
void WorldGenerationUI::removeAllChildren() {
    auto children = getChildren();
    for (auto& child : children) {
        removeChild(child);
    }
}

std::string WorldGenerationUI::GetPhaseDisplayName(GenerationPhase phase) {
    switch (phase) {
        case GenerationPhase::CONFIGURATION: return "Configuration";
        case GenerationPhase::TECTONICS: return "Tectonic Simulation";
        case GenerationPhase::EROSION: return "Erosion & Weathering";
        case GenerationPhase::HYDROLOGY: return "River & Lake Formation";
        case GenerationPhase::CLIMATE: return "Climate Calculation";
        case GenerationPhase::BIOMES: return "Biome Assignment";
        case GenerationPhase::CIVILIZATION: return "Ancient Civilizations";
        case GenerationPhase::COMPLETE: return "Complete";
        default: return "Unknown";
    }
}

std::string WorldGenerationUI::FormatTimeRemaining(float secondsRemaining) {
    int minutes = static_cast<int>(secondsRemaining) / 60;
    int seconds = static_cast<int>(secondsRemaining) % 60;
    
    std::ostringstream oss;
    oss << minutes << "m " << seconds << "s";
    return oss.str();
}

float WorldGenerationUI::GetPhaseExpectedDuration(GenerationPhase phase) {
    // Duration in seconds for each phase (adjusted based on simulation depth)
    float baseDuration = 3.0f; // Base duration for normal simulation
    float depthMultiplier = config_.simulationDepth; // 1=Fast, 2=Normal, 3=Epic
    
    switch (phase) {
        case GenerationPhase::TECTONICS: return baseDuration * depthMultiplier * 1.5f;
        case GenerationPhase::EROSION: return baseDuration * depthMultiplier * 1.2f;
        case GenerationPhase::HYDROLOGY: return baseDuration * depthMultiplier * 1.0f;
        case GenerationPhase::CLIMATE: return baseDuration * depthMultiplier * 0.8f;
        case GenerationPhase::BIOMES: return baseDuration * depthMultiplier * 1.0f;
        case GenerationPhase::CIVILIZATION: return baseDuration * depthMultiplier * 0.5f;
        default: return baseDuration;
    }
}

bool WorldGenerationUI::handleInput(float mouseX, float mouseY, bool clicked) {
    return BaseMenu::handleInput(mouseX, mouseY, clicked);
}

} // namespace VoxelCastle::UI