"""
Organic Texture Generators
=========================

Generators for leaves, plants, and other organic materials.
Each organic type has unique patterns and characteristics.
"""

from PIL import ImageDraw
import random
from .base_patterns import draw_mottled_pattern, draw_speckled_pattern
from .color_palettes import get_palette

def generate_oak_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Oak leaves with mottled green pattern and leaf veins."""
    palette = get_palette('oak_leaves')
    draw_mottled_pattern(draw, x0, y0, size, palette, mottle_density=4)
    
    # Add leaf veins
    vein_color = palette.get('vein', (70, 110, 35, 255))
    for _ in range(size // 4):
        # Random vein starting point
        vx = random.randint(x0, x0 + size - 1)
        vy = random.randint(y0, y0 + size - 1)
        
        # Draw short vein lines
        vein_length = random.randint(2, size // 3)
        direction = random.choice(['horizontal', 'vertical', 'diagonal'])
        
        for i in range(vein_length):
            if direction == 'horizontal':
                px = max(x0, min(x0 + size - 1, vx + i))
                py = vy
            elif direction == 'vertical':
                px = vx
                py = max(y0, min(y0 + size - 1, vy + i))
            else:  # diagonal
                px = max(x0, min(x0 + size - 1, vx + i))
                py = max(y0, min(y0 + size - 1, vy + i))
            
            if x0 <= px < x0 + size and y0 <= py < y0 + size:
                draw.point((px, py), fill=vein_color)

def generate_pine_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Pine needles with fine, textured appearance."""
    palette = get_palette('pine_leaves')
    
    # Base color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Draw needle-like texture
    needle_color = palette.get('needle', (50, 90, 50, 255))
    highlight_color = palette.get('highlight', (60, 100, 60, 255))
    
    # Many small needle marks
    for _ in range(size * size // 3):
        nx = random.randint(x0, x0 + size - 1)
        ny = random.randint(y0, y0 + size - 1)
        
        needle_type = random.choice([needle_color, highlight_color, palette['shadow']])
        draw.point((nx, ny), fill=needle_type)
        
        # Sometimes draw small needle clusters
        if random.random() < 0.2:
            for dx in [-1, 0, 1]:
                px = max(x0, min(x0 + size - 1, nx + dx))
                if x0 <= px < x0 + size:
                    draw.point((px, ny), fill=needle_type)

def generate_birch_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Birch leaves with lighter green and yellow tints."""
    palette = get_palette('birch_leaves')
    draw_mottled_pattern(draw, x0, y0, size, palette, mottle_density=5)
    
    # Add some autumn coloring
    autumn_color = palette.get('autumn', (180, 150, 80, 255))
    for _ in range(size // 6):
        ax = random.randint(x0, x0 + size - 1)
        ay = random.randint(y0, y0 + size - 1)
        draw.point((ax, ay), fill=autumn_color)

def generate_palm_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Palm fronds with long, flowing patterns."""
    palette = {
        'base': (60, 120, 60, 255),
        'highlight': (80, 140, 80, 255),
        'shadow': (40, 100, 40, 255),
        'frond': (50, 110, 50, 255)
    }
    
    # Base color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Draw palm frond lines
    frond_color = palette['frond']
    for i in range(size // 2):
        # Diagonal frond lines
        start_x = x0 + random.randint(0, size // 2)
        start_y = y0
        
        frond_length = size
        for j in range(frond_length):
            fx = start_x + j
            fy = start_y + j
            
            if x0 <= fx < x0 + size and y0 <= fy < y0 + size:
                draw.point((fx, fy), fill=frond_color)

def generate_brown_mushroom(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Brown mushroom with cap texture."""
    palette = {
        'cap': (139, 90, 70, 255),
        'cap_light': (160, 110, 90, 255),
        'cap_dark': (110, 70, 50, 255),
        'stem': (200, 180, 160, 255)
    }
    
    # Mushroom cap (top portion)
    cap_height = size // 2
    draw.rectangle([x0, y0, x0 + size - 1, y0 + cap_height - 1], fill=palette['cap'])
    
    # Add cap speckles
    for _ in range(cap_height * size // 6):
        cx = random.randint(x0, x0 + size - 1)
        cy = random.randint(y0, y0 + cap_height - 1)
        cap_color = random.choice([palette['cap_light'], palette['cap_dark']])
        draw.point((cx, cy), fill=cap_color)
    
    # Mushroom stem (bottom portion)
    stem_width = max(1, size // 3)
    stem_start_x = x0 + (size - stem_width) // 2
    draw.rectangle([
        stem_start_x, y0 + cap_height,
        stem_start_x + stem_width - 1, y0 + size - 1
    ], fill=palette['stem'])

def generate_red_mushroom(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Red mushroom with spotted cap."""
    palette = {
        'cap': (180, 50, 50, 255),
        'spots': (255, 255, 255, 255),
        'cap_shadow': (140, 30, 30, 255),
        'stem': (220, 220, 200, 255)
    }
    
    # Mushroom cap (top portion)
    cap_height = size // 2
    draw.rectangle([x0, y0, x0 + size - 1, y0 + cap_height - 1], fill=palette['cap'])
    
    # Add white spots
    spot_count = size // 3
    for _ in range(spot_count):
        sx = random.randint(x0 + 1, x0 + size - 2)
        sy = random.randint(y0 + 1, y0 + cap_height - 2)
        spot_size = random.randint(1, max(1, size // 6))
        
        draw.ellipse([
            sx - spot_size//2, sy - spot_size//2,
            sx + spot_size//2, sy + spot_size//2
        ], fill=palette['spots'])
    
    # Mushroom stem
    stem_width = max(1, size // 3)
    stem_start_x = x0 + (size - stem_width) // 2
    draw.rectangle([
        stem_start_x, y0 + cap_height,
        stem_start_x + stem_width - 1, y0 + size - 1
    ], fill=palette['stem'])

def generate_cactus(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Cactus with spines and desert coloring."""
    palette = {
        'base': (80, 120, 60, 255),
        'dark': (60, 100, 40, 255),
        'light': (100, 140, 80, 255),
        'spine': (240, 240, 180, 255)
    }
    
    # Base cactus color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add texture variation
    for _ in range(size * size // 8):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        texture_color = random.choice([palette['dark'], palette['light']])
        draw.point((tx, ty), fill=texture_color)
    
    # Add spines
    spine_count = size // 2
    for _ in range(spine_count):
        spine_x = random.randint(x0, x0 + size - 1)
        spine_y = random.randint(y0, y0 + size - 1)
        draw.point((spine_x, spine_y), fill=palette['spine'])

# Lookup table for organic generators
ORGANIC_GENERATORS = {
    34: generate_oak_leaves,      # LEAVES_OAK (legacy LEAVES at new position)
    35: generate_pine_leaves,     # LEAVES_PINE
    36: generate_birch_leaves,    # LEAVES_BIRCH
    37: generate_palm_leaves,     # LEAVES_PALM
    38: generate_brown_mushroom,  # MUSHROOM_BROWN
    39: generate_red_mushroom,    # MUSHROOM_RED
    43: generate_cactus,          # CACTUS
}
