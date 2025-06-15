#pragma once
#include "ui/UIElement.h"
#include "world/GeologicalTypes.h"
#include "world/GeologicalSnapshot.h"
#include "world/BackgroundSimulationEngine.h"
#include <memory>
#include <functional>
#include <string>

namespace VoxelEngine {
namespace UI {

/**
 * @brief Debug UI panel for real-time geological simulation control and monitoring
 * 
 * Provides:
 * - Real-time parameter adjustment with immediate effect
 * - Simulation state monitoring and visualization
 * - Performance metrics and progress tracking
 * - Phase-specific controls and debugging
 * - Background simulation thread management
 */
class GeologicalDebugUI : public UIElement {
public:
    GeologicalDebugUI();
    virtual ~GeologicalDebugUI() = default;

    // UIElement interface
    void update(float deltaTime) override;
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

    // Simulation control
    void setBackgroundSimulation(std::shared_ptr<VoxelCastle::World::BackgroundSimulationEngine> bgSim);
    void setConfigurationCallback(std::function<void(const VoxelCastle::World::GeologicalConfig&)> callback);
    void setCurrentSnapshot(const VoxelCastle::World::GeologicalSnapshot& snapshot);
    
    // Display options
    void setShowSimulationControls(bool show) { showSimulationControls_ = show; }
    void setShowParameterSliders(bool show) { showParameterSliders_ = show; }
    void setShowProgressMonitoring(bool show) { showProgressMonitoring_ = show; }
    void setShowPerformanceMetrics(bool show) { showPerformanceMetrics_ = show; }
    void setShowPhaseControls(bool show) { showPhaseControls_ = show; }
    void setShowTerrainStats(bool show) { showTerrainStats_ = show; }
    void setShowSnapshotInfo(bool show) { showSnapshotInfo_ = show; }

    // Toggle visibility
    void toggleVisibility() { setVisible(!isVisible()); }
    
    // Configuration presets
    void loadPreset(VoxelCastle::World::GeologicalPreset preset);
    void saveCurrentAsCustom();
    void resetToDefaults();

private:
    // Current configuration state
    VoxelCastle::World::GeologicalConfig currentConfig_;
    VoxelCastle::World::GeologicalSnapshot latestSnapshot_;
    std::shared_ptr<VoxelCastle::World::BackgroundSimulationEngine> backgroundSim_;
    std::function<void(const VoxelCastle::World::GeologicalConfig&)> configCallback_;
    
    // UI state
    bool showSimulationControls_ = true;
    bool showParameterSliders_ = true;
    bool showProgressMonitoring_ = true;
    bool showPerformanceMetrics_ = false;
    bool showPhaseControls_ = false;
    bool showTerrainStats_ = true;
    bool showSnapshotInfo_ = false;
    
    // UI layout constants
    static constexpr float SECTION_SPACING = 15.0f;
    static constexpr float ELEMENT_HEIGHT = 25.0f;
    static constexpr float SLIDER_WIDTH = 200.0f;
    static constexpr float LABEL_WIDTH = 150.0f;
    static constexpr float BUTTON_WIDTH = 80.0f;
    
    // Current UI position tracking
    float currentY_ = 10.0f;
    
    // Render sections
    void renderHeader();
    void renderSimulationControls();
    void renderParameterSliders();
    void renderProgressMonitoring();
    void renderPerformanceMetrics();
    void renderPhaseControls();
    void renderTerrainStats();
    void renderSnapshotInfo();
    
    // UI helpers
    void renderSectionHeader(const std::string& title, bool& toggleState);
    float renderFloatSlider(const std::string& label, float& value, float min, float max, const std::string& format = "%.2f");
    float renderIntSlider(const std::string& label, int& value, int min, int max);
    float renderBoolToggle(const std::string& label, bool& value);
    float renderButton(const std::string& label, std::function<void()> callback, float width = BUTTON_WIDTH);
    float renderProgressBar(const std::string& label, float progress, const std::string& status = "");
    float renderStatusText(const std::string& label, const std::string& value);
    
    // Configuration helpers
    void applyConfigurationChange();
    void updateParametersFromPreset();
    std::string formatDuration(float seconds);
    std::string formatMemorySize(size_t bytes);
    
    // Performance tracking
    float lastUpdateTime_ = 0.0f;
    float updateInterval_ = 0.1f; // Update UI 10 times per second
    
    // Cached strings to avoid frequent allocations
    std::string cachedProgressText_;
    std::string cachedPerformanceText_;
    std::string cachedStatsText_;
    
    // UI interaction state
    bool isDragging_ = false;
    float dragStartX_ = 0.0f;
    float dragStartY_ = 0.0f;
};

} // namespace UI
} // namespace VoxelEngine
