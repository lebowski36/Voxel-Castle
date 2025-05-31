# Menu System Implementation

## Overview
Implement a comprehensive menu system with modified input handling where ESC opens a main menu instead of toggling mouse capture, and mouse capture is enabled by default.

## Current State Analysis
- Mouse capture currently defaults to true in Game class
- ESC key currently toggles mouse capture in InputManager.cpp (line 170)
- UI system exists with UISystem, UIRenderer, UIElement hierarchy
- No menu implementation currently exists

## Requirements
1. **Mouse capture behavior change**: Mouse capture should be enabled by default and not bound to ESC
2. **ESC key new functionality**: ESC should open a basic main menu
3. **Main menu implementation**: Create menu with Close and Settings options
4. **Settings menu**: Include fullscreen toggle at minimum

## Sub-Tasks

### Phase 1: Input Handling Modifications
- [ ] **1.1** Remove ESC key mouse capture toggle from InputManager.cpp
  - Location: `/home/system-x1/Projects/Voxel Castle/game/src/core/InputManager.cpp` line ~170
  - Remove or comment out the mouse capture toggle logic
- [ ] **1.2** Ensure mouse capture is enabled by default
  - Verify in Game class initialization that mouse capture starts as true
- [ ] **1.3** Implement ESC key menu toggle
  - Add ESC key handler to open/close main menu
  - Define game state for menu (paused/unpaused)

### Phase 2: Menu System Architecture
- [ ] **2.1** Create MenuSystem class
  - Inherit from or integrate with existing UISystem
  - Handle menu state management (open/closed)
  - Manage menu navigation and input
- [ ] **2.2** Create MainMenu class
  - UI container for main menu options
  - Include Close and Settings buttons
  - Handle button interactions
- [ ] **2.3** Create SettingsMenu class
  - Submenu for game settings
  - Include fullscreen toggle initially
  - Expandable for future settings

### Phase 3: Menu UI Implementation
- [ ] **3.1** Design main menu layout
  - Position menu elements in center of screen
  - Apply consistent styling with existing UI
  - Ensure proper scaling for different resolutions
- [ ] **3.2** Implement Close button functionality
  - Close menu and return to game
  - Or exit game entirely (confirm with user preference)
- [ ] **3.3** Implement Settings button functionality
  - Open settings submenu
  - Navigate back to main menu

### Phase 4: Settings Implementation
- [ ] **4.1** Implement fullscreen toggle
  - Connect to existing window management system
  - Save setting preference (if save system exists)
  - Apply setting immediately
- [ ] **4.2** Add settings validation and error handling
  - Handle fullscreen toggle failures gracefully
  - Provide user feedback for setting changes

### Phase 5: Integration and Testing
- [ ] **5.1** Integrate menu system with game loop
  - Pause game when menu is open
  - Resume game when menu is closed
  - Handle input properly in both states
- [ ] **5.2** Test menu functionality
  - ESC key opens/closes menu
  - All buttons work as expected
  - Settings changes apply correctly
  - No regressions in existing functionality
- [ ] **5.3** Polish and refinement
  - Smooth menu transitions
  - Visual feedback for button interactions
  - Consistent styling

## Implementation Notes

### File Locations
- **Input handling**: `/home/system-x1/Projects/Voxel Castle/game/src/core/InputManager.cpp`
- **Game state**: `/home/system-x1/Projects/Voxel Castle/game/src/core/game.cpp`
- **UI system**: `/home/system-x1/Projects/Voxel Castle/engine/src/ui/`
- **New menu classes**: To be created in `/home/system-x1/Projects/Voxel Castle/game/src/ui/` or `/home/system-x1/Projects/Voxel Castle/engine/src/ui/`

### Technical Considerations
- Use existing UI framework for consistency
- Ensure menu respects existing game state management
- Consider keyboard navigation for accessibility
- Plan for future menu expansion (audio settings, graphics settings, etc.)

### Dependencies
- Existing UI system (UISystem, UIRenderer, UIElement)
- Window management for fullscreen toggle
- Input management system
- Game state management

## Success Criteria
- [ ] ESC key opens main menu instead of toggling mouse capture
- [ ] Mouse capture is enabled by default
- [ ] Main menu contains functional Close and Settings options
- [ ] Settings menu includes working fullscreen toggle
- [ ] All menu interactions work smoothly
- [ ] No regressions in existing game functionality
- [ ] Menu integrates seamlessly with existing UI system
