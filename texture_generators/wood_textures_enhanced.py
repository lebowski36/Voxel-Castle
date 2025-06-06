"""
Wood Texture Generators - Enhanced for 25cm Voxels
=================================================

Generates realistic wood textures with species-specific bark and grain patterns.
Each wood type has COMPLETELY DIFFERENT bark patterns for visual authenticity.
Enhanced for 25cm×25cm voxel scale for detailed grain and bark visibility.
"""

from PIL import Image, ImageDraw
import random
from texture_generators.base_patterns import draw_grain_pattern, draw_speckled_pattern, draw_mottled_pattern
from texture_generators.color_palettes import get_palette, vary_color, blend_colors

def generate_wood_texture(texture_size: int = 16, wood_type: str = 'oak', face_type: str = 'side') -> Image.Image:
    """
    Generate wood texture with species-specific patterns.
    
    Args:
        texture_size: Size of texture in pixels
        wood_type: Wood species ('oak', 'pine', 'birch', 'mahogany')  
        face_type: 'side' for bark, 'top'/'bottom' for end grain
    
    Returns:
        PIL Image with wood texture
    """
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    palette = get_palette(f'{wood_type}_wood')
    
    if face_type in ['top', 'bottom']:
        # End grain pattern - visible tree rings at 25cm scale
        generate_end_grain_pattern(draw, texture_size, palette, wood_type)
    else:
        # Side bark pattern - completely different per species
        generate_bark_pattern(draw, texture_size, palette, wood_type)
    
    return image

def generate_end_grain_pattern(draw: ImageDraw.Draw, texture_size: int, palette: dict, wood_type: str) -> None:
    """Generate end grain (top/bottom face) showing tree rings."""
    base_color = palette['base']
    ring_color = palette['grain_dark']
    
    # Fill base
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
    
    # Draw concentric tree rings from center
    center_x = texture_size // 2
    center_y = texture_size // 2
    
    # Number of rings visible in 25cm cross-section
    max_rings = max(3, texture_size // 3)
    
    for ring in range(1, max_rings + 1):
        ring_radius = (ring * texture_size) // (max_rings * 2)
        ring_thickness = random.randint(1, 2)
        
        # Draw ring as circle approximation
        for angle in range(0, 360, 10):  # Sample points around circle
            # Convert angle to radians (simplified)
            x = center_x + int(ring_radius * (angle / 180.0 - 1.0))  # Simplified trig
            y = center_y + int(ring_radius * ((angle + 90) / 180.0 - 1.0))
            
            # Clamp to texture bounds
            if 0 <= x < texture_size and 0 <= y < texture_size:
                ring_draw_color = vary_color(ring_color, 15)
                draw.point((x, y), fill=ring_draw_color)
                
                # Make rings slightly thicker
                for offset in [(0,1), (1,0), (0,-1), (-1,0)]:
                    nx, ny = x + offset[0], y + offset[1]
                    if 0 <= nx < texture_size and 0 <= ny < texture_size:
                        if random.random() < 0.5:
                            draw.point((nx, ny), fill=ring_draw_color)

def generate_bark_pattern(draw: ImageDraw.Draw, texture_size: int, palette: dict, wood_type: str) -> None:
    """Generate species-specific bark patterns - COMPLETELY DIFFERENT per species."""
    
    if wood_type == 'oak':
        # Oak bark: Deep furrows, blocky pattern, rough texture
        base_color = palette['base']
        furrow_color = palette['grain_dark']
        ridge_color = palette['grain_light']
        
        # Base rough texture
        draw_speckled_pattern(draw, 0, 0, texture_size, palette, density=4, variation=30)
        
        # Deep vertical furrows characteristic of oak
        num_furrows = max(2, texture_size // 6)
        for i in range(num_furrows):
            furrow_x = (texture_size // (num_furrows + 1)) * (i + 1)
            furrow_x += random.randint(-2, 2)
            
            # Draw irregular vertical furrow
            for y in range(texture_size):
                if random.random() < 0.8:  # Irregular furrow
                    fx = furrow_x + random.randint(-1, 1)
                    fx = max(0, min(texture_size-1, fx))
                    draw.point((fx, y), fill=furrow_color)
                    
                    # Add furrow depth
                    if fx + 1 < texture_size and random.random() < 0.6:
                        draw.point((fx + 1, y), fill=vary_color(furrow_color, -20))
        
        # Add blocky ridge patterns
        for _ in range(texture_size // 4):
            ridge_x = random.randint(1, texture_size - 3)
            ridge_y = random.randint(1, texture_size - 3)
            ridge_size = random.randint(2, 4)
            
            draw.rectangle([ridge_x, ridge_y, 
                          min(ridge_x + ridge_size, texture_size-1),
                          min(ridge_y + ridge_size, texture_size-1)], 
                         fill=ridge_color)
    
    elif wood_type == 'pine':
        # Pine bark: Scaly, plated pattern, reddish-brown
        base_color = palette['base']
        scale_color = palette['grain_dark']
        plate_color = palette['grain_light']
        
        # Fill base
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
        
        # Draw characteristic scale/plate pattern
        scale_size = max(2, texture_size // 8)
        
        for y in range(0, texture_size, scale_size):
            for x in range(0, texture_size, scale_size):
                # Offset every other row for natural pattern
                offset_x = (scale_size // 2) if (y // scale_size) % 2 == 1 else 0
                scale_x = x + offset_x
                
                if scale_x < texture_size and y < texture_size:
                    # Draw individual bark scale
                    scale_width = min(scale_size, texture_size - scale_x)
                    scale_height = min(scale_size, texture_size - y)
                    
                    # Vary scale color
                    varied_scale_color = vary_color(scale_color, 20)
                    draw.rectangle([scale_x, y, 
                                  scale_x + scale_width - 1, 
                                  y + scale_height - 1], 
                                 fill=varied_scale_color)
                    
                    # Add scale edge highlight
                    if scale_width > 1 and scale_height > 1:
                        draw.point((scale_x, y), fill=plate_color)
    
    elif wood_type == 'birch':
        # Birch bark: Smooth white with dark horizontal lines, papery texture
        base_color = palette['base']  # Light/white
        line_color = palette['grain_dark']  # Dark lines
        spot_color = palette.get('bark_spot', (180, 170, 140, 255))
        
        # Smooth white base
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
        
        # Characteristic horizontal dark lines
        line_spacing = max(2, texture_size // 6)
        for y in range(0, texture_size, line_spacing):
            line_y = y + random.randint(-1, 1)
            if 0 <= line_y < texture_size:
                # Draw horizontal line with breaks
                for x in range(texture_size):
                    if random.random() < 0.7:  # Broken line pattern
                        draw.point((x, line_y), fill=line_color)
                        
                        # Sometimes make line thicker
                        if random.random() < 0.4 and line_y + 1 < texture_size:
                            draw.point((x, line_y + 1), fill=line_color)
        
        # Add small dark spots/marks characteristic of birch
        spot_count = texture_size // 4
        for _ in range(spot_count):
            spot_x = random.randint(0, texture_size - 1)
            spot_y = random.randint(0, texture_size - 1)
            draw.point((spot_x, spot_y), fill=spot_color)
    
    elif wood_type == 'mahogany':
        # Mahogany bark: Dark reddish-brown, fine vertical striations
        base_color = palette['base']  # Dark reddish-brown
        stripe_color = palette['grain_dark']
        highlight_color = palette['rich_tone']
        
        # Rich base color
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
        
        # Fine vertical grain/striations
        draw_grain_pattern(draw, 0, 0, texture_size, palette, 
                          grain_direction='vertical', line_variation=1)
        
        # Add rich mahogany color variations
        variation_count = texture_size * texture_size // 8
        for _ in range(variation_count):
            vx = random.randint(0, texture_size - 1)
            vy = random.randint(0, texture_size - 1)
            variation_color = random.choice([highlight_color, vary_color(base_color, 15)])
            draw.point((vx, vy), fill=variation_color)
    
    else:
        # Default wood grain pattern
        draw_grain_pattern(draw, 0, 0, texture_size, palette, grain_direction='vertical')

def generate_plank_texture(texture_size: int = 16, wood_type: str = 'oak') -> Image.Image:
    """
    Generate processed wood plank texture with sawing marks.
    Shows individual plank with visible processing marks at 25cm scale.
    """
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    palette = get_palette(f'{wood_type}_wood')
    base_color = palette['base']
    grain_color = palette['grain_dark']
    saw_mark_color = vary_color(base_color, -30)
    
    # Base plank color
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
    
    # Wood grain running along plank length
    draw_grain_pattern(draw, 0, 0, texture_size, palette, 
                      grain_direction='horizontal', line_variation=1)
    
    # Add sawing marks (visible at 25cm scale)
    saw_mark_spacing = max(2, texture_size // 8)
    for y in range(0, texture_size, saw_mark_spacing):
        saw_y = y + random.randint(-1, 1)
        if 0 <= saw_y < texture_size:
            # Subtle horizontal saw marks
            for x in range(texture_size):
                if random.random() < 0.3:  # Subtle marks
                    draw.point((x, saw_y), fill=saw_mark_color)
    
    # Add plank edge detail
    edge_color = vary_color(base_color, -20)
    # Top edge
    for x in range(texture_size):
        if random.random() < 0.6:
            draw.point((x, 0), fill=edge_color)
    # Bottom edge  
    for x in range(texture_size):
        if random.random() < 0.6:
            draw.point((x, texture_size-1), fill=edge_color)
    
    return image

# Export functions
__all__ = ['generate_wood_texture', 'generate_plank_texture']
