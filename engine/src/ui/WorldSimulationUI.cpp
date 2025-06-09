#include "ui/WorldSimulationUI.h"
#include "ui/elements/UIButton.h"
#include <iostream>
#include <sstream>
#include <iomanip>

WorldSimulationUI::WorldSimulationUI(VoxelEngine::UI::UIRenderer* renderer) 
    : BaseMenu(renderer)
    , currentY_(TITLE_HEIGHT + PANEL_MARGIN)
    , currentPhase_(GenerationPhase::TECTONICS)
    , currentProgress_(0.0f)
    , phaseProgress_(0.0f)
    , isPaused_(false)
    , isRunning_(false)
    , visualizationMode_(VisualizationMode::ELEVATION)
    , onSimulationComplete_(nullptr)
    , onBack_(nullptr) {
}

bool WorldSimulationUI::initialize(int screenWidth, int screenHeight) {
    // Use larger portion of screen for simulation view
    float uiWidth = screenWidth * 0.9f;
    float uiHeight = screenHeight * 0.9f;
    
    setPosition((screenWidth - uiWidth) / 2, (screenHeight - uiHeight) / 2);
    setSize(uiWidth, uiHeight);
    
    std::cout << "[WorldSimulationUI] Initializing with size: " << getSize().x << "x" << getSize().y 
              << " (screen: " << screenWidth << "x" << screenHeight << ")" << std::endl;
    
    createUIElements();
    setVisible(true);
    
    return true;
}

void WorldSimulationUI::render() {
    BaseMenu::render();
}

bool WorldSimulationUI::handleInput(float mouseX, float mouseY, bool clicked) {
    return BaseMenu::handleInput(mouseX, mouseY, clicked);
}

void WorldSimulationUI::update(float deltaTime) {
    BaseMenu::update(deltaTime);
    
    if (isRunning_ && !isPaused_) {
        updateSimulation(deltaTime * generationSpeed_);
    }
}

void WorldSimulationUI::createUIElements() {
    // Clear existing children manually since there's no removeAllChildren()
    children_.clear();
    currentY_ = TITLE_HEIGHT + PANEL_MARGIN;
    
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Title
    auto titleLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    titleLabel->setText("World Generation Simulation");
    titleLabel->setPosition(PANEL_MARGIN, PANEL_MARGIN);
    titleLabel->setSize(panelWidth, TITLE_HEIGHT);
    titleLabel->setBackgroundColor({0.3f, 0.2f, 0.4f, 0.9f});
    addChild(titleLabel);
    
    if (currentPhase_ == GenerationPhase::COMPLETE) {
        createWorldSummaryUI();
    } else {
        createVisualizationControls();
        createWorldPreview();
        createProgressPanels();
        createGenerationLog();
    }
    
    createActionButtons();
}

void WorldSimulationUI::createVisualizationControls() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float maxButtonWidth = calculateMaxVisualizationButtonWidth();
    
    // Visualization mode label
    auto vizControlsLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    vizControlsLabel->setText("Visualization Mode:");
    vizControlsLabel->setPosition(PANEL_MARGIN, currentY_);
    vizControlsLabel->autoSizeToText(8.0f);
    vizControlsLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(vizControlsLabel);
    
    // Visualization mode buttons with calculated layout
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
        modeButton->autoSizeToText(6.0f);
        bool isActive = (visualizationMode_ == mode.second);
        modeButton->setBackgroundColor(isActive ? 
            glm::vec4{0.3f, 0.5f, 0.3f, 0.8f} : glm::vec4{0.2f, 0.2f, 0.2f, 0.6f});
        modeButton->setOnClick([this, mode]() { 
            onVisualizationModeChanged(mode.second); 
        });
        addChild(modeButton);
        vizButtonX += maxButtonWidth + ELEMENT_SPACING;
    }
    currentY_ += VERTICAL_SPACING;
}

void WorldSimulationUI::createWorldPreview() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float previewHeight = 200.0f;
    
    // World preview panel
    auto previewPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    std::string previewText = "[World Generation Preview - " + getVisualizationModeDisplayName(visualizationMode_) + "]\n";
    previewText += "Phase: " + getPhaseDisplayName(currentPhase_) + "\n";
    previewText += "Progress: " + std::to_string((int)(currentProgress_ * 100)) + "%";
    previewPanel->setText(previewText);
    previewPanel->setPosition(PANEL_MARGIN, currentY_);
    previewPanel->setSize(panelWidth, previewHeight);
    previewPanel->setBackgroundColor({0.15f, 0.25f, 0.2f, 0.8f});
    addChild(previewPanel);
    currentY_ += previewHeight + ELEMENT_SPACING;
}

void WorldSimulationUI::createProgressPanels() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float progressHeight = 80.0f;
    
    // Current phase progress
    auto progressLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    progressLabel->setText("Current Phase: " + getPhaseDisplayName(currentPhase_));
    progressLabel->setPosition(PANEL_MARGIN, currentY_);
    progressLabel->setSize(panelWidth, 25.0f);
    progressLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(progressLabel);
    currentY_ += 30.0f;
    
    // Progress bar placeholder
    auto progressBar = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    std::string progressText = "Phase Progress: " + std::to_string((int)(phaseProgress_ * 100)) + "%\n";
    progressText += "Overall Progress: " + std::to_string((int)(currentProgress_ * 100)) + "%\n";
    float timeRemaining = calculateTimeRemaining();
    progressText += "Estimated Time Remaining: " + std::to_string((int)timeRemaining) + " seconds";
    progressBar->setText(progressText);
    progressBar->setPosition(PANEL_MARGIN, currentY_);
    progressBar->setSize(panelWidth, progressHeight);
    progressBar->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.7f});
    addChild(progressBar);
    currentY_ += progressHeight + ELEMENT_SPACING;
}

void WorldSimulationUI::createGenerationLog() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float logHeight = 100.0f;
    
    // Generation log panel
    auto logLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    logLabel->setText("Generation Log:");
    logLabel->setPosition(PANEL_MARGIN, currentY_);
    logLabel->setSize(panelWidth, 25.0f);
    logLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(logLabel);
    currentY_ += 30.0f;
    
    auto logPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    std::string logText = "";
    size_t entriesToShow = std::min(static_cast<size_t>(5), generationLog_.size());
    if (entriesToShow > 0) {
        for (size_t i = generationLog_.size() - entriesToShow; i < generationLog_.size(); ++i) {
            logText += "Year " + std::to_string(generationLog_[i].simulationYear) + ": " + generationLog_[i].message + "\n";
        }
    } else {
        logText = "Simulation starting...\n";
    }
    logPanel->setText(logText);
    logPanel->setPosition(PANEL_MARGIN, currentY_);
    logPanel->setSize(panelWidth, logHeight);
    logPanel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    addChild(logPanel);
    currentY_ += logHeight + ELEMENT_SPACING;
}

void WorldSimulationUI::createWorldSummaryUI() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // World completion title
    auto completionLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    completionLabel->setText("World Generation Complete! - \"Generated Realm\"");
    completionLabel->setPosition(PANEL_MARGIN, currentY_);
    completionLabel->setSize(panelWidth, 30.0f);
    completionLabel->setBackgroundColor({0.2f, 0.4f, 0.2f, 0.8f});
    addChild(completionLabel);
    currentY_ += 50.0f;
    
    // World statistics
    auto statsPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    std::string statsText = "World Statistics:\n";
    statsText += "Mountain Ranges: " + std::to_string(stats_.mountainRanges) + "\n";
    statsText += "Major Rivers: " + std::to_string(stats_.majorRivers) + "\n";
    statsText += "Biomes Identified: " + std::to_string(stats_.biomesIdentified) + "\n";
    statsText += "Simulation Years: " + std::to_string(stats_.simulationYears) + "\n";
    statsText += "Highest Peak: " + std::to_string(stats_.highestPeak) + "m";
    statsPanel->setText(statsText);
    statsPanel->setPosition(PANEL_MARGIN, currentY_);
    statsPanel->setSize(panelWidth, 150.0f);
    statsPanel->setBackgroundColor({0.15f, 0.2f, 0.15f, 0.8f});
    addChild(statsPanel);
    currentY_ += 160.0f;
    
    // Final world preview
    auto finalPreviewPanel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    finalPreviewPanel->setText("[Final World Overview - Click to Explore]");
    finalPreviewPanel->setPosition(PANEL_MARGIN, currentY_);
    finalPreviewPanel->setSize(panelWidth, 120.0f);
    finalPreviewPanel->setBackgroundColor({0.15f, 0.25f, 0.2f, 0.8f});
    addChild(finalPreviewPanel);
    currentY_ += 130.0f;
}

void WorldSimulationUI::createActionButtons() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float buttonY = getSize().y - 60.0f;
    float buttonWidth = 120.0f;
    
    if (currentPhase_ == GenerationPhase::COMPLETE) {
        // Completion buttons
        auto beginGameButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        beginGameButton->setText("Begin Game");
        beginGameButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN, buttonY);
        beginGameButton->autoSizeToText(16.0f);
        beginGameButton->setBackgroundColor({0.2f, 0.4f, 0.2f, 0.8f});
        beginGameButton->setOnClick([this]() { onBeginGameClicked(); });
        addChild(beginGameButton);
        
        auto backButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        backButton->setText("Back to Menu");
        backButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN, buttonY);
        backButton->autoSizeToText(16.0f);
        backButton->setBackgroundColor({0.4f, 0.2f, 0.2f, 0.8f});
        backButton->setOnClick([this]() { onBackClicked(); });
        addChild(backButton);
    } else {
        // Simulation control buttons
        auto pauseResumeButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        pauseResumeButton->setText(isPaused_ ? "Resume" : "Pause");
        pauseResumeButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN, buttonY);
        pauseResumeButton->autoSizeToText(16.0f);
        pauseResumeButton->setBackgroundColor({0.2f, 0.3f, 0.4f, 0.8f});
        pauseResumeButton->setOnClick([this]() { onPauseResumeClicked(); });
        addChild(pauseResumeButton);
        
        auto stopButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        stopButton->setText("Stop");
        stopButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN - buttonWidth - PANEL_MARGIN, buttonY);
        stopButton->autoSizeToText(16.0f);
        stopButton->setBackgroundColor({0.4f, 0.3f, 0.2f, 0.8f});
        stopButton->setOnClick([this]() { onStopClicked(); });
        addChild(stopButton);
        
        auto backButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        backButton->setText("Back");
        backButton->setPosition(getSize().x - buttonWidth - PANEL_MARGIN, buttonY);
        backButton->autoSizeToText(16.0f);
        backButton->setBackgroundColor({0.4f, 0.2f, 0.2f, 0.8f});
        backButton->setOnClick([this]() { onBackClicked(); });
        addChild(backButton);
    }
}

void WorldSimulationUI::startSimulation(const WorldConfig& config) {
    config_ = config;
    currentPhase_ = GenerationPhase::TECTONICS;
    currentProgress_ = 0.0f;
    phaseProgress_ = 0.0f;
    isPaused_ = false;
    isRunning_ = true;
    simulationStartTime_ = std::chrono::steady_clock::now();
    phaseStartTime_ = simulationStartTime_;
    
    // Initialize log
    generationLog_.clear();
    addLogEntry("Beginning world generation...", 0);
    addLogEntry("Initializing tectonic simulation", 1);
    
    createUIElements();
}

void WorldSimulationUI::pauseSimulation() {
    isPaused_ = true;
    addLogEntry("Simulation paused", stats_.simulationYears);
    createUIElements();
}

void WorldSimulationUI::resumeSimulation() {
    isPaused_ = false;
    addLogEntry("Simulation resumed", stats_.simulationYears);
    createUIElements();
}

void WorldSimulationUI::stopSimulation() {
    isRunning_ = false;
    isPaused_ = false;
    addLogEntry("Simulation stopped by user", stats_.simulationYears);
    if (onBack_) {
        onBack_();
    }
}

void WorldSimulationUI::updateSimulation(float deltaTime) {
    // Update phase progress
    phaseProgress_ += deltaTime / getPhaseExpectedDuration(currentPhase_);
    stats_.simulationYears += (int)(deltaTime * 10); // Arbitrary time scaling
    
    // Simulate current phase
    simulatePhase(currentPhase_, deltaTime);
    
    // Check for phase completion
    if (phaseProgress_ >= 1.0f) {
        advancePhase();
    }
    
    // Update overall progress
    float phaseWeight = 1.0f / 6.0f; // 6 total phases (excluding CONFIGURATION and COMPLETE)
    int phaseIndex = static_cast<int>(currentPhase_) - 1; // Subtract 1 for CONFIGURATION
    currentProgress_ = (phaseIndex * phaseWeight) + (phaseProgress_ * phaseWeight);
    
    // Recreate UI periodically to update progress displays
    static float lastUIUpdate = 0.0f;
    lastUIUpdate += deltaTime;
    if (lastUIUpdate >= 0.5f) { // Update UI every 0.5 seconds
        createUIElements();
        lastUIUpdate = 0.0f;
    }
}

void WorldSimulationUI::advancePhase() {
    phaseProgress_ = 0.0f;
    phaseStartTime_ = std::chrono::steady_clock::now();
    
    switch (currentPhase_) {
        case GenerationPhase::TECTONICS:
            currentPhase_ = GenerationPhase::EROSION;
            addLogEntry("Tectonic simulation complete. Beginning erosion modeling...", stats_.simulationYears);
            stats_.mountainRanges = 3 + (config_.worldSize / 512); // Scale with world size
            break;
        case GenerationPhase::EROSION:
            currentPhase_ = GenerationPhase::HYDROLOGY;
            addLogEntry("Erosion modeling complete. Simulating water systems...", stats_.simulationYears);
            break;
        case GenerationPhase::HYDROLOGY:
            currentPhase_ = GenerationPhase::CLIMATE;
            addLogEntry("Hydrology simulation complete. Calculating climate patterns...", stats_.simulationYears);
            stats_.majorRivers = 2 + (config_.worldSize / 256);
            break;
        case GenerationPhase::CLIMATE:
            currentPhase_ = GenerationPhase::BIOMES;
            addLogEntry("Climate calculation complete. Assigning biomes...", stats_.simulationYears);
            break;
        case GenerationPhase::BIOMES:
            if (config_.enableCivilizations) {
                currentPhase_ = GenerationPhase::CIVILIZATION;
                addLogEntry("Biome assignment complete. Simulating civilizations...", stats_.simulationYears);
                stats_.biomesIdentified = 8 + (config_.worldSize / 128);
            } else {
                completeSimulation();
                return;
            }
            break;
        case GenerationPhase::CIVILIZATION:
            completeSimulation();
            return;
        default:
            completeSimulation();
            return;
    }
    
    createUIElements();
}

void WorldSimulationUI::simulatePhase(GenerationPhase phase, float deltaTime) {
    // Add phase-specific log entries periodically
    static float lastLogEntry = 0.0f;
    lastLogEntry += deltaTime;
    
    if (lastLogEntry >= 2.0f) { // Add log entry every 2 seconds
        switch (phase) {
            case GenerationPhase::TECTONICS:
                addLogEntry("Tectonic plates shifting, mountains rising...", stats_.simulationYears);
                stats_.highestPeak += deltaTime * 100.0f; // Arbitrary growth
                break;
            case GenerationPhase::EROSION:
                addLogEntry("Water and wind carving valleys...", stats_.simulationYears);
                stats_.deepestValley -= deltaTime * 50.0f; // Arbitrary deepening
                break;
            case GenerationPhase::HYDROLOGY:
                addLogEntry("Rivers forming, lakes filling...", stats_.simulationYears);
                stats_.largestLakeSize += deltaTime * 20.0f;
                break;
            case GenerationPhase::CLIMATE:
                addLogEntry("Weather patterns stabilizing...", stats_.simulationYears);
                break;
            case GenerationPhase::BIOMES:
                addLogEntry("Flora and fauna establishing territories...", stats_.simulationYears);
                break;
            case GenerationPhase::CIVILIZATION:
                addLogEntry("Early settlements appearing...", stats_.simulationYears);
                break;
            default:
                break;
        }
        lastLogEntry = 0.0f;
    }
}

void WorldSimulationUI::completeSimulation() {
    currentPhase_ = GenerationPhase::COMPLETE;
    currentProgress_ = 1.0f;
    phaseProgress_ = 1.0f;
    isRunning_ = false;
    isPaused_ = false;
    
    // Finalize statistics
    stats_.highestPeakName = "Mount Skyreach";
    stats_.deepestValleyName = "Shadowrift Canyon";
    stats_.largestLakeName = "Lake Serenity";
    stats_.longestRiverName = "Goldflow River";
    
    addLogEntry("World generation complete!", stats_.simulationYears);
    createUIElements();
    
    if (onSimulationComplete_) {
        onSimulationComplete_(stats_);
    }
}

void WorldSimulationUI::addLogEntry(const std::string& message, int year) {
    LogEntry entry;
    entry.message = message;
    entry.timestamp = std::chrono::steady_clock::now();
    entry.simulationYear = year > 0 ? year : stats_.simulationYears;
    
    generationLog_.push_back(entry);
    
    // Keep log size manageable
    if (generationLog_.size() > 50) {
        generationLog_.pop_front();
    }
}

// Event handlers
void WorldSimulationUI::onVisualizationModeChanged(VisualizationMode mode) {
    visualizationMode_ = mode;
    createUIElements();
}

void WorldSimulationUI::onPauseResumeClicked() {
    if (isPaused_) {
        resumeSimulation();
    } else {
        pauseSimulation();
    }
}

void WorldSimulationUI::onStopClicked() {
    stopSimulation();
}

void WorldSimulationUI::onBackClicked() {
    if (onBack_) {
        onBack_();
    }
}

void WorldSimulationUI::onBeginGameClicked() {
    // TODO: Implement game start logic
    if (onSimulationComplete_) {
        onSimulationComplete_(stats_);
    }
}

// Helper methods
float WorldSimulationUI::calculateMaxVisualizationButtonWidth() {
    std::vector<std::string> modeNames = {"Elevation", "Temperature", "Precipitation", "Biomes", "Hydrology"};
    float maxWidth = 0.0f;
    for (const auto& name : modeNames) {
        float textWidth = renderer_->getTextWidth(name);
        maxWidth = std::max(maxWidth, textWidth + 16.0f); // Add padding
    }
    return maxWidth;
}

float WorldSimulationUI::calculateOptimalRowSpacing() {
    return VERTICAL_SPACING;
}

std::string WorldSimulationUI::getPhaseDisplayName(GenerationPhase phase) {
    switch (phase) {
        case GenerationPhase::CONFIGURATION: return "Configuration";
        case GenerationPhase::TECTONICS: return "Tectonic Simulation";
        case GenerationPhase::EROSION: return "Erosion Modeling";
        case GenerationPhase::HYDROLOGY: return "Hydrology Simulation";
        case GenerationPhase::CLIMATE: return "Climate Calculation";
        case GenerationPhase::BIOMES: return "Biome Assignment";
        case GenerationPhase::CIVILIZATION: return "Civilization Simulation";
        case GenerationPhase::COMPLETE: return "Complete";
        default: return "Unknown";
    }
}

std::string WorldSimulationUI::getVisualizationModeDisplayName(VisualizationMode mode) {
    switch (mode) {
        case VisualizationMode::ELEVATION: return "Elevation";
        case VisualizationMode::TEMPERATURE: return "Temperature";
        case VisualizationMode::PRECIPITATION: return "Precipitation";
        case VisualizationMode::BIOMES: return "Biomes";
        case VisualizationMode::HYDROLOGY: return "Hydrology";
        case VisualizationMode::GEOLOGY: return "Geology";
        default: return "Unknown";
    }
}

float WorldSimulationUI::getPhaseExpectedDuration(GenerationPhase phase) {
    // Duration in seconds (simulation time)
    float baseTime = 5.0f * config_.simulationDepth; // Scale with simulation depth
    
    switch (phase) {
        case GenerationPhase::TECTONICS: return baseTime * 1.5f;
        case GenerationPhase::EROSION: return baseTime * 1.2f;
        case GenerationPhase::HYDROLOGY: return baseTime * 1.0f;
        case GenerationPhase::CLIMATE: return baseTime * 0.8f;
        case GenerationPhase::BIOMES: return baseTime * 1.0f;
        case GenerationPhase::CIVILIZATION: return baseTime * 2.0f; // Longest phase
        default: return baseTime;
    }
}

float WorldSimulationUI::calculateTimeRemaining() {
    if (!isRunning_ || currentPhase_ == GenerationPhase::COMPLETE) {
        return 0.0f;
    }
    
    // Calculate remaining time for current phase
    float currentPhaseRemaining = (1.0f - phaseProgress_) * getPhaseExpectedDuration(currentPhase_);
    
    // Calculate time for remaining phases
    float remainingPhasesTime = 0.0f;
    GenerationPhase phase = currentPhase_;
    while (phase != GenerationPhase::COMPLETE) {
        phase = static_cast<GenerationPhase>(static_cast<int>(phase) + 1);
        if (phase == GenerationPhase::CIVILIZATION && !config_.enableCivilizations) {
            break; // Skip civilization if disabled
        }
        if (phase != GenerationPhase::COMPLETE) {
            remainingPhasesTime += getPhaseExpectedDuration(phase);
        }
    }
    
    return currentPhaseRemaining + remainingPhasesTime;
}
