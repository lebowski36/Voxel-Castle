"""
Simple Texture Coordinator
==========================

Simplified texture coordinator for testing the modular texture generation system.
This version uses the actual available functions instead of non-existent classes.
"""

from PIL import Image, ImageDraw
from typing import Dict, Tuple, Optional, List
import random

from texture_generators.base_patterns import *
from texture_generators.color_palettes import *
# Import enhanced generator functions that actually exist
from texture_generators.stone_textures_enhanced import generate_stone_texture, generate_processed_stone_texture
from texture_generators.wood_textures_enhanced import generate_wood_texture, generate_plank_texture

class SimpleTextureCoordinator:
    """
    Simple coordinator for texture generation using available functions.
    """
    
    def __init__(self, tile_size: int = 16, seed: Optional[int] = None):
        """
        Initialize the texture coordinator.
        
        Args:
            tile_size: Size of each texture tile (e.g., 16x16 pixels)
            seed: Random seed for reproducible texture generation
        """
        self.tile_size = tile_size
        if seed is not None:
            random.seed(seed)
    
    def generate_block_texture(self, block_type: str, face: str = 'all', size: int = None) -> Image.Image:
        """
        Generate a texture for a specific block type and face.
        
        Args:
            block_type: The type of block to generate (e.g., 'granite', 'oak_wood')
            face: Which face to generate ('top', 'bottom', 'side', 'all')
            size: Texture size in pixels (uses default if None)
            
        Returns:
            PIL Image of the generated texture
        """
        if size is None:
            size = self.tile_size
            
        # Route to appropriate generator function
        try:
            if block_type.endswith('_stone') or block_type in ['granite', 'limestone', 'marble', 'sandstone', 'slate', 'basalt']:
                return generate_stone_texture(size, block_type)
            elif block_type.endswith('_wood') or block_type in ['oak', 'pine', 'birch', 'mahogany']:
                return generate_wood_texture(size, block_type)
            elif block_type.endswith('_brick') or block_type in ['stone_brick', 'granite_brick']:
                return generate_processed_stone_texture(size, block_type)
            elif block_type.endswith('_planks') or block_type in ['oak_planks', 'pine_planks', 'birch_planks']:
                return generate_plank_texture(size, block_type.replace('_planks', ''))
            else:
                # Generate a simple textured block using base patterns
                return self._generate_simple_texture(block_type, size)
        except Exception as e:
            print(f"Warning: Failed to generate texture for {block_type}: {e}")
            return self._generate_fallback_texture(block_type, size)
    
    def _generate_simple_texture(self, block_type: str, size: int) -> Image.Image:
        """Generate a simple texture using base patterns."""
        img = Image.new('RGBA', (size, size), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)
        
        # Get a basic color palette
        base_color = self._get_default_color(block_type)
        palette = {
            'base': base_color,
            'dark_speckle': self._vary_color(base_color, -30),
            'light_speckle': self._vary_color(base_color, 30)
        }
        
        # Apply a basic speckled pattern
        draw_speckled_pattern(draw, 0, 0, size, palette)
        return img
    
    def _generate_fallback_texture(self, block_type: str, size: int) -> Image.Image:
        """Generate a simple colored fallback texture."""
        img = Image.new('RGBA', (size, size), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)
        
        base_color = self._get_default_color(block_type)
        draw.rectangle([0, 0, size-1, size-1], fill=base_color)
        return img
    
    def _get_default_color(self, block_type: str) -> Tuple[int, int, int, int]:
        """Get a default color for block types."""
        colors = {
            'dirt': (139, 69, 19, 255),
            'grass': (34, 139, 34, 255),
            'sand': (238, 203, 173, 255),
            'water': (30, 144, 255, 255),
            'lava': (255, 100, 0, 255),
            'air': (0, 0, 0, 0),
            'gravel': (105, 105, 105, 255),
            'clay': (150, 111, 51, 255),
            'coal_ore': (64, 64, 64, 255),
            'iron_ore': (139, 69, 19, 255),
            'gold_ore': (255, 215, 0, 255),
        }
        return colors.get(block_type, (128, 128, 128, 255))  # Default gray
    
    def _vary_color(self, color: Tuple[int, int, int, int], variation: int) -> Tuple[int, int, int, int]:
        """Vary a color by a given amount."""
        r, g, b, a = color
        r = max(0, min(255, r + variation))
        g = max(0, min(255, g + variation))
        b = max(0, min(255, b + variation))
        return (r, g, b, a)

    def generate_test_textures(self, output_dir: str = "test_textures/") -> Dict[str, str]:
        """
        Generate a set of test textures to validate the system.
        
        Returns:
            Dictionary mapping block types to generated file paths
        """
        import os
        os.makedirs(output_dir, exist_ok=True)
        
        test_blocks = [
            'granite', 'limestone', 'marble', 'sandstone',
            'oak', 'pine', 'birch', 'mahogany',
            'stone_brick', 'granite_brick',
            'oak_planks', 'pine_planks',
            'dirt', 'grass', 'sand', 'gravel',
            'coal_ore', 'iron_ore', 'gold_ore'
        ]
        
        generated_files = {}
        
        for block_type in test_blocks:
            try:
                texture = self.generate_block_texture(block_type)
                filename = f"{block_type}_texture.png"
                filepath = os.path.join(output_dir, filename)
                texture.save(filepath)
                generated_files[block_type] = filepath
                print(f"✓ Generated texture for {block_type}")
            except Exception as e:
                print(f"✗ Failed to generate texture for {block_type}: {e}")
        
        return generated_files
