#include "ui/GeologicalDebugUI.h"
#include "ui/UIRenderer.h"
#include "ui/UILogger.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace VoxelEngine {
namespace UI {

GeologicalDebugUI::GeologicalDebugUI() {
    // Initialize with default balanced configuration
    currentConfig_.preset = VoxelCastle::World::GeologicalPreset::BALANCED;
    updateParametersFromPreset();
    
    // Set initial size and position
    setSize(400.0f, 600.0f);
    setPosition(50.0f, 50.0f);
    setBackgroundColor({0.1f, 0.1f, 0.1f, 0.9f});
    
    UILogger::getInstance().info("GeologicalDebugUI", "Geological Debug UI initialized");
}

void GeologicalDebugUI::update(float deltaTime) {
    UIElement::update(deltaTime);
    
    lastUpdateTime_ += deltaTime;
    if (lastUpdateTime_ >= updateInterval_) {
        // Update cached display strings periodically to reduce string allocations
        if (backgroundSim_) {
            auto progress = backgroundSim_->GetProgress();
            cachedProgressText_ = std::to_string(static_cast<int>(progress.percentComplete * 100)) + "%";
            
            std::ostringstream perfStream;
            perfStream << std::fixed << std::setprecision(1);
            perfStream << "Steps/sec: " << (1.0f / std::max(0.001f, progress.averageStepTime));
            cachedPerformanceText_ = perfStream.str();
        }
        
        // Update terrain statistics from latest snapshot
        if (latestSnapshot_.isValid) {
            std::ostringstream statsStream;
            statsStream << std::fixed << std::setprecision(1);
            statsStream << "Avg Elevation: " << latestSnapshot_.averageElevation << "m\n";
            statsStream << "Max Elevation: " << latestSnapshot_.maxElevation << "m\n";
            statsStream << "Min Elevation: " << latestSnapshot_.minElevation << "m\n";
            statsStream << "Active Regions: " << latestSnapshot_.activeRegions;
            cachedStatsText_ = statsStream.str();
        }
        
        lastUpdateTime_ = 0.0f;
    }
}

void GeologicalDebugUI::render() {
    if (!isVisible()) return;
    
    // Reset Y position for rendering
    currentY_ = getPosition().y + 10.0f;
    
    renderHeader();
    
    if (showSimulationControls_) {
        renderSimulationControls();
    }
    
    if (showParameterSliders_) {
        renderParameterSliders();
    }
    
    if (showProgressMonitoring_) {
        renderProgressMonitoring();
    }
    
    if (showPerformanceMetrics_) {
        renderPerformanceMetrics();
    }
    
    if (showPhaseControls_) {
        renderPhaseControls();
    }
    
    if (showTerrainStats_) {
        renderTerrainStats();
    }
    
    if (showSnapshotInfo_) {
        renderSnapshotInfo();
    }
    
    UIElement::render();
}

bool GeologicalDebugUI::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!isVisible()) return false;
    
    // Handle dragging the panel
    if (clicked) {
        float relX = mouseX - getPosition().x;
        float relY = mouseY - getPosition().y;
        
        // Check if clicking in header area for dragging
        if (relX >= 0 && relX <= getSize().x && relY >= 0 && relY <= 30.0f) {
            isDragging_ = true;
            dragStartX_ = relX;
            dragStartY_ = relY;
            return true;
        }
    } else {
        isDragging_ = false;
    }
    
    if (isDragging_) {
        setPosition(mouseX - dragStartX_, mouseY - dragStartY_);
        return true;
    }
    
    return UIElement::handleInput(mouseX, mouseY, clicked);
}

void GeologicalDebugUI::setBackgroundSimulation(std::shared_ptr<VoxelCastle::World::BackgroundSimulationEngine> bgSim) {
    backgroundSim_ = bgSim;
    UILogger::getInstance().info("GeologicalDebugUI", "Background simulation engine connected");
}

void GeologicalDebugUI::setConfigurationCallback(std::function<void(const VoxelCastle::World::GeologicalConfig&)> callback) {
    configCallback_ = callback;
}

void GeologicalDebugUI::setCurrentSnapshot(const VoxelCastle::World::GeologicalSnapshot& snapshot) {
    latestSnapshot_ = snapshot;
}

void GeologicalDebugUI::renderHeader() {
    float x = getPosition().x + 10.0f;
    
    // Main title
    if (getRenderer()) {
        getRenderer()->drawText("Geological Simulation Debug", x, currentY_, 16.0f, {1.0f, 1.0f, 1.0f, 1.0f});
    }
    currentY_ += 25.0f;
    
    // Preset selector
    std::string presetText = "Preset: ";
    switch (currentConfig_.preset) {
        case VoxelCastle::World::GeologicalPreset::PERFORMANCE: presetText += "Performance"; break;
        case VoxelCastle::World::GeologicalPreset::BALANCED: presetText += "Balanced"; break;
        case VoxelCastle::World::GeologicalPreset::QUALITY: presetText += "Quality"; break;
        case VoxelCastle::World::GeologicalPreset::ULTRA_REALISM: presetText += "Ultra Realism"; break;
        case VoxelCastle::World::GeologicalPreset::CUSTOM: presetText += "Custom"; break;
    }
    
    if (getRenderer()) {
        getRenderer()->drawText(presetText, x, currentY_, 12.0f, {0.8f, 0.8f, 0.8f, 1.0f});
    }
    currentY_ += 20.0f;
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderSimulationControls() {
    renderSectionHeader("Simulation Control", showSimulationControls_);
    
    float x = getPosition().x + 20.0f;
    
    // Start/Stop/Pause buttons
    if (backgroundSim_) {
        auto status = backgroundSim_->GetStatus();
        
        if (status == VoxelCastle::World::SimulationStatus::IDLE) {
            renderButton("Start", [this]() {
                if (backgroundSim_) {
                    backgroundSim_->StartSimulation(currentConfig_);
                    UILogger::getInstance().info("GeologicalDebugUI", "Simulation started");
                }
            });
        } else if (status == VoxelCastle::World::SimulationStatus::RUNNING) {
            renderButton("Pause", [this]() {
                if (backgroundSim_) {
                    backgroundSim_->PauseSimulation();
                    UILogger::getInstance().info("GeologicalDebugUI", "Simulation paused");
                }
            });
        } else if (status == VoxelCastle::World::SimulationStatus::PAUSED) {
            renderButton("Resume", [this]() {
                if (backgroundSim_) {
                    backgroundSim_->ResumeSimulation();
                    UILogger::getInstance().info("GeologicalDebugUI", "Simulation resumed");
                }
            });
        }
        
        // Stop button (always available when not idle)
        if (status != VoxelCastle::World::SimulationStatus::IDLE) {
            renderButton("Stop", [this]() {
                if (backgroundSim_) {
                    backgroundSim_->StopSimulation();
                    UILogger::getInstance().info("GeologicalDebugUI", "Simulation stopped");
                }
            });
        }
        
        // Reset button
        renderButton("Reset", [this]() {
            if (backgroundSim_) {
                backgroundSim_->StopSimulation();
                backgroundSim_->ResetSimulation();
                UILogger::getInstance().info("GeologicalDebugUI", "Simulation reset");
            }
        });
    } else {
        if (getRenderer()) {
            getRenderer()->drawText("No simulation engine connected", x, currentY_, 12.0f, {0.8f, 0.4f, 0.4f, 1.0f});
        }
        currentY_ += ELEMENT_HEIGHT;
    }
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderParameterSliders() {
    renderSectionHeader("Parameters", showParameterSliders_);
    
    // Time scale multiplier
    currentY_ += renderFloatSlider("Time Scale", currentConfig_.custom.timeScaleMultiplier, 0.1f, 5.0f, "%.1fx");
    
    // Simulation detail level
    currentY_ += renderIntSlider("Detail Level", currentConfig_.custom.simulationDetailLevel, 100, 10000);
    
    // Feature toggles
    currentY_ += renderBoolToggle("Chemical Weathering", currentConfig_.custom.enableChemicalWeathering);
    currentY_ += renderBoolToggle("Joint Systems", currentConfig_.custom.enableJointSystems);
    currentY_ += renderBoolToggle("Cave Systems", currentConfig_.custom.enableCaveSystems);
    currentY_ += renderBoolToggle("Glacial Features", currentConfig_.custom.enableGlacialFeatures);
    
    // Apply changes button
    currentY_ += renderButton("Apply Changes", [this]() {
        applyConfigurationChange();
    }, SLIDER_WIDTH);
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderProgressMonitoring() {
    renderSectionHeader("Progress", showProgressMonitoring_);
    
    if (backgroundSim_) {
        auto progress = backgroundSim_->GetProgress();
        
        // Progress bar
        std::string statusText = "Step " + std::to_string(progress.currentStep) + "/" + std::to_string(progress.totalSteps);
        currentY_ += renderProgressBar("Simulation", progress.percentComplete, statusText);
        
        // Current phase
        std::string phaseText = "Phase: ";
        switch (progress.currentPhase) {
            case VoxelCastle::World::GeologicalPhase::TECTONICS: phaseText += "Tectonics"; break;
            case VoxelCastle::World::GeologicalPhase::MOUNTAIN_BUILDING: phaseText += "Mountain Building"; break;
            case VoxelCastle::World::GeologicalPhase::EROSION: phaseText += "Erosion"; break;
            case VoxelCastle::World::GeologicalPhase::DETAIL: phaseText += "Detail"; break;
        }
        currentY_ += renderStatusText("", phaseText);
        
        // Time remaining
        if (progress.estimatedTimeRemaining > 0) {
            std::string timeText = "ETA: " + formatDuration(progress.estimatedTimeRemaining);
            currentY_ += renderStatusText("", timeText);
        }
    }
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderPerformanceMetrics() {
    renderSectionHeader("Performance", showPerformanceMetrics_);
    
    if (backgroundSim_) {
        auto progress = backgroundSim_->GetProgress();
        
        currentY_ += renderStatusText("Steps/sec", std::to_string(static_cast<int>(1.0f / std::max(0.001f, progress.averageStepTime))));
        currentY_ += renderStatusText("Avg Step Time", formatDuration(progress.averageStepTime));
        
        // Memory usage (estimated)
        if (latestSnapshot_.isValid) {
            size_t estimatedMemory = latestSnapshot_.activeRegions * 1024 * 1024; // Rough estimate
            currentY_ += renderStatusText("Memory Usage", formatMemorySize(estimatedMemory));
        }
    }
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderPhaseControls() {
    renderSectionHeader("Phase Control", showPhaseControls_);
    
    // Individual phase toggles (for future phase-specific control)
    currentY_ += renderStatusText("", "Phase-specific controls coming soon...");
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderTerrainStats() {
    renderSectionHeader("Terrain Statistics", showTerrainStats_);
    
    if (latestSnapshot_.isValid) {
        currentY_ += renderStatusText("Avg Elevation", std::to_string(static_cast<int>(latestSnapshot_.averageElevation)) + "m");
        currentY_ += renderStatusText("Max Elevation", std::to_string(static_cast<int>(latestSnapshot_.maxElevation)) + "m");
        currentY_ += renderStatusText("Min Elevation", std::to_string(static_cast<int>(latestSnapshot_.minElevation)) + "m");
        currentY_ += renderStatusText("Active Regions", std::to_string(latestSnapshot_.activeRegions));
        currentY_ += renderStatusText("Water Coverage", std::to_string(static_cast<int>(latestSnapshot_.waterCoverage * 100)) + "%");
    } else {
        currentY_ += renderStatusText("", "No terrain data available");
    }
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderSnapshotInfo() {
    renderSectionHeader("Snapshot Info", showSnapshotInfo_);
    
    if (latestSnapshot_.isValid) {
        currentY_ += renderStatusText("Timestamp", std::to_string(latestSnapshot_.timestamp));
        currentY_ += renderStatusText("Step Number", std::to_string(latestSnapshot_.stepNumber));
        currentY_ += renderStatusText("Phase", std::to_string(static_cast<int>(latestSnapshot_.currentPhase)));
    }
    
    currentY_ += SECTION_SPACING;
}

void GeologicalDebugUI::renderSectionHeader(const std::string& title, bool& toggleState) {
    float x = getPosition().x + 10.0f;
    
    // Render collapsible section header
    std::string headerText = (toggleState ? "▼ " : "► ") + title;
    
    if (getRenderer()) {
        getRenderer()->drawText(headerText, x, currentY_, 14.0f, {0.9f, 0.9f, 0.9f, 1.0f});
    }
    
    currentY_ += 20.0f;
    
    if (!toggleState) {
        currentY_ += 5.0f; // Small spacing when collapsed
    }
}

float GeologicalDebugUI::renderFloatSlider(const std::string& label, float& value, float min, float max, const std::string& format) {
    float x = getPosition().x + 20.0f;
    
    // Label
    if (getRenderer()) {
        getRenderer()->drawText(label, x, currentY_, 12.0f, {0.8f, 0.8f, 0.8f, 1.0f});
    }
    
    // Value display
    char valueStr[32];
    snprintf(valueStr, sizeof(valueStr), format.c_str(), value);
    
    if (getRenderer()) {
        getRenderer()->drawText(valueStr, x + LABEL_WIDTH + SLIDER_WIDTH + 10.0f, currentY_, 12.0f, {1.0f, 1.0f, 1.0f, 1.0f});
    }
    
    // TODO: Implement actual slider rendering and interaction
    // For now, just reserve space
    
    return ELEMENT_HEIGHT;
}

float GeologicalDebugUI::renderIntSlider(const std::string& label, int& value, int min, int max) {
    float x = getPosition().x + 20.0f;
    
    // Label
    if (getRenderer()) {
        getRenderer()->drawText(label, x, currentY_, 12.0f, {0.8f, 0.8f, 0.8f, 1.0f});
    }
    
    // Value display
    std::string valueStr = std::to_string(value);
    
    if (getRenderer()) {
        getRenderer()->drawText(valueStr, x + LABEL_WIDTH + SLIDER_WIDTH + 10.0f, currentY_, 12.0f, {1.0f, 1.0f, 1.0f, 1.0f});
    }
    
    // TODO: Implement actual slider rendering and interaction
    
    return ELEMENT_HEIGHT;
}

float GeologicalDebugUI::renderBoolToggle(const std::string& label, bool& value) {
    float x = getPosition().x + 20.0f;
    
    // Label
    if (getRenderer()) {
        getRenderer()->drawText(label, x, currentY_, 12.0f, {0.8f, 0.8f, 0.8f, 1.0f});
    }
    
    // Toggle state
    std::string toggleText = value ? "ON" : "OFF";
    float color = value ? 0.8f : 0.4f;
    
    if (getRenderer()) {
        getRenderer()->drawText(toggleText, x + LABEL_WIDTH, currentY_, 12.0f, {color, color, color, 1.0f});
    }
    
    // TODO: Implement actual toggle rendering and interaction
    
    return ELEMENT_HEIGHT;
}

float GeologicalDebugUI::renderButton(const std::string& label, std::function<void()> callback, float width) {
    float x = getPosition().x + 20.0f;
    
    // Button background (simple rectangle for now)
    if (getRenderer()) {
        getRenderer()->drawText(label, x, currentY_, 12.0f, {0.9f, 0.9f, 0.9f, 1.0f});
    }
    
    // TODO: Implement actual button rendering and click detection
    
    return ELEMENT_HEIGHT;
}

float GeologicalDebugUI::renderProgressBar(const std::string& label, float progress, const std::string& status) {
    float x = getPosition().x + 20.0f;
    
    // Label
    if (getRenderer()) {
        getRenderer()->drawText(label, x, currentY_, 12.0f, {0.8f, 0.8f, 0.8f, 1.0f});
    }
    
    // Progress percentage
    std::string progressText = std::to_string(static_cast<int>(progress * 100)) + "%";
    if (getRenderer()) {
        getRenderer()->drawText(progressText, x + LABEL_WIDTH, currentY_, 12.0f, {1.0f, 1.0f, 1.0f, 1.0f});
    }
    
    // Status text
    if (!status.empty() && getRenderer()) {
        getRenderer()->drawText(status, x + LABEL_WIDTH + 50.0f, currentY_, 10.0f, {0.7f, 0.7f, 0.7f, 1.0f});
    }
    
    // TODO: Implement actual progress bar rendering
    
    return ELEMENT_HEIGHT;
}

float GeologicalDebugUI::renderStatusText(const std::string& label, const std::string& value) {
    float x = getPosition().x + 20.0f;
    
    if (!label.empty() && getRenderer()) {
        getRenderer()->drawText(label, x, currentY_, 12.0f, {0.8f, 0.8f, 0.8f, 1.0f});
        getRenderer()->drawText(value, x + LABEL_WIDTH, currentY_, 12.0f, {1.0f, 1.0f, 1.0f, 1.0f});
    } else if (getRenderer()) {
        getRenderer()->drawText(value, x, currentY_, 12.0f, {1.0f, 1.0f, 1.0f, 1.0f});
    }
    
    return ELEMENT_HEIGHT;
}

void GeologicalDebugUI::applyConfigurationChange() {
    // Set to custom preset when parameters are manually changed
    currentConfig_.preset = VoxelCastle::World::GeologicalPreset::CUSTOM;
    
    // Apply changes through callback
    if (configCallback_) {
        configCallback_(currentConfig_);
        UILogger::getInstance().info("GeologicalDebugUI", "Configuration changes applied");
    }
    
    // Update running simulation if active
    if (backgroundSim_ && backgroundSim_->GetStatus() == VoxelCastle::World::SimulationStatus::RUNNING) {
        backgroundSim_->UpdateConfiguration(currentConfig_);
        UILogger::getInstance().info("GeologicalDebugUI", "Live configuration update sent to simulation");
    }
}

void GeologicalDebugUI::updateParametersFromPreset() {
    // This would populate custom settings based on the selected preset
    // For now, keeping current custom settings when switching presets
}

void GeologicalDebugUI::loadPreset(VoxelCastle::World::GeologicalPreset preset) {
    currentConfig_.preset = preset;
    updateParametersFromPreset();
    applyConfigurationChange();
}

void GeologicalDebugUI::saveCurrentAsCustom() {
    currentConfig_.preset = VoxelCastle::World::GeologicalPreset::CUSTOM;
    UILogger::getInstance().info("GeologicalDebugUI", "Current settings saved as custom preset");
}

void GeologicalDebugUI::resetToDefaults() {
    currentConfig_ = VoxelCastle::World::GeologicalConfig{};
    currentConfig_.preset = VoxelCastle::World::GeologicalPreset::BALANCED;
    updateParametersFromPreset();
    applyConfigurationChange();
    UILogger::getInstance().info("GeologicalDebugUI", "Configuration reset to defaults");
}

std::string GeologicalDebugUI::formatDuration(float seconds) {
    if (seconds < 60.0f) {
        return std::to_string(static_cast<int>(seconds)) + "s";
    } else if (seconds < 3600.0f) {
        int minutes = static_cast<int>(seconds / 60.0f);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0f);
        int minutes = static_cast<int>(seconds / 60.0f) % 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

std::string GeologicalDebugUI::formatMemorySize(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + "B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + "KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + "MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + "GB";
    }
}

} // namespace UI
} // namespace VoxelEngine
