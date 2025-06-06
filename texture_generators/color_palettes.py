"""
Color Palettes for Texture Generation
====================================

Defines color schemes for different materials that can be mixed and combined.
"""

import random
from typing import Tuple, List, Dict

# Type aliases
Color = Tuple[int, int, int, int]  # RGBA
ColorPalette = Dict[str, Color]

# ========== NATURAL STONE PALETTES ==========

STONE_BASIC = {
    'base': (128, 128, 128, 255),
    'dark_speckle': (90, 90, 90, 255),
    'light_speckle': (160, 160, 160, 255)
}

GRANITE = {
    'base': (150, 120, 100, 255),
    'dark_speckle': (100, 80, 60, 255),
    'light_speckle': (180, 150, 130, 255),
    'crystal': (200, 180, 160, 255)
}

MARBLE = {
    'base': (240, 240, 235, 255),
    'vein_light': (250, 250, 250, 255),
    'vein_dark': (200, 200, 195, 255),
    'accent': (220, 220, 215, 255)
}

LIMESTONE = {
    'base': (220, 210, 180, 255),
    'light': (240, 230, 200, 255),
    'dark': (180, 170, 140, 255)
}

SANDSTONE = {
    'base': (194, 178, 128, 255),
    'light': (220, 200, 150, 255),
    'dark': (160, 145, 100, 255)
}

SLATE = {
    'base': (70, 80, 90, 255),
    'light': (100, 110, 120, 255),
    'dark': (40, 50, 60, 255)
}

OBSIDIAN = {
    'base': (20, 20, 25, 255),
    'shine': (40, 40, 50, 255),
    'reflection': (60, 60, 80, 255)
}

# ========== WOOD PALETTES ==========

OAK_WOOD = {
    'base': (160, 120, 80, 255),
    'grain_dark': (120, 90, 60, 255),
    'grain_light': (180, 140, 100, 255),
    'knot': (80, 60, 40, 255)
}

PINE_WOOD = {
    'base': (200, 180, 120, 255),
    'grain_dark': (160, 140, 90, 255),
    'grain_light': (220, 200, 140, 255),
    'knot': (120, 100, 70, 255)
}

BIRCH_WOOD = {
    'base': (245, 235, 200, 255),
    'grain_dark': (200, 190, 160, 255),
    'grain_light': (255, 245, 220, 255),
    'bark_spot': (180, 170, 140, 255)
}

MAHOGANY_WOOD = {
    'base': (120, 60, 40, 255),
    'grain_dark': (80, 40, 25, 255),
    'grain_light': (150, 80, 60, 255),
    'rich_tone': (140, 70, 50, 255)
}

# ========== ORE & MINERAL PALETTES ==========

COAL_ORE = {
    'stone_base': STONE_BASIC['base'],
    'coal': (25, 25, 25, 255),
    'coal_shine': (45, 45, 45, 255)
}

IRON_ORE = {
    'stone_base': STONE_BASIC['base'],
    'iron': (139, 90, 70, 255),
    'iron_rust': (160, 82, 45, 255)
}

COPPER_ORE = {
    'stone_base': STONE_BASIC['base'],
    'copper': (184, 115, 51, 255),
    'copper_oxide': (120, 180, 120, 255)
}

GOLD_ORE = {
    'stone_base': STONE_BASIC['base'],
    'gold': (255, 215, 0, 255),
    'gold_bright': (255, 235, 50, 255)
}

# ========== LEAF PALETTES ==========

OAK_LEAVES = {
    'base': (80, 120, 40, 255),
    'highlight': (100, 140, 60, 255),
    'shadow': (60, 100, 30, 255),
    'vein': (70, 110, 35, 255)
}

PINE_LEAVES = {
    'base': (40, 80, 40, 255),
    'highlight': (60, 100, 60, 255),
    'shadow': (30, 60, 30, 255),
    'needle': (50, 90, 50, 255)
}

BIRCH_LEAVES = {
    'base': (120, 150, 60, 255),
    'highlight': (140, 170, 80, 255),
    'shadow': (100, 130, 50, 255),
    'autumn': (180, 150, 80, 255)
}

# ========== FLUID PALETTES ==========

WATER = {
    'base': (60, 120, 200, 200),
    'light': (100, 150, 220, 180),
    'dark': (40, 90, 160, 220),
    'foam': (200, 220, 240, 150)
}

LAVA = {
    'base': (200, 80, 20, 255),
    'hot': (255, 150, 50, 255),
    'cooled': (120, 40, 10, 255),
    'glow': (255, 200, 100, 255)
}

# ========== SOIL & TERRAIN PALETTES ==========

DIRT = {
    'base': (139, 90, 60, 255),
    'dark': (100, 65, 40, 255),
    'light': (160, 110, 80, 255),
    'organic': (120, 80, 50, 255)
}

GRASS = {
    'top': (80, 140, 60, 255),
    'side_grass': (70, 120, 50, 255),
    'side_dirt': DIRT['base'],
    'blade': (60, 100, 40, 255)
}

SAND = {
    'base': (240, 220, 160, 255),
    'light': (250, 230, 180, 255),
    'dark': (200, 180, 120, 255)
}

# ========== SPECIAL EFFECT PALETTES ==========

CRYSTAL_CLEAR = {
    'base': (240, 250, 255, 180),
    'edge': (200, 220, 240, 255),
    'shine': (255, 255, 255, 100)
}

MAGICAL_ENERGY = {
    'base': (120, 80, 200, 200),
    'glow': (180, 140, 255, 150),
    'spark': (255, 200, 255, 100)
}

# ========== PALETTE UTILITIES ==========

def get_palette(material_name: str) -> ColorPalette:
    """Get a color palette by material name."""
    palettes = {
        'stone_basic': STONE_BASIC,
        'granite': GRANITE,
        'marble': MARBLE,
        'limestone': LIMESTONE,
        'sandstone': SANDSTONE,
        'slate': SLATE,
        'obsidian': OBSIDIAN,
        'oak_wood': OAK_WOOD,
        'pine_wood': PINE_WOOD,
        'birch_wood': BIRCH_WOOD,
        'mahogany_wood': MAHOGANY_WOOD,
        'coal_ore': COAL_ORE,
        'iron_ore': IRON_ORE,
        'copper_ore': COPPER_ORE,
        'gold_ore': GOLD_ORE,
        'oak_leaves': OAK_LEAVES,
        'pine_leaves': PINE_LEAVES,
        'birch_leaves': BIRCH_LEAVES,
        'water': WATER,
        'lava': LAVA,
        'dirt': DIRT,
        'grass': GRASS,
        'sand': SAND,
        'crystal_clear': CRYSTAL_CLEAR,
        'magical_energy': MAGICAL_ENERGY
    }
    return palettes.get(material_name, STONE_BASIC)

def blend_colors(color1: Color, color2: Color, ratio: float = 0.5) -> Color:
    """Blend two colors together. Ratio 0.0 = all color1, 1.0 = all color2."""
    ratio = max(0.0, min(1.0, ratio))
    return (
        int(color1[0] * (1 - ratio) + color2[0] * ratio),
        int(color1[1] * (1 - ratio) + color2[1] * ratio),
        int(color1[2] * (1 - ratio) + color2[2] * ratio),
        int(color1[3] * (1 - ratio) + color2[3] * ratio)
    )

def vary_color(color: Color, variation: int = 20) -> Color:
    """Add random variation to a color within specified range."""
    return (
        max(0, min(255, color[0] + random.randint(-variation, variation))),
        max(0, min(255, color[1] + random.randint(-variation, variation))),
        max(0, min(255, color[2] + random.randint(-variation, variation))),
        color[3]  # Keep alpha unchanged
    )
