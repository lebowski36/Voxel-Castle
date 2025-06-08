#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <deque>

#include "world/seed_world_generator.h"
#include "world/world_parameters.h"
#include "world/world_seed.h"
#include "ui/elements/BaseMenu.h"

namespace VoxelCastle::UI {

/**
 * @brief World Generation UI - Dwarf Fortress-inspired world creation visualization
 * 
 * This UI provides real-time visualization of the world generation process,
 * showing tectonic simulation, climate formation, biome assignment, and more.
 * Based on the specifications in WORLD_GENERATION_UI_SPECIFICATION.md
 */
class WorldGenerationUI : public VoxelEngine::UI::BaseMenu {

public:
    // World generation phases
    enum class GenerationPhase {
        CONFIGURATION,  // Parameter selection
        TECTONICS,     // Mountain/terrain formation
        EROSION,       // Valley carving and sediment
        HYDROLOGY,     // Rivers and lakes
        CLIMATE,       // Temperature and precipitation
        BIOMES,        // Biome assignment
        CIVILIZATION,  // Historical civilizations (optional)
        COMPLETE       // Generation finished
    };

    // Visualization modes
    enum class VisualizationMode {
        ELEVATION,
        TEMPERATURE,
        PRECIPITATION,
        BIOMES,
        HYDROLOGY,
        GEOLOGY
    };

    // World configuration parameters
    struct WorldConfig {
        int worldSize = 1024;           // World size in regions
        int simulationDepth = 2;        // 1=Fast, 2=Normal, 3=Epic
        int climateType = 1;            // 0=Arctic, 1=Temperate, 2=Tropical, 3=Desert
        int geologicalActivity = 1;     // 0=Stable, 1=Moderate, 2=Highly Active
        int hydrologyLevel = 1;         // 0=Arid, 1=Normal, 2=Water-rich
        uint32_t customSeed = 0;        // 0=Random seed
        bool enableCivilizations = true;
    };

    // Generation log entry
    struct LogEntry {
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
        int simulationYear;
    };

    // World statistics
    struct WorldStats {
        int mountainRanges = 0;
        int majorRivers = 0;
        int biomesIdentified = 0;
        int simulationYears = 0;
        float highestPeak = 0.0f;
        float deepestValley = 0.0f;
        float largestLakeSize = 0.0f;
        float longestRiverLength = 0.0f;
        std::string highestPeakName = "";
        std::string deepestValleyName = "";
        std::string largestLakeName = "";
        std::string longestRiverName = "";
    };

public:
    WorldGenerationUI(VoxelEngine::UI::UIRenderer* renderer);
    ~WorldGenerationUI();

    /**
     * @brief Initialize the world generation UI
     * @param menuSystem Pointer to the menu system
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(VoxelEngine::UI::MenuSystem* menuSystem);

    /**
     * @brief Update the world generation UI (called each frame)
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime) override;

    /**
     * @brief Render the world generation UI
     */
    void render() override;

    /**
     * @brief Check if generation is complete and ready to start game
     * @return true if generation is complete
     */
    bool IsGenerationComplete() const;

    /**
     * @brief Start world generation process
     */
    void StartGeneration();

    /**
     * @brief Cancel ongoing generation
     */
    void CancelGeneration();

    /**
     * @brief Check if generation is currently running
     */
    bool IsGenerating() const { return isGenerating_; }

    /**
     * @brief Get the generated world (after completion)
     */
    std::shared_ptr<VoxelCastle::World::SeedWorldGenerator> GetGeneratedWorld() const { return worldGenerator_; }

    /**
     * @brief Set callback for when generation completes
     */
    void SetCompletionCallback(std::function<void(std::shared_ptr<VoxelCastle::World::SeedWorldGenerator>)> callback);

    /**
     * @brief Handle input events for the world generation UI
     * @param mouseX Mouse X coordinate
     * @param mouseY Mouse Y coordinate
     * @param clicked Whether mouse was clicked
     * @return true if input was handled
     */
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

private:
    // UI State
    GenerationPhase currentPhase_ = GenerationPhase::CONFIGURATION;
    VisualizationMode visualizationMode_ = VisualizationMode::ELEVATION;
    bool isPaused_ = false;
    float generationSpeed_ = 1.0f;
    
    // Generation state
    bool isGenerating_ = false;
    bool generationComplete_ = false;
    float generationProgress_ = 0.0f;
    float phaseProgress_ = 0.0f;
    std::chrono::steady_clock::time_point generationStartTime_;
    std::chrono::steady_clock::time_point phaseStartTime_;
    
    // World configuration
    WorldConfig config_;
    WorldStats stats_;
    std::deque<LogEntry> generationLog_;
    
    // World generation components
    std::shared_ptr<VoxelCastle::World::WorldSeed> worldSeed_;
    std::shared_ptr<VoxelCastle::World::WorldParameters> worldParameters_;
    std::shared_ptr<VoxelCastle::World::SeedWorldGenerator> worldGenerator_;
    
    // UI elements
    VoxelEngine::UI::MenuSystem* menuSystem_ = nullptr;
    
    // Completion callback
    std::function<void(std::shared_ptr<VoxelCastle::World::SeedWorldGenerator>)> completionCallback_;
    
    // Generation phases and simulation
    void UpdateGeneration(float deltaTime);
    void AdvancePhase();
    void CompleteGeneration();
    void SimulatePhase(GenerationPhase phase, float deltaTime);
    void AddLogEntry(const std::string& message, int year = -1);
    
    // UI creation methods
    void createUIElements();
    void createConfigurationUI();
    void createGenerationUI();
    void createWorldSummaryUI();
    
    // UI update methods
    void updateProgressPanel();
    void updateVisualization();
    void updateGenerationLog();
    void updateWorldStats();
    
    // UI event handlers
    void OnStartClicked();
    void OnCancelClicked();
    void OnBackToMainMenuClicked();
    void OnBeginGameClicked();
    void OnPauseResumeClicked();
    void OnVisualizationModeChanged(VisualizationMode mode);
    void OnSpeedChanged(float speed);
    void OnWorldSizeChanged(int size);
    void OnClimateTypeChanged(int type);
    
    // Helper methods
    void removeAllChildren();
    std::string GetPhaseDisplayName(GenerationPhase phase);
    std::string FormatTimeRemaining(float secondsRemaining);
    float GetPhaseExpectedDuration(GenerationPhase phase);
    
    // Constants for layout
    static constexpr float PANEL_MARGIN = 10.0f;
    static constexpr float ELEMENT_SPACING = 8.0f;
    static constexpr float BUTTON_HEIGHT = 30.0f;
    static constexpr float PROGRESS_BAR_HEIGHT = 20.0f;
    static constexpr int MAX_LOG_ENTRIES = 50;
    
    // UI positioning
    float currentY_ = 0.0f;
};

} // namespace VoxelCastle::UI
