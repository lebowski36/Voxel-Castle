"""
Stone Texture Generators - Enhanced for 25cm Voxels
==================================================

Generates realistic stone textures with geological accuracy.
Enhanced for 25cm×25cm voxel scale - allows 4× more detail than typical 1m voxel games.
"""

from PIL import Image, ImageDraw
import random
from texture_generators.base_patterns import draw_speckled_pattern, draw_vein_pattern, get_pattern_function
from texture_generators.color_palettes import get_palette, vary_color, blend_colors

def generate_stone_texture(texture_size: int = 16, stone_type: str = 'granite') -> Image.Image:
    """
    Generate a stone texture with geological realism.
    
    Args:
        texture_size: Size of texture in pixels (typically 16x16 for atlas)
        stone_type: Type of stone ('granite', 'limestone', 'marble', etc.)
    
    Returns:
        PIL Image with stone texture
    """
    # Create image with alpha channel
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Get appropriate palette for stone type
    palette = get_palette(stone_type)
    
    if stone_type == 'granite':
        # Granite: speckled igneous rock with visible crystals
        # 25cm scale allows individual crystal visibility
        draw_speckled_pattern(draw, 0, 0, texture_size, palette, density=6, variation=25)
        
        # Add larger feldspar crystals (visible at 25cm scale)
        crystal_palette = {
            'base': palette.get('crystal', (200, 180, 160, 255)),
            'edge': vary_color(palette['crystal'], 20, 42),
            'shine': (255, 255, 255, 100)
        }
        
        num_crystals = max(2, texture_size // 8)
        # Fixed crystal positions based on texture size and type
        crystal_positions = [
            (texture_size // 4, texture_size // 3),
            (texture_size // 2, texture_size // 4),
            (3 * texture_size // 4, 2 * texture_size // 3),
            (texture_size // 6, 5 * texture_size // 6),
            (5 * texture_size // 6, texture_size // 6)
        ]
        
        for i in range(min(num_crystals, len(crystal_positions))):
            cx, cy = crystal_positions[i]
            cx = max(2, min(texture_size - 3, cx))  # Keep within bounds
            cy = max(2, min(texture_size - 3, cy))
            crystal_size = max(1, texture_size // 6)  # Fixed size
            
            # Draw angular crystal shape
            draw.rectangle([cx-1, cy-1, cx+crystal_size, cy+crystal_size], 
                         fill=crystal_palette['base'])
            
            # Add crystal highlight
            if crystal_size > 1:
                draw.point((cx, cy), fill=crystal_palette['shine'])
    
    elif stone_type == 'marble':
        # Marble: metamorphic rock with distinctive veining
        # Base metamorphic texture
        draw_speckled_pattern(draw, 0, 0, texture_size, palette, density=10, variation=15)
        
        # Add marble veining - more visible at 25cm scale
        vein_palette = {
            'base': palette['base'],
            'vein_dark': palette['vein_dark']
        }
        draw_vein_pattern(draw, 0, 0, texture_size, vein_palette, vein_count=3)
        
        # Add subtle secondary veins
        light_vein_palette = {
            'base': palette['base'],
            'vein_dark': palette['vein_light']
        }
        draw_vein_pattern(draw, 0, 0, texture_size, light_vein_palette, vein_count=2)
    
    elif stone_type == 'limestone':
        # Limestone: sedimentary rock with fine grain
        # Fine speckled pattern appropriate for sedimentary origin
        draw_speckled_pattern(draw, 0, 0, texture_size, palette, density=8, variation=15)
        
        # Add sedimentary layering hints (visible at 25cm scale)
        layer_color = vary_color(palette['base'], -20, 123)  # Fixed seed instead of undefined x + y
        num_layers = max(1, texture_size // 6)
        layer_variations = [-1, 0, 1, -1, 0]  # Fixed pattern
        for i in range(num_layers):
            layer_y = (texture_size // (num_layers + 1)) * (i + 1)
            layer_y += layer_variations[i % len(layer_variations)]  # Deterministic variation
            
            # Draw subtle horizontal layer line
            for layer_x in range(texture_size):
                if (layer_x + i) % 3 != 0:  # Deterministic gaps instead of random
                    draw.point((layer_x, layer_y), fill=layer_color)
    
    elif stone_type == 'sandstone':
        # Sandstone: cemented sand grains
        # Individual sand grain visibility at 25cm scale
        base_color = palette['base']
        grain_colors = [
            palette['light'],
            palette['dark'],
            vary_color(base_color, 10, 456),  # Fixed seed instead of undefined x + y
            vary_color(base_color, -10, 789)  # Fixed seed instead of undefined x + y * 2
        ]
        
        # Fill with base
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
        
        # Draw individual sand grains - deterministic pattern
        grain_density = texture_size * texture_size // 4  # Dense grain pattern
        grain_index = 0
        for y in range(0, texture_size, 2):
            for x in range(0, texture_size, 2):
                if grain_index >= grain_density:
                    break
                # Use deterministic pattern for grain positions
                gx = x + (grain_index % 2)
                gy = y + ((grain_index // 2) % 2)
                if gx < texture_size and gy < texture_size:
                    grain_color = grain_colors[grain_index % len(grain_colors)]
                    
                    # Deterministic grain size based on position
                    if texture_size >= 16 and (x + y) % 7 == 0:
                        # Larger grains occasionally
                        grain_size = 1 if texture_size < 32 else 2
                        draw.rectangle([gx, gy, 
                                      min(gx + grain_size, texture_size-1), 
                                      min(gy + grain_size, texture_size-1)], 
                                     fill=grain_color)
                    else:
                        draw.point((gx, gy), fill=grain_color)
                grain_index += 1
    
    elif stone_type == 'slate':
        # Slate: metamorphic rock with pronounced layering
        # Base color
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=palette['base'])
        
        # Strong horizontal layering characteristic of slate
        layer_spacing = max(2, texture_size // 8)
        layer_variations = [-1, 0, 1, 0, -1]  # Fixed pattern
        layer_colors = [palette['light'], palette['dark']]
        
        layer_index = 0
        for y in range(0, texture_size, layer_spacing):
            layer_y = y + layer_variations[layer_index % len(layer_variations)]
            if layer_y < 0 or layer_y >= texture_size:
                layer_index += 1
                continue
                
            layer_color = layer_colors[layer_index % len(layer_colors)]
            
            # Draw layer line with deterministic breaks
            for x in range(texture_size):
                if (x + layer_index) % 5 != 0:  # Deterministic gaps
                    draw.point((x, layer_y), fill=layer_color)
                    
                    # Sometimes draw thick layers - deterministic
                    if (x + layer_index) % 11 == 0 and layer_y + 1 < texture_size:
                        draw.point((x, layer_y + 1), fill=layer_color)
            layer_index += 1
    
    elif stone_type == 'obsidian':
        # Obsidian: volcanic glass
        # Base black/dark color
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=palette['base'])
        
        # Add glassy highlights and reflections - deterministic positions
        highlight_count = max(3, texture_size // 4)
        highlight_positions = [
            (texture_size // 4, texture_size // 3),
            (texture_size // 2, texture_size // 5),
            (3 * texture_size // 4, 2 * texture_size // 3),
            (texture_size // 6, 4 * texture_size // 5),
            (5 * texture_size // 6, texture_size // 4),
            (texture_size // 3, 3 * texture_size // 4)
        ]
        highlight_colors = [palette['shine'], palette['reflection']]
        
        for i in range(min(highlight_count, len(highlight_positions))):
            hx, hy = highlight_positions[i]
            hx = max(0, min(texture_size - 1, hx))
            hy = max(0, min(texture_size - 1, hy))
            
            highlight_color = highlight_colors[i % len(highlight_colors)]
            draw.point((hx, hy), fill=highlight_color)
            
            # Occasionally add larger reflective areas - deterministic
            if i % 5 == 0 and texture_size >= 16:
                reflection_size = 1 if texture_size < 32 else 2
                draw.rectangle([hx, hy, 
                              min(hx + reflection_size, texture_size-1),
                              min(hy + reflection_size, texture_size-1)], 
                             fill=highlight_color)
    
    elif stone_type == 'basalt':
        # Dark volcanic rock with fine-grained texture
        palette_basalt = {
            'base': (40, 40, 45, 255),
            'dark_speckle': (20, 20, 25, 255),
            'light_speckle': (60, 60, 70, 255)
        }
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_basalt, density=12, variation=15)
        
    elif stone_type == 'quartzite':
        # Very hard, crystalline rock with sparkly appearance
        palette_quartzite = {
            'base': (220, 220, 240, 255),
            'edge': (240, 240, 255, 255),
            'shine': (255, 255, 255, 255)
        }
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_quartzite, density=8, variation=15)
        # Add crystalline sparkles - deterministic positions
        sparkle_positions = [
            (texture_size // 4, texture_size // 6),
            (texture_size // 2, texture_size // 3),
            (3 * texture_size // 4, texture_size // 2),
            (texture_size // 6, 5 * texture_size // 6),
            (5 * texture_size // 6, texture_size // 4)
        ]
        sparkle_count = min(texture_size // 4, len(sparkle_positions))
        for i in range(sparkle_count):
            sx, sy = sparkle_positions[i]
            sx = max(0, min(texture_size-1, sx))
            sy = max(0, min(texture_size-1, sy))
            draw.point((sx, sy), fill=(255, 255, 255, 200))
            
    elif stone_type == 'pumice':
        # Lightweight volcanic rock with porous texture
        palette_pumice = {
            'base': (160, 160, 160, 255),
            'dark_speckle': (120, 120, 120, 255),
            'light_speckle': (200, 200, 200, 255)
        }
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_pumice, density=3, variation=30)
        
    elif stone_type == 'shale':
        # Layered sedimentary rock
        palette_shale = {
            'base': (100, 90, 80, 255),
            'dark_speckle': (70, 65, 55, 255),
            'light_speckle': (130, 120, 105, 255)
        }
        # Create layered appearance with horizontal patterns
        for y in range(0, texture_size, max(1, texture_size // 8)):
            layer_color = vary_color(palette_shale['base'], 15, 234 + y * 3)  # Fixed seed instead of undefined x
            draw.line([(0, y), (texture_size-1, y)], fill=layer_color)
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_shale, density=10, variation=20)
        
    elif stone_type in ['gravel', 'desert_rock']:
        # Gravel - loose stone fragments
        if stone_type == 'gravel':
            palette_gravel = {
                'base': (120, 110, 100, 255),
                'dark_speckle': (80, 75, 65, 255),
                'light_speckle': (160, 150, 135, 255)
            }
        else:  # desert_rock
            palette_gravel = {
                'base': (180, 140, 100, 255),
                'dark_speckle': (140, 110, 70, 255),
                'light_speckle': (220, 180, 130, 255)
            }
        # Very dense speckles to simulate individual rock fragments
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_gravel, density=2, variation=40)
        
    elif stone_type == 'bedrock':
        # Indestructible base layer - dark, dense appearance
        palette_bedrock = {
            'base': (30, 30, 35, 255),
            'dark_speckle': (15, 15, 20, 255),
            'light_speckle': (45, 45, 50, 255)
        }
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_bedrock, density=15, variation=10)
        
    # Handle brick/processed variants by delegating to base type
    elif stone_type.endswith('_brick'):
        base_type = stone_type.replace('_brick', '')
        return generate_stone_texture(texture_size, base_type)
        
    elif stone_type.endswith('_tile'):
        base_type = stone_type.replace('_tile', '')
        return generate_stone_texture(texture_size, base_type)
        
    elif stone_type.startswith('polished_'):
        base_type = stone_type.replace('polished_', '')
        return generate_stone_texture(texture_size, base_type)
        
    elif stone_type == 'cobblestone':
        # Use stone base but with more variation
        palette_cobble = {
            'base': (120, 120, 120, 255),
            'dark_speckle': (80, 80, 80, 255),
            'light_speckle': (160, 160, 160, 255)
        }
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_cobble, density=4, variation=30)
        
    elif stone_type == 'smooth_stone':
        # Smoother version of basic stone
        palette_smooth = {
            'base': (140, 140, 140, 255),
            'dark_speckle': (120, 120, 120, 255),
            'light_speckle': (160, 160, 160, 255)
        }
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_smooth, density=12, variation=15)
        
    elif stone_type == 'flagstone':
        # Large flat stones
        palette_flag = {
            'base': (130, 130, 125, 255),
            'dark_speckle': (100, 100, 95, 255),
            'light_speckle': (160, 160, 155, 255)
        }
        draw_speckled_pattern(draw, 0, 0, texture_size, palette_flag, density=8, variation=25)
        
    else:
        # Default basic stone texture
        draw_speckled_pattern(draw, 0, 0, texture_size, palette, density=8, variation=20)
    
    return image

def generate_processed_stone_texture(texture_size: int = 16, processed_type: str = 'stone_brick') -> Image.Image:
    """
    Generate processed stone textures (bricks, tiles, etc.).
    Enhanced for 25cm scale - individual bricks/tiles visible.
    """
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    if processed_type == 'stone_brick':
        # Individual brick pattern visible at 25cm scale
        # Standard brick is ~20cm, fits well in 25cm voxel
        brick_color = (150, 120, 100, 255)
        mortar_color = (120, 100, 80, 255)
        
        # Fill with mortar base
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=mortar_color)
        
        # Calculate brick layout
        mortar_thickness = max(1, texture_size // 16)  # Mortar lines
        brick_height = max(3, texture_size // 4)  # Individual brick height
        
        y_offset = 0
        row = 0
        while y_offset < texture_size:
            brick_width = max(4, texture_size // 2)  # Individual brick width
            
            # Offset every other row for proper brick pattern
            x_offset = (brick_width // 2) if row % 2 == 1 else 0
            
            x = x_offset
            while x < texture_size:
                # Draw individual brick
                brick_x1 = x
                brick_y1 = y_offset + mortar_thickness
                brick_x2 = min(x + brick_width - mortar_thickness, texture_size - 1)
                brick_y2 = min(y_offset + brick_height - mortar_thickness, texture_size - 1)
                
                if brick_x2 > brick_x1 and brick_y2 > brick_y1:
                    # Add slight color variation to each brick
                    varied_brick_color = vary_color(brick_color, 10, brick_x1 + brick_y1)
                    draw.rectangle([brick_x1, brick_y1, brick_x2, brick_y2], 
                                 fill=varied_brick_color)
                
                x += brick_width
            
            y_offset += brick_height
            row += 1
    
    elif processed_type == 'cobblestone':
        # Irregular stone pieces - visible individual stones at 25cm scale
        base_color = (120, 120, 120, 255)
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
        
        # Generate irregular stone shapes
        num_stones = max(4, texture_size // 3)
        stone_colors = [
            (140, 140, 140, 255),
            (100, 100, 100, 255),
            (130, 125, 120, 255),
            (110, 115, 125, 255)
        ]
        
        # Generate irregular stone shapes - deterministic positions
        num_stones = max(4, texture_size // 3)
        stone_colors = [
            (140, 140, 140, 255),
            (100, 100, 100, 255),
            (130, 125, 120, 255),
            (110, 115, 125, 255)
        ]
        
        stone_positions = [
            (texture_size // 6, texture_size // 4),
            (texture_size // 2, texture_size // 6),
            (2 * texture_size // 3, texture_size // 3),
            (texture_size // 4, 2 * texture_size // 3),
            (5 * texture_size // 6, 5 * texture_size // 6),
            (texture_size // 8, 7 * texture_size // 8)
        ]
        
        for i in range(min(num_stones, len(stone_positions))):
            # Deterministic stone position and size
            stone_x, stone_y = stone_positions[i]
            stone_x = max(0, min(texture_size - 2, stone_x))
            stone_y = max(0, min(texture_size - 2, stone_y))
            stone_size = max(2, texture_size // 4) if i % 2 == 0 else max(2, texture_size // 6)
            
            stone_color = stone_colors[i % len(stone_colors)]
            
            # Draw irregular stone shape
            draw.rectangle([stone_x, stone_y, 
                          min(stone_x + stone_size, texture_size-1),
                          min(stone_y + stone_size, texture_size-1)], 
                         fill=stone_color)
    
    # Handle specific stone type bricks/tiles/processed variants
    elif 'brick' in processed_type:
        # Extract base stone type (granite_brick -> granite)
        base_stone = processed_type.replace('_brick', '')
        return generate_brick_texture(texture_size, base_stone)
    
    elif 'tile' in processed_type:
        # Extract base stone type (marble_tile -> marble)
        base_stone = processed_type.replace('_tile', '')
        return generate_tile_texture(texture_size, base_stone)
    
    elif 'polished' in processed_type:
        # Extract base stone type (polished_stone -> stone)
        base_stone = processed_type.replace('polished_', '')
        return generate_polished_texture(texture_size, base_stone)
    
    elif 'smooth' in processed_type:
        # Extract base stone type (smooth_stone -> stone)
        base_stone = processed_type.replace('smooth_', '')
        return generate_smooth_texture(texture_size, base_stone)
    
    elif 'chiseled' in processed_type:
        # Extract base stone type (chiseled_stone -> stone)
        base_stone = processed_type.replace('chiseled_', '')
        return generate_chiseled_texture(texture_size, base_stone)
    
    return image

def generate_brick_texture(texture_size: int, base_stone: str) -> Image.Image:
    """Generate brick texture based on stone type"""
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Get base stone colors
    palette = get_stone_base_colors(base_stone)
    brick_color = palette['base']
    mortar_color = vary_color(brick_color, -30, 42)
    
    # Fill with mortar base
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=mortar_color)
    
    # Calculate brick layout
    mortar_thickness = max(1, texture_size // 16)
    brick_height = max(3, texture_size // 4)
    
    y_offset = 0
    row = 0
    while y_offset < texture_size:
        brick_width = max(4, texture_size // 2)
        
        # Offset every other row for proper brick pattern
        x_offset = (brick_width // 2) if row % 2 == 1 else 0
        
        x = x_offset
        while x < texture_size:
            # Draw individual brick
            brick_x1 = x
            brick_y1 = y_offset + mortar_thickness
            brick_x2 = min(x + brick_width - mortar_thickness, texture_size - 1)
            brick_y2 = min(y_offset + brick_height - mortar_thickness, texture_size - 1)
            
            if brick_x2 > brick_x1 and brick_y2 > brick_y1:
                # Add slight color variation to each brick
                varied_brick_color = vary_color(brick_color, 10, brick_x1 + brick_y1)
                draw.rectangle([brick_x1, brick_y1, brick_x2, brick_y2], 
                             fill=varied_brick_color)
            
            x += brick_width
        
        y_offset += brick_height
        row += 1
    
    return image

def generate_tile_texture(texture_size: int, base_stone: str) -> Image.Image:
    """Generate tile texture based on stone type"""
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Get base stone colors
    palette = get_stone_base_colors(base_stone)
    tile_color = palette['base']
    grout_color = vary_color(tile_color, -40, 42)
    
    # Fill with grout base
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=grout_color)
    
    # Calculate tile layout - square tiles
    grout_thickness = max(1, texture_size // 20)
    tile_size = max(3, texture_size // 3)
    
    y = 0
    while y < texture_size:
        x = 0
        while x < texture_size:
            # Draw individual tile
            tile_x1 = x + grout_thickness
            tile_y1 = y + grout_thickness
            tile_x2 = min(x + tile_size - grout_thickness, texture_size - 1)
            tile_y2 = min(y + tile_size - grout_thickness, texture_size - 1)
            
            if tile_x2 > tile_x1 and tile_y2 > tile_y1:
                # Add slight color variation to each tile
                varied_tile_color = vary_color(tile_color, 8, x + y)
                draw.rectangle([tile_x1, tile_y1, tile_x2, tile_y2], 
                             fill=varied_tile_color)
            
            x += tile_size
        y += tile_size
    
    return image

def generate_polished_texture(texture_size: int, base_stone: str) -> Image.Image:
    """Generate polished stone texture - smooth with subtle reflection"""
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Get base stone colors
    palette = get_stone_base_colors(base_stone)
    base_color = palette['base']
    
    # Fill with base color
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
    
    # Add subtle polished highlights
    highlight_color = tuple(min(255, c + 20) for c in base_color[:3]) + (128,)
    
    # Diagonal highlights for polished effect
    for i in range(0, texture_size, 4):
        if i < texture_size - 1:
            draw.line([(i, 0), (i+1, 0)], fill=highlight_color)
            draw.line([(0, i), (0, i+1)], fill=highlight_color)
    
    return image

def generate_smooth_texture(texture_size: int, base_stone: str) -> Image.Image:
    """Generate smooth stone texture - very uniform color"""
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Get base stone colors
    palette = get_stone_base_colors(base_stone)
    base_color = palette['base']
    
    # Fill with very uniform color with minimal variation
    for y in range(texture_size):
        for x in range(texture_size):
            # Very subtle variation for texture
            variation = 3 if (x + y) % 4 == 0 else 0
            varied_color = vary_color(base_color, variation, x * y)
            draw.point((x, y), fill=varied_color)
    
    return image

def generate_chiseled_texture(texture_size: int, base_stone: str) -> Image.Image:
    """Generate chiseled stone texture with carved patterns"""
    image = Image.new('RGBA', (texture_size, texture_size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Get base stone colors
    palette = get_stone_base_colors(base_stone)
    base_color = palette['base']
    shadow_color = vary_color(base_color, -40, 42)
    highlight_color = vary_color(base_color, 20, 42)
    
    # Fill with base color
    draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
    
    # Add chiseled border pattern
    border_width = max(1, texture_size // 8)
    
    # Outer border - darker
    for i in range(border_width):
        draw.rectangle([i, i, texture_size-1-i, texture_size-1-i], 
                      outline=shadow_color)
    
    # Inner border - lighter
    inner_start = border_width + 1
    for i in range(max(1, border_width // 2)):
        if inner_start + i < texture_size // 2:
            draw.rectangle([inner_start + i, inner_start + i, 
                          texture_size-1-(inner_start + i), 
                          texture_size-1-(inner_start + i)], 
                         outline=highlight_color)
    
    return image

def get_stone_base_colors(stone_type: str) -> dict:
    """Get color palette for different stone types"""
    palettes = {
        'granite': {
            'base': (180, 160, 140, 255),
            'accent': (200, 180, 160, 255)
        },
        'marble': {
            'base': (240, 235, 230, 255),
            'accent': (250, 245, 240, 255)
        },
        'sandstone': {
            'base': (210, 180, 140, 255),
            'accent': (230, 200, 160, 255)
        },
        'slate': {
            'base': (70, 80, 90, 255),
            'accent': (90, 100, 110, 255)
        },
        'stone': {
            'base': (150, 150, 150, 255),
            'accent': (170, 170, 170, 255)
        }
    }
    
    return palettes.get(stone_type, palettes['stone'])

# Export the main functions
__all__ = ['generate_stone_texture', 'generate_processed_stone_texture']
