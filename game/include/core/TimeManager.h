#pragma once

#include <memory>
#include <functional>

namespace flecs {
    class world;
}

namespace VoxelCastle {
namespace Core {

/**
 * @brief Manages game time and simulation speed
 * 
 * The TimeManager controls:
 * - Simulation pause/resume
 * - Time scaling (1x, 2x, 0.5x speeds)
 * - System-specific time control
 * - Integration with ECS for pausing specific systems
 */
class TimeManager {
public:
    // Time scale presets
    enum class TimeScale {
        PAUSED = 0,     // Simulation stopped
        VERY_SLOW = 1,  // 0.25x speed
        SLOW = 2,       // 0.5x speed
        NORMAL = 3,     // 1.0x speed (default)
        FAST = 4,       // 2.0x speed
        VERY_FAST = 5   // 4.0x speed
    };

    // Time change notification callback type
    using TimeScaleChangeCallback = std::function<void(float oldScale, float newScale)>;

    /**
     * @brief Construct a new Time Manager
     * @param world The ECS world to control
     */
    TimeManager(flecs::world* world);
    
    /**
     * @brief Destroy the Time Manager
     */
    ~TimeManager() = default;

    /**
     * @brief Initialize time manager systems and components
     * Called once after construction
     */
    void initialize();

    /**
     * @brief Update time-related systems
     * Called once per frame from game loop
     * @param realDeltaTime Unscaled delta time from system clock
     * @return Scaled delta time for simulation use
     */
    float update(float realDeltaTime);

    /**
     * @brief Pause the simulation
     * @return true if successfully paused
     */
    bool pause();

    /**
     * @brief Resume the simulation at previous speed
     * @return true if successfully resumed
     */
    bool resume();

    /**
     * @brief Toggle between paused and running states
     * @return New paused state (true if paused)
     */
    bool togglePause();

    /**
     * @brief Check if simulation is currently paused
     * @return true if paused
     */
    bool isPaused() const;

    /**
     * @brief Set the simulation time scale
     * @param scale The time scale to use
     */
    void setTimeScale(TimeScale scale);

    /**
     * @brief Set the simulation time scale as a float
     * @param scale The exact scale factor (1.0 = normal speed)
     */
    void setTimeScaleFactor(float scale);

    /**
     * @brief Get the current time scale
     * @return The current TimeScale preset
     */
    TimeScale getTimeScale() const;

    /**
     * @brief Get the current time scale factor
     * @return The current scale factor (1.0 = normal)
     */
    float getTimeScaleFactor() const;

    /**
     * @brief Get a human-readable representation of the current time scale
     * @return String like "Normal (1.0x)" or "Fast (2.0x)"
     */
    std::string getTimeScaleString() const;

    /**
     * @brief Register a callback for time scale changes
     * @param callback Function to call when time scale changes
     */
    void registerTimeScaleChangeCallback(TimeScaleChangeCallback callback);

    /**
     * @brief Determine if a system should be paused
     * Based on system type and current pause state
     * @param systemName The system name to check
     * @return true if the system should pause
     */
    bool shouldSystemPause(const char* systemName) const;

private:
    // Applies the current time scale settings to ECS systems
    void applyTimeScaleToSystems();

    // Convert TimeScale enum to float factor
    float timeScaleToFactor(TimeScale scale) const;

    // Convert float factor to closest TimeScale enum
    TimeScale factorToTimeScale(float factor) const;

    // Internal state
    bool isPaused_ = false;
    float timeScaleFactor_ = 1.0f;
    TimeScale currentTimeScale_ = TimeScale::NORMAL;
    float previousTimeScaleFactor_ = 1.0f;
    float accumulatedTime_ = 0.0f;
    float simulationTime_ = 0.0f;

    // Systems that should always run regardless of pause state
    std::vector<std::string> alwaysRunSystems_;

    // Callback for time scale changes
    std::vector<TimeScaleChangeCallback> timeScaleCallbacks_;

    // Pointer to ECS world
    flecs::world* ecsWorld_;
};

} // namespace Core
} // namespace VoxelCastle
