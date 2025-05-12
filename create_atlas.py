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
    tiles_per_row = atlas_size_px // tile_size_px

    # Define colors for VoxelTypes (ID: (R, G, B, A))
    # AIR should be transparent
    voxel_colors = {
        0: (0, 0, 0, 0),        # AIR (Transparent)
        1: (128, 128, 128, 255), # STONE (Medium Grey)
        2: (139, 69, 19, 255),   # DIRT (Brown)
        3: (0, 128, 0, 255),     # GRASS (Green)
        4: (160, 82, 45, 255),   # WOOD (Wood Brown)
        5: (0, 100, 0, 255),     # LEAVES (Dark Green)
        6: (0, 0, 200, 255),     # WATER (Blue)
        7: (244, 164, 96, 255),  # SAND (Yellow/Tan)
    }
    default_color = (0, 0, 0, 255) # Black for unassigned tiles

    # Create a new RGBA image (for transparency)
    atlas_image = Image.new("RGBA", (atlas_size_px, atlas_size_px), (0,0,0,0)) # Fully transparent background
    draw = ImageDraw.Draw(atlas_image)

    for tile_y_idx in range(tiles_per_row):
        for tile_x_idx in range(tiles_per_row):
            voxel_id = tile_y_idx * tiles_per_row + tile_x_idx
            
            base_tile_color = voxel_colors.get(voxel_id, default_color)
            
            # Calculate pixel coordinates for the tile
            x0 = tile_x_idx * tile_size_px
            y0 = tile_y_idx * tile_size_px
            
            if voxel_id == 0: # AIR
                draw.rectangle([x0, y0, x0 + tile_size_px - 1, y0 + tile_size_px - 1], fill=(0,0,0,0))
            elif voxel_id == 1: # STONE
                draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=40, speckle_intensity_light=35, density_divider=6)
            elif voxel_id == 2: # DIRT
                draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=50, speckle_intensity_light=10, density_divider=5) # More dark, less light, denser
            elif voxel_id == 3: # GRASS
                draw_grass_blades_texture(draw, x0, y0, tile_size_px, base_tile_color, blade_color_offset=40)
            elif voxel_id == 4: # WOOD
                draw_wood_grain_texture(draw, x0, y0, tile_size_px, base_tile_color, line_color_offset=60)
            elif voxel_id == 5: # LEAVES
                draw_leaves_mottle_texture(draw, x0, y0, tile_size_px, base_tile_color, mottle_color_offset=65)
            elif voxel_id == 6: # WATER
                draw_water_lines_texture(draw, x0, y0, tile_size_px, base_tile_color, wave_color_offset=50)
            elif voxel_id == 7: # SAND
                draw_speckled_texture(draw, x0, y0, tile_size_px, base_tile_color, speckle_intensity_dark=25, speckle_intensity_light=30, density_divider=5)
            else: 
                # For any other voxel IDs, draw the base color or default if not specified
                draw.rectangle([x0, y0, x0 + tile_size_px - 1, y0 + tile_size_px - 1], fill=base_tile_color)

    # Ensure the output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    atlas_image.save(output_path)
    print(f"Texture atlas saved to {output_path}")

if __name__ == "__main__":
    # Assuming the script is run from the project root
    project_root = "/home/system-x1/Projects/Voxel Castle/"
    output_file_path = os.path.join(project_root, "assets/textures/atlas.png")
    generate_texture_atlas(output_path=output_file_path)
