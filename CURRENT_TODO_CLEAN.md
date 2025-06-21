# CURRENT TODO - UI System Critical Issues

## IMMEDIATE CRITICAL ISSUES

### 1. Segmentation Fault in UI System
**STATUS: CRITICAL - BLOCKING**
- Segfault occurs after "UISystem] Screen size set to 1920x1080" 
- This is NEW - didn't exist before UI system rebuild
- Likely in UIInputManager or UIRenderer initialization
- **ACTION NEEDED**: Debug with gdb to find exact crash location

### 2. Screen Size Mismatch 
**STATUS: HIGH PRIORITY**
- System initializing with 1920x1080 but actual screen is 720p
- Multiple systems have conflicting screen size constants
- **ROOT CAUSE**: No centralized screen size management
- **ACTION NEEDED**: Create global screen size constants that all systems use

## ANALYSIS FROM CONSOLE OUTPUT

**Working Systems:**
- ✅ OpenGL context initialization
- ✅ TextureAtlas loading (main, side, bottom)
- ✅ Game state and time managers
- ✅ MenuSystem basic initialization
- ✅ UISystem initialization (partly)
- ✅ MainMenu and CreateWorldMenu creation
- ✅ Button and TextInput components

**Crash Point:**
- Last successful: "[UISystem] Screen size set to 1920x1080"
- **SEGFAULT IMMEDIATELY AFTER**

## NEXT STEPS

1. **Debug the segfault** - Use gdb to get stack trace
2. **Fix screen size constants** - Create global SCREEN_WIDTH/SCREEN_HEIGHT
3. **Verify UIInputManager initialization** - Likely culprit for crash
4. **Test UI system after fixes**

## LOCATIONS TO INVESTIGATE

- `game/src/core/game.cpp` - screen size constants (1280x720 vs 1920x1080)
- `engine/src/ui/core/UIInputManager.cpp` - potential null pointer dereference
- `engine/src/ui/core/UISystem.cpp` - setScreenSize implementation
- `game/src/core/GameRenderCoordinator.cpp` - screen size calls

## TECHNICAL DEBT

- Remove remaining legacy UI compatibility methods after crash is fixed
- Standardize all screen size references to use global constants
- Complete SettingsMenu implementation (currently stubbed)
