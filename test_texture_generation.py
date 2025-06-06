#!/usr/bin/env python3
"""
Test Texture Generation
======================

Simple test script to verify our modular texture generation system works
before integrating with the main atlas generation.
"""

import sys
import os
from PIL import Image

# Add texture_generators to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'texture_generators'))

from texture_generators.simple_texture_coordinator import SimpleTextureCoordinator

def test_basic_generation():
    """Test basic texture generation for a few key block types."""
    print("Testing modular texture generation system...")
    
    # Initialize coordinator
    coordinator = SimpleTextureCoordinator(tile_size=32, seed=12345)
    
    # Test a few representative block types
    test_blocks = [
        (0, "AIR"),
        (1, "STONE"),
        (2, "DIRT"), 
        (3, "GRASS"),
        (4, "SAND"),
        (30, "WOOD_OAK"),
        (34, "LEAVES_OAK"),
        (50, "WATER")
    ]
    
    print(f"Generating textures for {len(test_blocks)} test blocks...")
    
    for block_id, block_name in test_blocks:
        try:
            print(f"  Generating {block_name} (ID: {block_id})...")
            
            # Generate textures for all faces
            top_texture = coordinator.generate_texture(block_id, 'top')
            side_texture = coordinator.generate_texture(block_id, 'side')
            bottom_texture = coordinator.generate_texture(block_id, 'bottom')
            
            # Save test images
            os.makedirs('test_textures', exist_ok=True)
            top_texture.save(f'test_textures/{block_name}_top.png')
            side_texture.save(f'test_textures/{block_name}_side.png')
            bottom_texture.save(f'test_textures/{block_name}_bottom.png')
            
            print(f"    ✓ Saved {block_name} textures")
            
        except Exception as e:
            print(f"    ✗ Error generating {block_name}: {e}")
            return False
    
    print("✓ Basic texture generation test completed successfully!")
    return True

def test_full_generation():
    """Test full texture generation for all 60 mapped block types."""
    print("\nTesting full texture generation...")
    
    coordinator = SimpleTextureCoordinator(tile_size=32, seed=12345)
    
    try:
        all_textures = coordinator.generate_all_textures()
        print(f"✓ Generated textures for {len(all_textures)} block types")
        
        # Save a sample atlas showing first 16 blocks
        atlas_size = 4  # 4x4 grid
        tile_size = coordinator.tile_size
        atlas_image = Image.new('RGBA', (atlas_size * tile_size, atlas_size * tile_size))
        
        block_ids = sorted(list(all_textures.keys()))[:16]  # First 16 blocks
        
        for i, block_id in enumerate(block_ids):
            if i >= atlas_size * atlas_size:
                break
                
            row = i // atlas_size
            col = i % atlas_size
            x = col * tile_size
            y = row * tile_size
            
            # Use 'side' texture for the atlas preview
            block_texture = all_textures[block_id]['side']
            atlas_image.paste(block_texture, (x, y))
        
        atlas_image.save('test_textures/sample_atlas.png')
        print("✓ Saved sample atlas (first 16 blocks)")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in full generation: {e}")
        return False

def main():
    """Run texture generation tests."""
    print("=" * 50)
    print("TEXTURE GENERATION SYSTEM TEST")
    print("=" * 50)
    
    # Test basic generation first
    if not test_basic_generation():
        print("\n✗ Basic generation test failed!")
        return 1
    
    # Test full generation
    if not test_full_generation():
        print("\n✗ Full generation test failed!")
        return 1
    
    print("\n" + "=" * 50)
    print("✓ ALL TESTS PASSED!")
    print("✓ Modular texture generation system is working")
    print("✓ Check 'test_textures/' directory for generated images")
    print("=" * 50)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
