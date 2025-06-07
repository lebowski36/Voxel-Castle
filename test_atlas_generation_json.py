#!/usr/bin/env python3
"""
Test atlas generation using JSON-generated data
This script tests atlas generation with our new JSON-derived block mapping.
"""

from PIL import Image, ImageDraw
import os
import sys
import random

# Import the generated BLOCK_MAPPING
from block_mapping_generated import BLOCK_MAPPING

# Add texture_generators to path for modular texture system
current_dir = os.path.dirname(os.path.abspath(__file__))
texture_generators_path = os.path.join(current_dir, 'texture_generators')
sys.path.insert(0, texture_generators_path)

# Import our modular texture generation system
try:
    from texture_generators.simple_texture_coordinator import SimpleTextureCoordinator
    from texture_generators.stone_textures_enhanced import generate_stone_texture, generate_processed_stone_texture
    from texture_generators.wood_textures_enhanced import generate_wood_texture, generate_plank_texture
    from texture_generators.organic_textures import generate_organic_texture
    from texture_generators.special_textures import generate_special_texture
    from texture_generators.crystal_textures import generate_crystal_texture
    from texture_generators.ceramic_textures import generate_ceramic_texture
    from texture_generators.metal_textures import generate_metal_texture
    from texture_generators.fluid_textures import generate_fluid_texture
    from texture_generators.ore_textures import generate_ore_texture
    MODULAR_SYSTEM_AVAILABLE = True
    print("✓ Modular texture generation system loaded successfully")
except ImportError as e:
    print(f"⚠ Warning: Modular texture system not available ({e}). Using legacy generation.")
    MODULAR_SYSTEM_AVAILABLE = False

# Import atlas system for face patterns
from atlas_face_system import BLOCK_FACE_PATTERNS, FacePattern, AtlasType

def generate_texture_for_block(block_id, texture_size=32):
    """Generate texture for a specific block using the modular system"""
    if block_id not in BLOCK_MAPPING:
        print(f"Warning: Block {block_id} not in mapping")
        # Create a simple debug texture
        img = Image.new('RGB', (texture_size, texture_size), (255, 0, 255))
        return img
    
    block_info = BLOCK_MAPPING[block_id]
    block_type = block_info['type']
    subtype = block_info['subtype']
    
    if not MODULAR_SYSTEM_AVAILABLE:
        # Fallback: simple colored squares
        colors = {
            'stone': (128, 128, 128),
            'organic': (101, 67, 33),
            'wood': (139, 69, 19),
            'air': (0, 0, 0, 0),
            'special': (255, 255, 255),
            'crystal': (255, 0, 255),
            'ceramic': (205, 133, 63),
            'metal': (192, 192, 192),
            'fluid': (0, 0, 255),
            'ore': (64, 64, 64)
        }
        color = colors.get(block_type, (255, 0, 255))
        img = Image.new('RGB', (texture_size, texture_size), color)
        return img
    
    # Use modular system
    try:
        coordinator = SimpleTextureCoordinator()
        if block_type == 'stone':
            return generate_stone_texture(subtype, texture_size)
        elif block_type == 'wood':
            return generate_wood_texture(subtype, texture_size)
        elif block_type == 'organic':
            return generate_organic_texture(subtype, texture_size)
        elif block_type == 'special':
            return generate_special_texture(subtype, texture_size)
        elif block_type == 'crystal':
            return generate_crystal_texture(subtype, texture_size)
        elif block_type == 'ceramic':
            return generate_ceramic_texture(subtype, texture_size)
        elif block_type == 'metal':
            return generate_metal_texture(subtype, texture_size)
        elif block_type == 'fluid':
            return generate_fluid_texture(subtype, texture_size)
        elif block_type == 'ore':
            return generate_ore_texture(subtype, texture_size)
        else:
            # Fallback for unknown types
            img = Image.new('RGB', (texture_size, texture_size), (255, 0, 255))
            return img
    except Exception as e:
        print(f"Warning: Error generating texture for {block_type}/{subtype}: {e}")
        img = Image.new('RGB', (texture_size, texture_size), (255, 0, 255))
        return img

def main():
    """Test atlas generation with a small subset of blocks"""
    print(f"Testing atlas generation with JSON-derived block mapping...")
    
    texture_size = 32
    test_blocks = [0, 1, 2, 3, 4, 30, 31, 50, 51]  # Sample of different block types
    
    # Create a simple test atlas (3x3 grid)
    atlas_size = 3
    atlas_width = atlas_size * texture_size
    atlas_height = atlas_size * texture_size
    atlas = Image.new('RGB', (atlas_width, atlas_height), (0, 0, 0))
    
    print(f"Generating {len(test_blocks)} test textures...")
    
    for i, block_id in enumerate(test_blocks):
        if i >= atlas_size * atlas_size:
            break
            
        x = (i % atlas_size) * texture_size
        y = (i // atlas_size) * texture_size
        
        # Generate texture for this block
        texture = generate_texture_for_block(block_id, texture_size)
        atlas.paste(texture, (x, y))
        
        # Log what we generated
        block_info = BLOCK_MAPPING.get(block_id, {'type': 'unknown', 'subtype': 'unknown'})
        face_pattern = BLOCK_FACE_PATTERNS.get(block_id, 'UNKNOWN')
        print(f"  Block {block_id}: {block_info['type']}/{block_info['subtype']} ({face_pattern.value if hasattr(face_pattern, 'value') else face_pattern}) -> slot {i}")
    
    # Save the test atlas
    output_file = "test_atlas_from_json.png"
    atlas.save(output_file)
    print(f"✓ Test atlas saved as: {output_file}")
    
    # Display atlas information
    print(f"\nAtlas Information:")
    print(f"  Size: {atlas_width}x{atlas_height} pixels")
    print(f"  Grid: {atlas_size}x{atlas_size} slots")
    print(f"  Texture size: {texture_size}x{texture_size} pixels")
    print(f"  Blocks tested: {len(test_blocks)}")
    
    return True

if __name__ == "__main__":
    try:
        success = main()
        if success:
            print(f"\n✅ JSON-to-atlas bridge test completed successfully!")
        else:
            print(f"\n❌ Test failed!")
            sys.exit(1)
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
