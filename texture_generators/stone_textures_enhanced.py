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
            'edge': vary_color(palette['crystal'], 20),
            'shine': (255, 255, 255, 100)
        }
        
        num_crystals = max(2, texture_size // 8)
        for _ in range(num_crystals):
            cx = random.randint(2, texture_size - 3)
            cy = random.randint(2, texture_size - 3)
            crystal_size = random.randint(1, max(1, texture_size // 6))
            
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
        layer_color = vary_color(palette['base'], -20)
        num_layers = max(1, texture_size // 6)
        for i in range(num_layers):
            y = (texture_size // (num_layers + 1)) * (i + 1)
            y += random.randint(-1, 1)  # Slight variation
            
            # Draw subtle horizontal layer line
            for x in range(texture_size):
                if random.random() < 0.7:  # Not continuous
                    draw.point((x, y), fill=layer_color)
    
    elif stone_type == 'sandstone':
        # Sandstone: cemented sand grains
        # Individual sand grain visibility at 25cm scale
        base_color = palette['base']
        grain_colors = [
            palette['light'],
            palette['dark'],
            vary_color(base_color, 10),
            vary_color(base_color, -10)
        ]
        
        # Fill with base
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=base_color)
        
        # Draw individual sand grains
        grain_density = texture_size * texture_size // 4  # Dense grain pattern
        for _ in range(grain_density):
            gx = random.randint(0, texture_size - 1)
            gy = random.randint(0, texture_size - 1)
            grain_color = random.choice(grain_colors)
            
            # Vary grain size based on texture resolution
            if texture_size >= 16 and random.random() < 0.3:
                # Larger grains occasionally
                grain_size = random.randint(1, 2)
                draw.rectangle([gx, gy, 
                              min(gx + grain_size, texture_size-1), 
                              min(gy + grain_size, texture_size-1)], 
                             fill=grain_color)
            else:
                draw.point((gx, gy), fill=grain_color)
    
    elif stone_type == 'slate':
        # Slate: metamorphic rock with pronounced layering
        # Base color
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=palette['base'])
        
        # Strong horizontal layering characteristic of slate
        layer_spacing = max(2, texture_size // 8)
        for y in range(0, texture_size, layer_spacing):
            layer_y = y + random.randint(-1, 1)
            if layer_y < 0 or layer_y >= texture_size:
                continue
                
            layer_color = random.choice([palette['light'], palette['dark']])
            
            # Draw layer line with some breaks
            for x in range(texture_size):
                if random.random() < 0.8:
                    draw.point((x, layer_y), fill=layer_color)
                    
                    # Sometimes draw thick layers
                    if random.random() < 0.3 and layer_y + 1 < texture_size:
                        draw.point((x, layer_y + 1), fill=layer_color)
    
    elif stone_type == 'obsidian':
        # Obsidian: volcanic glass
        # Base black/dark color
        draw.rectangle([0, 0, texture_size-1, texture_size-1], fill=palette['base'])
        
        # Add glassy highlights and reflections
        highlight_count = max(3, texture_size // 4)
        for _ in range(highlight_count):
            hx = random.randint(0, texture_size - 1)
            hy = random.randint(0, texture_size - 1)
            
            highlight_color = random.choice([palette['shine'], palette['reflection']])
            draw.point((hx, hy), fill=highlight_color)
            
            # Occasionally add larger reflective areas
            if random.random() < 0.2 and texture_size >= 16:
                reflection_size = random.randint(1, 2)
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
        # Add crystalline sparkles
        for _ in range(texture_size // 4):
            sx, sy = random.randint(0, texture_size-1), random.randint(0, texture_size-1)
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
            layer_color = vary_color(palette_shale['base'], 15)
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
                    varied_brick_color = vary_color(brick_color, 10)
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
        
        for _ in range(num_stones):
            # Random stone position and size
            stone_x = random.randint(0, texture_size - 2)
            stone_y = random.randint(0, texture_size - 2)
            stone_size = random.randint(2, max(2, texture_size // 4))
            
            stone_color = random.choice(stone_colors)
            
            # Draw irregular stone shape
            draw.rectangle([stone_x, stone_y, 
                          min(stone_x + stone_size, texture_size-1),
                          min(stone_y + stone_size, texture_size-1)], 
                         fill=stone_color)
    
    return image

# Export the main functions
__all__ = ['generate_stone_texture', 'generate_processed_stone_texture']
