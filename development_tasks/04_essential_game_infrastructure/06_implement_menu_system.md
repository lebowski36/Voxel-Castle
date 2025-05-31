# Menu System Implementation

## Overview
Implement a comprehensive menu system with modified input handling where ESC opens a main menu instead of toggling mouse capture, and mouse capture is enabled by default.

## Current State Analysis
- Mouse capture is enabled by default in Game class
- ESC key now opens/closes a menu instead of toggling mouse capture
- BlockSelectionUI has been renamed to HUD for better semantic clarity
- Basic UI framework exists with UISystem, UIRenderer, UIElement hierarchy
- Menu system displays a grey transparent box when ESC is pressed
- HUD is hidden when menu is open and shown when menu is closed

## Requirements
1. **Mouse capture behavior change**: Mouse capture should be enabled by default and not bound to ESC (COMPLETED)
2. **ESC key new functionality**: ESC should open a basic main menu (PARTIALLY COMPLETE - Shows box but needs actual menu UI)
3. **Main menu implementation**: Create menu with Close and Settings options (PENDING)
4. **Settings menu**: Include fullscreen toggle at minimum (PENDING)

## Sub-Tasks

### Phase 1: Input Handling Modifications
- [x] **1.1** Remove ESC key mouse capture toggle from InputManager.cpp
  - Location: `/home/system-x1/Projects/Voxel Castle/game/src/core/InputManager.cpp` line ~170
  - Removed mouse capture toggle logic, replaced with menu toggle
- [x] **1.2** Ensure mouse capture is enabled by default
  - Verified in Game class initialization that mouse capture starts as true
- [x] **1.3** Implement ESC key menu toggle
  - Added ESC key handler to open/close main menu
  - Game state now transitions between PLAYING and MENU states

### Phase 2: Menu System Architecture
- [x] **2.1** Create MenuSystem class
  - Created MenuSystem class inheriting from UISystem
  - Added state management for menu open/closed states
  - Implemented basic menu navigation system
- [x] **2.2** Create MainMenu class
  - Created UI container for main menu options
  - Added appropriate structure for buttons
  - Set up for handling button interactions
- [x] **2.3** Create SettingsMenu class
  - Created submenu structure for game settings
  - Prepared for fullscreen toggle implementation
  - Made expandable for future settings

### Phase 3: Menu UI Implementation
- [ ] **3.1** Design main menu layout
  - Currently displaying a gray transparent box in the center of screen
  - Need to add proper buttons and styling
  - Ensure proper scaling for different resolutions
- [ ] **3.2** Implement Close button functionality
  - Need to add button to close menu and return to game
  - Add option to exit game entirely with confirmation
- [ ] **3.3** Implement Settings button functionality
  - Need to add button to open settings submenu
  - Implement navigation back to main menu

### Phase 4: Settings Implementation
- [ ] **4.1** Implement fullscreen toggle
  - Connect to existing window management system
  - Save setting preference (if save system exists)
  - Apply setting immediately
- [ ] **4.2** Add settings validation and error handling
  - Handle fullscreen toggle failures gracefully
  - Provide user feedback for setting changes

### Phase 5: Integration and Testing
- [x] **5.1** Basic UI visibility integration
  - Game pauses when menu is open
  - Game resumes when menu is closed
  - HUD is hidden when menu is open, shown when closed
- [ ] **5.2** Test menu functionality
  - ESC key opens/closes menu (WORKING)
  - Button interactions not yet implemented
  - Settings changes not yet implemented
  - No regressions in existing functionality observed
- [ ] **5.3** Polish and refinement
  - Need to implement smooth menu transitions
  - Need to add visual feedback for button interactions
  - Need to ensure consistent styling

## Implementation Notes

### File Locations
- **Input handling**: `/home/system-x1/Projects/Voxel Castle/game/src/core/InputManager.cpp`
- **Game state**: `/home/system-x1/Projects/Voxel Castle/game/src/core/game.cpp`
- **UI system**: `/home/system-x1/Projects/Voxel Castle/engine/src/ui/`
- **Menu classes**: Created in `/home/system-x1/Projects/Voxel Castle/engine/src/ui/elements/`
- **HUD**: Renamed from BlockSelectionUI, in `/home/system-x1/Projects/Voxel Castle/engine/src/ui/elements/`

### Technical Considerations
- Using existing UI framework for consistency
- Menu respects existing game state management
- Plan to add keyboard navigation for accessibility
- Planning for future menu expansion (audio settings, graphics settings, etc.)

### Dependencies
- Existing UI system (UISystem, UIRenderer, UIElement)
- Window management for fullscreen toggle
- Input management system
- Game state management

### Recent Changes
- **UI Naming Change**: Renamed BlockSelectionUI to HUD for better semantic clarity
- **Integration Progress**: Successfully implemented proper UI visibility behavior with menu toggle
- **Window Close Bug Fix**: Fixed bug where clicking window close button (X) doesn't work when menu is open

## Success Criteria
- [x] ESC key opens main menu instead of toggling mouse capture
- [x] Mouse capture is enabled by default
- [ ] Main menu contains functional Close and Settings options
- [ ] Settings menu includes working fullscreen toggle
- [ ] All menu interactions work smoothly
- [x] No regressions in existing game functionality
- [x] Menu integrates seamlessly with existing UI system
