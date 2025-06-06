#!/usr/bin/env python3
"""
Test Texture Generation
======================

Simple test script to verify our modular texture generation system works
before integrating with the main atlas generation.
"""

import sys
import os
from PIL import Image, ImageDraw

# Add texture_generators to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'texture_generators'))

from texture_generators.simple_texture_coordinator import SimpleTextureCoordinator

def test_basic_generation():
    """Test basic texture generation for a few key block types."""
    print("Testing modular texture generation system...")
    
    # Initialize coordinator
    coordinator = SimpleTextureCoordinator(tile_size=32, seed=12345)
    
    # Test using the built-in test method
    print("Running comprehensive texture generation test...")
    try:
        generated_files = coordinator.generate_test_textures("test_textures/")
        print(f"\n✓ Successfully generated {len(generated_files)} textures!")
        
        # List generated files
        print("\nGenerated textures:")
        for block_type, filepath in generated_files.items():
            print(f"  {block_type}: {filepath}")
            
        return True
    except Exception as e:
        print(f"✗ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_individual_patterns():
    """Test individual pattern generation functions."""
    print("\nTesting individual pattern functions...")
    
    from texture_generators.base_patterns import (
        draw_speckled_pattern, draw_grain_pattern, draw_crystalline_pattern,
        draw_mottled_pattern, draw_vein_pattern, draw_fluid_pattern, draw_brick_pattern
    )
    from texture_generators.color_palettes import get_palette
    
    try:
        # Create a test image
        img = Image.new('RGBA', (128, 128), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)
        
        # Test each pattern in a grid
        patterns = [
            ('speckled', draw_speckled_pattern),
            ('grain', draw_grain_pattern),
            ('crystalline', draw_crystalline_pattern),
            ('mottled', draw_mottled_pattern),
        ]
        
        size = 32
        for i, (name, pattern_func) in enumerate(patterns):
            x = (i % 4) * size
            y = (i // 4) * size
            
            # Get a basic palette
            palette = {
                'base': (100 + i*30, 100 + i*20, 100 + i*10, 255),
                'dark_speckle': (50 + i*20, 50 + i*15, 50 + i*5, 255),
                'light_speckle': (150 + i*20, 150 + i*15, 150 + i*5, 255)
            }
            
            pattern_func(draw, x, y, size, palette)
        
        # Save test pattern image
        os.makedirs('test_textures', exist_ok=True)
        img.save('test_textures/pattern_test.png')
        print("✓ Pattern test completed successfully!")
        return True
        
    except Exception as e:
        print(f"✗ Pattern test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 50)
    print("MODULAR TEXTURE GENERATION SYSTEM TEST")
    print("=" * 50)
    
    success = True
    
    # Test basic generation
    if not test_basic_generation():
        success = False
    
    # Test individual patterns
    if not test_individual_patterns():
        success = False
    
    print("\n" + "=" * 50)
    if success:
        print("✓ ALL TESTS PASSED!")
        print("The modular texture generation system is working correctly.")
    else:
        print("✗ SOME TESTS FAILED!")
        print("Please check the error messages above.")
    print("=" * 50)
    
    return success

if __name__ == "__main__":
    main()
