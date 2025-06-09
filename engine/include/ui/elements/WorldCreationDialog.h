#pragma once

#include "ui/elements/BaseMenu.h"
#include "ui/elements/UIButton.h"
#include "world/world_seed.h"
#include <memory>
#include <string>
#include <functional>

namespace VoxelEngine {
namespace UI {

class MenuSystem;

/**
 * @brief World creation dialog for the new world generation system
 * 
 * Provides a simple interface for creating new worlds with:
 * - Seed entry (initially via random generation)
 * - World size selection
 * - Basic parameter controls
 * - Preview information
 */
class WorldCreationDialog : public BaseMenu {
public:
    /**
     * @brief Available world sizes with their characteristics
     */
    enum class WorldSize {
        STARTER,      // 10km² - Quick generation, small worlds
        REGIONAL,     // 1,000km² - Standard play area
        CONTINENTAL,  // 100,000km² - Large exploration
        MASSIVE       // 25,000,000km² - Currently disabled
    };

    WorldCreationDialog(UIRenderer* renderer);
    ~WorldCreationDialog() override = default;

    /**
     * @brief Initialize the world creation dialog
     * @param menuSystem Pointer to the menu system
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(MenuSystem* menuSystem);

    /**
     * @brief Set callback for when world creation is requested
     * @param callback Function to call with the configured world parameters
     */
    void setOnWorldCreate(std::function<void(const VoxelCastle::World::WorldSeed&, WorldSize)> callback) {
        onWorldCreate_ = callback;
    }

    /**
     * @brief Set callback for when dialog is cancelled
     * @param callback Function to call when cancelled
     */
    void setOnCancel(std::function<void()> callback) {
        onCancel_ = callback;
    }

    /**
     * @brief Update the dialog, including any preview generation
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime) override;

private:
    // Current configuration
    VoxelCastle::World::WorldSeed currentSeed_;
    WorldSize currentWorldSize_;
    std::string seedDisplayText_;
    bool seedInitialized_;  // Track if seed has been initialized yet
    
    // UI Elements
    std::shared_ptr<UIButton> randomSeedButton_;
    std::shared_ptr<UIButton> starterSizeButton_;
    std::shared_ptr<UIButton> regionalSizeButton_;
    std::shared_ptr<UIButton> continentalSizeButton_;
    std::shared_ptr<UIButton> massiveSizeButton_;  // Disabled for now
    std::shared_ptr<UIButton> createWorldButton_;
    std::shared_ptr<UIButton> cancelButton_;
    
    // Callbacks
    std::function<void(const VoxelCastle::World::WorldSeed&, WorldSize)> onWorldCreate_;
    std::function<void()> onCancel_;
    
    // Event handlers
    void onRandomSeedClicked();
    void onSizeButtonClicked(WorldSize size);
    void onCreateWorldClicked();
    void onCancelClicked();
    
    // Helper methods
    void updateSeedDisplay();
    void updateSizeButtons();
    std::string getWorldSizeDescription(WorldSize size) const;
    std::string getGenerationTimeEstimate(WorldSize size) const;
    bool isWorldSizeAvailable(WorldSize size) const;
    
    // UI layout helpers
    void addSeedSection();
    void addWorldSizeSection();
    void addActionButtons();
};

} // namespace UI
} // namespace VoxelEngine
