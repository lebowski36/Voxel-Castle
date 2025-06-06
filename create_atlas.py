from PIL import Image, ImageDraw
import os
import random # Added import

# Helper functions for drawing textures
def draw_speckled_texture(draw, x0, y0, size, base_color, speckle_intensity_dark=30, speckle_intensity_light=30, density_divider=8):
    """Draws a base color tile with lighter and darker speckles."""
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    num_speckles = (size * size) // density_divider
    for _ in range(num_speckles):
        sx = random.randint(x0, x0 + size - 1)
        sy = random.randint(y0, y0 + size - 1)
        if random.choice([True, False]): # Darker speckle
            speckle_color = (
                max(0, base_color[0] - random.randint(speckle_intensity_dark // 2, speckle_intensity_dark)),
                max(0, base_color[1] - random.randint(speckle_intensity_dark // 2, speckle_intensity_dark)),
                max(0, base_color[2] - random.randint(speckle_intensity_dark // 2, speckle_intensity_dark)),
                base_color[3]
            )
        else: # Lighter speckle
            speckle_color = (
                min(255, base_color[0] + random.randint(speckle_intensity_light // 2, speckle_intensity_light)),
                min(255, base_color[1] + random.randint(speckle_intensity_light // 2, speckle_intensity_light)),
                min(255, base_color[2] + random.randint(speckle_intensity_light // 2, speckle_intensity_light)),
                base_color[3]
            )
        draw.point((sx, sy), fill=speckle_color)

def draw_wood_grain_texture(draw, x0, y0, size, base_color, line_color_offset=40):
    """Draws a base color tile with vertical wood grain lines."""
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    line_color = (
        max(0, base_color[0] - line_color_offset),
        max(0, base_color[1] - line_color_offset), # Adjusted for wood color
        max(0, base_color[2] - line_color_offset),
        base_color[3]
    )
    num_lines = size // 3 + random.randint(-1, 1)
    for i in range(num_lines):
        # Ensure lx is calculated relative to the current tile's x0
        base_lx = (size // num_lines) * i
        lx = x0 + base_lx + random.randint(-(size // (num_lines * 2 +1)), (size // (num_lines * 2+1))) # Add some jitter
        lx = max(x0, min(lx, x0 + size -1)) # clamp

        line_start_y = y0
        line_end_y = y0 + size -1
        
        mid_y = y0 + size // 2
        offset_x_mid = random.randint(-1,1)
        offset_x_end = random.randint(-1,1)

        # Draw line in two segments for a slightly less straight appearance
        draw.line([(lx, line_start_y), (max(x0,min(x0+size-1,lx + offset_x_mid)), mid_y)], fill=line_color, width=1)
        draw.line([(max(x0,min(x0+size-1,lx + offset_x_mid)), mid_y), (max(x0,min(x0+size-1,lx + offset_x_end)), line_end_y)], fill=line_color, width=1)

        if random.random() < 0.1: # 10% chance of a knot
            knot_x_rel = base_lx + random.randint(-2, 2) # Relative to tile for consistency
            knot_x_abs = x0 + knot_x_rel
            knot_y = y0 + random.randint(size // 4, 3 * size // 4)
            knot_radius = random.randint(1, max(1,size//8)) # Scale knot size with tile_size
            if x0 <= knot_x_abs < x0 + size and y0 <= knot_y < y0 + size:
                draw.ellipse(
                    [(knot_x_abs - knot_radius, knot_y - knot_radius), 
                     (knot_x_abs + knot_radius, knot_y + knot_radius)], 
                    fill=line_color
                )

def draw_grass_blades_texture(draw, x0, y0, size, base_color, blade_color_offset=28):
    """Draws a base color tile with simple grass blades pointing upwards."""
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    blade_color = (
        max(0, base_color[0]), # Grass is primarily green
        max(0, base_color[1] - blade_color_offset),
        max(0, base_color[2]),
        base_color[3]
    )
    num_blades = size // 2 + random.randint(-size//8, size//8) # Vary blade count slightly
    for _ in range(num_blades):
        bx_start = random.randint(x0, x0 + size - 1)
        blade_height = random.randint(size // 2, size -1) # Blades are relatively tall
        
        # Blades grow upwards from the bottom of the tile portion
        by_start = y0 + size - 1 
        by_end = y0 + size - 1 - blade_height
        
        tip_offset = random.randint(-max(1,size//8),max(1,size//8)) # Tip can sway a bit
        bx_end = max(x0, min(x0 + size - 1, bx_start + tip_offset))
        
        draw.line([(bx_start, by_start), (bx_end, by_end)], fill=blade_color, width=1)

def draw_leaves_mottle_texture(draw, x0, y0, size, base_color, mottle_color_offset=50):
    """Draws a base color tile with lighter mottling for leaves."""
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    mottle_color = ( # Lighter green for mottles
        min(255, base_color[0] + random.randint(0, mottle_color_offset // 3)), 
        min(255, base_color[1] + random.randint(mottle_color_offset // 2, mottle_color_offset)),
        min(255, base_color[2] + random.randint(0, mottle_color_offset // 3)),
        base_color[3]
    )
    num_mottles = (size * size) // 4 + random.randint(-size, size) # Density of mottles
    mottle_max_w = max(1, size//4)
    mottle_max_h = max(1, size//4)

    for _ in range(num_mottles):
        # Ensure mottles are within tile boundaries
        m_w = random.randint(1,mottle_max_w)
        m_h = random.randint(1,mottle_max_h)
        mx = random.randint(x0, x0 + size - m_w) 
        my = random.randint(y0, y0 + size - m_h)
        draw.rectangle([mx, my, mx + m_w -1 , my + m_h -1], fill=mottle_color)


def draw_water_lines_texture(draw, x0, y0, size, base_color, wave_color_offset=30):
    """Draws a base color tile with horizontal wavy lines for water."""
    draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
    wave_color = ( # Lighter blue for waves/highlights
        min(255, base_color[0] + random.randint(wave_color_offset //2, wave_color_offset)),
        min(255, base_color[1] + random.randint(wave_color_offset//2, wave_color_offset)),
        min(255, base_color[2] + random.randint(wave_color_offset//2, wave_color_offset)), 
        base_color[3]
    )
    num_waves = size // 3 + random.randint(-1,1) # Number of wave lines
    
    for i in range(num_waves):
        # Spread waves across the tile height
        base_wy = (size / (num_waves +1)) * (i+1)
        wy = y0 + int(base_wy) + random.randint(-1,1)
        wy = max(y0, min(wy, y0 + size -1)) # clamp

        start_x = x0
        end_x = x0 + size - 1
        
        # Create a wave with a few segments
        num_segments = 3 # e.g. left, middle, right parts of the wave
        px = start_x
        py = wy
        
        for seg_idx in range(num_segments):
            next_px = x0 + int((size / num_segments) * (seg_idx + 1))
            next_px = min(end_x, next_px + random.randint(-size//(num_segments*2+1), size//(num_segments*2+1)))
            
            # Make y oscillate slightly for wave effect
            y_offset = random.randint(-1,1) if seg_idx < num_segments -1 else 0 # last segment tends to flatten
            next_py = wy + y_offset
            next_py = max(y0, min(next_py, y0 + size -1)) # clamp y

            draw.line([(px, py), (next_px, next_py)], fill=wave_color, width=1)
            px, py = next_px, next_py

def generate_texture_atlas(output_path="assets/textures/atlas.png"):
    atlas_size_px = 256
    tile_size_px = 16
    tiles_per_row = atlas_size_px // tile_size_px  # 16x16 grid = 256 total tiles

    # Define colors for existing VoxelTypes (preserve original block IDs)
    # These are the legacy blocks that must maintain their positions
    legacy_voxel_colors = {
        0: (0, 0, 0, 0),        # AIR (Transparent)
        1: (128, 128, 128, 255), # STONE (Medium Grey)
        2: (139, 69, 19, 255),   # DIRT (Brown)
        3: (0, 128, 0, 255),     # GRASS (Green)
        4: (244, 164, 96, 255),  # SAND (moved from old ID 7 to new ID 4)
        5: (128, 128, 128, 255), # GRAVEL (Grey-ish)
        6: (139, 90, 43, 255),   # CLAY (Clay Brown)
        7: (32, 32, 32, 255),    # BEDROCK (Very Dark Grey)
        
        # Stone Variants (10-19)
        10: (150, 100, 100, 255), # GRANITE (Reddish stone)
        11: (220, 220, 220, 255), # MARBLE (White stone)
        12: (194, 178, 128, 255), # SANDSTONE (Tan)
        13: (169, 169, 169, 255), # LIMESTONE (Light grey)
        14: (47, 79, 79, 255),    # SLATE (Dark grey-blue)
        15: (25, 25, 25, 255),    # OBSIDIAN (Very dark)
        16: (160, 160, 160, 255), # PUMICE (Light grey)
        17: (70, 70, 70, 255),    # BASALT (Dark stone)
        18: (220, 220, 240, 255), # QUARTZITE (Off-white)
        
        # Ores & Gems (20-29)
        20: (50, 50, 50, 255),    # COAL_ORE (Dark with black specks)
        21: (130, 90, 70, 255),   # IRON_ORE (Brown with iron specks)
        22: (184, 115, 51, 255),  # COPPER_ORE (Orange-brown)
        23: (192, 192, 192, 255), # TIN_ORE (Silvery)
        24: (192, 192, 192, 255), # SILVER_ORE (Silver)
        25: (255, 215, 0, 255),   # GOLD_ORE (Gold)
        26: (220, 20, 60, 255),   # GEM_RUBY (Red)
        27: (65, 105, 225, 255),  # GEM_SAPPHIRE (Blue)
        28: (50, 205, 50, 255),   # GEM_EMERALD (Green)
        29: (185, 242, 255, 255), # GEM_DIAMOND (Light blue-white)
        
        # Organic Natural (30-39)
        30: (160, 82, 45, 255),   # WOOD_OAK (Wood Brown) - legacy WOOD at new position
        31: (139, 119, 101, 255), # WOOD_PINE (Lighter brown)
        32: (245, 245, 220, 255), # WOOD_BIRCH (Pale)
        33: (117, 42, 42, 255),   # WOOD_MAHOGANY (Dark red-brown)
        34: (0, 100, 0, 255),     # LEAVES_OAK (Dark Green) - legacy LEAVES at new position
        35: (34, 139, 34, 255),   # LEAVES_PINE (Forest green)
        36: (154, 205, 50, 255),  # LEAVES_BIRCH (Yellow-green)
        37: (0, 128, 0, 255),     # LEAVES_PALM (Green)
        38: (139, 69, 19, 255),   # MUSHROOM_BROWN (Brown)
        39: (220, 20, 60, 255),   # MUSHROOM_RED (Red)
        
        # Biome Specific (40-49)
        40: (255, 250, 250, 255), # SNOW (White)
        41: (173, 216, 230, 255), # ICE (Light blue)
        42: (135, 206, 235, 255), # PACKED_ICE (Sky blue)
        43: (34, 139, 34, 255),   # CACTUS (Green)
        44: (85, 107, 47, 255),   # JUNGLE_VINE (Dark olive)
        45: (255, 182, 193, 255), # CORAL_PINK (Pink)
        46: (100, 149, 237, 255), # CORAL_BLUE (Cornflower blue)
        47: (46, 125, 50, 255),   # SEAWEED (Sea green)
        48: (128, 128, 0, 255),   # TUNDRA_MOSS (Olive)
        49: (205, 133, 63, 255),  # DESERT_ROCK (Peru)
        
        # Fluids (50-54)
        50: (0, 0, 200, 255),     # WATER (Blue) - legacy WATER at new position
        51: (255, 69, 0, 255),    # LAVA (Orange-red)
        52: (25, 25, 25, 255),    # OIL (Black)
        53: (173, 255, 47, 255),  # ACID (Green-yellow)
        54: (255, 215, 0, 255),   # HONEY (Golden)
        
        # Gases (55-59)
        55: (248, 248, 255, 128), # STEAM (Semi-transparent white)
        56: (128, 0, 128, 180),   # TOXIC_GAS (Semi-transparent purple)
        57: (255, 255, 224, 100), # NATURAL_GAS (Semi-transparent light yellow)
        58: (138, 43, 226, 150),  # MAGICAL_MIST (Semi-transparent purple)
        59: (105, 105, 105, 200), # SMOKE (Semi-transparent grey)
    }
    
    # Default color for unmapped blocks (placeholder texture)
    default_color = (128, 64, 128, 255)  # Purple placeholder

    # Create a new RGBA image (for transparency)
    atlas_image = Image.new("RGBA", (atlas_size_px, atlas_size_px), (0,0,0,0)) # Fully transparent background
    draw = ImageDraw.Draw(atlas_image)

    print(f"Generating texture atlas with {tiles_per_row}x{tiles_per_row} = {tiles_per_row * tiles_per_row} tiles")
    print(f"Preserving {len(legacy_voxel_colors)} existing textures")

    for tile_y_idx in range(tiles_per_row):
        for tile_x_idx in range(tiles_per_row):
            voxel_id = tile_y_idx * tiles_per_row + tile_x_idx
            
            # Use legacy color if defined, otherwise use placeholder
            base_tile_color = legacy_voxel_colors.get(voxel_id, default_color)
            
            # Calculate pixel coordinates for the tile
            x0 = tile_x_idx * tile_size_px
            y0 = tile_y_idx * tile_size_px
            
            # Apply existing texture generation for legacy blocks (0-59)
            if voxel_id in legacy_voxel_colors:
                if voxel_id == 0: # AIR
                    draw.rectangle([x0, y0, x0 + tile_size_px - 1, y0 + tile_size_px - 1], fill=(0,0,0,0))
                elif voxel_id == 1: # STONE
                    draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=40, speckle_intensity_light=35, density_divider=6)
                elif voxel_id == 2: # DIRT
                    draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=50, speckle_intensity_light=10, density_divider=5)
                elif voxel_id == 3: # GRASS
                    draw_grass_blades_texture(draw, x0, y0, tile_size_px, base_tile_color, blade_color_offset=40)
                elif voxel_id == 4: # SAND (moved from ID 7)
                    draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=25, speckle_intensity_light=30, density_divider=5)
                elif voxel_id == 30: # WOOD_OAK (legacy WOOD)
                    draw_wood_grain_texture(draw, x0, y0, tile_size_px, base_tile_color, line_color_offset=60)
                elif voxel_id == 34: # LEAVES_OAK (legacy LEAVES)
                    draw_leaves_mottle_texture(draw, x0, y0, tile_size_px, base_tile_color, mottle_color_offset=65)
                elif voxel_id == 50: # WATER (moved from ID 6)
                    draw_water_lines_texture(draw, x0, y0, tile_size_px, base_tile_color, wave_color_offset=50)
                elif voxel_id in [31, 32, 33]: # Other wood types
                    draw_wood_grain_texture(draw, x0, y0, tile_size_px, base_tile_color, line_color_offset=60)
                elif voxel_id in [35, 36, 37]: # Other leaf types
                    draw_leaves_mottle_texture(draw, x0, y0, tile_size_px, base_tile_color, mottle_color_offset=65)
                elif voxel_id in [51]: # LAVA - special animated appearance
                    draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=100, speckle_intensity_light=50, density_divider=3)
                elif voxel_id in range(10, 20): # Stone variants
                    draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=30, speckle_intensity_light=25, density_divider=8)
                elif voxel_id in range(20, 30): # Ores and gems
                    draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=60, speckle_intensity_light=40, density_divider=4)
                else:
                    # Default solid color for other legacy blocks
                    draw.rectangle([x0, y0, x0 + tile_size_px - 1, y0 + tile_size_px - 1], fill=base_tile_color)
            else:
                # For new blocks (60-255), create a placeholder texture
                # Purple checkered pattern to indicate "texture needed"
                checker_size = tile_size_px // 4
                for cy in range(0, tile_size_px, checker_size):
                    for cx in range(0, tile_size_px, checker_size):
                        checker_x = (cx // checker_size) % 2
                        checker_y = (cy // checker_size) % 2
                        if (checker_x + checker_y) % 2 == 0:
                            color = default_color
                        else:
                            color = (160, 32, 160, 255)  # Darker purple
                        draw.rectangle([x0 + cx, y0 + cy, x0 + cx + checker_size - 1, y0 + cy + checker_size - 1], fill=color)

    # Ensure the output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    atlas_image.save(output_path)
    legacy_count = len(legacy_voxel_colors)
    placeholder_count = (tiles_per_row * tiles_per_row) - legacy_count
    print(f"Texture atlas saved to {output_path}")
    print(f"Generated {legacy_count} existing textures and {placeholder_count} placeholder textures")
    print(f"Total atlas capacity: {tiles_per_row * tiles_per_row} block types")

if __name__ == "__main__":
    # Assuming the script is run from the project root
    project_root = "/home/system-x1/Projects/Voxel Castle/"
    output_file_path = os.path.join(project_root, "assets/textures/atlas.png")
    generate_texture_atlas(output_path=output_file_path)
