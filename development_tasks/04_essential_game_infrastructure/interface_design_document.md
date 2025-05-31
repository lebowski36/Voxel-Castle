# Game Module Interface Design Document

## Overview
This document defines the interfaces between the Game class and the extracted modules to ensure clean separation of concerns and minimal coupling. The design follows SOLID principles and maintains clear boundaries between modules.

## Design Principles
1. **Single Responsibility**: Each module has one clear purpose
2. **Minimal Coupling**: Modules depend only on what they need
3. **Interface Segregation**: Small, focused interfaces rather than large ones
4. **Dependency Inversion**: Depend on abstractions, not concretions
5. **Performance**: Avoid unnecessary indirection or copying

## Module Interface Specifications

### 1. GameLoop Interface (Already Implemented)
**Purpose**: Manages the main game loop timing and coordination.

```cpp
class GameLoop {
public:
    // Main interface method
    int run(Game& game);
    
private:
    // Internal timing methods
    float calculateDeltaTime(...);
    void limitFrameRate(...);
};
```

**Interface Contract**:
- **Input**: Reference to Game instance
- **Output**: Exit code (int)
- **Dependencies**: Game must provide `processInput()`, `update(float)`, `render()`, `isRunning()`, `isWindowRunning()`
- **Responsibilities**: Frame timing, delta time calculation, update/render coordination

### 2. GameRenderCoordinator Interface (Partially Implemented)
**Purpose**: Coordinates all rendering operations in the correct order.

```cpp
namespace VoxelCastle::Core {
class GameRenderCoordinator {
public:
    // Main render coordination method
    void render(
        SpectatorCamera* camera,
        VoxelEngine::Rendering::MeshRenderer* meshRenderer,
        VoxelEngine::Rendering::TextureAtlas* textureAtlas,
        Window* gameWindow,
        VoxelCastle::World::WorldManager* worldManager,
        VoxelEngine::UI::UISystem* uiSystem,
        int screenWidth,
        int screenHeight
    );
    
private:
    // Internal rendering helper methods
    void renderWorld(...);
    void renderUI(...);
    void manageRenderState(...);
};
}
```

**Interface Contract**:
- **Input**: All rendering-related systems and current screen dimensions
- **Output**: None (void)
- **Dependencies**: All major rendering systems, camera, world data
- **Responsibilities**: 3D world rendering, UI rendering, OpenGL state management, buffer swapping

### 3. GameWorldManager Interface (Planned - Optional)
**Purpose**: Manages world initialization, readiness checks, and coordination.

```cpp
namespace VoxelCastle::Core {
class GameWorldManager {
public:
    // Constructor with required dependencies
    GameWorldManager(
        VoxelCastle::World::WorldManager* worldManager,
        VoxelCastle::World::WorldGenerator* worldGenerator
    );
    
    // World readiness interface
    bool isWorldReadyForBlockOperations() const;
    void markWorldAsFullyLoaded();
    bool isWorldFullyLoaded() const;
    
    // World initialization coordination
    void initializeWorldContent();
    void updateWorldInitializationStatus();
    
private:
    // World system references
    VoxelCastle::World::WorldManager* worldManager_;
    VoxelCastle::World::WorldGenerator* worldGenerator_;
    
    // World state tracking
    std::chrono::steady_clock::time_point worldInitTime_;
    bool isWorldFullyLoaded_ = false;
    
    // Helper methods
    void checkChunkLoadingProgress();
    bool hasMinimumChunksLoaded() const;
};
}
```

**Interface Contract**:
- **Input**: World management system references
- **Output**: World readiness status and coordination
- **Dependencies**: WorldManager, WorldGenerator
- **Responsibilities**: World initialization timing, chunk loading coordination, readiness checks

## Interface Integration Strategy

### 1. Game Class Coordination Role
After extraction, the Game class becomes a **coordinator** that:
- **Owns** all major system instances (window, ECS, world systems, rendering, UI)
- **Delegates** specific operations to specialized modules
- **Provides** access to systems through getter methods
- **Coordinates** high-level game state and lifecycle

### 2. Dependency Injection Pattern
All extracted modules receive their dependencies through:
- **Constructor injection** for permanent dependencies
- **Method parameters** for per-operation data
- **Reference passing** to avoid unnecessary copying

### 3. Module Communication Strategy

#### A. GameLoop ↔ Game
```cpp
// GameLoop calls these Game methods:
game.processInput();           // Input handling
game.update(deltaTime);        // Game logic update
game.render();                 // Rendering coordination
game.isRunning();              // Main loop condition
game.isWindowRunning();        // Window validity check
```

#### B. GameRenderCoordinator ↔ Game
```cpp
// Game provides these systems to render coordinator:
renderCoordinator_->render(
    camera_.get(),             // Camera system
    meshRenderer_.get(),       // Mesh rendering
    textureAtlas_.get(),       // Texture management
    gameWindow_.get(),         // Window for buffer swap
    worldManager_.get(),       // World/chunk data
    uiSystem_.get(),           // UI rendering
    screenWidth_,              // Current viewport
    screenHeight_
);
```

#### C. GameWorldManager ↔ Game (if implemented)
```cpp
// Game delegates world management:
worldManagerHelper_->initializeWorldContent();
bool ready = worldManagerHelper_->isWorldReadyForBlockOperations();
worldManagerHelper_->markWorldAsFullyLoaded();
```

### 4. Error Handling Strategy
- **Null checks**: All modules validate input parameters
- **Graceful degradation**: Missing systems don't crash the game
- **Error propagation**: Critical errors bubble up to Game class
- **Logging**: Each module logs its specific errors

### 5. Memory Management Strategy
- **RAII**: All modules use RAII for automatic cleanup
- **Smart pointers**: Game class owns systems with unique_ptr
- **Reference passing**: Modules receive raw pointers/references (no ownership transfer)
- **No circular dependencies**: Modules don't hold references to Game class

## Interface Evolution Guidelines

### Adding New Modules
1. **Define clear responsibility** - what exactly does this module do?
2. **Identify minimal dependencies** - what systems does it need?
3. **Design small interface** - keep public methods focused
4. **Plan for testing** - how will this module be tested in isolation?

### Modifying Existing Interfaces
1. **Backward compatibility** - don't break existing usage
2. **Gradual migration** - change interfaces incrementally
3. **Documentation updates** - keep this document current
4. **Integration testing** - verify all modules still work together

## Performance Considerations

### Optimization Guidelines
- **Minimize indirection**: Direct function calls where possible
- **Avoid copying**: Pass large objects by reference
- **Cache lookups**: Don't repeatedly access the same data
- **Batch operations**: Group related operations together

### Memory Layout
- **Keep modules small**: Each extracted class should be lightweight
- **Minimize state**: Only store what's necessary between calls
- **Use stack allocation**: Prefer stack objects over heap when possible

## Testing Strategy

### Unit Testing
- **Mock dependencies**: Test modules in isolation
- **Test interfaces**: Verify contract compliance
- **Test error conditions**: Handle edge cases gracefully

### Integration Testing
- **Module interactions**: Verify modules work together
- **Performance testing**: Ensure no significant overhead
- **Regression testing**: Verify existing functionality preserved

## Future Extensibility

### Planned Extensions
- **Async operations**: Some modules may need async support
- **Plugin architecture**: Modules may become pluggable
- **Multi-threading**: Some operations may move to background threads

### Design Flexibility
- **Abstract interfaces**: Consider interfaces/abstract classes for major modules
- **Configuration**: Allow module behavior to be configured
- **Dependency injection**: Consider formal DI container if complexity grows

---

## Implementation Notes

### Current Status
- ✅ GameLoop interface implemented and working
- ✅ GameRenderCoordinator interface defined, implementation in progress
- ⏳ GameWorldManager interface designed, implementation optional

### Next Steps
1. Complete GameRenderCoordinator implementation
2. Test render coordination extraction
3. Evaluate need for GameWorldManager based on complexity reduction
4. Perform final integration testing

### Success Metrics
- Game.cpp reduced from ~415 lines to <250 lines
- Each module has <100 lines of code
- No performance regression
- All existing functionality preserved
- Improved maintainability and readability
