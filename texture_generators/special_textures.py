"""
Special Texture Generators
==========================

Generators for unique, functional, and special property blocks.
These include invisible blocks, mechanisms, and advanced technology.
Designed for 25cm×25cm voxel scale for enhanced detail.
"""

from PIL import ImageDraw
import random
from texture_generators.base_patterns import draw_speckled_pattern, draw_grain_pattern
from texture_generators.color_palettes import get_palette, vary_color

def generate_invisible_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Invisible block - completely transparent with subtle shimmer."""
    palette = {
        'base': (255, 255, 255, 0),       # Completely transparent
        'shimmer': (200, 200, 255, 20),   # Very faint shimmer
        'edge': (150, 150, 200, 30)       # Barely visible edges
    }
    
    # Base transparency
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add very subtle shimmer effects (barely visible)
    for _ in range(size // 8):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.3:
            draw.point((sx, sy), fill=palette['shimmer'])
        elif random.random() < 0.5:
            draw.point((sx, sy), fill=palette['edge'])

def generate_intangible_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Intangible block - ghostly, semi-transparent appearance."""
    palette = {
        'base': (180, 180, 255, 60),      # Ghostly blue
        'wisp': (200, 200, 255, 40),      # Wispy patterns
        'fade': (160, 160, 240, 80),      # Fading edges
        'spirit': (220, 220, 255, 30)     # Spiritual energy
    }
    
    # Base ghostly appearance
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add ghostly patterns
    for _ in range(size * size // 6):
        gx = random.randint(x0, x0 + size - 1)
        gy = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((gx, gy), fill=palette['wisp'])
        elif random.random() < 0.6:
            draw.point((gx, gy), fill=palette['fade'])
        elif random.random() < 0.8:
            draw.point((gx, gy), fill=palette['spirit'])

def generate_antigrav_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Anti-gravity block with levitating particle effects."""
    palette = {
        'base': (120, 100, 160, 200),     # Purple anti-grav material
        'particle': (180, 160, 220, 150), # Floating particles
        'energy': (200, 180, 255, 120),   # Anti-grav energy
        'field': (140, 120, 200, 100)     # Gravity field distortion
    }
    
    # Base anti-gravity material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add floating particle effects
    particle_count = size // 3
    for _ in range(particle_count):
        px = random.randint(x0, x0 + size - 1)
        py = random.randint(y0, y0 + size - 1)
        
        # Floating particles
        if random.random() < 0.6:
            particle_size = random.randint(1, max(1, size // 8))
            draw.ellipse([
                px - particle_size//2, py - particle_size//2,
                px + particle_size//2, py + particle_size//2
            ], fill=palette['particle'])
        
        # Energy field effects
        elif random.random() < 0.8:
            draw.point((px, py), fill=palette['energy'])
        else:
            draw.point((px, py), fill=palette['field'])

def generate_magnetic_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Magnetic block with field line patterns."""
    palette = {
        'base': (80, 80, 100, 255),       # Magnetic metal
        'field': (120, 120, 140, 200),    # Magnetic field lines
        'pole': (160, 60, 60, 180),       # Magnetic poles (red)
        'attract': (100, 100, 120, 150)   # Attraction effects
    }
    
    # Base magnetic material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add magnetic field line patterns
    field_lines = size // 4
    for _ in range(field_lines):
        # Curved field lines from center
        center_x = x0 + size // 2
        center_y = y0 + size // 2
        
        # Start point on edge
        start_angle = random.uniform(0, 6.28)  # 2*pi
        start_x = center_x + int((size // 3) * (0.5 + 0.5 * start_angle / 6.28))
        start_y = center_y + int((size // 3) * (0.5 + 0.5 * ((start_angle + 1.57) / 6.28)))
        
        # Draw curved field line
        line_length = random.randint(size // 4, size // 2)
        for i in range(line_length):
            t = i / line_length
            # Simple curved path
            fx = int(start_x + t * (center_x - start_x) * 0.5)
            fy = int(start_y + t * (center_y - start_y) * 0.5)
            
            if x0 <= fx < x0 + size and y0 <= fy < y0 + size:
                draw.point((fx, fy), fill=palette['field'])
    
    # Add magnetic pole indicators
    pole_count = 2
    for _ in range(pole_count):
        pole_x = x0 + random.randint(size//4, 3*size//4)
        pole_y = y0 + random.randint(size//4, 3*size//4)
        pole_size = random.randint(1, max(1, size // 6))
        
        draw.ellipse([
            pole_x - pole_size//2, pole_y - pole_size//2,
            pole_x + pole_size//2, pole_y + pole_size//2
        ], fill=palette['pole'])

def generate_temporal_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Temporal block with time distortion effects."""
    palette = {
        'base': (60, 80, 120, 200),       # Time-blue base
        'past': (100, 60, 80, 150),       # Past time (reddish)
        'future': (60, 100, 160, 150),    # Future time (bluer)
        'distortion': (80, 120, 180, 120) # Time distortion
    }
    
    # Base temporal material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add temporal distortion patterns
    for _ in range(size * size // 8):
        tx = random.randint(x0, x0 + size - 1)
        ty = random.randint(y0, y0 + size - 1)
        
        # Random temporal effects
        time_effect = random.choice(['past', 'future', 'distortion'])
        draw.point((tx, ty), fill=palette[time_effect])
    
    # Add temporal waves
    wave_count = size // 6
    for _ in range(wave_count):
        center_x = x0 + size // 2
        center_y = y0 + size // 2
        
        # Rippling waves from center
        wave_radius = random.randint(size // 6, size // 3)
        wave_points = 8
        
        for i in range(wave_points):
            angle = (2 * 3.14159 * i) / wave_points
            wx = center_x + int(wave_radius * (0.5 + 0.5 * (angle / 6.28)))
            wy = center_y + int(wave_radius * (0.5 + 0.5 * ((angle + 1.57) / 6.28)))
            
            if x0 <= wx < x0 + size and y0 <= wy < y0 + size:
                draw.point((wx, wy), fill=palette['distortion'])

def generate_dimensional_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Dimensional block with portal-like properties."""
    palette = {
        'base': (40, 20, 60, 180),        # Dark dimensional base
        'portal': (120, 60, 200, 150),    # Portal energy
        'void': (20, 10, 30, 200),        # Void spaces
        'energy': (160, 80, 240, 120)     # Dimensional energy
    }
    
    # Base dimensional material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add portal patterns
    portal_center_x = x0 + size // 2
    portal_center_y = y0 + size // 2
    
    # Concentric portal rings
    for ring in range(1, size // 4):
        ring_radius = ring * 2
        ring_points = ring * 4
        
        for i in range(ring_points):
            angle = (2 * 3.14159 * i) / ring_points
            px = portal_center_x + int(ring_radius * (0.5 + 0.5 * (angle / 6.28)))
            py = portal_center_y + int(ring_radius * (0.5 + 0.5 * ((angle + 1.57) / 6.28)))
            
            if x0 <= px < x0 + size and y0 <= py < y0 + size:
                if ring % 2 == 0:
                    draw.point((px, py), fill=palette['portal'])
                else:
                    draw.point((px, py), fill=palette['energy'])
    
    # Add void spaces
    for _ in range(size // 8):
        vx = random.randint(x0, x0 + size - 1)
        vy = random.randint(y0, y0 + size - 1)
        draw.point((vx, vy), fill=palette['void'])

def generate_regenerating_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Self-healing block with regeneration patterns."""
    palette = {
        'base': (80, 120, 80, 255),       # Living green base
        'heal': (120, 180, 120, 200),     # Healing energy
        'growth': (100, 160, 100, 220),   # Growth patterns
        'life': (140, 200, 140, 180)      # Life force
    }
    
    # Base regenerating material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add regeneration patterns
    for _ in range(size * size // 6):
        rx = random.randint(x0, x0 + size - 1)
        ry = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((rx, ry), fill=palette['heal'])
        elif random.random() < 0.6:
            draw.point((rx, ry), fill=palette['growth'])
        elif random.random() < 0.8:
            draw.point((rx, ry), fill=palette['life'])
    
    # Add healing pulses
    pulse_count = size // 8
    for _ in range(pulse_count):
        pulse_x = random.randint(x0, x0 + size - 1)
        pulse_y = random.randint(y0, y0 + size - 1)
        pulse_size = random.randint(2, max(2, size // 4))
        
        # Healing pulse circle
        draw.ellipse([
            pulse_x - pulse_size//2, pulse_y - pulse_size//2,
            pulse_x + pulse_size//2, pulse_y + pulse_size//2
        ], outline=palette['heal'])

def generate_explosive_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Explosive block with unstable energy patterns."""
    palette = {
        'base': (160, 80, 40, 255),       # Explosive orange-red
        'unstable': (200, 100, 50, 220),  # Unstable energy
        'spark': (255, 150, 80, 180),     # Energy sparks
        'danger': (180, 60, 30, 200)      # Danger warning
    }
    
    # Base explosive material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add unstable energy patterns
    for _ in range(size * size // 5):
        ex = random.randint(x0, x0 + size - 1)
        ey = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((ex, ey), fill=palette['unstable'])
        elif random.random() < 0.6:
            draw.point((ex, ey), fill=palette['spark'])
        elif random.random() < 0.8:
            draw.point((ex, ey), fill=palette['danger'])
    
    # Add warning pattern (diagonal stripes)
    stripe_count = size // 4
    for i in range(stripe_count):
        stripe_y = y0 + (size // stripe_count) * i
        for x in range(x0, x0 + size):
            if (x + stripe_y) % 4 == 0:  # Simple diagonal pattern
                if y0 <= stripe_y < y0 + size:
                    draw.point((x, stripe_y), fill=palette['danger'])

def generate_absorbing_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Energy-absorbing block with dampening properties."""
    palette = {
        'base': (40, 40, 60, 255),        # Dark absorbing material
        'absorb': (20, 20, 40, 200),      # Absorption zones
        'dampen': (60, 60, 80, 180),      # Dampening field
        'null': (10, 10, 20, 220)         # Energy nullification
    }
    
    # Base absorbing material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add absorption patterns
    for _ in range(size * size // 7):
        ax = random.randint(x0, x0 + size - 1)
        ay = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.5:
            draw.point((ax, ay), fill=palette['absorb'])
        elif random.random() < 0.7:
            draw.point((ax, ay), fill=palette['dampen'])
        elif random.random() < 0.9:
            draw.point((ax, ay), fill=palette['null'])

def generate_amplifying_block(draw: ImageDraw.Draw, x0: int, y0: int, size: int) -> None:
    """Energy-amplifying block with boosting properties."""
    palette = {
        'base': (100, 120, 160, 255),     # Amplifying crystal base
        'boost': (140, 160, 200, 200),    # Energy boost
        'amplify': (180, 200, 240, 180),  # Amplification field
        'power': (220, 240, 255, 160)     # Power enhancement
    }
    
    # Base amplifying material
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=palette['base'])
    
    # Add amplification patterns
    for _ in range(size * size // 6):
        ax = random.randint(x0, x0 + size - 1)
        ay = random.randint(y0, y0 + size - 1)
        
        if random.random() < 0.4:
            draw.point((ax, ay), fill=palette['boost'])
        elif random.random() < 0.6:
            draw.point((ax, ay), fill=palette['amplify'])
        elif random.random() < 0.8:
            draw.point((ax, ay), fill=palette['power'])
    
    # Add energy focusing lines
    focus_lines = size // 6
    for _ in range(focus_lines):
        # Energy concentration lines toward center
        center_x = x0 + size // 2
        center_y = y0 + size // 2
        
        start_x = random.choice([x0, x0 + size - 1])
        start_y = random.randint(y0, y0 + size - 1)
        
        line_length = size // 2
        for i in range(line_length):
            t = i / line_length
            lx = int(start_x + t * (center_x - start_x))
            ly = int(start_y + t * (center_y - start_y))
            
            if x0 <= lx < x0 + size and y0 <= ly < y0 + size:
                draw.point((lx, ly), fill=palette['amplify'])

# Lookup table for special generators
SPECIAL_GENERATORS = {
    170: generate_invisible_block,      # BLOCK_INVISIBLE
    171: generate_intangible_block,     # BLOCK_INTANGIBLE
    172: generate_antigrav_block,       # BLOCK_ANTIGRAV
    173: generate_magnetic_block,       # BLOCK_MAGNETIC
    174: generate_temporal_block,       # BLOCK_TEMPORAL
    175: generate_dimensional_block,    # BLOCK_DIMENSIONAL
    176: generate_regenerating_block,   # BLOCK_REGENERATING
    177: generate_explosive_block,      # BLOCK_EXPLOSIVE
    178: generate_absorbing_block,      # BLOCK_ABSORBING
    179: generate_amplifying_block,     # BLOCK_AMPLIFYING
}
