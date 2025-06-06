"""
Ore Texture Generators
=====================

Generators for ore blocks using stone base with mineral overlays.
"""

from PIL import ImageDraw
import random
import math
from texture_generators.base_patterns import draw_speckled_pattern, draw_crystalline_pattern, draw_vein_pattern
from texture_generators.color_palettes import get_palette
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
    """Iron ore with rusty iron deposits and veins in stone."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Iron veins and deposits
    iron_palette = get_palette('iron_ore')
    
    # Draw prominent iron veins
    vein_palette = {
        'base': stone_palette['base'],  # Use for vein pattern background (not actually used)
        'vein_dark': iron_palette['iron_rust']  # Use for the actual veins
    }
    
    # Draw 2-3 rusty iron veins
    for _ in range(random.randint(2, 3)):
        # Choose random start and end points
        start_x = x0 + random.randint(0, size // 4)
        start_y = y0 + random.randint(0, size - 1)
        end_x = x0 + random.randint(3 * size // 4, size - 1)
        end_y = y0 + random.randint(0, size - 1)
        
        # Draw curved vein
        steps = max(size // 2, 5)
        vein_width = random.randint(1, 2)
        
        for i in range(steps + 1):
            t = i / steps
            
            # Linear interpolation with some randomness for natural look
            vx = int(start_x + t * (end_x - start_x))
            vy = int(start_y + t * (end_y - start_y))
            
            # Add some curve variation
            variation = size // 8
            vx += random.randint(-variation, variation)
            vy += random.randint(-variation, variation)
            
            # Ensure we're in bounds
            vx = max(x0, min(x0 + size - 1, vx))
            vy = max(y0, min(y0 + size - 1, vy))
            
            # Draw vein segment
            for w in range(vein_width):
                wx = max(x0, min(x0 + size - 1, vx + w))
                wy = max(y0, min(y0 + size - 1, vy + w))
                
                # Iron color varies between metallic and rusty
                iron_color = random.choice([iron_palette['iron'], iron_palette['iron_rust']])
                draw.point((wx, wy), fill=iron_color)
                
                # Add some clusters around points
                if random.random() < 0.3:
                    for dx in [-1, 0, 1]:
                        for dy in [-1, 0, 1]:
                            px = max(x0, min(x0 + size - 1, wx + dx))
                            py = max(y0, min(y0 + size - 1, wy + dy))
                            if random.random() < 0.3:
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
    draw_vein_pattern(draw, x0, y0, size, vein_palette, vein_count=2)
    
    # Add copper oxidation spots
    for _ in range(size // 4):
        ox = random.randint(x0, x0 + size - 1)
        oy = random.randint(y0, y0 + size - 1)
        draw.point((ox, oy), fill=copper_palette['copper_oxide'])
        
        # Sometimes add a small oxidation cluster for more visibility
        if random.random() < 0.3:
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    if random.random() < 0.4:
                        px = max(x0, min(x0 + size - 1, ox + dx))
                        py = max(y0, min(y0 + size - 1, oy + dy))
                        draw.point((px, py), fill=copper_palette['copper_oxide'])

def generate_gold_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Gold ore with bright gold veins and flakes in stone."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Gold veins
    gold_palette = get_palette('gold_ore')
    vein_palette = {
        'base': stone_palette['base'],  # Use stone as background
        'vein_dark': gold_palette['gold']  # Gold veins
    }
    
    # Draw 1-2 gold veins
    for _ in range(random.randint(1, 2)):
        # Choose random start and end points
        start_x = x0 + random.randint(0, size // 4)
        start_y = y0 + random.randint(0, size - 1)
        end_x = x0 + random.randint(3 * size // 4, size - 1)
        end_y = y0 + random.randint(0, size - 1)
        
        # Draw curved vein
        steps = max(size // 2, 5)
        vein_width = random.randint(1, 2)
        
        for i in range(steps + 1):
            t = i / steps
            
            # Linear interpolation with some randomness for natural look
            vx = int(start_x + t * (end_x - start_x))
            vy = int(start_y + t * (end_y - start_y))
            
            # Add some curve variation
            variation = size // 8
            vx += random.randint(-variation, variation)
            vy += random.randint(-variation, variation)
            
            # Ensure we're in bounds
            vx = max(x0, min(x0 + size - 1, vx))
            vy = max(y0, min(y0 + size - 1, vy))
            
            # Draw vein segment
            for w in range(vein_width):
                wx = max(x0, min(x0 + size - 1, vx + w))
                wy = max(y0, min(y0 + size - 1, vy + w))
                
                # Gold color varies between standard and bright
                gold_color = random.choice([gold_palette['gold'], gold_palette['gold_bright']])
                draw.point((wx, wy), fill=gold_color)
                
                # Add some clusters around points
                if random.random() < 0.4:
                    for dx in [-1, 0, 1]:
                        for dy in [-1, 0, 1]:
                            px = max(x0, min(x0 + size - 1, wx + dx))
                            py = max(y0, min(y0 + size - 1, wy + dy))
                            if random.random() < 0.3:
                                draw.point((px, py), fill=gold_color)
    
    # Add additional gold flakes for sparkle
    for _ in range(size // 4):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        gold_color = random.choice([gold_palette['gold'], gold_palette['gold_bright']])
        draw.point((gx, gy), fill=gold_color)

def generate_silver_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Silver ore with metallic silver veins and deposits."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Silver deposits
    silver_palette = {
        'base': (192, 192, 192, 255),
        'bright': (220, 220, 220, 255),
        'dark': (160, 160, 170, 255)
    }
    
    # Draw silver veins
    vein_palette = {
        'base': stone_palette['base'],  # Use stone as background
        'vein_dark': silver_palette['base']  # Silver veins
    }
    
    # Draw 1-2 silver veins
    for _ in range(random.randint(1, 2)):
        # Choose random start and end points
        start_x = x0 + random.randint(0, size // 3)
        start_y = y0 + random.randint(0, size - 1)
        end_x = x0 + random.randint(2 * size // 3, size - 1)
        end_y = y0 + random.randint(0, size - 1)
        
        # Draw curved vein
        steps = max(size // 2, 5)
        
        for i in range(steps + 1):
            t = i / steps
            
            # Linear interpolation with some randomness for natural look
            vx = int(start_x + t * (end_x - start_x))
            vy = int(start_y + t * (end_y - start_y))
            
            # Add some curve variation
            variation = size // 8
            vx += random.randint(-variation, variation)
            vy += random.randint(-variation, variation)
            
            # Ensure we're in bounds
            vx = max(x0, min(x0 + size - 1, vx))
            vy = max(y0, min(y0 + size - 1, vy))
            
            # Draw vein segment with varying silver colors
            silver_color = random.choice([silver_palette['base'], silver_palette['bright']])
            draw.point((vx, vy), fill=silver_color)
            
            # Add some clusters around points
            if random.random() < 0.3:
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        px = max(x0, min(x0 + size - 1, vx + dx))
                        py = max(y0, min(y0 + size - 1, vy + dy))
                        if random.random() < 0.3:
                            silver_color = random.choice([silver_palette['base'], 
                                                       silver_palette['bright'],
                                                       silver_palette['dark']])
                            draw.point((px, py), fill=silver_color)
    
    # Add additional silver deposits
    for _ in range(size // 5):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        silver_color = random.choice([silver_palette['base'], silver_palette['bright']])
        draw.point((sx, sy), fill=silver_color)

def generate_tin_ore(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Tin ore with dull metallic veins and deposits."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=8, variation=20)
    
    # Tin deposits
    tin_palette = {
        'base': (160, 160, 160, 255),
        'dull': (140, 140, 140, 255),
        'light': (175, 175, 175, 255),
        'dark': (120, 120, 120, 255)
    }
    
    # Draw tin veins
    for _ in range(random.randint(1, 2)):
        # Create a branching vein pattern
        start_x = x0 + random.randint(0, size // 3)
        start_y = y0 + random.randint(0, size - 1)
        
        # Draw main vein
        vein_length = random.randint(size // 2, size)
        angle = random.random() * 2 * 3.14159
        
        # Create unique tin vein look with crystalline formations
        for i in range(vein_length):
            vein_progress = i / vein_length
            
            # Curve the vein
            curve_factor = random.random() * 0.2 - 0.1
            angle += curve_factor
            
            dx = int(math.cos(angle) * i * size / vein_length)
            dy = int(math.sin(angle) * i * size / vein_length)
            
            vx = start_x + dx
            vy = start_y + dy
            
            # Ensure we're in bounds
            if x0 <= vx < x0 + size and y0 <= vy < y0 + size:
                # Vary color along the vein
                tin_color = random.choice([tin_palette['base'], tin_palette['dull'], tin_palette['light']])
                draw.point((vx, vy), fill=tin_color)
                
                # Add crystal formations along the vein
                if random.random() < 0.3:
                    crystal_size = random.randint(1, 3)
                    for cx in range(-crystal_size, crystal_size + 1):
                        for cy in range(-crystal_size, crystal_size + 1):
                            if cx*cx + cy*cy <= crystal_size*crystal_size:
                                px = max(x0, min(x0 + size - 1, vx + cx))
                                py = max(y0, min(y0 + size - 1, vy + cy))
                                # Create a gradient effect in the crystal
                                dist = (cx*cx + cy*cy) / (crystal_size*crystal_size)
                                if dist < 0.3:
                                    crystal_color = tin_palette['light']
                                elif dist < 0.7:
                                    crystal_color = tin_palette['base']
                                else:
                                    crystal_color = tin_palette['dull']
                                draw.point((px, py), fill=crystal_color)
    
    # Add some scattered tin deposits
    for _ in range(size // 6):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        
        tin_color = random.choice([tin_palette['base'], tin_palette['dull']])
        draw.point((tx, ty), fill=tin_color)

def generate_ruby_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Ruby with crystalline red structure and distinct appearance."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Ruby crystals with deeper red color
    ruby_palette = {
        'base': (180, 20, 40, 255),      # Deep ruby red
        'edge': (140, 10, 30, 255),      # Darker edge
        'shine': (255, 120, 140, 255)    # Bright highlight
    }
    
    # Draw larger distinct ruby crystals embedded in stone
    crystal_count = random.randint(2, 3)
    for i in range(crystal_count):
        # Create a central larger crystal structure
        cx = x0 + random.randint(size//4, 3*size//4)
        cy = y0 + random.randint(size//4, 3*size//4)
        crystal_size = random.randint(size//5, size//3)
        
        # Draw faceted ruby crystal - hexagonal shape
        points = []
        sides = random.randint(6, 8)  # Ruby-specific facet count
        for j in range(sides):
            angle = (2 * math.pi * j) / sides
            radius = crystal_size * (0.8 + 0.2 * random.random())
            px = cx + int(radius * math.cos(angle))
            py = cy + int(radius * math.sin(angle))
            
            # Clamp to bounds
            px = max(x0, min(x0 + size - 1, px))
            py = max(y0, min(y0 + size - 1, py))
            points.append((px, py))
        
        # Draw the ruby crystal
        if len(points) >= 3:
            draw.polygon(points, fill=ruby_palette['base'])
            
            # Add internal facets for depth
            if random.random() < 0.7:
                inner_points = []
                inner_scale = 0.7
                for px, py in points:
                    inner_x = cx + int((px - cx) * inner_scale)
                    inner_y = cy + int((py - cy) * inner_scale)
                    inner_points.append((inner_x, inner_y))
                
                draw.polygon(inner_points, fill=ruby_palette['edge'])
            
            # Add bright highlight
            highlight_x = cx + random.randint(-crystal_size//4, crystal_size//4)
            highlight_y = cy + random.randint(-crystal_size//4, crystal_size//4)
            highlight_size = max(1, crystal_size // 5)
            draw.ellipse([
                highlight_x - highlight_size, highlight_y - highlight_size,
                highlight_x + highlight_size, highlight_y + highlight_size
            ], fill=ruby_palette['shine'])

def generate_sapphire_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Sapphire with crystalline blue structure and distinct appearance."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Sapphire crystals with deeper blue color and clarity
    sapphire_palette = {
        'base': (30, 60, 180, 240),      # Deep sapphire blue
        'edge': (15, 30, 120, 255),      # Darker edge
        'shine': (100, 150, 255, 200)    # Bright highlight
    }
    
    # Draw larger distinct sapphire crystals embedded in stone
    crystal_count = random.randint(1, 3)
    
    for i in range(crystal_count):
        # Create rectangular crystal structures (sapphires often have rectangular/prismatic crystal habit)
        cx = x0 + random.randint(size//5, 4*size//5)
        cy = y0 + random.randint(size//5, 4*size//5)
        
        # Determine crystal dimensions
        width = random.randint(size//6, size//3)
        height = random.randint(size//6, size//3)
        rotation = random.random() * math.pi / 4  # Slight rotation
        
        # Create points for rotated rectangle
        points = []
        for corner in [(-1, -1), (1, -1), (1, 1), (-1, 1)]:
            corner_x = corner[0] * width/2
            corner_y = corner[1] * height/2
            
            # Rotate point
            rot_x = corner_x * math.cos(rotation) - corner_y * math.sin(rotation)
            rot_y = corner_x * math.sin(rotation) + corner_y * math.cos(rotation)
            
            # Translate to center position and add to points list
            px = cx + int(rot_x)
            py = cy + int(rot_y)
            
            # Clamp to bounds
            px = max(x0, min(x0 + size - 1, px))
            py = max(y0, min(y0 + size - 1, py))
            points.append((px, py))
        
        # Draw the sapphire crystal
        if len(points) >= 3:
            draw.polygon(points, fill=sapphire_palette['base'])
            
            # Add internal structure lines for depth and clarity
            if width > 3 and height > 3:
                # Horizontal lines
                for j in range(1, 3):
                    y_pos = cy - height//2 + j*height//3
                    for x_pos in range(cx - width//2, cx + width//2):
                        if x0 <= x_pos < x0 + size and y0 <= y_pos < y0 + size:
                            if random.random() < 0.7:  # Some randomness in lines
                                draw.point((x_pos, y_pos), fill=sapphire_palette['edge'])
            
            # Add bright highlight
            highlight_x = cx + random.randint(-width//4, width//4)
            highlight_y = cy + random.randint(-height//4, height//4)
            highlight_size = max(1, min(width, height) // 6)
            draw.ellipse([
                highlight_x - highlight_size, highlight_y - highlight_size,
                highlight_x + highlight_size, highlight_y + highlight_size
            ], fill=sapphire_palette['shine'])

def generate_emerald_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Emerald with crystalline green structure and distinct appearance."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Emerald crystals with rich green color
    emerald_palette = {
        'base': (0, 160, 80, 255),       # Rich emerald green
        'edge': (0, 100, 50, 255),       # Darker edge
        'shine': (120, 255, 180, 255),   # Bright highlight
        'inclusion': (30, 80, 50, 255)   # Typical emerald inclusions
    }
    
    # Draw hexagonal prismatic crystal structure (typical of emeralds)
    crystal_count = random.randint(1, 2)  # Emeralds typically have fewer, larger crystals
    
    for i in range(crystal_count):
        # Create a hexagonal prism crystal shape
        cx = x0 + random.randint(size//4, 3*size//4)
        cy = y0 + random.randint(size//4, 3*size//4)
        crystal_size = random.randint(size//4, size//3)  # Emeralds tend to be larger
        
        # Draw hexagonal emerald crystal
        points = []
        sides = 6  # Emeralds are hexagonal
        for j in range(sides):
            angle = (2 * math.pi * j) / sides
            radius = crystal_size * (0.9 + 0.1 * random.random())  # Less variation for clean crystal
            px = cx + int(radius * math.cos(angle))
            py = cy + int(radius * math.sin(angle))
            
            # Clamp to bounds
            px = max(x0, min(x0 + size - 1, px))
            py = max(y0, min(y0 + size - 1, py))
            points.append((px, py))
        
        # Draw the emerald crystal
        if len(points) >= 3:
            draw.polygon(points, fill=emerald_palette['base'])
            
            # Add internal flaws and inclusions (characteristic of emeralds)
            inclusion_count = random.randint(3, 6)
            for _ in range(inclusion_count):
                ix = cx + random.randint(-crystal_size//2, crystal_size//2)
                iy = cy + random.randint(-crystal_size//2, crystal_size//2)
                
                # Ensure inclusion is within crystal and bounds
                if x0 <= ix < x0 + size and y0 <= iy < y0 + size:
                    inclusion_size = random.randint(1, 3)
                    draw.ellipse([
                        ix - inclusion_size, iy - inclusion_size,
                        ix + inclusion_size, iy + inclusion_size
                    ], fill=emerald_palette['inclusion'])
            
            # Add bright central reflection
            highlight_x = cx
            highlight_y = cy
            highlight_size = max(1, crystal_size // 5)
            highlight_shape = [
                highlight_x - highlight_size, highlight_y - highlight_size//2,
                highlight_x + highlight_size, highlight_y + highlight_size//2
            ]
            draw.ellipse(highlight_shape, fill=emerald_palette['shine'])

def generate_diamond_gem(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Diamond with brilliant crystalline structure and distinctive appearance."""
    # Stone base
    stone_palette = get_palette('stone_basic')
    draw_speckled_pattern(draw, x0, y0, size, stone_palette, density=10, variation=15)
    
    # Diamond crystals with clarity and brilliance
    diamond_palette = {
        'base': (235, 245, 255, 210),     # Clear diamond base
        'edge': (200, 230, 255, 230),     # Slight blue tint on edges
        'facet': (210, 235, 245, 240),    # Facet color
        'shine': (255, 255, 255, 255),    # Bright highlight
        'fire_red': (255, 220, 220, 180), # Diamond fire (dispersion)
        'fire_blue': (220, 220, 255, 180)  # Diamond fire blue
    }
    
    # Draw octahedral diamond crystal (most common natural form)
    cx = x0 + size // 2
    cy = y0 + size // 2
    crystal_size = size // 3
    
    # Create octahedral shape (appears as a square from top view with triangular facets)
    # First draw the main diamond shape
    diamond_points = [
        (cx, cy - crystal_size),               # Top
        (cx + crystal_size, cy),               # Right
        (cx, cy + crystal_size),               # Bottom
        (cx - crystal_size, cy)                # Left
    ]
    
    # Draw the diamond base
    draw.polygon(diamond_points, fill=diamond_palette['base'])
    
    # Add facets (triangular sections)
    # Top-right facet
    tr_facet = [(cx, cy), (cx, cy - crystal_size), (cx + crystal_size, cy)]
    draw.polygon(tr_facet, fill=diamond_palette['facet'])
    
    # Bottom-right facet
    br_facet = [(cx, cy), (cx + crystal_size, cy), (cx, cy + crystal_size)]
    draw.polygon(br_facet, fill=diamond_palette['edge'])
    
    # Bottom-left facet
    bl_facet = [(cx, cy), (cx, cy + crystal_size), (cx - crystal_size, cy)]
    draw.polygon(bl_facet, fill=diamond_palette['facet'])
    
    # Top-left facet
    tl_facet = [(cx, cy), (cx - crystal_size, cy), (cx, cy - crystal_size)]
    draw.polygon(tl_facet, fill=diamond_palette['edge'])
    
    # Add light reflections and fire (dispersion)
    # Central reflection
    center_size = crystal_size // 4
    draw.ellipse([
        cx - center_size, cy - center_size,
        cx + center_size, cy + center_size
    ], fill=diamond_palette['shine'])
    
    # Add colorful fire reflections (dispersion effect)
    for _ in range(4):
        angle = random.random() * 2 * math.pi
        distance = random.randint(crystal_size//3, crystal_size-1)
        
        fire_x = cx + int(math.cos(angle) * distance)
        fire_y = cy + int(math.sin(angle) * distance)
        
        if x0 <= fire_x < x0 + size and y0 <= fire_y < y0 + size:
            fire_size = random.randint(1, 3)
            fire_color = random.choice([
                diamond_palette['fire_red'],
                diamond_palette['fire_blue'],
                diamond_palette['shine']
            ])
            
            draw.ellipse([
                fire_x - fire_size, fire_y - fire_size,
                fire_x + fire_size, fire_y + fire_size
            ], fill=fire_color)

# PIL Image-returning wrapper function for create_atlas.py compatibility
def generate_ore_texture(ore_type: str, size: int = 32):
    """Generate ore texture and return as PIL Image."""
    from PIL import Image
    import sys
    
    # Create image and draw object
    image = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Debug info
    print(f"Debug: Generating ore texture for type '{ore_type}', size={size}")
    
    # Map ore types to generator functions
    ore_mapping = {
        # Full names (from original design)
        'coal_ore': generate_coal_ore,
        'iron_ore': generate_iron_ore,
        'copper_ore': generate_copper_ore,
        'tin_ore': generate_tin_ore,
        'silver_ore': generate_silver_ore,
        'gold_ore': generate_gold_ore,
        
        # Simple names (as passed from create_atlas_efficient.py)
        'coal': generate_coal_ore,
        'iron': generate_iron_ore,
        'copper': generate_copper_ore,
        'tin': generate_tin_ore,
        'silver': generate_silver_ore,
        'gold': generate_gold_ore,
        
        # Gems
        'ruby': generate_ruby_gem,
        'sapphire': generate_sapphire_gem,
        'emerald': generate_emerald_gem,
        'diamond': generate_diamond_gem,
    }
    
    # Generate the texture
    if ore_type in ore_mapping:
        ore_mapping[ore_type](draw, 0, 0, size)
    else:
        # Fallback to coal ore for unknown types
        print(f"Warning: Unknown ore_type '{ore_type}', falling back to coal ore")
        generate_coal_ore(draw, 0, 0, size)
    
    return image

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
