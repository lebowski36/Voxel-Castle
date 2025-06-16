#include "world/BackgroundSimulationEngine.h"
#include "world/GeologicalSimulator.h"
#include "world/GeologicalSnapshot.h"
#include <iostream>

namespace VoxelCastle {
namespace World {

BackgroundSimulationEngine::BackgroundSimulationEngine(GeologicalSimulator* simulator)
    : simulator_(simulator), lastSnapshotTime_(std::chrono::steady_clock::now()) {
    
    if (!simulator_) {
        throw std::invalid_argument("BackgroundSimulationEngine requires valid GeologicalSimulator pointer");
    }
    
    std::cout << "[BackgroundSimulationEngine] Initialized with max step time: " 
              << maxStepTimeMs_ << "ms" << std::endl;
}

BackgroundSimulationEngine::~BackgroundSimulationEngine() {
    StopBackgroundSimulation();
}

bool BackgroundSimulationEngine::StartBackgroundSimulation() {
    if (simulationRunning_.load()) {
        std::cout << "[BackgroundSimulationEngine] Already running" << std::endl;
        return false;
    }
    
    // Reset state
    simulationRunning_.store(true);
    simulationPaused_.store(false);
    shutdownRequested_.store(false);
    backgroundProgress_.store(0.0f);
    
    // Clear pending snapshots
    {
        std::lock_guard<std::mutex> lock(snapshotMutex_);
        while (!pendingSnapshots_.empty()) {
            pendingSnapshots_.pop();
        }
    }
    
    // Start background thread
    simulationThread_ = std::thread(&BackgroundSimulationEngine::RunBackgroundSimulation, this);
    
    std::cout << "[BackgroundSimulationEngine] Background simulation started" << std::endl;
    return true;
}

void BackgroundSimulationEngine::StopBackgroundSimulation() {
    if (!simulationRunning_.load()) {
        return; // Already stopped
    }
    
    std::cout << "[BackgroundSimulationEngine] Stopping background simulation..." << std::endl;
    
    // Signal shutdown
    shutdownRequested_.store(true);
    simulationRunning_.store(false);
    
    // Wait for thread to finish
    if (simulationThread_.joinable()) {
        simulationThread_.join();
    }
    
    // Clear pending snapshots
    {
        std::lock_guard<std::mutex> lock(snapshotMutex_);
        while (!pendingSnapshots_.empty()) {
            pendingSnapshots_.pop();
        }
    }
    
    std::cout << "[BackgroundSimulationEngine] Background simulation stopped" << std::endl;
}

void BackgroundSimulationEngine::SetPaused(bool paused) {
    bool wasPaused = simulationPaused_.exchange(paused);
    
    if (wasPaused != paused) {
        std::cout << "[BackgroundSimulationEngine] Simulation " 
                  << (paused ? "paused" : "resumed") << std::endl;
    }
}

bool BackgroundSimulationEngine::HasPendingSnapshots() const {
    std::lock_guard<std::mutex> lock(snapshotMutex_);
    return !pendingSnapshots_.empty();
}

std::shared_ptr<GeologicalSnapshot> BackgroundSimulationEngine::GetNextSnapshot() {
    std::lock_guard<std::mutex> lock(snapshotMutex_);
    
    if (pendingSnapshots_.empty()) {
        return nullptr;
    }
    
    auto snapshot = pendingSnapshots_.front();
    pendingSnapshots_.pop();
    return snapshot;
}

void BackgroundSimulationEngine::RunBackgroundSimulation() {
    std::cout << "[BackgroundSimulationEngine] Background thread started" << std::endl;
    
    try {
        // Initialize simulation if needed
        if (!simulator_->isSimulationComplete() && !simulator_->isSimulationComplete()) {
            if (!simulator_->initializeSimulation()) {
                std::cerr << "[BackgroundSimulationEngine] Failed to initialize simulation" << std::endl;
                simulationRunning_.store(false);
                return;
            }
        }
        
        lastSnapshotTime_ = std::chrono::steady_clock::now();
        
        // Main simulation loop
        while (simulationRunning_.load() && !shutdownRequested_.load()) {
            if (!simulationPaused_.load()) {
                // Execute time-sliced simulation step
                ExecuteTimeSlicedStep();
                
                // Update progress
                // Note: This would need access to simulator's progress info
                // For now, we'll use a simple calculation
                if (simulator_->isSimulationComplete()) {
                    backgroundProgress_.store(1.0f);
                    simulationRunning_.store(false);
                    break;
                }
                
                // Create periodic snapshots
                auto now = std::chrono::steady_clock::now();
                float timeSinceLastSnapshot = std::chrono::duration<float>(now - lastSnapshotTime_).count();
                if (timeSinceLastSnapshot >= SNAPSHOT_INTERVAL_SECONDS) {
                    CreateAndQueueSnapshot();
                    lastSnapshotTime_ = now;
                }
                
                // Brief yield to prevent 100% CPU usage
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } else {
                // Simulation paused - sleep longer to reduce CPU usage
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        
        // Create final snapshot if simulation completed
        if (simulator_->isSimulationComplete()) {
            CreateAndQueueSnapshot();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[BackgroundSimulationEngine] Exception in background thread: " 
                  << e.what() << std::endl;
    }
    
    simulationRunning_.store(false);
    std::cout << "[BackgroundSimulationEngine] Background thread finished" << std::endl;
}

void BackgroundSimulationEngine::ExecuteTimeSlicedStep() {
    auto stepStartTime = std::chrono::steady_clock::now();
    
    // Execute one simulation step
    bool success = simulator_->stepSimulation();
    
    if (!success) {
        std::cerr << "[BackgroundSimulationEngine] Simulation step failed" << std::endl;
        simulationRunning_.store(false);
        return;
    }
    
    // Check if we should yield to UI
    if (ShouldYieldToUI()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool BackgroundSimulationEngine::ShouldYieldToUI() const {
    // For now, always yield briefly after each step
    // In a more sophisticated implementation, we could time each step
    // and yield only if it took longer than maxStepTimeMs_
    return true;
}

void BackgroundSimulationEngine::CreateAndQueueSnapshot() {
    try {
        // Create snapshot with current simulation state from the real simulator
        PhaseInfo progressInfo = simulator_->getProgressInfo();
        
        std::string description = "Background Simulation (" + 
                                std::to_string(static_cast<int>(progressInfo.totalProgress * 100)) + "%)";
        
        // Use the simulator's createSnapshot method to copy real geological data
        simulator_->createSnapshot(description, progressInfo.totalProgress);
        
        std::cout << "[BackgroundSimulationEngine] Created snapshot with real geological data: " 
                  << description << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[BackgroundSimulationEngine] Failed to create snapshot: " 
                  << e.what() << std::endl;
    }
}

void BackgroundSimulationEngine::AddSnapshot(std::shared_ptr<GeologicalSnapshot> snapshot) {
    std::lock_guard<std::mutex> lock(snapshotMutex_);
    
    pendingSnapshots_.push(snapshot);
    
    // Prevent memory buildup by removing old snapshots
    CleanupOldSnapshots();
}

void BackgroundSimulationEngine::CleanupOldSnapshots() {
    // Remove old snapshots if we have too many
    while (pendingSnapshots_.size() > MAX_PENDING_SNAPSHOTS) {
        pendingSnapshots_.pop();
    }
}

} // namespace World
} // namespace VoxelCastle
