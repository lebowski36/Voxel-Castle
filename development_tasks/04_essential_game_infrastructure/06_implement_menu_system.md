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
  - Added Resume Game and Settings buttons
  - Implemented button click handlers
- [x] **2.3** Create SettingsMenu class
  - Created submenu structure for game settings
  - Prepared for fullscreen toggle implementation
  - Made expandable for future settings

### Phase 3: Menu UI Implementation
- [x] **3.1** Design main menu layout
  - Created dark semi-transparent panel background
  - Added title panel and properly positioned buttons
  - Buttons are created but may need rendering fixes
- [x] **3.2** Implement Close button functionality
  - Connected "Resume Game" button to menu system's closeMenus() method
  - Set up onMenuClosed callback to handle game state restoration
- [x] **3.3** Implement Settings button functionality
  - Connected Settings button to show settings menu
  - Implemented Back button to navigate from settings to main menu

### Phase 4: Settings Implementation
- [x] **4.1** Implement fullscreen toggle
  - Added fullscreen toggle to Window class
  - Connected checkbox to Game's fullscreen toggle functionality
  - Implemented toggle in real-time
- [x] **4.2** Add settings validation and error handling
  - Added success/failure handling for fullscreen toggle
  - Implemented checkbox state restoration if toggle fails

### Phase 5: Integration and Testing
- [x] **5.1** Basic UI visibility integration
  - Game pauses when menu is open
  - Game resumes when menu is closed
  - HUD is hidden when menu is open, shown when closed
- [x] **5.2** Core menu functionality
  - ESC key opens/closes menu (CONFIRMED WORKING)
  - Resume Game button functionality (CONFIRMED WORKING)
  - Settings button functionality (CONFIRMED WORKING)
  - Back button in settings returns to main menu (CONFIRMED WORKING)
  - [x] **5.2.1** Fullscreen toggle checkbox functionality (CONFIRMED WORKING)
- [ ] **5.3** UI Layout and Interaction Fixes
  - [x] **5.3.1** Fix menu positioning and interaction after fullscreen toggle (COMPLETED)
  - [x] **5.3.2** Fix checkbox state not updating visually when fullscreen is applied (COMPLETED)
  - [ ] **5.3.3** Center buttons properly in menu layout
  - [x] **5.3.4** Add game title to menu title bar (CONFIRMED: "Voxel Castle" title exists)
  - [ ] **5.3.5** Improve overall menu styling and formatting
- [ ] **5.4** Polish and refinement
  - [ ] **5.4.1** Implement smooth menu transitions
  - [ ] **5.4.2** Add visual feedback for button interactions
  - [ ] **5.4.3** Ensure consistent styling across all menu elements

### Phase 6: Enhanced Features
- [ ] **6.1** UI Scaling System
  - [ ] **6.1.1** Add UI scale slider to settings menu (1.0x to 2.0x range)
  - [ ] **6.1.2** Implement unified scaling for both menu and HUD elements
  - [ ] **6.1.3** Save/load UI scale preference
- [ ] **6.2** Improved Button Interaction
  - [ ] **6.2.1** Make text labels clickable (not just button backgrounds)
  - [ ] **6.2.2** Increase button click areas for better usability
  - [ ] **6.2.3** Add button hover effects

## Current Issues (Discovered in Testing)
1. **Fullscreen UI Bug**: After switching to fullscreen, menu becomes unclickable due to positioning issues
2. **Checkbox State Bug**: Fullscreen checkbox doesn't show as checked when fullscreen is active
3. **Layout Issues**: Buttons are not properly centered and formatted
4. **Missing Game Title**: Menu title bar should display the game name
5. **General Styling**: Menu needs better visual styling and formatting

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
- **Menu Layout Progress**: Main menu now displays grey transparent panel with button structure in place
- **File Cleanup**: Removed old BlockSelectionUI files, updated all references to use HUD

## Success Criteria
- [x] ESC key opens main menu instead of toggling mouse capture
- [x] Mouse capture is enabled by default
- [ ] Main menu contains functional Close and Settings options
- [ ] Settings menu includes working fullscreen toggle
- [ ] All menu interactions work smoothly
- [x] No regressions in existing game functionality
- [x] Menu integrates seamlessly with existing UI system
