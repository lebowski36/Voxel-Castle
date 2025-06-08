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
    
    # Coal chunks - deterministic positions based on tile coordinates
    coal_palette = get_palette('coal_ore')
    coal_chunk_count = size // 3
    
    # Fixed positions for coal chunks based on tile coordinates
    coal_positions = [
        (x0 + size // 4, y0 + size // 6),
        (x0 + 3 * size // 4, y0 + 2 * size // 3),
        (x0 + size // 6, y0 + 4 * size // 5),
        (x0 + 2 * size // 3, y0 + size // 3),
        (x0 + size // 2, y0 + size // 8)
    ]
    
    for i in range(min(coal_chunk_count, len(coal_positions))):
        cx, cy = coal_positions[i]
        cx = max(x0, min(x0 + size - 2, cx))
        cy = max(y0, min(y0 + size - 2, cy))
        
        # Fixed chunk sizes based on position index
        chunk_size = max(1, (size // 4) - (i % 2))
        
        # Draw coal chunk
        draw.rectangle([
            cx, cy, 
            min(x0 + size - 1, cx + chunk_size),
            min(y0 + size - 1, cy + chunk_size)
        ], fill=coal_palette['coal'])
        
        # Add shine to first and third chunks (deterministic)
        if i % 3 == 0:
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
    
    # Draw 2-3 rusty iron veins - fixed count
    vein_count = 2 if size < 16 else 3
    
    # Fixed vein paths
    vein_configs = [
        {'start': (x0, y0 + size // 4), 'end': (x0 + size - 1, y0 + 3 * size // 4), 'width': 1},
        {'start': (x0 + size // 8, y0), 'end': (x0 + 7 * size // 8, y0 + size - 1), 'width': 2},
        {'start': (x0 + size // 3, y0 + size // 6), 'end': (x0 + 2 * size // 3, y0 + 5 * size // 6), 'width': 1}
    ]
    
    for vein_idx in range(min(vein_count, len(vein_configs))):
        config = vein_configs[vein_idx]
        start_x, start_y = config['start']
        end_x, end_y = config['end']
        vein_width = config['width']
        
        # Draw curved vein
        steps = max(size // 2, 5)
        
        for i in range(steps + 1):
            t = i / steps
            
            # Linear interpolation with fixed variation pattern
            vx = int(start_x + t * (end_x - start_x))
            vy = int(start_y + t * (end_y - start_y))
            
            # Add fixed curve variation based on step and vein index
            variation = size // 8
            var_x = int(variation * math.sin(i * 0.5 + vein_idx)) // 2
            var_y = int(variation * math.cos(i * 0.3 + vein_idx)) // 2
            vx += var_x
            vy += var_y
            
            # Ensure we're in bounds
            vx = max(x0, min(x0 + size - 1, vx))
            vy = max(y0, min(y0 + size - 1, vy))
            
            # Draw vein segment
            for w in range(vein_width):
                wx = max(x0, min(x0 + size - 1, vx + w))
                wy = max(y0, min(y0 + size - 1, vy + w))
                
                # Iron color alternates deterministically
                iron_color = iron_palette['iron'] if (i + w) % 3 == 0 else iron_palette['iron_rust']
                draw.point((wx, wy), fill=iron_color)
                
                # Add clusters at specific intervals
                if i % 4 == 0:
                    for dx in [-1, 0, 1]:
                        for dy in [-1, 0, 1]:
                            px = max(x0, min(x0 + size - 1, wx + dx))
                            py = max(y0, min(y0 + size - 1, wy + dy))
                            if (dx + dy) % 2 == 0:  # Checkered pattern
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
    
    # Add copper oxidation spots - deterministic positions
    oxidation_count = size // 4
    oxidation_positions = [
        (x0 + size // 5, y0 + size // 3),
        (x0 + 3 * size // 4, y0 + size // 8),
        (x0 + size // 2, y0 + 4 * size // 5),
        (x0 + size // 8, y0 + 2 * size // 3),
        (x0 + 5 * size // 6, y0 + size // 2)
    ]
    
    for i in range(min(oxidation_count, len(oxidation_positions))):
        ox, oy = oxidation_positions[i]
        ox = max(x0, min(x0 + size - 1, ox))
        oy = max(y0, min(y0 + size - 1, oy))
        
        draw.point((ox, oy), fill=copper_palette['copper_oxide'])
        
        # Add oxidation cluster for first and third spots
        if i % 2 == 0:
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    if (dx + dy) % 2 == 0:  # Checkered pattern
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
    
    # Draw 1-2 gold veins - deterministic count
    vein_count = 1 if size < 16 else 2
    
    # Fixed vein configurations
    vein_configs = [
        {'start': (x0, y0 + size // 3), 'end': (x0 + size - 1, y0 + 2 * size // 3)},
        {'start': (x0 + size // 4, y0), 'end': (x0 + 3 * size // 4, y0 + size - 1)}
    ]
    
    for vein_idx in range(min(vein_count, len(vein_configs))):
        config = vein_configs[vein_idx]
        start_x, start_y = config['start']
        end_x, end_y = config['end']
        
        # Draw curved vein
        steps = max(size // 2, 5)
        vein_width = 1 if vein_idx == 0 else 2
        
        for i in range(steps + 1):
            t = i / steps
            
            # Linear interpolation with fixed variation pattern
            vx = int(start_x + t * (end_x - start_x))
            vy = int(start_y + t * (end_y - start_y))
            
            # Add fixed curve variation
            variation = size // 8
            var_x = int(variation * math.sin(i * 0.4 + vein_idx * 1.5)) // 2
            var_y = int(variation * math.cos(i * 0.2 + vein_idx)) // 2
            vx += var_x
            vy += var_y
            
            # Ensure we're in bounds
            vx = max(x0, min(x0 + size - 1, vx))
            vy = max(y0, min(y0 + size - 1, vy))
            
            # Draw vein segment
            for w in range(vein_width):
                wx = max(x0, min(x0 + size - 1, vx + w))
                wy = max(y0, min(y0 + size - 1, vy + w))
                
                # Gold color alternates deterministically
                gold_color = gold_palette['gold'] if (i + w + vein_idx) % 2 == 0 else gold_palette['gold_bright']
                draw.point((wx, wy), fill=gold_color)
                
                # Add clusters at specific intervals
                if i % 3 == 0:
                    for dx in [-1, 0, 1]:
                        for dy in [-1, 0, 1]:
                            px = max(x0, min(x0 + size - 1, wx + dx))
                            py = max(y0, min(y0 + size - 1, wy + dy))
                            if (dx * dy) == 0:  # Cross pattern
                                draw.point((px, py), fill=gold_color)
    
    # Add additional gold flakes for sparkle - deterministic positions
    flake_count = size // 4
    flake_positions = [
        (x0 + size // 6, y0 + size // 4),
        (x0 + 4 * size // 5, y0 + size // 6),
        (x0 + size // 3, y0 + 5 * size // 6),
        (x0 + 2 * size // 3, y0 + size // 8),
        (x0 + size // 8, y0 + 3 * size // 4)
    ]
    
    for i in range(min(flake_count, len(flake_positions))):
        gx, gy = flake_positions[i]
        gx = max(x0, min(x0 + size - 1, gx))
        gy = max(y0, min(y0 + size - 1, gy))
        
        gold_color = gold_palette['gold'] if i % 2 == 0 else gold_palette['gold_bright']
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
    
    # Draw 1-2 silver veins - deterministic count
    vein_count = 1 if size < 16 else 2
    
    # Fixed vein configurations
    vein_configs = [
        {'start': (x0, y0 + size // 3), 'end': (x0 + size - 1, y0 + 2 * size // 3)},
        {'start': (x0 + size // 4, y0), 'end': (x0 + 3 * size // 4, y0 + size - 1)}
    ]
    
    for vein_idx in range(min(vein_count, len(vein_configs))):
        config = vein_configs[vein_idx]
        start_x, start_y = config['start']
        end_x, end_y = config['end']
        
        # Draw curved vein
        steps = max(size // 2, 5)
        
        for i in range(steps + 1):
            t = i / steps
            
            # Linear interpolation with fixed variation pattern
            vx = int(start_x + t * (end_x - start_x))
            vy = int(start_y + t * (end_y - start_y))
            
            # Add fixed curve variation
            variation = size // 8
            var_x = int(variation * math.sin(i * 0.3 + vein_idx * 2)) // 2
            var_y = int(variation * math.cos(i * 0.5 + vein_idx)) // 2
            vx += var_x
            vy += var_y
            
            # Ensure we're in bounds
            vx = max(x0, min(x0 + size - 1, vx))
            vy = max(y0, min(y0 + size - 1, vy))
            
            # Draw vein segment with alternating silver colors
            color_index = (i + vein_idx) % 2
            silver_color = silver_palette['base'] if color_index == 0 else silver_palette['bright']
            draw.point((vx, vy), fill=silver_color)
            
            # Add clusters at specific intervals
            if i % 4 == 0:
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        px = max(x0, min(x0 + size - 1, vx + dx))
                        py = max(y0, min(y0 + size - 1, vy + dy))
                        if (dx + dy) % 2 == 0:  # Checkered pattern
                            cluster_color_index = (dx + dy + i) % 3
                            if cluster_color_index == 0:
                                cluster_color = silver_palette['base']
                            elif cluster_color_index == 1:
                                cluster_color = silver_palette['bright']
                            else:
                                cluster_color = silver_palette['dark']
                            draw.point((px, py), fill=cluster_color)
    
    # Add additional silver deposits - deterministic positions
    deposit_count = size // 5
    deposit_positions = [
        (x0 + size // 8, y0 + size // 4),
        (x0 + 3 * size // 4, y0 + size // 8),
        (x0 + size // 2, y0 + 5 * size // 6),
        (x0 + size // 6, y0 + 2 * size // 3),
        (x0 + 4 * size // 5, y0 + size // 2)
    ]
    
    for i in range(min(deposit_count, len(deposit_positions))):
        sx, sy = deposit_positions[i]
        sx = max(x0, min(x0 + size - 1, sx))
        sy = max(y0, min(y0 + size - 1, sy))
        
        silver_color = silver_palette['base'] if i % 2 == 0 else silver_palette['bright']
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
    
    # Draw tin veins - deterministic count and patterns
    vein_count = 1 if size < 16 else 2
    
    # Fixed vein configurations
    vein_configs = [
        {'start': (x0, y0 + size // 4), 'angle': 0.7854},  # 45 degrees
        {'start': (x0 + size // 3, y0), 'angle': 1.5708}   # 90 degrees
    ]
    
    for vein_idx in range(min(vein_count, len(vein_configs))):
        config = vein_configs[vein_idx]
        start_x, start_y = config['start']
        base_angle = config['angle']
        
        # Draw main vein with fixed length
        vein_length = size // 2 + size // 4
        
        # Create unique tin vein look with crystalline formations
        for i in range(vein_length):
            vein_progress = i / vein_length
            
            # Fixed curve pattern
            angle = base_angle + 0.1 * math.sin(i * 0.3)
            
            dx = int(math.cos(angle) * i * size / vein_length)
            dy = int(math.sin(angle) * i * size / vein_length)
            
            vx = start_x + dx
            vy = start_y + dy
            
            # Ensure we're in bounds
            if x0 <= vx < x0 + size and y0 <= vy < y0 + size:
                # Color varies along the vein deterministically
                color_index = i % 3
                if color_index == 0:
                    tin_color = tin_palette['base']
                elif color_index == 1:
                    tin_color = tin_palette['dull']
                else:
                    tin_color = tin_palette['light']
                draw.point((vx, vy), fill=tin_color)
                
                # Add crystal formations at specific intervals
                if i % 5 == 0:
                    crystal_size = 1 + (i % 3)
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
    
    # Add some scattered tin deposits - deterministic positions
    deposit_count = size // 6
    deposit_positions = [
        (x0 + size // 7, y0 + size // 3),
        (x0 + 4 * size // 5, y0 + size // 6),
        (x0 + size // 2, y0 + 5 * size // 6),
        (x0 + size // 4, y0 + 2 * size // 3),
        (x0 + 3 * size // 4, y0 + size // 8)
    ]
    
    for i in range(min(deposit_count, len(deposit_positions))):
        tx, ty = deposit_positions[i]
        tx = max(x0, min(x0 + size - 1, tx))
        ty = max(y0, min(y0 + size - 1, ty))
        
        tin_color = tin_palette['base'] if i % 2 == 0 else tin_palette['dull']
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
    
    # Draw larger distinct ruby crystals embedded in stone - deterministic
    crystal_count = 2 if size < 16 else 3
    
    # Fixed crystal positions
    crystal_configs = [
        {'center': (x0 + size//3, y0 + size//3), 'size': size//5},
        {'center': (x0 + 2*size//3, y0 + 2*size//3), 'size': size//4},
        {'center': (x0 + size//2, y0 + 3*size//4), 'size': size//6}
    ]
    
    for i in range(min(crystal_count, len(crystal_configs))):
        config = crystal_configs[i]
        cx, cy = config['center']
        crystal_size = config['size']
        
        # Draw faceted ruby crystal - hexagonal shape with fixed vertices
        points = []
        sides = 6 + (i % 3)  # Ruby-specific facet count (6, 7, or 8)
        for j in range(sides):
            angle = (2 * math.pi * j) / sides
            # Fixed radius variation based on vertex index
            radius_variation = 0.8 + 0.2 * (j % 2)
            radius = crystal_size * radius_variation
            px = cx + int(radius * math.cos(angle))
            py = cy + int(radius * math.sin(angle))
            
            # Clamp to bounds
            px = max(x0, min(x0 + size - 1, px))
            py = max(y0, min(y0 + size - 1, py))
            points.append((px, py))
        
        # Draw the ruby crystal
        if len(points) >= 3:
            draw.polygon(points, fill=ruby_palette['base'])
            
            # Add internal facets for depth - deterministic
            if i % 2 == 0:  # Add inner facets to every other crystal
                inner_points = []
                inner_scale = 0.7
                for px, py in points:
                    inner_x = cx + int((px - cx) * inner_scale)
                    inner_y = cy + int((py - cy) * inner_scale)
                    inner_points.append((inner_x, inner_y))
                
                draw.polygon(inner_points, fill=ruby_palette['edge'])
            
            # Add bright highlight - fixed position relative to center
            highlight_offset_x = crystal_size // 4 if i % 2 == 0 else -crystal_size // 4
            highlight_offset_y = crystal_size // 4 if i % 3 == 0 else -crystal_size // 4
            highlight_x = cx + highlight_offset_x
            highlight_y = cy + highlight_offset_y
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
    
    # Draw larger distinct sapphire crystals embedded in stone - deterministic
    crystal_count = 1 if size < 12 else (2 if size < 20 else 3)
    
    # Fixed crystal configurations
    crystal_configs = [
        {'center': (x0 + 2*size//5, y0 + 2*size//5), 'width': size//6, 'height': size//4, 'rotation': 0.2},
        {'center': (x0 + 3*size//5, y0 + 3*size//5), 'width': size//4, 'height': size//6, 'rotation': 0.8},
        {'center': (x0 + size//4, y0 + 3*size//4), 'width': size//5, 'height': size//5, 'rotation': 0.5}
    ]
    
    for i in range(min(crystal_count, len(crystal_configs))):
        config = crystal_configs[i]
        cx, cy = config['center']
        width = config['width']
        height = config['height']
        rotation = config['rotation']
        
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
            
            # Add internal structure lines for depth and clarity - deterministic
            if width > 3 and height > 3:
                # Horizontal lines at fixed intervals
                for j in range(1, 3):
                    y_pos = cy - height//2 + j*height//3
                    for x_pos in range(cx - width//2, cx + width//2):
                        if x0 <= x_pos < x0 + size and y0 <= y_pos < y0 + size:
                            if (x_pos + j) % 2 == 0:  # Deterministic pattern
                                draw.point((x_pos, y_pos), fill=sapphire_palette['edge'])
            
            # Add bright highlight - fixed position
            highlight_x = cx + width//4 if i % 2 == 0 else cx - width//4
            highlight_y = cy + height//4 if i % 3 == 0 else cy - height//4
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
