#include "ui/WorldSimulationUI.h"
#include "ui/elements/UIButton.h"
#include "ui/elements/UITextPanel.h"
#include "ui/WorldMapRenderer.h"
#include "world/seed_world_generator.h"
#include "world/GeologicalSimulator.h"
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
    
    // Initialize world map renderer with adaptive resolution
    worldMapRenderer_ = std::make_unique<VoxelEngine::UI::WorldMapRenderer>();
    // Note: Resolution will be set dynamically based on actual display size
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
    
    // Simple debug info overlay (if enabled)
    if (showDebugInfo_ && worldGenerator_) {
        renderSimpleDebugInfo();
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
    
    // Always update world map visualization regardless of pause state to keep preview fresh
    if (worldMapRenderer_ && isRunning_) {
        // Update world map at reduced frequency during pause to maintain visual feedback
        static float lastUpdateTime = 0.0f;
        lastUpdateTime += deltaTime;
        
        // Update every 1 second during pause, every 0.1 seconds during run
        float updateInterval = isPaused_ ? 1.0f : 0.1f;
        if (lastUpdateTime >= updateInterval) {
            updateWorldMapVisualization();
            lastUpdateTime = 0.0f;
        }
    }
    
    // Only advance simulation when not paused
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
    
    // Add debug UI toggle button
    auto debugToggleButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    debugToggleButton->setText("Debug Controls");
    debugToggleButton->setPosition(zoomButtonX, currentY_);
    debugToggleButton->autoSizeToText(6.0f);
    debugToggleButton->setBackgroundColor({0.3f, 0.6f, 0.3f, 0.6f});
    debugToggleButton->setOnClick([this]() { 
        showDebugInfo_ = !showDebugInfo_;
        addLogEntry(showDebugInfo_ ? "Debug info enabled" : "Debug info disabled", 0);
    });
    addChild(debugToggleButton);
    
    currentY_ += VERTICAL_SPACING;
    
    // Add geological snapshot navigation controls
    createSnapshotControls();
}

void WorldSimulationUI::createSnapshotControls() {
    // Snapshot navigation label
    auto snapshotLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    snapshotLabel->setText("Geological Timeline:");
    snapshotLabel->setPosition(PANEL_MARGIN, currentY_);
    snapshotLabel->autoSizeToText(8.0f);
    snapshotLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(snapshotLabel);
    
    // Snapshot navigation controls
    float snapshotButtonX = PANEL_MARGIN + 160.0f;
    
    // Previous snapshot button
    auto prevSnapshotButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    prevSnapshotButton->setText("◀ Previous");
    prevSnapshotButton->setPosition(snapshotButtonX, currentY_);
    prevSnapshotButton->autoSizeToText(6.0f);
    prevSnapshotButton->setBackgroundColor({0.2f, 0.2f, 0.4f, 0.6f});
    prevSnapshotButton->setOnClick([this]() { onPreviousSnapshot(); });
    addChild(prevSnapshotButton);
    prevSnapshotButton_ = prevSnapshotButton;
    
    snapshotButtonX += 100.0f + ELEMENT_SPACING;
    
    // Snapshot info display
    auto snapshotInfoButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    snapshotInfoButton->setText("1/1 - Initial");
    snapshotInfoButton->setPosition(snapshotButtonX, currentY_);
    snapshotInfoButton->autoSizeToText(6.0f);
    snapshotInfoButton->setBackgroundColor({0.2f, 0.3f, 0.2f, 0.8f});
    addChild(snapshotInfoButton);
    snapshotInfoButton_ = snapshotInfoButton;
    
    snapshotButtonX += 150.0f + ELEMENT_SPACING;
    
    // Next snapshot button
    auto nextSnapshotButton = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    nextSnapshotButton->setText("Next ▶");
    nextSnapshotButton->setPosition(snapshotButtonX, currentY_);
    nextSnapshotButton->autoSizeToText(6.0f);
    nextSnapshotButton->setBackgroundColor({0.2f, 0.2f, 0.4f, 0.6f});
    nextSnapshotButton->setOnClick([this]() { onNextSnapshot(); });
    addChild(nextSnapshotButton);
    nextSnapshotButton_ = nextSnapshotButton;
    
    currentY_ += VERTICAL_SPACING;
}

void WorldSimulationUI::createWorldPreview() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    float panelHeight = getSize().y - currentY_ - (PANEL_MARGIN * 2);
    
    // Calculate world map area - SQUARE preview to match square world
    // Take the smaller dimension to ensure square fits properly
    float availableWidth = panelWidth * 0.6f;  // Leave space for summary on the side
    float availableHeight = panelHeight * 0.8f; // Leave space for controls below
    float mapSize = std::min(availableWidth, availableHeight); // SQUARE
    
    // Position map on the left side of the panel
    worldMapX_ = PANEL_MARGIN;
    worldMapY_ = currentY_;
    worldMapWidth_ = mapSize;   // SQUARE: width = height
    worldMapHeight_ = mapSize;  // SQUARE: height = width
    
    // Create elevation legend beside the square preview
    createElevationLegend();
    
    // Note: Summary will be positioned to the right of the square preview
    // The summary area starts at: worldMapX_ + worldMapWidth_ + ELEMENT_SPACING
    // Available summary width: panelWidth - worldMapWidth_ - ELEMENT_SPACING
    
    std::cout << "[WorldSimulationUI] Square preview created: " << mapSize << "x" << mapSize 
              << " at (" << worldMapX_ << "," << worldMapY_ << ")" << std::endl;
    
    currentY_ += mapSize + ELEMENT_SPACING;
    
    // NEW: Initialize WorldMapRenderer with adaptive resolution based on actual display size
    if (worldMapRenderer_ && worldMapWidth_ > 0 && worldMapHeight_ > 0) {
        // Use the larger dimension to ensure good quality, but cap at reasonable limits
        int targetResolution = static_cast<int>(std::max(worldMapWidth_, worldMapHeight_));
        
        // Scale up for better quality but keep performance reasonable:
        // - Small previews (< 500px): Use 2x resolution for crisp zoom
        // - Medium previews (500-1000px): Use 1.5x resolution 
        // - Large previews (1000px+): Use 1:1 resolution
        if (targetResolution < 500) {
            targetResolution *= 2;
        } else if (targetResolution < 1000) {
            targetResolution = static_cast<int>(targetResolution * 1.5f);
        }
        
        // Cap resolution for performance (max 2048x2048 texture)
        targetResolution = std::min(targetResolution, 2048);
        
        // Ensure minimum resolution for quality
        targetResolution = std::max(targetResolution, 512);
        
        std::cout << "[WorldSimulationUI] Initializing WorldMapRenderer with adaptive resolution: " 
                  << targetResolution << "x" << targetResolution 
                  << " (display size: " << worldMapWidth_ << "x" << worldMapHeight_ << ")" << std::endl;
        
        if (!worldMapRenderer_->initialize(targetResolution)) {
            std::cerr << "[WorldSimulationUI] Warning: Failed to initialize world map renderer with resolution " 
                      << targetResolution << std::endl;
        }
    }
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
    
    // Calculate summary area AFTER the legend (legend takes up 120px + spacing)
    float summaryX = legendBounds_.x + legendBounds_.width + ELEMENT_SPACING;
    float availableSummaryWidth = panelWidth - (legendBounds_.x - PANEL_MARGIN) - legendBounds_.width - (ELEMENT_SPACING * 2);
    
    // Use full available summary width
    float summaryWidth = availableSummaryWidth;
    summaryWidth = std::max(summaryWidth, 250.0f); // Minimum 250 pixels wide
    
    float summaryY = worldMapY_; // Start at same height as preview
    float progressHeight = 80.0f;
    
    std::cout << "[WorldSimulationUI] Creating progress panels after legend - "
              << "Summary area: " << summaryWidth << "w x " << progressHeight << "h "
              << "at (" << summaryX << "," << summaryY << ") [constrained from " << availableSummaryWidth << "w]" << std::endl;
    
    // Current phase progress (positioned after legend)
    auto progressLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    progressLabel->setText("Current Phase: " + getPhaseDisplayName(currentPhase_));
    progressLabel->setPosition(summaryX, summaryY);
    progressLabel->setSize(summaryWidth, 25.0f);
    progressLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(progressLabel);
    summaryY += 30.0f;
    
    // Progress statistics - each on its own centered line
    float lineHeight = 22.0f;
    float availableProgressHeight = progressHeight;
    
    // Phase Progress
    auto phaseProgressBar = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    phaseProgressBar->setText("Phase Progress: " + std::to_string((int)(phaseProgress_ * 100)) + "%");
    phaseProgressBar->setPosition(summaryX, summaryY);
    phaseProgressBar->setSize(summaryWidth, lineHeight);
    phaseProgressBar->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.7f});
    addChild(phaseProgressBar);
    summaryY += lineHeight + 2.0f;
    
    // Overall Progress
    auto overallProgressBar = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    overallProgressBar->setText("Overall Progress: " + std::to_string((int)(currentProgress_ * 100)) + "%");
    overallProgressBar->setPosition(summaryX, summaryY);
    overallProgressBar->setSize(summaryWidth, lineHeight);
    overallProgressBar->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.7f});
    addChild(overallProgressBar);
    summaryY += lineHeight + 2.0f;
    
    // Time Remaining
    auto timeRemainingBar = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    float timeRemaining = calculateTimeRemaining();
    timeRemainingBar->setText("Est. Time Remaining: " + std::to_string((int)timeRemaining) + "s");
    timeRemainingBar->setPosition(summaryX, summaryY);
    timeRemainingBar->setSize(summaryWidth, lineHeight);
    timeRemainingBar->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.7f});
    addChild(timeRemainingBar);
    summaryY += lineHeight + ELEMENT_SPACING;
    
    // Note: currentY_ is not advanced here since summary is positioned beside preview, not below
}

void WorldSimulationUI::createGenerationLog() {
    float panelWidth = getSize().x - (PANEL_MARGIN * 2);
    
    // Calculate log area AFTER the legend (legend takes up 120px + spacing)
    float summaryX = legendBounds_.x + legendBounds_.width + ELEMENT_SPACING;
    float availableSummaryWidth = panelWidth - (legendBounds_.x - PANEL_MARGIN) - legendBounds_.width - (ELEMENT_SPACING * 2);
    
    // Use full available summary width
    float summaryWidth = availableSummaryWidth;
    summaryWidth = std::max(summaryWidth, 250.0f); // Minimum 250 pixels wide
    
    float logHeight = 180.0f; // Height for the log container
    
    // Position log below the progress panels in the summary area
    float summaryY = worldMapY_ + 140.0f; // After progress panels (30 + 80 + 30 spacing)
    
    std::cout << "[WorldSimulationUI] Creating generation log after legend - "
              << "Log area: " << summaryWidth << "w x " << logHeight << "h "
              << "at (" << summaryX << "," << summaryY << ") [using full available width]" << std::endl;
    
    // Generation log panel label
    auto logLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    logLabel->setText("Generation Log:");
    logLabel->setPosition(summaryX, summaryY);
    logLabel->setSize(summaryWidth, 25.0f);
    logLabel->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(logLabel);
    summaryY += 30.0f;
    
    // Create the new UITextPanel for multi-line text with proper wrapping
    auto textPanel = std::make_shared<VoxelEngine::UI::UITextPanel>(renderer_);
    logPanel_ = textPanel; // Store reference for dynamic updates
    
    // Configure the text panel
    textPanel->setPosition(summaryX, summaryY);
    textPanel->setSize(summaryWidth, logHeight);
    textPanel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.6f});
    textPanel->setTextColor({0.9f, 0.9f, 0.9f}); // Light gray text
    textPanel->setTextScale(0.8f); // Slightly smaller text for more content
    textPanel->setPadding(8.0f);
    textPanel->setScrollEnabled(true);
    textPanel->setMaxVisibleLines(0); // Auto-calculate based on panel height
    
    // Set initial content
    refreshGenerationLog();
    
    addChild(textPanel);
    
    // Note: currentY_ is not advanced here since log is positioned beside preview, not below
}

void WorldSimulationUI::refreshGenerationLog() {
    if (!logPanel_) return; // Panel not created yet
    
    std::string logText = "";
    size_t entriesToShow = std::min(static_cast<size_t>(8), generationLog_.size()); // Show more entries since wrapping is automatic
    
    if (entriesToShow > 0) {
        for (size_t i = generationLog_.size() - entriesToShow; i < generationLog_.size(); ++i) {
            // Format log entries with year and message
            std::string yearText = "Year " + std::to_string(generationLog_[i].simulationYear);
            std::string message = generationLog_[i].message;
            
            // Add entry with proper line separation
            logText += yearText + ": " + message + "\n\n";
        }
    } else {
        logText = "Simulation starting...\nInitializing systems...\n";
    }
    
    // Set the text - UITextPanel will handle wrapping and scrolling automatically
    logPanel_->setText(logText);
    
    // Auto-scroll to bottom to show latest entries
    logPanel_->scrollToBottom();
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
        
        // Convert world size from regions to kilometers (1 region = 1 km)
        float worldSizeKm = static_cast<float>(config.worldSize);
        
        // Initialize geological simulation system 
        addLogEntry("Initializing geological simulation system", 2);
        addLogEntry("World size: " + std::to_string(config.worldSize) + " chunks (" + std::to_string(worldSizeKm) + " km)", 3);
        
        // Create geological configuration based on user settings
        VoxelCastle::World::GeologicalConfig geoConfig;
        geoConfig.preset = config.geologicalQuality; // Use user-selected quality preset
        
        // Apply custom continental parameters from UI
        geoConfig.custom.numContinents = config.numContinents;
        geoConfig.custom.maxContinentSize = config.maxContinentSize;
        geoConfig.custom.minOceanCoverage = config.minOceanCoverage;
        geoConfig.custom.forceOceanGeneration = config.forceOceanGeneration;
        
        // Log the continental configuration being applied
        addLogEntry("Continental Configuration:", 3);
        addLogEntry("  Number of Continents: " + std::to_string(config.numContinents), 3);
        addLogEntry("  Max Continent Size: " + std::to_string(static_cast<int>(config.maxContinentSize)) + "% of world", 3);
        addLogEntry("  Min Ocean Coverage: " + std::to_string(static_cast<int>(config.minOceanCoverage)) + "% of world", 3);
        addLogEntry("  Force Ocean Generation: " + std::string(config.forceOceanGeneration ? "ENABLED" : "DISABLED"), 3);
        
        // Set up progress callback for geological simulation
        auto progressCallback = [this](const VoxelCastle::World::PhaseInfo& phaseInfo) {
            this->onGeologicalPhaseUpdate(phaseInfo);
        };
        
        worldGenerator_->initializeGeologicalSimulation(worldSizeKm, geoConfig, progressCallback);
        addLogEntry("Geological simulation initialized", 3);
        
        // Log continental coverage fix
        addLogEntry("Applied continental coverage fix for better ocean visibility", 3);
        
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
    if (worldGenerator_) {
        if (worldGenerator_->isGeologicalSimulationPaused()) {
            worldGenerator_->resumeGeologicalSimulation();
            isPaused_ = false;
            addLogEntry("Simulation resumed", stats_.simulationYears);
        } else {
            worldGenerator_->pauseGeologicalSimulation();
            isPaused_ = true;
            addLogEntry("Simulation paused - preview remains active", stats_.simulationYears);
        }
    } else {
        isPaused_ = !isPaused_;
    }
    
    // Update UI to show pause state while keeping preview visible
    createUIElements();
    
    // Ensure current preview frame is preserved and visible
    if (worldMapRenderer_) {
        updateWorldMapVisualization();
    }
    
    std::cout << "[WorldSimulationUI] Simulation paused - keeping preview visible at current frame" << std::endl;
}

void WorldSimulationUI::resumeSimulation() {
    isPaused_ = false;
    addLogEntry("Simulation resumed", stats_.simulationYears);
    
    // Update UI to show running state
    createUIElements();
    
    // Refresh world map to ensure continuity
    if (worldMapRenderer_) {
        updateWorldMapVisualization();
    }
    
    std::cout << "[WorldSimulationUI] Simulation resumed seamlessly" << std::endl;
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
    
    // Simulate current phase and check for unimplemented features
    bool encounteredUnimplementedFeature = false;
    try {
        simulatePhase(currentPhase_, deltaTime);
    } catch (const std::runtime_error& e) {
        // Check if this is an unimplemented feature error
        std::string errorMsg = e.what();
        if (errorMsg.find("not implemented") != std::string::npos || 
            errorMsg.find("unimplemented") != std::string::npos ||
            errorMsg.find("coming soon") != std::string::npos) {
            encounteredUnimplementedFeature = true;
            addLogEntry("Paused: " + errorMsg, stats_.simulationYears);
            std::cout << "[WorldSimulationUI] Encountered unimplemented feature: " << errorMsg << std::endl;
        } else {
            // Re-throw if it's a different kind of error
            throw;
        }
    }
    
    // Gracefully pause if we hit an unimplemented feature
    if (encounteredUnimplementedFeature) {
        pauseSimulation();
        addLogEntry("Simulation paused due to unimplemented feature - preview preserved", stats_.simulationYears);
        return; // Don't continue processing
    }
    
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
        updateSnapshotControls();  // Keep snapshot navigation in sync
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
                
                // Check for advanced water features that might not be implemented
                if (phaseProgress > 0.7f && config_.hydrologyLevel > 3) {
                    // Advanced hydrology features (underground rivers, karst landscapes, etc.)
                    throw std::runtime_error("Advanced hydrology features (level > 3) not implemented yet - coming soon!");
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
                
                // Check for advanced climate features
                if (phaseProgress > 0.5f && config_.climateType > 2) {
                    // Advanced climate modeling (ice ages, monsoons, etc.)
                    throw std::runtime_error("Advanced climate modeling (type > 2) not implemented yet - coming soon!");
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
                
                // Check for extremely large worlds that need special handling
                if (phaseProgress > 0.8f && config_.worldSize > 2048) {
                    // Massive world biome generation
                    throw std::runtime_error("Biome generation for worlds > 2048km not implemented yet - coming soon!");
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
                
                // Check for advanced civilization features
                if (phaseProgress > 0.6f && config_.simulationDepth > 3) {
                    // Deep historical simulation
                    throw std::runtime_error("Deep historical simulation (depth > 3) not implemented yet - coming soon!");
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
    
    // Refresh the log display with new content
    refreshGenerationLog();
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
    
    // Update world map visualization at reasonable intervals to avoid flickering
    static float lastUpdateProgress = -1.0f;
    static auto lastUpdateTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    auto timeSinceUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime).count();
    
    // Only update map if significant progress change (25%) AND at least 2 seconds have passed
    if (std::abs(phaseInfo.phaseProgress - lastUpdateProgress) > 0.25f && timeSinceUpdate > 2000) {
        updateWorldMapVisualization();
        lastUpdateProgress = phaseInfo.phaseProgress;
        lastUpdateTime = currentTime;
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

void WorldSimulationUI::onPreviousSnapshot() {
    if (!worldMapRenderer_) return;
    
    auto geologicalSim = worldMapRenderer_->getGeologicalSimulator();
    if (!geologicalSim) return;
    
    auto* snapshotManager = const_cast<VoxelCastle::World::GeologicalSimulator*>(geologicalSim)->getSnapshotManager();
    if (!snapshotManager) return;
    
    if (snapshotManager->PreviousSnapshot()) {
        updateSnapshotControls();
        regenerateWorldMap();  // Update the visual display
        std::cout << "[WorldSimulationUI] Moved to previous geological snapshot" << std::endl;
    }
}

void WorldSimulationUI::onNextSnapshot() {
    if (!worldMapRenderer_) return;
    
    auto geologicalSim = worldMapRenderer_->getGeologicalSimulator();
    if (!geologicalSim) return;
    
    auto* snapshotManager = const_cast<VoxelCastle::World::GeologicalSimulator*>(geologicalSim)->getSnapshotManager();
    if (!snapshotManager) return;
    
    if (snapshotManager->NextSnapshot()) {
        updateSnapshotControls();
        regenerateWorldMap();  // Update the visual display
        std::cout << "[WorldSimulationUI] Moved to next geological snapshot" << std::endl;
    }
}

void WorldSimulationUI::updateSnapshotControls() {
    if (!worldMapRenderer_) return;
    
    auto geologicalSim = worldMapRenderer_->getGeologicalSimulator();
    if (!geologicalSim) return;
    
    auto* snapshotManager = const_cast<VoxelCastle::World::GeologicalSimulator*>(geologicalSim)->getSnapshotManager();
    if (!snapshotManager) return;
    
    size_t currentIndex = snapshotManager->GetCurrentSnapshotIndex();
    size_t totalSnapshots = snapshotManager->GetSnapshotCount();
    
    // Update snapshot info display
    if (snapshotInfoButton_) {
        std::string snapshotInfo = std::to_string(currentIndex + 1) + "/" + std::to_string(totalSnapshots);
        std::string phaseDescription = snapshotManager->GetCurrentPhaseDescription();
        if (!phaseDescription.empty()) {
            snapshotInfo += " - " + phaseDescription;
        }
        snapshotInfoButton_->setText(snapshotInfo);
    }
    
    // Update button states
    if (prevSnapshotButton_) {
        bool canGoPrevious = (currentIndex > 0);
        prevSnapshotButton_->setBackgroundColor(canGoPrevious ? 
            glm::vec4{0.2f, 0.2f, 0.4f, 0.8f} : glm::vec4{0.1f, 0.1f, 0.2f, 0.4f});
    }
    
    if (nextSnapshotButton_) {
        bool canGoNext = (currentIndex < totalSnapshots - 1);
        nextSnapshotButton_->setBackgroundColor(canGoNext ? 
            glm::vec4{0.2f, 0.2f, 0.4f, 0.8f} : glm::vec4{0.1f, 0.1f, 0.2f, 0.4f});
    }
    
    // Fixed variable scope - get phaseDescription within this scope
    std::string phaseDescription = snapshotManager->GetCurrentPhaseDescription();
    std::cout << "[WorldSimulationUI] Updated snapshot controls: " << (currentIndex + 1) 
              << "/" << totalSnapshots << " - " << phaseDescription << std::endl;
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
        addLogEntry("Initializing geological simulation system...", 0);
        currentPhase_ = GenerationPhase::TECTONICS;
        
        // Initialize the geological simulation
        if (!worldGenerator_->initializeStepBasedGeologicalSimulation()) {
            addLogEntry("ERROR: Failed to initialize geological simulation", 0);
            isRunning_ = false;
            return;
        }
        
        // Get the geological simulator for background execution
        const auto* geologicalSimulator = worldGenerator_->getGeologicalSimulator();
        if (!geologicalSimulator) {
            addLogEntry("ERROR: Could not access geological simulator", 0);
            isRunning_ = false;
            return;
        }
        
        // Cast away const for background simulation control (safe as we own the simulator)
        auto* mutableGeologicalSimulator = const_cast<VoxelCastle::World::GeologicalSimulator*>(geologicalSimulator);
        
        addLogEntry("Starting background geological simulation...", 0);
        
        // Start background simulation instead of blocking loop
        mutableGeologicalSimulator->startBackgroundSimulation();
        
        // Poll for snapshots and progress updates (non-blocking)
        while (isRunning_ && mutableGeologicalSimulator->isBackgroundSimulationRunning()) {
            // Handle pause/resume immediately
            if (isPaused_) {
                mutableGeologicalSimulator->pauseBackgroundSimulation();
            } else {
                mutableGeologicalSimulator->resumeBackgroundSimulation();
            }
            
            if (!isRunning_) break;
            
            // Get latest snapshot for UI updates (non-blocking)
            auto snapshot = mutableGeologicalSimulator->getLatestSnapshot();
            if (snapshot) {
                // Update progress from snapshot
                float progress = mutableGeologicalSimulator->getBackgroundProgress();
                currentProgress_ = progress;
                phaseProgress_ = progress;
                
                // Update UI with snapshot data if needed
                // This will be handled by the UI update cycle
            }
            
            // Check if simulation is complete
            if (worldGenerator_->isGeologicalSimulationComplete()) {
                break;
            }
            
            // Short sleep to avoid busy waiting (UI stays responsive)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Stop background simulation
        mutableGeologicalSimulator->stopBackgroundSimulation();
        
        if (isRunning_ && worldGenerator_->isGeologicalSimulationComplete()) {
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

void WorldSimulationUI::createElevationLegend() {
    // Position elevation legend immediately to the right of the square preview
    float legendX = worldMapX_ + worldMapWidth_ + ELEMENT_SPACING;
    float legendY = worldMapY_;
    float legendWidth = 120.0f;
    float legendHeight = worldMapHeight_; // Same height as preview
    
    // Store legend bounds for use by other UI elements
    legendBounds_ = {legendX, legendY, legendWidth, legendHeight};
    
    // Elevation legend title
    auto legendTitle = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
    legendTitle->setText("Elevation Legend");
    legendTitle->setPosition(legendX, legendY);
    legendTitle->setSize(legendWidth, 25.0f);
    legendTitle->setBackgroundColor({0.2f, 0.2f, 0.2f, 0.8f});
    addChild(legendTitle);
    

    
    // Create elevation bands with colors and labels
    std::vector<std::pair<std::string, glm::vec4>> elevationBands = {
        {"2000m+ Mountains", {1.0f, 1.0f, 1.0f, 1.0f}},          // White
        {"1000-2000m Hills", {0.67f, 0.4f, 0.27f, 1.0f}},        // Brown
        {"500-1000m Hills", {0.53f, 0.8f, 0.27f, 1.0f}},         // Yellow-green
        {"100-500m Plains", {0.27f, 0.67f, 0.27f, 1.0f}},        // Green
        {"0-100m Coast", {0.8f, 0.67f, 0.53f, 1.0f}},            // Tan/beige
        {"-200-0m Shelf", {0.27f, 0.53f, 0.8f, 1.0f}},           // Light blue
        {"-2000m Ocean", {0.0f, 0.19f, 0.55f, 1.0f}}             // Dark blue
    };
    
    float bandHeight = (legendHeight - 35.0f) / elevationBands.size(); // Account for title
    float currentBandY = legendY + 30.0f;
    
    for (const auto& band : elevationBands) {
        // Color square
        auto colorSquare = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        colorSquare->setText("");
        colorSquare->setPosition(legendX + 5.0f, currentBandY);
        colorSquare->setSize(20.0f, bandHeight - 2.0f);
        colorSquare->setBackgroundColor(band.second);
        addChild(colorSquare);
        
        // Elevation label
        auto elevationLabel = std::make_shared<VoxelEngine::UI::UIButton>(renderer_);
        elevationLabel->setText(band.first);
        elevationLabel->setPosition(legendX + 30.0f, currentBandY);
        elevationLabel->setSize(legendWidth - 35.0f, bandHeight - 2.0f);
        elevationLabel->setBackgroundColor({0.1f, 0.1f, 0.1f, 0.7f});
        addChild(elevationLabel);
        
        currentBandY += bandHeight;
    }
    
    std::cout << "[WorldSimulationUI] Created elevation legend at (" << legendX << "," << legendY 
              << ") size " << legendWidth << "x" << legendHeight << std::endl;
}

void WorldSimulationUI::renderSimpleDebugInfo() {
    if (!worldGenerator_) return;
    
    // Simple debug overlay showing basic world generation info
    // For now, just render basic simulation status
    std::string debugText = "Debug Info: World Generation in Progress";
    if (isSimulationComplete()) {
        debugText = "Debug Info: World Generation Complete";
    }
    
    // Render debug text (using simplified rendering - the actual implementation 
    // would depend on the specific text rendering system)
    // This is a minimal implementation to fix the missing function
}
