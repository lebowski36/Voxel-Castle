#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <functional>
#include <chrono>
#include <memory>

namespace VoxelCastle {
namespace World {

// Forward declarations
class GeologicalSimulator;
class GeologicalSnapshot;

/**
 * @brief Background simulation engine for responsive UI during geological simulation
 * 
 * This class handles the threading architecture to keep the UI responsive while
 * geological simulation runs in the background. It implements time-sliced execution
 * to prevent any single simulation step from blocking the UI thread.
 */
class BackgroundSimulationEngine {
public:
    /**
     * @brief Constructor
     * @param simulator Pointer to the geological simulator to run in background
     */
    explicit BackgroundSimulationEngine(GeologicalSimulator* simulator);
    
    /**
     * @brief Destructor - ensures clean thread shutdown
     */
    ~BackgroundSimulationEngine();
    
    /**
     * @brief Start background simulation
     * @return True if started successfully, false if already running
     */
    bool StartBackgroundSimulation();
    
    /**
     * @brief Stop background simulation
     * Safely stops the background thread and waits for completion
     */
    void StopBackgroundSimulation();
    
    /**
     * @brief Pause/Resume background simulation
     * @param paused True to pause, false to resume
     */
    void SetPaused(bool paused);
    
    /**
     * @brief Check if simulation is currently running
     */
    bool IsRunning() const { return simulationRunning_.load(); }
    
    /**
     * @brief Check if simulation is currently paused
     */
    bool IsPaused() const { return simulationPaused_.load(); }
    
    /**
     * @brief Get current simulation progress (0.0 - 1.0)
     */
    float GetProgress() const { return backgroundProgress_.load(); }
    
    /**
     * @brief Check if snapshots are available for UI consumption
     */
    bool HasPendingSnapshots() const;
    
    /**
     * @brief Get the next snapshot for UI display
     * @return Shared pointer to snapshot, or nullptr if none available
     */
    std::shared_ptr<GeologicalSnapshot> GetNextSnapshot();
    
    /**
     * @brief Set maximum time per simulation step (for UI responsiveness)
     * @param maxTimeMs Maximum time in milliseconds (default: 16ms for 60 FPS)
     */
    void SetMaxStepTime(float maxTimeMs) { maxStepTimeMs_ = maxTimeMs; }

private:
    // Thread management
    std::thread simulationThread_;
    std::atomic<bool> simulationRunning_{false};
    std::atomic<bool> simulationPaused_{false};
    std::atomic<bool> shutdownRequested_{false};
    
    // Progress tracking
    std::atomic<float> backgroundProgress_{0.0f};
    
    // Snapshot management
    mutable std::mutex snapshotMutex_;
    std::queue<std::shared_ptr<GeologicalSnapshot>> pendingSnapshots_;
    static constexpr size_t MAX_PENDING_SNAPSHOTS = 10;
    
    // Time-sliced execution
    float maxStepTimeMs_{16.0f};  // 60 FPS
    std::chrono::steady_clock::time_point lastSnapshotTime_;
    static constexpr float SNAPSHOT_INTERVAL_SECONDS = 0.5f;  // Create snapshot every 500ms
    
    // Reference to simulator
    GeologicalSimulator* simulator_;
    
    // Background thread methods
    void RunBackgroundSimulation();
    void ExecuteTimeSlicedStep();
    bool ShouldYieldToUI() const;
    void CreateAndQueueSnapshot();
    
    // Thread-safe snapshot management
    void AddSnapshot(std::shared_ptr<GeologicalSnapshot> snapshot);
    void CleanupOldSnapshots();
};

} // namespace World
} // namespace VoxelCastle
