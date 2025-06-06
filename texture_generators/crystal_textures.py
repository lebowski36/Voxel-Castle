"""
Crystal Texture Generators
==========================

Generators for magical crystals, gems, and special crystalline materials.
Each crystal type has unique faceting, colors, and magical properties.
Designed for 25cm×25cm voxel scale for enhanced detail.
"""

from PIL import ImageDraw
import random
from texture_generators.base_patterns import draw_crystalline_pattern, draw_speckled_pattern
from texture_generators.color_palettes import get_palette, vary_color

def generate_crystal_clear(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Clear crystal with transparent, refractive properties."""
    palette = {
        'base': (240, 240, 255, 150),     # Very transparent
        'edge': (200, 200, 220, 200),     # Crystal edges
        'shine': (255, 255, 255, 120),    # Bright reflections
        'refraction': (220, 220, 240, 100) # Light refraction
    }
    
    # Base transparent crystal
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add crystalline structure
    draw_crystalline_pattern(draw, x0, y0, size, palette, crystal_count=4)
    
    # Add internal refractions
    for _ in range(size * size // 8):
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((rx, ry), fill=palette['shine'])
        elif random.random() < 0.7:
            draw.point((rx, ry), fill=palette['refraction'])

def generate_crystal_blue(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Blue crystal with deep sapphire-like coloring."""
    palette = {
        'base': (60, 120, 200, 180),      # Deep blue crystal
        'edge': (40, 80, 160, 220),       # Darker edges
        'shine': (120, 180, 255, 150),    # Blue highlights
        'deep': (40, 60, 140, 200)        # Deep blue core
    }
    
    # Base blue crystal
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add crystalline structure
    draw_crystalline_pattern(draw, x0, y0, size, palette, crystal_count=3)
    
    # Add blue crystal variations
    for _ in range(size * size // 10):
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((bx, by), fill=palette['shine'])
        elif random.random() < 0.6:
            draw.point((bx, by), fill=palette['deep'])

def generate_crystal_red(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Red crystal with ruby-like properties."""
    palette = {
        'base': (180, 60, 80, 200),       # Deep red crystal
        'edge': (140, 40, 60, 240),       # Dark red edges
        'shine': (255, 120, 140, 160),    # Red highlights
        'fire': (220, 80, 100, 180)       # Fiery core
    }
    
    # Base red crystal
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add crystalline structure
    draw_crystalline_pattern(draw, x0, y0, size, palette, crystal_count=3)
    
    # Add red crystal fire
    for _ in range(size * size // 8):
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((rx, ry), fill=palette['shine'])
        elif random.random() < 0.7:
            draw.point((rx, ry), fill=palette['fire'])

def generate_crystal_green(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Green crystal with emerald-like brilliance."""
    palette = {
        'base': (60, 160, 80, 190),       # Emerald green
        'edge': (40, 120, 60, 230),       # Dark green edges
        'shine': (120, 220, 140, 140),    # Bright green highlights
        'forest': (80, 140, 100, 180)     # Forest green depths
    }
    
    # Base green crystal
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add crystalline structure
    draw_crystalline_pattern(draw, x0, y0, size, palette, crystal_count=4)
    
    # Add emerald-like variations
    for _ in range(size * size // 9):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((gx, gy), fill=palette['shine'])
        elif random.random() < 0.6:
            draw.point((gx, gy), fill=palette['forest'])

def generate_enchanted_stone(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Enchanted stone with magical runes and glowing properties."""
    palette = {
        'base': (100, 100, 120, 255),     # Dark magical stone
        'light': (140, 140, 160, 255),    # Lighter stone
        'rune': (180, 120, 200, 200),     # Magical rune glow
        'magic': (200, 150, 255, 150)     # Magical energy
    }
    
    # Base enchanted stone
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add stone texture
    draw_speckled_pattern(draw, x0, y0, size, palette, density=8, variation=15)
    
    # Add magical runes and energy
    rune_count = size // 8
    for _ in range(rune_count):
        rx = random.randint(x0 + 1, x0 + size - 2)
        ry = random.randint(y0 + 1, y0 + size - 2)
        rune_size = random.randint(1, max(1, size // 6))
        
        # Draw simple rune shapes
        if random.choice([True, False]):
            # Horizontal rune line
            for i in range(rune_size):
                if rx + i < x0 + size:
                    draw.point((rx + i, ry), fill=palette['rune'])
        else:
            # Vertical rune line
            for i in range(rune_size):
                if ry + i < y0 + size:
                    draw.point((rx, ry + i), fill=palette['rune'])
    
    # Add magical energy sparkles
    for _ in range(size * size // 15):
        mx = random.randint(x0, x0 + size - 1)
        my = random.randint(y0, y0 + size - 1)
        draw.point((mx, my), fill=palette['magic'])

def generate_runic_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Stone block with carved runes and magical inscriptions."""
    palette = {
        'base': (120, 110, 100, 255),     # Stone base
        'carved': (80, 70, 60, 255),      # Carved areas
        'rune_glow': (150, 100, 180, 180), # Glowing runes
        'magic': (180, 120, 200, 150)     # Magical emanation
    }
    
    # Base stone
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add stone texture
    draw_speckled_pattern(draw, x0, y0, size, palette, density=10, variation=10)
    
    # Add carved runic patterns
    pattern_count = size // 6
    for _ in range(pattern_count):
        px = random.randint(x0, x0 + size - 3)
        py = random.randint(y0, y0 + size - 3)
        pattern_size = random.randint(2, max(2, size // 4))
        
        # Draw runic pattern (simple geometric shapes)
        pattern_type = random.randint(1, 3)
        
        if pattern_type == 1:
            # Square rune
            draw.rectangle([px, py, px + pattern_size - 1, py + pattern_size - 1], 
                         outline=palette['carved'])
        elif pattern_type == 2:
            # Cross rune
            mid_x = px + pattern_size // 2
            mid_y = py + pattern_size // 2
            # Horizontal line
            for i in range(pattern_size):
                if px + i < x0 + size:
                    draw.point((px + i, mid_y), fill=palette['carved'])
            # Vertical line
            for i in range(pattern_size):
                if py + i < y0 + size:
                    draw.point((mid_x, py + i), fill=palette['carved'])
        else:
            # Circle rune
            center_x = px + pattern_size // 2
            center_y = py + pattern_size // 2
            radius = pattern_size // 2
            draw.ellipse([
                center_x - radius, center_y - radius,
                center_x + radius, center_y + radius
            ], outline=palette['carved'])
    
    # Add magical glow to some runes
    for _ in range(size // 10):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        if random.random() < 0.7:
            draw.point((gx, gy), fill=palette['rune_glow'])
        else:
            draw.point((gx, gy), fill=palette['magic'])

def generate_ether_crystal(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Ethereal crystal with otherworldly properties."""
    palette = {
        'base': (180, 200, 255, 120),     # Ethereal blue-white
        'edge': (150, 180, 240, 160),     # Ethereal edges
        'glow': (200, 220, 255, 100),     # Soft glow
        'void': (100, 120, 200, 80)       # Void-like depths
    }
    
    # Base ethereal crystal
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add ethereal crystal structure
    draw_crystalline_pattern(draw, x0, y0, size, palette, crystal_count=5)
    
    # Add ethereal glow effects
    for _ in range(size * size // 6):
        ex = random.randint(x0, x0 + size - 1)
        ey = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.5:
            draw.point((ex, ey), fill=palette['glow'])
        elif random.random() < 0.8:
            draw.point((ex, ey), fill=palette['void'])

def generate_void_stone(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Dark void stone that seems to absorb light."""
    palette = {
        'base': (20, 15, 25, 255),        # Very dark purple-black
        'void': (10, 5, 15, 255),         # Deeper void
        'edge': (40, 30, 50, 255),        # Slightly lighter edges
        'anti_glow': (30, 20, 40, 200)    # Anti-light effect
    }
    
    # Base void stone
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add void patterns
    for _ in range(size * size // 5):
        vx = random.randint(x0, x0 + size - 1)
        vy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.6:
            draw.point((vx, vy), fill=palette['void'])
        elif random.random() < 0.8:
            draw.point((vx, vy), fill=palette['anti_glow'])
        else:
            draw.point((vx, vy), fill=palette['edge'])

def generate_celestial_marble(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Celestial marble with starlike inclusions."""
    palette = {
        'base': (200, 190, 220, 255),     # Light marble
        'vein': (160, 150, 180, 255),     # Marble veins
        'star': (255, 255, 255, 200),     # Star-like sparkles
        'celestial': (180, 200, 255, 150) # Celestial blue
    }
    
    # Base celestial marble
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add marble veining
    vein_count = size // 4
    for _ in range(vein_count):
        # Random vein path
        start_x = random.randint(x0, x0 + size - 1)
        start_y = random.randint(y0, y0 + size - 1)
        
        vein_length = random.randint(size // 3, size)
        for i in range(vein_length):
            vx = start_x + random.randint(-2, 2)
            vy = start_y + i // 2 + random.randint(-1, 1)
            
            if x0 <= vx < x0 + size and y0 <= vy < y0 + size:
                draw.point((vx, vy), fill=palette['vein'])
    
    # Add celestial stars
    star_count = size // 6
    for _ in range(star_count):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        # Star sparkle
        draw.point((sx, sy), fill=palette['star'])
        
        # Celestial glow around star
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if x0 <= sx + dx < x0 + size and y0 <= sy + dy < y0 + size:
                    if random.random() < 0.3:
                        draw.point((sx + dx, sy + dy), fill=palette['celestial'])

def generate_shadow_glass(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Dark, smoky glass with shadow-like properties."""
    palette = {
        'base': (60, 50, 70, 180),        # Dark smoky glass
        'shadow': (40, 30, 50, 200),      # Deeper shadows
        'light': (100, 80, 120, 150),     # Reflected light
        'smoke': (70, 60, 80, 160)        # Smoky patterns
    }
    
    # Base shadow glass
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add smoky patterns
    for _ in range(size * size // 7):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((gx, gy), fill=palette['shadow'])
        elif random.random() < 0.6:
            draw.point((gx, gy), fill=palette['smoke'])
        elif random.random() < 0.8:
            draw.point((gx, gy), fill=palette['light'])

# Lookup table for crystal generators
CRYSTAL_GENERATORS = {
    150: generate_crystal_clear,    # CRYSTAL_CLEAR
    151: generate_crystal_blue,     # CRYSTAL_BLUE
    152: generate_crystal_red,      # CRYSTAL_RED
    153: generate_crystal_green,    # CRYSTAL_GREEN
    154: generate_enchanted_stone,  # ENCHANTED_STONE
    155: generate_runic_block,      # RUNIC_BLOCK
    156: generate_ether_crystal,    # ETHER_CRYSTAL
    157: generate_void_stone,       # VOID_STONE
    158: generate_celestial_marble, # CELESTIAL_MARBLE
    159: generate_shadow_glass,     # SHADOW_GLASS
}

def generate_crystal_texture(crystal_type: str, size: int = 32):
    """Generate crystal texture and return PIL Image."""
    from PIL import Image, ImageDraw
    
    # Create RGBA image for transparency support
    image = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Generate based on crystal type
    if crystal_type == 'clear':
        generate_crystal_clear(draw, 0, 0, size)
    elif crystal_type == 'blue':
        generate_crystal_blue(draw, 0, 0, size)
    elif crystal_type == 'red':
        generate_crystal_red(draw, 0, 0, size)
    elif crystal_type == 'green':
        generate_crystal_green(draw, 0, 0, size)
    else:
        # Default to clear crystal
        generate_crystal_clear(draw, 0, 0, size)
    
    return image
