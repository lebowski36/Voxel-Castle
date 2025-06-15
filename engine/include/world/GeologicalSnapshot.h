#pragma once

#include "world/ContinuousField.h"
#include "world/geological_data.h"  // For RockType
#include <string>
#include <vector>
#include <memory>

namespace VoxelCastle {
namespace World {

/**
 * @brief Represents a snapshot of geological simulation state at a specific time
 * 
 * Contains all the data needed to visualize the geological state without
 * requiring real-time computation. Used to decouple UI rendering from
 * heavy geological simulation processing.
 */
struct GeologicalSnapshot {
    // Core geological data
    std::unique_ptr<ContinuousField<float>> elevationData;
    std::unique_ptr<ContinuousField<RockType>> rockTypeData;
    std::unique_ptr<ContinuousField<float>> mantleStressData;
    
    // ===== Step 4.1.2: Water System Data for Visualization =====
    std::unique_ptr<ContinuousField<float>> surfaceWaterDepth;      // Surface water (rivers/lakes)
    std::unique_ptr<ContinuousField<float>> precipitationField;     // Precipitation intensity
    std::unique_ptr<ContinuousField<float>> groundwaterTable;       // Groundwater table depth
    std::unique_ptr<ContinuousField<float>> waterFlowField;         // Water flow patterns
    std::unique_ptr<ContinuousField<float>> sedimentLoadField;      // Sediment transport
    
    // Simulation metadata
    float simulationTime;           // Geological time in millions of years
    std::string phaseDescription;   // "Interleaved Simulation", "Continental Drift", etc.
    int stepNumber;                 // Which step in the simulation sequence
    float completionPercentage;     // 0.0 to 1.0
    
    // Performance tracking
    float generationTimeMs;         // How long this snapshot took to generate
    
    GeologicalSnapshot(int width, int height, float spacing, 
                      float simTime, const std::string& phase, 
                      int step, float completion);
    
    ~GeologicalSnapshot() = default;
    
    // Move-only semantics (snapshots contain large data)
    GeologicalSnapshot(const GeologicalSnapshot&) = delete;
    GeologicalSnapshot& operator=(const GeologicalSnapshot&) = delete;
    GeologicalSnapshot(GeologicalSnapshot&&) = default;
    GeologicalSnapshot& operator=(GeologicalSnapshot&&) = default;
    
    /**
     * @brief Get elevation at world coordinates (safe sampling)
     */
    float GetElevationAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get rock type at world coordinates (safe sampling)
     */
    RockType GetRockTypeAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get mantle stress at world coordinates (safe sampling)
     */
    float GetMantleStressAt(float worldX, float worldZ) const;
    
    // ===== Step 4.1.2: Water System Access Methods =====
    /**
     * @brief Get surface water depth at world coordinates (safe sampling)
     */
    float GetSurfaceWaterAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get precipitation intensity at world coordinates (safe sampling)
     */
    float GetPrecipitationAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get groundwater table depth at world coordinates (safe sampling)
     */
    float GetGroundwaterAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get water flow intensity at world coordinates (safe sampling)
     */
    float GetWaterFlowAt(float worldX, float worldZ) const;
    
    /**
     * @brief Get sediment load at world coordinates (safe sampling)
     */
    float GetSedimentLoadAt(float worldX, float worldZ) const;
};

/**
 * @brief Manages geological simulation snapshots for UI visualization
 * 
 * Provides a clean interface between the geological simulation and the UI,
 * allowing the simulation to run independently while providing discrete
 * snapshots for visualization.
 */
class GeologicalSnapshotManager {
private:
    std::vector<std::unique_ptr<GeologicalSnapshot>> snapshots_;
    size_t currentSnapshotIndex_;
    bool isGenerating_;
    
    // Configuration
    int maxSnapshots_;              // Maximum number of snapshots to keep
    float worldSizeKm_;
    int simulationWidth_;
    int simulationHeight_;
    float spacing_;
    
public:
    GeologicalSnapshotManager(float worldSizeKm, int simulationWidth, int simulationHeight, float spacing);
    ~GeologicalSnapshotManager() = default;
    
    /**
     * @brief Add a new snapshot from current geological simulation state
     */
    void AddSnapshot(const ContinuousField<float>& elevationField,
                    const ContinuousField<RockType>& rockTypeField, 
                    const ContinuousField<float>& mantleStressField,
                    // ===== Step 4.1.2: Water System Fields =====
                    const ContinuousField<float>& surfaceWaterField,
                    const ContinuousField<float>& precipitationField,
                    const ContinuousField<float>& groundwaterField,
                    const ContinuousField<float>& waterFlowField,
                    const ContinuousField<float>& sedimentLoadField,
                    float simulationTime,
                    const std::string& phaseDescription,
                    int stepNumber,
                    float completionPercentage);
    
    /**
     * @brief Get the current snapshot for UI display
     */
    const GeologicalSnapshot* GetCurrentSnapshot() const;
    
    /**
     * @brief Get specific snapshot by index
     */
    const GeologicalSnapshot* GetSnapshot(size_t index) const;
    
    /**
     * @brief Navigate to next snapshot
     */
    bool NextSnapshot();
    
    /**
     * @brief Navigate to previous snapshot
     */
    bool PreviousSnapshot();
    
    /**
     * @brief Jump to specific snapshot
     */
    bool SetCurrentSnapshot(size_t index);
    
    /**
     * @brief Get total number of available snapshots
     */
    size_t GetSnapshotCount() const { return snapshots_.size(); }
    
    /**
     * @brief Get current snapshot index
     */
    size_t GetCurrentSnapshotIndex() const { return currentSnapshotIndex_; }
    
    /**
     * @brief Check if we're currently generating a new snapshot
     */
    bool IsGenerating() const { return isGenerating_; }
    
    /**
     * @brief Mark that snapshot generation is starting
     */
    void SetGenerating(bool generating) { isGenerating_ = generating; }
    
    /**
     * @brief Clear all snapshots (for new world generation)
     */
    void Clear();
    
    /**
     * @brief Get snapshot generation progress (for UI)
     */
    float GetGenerationProgress() const;
    
    /**
     * @brief Get current phase description
     */
    std::string GetCurrentPhaseDescription() const;
};

} // namespace World
} // namespace VoxelCastle
