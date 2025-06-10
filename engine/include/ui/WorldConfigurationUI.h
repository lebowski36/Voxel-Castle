#pragma once

#include "elements/BaseMenu.h"
#include <functional>
#include <memory>
#include <chrono>

// Forward declarations
namespace VoxelEngine::UI {
    class UIButton;
    class UIRenderer;
    class WorldPreviewRenderer;
}

namespace VoxelCastle::World {
    class WorldSeed;
    class WorldParameters;
}

class WorldConfigurationUI : public VoxelEngine::UI::BaseMenu {
public:
    // World configuration parameters (matching original WorldGenerationUI)
    struct WorldConfig {
        std::string worldName = "New World";    // World name (user-definable)
        int worldSize = 1024;           // World size in regions
        int simulationDepth = 2;        // 1=Fast, 2=Normal, 3=Epic
        int climateType = 1;            // 0=Arctic, 1=Temperate, 2=Tropical, 3=Desert
        int geologicalActivity = 1;     // 0=Stable, 1=Moderate, 2=Highly Active
        int hydrologyLevel = 1;         // 0=Arid, 1=Normal, 2=Water-rich
        uint32_t customSeed = 0;        // 0=Random seed
        bool enableCivilizations = true;
    };

    // Configuration completion callback
    using OnConfigurationCompleteCallback = std::function<void(const WorldConfig&)>;
    using OnBackCallback = std::function<void()>;

    WorldConfigurationUI(VoxelEngine::UI::UIRenderer* renderer);
    virtual ~WorldConfigurationUI();

    /**
     * @brief Initialize the world configuration UI
     * @param screenWidth Screen width in pixels
     * @param screenHeight Screen height in pixels
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(int screenWidth, int screenHeight);

    // Override base menu methods
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;
    bool handleKeyboardInput(int key, bool pressed);  // Add keyboard input handling

    // Callbacks
    void setOnConfigurationCompleteCallback(OnConfigurationCompleteCallback callback) { onConfigurationComplete_ = callback; }
    void setOnBackCallback(OnBackCallback callback) { onBack_ = callback; }

    // Configuration access
    const WorldConfig& getConfiguration() const { return config_; }
    void setConfiguration(const WorldConfig& config) { config_ = config; createUIElements(); }

private:
    void createUIElements();
    void createParameterControls();
    void createPreviewSection();
    void createActionButtons();
    
    // Layout calculation helpers
    float calculateMaxParameterLabelWidth();
    float calculateMaxParameterValueWidth();
    float calculateOptimalRowSpacing();
    
    // Event handlers
    void onStartGenerationClicked();
    void onBackClicked();
    void onParameterChanged();
    void onWorldNameChanged();
    
    // Preview management
    void updateWorldPreview();
    void initializeWorldGenerationObjects();
    void renderWorldPreview();
    
    // Preview area coordinates (set by createPreviewSection)
    float previewX_, previewY_, previewWidth_, previewHeight_;
    
    // World name validation
    void validateWorldName();
    bool isWorldNameValid() const;

    // Configuration state
    WorldConfig config_;
    
    // World preview rendering
    std::unique_ptr<VoxelEngine::UI::WorldPreviewRenderer> previewRenderer_;
    std::shared_ptr<VoxelCastle::World::WorldSeed> currentWorldSeed_;
    std::shared_ptr<VoxelCastle::World::WorldParameters> currentWorldParameters_;
    bool previewNeedsUpdate_;
    
    // World name input state
    char worldNameBuffer_[64];          // Buffer for ImGui text input
    bool worldNameExists_ = false;      // True if world name already exists
    std::string worldNameError_;        // Error message for world name validation
    bool isEditingWorldName_ = false;   // True when user is editing world name
    
    // Callbacks
    OnConfigurationCompleteCallback onConfigurationComplete_;
    OnBackCallback onBack_;
    
    // Layout state
    float currentY_;
    
    // Constants for layout
    static constexpr float PANEL_MARGIN = 15.0f;
    static constexpr float ELEMENT_SPACING = 12.0f;
    static constexpr float BUTTON_HEIGHT = 35.0f;
    static constexpr float TEXT_HEIGHT = 25.0f;
    static constexpr float VERTICAL_SPACING = 40.0f;
    static constexpr float TITLE_HEIGHT = 45.0f;
};
