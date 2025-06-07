#!/usr/bin/env python3
"""
Atlas visualization - shows what textures were generated where
"""

from block_mapping_generated import BLOCK_MAPPING
from atlas_face_system import BLOCK_FACE_PATTERNS

def show_atlas_map():
    test_blocks = [0, 1, 2, 3, 4, 30, 31, 50, 51]
    
    print("🎨 TEST ATLAS MAP (test_atlas_from_json.png)")
    print("=" * 60)
    print("Grid layout (3x3, 32x32 pixels per block):")
    print()
    
    for row in range(3):
        for col in range(3):
            slot = row * 3 + col
            if slot < len(test_blocks):
                block_id = test_blocks[slot]
                block_info = BLOCK_MAPPING[block_id]
                pattern = BLOCK_FACE_PATTERNS[block_id]
                
                print(f"[{col},{row}] Block {block_id:3d}: {block_info['type']:8s}/{block_info['subtype']:12s} ({pattern.value})")
            else:
                print(f"[{col},{row}] Empty")
    
    print()
    print("🔍 WHAT YOU SHOULD SEE:")
    print("Position (0,0): Air - transparent/black")
    print("Position (1,0): Stone - gray with speckles")  
    print("Position (2,0): Dirt - brown with texture")
    print("Position (0,1): Grass - green/brown mix")
    print("Position (1,1): Sand - yellowish with dots")
    print("Position (2,1): Oak Wood - brown wood grain")
    print("Position (0,2): Pine Wood - brown wood grain") 
    print("Position (1,2): Water - blue fluid texture")
    print("Position (2,2): Lava - orange bubbly texture")
    print()
    print("✅ SUCCESS INDICATORS:")
    print("  - You see different colors/textures (not all pink)")
    print("  - Water looks blue")
    print("  - Lava looks orange/red with bubbles") 
    print("  - Wood looks brown")
    print("  - Sand looks yellow")
    print()
    print("🔧 The pink tiles are texture generation errors (normal for testing)")
    print("   These will be fixed as we improve the texture generators")

if __name__ == "__main__":
    show_atlas_map()
