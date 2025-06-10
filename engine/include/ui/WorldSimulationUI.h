#pragma once

#include "elements/BaseMenu.h"
#include <functional>
#include <memory>
#include <chrono>
#include <deque>
#include <string>
#include <thread>

namespace VoxelEngine::UI {
    class UIButton;
    class UIRenderer;
    class WorldMapRenderer;
}

namespace VoxelCastle::World {
    class SeedWorldGenerator;
    class WorldSeed;
    class WorldParameters;
    class WorldPersistenceManager;
}

class WorldSimulationUI : public VoxelEngine::UI::BaseMenu {
public:
    // World generation phases (from original WorldGenerationUI)
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

    // Visualization modes (from original WorldGenerationUI)
    enum class VisualizationMode {
        ELEVATION,
        TEMPERATURE,
        PRECIPITATION,
        BIOMES,
        HYDROLOGY,
        GEOLOGY
    };

    // World configuration parameters (matching original)
    struct WorldConfig {
        std::string worldName = "New World";  // World name
        int worldSize = 1024;           
        int simulationDepth = 2;        
        int climateType = 1;            
        int geologicalActivity = 1;     
        int hydrologyLevel = 1;         
        uint32_t customSeed = 0;        
        bool enableCivilizations = true;
    };

    // Generation log entry (from original)
    struct LogEntry {
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
        int simulationYear;
    };

    // World statistics (from original)
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

    // Simulation completion callback
    using OnSimulationCompleteCallback = std::function<void(const WorldStats&)>;
    using OnBackCallback = std::function<void()>;

    WorldSimulationUI(VoxelEngine::UI::UIRenderer* renderer);
    virtual ~WorldSimulationUI();

    /**
     * @brief Initialize the world simulation UI
     * @param screenWidth Screen width in pixels
     * @param screenHeight Screen height in pixels
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(int screenWidth, int screenHeight);

    // Override base menu methods
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;
    void update(float deltaTime) override;

    /**
     * @brief Handle extended input events including mouse wheel for map zoom
     * @param mouseX Mouse X coordinate
     * @param mouseY Mouse Y coordinate
     * @param clicked Whether mouse button is pressed
     * @param wheelDelta Mouse wheel delta for zooming
     * @return true if input was handled
     */
    bool handleExtendedInput(float mouseX, float mouseY, bool clicked, float wheelDelta);

    /**
     * @brief Check if mouse is over the world map area
     * @param mouseX Mouse X coordinate
     * @param mouseY Mouse Y coordinate
     * @return true if mouse is over the map
     */
    bool isMouseOverMap(float mouseX, float mouseY) const;

    // Simulation control
    void startSimulation(const WorldConfig& config, const std::string& worldName = "Generated World");
    void pauseSimulation();
    void resumeSimulation();
    void stopSimulation();
    bool isSimulationRunning() const { return isRunning_; }
    bool isSimulationComplete() const { return currentPhase_ == GenerationPhase::COMPLETE; }

    // World persistence
    void setWorldName(const std::string& worldName) { worldName_ = worldName; }
    const std::string& getWorldName() const { return worldName_; }

    // Callbacks
    void setOnSimulationCompleteCallback(OnSimulationCompleteCallback callback) { onSimulationComplete_ = callback; }
    void setOnBackCallback(OnBackCallback callback) { onBack_ = callback; }

    // Progress access
    float getCurrentProgress() const { return currentProgress_; }
    GenerationPhase getCurrentPhase() const { return currentPhase_; }
    const WorldStats& getWorldStats() const { return stats_; }

    /**
     * @brief Regenerate the world map with current settings
     */
    void regenerateWorldMap();

private:
    void createUIElements();
    void createVisualizationControls();
    void createWorldPreview();
    void createProgressPanels();
    void createGenerationLog();
    void createActionButtons();
    void createWorldSummaryUI();
    
    // World map rendering
    void renderWorldMap();
    void updateWorldMapVisualization();
    
    // Layout calculation helpers
    float calculateMaxVisualizationButtonWidth();
    float calculateOptimalRowSpacing();
    
    // Simulation management
    void updateSimulation(float deltaTime);
    void startGenerationThread();
    void generationWorker();
    std::string getPhaseDisplayName(GenerationPhase phase) const;
    void updateFinalStatistics();
    void advancePhase();
    void simulatePhase(GenerationPhase phase, float deltaTime);
    void completeSimulation();
    void addLogEntry(const std::string& message, int year = 0);
    
    // Event handlers
    void onVisualizationModeChanged(VisualizationMode mode);
    void onPauseResumeClicked();
    void onStopClicked();
    void onBackClicked();
    void onBeginGameClicked();
    
    // Helper methods
    std::string getPhaseDisplayName(GenerationPhase phase);
    std::string getVisualizationModeDisplayName(VisualizationMode mode);
    float getPhaseExpectedDuration(GenerationPhase phase);
    float calculateTimeRemaining();

    // Configuration and state
    WorldConfig config_;
    WorldStats stats_;
    std::deque<LogEntry> generationLog_;
    std::string worldName_ = "Generated World"; // Default world name
    
    // Current simulation state
    GenerationPhase currentPhase_;
    float currentProgress_;
    float phaseProgress_;
    bool isPaused_;
    bool isRunning_;
    std::chrono::steady_clock::time_point simulationStartTime_;
    std::chrono::steady_clock::time_point phaseStartTime_;
    
    // UI state
    VisualizationMode visualizationMode_;
    float generationSpeed_ = 1.0f;
    
    // World generation
    std::shared_ptr<VoxelCastle::World::SeedWorldGenerator> worldGenerator_;
    std::shared_ptr<VoxelCastle::World::WorldSeed> worldSeed_;
    std::shared_ptr<VoxelCastle::World::WorldParameters> worldParameters_;
    std::shared_ptr<VoxelCastle::World::WorldPersistenceManager> worldPersistence_;
    std::thread generationThread_;
    
    // World map visualization
    std::unique_ptr<VoxelEngine::UI::WorldMapRenderer> worldMapRenderer_;
    
    // World map area coordinates (set by createWorldPreview)
    float worldMapX_, worldMapY_, worldMapWidth_, worldMapHeight_;
    
    // Callbacks
    OnSimulationCompleteCallback onSimulationComplete_;
    OnBackCallback onBack_;
    
    // Layout state
    float currentY_;
    
    // Layout constants (matching original)
    static constexpr float PANEL_MARGIN = 20.0f;
    static constexpr float ELEMENT_SPACING = 10.0f;
    static constexpr float TITLE_HEIGHT = 40.0f;
    static constexpr float BUTTON_HEIGHT = 30.0f;
    static constexpr float TEXT_HEIGHT = 25.0f;
    static constexpr float VERTICAL_SPACING = 35.0f;
};
