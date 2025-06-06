"""
Fluid Texture Generators
========================

Generators for water, lava, and other fluid materials.
Each fluid type has unique flow patterns and characteristics.
Designed for 25cm×25cm voxel scale for enhanced detail.
"""

from PIL import ImageDraw
import random
from texture_generators.base_patterns import draw_fluid_pattern, draw_speckled_pattern
from texture_generators.color_palettes import get_palette, vary_color

def generate_water(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Clear water with subtle flow patterns and light refraction."""
    palette = {
        'base': (60, 120, 200, 180),      # Translucent blue
        'light': (120, 180, 255, 150),    # Light reflections
        'dark': (40, 80, 160, 200),       # Deeper water
        'foam': (200, 220, 255, 120)      # Surface foam/bubbles
    }
    
    # Base water color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add flow patterns
    draw_fluid_pattern(draw, x0, y0, size, palette, wave_count=3)
    
    # Add light reflections and ripples
    reflection_count = size // 4
    for _ in range(reflection_count):
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        
        # Light reflection spots
        if random.random() < 0.7:
            draw.point((rx, ry), fill=palette['light'])
        
        # Small bubbles/foam
        if random.random() < 0.3:
            bubble_size = random.randint(1, max(1, size // 8))
            draw.ellipse([
                rx - bubble_size//2, ry - bubble_size//2,
                rx + bubble_size//2, ry + bubble_size//2
            ], fill=palette['foam'])

def generate_lava(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Hot lava with glowing cracks and molten flow."""
    palette = {
        'base': (180, 60, 20, 255),       # Hot orange-red
        'hot': (255, 150, 0, 255),        # Bright molten areas
        'cool': (120, 40, 15, 255),       # Cooler/darker lava
        'glow': (255, 200, 50, 200)       # Bright glow
    }
    
    # Base lava color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add molten flow patterns
    draw_fluid_pattern(draw, x0, y0, size, palette, wave_count=4)
    
    # Add hot spots and cooling areas
    for _ in range(size * size // 6):
        lx = random.randint(x0, x0 + size - 1)
        ly = random.randint(y0, y0 + size - 1)
        
        # Hot glowing spots
        if random.random() < 0.4:
            glow_size = random.randint(1, max(1, size // 6))
            draw.ellipse([
                lx - glow_size//2, ly - glow_size//2,
                lx + glow_size//2, ly + glow_size//2
            ], fill=palette['hot'])
            
            # Bright center glow
            if glow_size > 2:
                center_size = max(1, glow_size // 3)
                draw.ellipse([
                    lx - center_size//2, ly - center_size//2,
                    lx + center_size//2, ly + center_size//2
                ], fill=palette['glow'])
        
        # Cooler spots
        elif random.random() < 0.6:
            draw.point((lx, ly), fill=palette['cool'])

def generate_oil(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Dark oil with iridescent surface reflections."""
    palette = {
        'base': (20, 15, 10, 255),        # Very dark brown/black
        'reflection': (60, 40, 80, 180),   # Purple-ish reflections
        'iridescent': (120, 80, 200, 120), # Iridescent sheen
        'dark': (10, 8, 5, 255)           # Deeper black
    }
    
    # Base oil color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add subtle flow patterns
    draw_fluid_pattern(draw, x0, y0, size, palette, wave_count=2)
    
    # Add iridescent surface effects
    for _ in range(size * size // 10):
        ox = random.randint(x0, x0 + size - 1)
        oy = random.randint(y0, y0 + size - 1)
        
        # Iridescent spots
        if random.random() < 0.3:
            draw.point((ox, oy), fill=palette['iridescent'])
        
        # Purple reflections
        elif random.random() < 0.5:
            draw.point((ox, oy), fill=palette['reflection'])
        
        # Deep dark spots
        else:
            draw.point((ox, oy), fill=palette['dark'])

def generate_acid(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Corrosive acid with bubbling and toxic green color."""
    palette = {
        'base': (120, 180, 60, 200),      # Toxic green
        'bubble': (180, 255, 120, 150),   # Bright bubbles
        'dark': (80, 120, 40, 255),       # Darker acid
        'foam': (200, 255, 150, 180)      # Foamy surface
    }
    
    # Base acid color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add bubbling patterns
    bubble_count = size * size // 4
    for _ in range(bubble_count):
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        # Various bubble sizes
        if random.random() < 0.4:
            bubble_size = random.randint(1, max(1, size // 4))
            draw.ellipse([
                bx - bubble_size//2, by - bubble_size//2,
                bx + bubble_size//2, by + bubble_size//2
            ], fill=palette['bubble'])
        
        # Foam patterns
        elif random.random() < 0.6:
            draw.point((bx, by), fill=palette['foam'])
        
        # Dark spots
        else:
            draw.point((bx, by), fill=palette['dark'])

def generate_honey(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Thick honey with golden flow patterns."""
    palette = {
        'base': (200, 150, 60, 220),      # Golden honey
        'light': (255, 200, 100, 180),    # Light golden areas
        'dark': (160, 110, 40, 255),      # Darker honey
        'amber': (220, 170, 80, 200)      # Amber highlights
    }
    
    # Base honey color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add thick flow patterns (slower, thicker than water)
    draw_fluid_pattern(draw, x0, y0, size, palette, wave_count=2)
    
    # Add amber-like variations
    for _ in range(size * size // 8):
        hx = random.randint(x0, x0 + size - 1)
        hy = random.randint(y0, y0 + size - 1)
        
        # Light spots
        if random.random() < 0.5:
            draw.point((hx, hy), fill=palette['light'])
        
        # Dark spots
        elif random.random() < 0.7:
            draw.point((hx, hy), fill=palette['dark'])
        
        # Amber highlights
        else:
            draw.point((hx, hy), fill=palette['amber'])

# Gas textures
def generate_steam(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Steam with wispy, translucent patterns."""
    palette = {
        'base': (240, 240, 255, 80),      # Very translucent white
        'dense': (220, 220, 240, 120),    # Denser steam
        'wisp': (255, 255, 255, 60)       # Light wisps
    }
    
    # Base steam color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add wispy patterns
    for _ in range(size * size // 3):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        # Random steam density
        if random.random() < 0.3:
            draw.point((sx, sy), fill=palette['dense'])
        elif random.random() < 0.6:
            draw.point((sx, sy), fill=palette['wisp'])

def generate_toxic_gas(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Toxic gas with sickly green color and swirling patterns."""
    palette = {
        'base': (100, 120, 60, 100),      # Sickly green, translucent
        'dense': (80, 100, 40, 150),      # Denser toxic areas
        'light': (120, 140, 80, 80)       # Lighter toxic wisps
    }
    
    # Base toxic color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add swirling toxic patterns
    for _ in range(size * size // 4):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        
        # Varying density
        if random.random() < 0.4:
            draw.point((tx, ty), fill=palette['dense'])
        elif random.random() < 0.7:
            draw.point((tx, ty), fill=palette['light'])

# Lookup table for fluid generators
FLUID_GENERATORS = {
    50: generate_water,       # WATER
    51: generate_lava,        # LAVA
    52: generate_oil,         # OIL
    53: generate_acid,        # ACID
    54: generate_honey,       # HONEY
    55: generate_steam,       # STEAM
    56: generate_toxic_gas,   # TOXIC_GAS
    # 57: natural_gas - similar to toxic_gas but lighter
    # 58: magical_mist - similar to steam but with magical colors
    # 59: smoke - similar to steam but darker
}
