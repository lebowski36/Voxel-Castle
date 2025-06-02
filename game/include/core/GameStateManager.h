#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

// Forward declaration
enum class GameState;

namespace VoxelCastle {
namespace Core {

/**
 * @brief Manages game state transitions and validation
 * 
 * The GameStateManager provides centralized state management with:
 * - State transition validation and rules
 * - State change callbacks and notifications
 * - State stack for nested states (e.g., pause during transitions)
 * - Debug and logging support for state changes
 */
class GameStateManager {
public:
    // State transition callback type
    using StateChangeCallback = std::function<void(GameState from, GameState to)>;
    
    GameStateManager();
    ~GameStateManager() = default;

    /**
     * @brief Initialize the state manager with initial state
     * @param initialState The starting game state
     */
    void initialize(GameState initialState);

    /**
     * @brief Request a state transition
     * @param newState The state to transition to
     * @param force If true, bypasses transition validation
     * @return true if transition was successful, false if blocked
     */
    bool requestStateChange(GameState newState, bool force = false);

    /**
     * @brief Get the current game state
     * @return Current state
     */
    GameState getCurrentState() const { return currentState_; }

    /**
     * @brief Get the previous game state
     * @return Previous state (useful for restoration)
     */
    GameState getPreviousState() const { return previousState_; }

    /**
     * @brief Check if a state transition is valid
     * @param from Source state
     * @param to Target state
     * @return true if transition is allowed
     */
    bool isTransitionValid(GameState from, GameState to) const;

    /**
     * @brief Push current state onto stack and transition to new state
     * Used for temporary state changes (e.g., menu over gameplay)
     * @param newState State to transition to
     * @return true if successful
     */
    bool pushState(GameState newState);

    /**
     * @brief Pop state from stack and restore previous state
     * @return true if successful, false if stack was empty
     */
    bool popState();

    /**
     * @brief Register callback for state changes
     * @param callback Function to call when state changes
     */
    void registerStateChangeCallback(StateChangeCallback callback);

    /**
     * @brief Clear all registered callbacks
     */
    void clearCallbacks();

    // Helper functions for common state queries
    bool isPlaying() const;
    bool isPaused() const;
    bool isInTransition() const;
    bool canAcceptInput() const;
    bool isInMenu() const;

    /**
     * @brief Get human-readable name for a state
     * @param state The state to get name for
     * @return String representation of the state
     */
    static std::string getStateName(GameState state);

    /**
     * @brief Enable/disable debug logging for state transitions
     * @param enabled True to enable debug output
     */
    void setDebugLogging(bool enabled) { debugLogging_ = enabled; }

private:
    GameState currentState_;
    GameState previousState_;
    std::vector<GameState> stateStack_;
    std::vector<StateChangeCallback> callbacks_;
    bool debugLogging_ = true;

    /**
     * @brief Setup default transition rules
     */
    void setupTransitionRules();

    /**
     * @brief Check if transition is allowed based on rules
     * @param from Source state
     * @param to Target state
     * @return true if allowed
     */
    bool checkTransitionRules(GameState from, GameState to) const;

    /**
     * @brief Execute the state transition
     * @param newState State to transition to
     */
    void executeStateTransition(GameState newState);

    /**
     * @brief Notify all registered callbacks of state change
     * @param from Previous state
     * @param to New state
     */
    void notifyStateChange(GameState from, GameState to);

    // Transition rules: from_state -> list of allowed to_states
    std::unordered_map<int, std::vector<int>> transitionRules_;
};

} // namespace Core
} // namespace VoxelCastle
