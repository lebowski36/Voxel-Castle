# 08b - Chunk Terrain Cycling Troubleshooting

## Problem Description

**Issue**: When resuming the game from the main menu, as each new chunk loads, ALL visible chunks update their appearance to match the most recently loaded chunk. After all chunks finish loading, they all display identical terrain that matches the last chunk that was loaded.

**Detailed Behavior**:
1. **Initial Load**: First chunk loads and displays correct terrain for its location
2. **Second Chunk Load**: When second chunk loads, BOTH chunks immediately update to display the second chunk's terrain pattern
3. **Sequential Overwriting**: As each subsequent chunk loads, ALL visible chunks update to match the new chunk's appearance
4. **Final State**: After all chunks finish loading, ALL chunks display identical terrain that matches the last loaded chunk
5. **Not Random**: The behavior is sequential and predictable - not random cycling

**Key Characteristics**:
- NOT individual chunks showing wrong terrain - ALL chunks show SAME terrain at any moment
- As each new chunk loads, it overwrites the appearance of ALL previously loaded chunks
- The final result is all chunks looking identical to the last loaded chunk
- Each individual terrain pattern IS correct for its original chunk location when first generated
- The issue is that chunk terrain/mesh data is being shared or overwritten instead of isolated
- Issue appeared after implementing main menu system (commit `2c9b296`)
- Did NOT occur when game started directly into 3D world
