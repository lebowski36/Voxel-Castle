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
    """Generate realistic end grain (top/bottom face) showing multiple smooth concentric tree rings (Lebensringe)."""
    import math
    
    base_color = palette['base']
    ring_color = palette['grain_dark']
    light_ring_color = palette['grain_light']
    
    # Fill base
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
    
    # Center the rings for clean appearance
    center_x = texture_size // 2
    center_y = texture_size // 2
    
    # Calculate maximum radius to texture edge
    max_radius = min(center_x, center_y, texture_size - center_x, texture_size - center_y)
    
    # Generate evenly spaced rings - completely deterministic
    ring_radii = []
    ring_spacing = max(2.0, texture_size / 10.0)  # Clean spacing for visibility
    current_radius = ring_spacing * 0.7  # Start with a small inner ring
    
    while current_radius < max_radius:
        ring_radii.append(current_radius)
        # Fixed spacing - no randomness
        current_radius += ring_spacing
    
    # Draw smooth concentric rings
    for y in range(texture_size):
        for x in range(texture_size):
            dx = x - center_x
            dy = y - center_y
            distance = math.sqrt(dx * dx + dy * dy)
            
            # Very minimal organic distortion for naturalness - deterministic based on position
            angle = math.atan2(dy, dx)
            # Use deterministic pattern based on wood type and position
            distortion_factor = {'oak': 0.1, 'pine': 0.05, 'birch': 0.15, 'mahogany': 0.08}.get(wood_type, 0.1)
            distortion = distortion_factor * math.sin(angle * 8)  # Deterministic wobble
            organic_distance = distance + distortion
            
            # Determine if we're on a ring boundary (dark line)
            is_ring_line = False
            ring_intensity = 0.0
            
            for radius in ring_radii:
                # Distance from ring line
                ring_distance = abs(organic_distance - radius)
                
                # Ring line thickness (1.0 pixels for clean appearance)
                ring_thickness = 0.8
                
                if ring_distance <= ring_thickness:
                    is_ring_line = True
                    # Smooth falloff from ring center to edge
                    ring_intensity = max(ring_intensity, 1.0 - (ring_distance / ring_thickness))
                    break
            
            # Color the pixel
            if is_ring_line:
                # Smooth blend from base color to dark ring color
                color = blend_colors(base_color, ring_color, ring_intensity * 0.7)
            else:
                # Base wood color with minimal variation
                color = vary_color(base_color, 15, x + y)
            
            draw.point((x, y), fill=color)
    
    # Add very subtle wood grain texture that doesn't interfere with rings
    grain_count = texture_size * texture_size // 64  # Minimal grain
    # Fixed positions for deterministic grain
    grain_positions = [(i * 7 + 3) % texture_size for i in range(grain_count)]
    for i in range(grain_count):
        grain_x = grain_positions[i]
        grain_y = grain_positions[(i * 3 + 5) % grain_count] % texture_size
        
        # Deterministic grain pattern (every other grain)
        if i % 2 == 0:
            color = vary_color(base_color, 10, x + y * 2)
            draw.point((grain_x, grain_y), fill=color)

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
        furrow_offsets = [-2, 1, -1, 2, 0, -2, 1]  # Fixed offsets pattern
        for i in range(num_furrows):
            furrow_x = (texture_size // (num_furrows + 1)) * (i + 1)
            furrow_x += furrow_offsets[i % len(furrow_offsets)]
            
            # Draw irregular vertical furrow
            y_pattern = [0, -1, 1, 0, -1, 0, 1, -1, 0]  # Fixed pattern for irregularity
            for y in range(texture_size):
                if y % 5 != 2:  # Skip some pixels for irregular furrow (deterministic)
                    fx = furrow_x + y_pattern[y % len(y_pattern)]
                    fx = max(0, min(texture_size-1, fx))
                    draw.point((fx, y), fill=furrow_color)
                    
                    # Add furrow depth
                    if fx + 1 < texture_size and y % 3 != 1:  # Deterministic depth pattern
                        draw.point((fx + 1, y), fill=vary_color(furrow_color, -20, fx + y))
        
        # Add blocky ridge patterns
        ridge_positions = [(3, 5, 3), (7, 2, 2), (12, 8, 4), (5, 12, 3), (10, 4, 2)]
        for i in range(min(len(ridge_positions), texture_size // 4)):
            ridge_x, ridge_y, ridge_size = ridge_positions[i]
            if ridge_x < texture_size - 3 and ridge_y < texture_size - 3:
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
                    varied_scale_color = vary_color(scale_color, 20, x + y * 3)
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
        line_offsets = [-1, 0, 1, 0, -1, 1, 0]  # Fixed pattern for line variation
        for i, y in enumerate(range(0, texture_size, line_spacing)):
            line_y = y + line_offsets[i % len(line_offsets)]
            if 0 <= line_y < texture_size:
                # Draw horizontal line with breaks
                break_pattern = [True, True, False, True, True, True, False, True]  # Fixed break pattern
                for x in range(texture_size):
                    if break_pattern[x % len(break_pattern)]:  # Deterministic broken line pattern
                        draw.point((x, line_y), fill=line_color)
                        
                        # Sometimes make line thicker (deterministic)
                        if x % 5 == 2 and line_y + 1 < texture_size:
                            draw.point((x, line_y + 1), fill=line_color)
        
        # Add small dark spots/marks characteristic of birch
        spot_count = texture_size // 4
        spot_positions = [(2, 3), (7, 1), (4, 8), (11, 5), (1, 11), (9, 2), (6, 9), (13, 7)]
        for i in range(min(spot_count, len(spot_positions))):
            spot_x, spot_y = spot_positions[i]
            if spot_x < texture_size and spot_y < texture_size:
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
        variation_positions = [(i * 3 + 1, i * 5 + 2) for i in range(variation_count)]
        variation_colors = [highlight_color, vary_color(base_color, 15, 42)]
        for i in range(variation_count):
            vx = variation_positions[i][0] % texture_size
            vy = variation_positions[i][1] % texture_size
            variation_color = variation_colors[i % len(variation_colors)]
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
    saw_mark_color = vary_color(base_color, -30, 123)
    
    # Base plank color
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
    
    # Wood grain running along plank length
    draw_grain_pattern(draw, 0, 0, texture_size, palette, 
                      grain_direction='horizontal', line_variation=1)
    
    # Add sawing marks (visible at 25cm scale)
    saw_mark_spacing = max(2, texture_size // 8)
    saw_offsets = [-1, 0, 1, 0, -1, 1]  # Fixed pattern for saw mark variation
    for i, y in enumerate(range(0, texture_size, saw_mark_spacing)):
        saw_y = y + saw_offsets[i % len(saw_offsets)]
        if 0 <= saw_y < texture_size:
            # Subtle horizontal saw marks
            mark_pattern = [False, False, True, False, False, False, True, False, True, False]
            for x in range(texture_size):
                if mark_pattern[x % len(mark_pattern)]:  # Deterministic subtle marks
                    draw.point((x, saw_y), fill=saw_mark_color)
    
    # Add plank edge detail
    edge_color = vary_color(base_color, -20, 456)
    # Top edge - deterministic pattern
    edge_pattern = [True, False, True, True, False, True]
    for x in range(texture_size):
        if edge_pattern[x % len(edge_pattern)]:
            draw.point((x, 0), fill=edge_color)
    # Bottom edge  
    for x in range(texture_size):
        if edge_pattern[(x + 2) % len(edge_pattern)]:  # Slightly offset pattern
            draw.point((x, texture_size-1), fill=edge_color)
    
    return image

# Export functions
__all__ = ['generate_wood_texture', 'generate_plank_texture']
