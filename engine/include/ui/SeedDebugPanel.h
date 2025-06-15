#pragma once
#include "ui/UIElement.h"
#include "world/world_seed.h"
#include <string>
#include <glm/glm.hpp>

namespace VoxelEngine {
namespace UI {

/**
 * Debug UI panel for visualizing and testing world seed functionality.
 * Provides real-time feedback on seed values and generation parameters.
 */
class SeedDebugPanel : public UIElement {
public:
    SeedDebugPanel();
    virtual ~SeedDebugPanel() = default;

    // UIElement interface
    void update(float deltaTime) override;
    void render() override;
    bool handleInput(float mouseX, float mouseY, bool clicked) override;

    // Configuration
    void setWorldSeed(const VoxelCastle::World::WorldSeed* worldSeed) { worldSeed_ = worldSeed; }
    void setPlayerPosition(const glm::vec3& position) { playerPosition_ = position; }
    void setCursorPosition(const glm::vec3& position) { cursorPosition_ = position; }
    
    // Display options
    void setShowSeedInfo(bool show) { showSeedInfo_ = show; }
    void setShowCoordinateSeeds(bool show) { showCoordinateSeeds_ = show; }
    void setShowFeatureBreakdown(bool show) { showFeatureBreakdown_ = show; }
    void setShowVisualizationOverlay(bool show) { showVisualizationOverlay_ = show; }
    void setShowSeedEntry(bool show) { showSeedEntry_ = show; }

    // Toggle visibility
    void toggleVisibility() { setVisible(!isVisible()); }
    
    // Seed testing functionality
    void setTestSeed(const std::string& seedString);
    void setTestSeed(uint64_t seed);
    void resetToWorldSeed();
    bool isTestingCustomSeed() const { return isTestingSeed_; }
    
    // Visualization overlay controls
    void toggleVisualizationOverlay() { showVisualizationOverlay_ = !showVisualizationOverlay_; }
    void setVisualizationFeature(VoxelCastle::World::FeatureType feature) { visualizationFeature_ = feature; }
    
    // Comparison mode
    void enableComparisonMode(bool enable) { comparisonMode_ = enable; }
    void setComparisonSeed(const std::string& seedString);
    
    // Development tools
    void exportSeedVisualization();
    void logSeedInformation();

private:
    const VoxelCastle::World::WorldSeed* worldSeed_{nullptr};
    glm::vec3 playerPosition_{0.0f};
    glm::vec3 cursorPosition_{0.0f};
    
    // Display flags
    bool showSeedInfo_{true};
    bool showCoordinateSeeds_{true};
    bool showFeatureBreakdown_{false};
    bool showVisualizationOverlay_{false};
    bool showSeedEntry_{false};
    
    // Testing functionality
    bool isTestingSeed_{false};
    VoxelCastle::World::WorldSeed testSeed_;
    std::string testSeedString_;
    
    // Visualization overlay
    VoxelCastle::World::FeatureType visualizationFeature_{VoxelCastle::World::FeatureType::TERRAIN};
    
    // Comparison mode
    bool comparisonMode_{false};
    VoxelCastle::World::WorldSeed comparisonSeed_;
    std::string comparisonSeedString_;
    
    // UI state
    float updateTimer_{0.0f};
    const float updateInterval_{0.1f}; // Update 10 times per second
    
    // Cached values for display
    std::string masterSeedText_;
    std::string playerSeedText_;
    std::string cursorSeedText_;
    std::string featureBreakdownText_;
    std::string testSeedText_;
    std::string comparisonText_;
    
    // Helper methods
    void updateSeedInfo();
    void updateCoordinateSeeds();
    void updateFeatureBreakdown();
    void updateTestSeedDisplay();
    void updateComparisonDisplay();
    void renderPanel();
    void renderSeedEntry();
    void renderVisualizationControls();
    void renderComparisonView();
    void renderText(const std::string& text, float x, float y, float scale = 1.0f);
    
    // Input handling
    void handleSeedEntryInput();
    bool handleKeyboardInput(int key, bool pressed);
    
    // Visualization helpers
    void renderVisualizationOverlay();
    float getSeedValueAtPosition(const glm::vec3& pos, VoxelCastle::World::FeatureType feature);
    glm::vec3 seedValueToColor(float value);
    
    // Format coordinate as chunk/region info
    std::string formatCoordinateInfo(const glm::vec3& pos);
    
    // Get debug info for a position
    std::string getSeedDebugInfo(const glm::vec3& pos, VoxelCastle::World::FeatureType feature = VoxelCastle::World::FeatureType::TERRAIN);
};

} // namespace UI
} // namespace VoxelEngine
