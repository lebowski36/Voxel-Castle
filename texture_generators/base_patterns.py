"""
Base Texture Patterns
====================

Reusable pattern generation functions that can be applied with different color palettes.
These patterns form the foundation for creating various block textures.
"""

import random
from PIL import ImageDraw
from typing import Tuple, Callable, List
from texture_generators.color_palettes import Color, ColorPalette, vary_color, blend_colors

# ========== CORE PATTERN FUNCTIONS ==========

def draw_speckled_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int, 
                         palette: ColorPalette, density: int = 8, 
                         variation: int = 20) -> None:
    """
    Draws a base color with lighter and darker speckles.
    Perfect for: Stone, dirt, sand, ore backgrounds.
    
    Args:
        density: Higher number = fewer speckles (size*size // density)
        variation: Color variation range for speckles
    """
    base_color = palette.get('base', (128, 128, 128, 255))
    dark_color = palette.get('dark_speckle', vary_color(base_color, -30))
    light_color = palette.get('light_speckle', vary_color(base_color, 30))
    
    # Fill base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add speckles
    num_speckles = (size * size) // density
    for _ in range(num_speckles):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        speckle_color = random.choice([dark_color, light_color])
        if variation > 0:
            speckle_color = vary_color(speckle_color, variation)
        
        draw.point((sx, sy), fill=speckle_color)

def draw_grain_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int,
                      palette: ColorPalette, grain_direction: str = 'vertical',
                      line_variation: int = 2) -> None:
    """
    Draws wood grain or similar linear patterns.
    Perfect for: All wood types, bamboo, some metals.
    
    Args:
        grain_direction: 'vertical', 'horizontal', or 'diagonal'
        line_variation: Amount of line waviness
    """
    base_color = palette.get('base', (160, 120, 80, 255))
    dark_grain = palette.get('grain_dark', vary_color(base_color, -40))
    light_grain = palette.get('grain_light', vary_color(base_color, 30))
    knot_color = palette.get('knot', vary_color(base_color, -60))
    
    # Fill base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Draw grain lines
    num_lines = size // 3 + random.randint(-1, 1)
    for i in range(num_lines):
        line_color = random.choice([dark_grain, light_grain])
        
        if grain_direction == 'vertical':
            base_x = x0 + (size // num_lines) * i
            line_x = base_x + random.randint(-line_variation, line_variation)
            line_x = max(x0, min(line_x, x0 + size - 1))
            
            # Draw wavy vertical line
            for y in range(y0, y0 + size):
                offset = random.randint(-line_variation//2, line_variation//2)
                px = max(x0, min(x0 + size - 1, line_x + offset))
                draw.point((px, y), fill=line_color)
                
        elif grain_direction == 'horizontal':
            base_y = y0 + (size // num_lines) * i
            line_y = base_y + random.randint(-line_variation, line_variation)
            line_y = max(y0, min(line_y, y0 + size - 1))
            
            # Draw wavy horizontal line
            for x in range(x0, x0 + size):
                offset = random.randint(-line_variation//2, line_variation//2)
                py = max(y0, min(y0 + size - 1, line_y + offset))
                draw.point((x, py), fill=line_color)
    
    # Occasionally add knots
    if random.random() < 0.15:  # 15% chance
        knot_x = x0 + random.randint(size//4, 3*size//4)
        knot_y = y0 + random.randint(size//4, 3*size//4)
        knot_radius = random.randint(1, max(1, size//6))
        
        draw.ellipse([
            (knot_x - knot_radius, knot_y - knot_radius),
            (knot_x + knot_radius, knot_y + knot_radius)
        ], fill=knot_color)

def draw_crystalline_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int,
                           palette: ColorPalette, crystal_count: int = 3) -> None:
    """
    Draws crystalline structures with faceted edges.
    Perfect for: Gems, magical crystals, ores, ice.
    """
    base_color = palette.get('base', (200, 200, 255, 200))
    edge_color = palette.get('edge', vary_color(base_color, 40))
    shine_color = palette.get('shine', (255, 255, 255, 150))
    
    # Fill base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Draw crystal facets
    for _ in range(crystal_count):
        # Random crystal center
        cx = x0 + random.randint(size//4, 3*size//4)
        cy = y0 + random.randint(size//4, 3*size//4)
        
        # Draw faceted crystal shape
        crystal_size = random.randint(size//6, size//3)
        
        # Create polygonal crystal shape
        points = []
        num_sides = random.randint(4, 7)
        for i in range(num_sides):
            angle = (2 * 3.14159 * i) / num_sides
            radius = crystal_size + random.randint(-crystal_size//3, crystal_size//3)
            px = cx + int(radius * (0.5 + 0.5 * (angle / 6.28)))  # Simplified cos
            py = cy + int(radius * (0.5 + 0.5 * ((angle + 1.57) / 6.28)))  # Simplified sin
            
            # Clamp to tile bounds
            px = max(x0, min(x0 + size - 1, px))
            py = max(y0, min(y0 + size - 1, py))
            points.append((px, py))
        
        if len(points) >= 3:
            draw.polygon(points, fill=edge_color)
            
            # Add shine spot
            shine_x = cx + random.randint(-crystal_size//4, crystal_size//4)
            shine_y = cy + random.randint(-crystal_size//4, crystal_size//4)
            shine_x = max(x0, min(x0 + size - 1, shine_x))
            shine_y = max(y0, min(y0 + size - 1, shine_y))
            
            shine_radius = max(1, crystal_size // 6)
            draw.ellipse([
                (shine_x - shine_radius, shine_y - shine_radius),
                (shine_x + shine_radius, shine_y + shine_radius)
            ], fill=shine_color)

def draw_mottled_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int,
                        palette: ColorPalette, mottle_density: int = 4) -> None:
    """
    Draws irregular mottled patches for organic textures.
    Perfect for: Leaves, moss, organic materials, weathered surfaces.
    """
    base_color = palette.get('base', (80, 120, 60, 255))
    highlight_color = palette.get('highlight', vary_color(base_color, 30))
    shadow_color = palette.get('shadow', vary_color(base_color, -30))
    
    # Fill base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add mottled patches
    num_mottles = (size * size) // mottle_density
    
    for _ in range(num_mottles):
        mottle_color = random.choice([highlight_color, shadow_color])
        
        # Random mottle shape and size
        mx = random.randint(x0, x0 + size - 3)
        my = random.randint(y0, y0 + size - 3)
        mw = random.randint(1, max(1, size//4))
        mh = random.randint(1, max(1, size//4))
        
        # Ensure mottle stays within bounds
        mw = min(mw, x0 + size - mx)
        mh = min(mh, y0 + size - my)
        
        draw.rectangle([mx, my, mx + mw - 1, my + mh - 1], fill=mottle_color)

def draw_vein_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int,
                     palette: ColorPalette, vein_count: int = 2) -> None:
    """
    Draws mineral veins or cracks through material.
    Perfect for: Marble, ore veins, cracked materials.
    """
    base_color = palette.get('base', (200, 200, 200, 255))
    vein_color = palette.get('vein_dark', vary_color(base_color, -50))
    
    # Fill base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Draw veins
    for _ in range(vein_count):
        # Random starting point on edge
        if random.choice([True, False]):
            # Start from left/right edge
            start_x = x0 if random.choice([True, False]) else x0 + size - 1
            start_y = random.randint(y0, y0 + size - 1)
            end_x = x0 + size - 1 if start_x == x0 else x0
            end_y = random.randint(y0, y0 + size - 1)
        else:
            # Start from top/bottom edge
            start_x = random.randint(x0, x0 + size - 1)
            start_y = y0 if random.choice([True, False]) else y0 + size - 1
            end_x = random.randint(x0, x0 + size - 1)
            end_y = y0 + size - 1 if start_y == y0 else y0
        
        # Draw curved vein
        steps = max(size, 8)
        for i in range(steps + 1):
            t = i / steps
            
            # Linear interpolation with some randomness
            vx = int(start_x + t * (end_x - start_x))
            vy = int(start_y + t * (end_y - start_y))
            
            # Add some curve variation
            variation = size // 8
            vx += random.randint(-variation, variation)
            vy += random.randint(-variation, variation)
            
            # Clamp to bounds
            vx = max(x0, min(x0 + size - 1, vx))
            vy = max(y0, min(y0 + size - 1, vy))
            
            # Draw vein point (could be thicker for larger sizes)
            draw.point((vx, vy), fill=vein_color)
            
            # Occasionally make vein thicker
            if size > 8 and random.random() < 0.3:
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        px = max(x0, min(x0 + size - 1, vx + dx))
                        py = max(y0, min(y0 + size - 1, vy + dy))
                        draw.point((px, py), fill=vein_color)

def draw_fluid_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int,
                      palette: ColorPalette, wave_count: int = 3) -> None:
    """
    Draws flowing liquid patterns with waves.
    Perfect for: Water, lava, oil, magical fluids.
    """
    base_color = palette.get('base', (60, 120, 200, 200))
    light_color = palette.get('light', vary_color(base_color, 40))
    dark_color = palette.get('dark', vary_color(base_color, -40))
    
    # Fill base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Draw flowing waves
    for i in range(wave_count):
        wave_y = y0 + (size // (wave_count + 1)) * (i + 1)
        wave_color = random.choice([light_color, dark_color])
        
        # Draw wavy horizontal line
        for x in range(x0, x0 + size):
            # Simple wave function
            wave_offset = int(2 * ((x - x0) / size) * 3.14159)  # Simplified sine wave
            wave_y_offset = wave_offset % 3 - 1  # Simple oscillation
            
            wy = wave_y + wave_y_offset + random.randint(-1, 1)
            wy = max(y0, min(y0 + size - 1, wy))
            
            draw.point((x, wy), fill=wave_color)

def draw_brick_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int,
                      palette: ColorPalette, brick_width: int = 8, brick_height: int = 4,
                      mortar_width: int = 1) -> Tuple[any, any]:
    """
    Draws a brick pattern with mortar lines.
    Perfect for: Ceramic blocks, brick walls, cobblestone.
    
    Args:
        brick_width: Width of individual bricks
        brick_height: Height of individual bricks  
        mortar_width: Width of mortar lines between bricks
    
    Returns:
        Tuple of (brick_mask, mortar_mask) for further processing
    """
    brick_color = palette.get('brick', (160, 100, 80, 255))
    mortar_color = palette.get('mortar', (120, 120, 100, 255))
    
    # Fill with mortar color as base
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=mortar_color)
    
    # Draw brick pattern
    y = y0
    row = 0
    brick_mask = []
    mortar_mask = []
    
    while y < y0 + size:
        # Stagger every other row
        x_offset = (brick_width // 2) if row % 2 == 1 else 0
        x = x0 + x_offset
        
        while x < x0 + size:
            # Draw brick rectangle
            x1 = min(x, x0 + size - 1)
            y1 = min(y, y0 + size - 1)
            x2 = min(x + brick_width - mortar_width, x0 + size - 1)
            y2 = min(y + brick_height - mortar_width, y0 + size - 1)
            
            if x2 > x1 and y2 > y1:
                varied_brick_color = vary_color(brick_color, 15)
                draw.rectangle([x1, y1, x2, y2], fill=varied_brick_color)
                brick_mask.append((x1, y1, x2, y2))
            
            x += brick_width
        
        row += 1
        y += brick_height
    
    return brick_mask, mortar_mask

# ========== PATTERN COMBINATION UTILITIES ==========

def apply_overlay_pattern(draw: ImageDraw.Draw, x0: int, y0: int, size: int,
                         base_pattern: Callable, overlay_pattern: Callable,
                         base_palette: ColorPalette, overlay_palette: ColorPalette) -> None:
    """Apply a base pattern, then overlay another pattern on top."""
    base_pattern(draw, x0, y0, size, base_palette)
    overlay_pattern(draw, x0, y0, size, overlay_palette)

def get_pattern_function(pattern_name: str) -> Callable:
    """Get pattern function by name."""
    patterns = {
        'speckled': draw_speckled_pattern,
        'grain': draw_grain_pattern,
        'crystalline': draw_crystalline_pattern,
        'mottled': draw_mottled_pattern,
        'vein': draw_vein_pattern,
        'fluid': draw_fluid_pattern,
        'brick': draw_brick_pattern
    }
    return patterns.get(pattern_name, draw_speckled_pattern)
