"""
Wood Texture Generators
======================

Generators for wood types with distinct grain patterns and colors.
Each wood type has unique characteristics for bark and grain.
"""

from PIL import ImageDraw
from texture_generators.base_patterns import draw_grain_pattern
from texture_generators.color_palettes import get_palette

def generate_oak_wood(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Oak wood with classic brown grain pattern."""
    palette = get_palette('oak_wood')
    draw_grain_pattern(draw, x0, y0, size, palette, grain_direction='vertical', line_variation=2)

def generate_pine_wood(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Pine wood with lighter, straighter grain."""
    palette = get_palette('pine_wood')
    draw_grain_pattern(draw, x0, y0, size, palette, grain_direction='vertical', line_variation=1)

def generate_birch_wood(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Birch wood with pale color and distinctive bark spots."""
    palette = get_palette('birch_wood')
    draw_grain_pattern(draw, x0, y0, size, palette, grain_direction='vertical', line_variation=1)
    
    # Add characteristic birch bark spots
    import random
    spot_color = palette.get('bark_spot', (180, 170, 140, 255))
    for _ in range(size // 3):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        spot_width = random.randint(2, max(2, size // 4))
        spot_height = random.randint(1, 2)
        
        # Draw horizontal bark lines
        for i in range(spot_width):
            for j in range(spot_height):
                px = max(x0, min(x0 + size - 1, sx + i))
                py = max(y0, min(y0 + size - 1, sy + j))
                draw.point((px, py), fill=spot_color)

def generate_mahogany_wood(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Mahogany wood with rich, deep red-brown coloring."""
    palette = get_palette('mahogany_wood')
    draw_grain_pattern(draw, x0, y0, size, palette, grain_direction='vertical', line_variation=2)
    
    # Add rich wood tones
    import random
    rich_color = palette.get('rich_tone', (140, 70, 50, 255))
    for _ in range(size // 4):
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        draw.point((rx, ry), fill=rich_color)

def generate_bamboo_wood(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Bamboo with segmented, hollow appearance."""
    palette = {
        'base': (220, 200, 140, 255),
        'grain_dark': (180, 160, 100, 255),
        'grain_light': (240, 220, 160, 255),
        'segment': (160, 140, 80, 255)
    }
    
    # Base bamboo color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Vertical grain
    draw_grain_pattern(draw, x0, y0, size, palette, grain_direction='vertical', line_variation=1)
    
    # Add bamboo segments (horizontal lines)
    import random
    segment_count = random.randint(1, 3)
    for i in range(segment_count):
        segment_y = y0 + (size // (segment_count + 1)) * (i + 1)
        segment_y += random.randint(-1, 1)
        segment_y = max(y0, min(y0 + size - 1, segment_y))
        
        # Draw segment line across width
        for x in range(x0, x0 + size):
            draw.point((x, segment_y), fill=palette['segment'])
            # Add thickness
            if segment_y + 1 < y0 + size:
                draw.point((x, segment_y + 1), fill=palette['segment'])

# Lookup table for wood generators
WOOD_GENERATORS = {
    30: generate_oak_wood,        # WOOD_OAK (legacy WOOD at new position)
    31: generate_pine_wood,       # WOOD_PINE
    32: generate_birch_wood,      # WOOD_BIRCH
    33: generate_mahogany_wood,   # WOOD_MAHOGANY
}
