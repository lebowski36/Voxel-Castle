# Implement UI Text Rendering

## Overview
Implement a basic text rendering system for the custom UI framework so that all UI elements (buttons, checkboxes, labels, etc.) can display text. This is required for a usable and visually clear menu system.

## Sub-Tasks

1. **Select Text Rendering Approach**
    - [ ] Decide on bitmap font, stb_truetype, or other method (use stb_truetype.h if no other system is present).
2. **Integrate Font Loading**
    - [ ] Add font loading to UIRenderer or a new FontManager/TextRenderer class.
    - [ ] Load a default font (e.g., from assets or embedded data).
3. **Implement Text Rendering Function**
    - [ ] Implement a function in UIRenderer (or TextRenderer) to render a string at a given position, size, and color.
4. **Add Text Rendering to UI Elements**
    - [ ] Update UIButton to render its text label.
    - [ ] Update UICheckbox to render its label.
    - [ ] Update menu panels to render section titles.
5. **Test and Polish**
    - [ ] Test text rendering in all menus.
    - [ ] Adjust font size, color, and alignment for clarity.
    - [ ] Add error handling/logging for missing fonts.

## Notes
- Use stb_truetype.h for font rasterization if no other system is present.
- Use a simple .ttf font from assets or open source if available.
- Keep the first version simple: no kerning, no advanced effects, just basic ASCII text.
- Integrate with UIRenderer for batching if possible.

## Success Criteria
- All UI elements display readable text labels.
- Menu system is visually clear and usable.
- No crashes or major performance issues from text rendering.
