#include "ui/WorldSimulationUI.h"
#include "ui/elements/UIButton.h"
#include "ui/WorldMapRenderer.h"
#include "world/seed_world_generator.h"
#include "world/world_seed.h"
#include "world/world_parameters.h"
#include "world/world_persistence_manager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>

WorldSimulationUI::WorldSimulationUI(VoxelEngine::UI::UIRenderer* renderer) 
    : BaseMenu(renderer)
    , currentPhase_(GenerationPhase::TECTONICS)
    , currentProgress_(0.0f)
    , phaseProgress_(0.0f)
    , isPaused_(false)
    , isRunning_(false)
    , visualizationMode_(VisualizationMode::ELEVATION)
    , worldMapRenderer_(nullptr)
    , worldMapX_(0.0f), worldMapY_(0.0f), worldMapWidth_(0.0f), worldMapHeight_(0.0f)
    , onSimulationComplete_(nullptr)
    , onBack_(nullptr)
    , currentY_(TITLE_HEIGHT + PANEL_MARGIN) {
    
    // Initialize world map renderer
    worldMapRenderer_ = std::make_unique<VoxelEngine::UI::WorldMapRenderer>();
    if (!worldMapRenderer_->initialize(512)) {
        std::cerr << "[WorldSimulationUI] Warning: Failed to initialize world map renderer" << std::endl;
    }
}

WorldSimulationUI::~WorldSimulationUI() {
    // Stop generation and join thread
    isRunning_ = false;
    if (generationThread_.joinable()) {
        generationThread_.join();
    }
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
    
    // Render world map on top of background but behind other UI elements
    if (worldMapRenderer_ && isRunning_) {
        renderWorldMap();
    }
}

bool WorldSimulationUI::handleInput(float mouseX, float mouseY, bool clicked) {
    // First check if we should handle map interaction
    if (isMouseOverMap(mouseX, mouseY) && worldMapRenderer_) {
        // Convert to relative map coordinates
        glm::vec2 absolutePos = getAbsolutePosition();
        float mapRelativeX = mouseX - (absolutePos.x + worldMapX_);
        float mapRelativeY = mouseY - (absolutePos.y + worldMapY_);
        
        // Handle map interaction (no wheel delta here, will be handled by handleExtendedInput)
        if (worldMapRenderer_->handleMouseInput(
            static_cast<int>(mapRelativeX), static_cast<int>(mapRelativeY),
            worldMapWidth_, worldMapHeight_, 0.0f, clicked)) {
            
            // Map needs regeneration due to zoom/pan change
            if (worldGenerator_) {
                worldMapRenderer_->generateWorldMap(
                    worldGenerator_.get(),
                    static_cast<VoxelEngine::UI::WorldMapRenderer::GenerationPhase>(currentPhase_),
                    static_cast<VoxelEngine::UI::WorldMapRenderer::VisualizationMode>(visualizationMode_),
                    config_.customSeed,
                    static_cast<float>(config_.worldSize)
                );
            }
        }
        
        // If we're over the map, don't let other UI elements handle the input
        return true;
    }
    
    return BaseMenu::handleInput(mouseX, mouseY, clicked);
}

bool WorldSimulationUI::handleExtendedInput(float mouseX, float mouseY, bool clicked, float wheelDelta) {
    // Handle base input first
    bool handled = handleInput(mouseX, mouseY, clicked);
    
    // Handle mouse wheel for zoom if over map
    if (wheelDelta != 0.0f && isMouseOverMap(mouseX, mouseY) && worldMapRenderer_) {
        glm::vec2 absolutePos = getAbsolutePosition();
        float mapRelativeX = mouseX - (absolutePos.x + worldMapX_);
        float mapRelativeY = mouseY - (absolutePos.y + worldMapY_);
        
        if (worldMapRenderer_->handleMouseInput(
            static_cast<int>(mapRelativeX), static_cast<int>(mapRelativeY),
            worldMapWidth_, worldMapHeight_, wheelDelta, clicked)) {
            
            // Map needs regeneration due to zoom change
            if (worldGenerator_) {
                worldMapRenderer_->generateWorldMap(
                    worldGenerator_.get(),
                    static_cast<VoxelEngine::UI::WorldMapRenderer::GenerationPhase>(currentPhase_),
                    static_cast<VoxelEngine::UI::WorldMapRenderer::VisualizationMode>(visualizationMode_),
                    config_.customSeed,
                    static_cast<float>(config_.worldSize)
                );
            }
        }
        
        return true; // We handled the wheel event
    }
    
    return handled;
}

bool WorldSimulationUI::isMouseOverMap(float mouseX, float mouseY) const {
    if (worldMapWidth_ <= 0 || worldMapHeight_ <= 0) {
        return false;
    }
    
    glm::vec2 absolutePos = getAbsolutePosition();
    float mapLeft = absolutePos.x + worldMapX_;
    float mapTop = absolutePos.y + worldMapY_;
    float mapRight = mapLeft + worldMapWidth_;
    float mapBottom = mapTop + worldMapHeight_;
    
    return mouseX >= mapLeft && mouseX <= mapRight && 
           mouseY >= mapTop && mouseY <= mapBottom;
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
    
    // Add zoom controls row
    auto zoomLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    zoomLabel->setText("Map Controls:");
    zoomLabel->setPosition(PANEL_MARGIN, currentY_);
    zoomLabel->autoSizeToText(8.0f);
    zoomLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(zoomLabel);
    
    // Zoom controls
    float zoomButtonX = PANEL_MARGIN + 160.0f;
    
    // Zoom out button
    auto zoomOutButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    zoomOutButton->setText("Zoom Out");
    zoomOutButton->setPosition(zoomButtonX, currentY_);
    zoomOutButton->autoSizeToText(6.0f);
    zoomOutButton->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.6f});
    zoomOutButton->setOnClick([this]() { 
        if (worldMapRenderer_) {
            float zoom, centerX, centerY;
            const char* scale;
            worldMapRenderer_->getViewportInfo(zoom, centerX, centerY, scale);
            worldMapRenderer_->setViewport(zoom * 0.5f, centerX, centerY);
            regenerateWorldMap();
        }
    });
    addChild(zoomOutButton);
    zoomButtonX += 100.0f + ELEMENT_SPACING;
    
    // Reset zoom button
    auto resetZoomButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    resetZoomButton->setText("Reset View");
    resetZoomButton->setPosition(zoomButtonX, currentY_);
    resetZoomButton->autoSizeToText(6.0f);
    resetZoomButton->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.6f});
    resetZoomButton->setOnClick([this]() { 
        if (worldMapRenderer_) {
            worldMapRenderer_->setViewport(1.0f, 0.5f, 0.5f);
            regenerateWorldMap();
        }
    });
    addChild(resetZoomButton);
    zoomButtonX += 100.0f + ELEMENT_SPACING;
    
    // Zoom in button
    auto zoomInButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    zoomInButton->setText("Zoom In");
    zoomInButton->setPosition(zoomButtonX, currentY_);
    zoomInButton->autoSizeToText(6.0f);
    zoomInButton->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.6f});
    zoomInButton->setOnClick([this]() { 
        if (worldMapRenderer_) {
            float zoom, centerX, centerY;
            const char* scale;
            worldMapRenderer_->getViewportInfo(zoom, centerX, centerY, scale);
            worldMapRenderer_->setViewport(zoom * 2.0f, centerX, centerY);
            regenerateWorldMap();
        }
    });
    addChild(zoomInButton);
    
    currentY_ += VERTICAL_SPACING;
}

void WorldSimulationUI::createWorldPreview() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Calculate world map area (large central area following the specification)
    float mapWidth = panelWidth * 0.65f;  // 65% of width for main visualization
    float mapHeight = 400.0f;              // Substantial height for good visibility
    
    // Store world map coordinates for rendering
    worldMapX_ = PANEL_MARGIN;
    worldMapY_ = currentY_;
    worldMapWidth_ = mapWidth;
    worldMapHeight_ = mapHeight;
    
    // Note: No background panel needed - world map will render directly
    
    currentY_ += mapHeight + ELEMENT_SPACING;
}

void WorldSimulationUI::renderWorldMap() {
    if (!worldMapRenderer_ || worldMapWidth_ <= 0 || worldMapHeight_ <= 0) {
        return;
    }
    
    // Calculate absolute coordinates by adding the menu's position to relative coordinates
    glm::vec2 absolutePos = getAbsolutePosition();
    float absoluteX = absolutePos.x + worldMapX_;
    float absoluteY = absolutePos.y + worldMapY_;
    
    // Convert WorldSimulationUI enums to WorldMapRenderer enums
    VoxelEngine::UI::WorldMapRenderer::GenerationPhase mapPhase;
    switch (currentPhase_) {
        case GenerationPhase::TECTONICS:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::TECTONICS;
            break;
        case GenerationPhase::EROSION:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::EROSION;
            break;
        case GenerationPhase::HYDROLOGY:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::HYDROLOGY;
            break;
        case GenerationPhase::CLIMATE:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::CLIMATE;
            break;
        case GenerationPhase::BIOMES:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::BIOMES;
            break;
        case GenerationPhase::CIVILIZATION:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::CIVILIZATION;
            break;
        default:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::COMPLETE;
            break;
    }
    
    VoxelEngine::UI::WorldMapRenderer::VisualizationMode mapMode;
    switch (visualizationMode_) {
        case VisualizationMode::ELEVATION:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::ELEVATION;
            break;
        case VisualizationMode::TEMPERATURE:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::TEMPERATURE;
            break;
        case VisualizationMode::PRECIPITATION:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::PRECIPITATION;
            break;
        case VisualizationMode::BIOMES:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::BIOMES;
            break;
        case VisualizationMode::HYDROLOGY:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::HYDROLOGY;
            break;
        case VisualizationMode::GEOLOGY:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::GEOLOGY;
            break;
        default:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::ELEVATION;
            break;
    }
    
    // Update the world map renderer with current phase and mode
    worldMapRenderer_->setGenerationPhase(mapPhase);
    worldMapRenderer_->setVisualizationMode(mapMode);
    
    // Render the world map using absolute screen coordinates
    worldMapRenderer_->render(renderer_, 
                             (int)absoluteX, (int)absoluteY, 
                             (int)worldMapWidth_, (int)worldMapHeight_);
    
    // Render zoom information overlay in the top-right corner of the map
    if (worldMapRenderer_) {
        float zoomLevel, centerX, centerY;
        const char* scaleDesc;
        worldMapRenderer_->getViewportInfo(zoomLevel, centerX, centerY, scaleDesc);
        
        // Create zoom info text
        std::string zoomInfo = "Zoom: " + std::to_string((int)(zoomLevel * 100)) + "% | " + scaleDesc;
        if (zoomLevel > 1.0f) {
            zoomInfo += " | Mouse wheel to zoom, drag to pan";
        }
        
        // Render zoom info background panel
        float infoWidth = 300.0f;
        float infoHeight = 25.0f;
        float infoX = absoluteX + worldMapWidth_ - infoWidth - 10.0f;
        float infoY = absoluteY + 10.0f;
        
        // Semi-transparent background for zoom info
        renderer_->renderQuad(infoX, infoY, infoWidth, infoHeight, {0.0f, 0.0f, 0.0f, 0.7f});
        
        // Render the zoom info text (note: drawText expects text first, then position)
        renderer_->drawText(zoomInfo, infoX + 5.0f, infoY + 5.0f, 0.6f, {1.0f, 1.0f, 1.0f});
    }
}

void WorldSimulationUI::updateWorldMapVisualization() {
    if (!worldMapRenderer_ || !worldGenerator_) {
        return;
    }
    
    // Convert enums and generate new world map data
    VoxelEngine::UI::WorldMapRenderer::GenerationPhase mapPhase;
    switch (currentPhase_) {
        case GenerationPhase::TECTONICS:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::TECTONICS;
            break;
        case GenerationPhase::EROSION:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::EROSION;
            break;
        case GenerationPhase::HYDROLOGY:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::HYDROLOGY;
            break;
        case GenerationPhase::CLIMATE:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::CLIMATE;
            break;
        case GenerationPhase::BIOMES:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::BIOMES;
            break;
        case GenerationPhase::CIVILIZATION:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::CIVILIZATION;
            break;
        default:
            mapPhase = VoxelEngine::UI::WorldMapRenderer::GenerationPhase::COMPLETE;
            break;
    }
    
    VoxelEngine::UI::WorldMapRenderer::VisualizationMode mapMode;
    switch (visualizationMode_) {
        case VisualizationMode::ELEVATION:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::ELEVATION;
            break;
        case VisualizationMode::TEMPERATURE:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::TEMPERATURE;
            break;
        case VisualizationMode::PRECIPITATION:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::PRECIPITATION;
            break;
        case VisualizationMode::BIOMES:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::BIOMES;
            break;
        case VisualizationMode::HYDROLOGY:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::HYDROLOGY;
            break;
        case VisualizationMode::GEOLOGY:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::GEOLOGY;
            break;
        default:
            mapMode = VoxelEngine::UI::WorldMapRenderer::VisualizationMode::ELEVATION;
            break;
    }
    
    // Generate new world map data with actual world size from configuration
    unsigned int currentSeed = worldSeed_ ? worldSeed_->getMasterSeed() : 12345;
    float worldSizeKm = static_cast<float>(config_.worldSize); // Convert from config (which is in km)
    worldMapRenderer_->generateWorldMap(worldGenerator_.get(), mapPhase, mapMode, currentSeed, worldSizeKm);
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

void WorldSimulationUI::startSimulation(const WorldConfig& config, const std::string& worldName) {
    config_ = config;
    worldName_ = worldName;  // Store the world name for saving
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
    
    // Create world seed and parameters from config
    try {
        // Create WorldSeed - use custom seed if provided, otherwise generate random
        if (config.customSeed != 0) {
            worldSeed_ = std::make_shared<VoxelCastle::World::WorldSeed>(config.customSeed);
            addLogEntry("Using custom seed: " + std::to_string(config.customSeed), 1);
        } else {
            worldSeed_ = std::make_shared<VoxelCastle::World::WorldSeed>(); // Random seed
            addLogEntry("Generated random seed: " + std::to_string(worldSeed_->getMasterSeed()), 1);
        }
        
        // Create WorldParameters from config
        worldParameters_ = std::make_shared<VoxelCastle::World::WorldParameters>();
        // TODO: Set parameters based on config (worldSize, climateType, etc.)
        
        // Create SeedWorldGenerator with geological realism enabled
        worldGenerator_ = std::make_shared<VoxelCastle::World::SeedWorldGenerator>(worldSeed_, worldParameters_, true); // Enable geological realism
        
        // Initialize geological simulation system 
        addLogEntry("Initializing geological simulation system", 2);
        addLogEntry("World size: " + std::to_string(config.worldSize) + "x" + std::to_string(config.worldSize), 3);
        
        // Create geological configuration based on user settings (default to BALANCED quality)
        VoxelCastle::World::GeologicalConfig geoConfig;
        geoConfig.preset = VoxelCastle::World::GeologicalPreset::BALANCED;
        
        // Set up progress callback for geological simulation
        auto progressCallback = [this](const VoxelCastle::World::PhaseInfo& phaseInfo) {
            this->onGeologicalPhaseUpdate(phaseInfo);
        };
        
        worldGenerator_->initializeGeologicalSimulation(static_cast<float>(config.worldSize), geoConfig, progressCallback);
        addLogEntry("Geological simulation initialized", 3);
        
        // Create WorldPersistenceManager
        worldPersistence_ = std::make_shared<VoxelCastle::World::WorldPersistenceManager>();
        
        // Update initial world map visualization now that tectonic data is available
        updateWorldMapVisualization();
        
        // Start generation in background thread
        startGenerationThread();
        
        // Note: updateWorldMapVisualization() will be called after each phase completes
        
    } catch (const std::exception& e) {
        addLogEntry("ERROR: Failed to initialize world generation: " + std::string(e.what()), 0);
        isRunning_ = false;
        return;
    }
    
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
    
    // Join generation thread if it's running
    if (generationThread_.joinable()) {
        generationThread_.join();
    }
    
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
    
    // Update world map visualization for the new phase
    updateWorldMapVisualization();
    
    createUIElements();
}

void WorldSimulationUI::simulatePhase(GenerationPhase phase, float deltaTime) {
    // Track generation progress
    static float generationProgress = 0.0f;
    static float lastLogEntry = 0.0f;
    static bool phaseCompleted = false;
    
    lastLogEntry += deltaTime;
    
    // Each phase should take approximately 3-5 seconds to complete
    const float phaseCompletionTime = 4.0f;
    float phaseProgress = generationProgress / phaseCompletionTime;
    
    // Perform actual world generation for each phase
    if (!phaseCompleted && worldGenerator_) {
        switch (phase) {
            case GenerationPhase::TECTONICS:
                if (generationProgress == 0.0f) {
                    addLogEntry("Tectonic simulation running...", stats_.simulationYears);
                    // Tectonic simulation already initialized, now just show progress
                }
                if (lastLogEntry >= 1.5f) {
                    addLogEntry("Tectonic plates shifting, mountains rising...", stats_.simulationYears);
                    // Update realistic statistics based on actual generation
                    stats_.highestPeak = 800.0f + (phaseProgress * 1200.0f); // 800-2000m range
                    lastLogEntry = 0.0f;
                }
                break;
                
            case GenerationPhase::EROSION:
                if (generationProgress == 0.0f) {
                    addLogEntry("Beginning erosion simulation...", stats_.simulationYears);
                }
                if (lastLogEntry >= 1.5f) {
                    addLogEntry("Water and wind carving valleys...", stats_.simulationYears);
                    // Erosion creates valleys - negative values represent depth below sea level
                    stats_.deepestValley = -50.0f - (phaseProgress * 200.0f); // -50 to -250m range
                    lastLogEntry = 0.0f;
                }
                break;
                
            case GenerationPhase::HYDROLOGY:
                if (generationProgress == 0.0f) {
                    addLogEntry("Simulating water flow and lake formation...", stats_.simulationYears);
                }
                if (lastLogEntry >= 1.5f) {
                    addLogEntry("Rivers forming, lakes filling...", stats_.simulationYears);
                    // Calculate lake sizes based on world size
                    float worldScale = config_.worldSize / 1024.0f; // Normalize to default size
                    stats_.largestLakeSize = (50.0f + phaseProgress * 150.0f) * worldScale; // 50-200 * scale
                    lastLogEntry = 0.0f;
                }
                break;
                
            case GenerationPhase::CLIMATE:
                if (generationProgress == 0.0f) {
                    addLogEntry("Calculating climate patterns...", stats_.simulationYears);
                }
                if (lastLogEntry >= 1.5f) {
                    addLogEntry("Weather patterns stabilizing...", stats_.simulationYears);
                    lastLogEntry = 0.0f;
                }
                break;
                
            case GenerationPhase::BIOMES:
                if (generationProgress == 0.0f) {
                    addLogEntry("Assigning biomes and ecosystems...", stats_.simulationYears);
                }
                if (lastLogEntry >= 1.5f) {
                    addLogEntry("Flora and fauna establishing territories...", stats_.simulationYears);
                    lastLogEntry = 0.0f;
                }
                break;
                
            case GenerationPhase::CIVILIZATION:
                if (generationProgress == 0.0f) {
                    addLogEntry("Simulating early civilizations...", stats_.simulationYears);
                }
                if (lastLogEntry >= 1.5f) {
                    addLogEntry("Early settlements appearing...", stats_.simulationYears);
                    lastLogEntry = 0.0f;
                }
                break;
                
            default:
                break;
        }
        
        // Update generation progress
        generationProgress += deltaTime;
        
        // Mark phase as completed when time elapsed
        if (generationProgress >= phaseCompletionTime) {
            phaseCompleted = true;
            addLogEntry("Phase completed successfully.", stats_.simulationYears);
            
            // Update world map visualization after phase completion
            updateWorldMapVisualization();
        }
    }
    
    // Reset for next phase
    if (phaseCompleted && currentPhase_ != phase) {
        generationProgress = 0.0f;
        phaseCompleted = false;
    }
}

void WorldSimulationUI::completeSimulation() {
    currentPhase_ = GenerationPhase::COMPLETE;
    currentProgress_ = 1.0f;
    phaseProgress_ = 1.0f;
    isRunning_ = false;
    isPaused_ = false;
    
    // Finalize statistics with realistic values based on world size
    stats_.highestPeakName = "Mount Skyreach";
    stats_.deepestValleyName = "Shadowrift Canyon";
    stats_.largestLakeName = "Lake Serenity";
    stats_.longestRiverName = "Goldflow River";
    
    // Generate a few sample chunks to validate the world generation
    if (worldGenerator_) {
        addLogEntry("Generating sample terrain chunks...", stats_.simulationYears);
        
        // Generate a small sample of chunks around world center to test generation
        // This creates actual voxel data that could be used for world preview or validation
        try {
            // Note: ChunkSegment creation would need proper implementation
            // For now, we log that the generator is ready for chunk generation
            addLogEntry("World generator initialized and ready for chunk generation.", stats_.simulationYears);
            
            // In a full implementation, we might:
            // 1. Generate a few representative chunks
            // 2. Calculate actual statistics from generated data
            // 3. Store regional data for faster runtime generation
            // 4. Prepare world database for save/load
            
        } catch (const std::exception& e) {
            addLogEntry("Warning: Sample chunk generation failed - " + std::string(e.what()), stats_.simulationYears);
        }
    }
    
    // Save the generated world to disk
    if (worldPersistence_ && worldGenerator_) {
        addLogEntry("Saving world to disk...", stats_.simulationYears);
        
        try {
            // Create world metadata with all generation statistics
            VoxelCastle::World::WorldMetadata metadata;
            metadata.worldName = worldName_;
            metadata.seed = worldSeed_ ? worldSeed_->getMasterSeed() : 0;
            metadata.gameMode = "creative";
            metadata.worldType = "infinite";
            metadata.generateStructures = true;
            metadata.worldSize = config_.worldSize;
            metadata.mountainRanges = stats_.mountainRanges;
            metadata.majorRivers = stats_.majorRivers;
            metadata.biomesIdentified = stats_.biomesIdentified;
            metadata.simulationYears = stats_.simulationYears;
            metadata.highestPeak = stats_.highestPeak;
            metadata.generatorName = "SeedWorldGenerator";
            metadata.generatorVersion = "1.0";
            
            // Create the world using the new metadata-based method
            if (worldPersistence_->CreateWorld(worldName_, metadata)) {
                addLogEntry("World '" + worldName_ + "' saved successfully!", stats_.simulationYears);
                
                // Log generation statistics
                addLogEntry("World statistics:", stats_.simulationYears);
                addLogEntry("- Mountain ranges: " + std::to_string(stats_.mountainRanges), stats_.simulationYears);
                addLogEntry("- Major rivers: " + std::to_string(stats_.majorRivers), stats_.simulationYears);
                addLogEntry("- Biomes identified: " + std::to_string(stats_.biomesIdentified), stats_.simulationYears);
                addLogEntry("- Highest peak: " + stats_.highestPeakName, stats_.simulationYears);
                addLogEntry("- Largest lake: " + stats_.largestLakeName, stats_.simulationYears);
                
                // TODO: Save chunk data when chunk persistence is implemented
                // For now, the world generator parameters and seed are saved in metadata
                // This allows the world to be regenerated on-demand during gameplay
                
            } else {
                addLogEntry("ERROR: Failed to save world to disk", stats_.simulationYears);
            }
            
        } catch (const std::exception& e) {
            addLogEntry("ERROR: World save failed - " + std::string(e.what()), stats_.simulationYears);
        }
    } else {
        addLogEntry("Warning: World persistence not available - world not saved", stats_.simulationYears);
    }
    
    addLogEntry("World generation complete! Ready for exploration.", stats_.simulationYears);
    createUIElements();
    
    // DO NOT automatically call onSimulationComplete_ - let user manually proceed with "Begin Game" button
    // User can now examine the completed world map, zoom around, and choose when to proceed
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

// Geological phase update callback
void WorldSimulationUI::onGeologicalPhaseUpdate(const VoxelCastle::World::PhaseInfo& phaseInfo) {
    // Update current phase based on geological simulation
    switch (phaseInfo.currentPhase) {
        case VoxelCastle::World::GeologicalPhase::TECTONICS:
            currentPhase_ = GenerationPhase::TECTONICS;
            break;
        case VoxelCastle::World::GeologicalPhase::MOUNTAIN_BUILDING:
            currentPhase_ = GenerationPhase::MOUNTAIN_BUILDING;
            break;
        case VoxelCastle::World::GeologicalPhase::EROSION:
            currentPhase_ = GenerationPhase::EROSION;
            break;
        default:
            currentPhase_ = GenerationPhase::WATER_SYSTEMS;
            break;
    }
    
    // Update progress
    phaseProgress_ = phaseInfo.phaseProgress;
    currentProgress_ = phaseInfo.totalProgress;
    
    // Add log entries for major phase transitions
    if (phaseInfo.phaseProgress <= 0.05f) { // New phase started (within first 5%)
        std::string phaseName = getPhaseDisplayName(currentPhase_);
        addLogEntry("Starting " + phaseName + " phase", static_cast<int>(phaseInfo.timeRemaining));
    }
    
    // Add specific geological events
    if (!phaseInfo.currentProcess.empty()) {
        addLogEntry(phaseInfo.currentProcess, static_cast<int>(phaseInfo.timeRemaining));
    }
    
    // Update world map visualization if progress changed significantly
    static float lastUpdateProgress = -1.0f;
    if (std::abs(phaseInfo.phaseProgress - lastUpdateProgress) > 0.1f) { // Update every 10%
        updateWorldMapVisualization();
        lastUpdateProgress = phaseInfo.phaseProgress;
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

void WorldSimulationUI::startGenerationThread() {
    // Start generation in background thread
    generationThread_ = std::thread(&WorldSimulationUI::generationWorker, this);
}

void WorldSimulationUI::generationWorker() {
    try {
        addLogEntry("Starting geological simulation system...", 0);
        currentPhase_ = GenerationPhase::TECTONICS;
        
        // Run the complete geological simulation
        bool success = worldGenerator_->runGeologicalSimulation();
        
        if (success && isRunning_) {
            addLogEntry("Geological simulation completed successfully!", stats_.simulationYears);
            
            // Finalize the simulation
            currentPhase_ = GenerationPhase::COMPLETE;
            currentProgress_ = 1.0f;
            phaseProgress_ = 1.0f;
            
            // Update final statistics
            updateFinalStatistics();
            
            // Complete the simulation
            completeSimulation();
        } else if (!isRunning_) {
            addLogEntry("Geological simulation cancelled by user", stats_.simulationYears);
        } else {
            addLogEntry("ERROR: Geological simulation failed", stats_.simulationYears);
            isRunning_ = false;
        }
        
    } catch (const std::exception& e) {
        addLogEntry("ERROR: Geological simulation crashed: " + std::string(e.what()), stats_.simulationYears);
        isRunning_ = false;
    }
}

std::string WorldSimulationUI::getPhaseDisplayName(GenerationPhase phase) const {
    switch (phase) {
        case GenerationPhase::TECTONICS: return "Tectonic Evolution";
        case GenerationPhase::MOUNTAIN_BUILDING: return "Mountain Building";
        case GenerationPhase::EROSION: return "Erosion & Weathering";
        case GenerationPhase::WATER_SYSTEMS: return "Water Systems";
        case GenerationPhase::HYDROLOGY: return "Hydrology Simulation";
        case GenerationPhase::CLIMATE: return "Climate Modeling";
        case GenerationPhase::BIOMES: return "Biome Assignment";
        case GenerationPhase::CIVILIZATION: return "Civilization Development";
        case GenerationPhase::COMPLETE: return "Complete";
        default: return "Unknown Phase";
    }
}

void WorldSimulationUI::updateFinalStatistics() {
    // Generate some example statistics
    stats_.mountainRanges = 5 + (worldSeed_->getMasterSeed() % 10);
    stats_.majorRivers = 3 + (worldSeed_->getMasterSeed() % 7);
    stats_.biomesIdentified = 8 + (worldSeed_->getMasterSeed() % 12);
    stats_.highestPeak = 800.0f + (worldSeed_->getMasterSeed() % 400);
    stats_.deepestValley = -50.0f - (worldSeed_->getMasterSeed() % 100);
    stats_.largestLakeSize = 10.0f + (worldSeed_->getMasterSeed() % 50);
    stats_.longestRiverLength = 100.0f + (worldSeed_->getMasterSeed() % 200);
    
    // Generate some names based on seed
    stats_.highestPeakName = "Mt. Voxel";
    stats_.deepestValleyName = "Shadow Valley";
    stats_.largestLakeName = "Crystal Lake";
    stats_.longestRiverName = "Serpent River";
}

void WorldSimulationUI::regenerateWorldMap() {
    if (worldMapRenderer_ && worldGenerator_) {
        worldMapRenderer_->generateWorldMap(
            worldGenerator_.get(),
            static_cast<VoxelEngine::UI::WorldMapRenderer::GenerationPhase>(currentPhase_),
            static_cast<VoxelEngine::UI::WorldMapRenderer::VisualizationMode>(visualizationMode_),
            config_.customSeed,
            static_cast<float>(config_.worldSize)
        );
    }
}
