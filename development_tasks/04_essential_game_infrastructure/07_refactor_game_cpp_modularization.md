# Game.cpp Refactoring - Code Modularization

## Overview
Refactor the large `game.cpp` file (483+ lines) into smaller, more focused modules to improve maintainability, readability, and reduce context length issues. Each extracted module should have a single responsibility and be thoroughly tested.

## Current State Analysis
- **File**: `/home/system-x1/Projects/Voxel Castle/game/src/core/game.cpp` (~483 lines)
- **Issues**: Monolithic structure, multiple responsibilities, difficult to maintain
- **Dependencies**: Multiple engine systems, UI, rendering, input, world management

## Refactoring Strategy
Extract major functional blocks into separate classes while maintaining the Game class as a coordinator.

## Sub-Tasks

### Phase 1: Analysis and Planning
- [x] **1.1** Analyze current game.cpp structure and identify extraction candidates
- [x] **1.2** Create detailed refactoring plan with module responsibilities
- [ ] **1.3** Design interfaces between Game class and extracted modules

### Phase 2: Game Loop Extraction
- [x] **2.1** Create GameLoop class
  - Extract `run()` method logic (~100 lines)
  - Handle frame timing, delta time calculation
  - Coordinate update/render calls
  - File: `/home/system-x1/Projects/Voxel Castle/game/include/core/GameLoop.h`
  - File: `/home/system-x1/Projects/Voxel Castle/game/src/core/GameLoop.cpp`
- [x] **2.2** Update Game class to use GameLoop
  - Replace `run()` implementation with GameLoop delegation
  - Test that game loop still functions correctly
- [x] **2.3** Build and test Game Loop extraction
  - Verify game still runs without regressions
  - Confirm frame timing and input handling work

### Phase 3: Game State Management Extraction
- [ ] **3.1** Create GameStateManager class
  - Extract menu state management (`toggleMenu()`, game state logic)
  - Handle camera mode switching
  - Manage game pause/resume functionality
  - Mouse capture state coordination
  - File: `/home/system-x1/Projects/Voxel Castle/game/include/core/GameStateManager.h`
  - File: `/home/system-x1/Projects/Voxel Castle/game/src/core/GameStateManager.cpp`
- [ ] **3.2** Update Game class to use GameStateManager
  - Replace state management methods with GameStateManager delegation
  - Update ESC key handling integration
- [ ] **3.3** Build and test Game State Management extraction
  - Verify ESC menu toggle still works
  - Confirm mouse capture toggles correctly
  - Test camera mode switching

### Phase 4: Render Coordination Extraction
- [ ] **4.1** Create GameRenderCoordinator class
  - Extract `render()` method logic (~50 lines)
  - Coordinate 3D world rendering
  - Coordinate UI rendering
  - Handle render state management
  - File: `/home/system-x1/Projects/Voxel Castle/game/include/core/GameRenderCoordinator.h`
  - File: `/home/system-x1/Projects/Voxel Castle/game/src/core/GameRenderCoordinator.cpp`
- [ ] **4.2** Update Game class to use GameRenderCoordinator
  - Replace `render()` implementation with coordinator delegation
  - Ensure all rendering subsystems are properly coordinated
- [ ] **4.3** Build and test Render Coordination extraction
  - Verify visual rendering works correctly
  - Confirm UI elements still display
  - Test world rendering pipeline

### Phase 5: World Management Helper Extraction
- [ ] **5.1** Create GameWorldManager class (optional, if time permits)
  - Extract world readiness checks (`isWorldReadyForBlockOperations()`)
  - Extract chunk loading coordination
  - Handle world initialization timing
  - File: `/home/system-x1/Projects/Voxel Castle/game/include/core/GameWorldManager.h`
  - File: `/home/system-x1/Projects/Voxel Castle/game/src/core/GameWorldManager.cpp`
- [ ] **5.2** Update Game class to use GameWorldManager (optional)
  - Replace world management methods with delegation
- [ ] **5.3** Build and test World Management extraction (optional)
  - Verify world loading works correctly
  - Test block placement functionality

### Phase 6: Final Integration and Cleanup
- [ ] **6.1** Update CMakeLists.txt
  - Add all new source files to build system
  - Ensure proper compilation order
- [ ] **6.2** Verify reduced Game.cpp size
  - Target: Reduce from ~483 lines to ~200-250 lines
  - Ensure Game class focuses on coordination, not implementation
- [ ] **6.3** Final integration testing
  - Complete gameplay test (movement, camera, world interaction)
  - Menu system test (ESC toggle, mouse capture)
  - Block placement test
  - Performance regression test
- [ ] **6.4** Code review and documentation
  - Add documentation comments to new classes
  - Update any relevant design documents
  - Verify clean interfaces between modules

## Success Criteria
- [ ] Game.cpp reduced from ~483 lines to ~250 lines or less
- [ ] Each extracted module has a single, clear responsibility
- [ ] All existing functionality works without regression
- [ ] Build system properly includes all new files
- [ ] Code is more maintainable and readable
- [ ] No performance degradation

## Technical Considerations
- **Dependency Management**: Ensure extracted classes don't create circular dependencies
- **Interface Design**: Keep interfaces simple and focused
- **Testing Strategy**: Test each extraction step individually before proceeding
- **Performance**: Avoid unnecessary indirection or overhead
- **Memory Management**: Maintain proper RAII and smart pointer usage

## File Locations
- **Game class**: `/home/system-x1/Projects/Voxel Castle/game/src/core/game.cpp`
- **New modules**: `/home/system-x1/Projects/Voxel Castle/game/src/core/` and `/game/include/core/`
- **Build system**: `/home/system-x1/Projects/Voxel Castle/game/CMakeLists.txt`

## Dependencies
- Existing Game class architecture
- Engine rendering systems
- UI framework
- Input management
- World management systems
- Mouse capture manager (recently implemented)

## Risk Mitigation
- Test each extraction step individually
- Maintain backup of working state before each change
- Keep extracted modules as simple as possible initially
- Ensure proper error handling in new modules
- Verify build system changes don't break compilation
