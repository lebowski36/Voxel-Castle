"""
Metal Texture Generators
========================

Generators for processed metal blocks (iron, copper, bronze, steel, etc.).
Each metal has unique surface properties and coloring.
Designed for 25cm×25cm voxel scale for enhanced detail.
"""

from PIL import ImageDraw
import random
from texture_generators.base_patterns import draw_speckled_pattern
from texture_generators.color_palettes import get_palette, vary_color

def generate_iron_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Iron block with metallic gray surface and subtle reflections."""
    palette = {
        'base': (140, 140, 150, 255),     # Metallic gray
        'light': (180, 180, 190, 255),    # Light reflections
        'dark': (100, 100, 110, 255),     # Darker areas
        'shine': (220, 220, 230, 200)     # Bright highlights
    }
    
    # Base iron color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add metallic surface texture
    for _ in range(size * size // 8):
        mx = random.randint(x0, x0 + size - 1)
        my = random.randint(y0, y0 + size - 1)
        
        # Surface variations
        if random.random() < 0.3:
            draw.point((mx, my), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((mx, my), fill=palette['dark'])
    
    # Add subtle shine lines (metallic reflections)
    for _ in range(size // 4):
        # Random shine line
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        line_length = random.randint(2, size // 3)
        
        # Horizontal or vertical shine lines
        if random.choice([True, False]):
            # Horizontal
            for i in range(line_length):
                px = max(x0, min(x0 + size - 1, sx + i))
                if x0 <= px < x0 + size and y0 <= sy < y0 + size:
                    draw.point((px, sy), fill=palette['shine'])
        else:
            # Vertical
            for i in range(line_length):
                py = max(y0, min(y0 + size - 1, sy + i))
                if x0 <= sx < x0 + size and y0 <= py < y0 + size:
                    draw.point((sx, py), fill=palette['shine'])

def generate_copper_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Copper block with reddish-brown metallic surface."""
    palette = {
        'base': (180, 120, 80, 255),      # Copper brown
        'light': (220, 160, 120, 255),    # Light copper
        'dark': (140, 90, 60, 255),       # Dark copper
        'patina': (100, 140, 120, 180)    # Green patina spots
    }
    
    # Base copper color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add metallic surface texture
    for _ in range(size * size // 6):
        cx = random.randint(x0, x0 + size - 1)
        cy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((cx, cy), fill=palette['light'])
        elif random.random() < 0.6:
            draw.point((cx, cy), fill=palette['dark'])
    
    # Add occasional patina spots (oxidation)
    for _ in range(size // 6):
        px = random.randint(x0, x0 + size - 3)
        py = random.randint(y0, y0 + size - 3)
        patina_size = random.randint(1, max(1, size // 8))
        
        draw.ellipse([
            px - patina_size//2, py - patina_size//2,
            px + patina_size//2, py + patina_size//2
        ], fill=palette['patina'])

def generate_bronze_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Bronze block (copper + tin alloy) with golden-brown surface."""
    palette = {
        'base': (160, 130, 70, 255),      # Bronze golden-brown
        'light': (200, 170, 110, 255),    # Light bronze
        'dark': (120, 100, 50, 255),      # Dark bronze
        'shine': (240, 200, 140, 200)     # Golden highlights
    }
    
    # Base bronze color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add metallic surface texture
    for _ in range(size * size // 7):
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((bx, by), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((bx, by), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((bx, by), fill=palette['shine'])

def generate_steel_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Steel block with clean, polished metallic surface."""
    palette = {
        'base': (160, 160, 170, 255),     # Steel gray
        'light': (200, 200, 210, 255),    # Light steel
        'dark': (120, 120, 130, 255),     # Dark steel
        'mirror': (240, 240, 250, 180)    # Mirror-like reflections
    }
    
    # Base steel color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add polished surface texture
    for _ in range(size * size // 10):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.2:
            draw.point((sx, sy), fill=palette['light'])
        elif random.random() < 0.3:
            draw.point((sx, sy), fill=palette['dark'])
    
    # Add mirror-like reflection lines
    for _ in range(size // 6):
        # Random reflection line
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        line_length = random.randint(3, size // 2)
        
        # Diagonal reflection lines
        for i in range(line_length):
            px = max(x0, min(x0 + size - 1, rx + i))
            py = max(y0, min(y0 + size - 1, ry + i))
            if x0 <= px < x0 + size and y0 <= py < y0 + size:
                draw.point((px, py), fill=palette['mirror'])

def generate_silver_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Silver block with bright, reflective metallic surface."""
    palette = {
        'base': (200, 200, 210, 255),     # Bright silver
        'light': (240, 240, 250, 255),    # Bright highlights
        'dark': (160, 160, 170, 255),     # Darker silver
        'shine': (255, 255, 255, 200)     # Pure white highlights
    }
    
    # Base silver color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add highly reflective surface
    for _ in range(size * size // 6):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((sx, sy), fill=palette['light'])
        elif random.random() < 0.6:
            draw.point((sx, sy), fill=palette['dark'])
        elif random.random() < 0.8:
            draw.point((sx, sy), fill=palette['shine'])

def generate_gold_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Gold block with rich, warm metallic surface."""
    palette = {
        'base': (200, 160, 60, 255),      # Rich gold
        'light': (240, 200, 100, 255),    # Bright gold
        'dark': (160, 120, 40, 255),      # Deep gold
        'shine': (255, 220, 120, 220)     # Golden shine
    }
    
    # Base gold color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add rich metallic surface
    for _ in range(size * size // 5):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((gx, gy), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((gx, gy), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((gx, gy), fill=palette['shine'])

def generate_brass_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Brass block (copper + zinc alloy) with yellowish metallic surface."""
    palette = {
        'base': (180, 150, 80, 255),      # Brass yellow
        'light': (220, 190, 120, 255),    # Light brass
        'dark': (140, 110, 60, 255),      # Dark brass
        'tarnish': (160, 140, 90, 200)    # Slight tarnishing
    }
    
    # Base brass color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add metallic surface texture
    for _ in range(size * size // 7):
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((bx, by), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((bx, by), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((bx, by), fill=palette['tarnish'])

# Advanced/Magical metals
def generate_mithril_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Mithril block with silvery-blue magical metallic surface."""
    palette = {
        'base': (180, 190, 220, 255),     # Silvery-blue
        'light': (220, 230, 255, 255),    # Bright magical shine
        'dark': (140, 150, 180, 255),     # Darker mithril
        'magic': (200, 220, 255, 180)     # Magical glow
    }
    
    # Base mithril color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add magical metallic properties
    for _ in range(size * size // 5):
        mx = random.randint(x0, x0 + size - 1)
        my = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((mx, my), fill=palette['light'])
        elif random.random() < 0.6:
            draw.point((mx, my), fill=palette['dark'])
        elif random.random() < 0.8:
            draw.point((mx, my), fill=palette['magic'])

def generate_adamantine_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Adamantine block with incredibly hard, dark metallic surface."""
    palette = {
        'base': (60, 60, 80, 255),        # Very dark metallic
        'light': (100, 100, 120, 255),    # Lighter areas
        'dark': (40, 40, 50, 255),        # Darker areas
        'edge': (120, 120, 140, 255)      # Sharp edges
    }
    
    # Base adamantine color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add incredibly hard surface texture
    for _ in range(size * size // 8):
        ax = random.randint(x0, x0 + size - 1)
        ay = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.2:
            draw.point((ax, ay), fill=palette['light'])
        elif random.random() < 0.4:
            draw.point((ax, ay), fill=palette['dark'])
        elif random.random() < 0.6:
            draw.point((ax, ay), fill=palette['edge'])

# Lookup table for metal generators
METAL_GENERATORS = {
    80: generate_iron_block,        # IRON_BLOCK
    81: generate_copper_block,      # COPPER_BLOCK
    82: generate_bronze_block,      # BRONZE_BLOCK
    83: generate_steel_block,       # STEEL_BLOCK
    84: generate_silver_block,      # SILVER_BLOCK
    85: generate_gold_block,        # GOLD_BLOCK
    86: generate_brass_block,       # BRASS_BLOCK
    # 87: pewter_block - similar to silver but duller
    88: generate_mithril_block,     # MITHRIL_BLOCK
    89: generate_adamantine_block,  # ADAMANTINE_BLOCK
}
