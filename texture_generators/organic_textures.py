"""
Organic Texture Generators
=========================

Generators for leaves, plants, and other organic materials.
Each organic type has unique patterns and characteristics.
"""

from PIL import ImageDraw
import random
from texture_generators.base_patterns import draw_mottled_pattern, draw_speckled_pattern
from texture_generators.color_palettes import get_palette

def generate_oak_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Oak leaves with mottled green pattern and leaf veins."""
    palette = get_palette('oak_leaves')
    draw_mottled_pattern(draw, x0, y0, size, palette, mottle_density=4)
    
    # Add leaf veins
    vein_color = palette.get('vein', (70, 110, 35, 255))
    for _ in range(size // 4):
        # Random vein starting point
        vx = random.randint(x0, x0 + size - 1)
        vy = random.randint(y0, y0 + size - 1)
        
        # Draw short vein lines
        vein_length = random.randint(2, size // 3)
        direction = random.choice(['horizontal', 'vertical', 'diagonal'])
        
        for i in range(vein_length):
            if direction == 'horizontal':
                px = max(x0, min(x0 + size - 1, vx + i))
                py = vy
            elif direction == 'vertical':
                px = vx
                py = max(y0, min(y0 + size - 1, vy + i))
            else:  # diagonal
                px = max(x0, min(x0 + size - 1, vx + i))
                py = max(y0, min(y0 + size - 1, vy + i))
            
            if x0 <= px < x0 + size and y0 <= py < y0 + size:
                draw.point((px, py), fill=vein_color)

def generate_pine_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Pine needles with fine, textured appearance."""
    palette = get_palette('pine_leaves')
    
    # Base color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Draw needle-like texture
    needle_color = palette.get('needle', (50, 90, 50, 255))
    highlight_color = palette.get('highlight', (60, 100, 60, 255))
    
    # Many small needle marks
    for _ in range(size * size // 3):
        nx = random.randint(x0, x0 + size - 1)
        ny = random.randint(y0, y0 + size - 1)
        
        needle_type = random.choice([needle_color, highlight_color, palette['shadow']])
        draw.point((nx, ny), fill=needle_type)
        
        # Sometimes draw small needle clusters
        if random.random() < 0.2:
            for dx in [-1, 0, 1]:
                px = max(x0, min(x0 + size - 1, nx + dx))
                if x0 <= px < x0 + size:
                    draw.point((px, ny), fill=needle_type)

def generate_birch_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Birch leaves with light, delicate appearance."""
    palette = get_palette('birch_leaves')
    draw_mottled_pattern(draw, x0, y0, size, palette, mottle_density=6)
    
    # Add light spots (characteristic of birch)
    light_color = (220, 255, 220, 255)
    for _ in range(size // 3):
        lx = random.randint(x0, x0 + size - 1)
        ly = random.randint(y0, y0 + size - 1)
        draw.point((lx, ly), fill=light_color)

def generate_palm_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Palm fronds with long, flowing patterns."""
    palette = {
        'base': (60, 120, 60, 255),
        'highlight': (80, 140, 80, 255),
        'shadow': (40, 100, 40, 255),
        'frond': (50, 110, 50, 255)
    }
    
    # Base color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Draw palm frond lines
    frond_color = palette['frond']
    for i in range(size // 2):
        # Diagonal frond lines
        start_x = x0 + random.randint(0, size // 2)
        start_y = y0
        
        frond_length = size
        for j in range(frond_length):
            fx = start_x + j
            fy = start_y + j
            
            if x0 <= fx < x0 + size and y0 <= fy < y0 + size:
                draw.point((fx, fy), fill=frond_color)

def generate_brown_mushroom(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Brown mushroom with cap pattern."""
    # Brown mushroom cap
    cap_color = (139, 69, 19, 255)  # Saddle brown
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=cap_color)
    
    # Add mushroom spots
    spot_color = (160, 82, 45, 255)  # Lighter brown
    for _ in range(size // 4):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        # Draw small circular spot
        spot_size = random.randint(1, 3)
        for dx in range(-spot_size, spot_size + 1):
            for dy in range(-spot_size, spot_size + 1):
                if dx*dx + dy*dy <= spot_size*spot_size:
                    if x0 <= sx + dx < x0 + size and y0 <= sy + dy < y0 + size:
                        draw.point((sx + dx, sy + dy), fill=spot_color)

def generate_red_mushroom(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Red mushroom with white spots."""
    # Red mushroom cap
    cap_color = (178, 34, 34, 255)  # Fire brick red
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=cap_color)
    
    # Add white spots
    spot_color = (255, 255, 255, 255)  # White
    for _ in range(size // 3):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        # Draw small circular spot
        spot_size = random.randint(1, 2)
        for dx in range(-spot_size, spot_size + 1):
            for dy in range(-spot_size, spot_size + 1):
                if dx*dx + dy*dy <= spot_size*spot_size:
                    if x0 <= sx + dx < x0 + size and y0 <= sy + dy < y0 + size:
                        draw.point((sx + dx, sy + dy), fill=spot_color)

def generate_cactus(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Cactus with spines and desert coloring."""
    # Cactus green
    base_color = (34, 139, 34, 255)  # Forest green, slightly yellow
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add cactus spines
    spine_color = (255, 255, 255, 255)  # White spines
    for _ in range(size // 2):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        draw.point((sx, sy), fill=spine_color)
    
    # Add vertical ridges
    ridge_color = (0, 100, 0, 255)  # Darker green
    for x in range(x0 + size // 4, x0 + size, size // 4):
        for y in range(y0, y0 + size):
            if random.random() < 0.6:  # 60% coverage
                draw.point((x, y), fill=ridge_color)

def generate_jungle_vine(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Jungle vines with hanging texture patterns."""
    base_color = (34, 139, 34, 255)  # Forest green
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add vine strands (vertical lines with variation)
    vine_color = (20, 100, 20, 255)
    for i in range(3, size, 6):  # Every 6 pixels, starting from 3
        x = x0 + i
        for y in range(y0, y0 + size):
            if random.random() < 0.8:  # 80% chance for each pixel
                offset = random.randint(-1, 1)
                vine_x = max(x0, min(x0 + size - 1, x + offset))
                draw.point((vine_x, y), fill=vine_color)

def generate_pink_coral(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Pink coral with branching patterns."""
    base_color = (255, 182, 193, 255)  # Light pink
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add coral branch patterns
    branch_color = (255, 20, 147, 255)  # Deep pink
    for _ in range(size // 3):
        # Random branch starting point
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        # Draw small branching pattern
        for length in range(3, 8):
            for angle in [(0, 1), (1, 0), (1, 1), (-1, 1)]:  # 4 directions
                new_x = bx + angle[0] * length
                new_y = by + angle[1] * length
                if x0 <= new_x < x0 + size and y0 <= new_y < y0 + size:
                    draw.point((new_x, new_y), fill=branch_color)

def generate_blue_coral(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Blue coral with branching patterns."""
    base_color = (173, 216, 230, 255)  # Light blue
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add coral branch patterns
    branch_color = (0, 191, 255, 255)  # Deep sky blue
    for _ in range(size // 3):
        # Random branch starting point
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        # Draw small branching pattern
        for length in range(3, 8):
            for angle in [(0, 1), (1, 0), (1, 1), (-1, 1)]:  # 4 directions
                new_x = bx + angle[0] * length
                new_y = by + angle[1] * length
                if x0 <= new_x < x0 + size and y0 <= new_y < y0 + size:
                    draw.point((new_x, new_y), fill=branch_color)

def generate_seaweed(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Seaweed with flowing patterns."""
    base_color = (46, 125, 50, 255)  # Dark green
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add flowing seaweed strands
    strand_color = (76, 175, 80, 255)  # Lighter green
    for i in range(2, size, 8):  # Every 8 pixels
        x = x0 + i
        for y in range(y0, y0 + size):
            # Create wavy pattern
            wave = int(2 * ((y - y0) / size * 3.14159 * 2))
            wave_offset = wave % 3 - 1
            strand_x = max(x0, min(x0 + size - 1, x + wave_offset))
            if random.random() < 0.7:  # 70% density
                draw.point((strand_x, y), fill=strand_color)

def generate_tundra_moss(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Tundra moss with sparse, low-growing pattern."""
    base_color = (85, 107, 47, 255)  # Dark olive green
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add moss patches (very sparse)
    moss_colors = [
        (107, 142, 35, 255),  # Olive drab
        (154, 205, 50, 255),  # Yellow green
        (124, 252, 0, 255)    # Lawn green
    ]
    
    for _ in range(size // 6):  # Sparse coverage
        mx = random.randint(x0, x0 + size - 1)
        my = random.randint(y0, y0 + size - 1)
        moss_color = random.choice(moss_colors)
        
        # Small moss patch (1-3 pixels)
        patch_size = random.randint(1, 3)
        for dx in range(patch_size):
            for dy in range(patch_size):
                if mx + dx < x0 + size and my + dy < y0 + size:
                    draw.point((mx + dx, my + dy), fill=moss_color)

# Update existing methods to include face parameter
def generate_oak_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Oak leaves with mottled green pattern and leaf veins."""
    palette = get_palette('oak_leaves')
    draw_mottled_pattern(draw, x0, y0, size, palette, mottle_density=4)
    
    # Add leaf veins
    vein_color = palette.get('vein', (70, 110, 35, 255))
    for _ in range(size // 4):
        # Random vein starting point
        vx = random.randint(x0, x0 + size - 1)
        vy = random.randint(y0, y0 + size - 1)
        
        # Draw short vein lines
        vein_length = random.randint(2, size // 3)
        direction = random.choice(['horizontal', 'vertical', 'diagonal'])
        
        for i in range(vein_length):
            if direction == 'horizontal':
                px = max(x0, min(x0 + size - 1, vx + i))
                py = vy
            elif direction == 'vertical':
                px = vx
                py = max(y0, min(y0 + size - 1, vy + i))
            else:  # diagonal
                px = max(x0, min(x0 + size - 1, vx + i))
                py = max(y0, min(y0 + size - 1, vy + i))
            
            if x0 <= px < x0 + size and y0 <= py < y0 + size:
                draw.point((px, py), fill=vein_color)

def generate_pine_needles(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Pine needles with fine, textured appearance."""
    palette = get_palette('pine_leaves')
    
    # Base color
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add needle texture (fine lines)
    needle_color = palette.get('dark', (20, 60, 20, 255))
    for _ in range(size * 2):  # Dense needle pattern
        nx = random.randint(x0, x0 + size - 1)
        ny = random.randint(y0, y0 + size - 1)
        
        # Draw small needle (1-3 pixels)
        needle_length = random.randint(1, 3)
        direction = random.choice([(1, 0), (0, 1), (1, 1), (-1, 1)])
        
        for i in range(needle_length):
            px = nx + direction[0] * i
            py = ny + direction[1] * i
            if x0 <= px < x0 + size and y0 <= py < y0 + size:
                draw.point((px, py), fill=needle_color)

def generate_birch_leaves(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Birch leaves with light, delicate appearance."""
    palette = get_palette('birch_leaves')
    draw_mottled_pattern(draw, x0, y0, size, palette, mottle_density=6)
    
    # Add light spots (characteristic of birch)
    light_color = (220, 255, 220, 255)
    for _ in range(size // 3):
        lx = random.randint(x0, x0 + size - 1)
        ly = random.randint(y0, y0 + size - 1)
        draw.point((lx, ly), fill=light_color)

def generate_palm_fronds(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Palm fronds with long, flowing patterns."""
    base_color = (85, 107, 47, 255)  # Dark olive
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add frond patterns (diagonal lines)
    frond_color = (124, 252, 0, 255)  # Bright green
    for i in range(0, size, 4):  # Every 4 pixels
        # Draw diagonal frond segments
        for y in range(y0, y0 + size, 2):
            fx = x0 + i + ((y - y0) // 4) % 3 - 1  # Slight diagonal offset
            fy = y
            if x0 <= fx < x0 + size and y0 <= fy < y0 + size:
                draw.point((fx, fy), fill=frond_color)

def generate_brown_mushroom(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Brown mushroom with cap pattern."""
    # Brown mushroom cap
    cap_color = (139, 69, 19, 255)  # Saddle brown
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=cap_color)
    
    # Add mushroom spots
    spot_color = (160, 82, 45, 255)  # Lighter brown
    for _ in range(size // 4):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        # Draw small circular spot
        spot_size = random.randint(1, 3)
        for dx in range(-spot_size, spot_size + 1):
            for dy in range(-spot_size, spot_size + 1):
                if dx*dx + dy*dy <= spot_size*spot_size:
                    if x0 <= sx + dx < x0 + size and y0 <= sy + dy < y0 + size:
                        draw.point((sx + dx, sy + dy), fill=spot_color)

def generate_red_mushroom(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Red mushroom with white spots."""
    # Red mushroom cap
    cap_color = (178, 34, 34, 255)  # Fire brick red
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=cap_color)
    
    # Add white spots
    spot_color = (255, 255, 255, 255)  # White
    for _ in range(size // 3):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        # Draw small circular spot
        spot_size = random.randint(1, 2)
        for dx in range(-spot_size, spot_size + 1):
            for dy in range(-spot_size, spot_size + 1):
                if dx*dx + dy*dy <= spot_size*spot_size:
                    if x0 <= sx + dx < x0 + size and y0 <= sy + dy < y0 + size:
                        draw.point((sx + dx, sy + dy), fill=spot_color)

def generate_cactus(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str = 'all') -> None:
    """Cactus with spines and desert coloring."""
    # Cactus green
    base_color = (34, 139, 34, 255)  # Forest green, slightly yellow
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add cactus spines
    spine_color = (255, 255, 255, 255)  # White spines
    for _ in range(size // 2):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        draw.point((sx, sy), fill=spine_color)
    
    # Add vertical ridges
    ridge_color = (0, 100, 0, 255)  # Darker green
    for x in range(x0 + size // 4, x0 + size, size // 4):
        for y in range(y0, y0 + size):
            if random.random() < 0.6:  # 60% coverage
                draw.point((x, y), fill=ridge_color)

def generate_jungle_vine(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Jungle vines with hanging texture patterns."""
    base_color = (34, 139, 34, 255)  # Forest green
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add vine strands (vertical lines with variation)
    vine_color = (20, 100, 20, 255)
    for i in range(3, size, 6):  # Every 6 pixels, starting from 3
        x = x0 + i
        for y in range(y0, y0 + size):
            if random.random() < 0.8:  # 80% chance for each pixel
                offset = random.randint(-1, 1)
                vine_x = max(x0, min(x0 + size - 1, x + offset))
                draw.point((vine_x, y), fill=vine_color)

def generate_pink_coral(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Pink coral with branching patterns."""
    base_color = (255, 182, 193, 255)  # Light pink
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add coral branch patterns
    branch_color = (255, 20, 147, 255)  # Deep pink
    for _ in range(size // 3):
        # Random branch starting point
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        # Draw small branching pattern
        for length in range(3, 8):
            for angle in [(0, 1), (1, 0), (1, 1), (-1, 1)]:  # 4 directions
                new_x = bx + angle[0] * length
                new_y = by + angle[1] * length
                if x0 <= new_x < x0 + size and y0 <= new_y < y0 + size:
                    draw.point((new_x, new_y), fill=branch_color)

def generate_blue_coral(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Blue coral with branching patterns."""
    base_color = (173, 216, 230, 255)  # Light blue
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add coral branch patterns
    branch_color = (0, 191, 255, 255)  # Deep sky blue
    for _ in range(size // 3):
        # Random branch starting point
        bx = random.randint(x0, x0 + size - 1)
        by = random.randint(y0, y0 + size - 1)
        
        # Draw small branching pattern
        for length in range(3, 8):
            for angle in [(0, 1), (1, 0), (1, 1), (-1, 1)]:  # 4 directions
                new_x = bx + angle[0] * length
                new_y = by + angle[1] * length
                if x0 <= new_x < x0 + size and y0 <= new_y < y0 + size:
                    draw.point((new_x, new_y), fill=branch_color)

def generate_seaweed(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Seaweed with flowing patterns."""
    base_color = (46, 125, 50, 255)  # Dark green
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add flowing seaweed strands
    strand_color = (76, 175, 80, 255)  # Lighter green
    for i in range(2, size, 8):  # Every 8 pixels
        x = x0 + i
        for y in range(y0, y0 + size):
            # Create wavy pattern
            wave = int(2 * ((y - y0) / size * 3.14159 * 2))
            wave_offset = wave % 3 - 1
            strand_x = max(x0, min(x0 + size - 1, x + wave_offset))
            if random.random() < 0.7:  # 70% density
                draw.point((strand_x, y), fill=strand_color)

def generate_tundra_moss(draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str) -> None:
    """Tundra moss with sparse, low-growing pattern."""
    base_color = (85, 107, 47, 255)  # Dark olive green
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    
    # Add moss patches (very sparse)
    moss_colors = [
        (107, 142, 35, 255),  # Olive drab
        (154, 205, 50, 255),  # Yellow green
        (124, 252, 0, 255)    # Lawn green
    ]
    
    for _ in range(size // 6):  # Sparse coverage
        mx = random.randint(x0, x0 + size - 1)
        my = random.randint(y0, y0 + size - 1)
        moss_color = random.choice(moss_colors)
        
        # Small moss patch (1-3 pixels)
        patch_size = random.randint(1, 3)
        for dx in range(patch_size):
            for dy in range(patch_size):
                if mx + dx < x0 + size and my + dy < y0 + size:
                    draw.point((mx + dx, my + dy), fill=moss_color)

# Lookup table for organic generators
ORGANIC_GENERATORS = {
    34: generate_oak_leaves,      # LEAVES_OAK (legacy LEAVES at new position)
    35: generate_pine_leaves,     # LEAVES_PINE
    36: generate_birch_leaves,    # LEAVES_BIRCH
    37: generate_palm_leaves,     # LEAVES_PALM
    38: generate_brown_mushroom,  # MUSHROOM_BROWN
    39: generate_red_mushroom,    # MUSHROOM_RED
    43: generate_cactus,          # CACTUS
}
