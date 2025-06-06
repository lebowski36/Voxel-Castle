"""
Ore Texture Generators
=====================

Generators for ore blocks using stone base with mineral overlays.
"""

from PIL import ImageDraw
import random
from texture_generators.base_patterns import draw_speckled_pattern, draw_crystalline_pattern, draw_vein_pattern
from texture_generators.color_palettes import get_palette

def generate_coal_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Coal ore with dark coal chunks in stone."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Coal chunks
    coal_palette = get_palette('coal_ore')
    for _ in range(size // 3):
        cx = random.randint(x0, x0 + size - 2)
        cy = random.randint(y0, y0 + size - 2)
        chunk_size = random.randint(1, max(1, size // 4))
        
        # Draw coal chunk
        draw.rectangle([
            cx, cy, 
            min(x0 + size - 1, cx + chunk_size),
            min(y0 + size - 1, cy + chunk_size)
        ], fill=coal_palette['coal'])
        
        # Add some shine
        if random.random() < 0.3:
            shine_x = cx + chunk_size // 2
            shine_y = cy + chunk_size // 2
            if x0 <= shine_x < x0 + size and y0 <= shine_y < y0 + size:
                draw.point((shine_x, shine_y), fill=coal_palette['coal_shine'])

def generate_iron_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Iron ore with rusty iron deposits in stone."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Iron deposits
    iron_palette = get_palette('iron_ore')
    for _ in range(size // 2):
        ix = random.randint(x0, x0 + size - 1)
        iy = random.randint(y0, y0 + size - 1)
        
        # Iron color varies between metallic and rusty
        iron_color = random.choice([iron_palette['iron'], iron_palette['iron_rust']])
        draw.point((ix, iy), fill=iron_color)
        
        # Sometimes add small iron clusters
        if random.random() < 0.2:
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    px = max(x0, min(x0 + size - 1, ix + dx))
                    py = max(y0, min(y0 + size - 1, iy + dy))
                    if random.random() < 0.5:
                        draw.point((px, py), fill=iron_color)

def generate_copper_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Copper ore with green oxidation and copper veins."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Copper veins
    copper_palette = get_palette('copper_ore')
    vein_palette = {
        'base': copper_palette['stone_base'],
        'vein_dark': copper_palette['copper']
    }
    draw_vein_pattern(draw, x0, y0, size, vein_palette, vein_count=1)
    
    # Add copper oxidation spots
    for _ in range(size // 4):
        ox = random.randint(x0, x0 + size - 1)
        oy = random.randint(y0, y0 + size - 1)
        draw.point((ox, oy), fill=copper_palette['copper_oxide'])

def generate_gold_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Gold ore with bright gold flakes in stone."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Gold flakes
    gold_palette = get_palette('gold_ore')
    for _ in range(size // 3):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        gold_color = random.choice([gold_palette['gold'], gold_palette['gold_bright']])
        draw.point((gx, gy), fill=gold_color)
        
        # Sometimes add small gold clusters
        if random.random() < 0.3:
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    if random.random() < 0.4:
                        px = max(x0, min(x0 + size - 1, gx + dx))
                        py = max(y0, min(y0 + size - 1, gy + dy))
                        draw.point((px, py), fill=gold_color)

def generate_silver_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Silver ore with metallic silver deposits."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Silver deposits
    silver_palette = {
        'base': (192, 192, 192, 255),
        'bright': (220, 220, 220, 255)
    }
    
    for _ in range(size // 3):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        silver_color = random.choice([silver_palette['base'], silver_palette['bright']])
        draw.point((sx, sy), fill=silver_color)

def generate_tin_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Tin ore with dull metallic deposits."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Tin deposits
    tin_palette = {
        'base': (160, 160, 160, 255),
        'dull': (140, 140, 140, 255)
    }
    
    for _ in range(size // 4):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        
        tin_color = random.choice([tin_palette['base'], tin_palette['dull']])
        draw.point((tx, ty), fill=tin_color)

def generate_ruby_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Ruby with crystalline red structure."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Ruby crystals
    ruby_palette = {
        'base': (220, 20, 60, 255),
        'edge': (180, 15, 45, 255),
        'shine': (255, 100, 120, 255)
    }
    draw_crystalline_pattern(draw, x0, y0, size, ruby_palette, crystal_count=2)

def generate_sapphire_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Sapphire with crystalline blue structure."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Sapphire crystals
    sapphire_palette = {
        'base': (65, 105, 225, 255),
        'edge': (45, 75, 180, 255),
        'shine': (120, 150, 255, 255)
    }
    draw_crystalline_pattern(draw, x0, y0, size, sapphire_palette, crystal_count=2)

def generate_emerald_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Emerald with crystalline green structure."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Emerald crystals
    emerald_palette = {
        'base': (50, 205, 50, 255),
        'edge': (35, 150, 35, 255),
        'shine': (100, 255, 100, 255)
    }
    draw_crystalline_pattern(draw, x0, y0, size, emerald_palette, crystal_count=2)

def generate_diamond_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Diamond with brilliant crystalline structure."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Diamond crystals
    diamond_palette = {
        'base': (225, 242, 255, 200),
        'edge': (200, 220, 240, 255),
        'shine': (255, 255, 255, 255)
    }
    draw_crystalline_pattern(draw, x0, y0, size, diamond_palette, crystal_count=1)

# Lookup table for ore generators
ORE_GENERATORS = {
    20: generate_coal_ore,        # COAL_ORE
    21: generate_iron_ore,        # IRON_ORE
    22: generate_copper_ore,      # COPPER_ORE
    23: generate_tin_ore,         # TIN_ORE
    24: generate_silver_ore,      # SILVER_ORE
    25: generate_gold_ore,        # GOLD_ORE
    26: generate_ruby_gem,        # GEM_RUBY
    27: generate_sapphire_gem,    # GEM_SAPPHIRE
    28: generate_emerald_gem,     # GEM_EMERALD
    29: generate_diamond_gem,     # GEM_DIAMOND
}
