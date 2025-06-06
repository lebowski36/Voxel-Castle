"""
Stone Texture Generators
=======================

Generators for all stone variants using base patterns with stone-specific palettes.
"""

from PIL import ImageDraw
from texture_generators.base_patterns import draw_speckled_pattern, draw_vein_pattern, draw_crystalline_pattern
from texture_generators.color_palettes import get_palette

def generate_basic_stone(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Basic grey stone with speckles."""
    palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, palette, density=6, variation=25)

def generate_granite(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Granite with larger crystals and speckles."""
    palette = get_palette('granite')
    # Base speckled pattern
    draw_speckled_pattern(draw, x0, y0, size, palette, density=5, variation=20)
    # Add some crystalline highlights
    crystal_palette = {
        'base': palette['crystal'],
        'edge': palette['light_speckle'],
        'shine': palette['base']
    }
    draw_crystalline_pattern(draw, x0, y0, size, crystal_palette, crystal_count=1)

def generate_marble(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Marble with characteristic veining."""
    palette = get_palette('marble')
    # Base color
    base_color = palette['base']
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add marble veining
    vein_palette = {
        'base': palette['base'],
        'vein_dark': palette['vein_dark']
    }
    draw_vein_pattern(draw, x0, y0, size, vein_palette, vein_count=2)

def generate_limestone(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Limestone with fine, uniform texture."""
    palette = get_palette('limestone')
    draw_speckled_pattern(draw, x0, y0, size, palette, density=8, variation=15)

def generate_sandstone(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Sandstone with layered, granular appearance."""
    palette = get_palette('sandstone')
    draw_speckled_pattern(draw, x0, y0, size, palette, density=4, variation=25)

def generate_slate(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Slate with fine grain and dark color."""
    palette = get_palette('slate')
    draw_speckled_pattern(draw, x0, y0, size, palette, density=10, variation=15)

def generate_obsidian(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Obsidian with glassy, reflective surface."""
    palette = get_palette('obsidian')
    # Base dark color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add glass-like reflections
    import random
    for _ in range(size // 4):
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        reflect_color = random.choice([palette['shine'], palette['reflection']])
        draw.point((rx, ry), fill=reflect_color)

def generate_basalt(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Basalt with volcanic, fine-grained texture."""
    palette = {
        'base': (70, 70, 70, 255),
        'dark_speckle': (40, 40, 40, 255),
        'light_speckle': (100, 100, 100, 255)
    }
    draw_speckled_pattern(draw, x0, y0, size, palette, density=7, variation=20)

def generate_quartzite(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Quartzite with crystalline, sparkly appearance."""
    palette = {
        'base': (220, 220, 240, 255),
        'edge': (240, 240, 255, 255),
        'shine': (255, 255, 255, 255)
    }
    # Base speckled
    draw_speckled_pattern(draw, x0, y0, size, palette, density=8, variation=15)
    # Add crystalline sparkles
    draw_crystalline_pattern(draw, x0, y0, size, palette, crystal_count=2)

def generate_pumice(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Pumice with porous, lightweight appearance."""
    palette = {
        'base': (160, 160, 160, 255),
        'dark_speckle': (120, 120, 120, 255),
        'light_speckle': (200, 200, 200, 255)
    }
    # Very dense speckles to simulate porous texture
    draw_speckled_pattern(draw, x0, y0, size, palette, density=3, variation=30)

# Lookup table for stone generators
STONE_GENERATORS = {
    1: generate_basic_stone,      # STONE
    10: generate_granite,         # GRANITE  
    11: generate_limestone,       # LIMESTONE
    12: generate_marble,          # MARBLE
    13: generate_sandstone,       # SANDSTONE
    14: generate_slate,           # SLATE
    15: generate_basalt,          # BASALT
    16: generate_quartzite,       # QUARTZITE
    17: generate_obsidian,        # OBSIDIAN
    18: generate_pumice,          # PUMICE
}
