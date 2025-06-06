#!/usr/bin/env python3
"""
Debug script to test wood texture generation.
"""
from PIL import Image
from texture_generators.wood_textures_enhanced import generate_wood_texture

def save_wood_samples():
    print("Generating debug wood samples...")
    size = 32  # 32x32 pixels

    # Generate samples for each wood type and face
    wood_types = ['oak', 'pine', 'birch', 'mahogany']
    
    for wood_type in wood_types:
        # Generate top/bottom texture (should show end grain/rings)
        top_texture = generate_wood_texture(size, wood_type, 'top')
        top_texture.save(f"debug_wood_{wood_type}_top.png")
        print(f"Saved top texture for {wood_type}")
        
        # Generate side texture (should show bark)
        side_texture = generate_wood_texture(size, wood_type, 'side')
        side_texture.save(f"debug_wood_{wood_type}_side.png")
        print(f"Saved side texture for {wood_type}")

    print("All debug wood samples generated. Check the root directory for PNG files.")

if __name__ == "__main__":
    save_wood_samples()
