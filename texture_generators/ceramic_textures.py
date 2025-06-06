"""
Ceramic Texture Generators
==========================

Generators for clay, brick, tile, and ceramic materials.
Each ceramic type has unique surface texture and firing characteristics.
Designed for 25cm×25cm voxel scale for enhanced detail.
"""

from PIL import ImageDraw
import random
from texture_generators.base_patterns import draw_brick_pattern, draw_speckled_pattern
from texture_generators.color_palettes import get_palette, vary_color

def generate_clay_brick(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Traditional fired clay brick with mortar lines and texture."""
    palette = {
        'brick': (150, 80, 60, 255),      # Traditional brick red
        'mortar': (200, 190, 180, 255),   # Light gray mortar
        'light': (180, 110, 90, 255),     # Light brick areas
        'dark': (120, 60, 40, 255)        # Dark brick areas
    }
    
    # Create brick pattern
    brick_mask, mortar_mask = draw_brick_pattern(
        size, size, 
        brick_width=size//2 - 1,
        brick_height=size//4,
        mortar_width=1
    )
    
    # Fill base with mortar color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['mortar'])
    
    # Draw brick areas
    for y in range(size):
        for x in range(size):
            if y < brick_mask.shape[0] and x < brick_mask.shape[1] and brick_mask[y, x] > 0:
                # Add brick texture variation
                brick_color = palette['brick']
                if random.random() < 0.2:
                    brick_color = palette['light']
                elif random.random() < 0.3:
                    brick_color = palette['dark']
                
                draw.point((x0 + x, y0 + y), fill=brick_color)
    
    # Add surface texture to bricks
    for _ in range(size * size // 12):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        
        # Only add texture to brick areas (not mortar)
        local_x = tx - x0
        local_y = ty - y0
        if (local_y < brick_mask.shape[0] and local_x < brick_mask.shape[1] and 
            brick_mask[local_y, local_x] > 0):
            texture_color = vary_color(palette['brick'], random.randint(-20, 20))
            draw.point((tx, ty), fill=texture_color)

def generate_terracotta(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Terracotta with earthy orange-brown color and natural texture."""
    palette = {
        'base': (180, 120, 80, 255),      # Terracotta orange
        'light': (200, 140, 100, 255),    # Light terracotta
        'dark': (150, 100, 60, 255),      # Dark terracotta
        'earth': (160, 110, 70, 255)      # Earthy tones
    }
    
    # Base terracotta color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add natural clay texture
    draw_speckled_pattern(draw, x0, y0, size, palette, density=6, variation=15)
    
    # Add firing marks and color variations
    for _ in range(size * size // 10):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((tx, ty), fill=palette['light'])
        elif random.random() < 0.6:
            draw.point((tx, ty), fill=palette['dark'])
        else:
            draw.point((tx, ty), fill=palette['earth'])

def generate_glazed_tile_white(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """White glazed tile with smooth, reflective surface."""
    palette = {
        'base': (240, 240, 245, 255),     # Off-white
        'light': (255, 255, 255, 255),    # Pure white highlights
        'dark': (220, 220, 225, 255),     # Slight shadows
        'reflection': (250, 250, 255, 200) # Glossy reflections
    }
    
    # Base tile color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add subtle glazed surface variations
    for _ in range(size * size // 15):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((gx, gy), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((gx, gy), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((gx, gy), fill=palette['reflection'])

def generate_glazed_tile_red(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Red glazed tile with glossy ceramic surface."""
    palette = {
        'base': (180, 60, 60, 255),       # Rich red
        'light': (220, 100, 100, 255),    # Light red
        'dark': (140, 40, 40, 255),       # Dark red
        'gloss': (240, 120, 120, 180)     # Glossy highlights
    }
    
    # Base tile color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add glazed surface texture
    for _ in range(size * size // 12):
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((rx, ry), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((rx, ry), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((rx, ry), fill=palette['gloss'])

def generate_glazed_tile_blue(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Blue glazed tile with deep ceramic coloring."""
    palette = {
        'base': (60, 80, 180, 255),       # Deep blue
        'light': (100, 120, 220, 255),    # Light blue
        'dark': (40, 60, 140, 255),       # Dark blue
        'azure': (80, 140, 200, 180)      # Azure highlights
    }
    
    # Base tile color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add glazed surface texture
    for _ in range(size * size // 12):
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((bx, by), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((bx, by), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((bx, by), fill=palette['azure'])

def generate_glazed_tile_green(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Green glazed tile with natural ceramic finish."""
    palette = {
        'base': (80, 140, 80, 255),       # Forest green
        'light': (120, 180, 120, 255),    # Light green
        'dark': (60, 100, 60, 255),       # Dark green
        'jade': (100, 160, 100, 180)      # Jade-like highlights
    }
    
    # Base tile color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add glazed surface texture
    for _ in range(size * size // 12):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((gx, gy), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((gx, gy), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((gx, gy), fill=palette['jade'])

def generate_porcelain(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Fine porcelain with smooth, translucent-looking surface."""
    palette = {
        'base': (245, 245, 250, 255),     # Very light, almost white
        'light': (255, 255, 255, 255),    # Pure white
        'shadow': (235, 235, 240, 255),   # Very subtle shadows
        'translucent': (250, 250, 255, 200) # Translucent quality
    }
    
    # Base porcelain color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add very subtle surface variations
    for _ in range(size * size // 20):
        px = random.randint(x0, x0 + size - 1)
        py = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((px, py), fill=palette['light'])
        elif random.random() < 0.6:
            draw.point((px, py), fill=palette['shadow'])
        elif random.random() < 0.8:
            draw.point((px, py), fill=palette['translucent'])

def generate_stoneware(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Stoneware with sturdy, matte ceramic finish."""
    palette = {
        'base': (120, 110, 100, 255),     # Gray-brown stoneware
        'light': (140, 130, 120, 255),    # Light areas
        'dark': (100, 90, 80, 255),       # Dark areas
        'speckle': (110, 100, 90, 255)    # Natural speckles
    }
    
    # Base stoneware color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add natural stoneware texture
    draw_speckled_pattern(draw, x0, y0, size, palette, density=8, variation=10)
    
    # Add additional texture variations
    for _ in range(size * size // 10):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((sx, sy), fill=palette['light'])
        elif random.random() < 0.6:
            draw.point((sx, sy), fill=palette['dark'])
        else:
            draw.point((sx, sy), fill=palette['speckle'])

def generate_earthenware(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Earthenware with rustic, unglazed ceramic finish."""
    palette = {
        'base': (140, 100, 80, 255),      # Earthy brown
        'light': (160, 120, 100, 255),    # Light earth
        'dark': (120, 80, 60, 255),       # Dark earth
        'rough': (130, 90, 70, 255)       # Rough texture
    }
    
    # Base earthenware color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add rough, unglazed texture
    draw_speckled_pattern(draw, x0, y0, size, palette, density=5, variation=20)
    
    # Add rustic surface variations
    for _ in range(size * size // 8):
        ex = random.randint(x0, x0 + size - 1)
        ey = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((ex, ey), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((ex, ey), fill=palette['dark'])
        else:
            draw.point((ex, ey), fill=palette['rough'])

def generate_ceramic_tile(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """General ceramic tile with clean, modern finish."""
    palette = {
        'base': (200, 200, 205, 255),     # Light ceramic gray
        'light': (220, 220, 225, 255),    # Light highlights
        'dark': (180, 180, 185, 255),     # Darker areas
        'edge': (240, 240, 245, 255)      # Edge highlights
    }
    
    # Base tile color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add clean ceramic surface
    for _ in range(size * size // 15):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((tx, ty), fill=palette['light'])
        elif random.random() < 0.5:
            draw.point((tx, ty), fill=palette['dark'])
        elif random.random() < 0.7:
            draw.point((tx, ty), fill=palette['edge'])

# Lookup table for ceramic generators
CERAMIC_GENERATORS = {
    90: generate_clay_brick,        # CLAY_BRICK
    91: generate_terracotta,        # TERRACOTTA
    92: generate_glazed_tile_white, # GLAZED_TILE_WHITE
    93: generate_glazed_tile_red,   # GLAZED_TILE_RED
    94: generate_glazed_tile_blue,  # GLAZED_TILE_BLUE
    95: generate_glazed_tile_green, # GLAZED_TILE_GREEN
    96: generate_porcelain,         # PORCELAIN
    97: generate_stoneware,         # STONEWARE
    98: generate_earthenware,       # EARTHENWARE
    99: generate_ceramic_tile,      # CERAMIC_TILE
}
