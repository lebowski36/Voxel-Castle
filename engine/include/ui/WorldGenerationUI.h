#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

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

private:
    // Generation state
    bool isGenerating_ = false;
    bool generationComplete_ = false;
    float generationProgress_ = 0.0f;
    std::chrono::steady_clock::time_point generationStartTime_;
    
    // World generation components
    std::shared_ptr<VoxelCastle::World::WorldSeed> worldSeed_;
    std::shared_ptr<VoxelCastle::World::WorldParameters> worldParameters_;
    std::shared_ptr<VoxelCastle::World::SeedWorldGenerator> worldGenerator_;
    
    // UI elements
    VoxelEngine::UI::MenuSystem* menuSystem_ = nullptr;
    
    // Completion callback
    std::function<void(std::shared_ptr<VoxelCastle::World::SeedWorldGenerator>)> completionCallback_;
    
    // Generation phases (simplified for now)
    void UpdateGeneration(float deltaTime);
    void CompleteGeneration();
    void createUIElements();
    
    // UI event handlers
    void OnStartClicked();
    void OnCancelClicked();
};

} // namespace VoxelCastle::UI
