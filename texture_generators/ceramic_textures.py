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
    """Traditional fired clay brick with mortar lines and texture - red/orange like sandstone bricks."""
    # Red/orange clay brick colors (like sandstone pattern but different colors)
    brick_color = (180, 70, 40, 255)    # Deep red clay brick
    mortar_color = (140, 120, 100, 255)  # Brown mortar
    
    # Fill with mortar base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=mortar_color)
    
    # Calculate brick layout (same pattern as sandstone bricks)
    mortar_thickness = max(1, size // 16)
    brick_height = max(3, size // 4)
    
    y_offset = y0
    row = 0
    while y_offset < y0 + size:
        brick_width = max(4, size // 2)
        
        # Offset every other row for proper brick pattern
        x_offset = (brick_width // 2) if row % 2 == 1 else 0
        
        x = x0 + x_offset
        while x < x0 + size:
            # Draw individual brick
            brick_x1 = x
            brick_y1 = y_offset + mortar_thickness
            brick_x2 = min(x + brick_width - mortar_thickness, x0 + size - 1)
            brick_y2 = min(y_offset + brick_height - mortar_thickness, y0 + size - 1)
            
            if brick_x2 > brick_x1 and brick_y2 > brick_y1:
                # Add slight color variation to each brick (deterministic)
                variation = 15 if (brick_x1 + brick_y1) % 3 == 0 else 0
                varied_brick_color = tuple(
                    max(0, min(255, c + variation)) for c in brick_color[:3]
                ) + (255,)
                draw.rectangle([brick_x1, brick_y1, brick_x2, brick_y2], 
                             fill=varied_brick_color)
            
            x += brick_width
        
        y_offset += brick_height
        row += 1

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

def generate_raw_clay(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Generate raw clay texture - smooth, natural earth tones."""
    # Clay color palette - natural earth tones
    base_clay = (139, 90, 60, 255)  # Rich brown clay
    light_clay = (165, 110, 80, 255)  # Lighter clay highlights
    dark_clay = (115, 70, 45, 255)   # Darker clay shadows
    
    # Fill base with primary clay color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_clay)
    
    # Add organic clay variations with smooth blending
    variation_size = max(2, size // 6)
    
    # Add natural clay color variations
    clay_variations = [
        (x0 + size // 4, y0 + size // 3, light_clay),
        (x0 + size // 2, y0 + size // 6, dark_clay),
        (x0 + 2 * size // 3, y0 + size // 2, light_clay),
        (x0 + size // 6, y0 + 2 * size // 3, dark_clay),
        (x0 + 3 * size // 4, y0 + 4 * size // 5, light_clay)
    ]
    
    for var_x, var_y, var_color in clay_variations:
        if var_x >= x0 and var_y >= y0 and var_x < x0 + size - variation_size and var_y < y0 + size - variation_size:
            # Create soft, organic shapes
            draw.ellipse([var_x, var_y, var_x + variation_size, var_y + variation_size], 
                        fill=var_color)
    
    # Add subtle clay texture with tiny particles
    particle_size = max(1, size // 16)
    clay_particles = [
        (x0 + size // 8, y0 + size // 4),
        (x0 + size // 3, y0 + size // 8),
        (x0 + size // 2, y0 + size // 3),
        (x0 + 2 * size // 3, y0 + 3 * size // 4),
        (x0 + 3 * size // 4, y0 + size // 6),
        (x0 + size // 6, y0 + 5 * size // 6)
    ]
    
    for part_x, part_y in clay_particles:
        if part_x >= x0 and part_y >= y0 and part_x < x0 + size - particle_size and part_y < y0 + size - particle_size:
            # Tiny clay particles for texture
            particle_color = dark_clay if (part_x + part_y) % 3 == 0 else light_clay
            draw.rectangle([part_x, part_y, part_x + particle_size, part_y + particle_size], 
                          fill=particle_color)

def generate_ceramic_texture(ceramic_type: str, size: int = 32):
    """Generate ceramic texture and return PIL Image."""
    from PIL import Image, ImageDraw
    
    # Create RGB image
    image = Image.new('RGB', (size, size), (0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Generate based on ceramic type
    if ceramic_type == 'clay':
        generate_raw_clay(draw, 0, 0, size)
    elif ceramic_type == 'clay_brick':
        generate_clay_brick(draw, 0, 0, size)
    elif ceramic_type == 'terracotta':
        generate_terracotta(draw, 0, 0, size)
    elif ceramic_type == 'glazed_white':
        generate_glazed_tile_white(draw, 0, 0, size)
    elif ceramic_type == 'glazed_red':
        generate_glazed_tile_red(draw, 0, 0, size)
    elif ceramic_type == 'glazed_blue':
        generate_glazed_tile_blue(draw, 0, 0, size)
    elif ceramic_type == 'glazed_green':
        generate_glazed_tile_green(draw, 0, 0, size)
    elif ceramic_type == 'porcelain':
        generate_porcelain(draw, 0, 0, size)
    elif ceramic_type == 'raw_clay':
        generate_raw_clay(draw, 0, 0, size)
    else:
        # Default to clay brick
        generate_clay_brick(draw, 0, 0, size)
    
    return image

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
