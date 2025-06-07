from PIL import Image, ImageDraw
import os
import sys
import random

# Add texture_generators to path for modular texture system
current_dir = os.path.dirname(os.path.abspath(__file__))
texture_generators_path = os.path.join(current_dir, 'texture_generators')
sys.path.insert(0, texture_generators_path)

# Import our modular texture generation system
try:
    from texture_generators.simple_texture_coordinator import SimpleTextureCoordinator
    from texture_generators.stone_textures_enhanced import generate_stone_texture, generate_processed_stone_texture
    from texture_generators.wood_textures_enhanced import generate_wood_texture, generate_plank_texture
    from texture_generators.organic_textures import generate_organic_texture
    from texture_generators.special_textures import generate_special_texture
    from texture_generators.crystal_textures import generate_crystal_texture
    from texture_generators.ceramic_textures import generate_ceramic_texture
    from texture_generators.metal_textures import generate_metal_texture
    from texture_generators.fluid_textures import generate_fluid_texture
    from texture_generators.ore_textures import generate_ore_texture
    MODULAR_SYSTEM_AVAILABLE = True
    print("✓ Modular texture generation system loaded successfully")
except ImportError as e:
    print(f"⚠ Warning: Modular texture system not available ({e}). Using legacy generation.")
    MODULAR_SYSTEM_AVAILABLE = False

# Block type classification for texture generation
BLOCK_MAPPING = {
    # Basic Terrain (0-9)
    0: {'type': 'air', 'subtype': 'transparent'},
    1: {'type': 'stone', 'subtype': 'stone'},
    2: {'type': 'organic', 'subtype': 'dirt'},
    3: {'type': 'organic', 'subtype': 'grass'},
    4: {'type': 'organic', 'subtype': 'sand'},
    5: {'type': 'stone', 'subtype': 'gravel'},
    6: {'type': 'ceramic', 'subtype': 'clay'},
    7: {'type': 'stone', 'subtype': 'bedrock'},
    8: {'type': 'organic', 'subtype': 'topsoil'},
    9: {'type': 'organic', 'subtype': 'subsoil'},
    
    # Stone Varieties (10-19)
    10: {'type': 'stone', 'subtype': 'granite'},
    11: {'type': 'stone', 'subtype': 'limestone'},
    12: {'type': 'stone', 'subtype': 'marble'},
    13: {'type': 'stone', 'subtype': 'sandstone'},
    14: {'type': 'stone', 'subtype': 'slate'},
    15: {'type': 'stone', 'subtype': 'basalt'},
    16: {'type': 'stone', 'subtype': 'quartzite'},
    17: {'type': 'stone', 'subtype': 'obsidian'},
    18: {'type': 'stone', 'subtype': 'pumice'},
    19: {'type': 'stone', 'subtype': 'shale'},
    
    # Ores & Minerals (20-29)
    20: {'type': 'ore', 'subtype': 'coal'},
    21: {'type': 'ore', 'subtype': 'iron'},
    22: {'type': 'ore', 'subtype': 'copper'},
    23: {'type': 'ore', 'subtype': 'tin'},
    24: {'type': 'ore', 'subtype': 'silver'},
    25: {'type': 'ore', 'subtype': 'gold'},
    26: {'type': 'crystal', 'subtype': 'ruby'},
    27: {'type': 'crystal', 'subtype': 'sapphire'},
    28: {'type': 'crystal', 'subtype': 'emerald'},
    29: {'type': 'crystal', 'subtype': 'diamond'},
    
    # Organic Natural (30-39)
    30: {'type': 'wood', 'subtype': 'oak'},
    31: {'type': 'wood', 'subtype': 'pine'},
    32: {'type': 'wood', 'subtype': 'birch'},
    33: {'type': 'wood', 'subtype': 'mahogany'},
    34: {'type': 'organic', 'subtype': 'oak_leaves'},
    35: {'type': 'organic', 'subtype': 'pine_leaves'},
    36: {'type': 'organic', 'subtype': 'birch_leaves'},
    37: {'type': 'organic', 'subtype': 'palm_leaves'},
    38: {'type': 'organic', 'subtype': 'mushroom_brown'},
    39: {'type': 'organic', 'subtype': 'mushroom_red'},
    
    # Biome Specific (40-49)
    40: {'type': 'special', 'subtype': 'snow'},
    41: {'type': 'special', 'subtype': 'ice'},
    42: {'type': 'special', 'subtype': 'packed_ice'},
    43: {'type': 'organic', 'subtype': 'cactus'},
    44: {'type': 'organic', 'subtype': 'jungle_vine'},
    45: {'type': 'organic', 'subtype': 'coral_pink'},
    46: {'type': 'organic', 'subtype': 'coral_blue'},
    47: {'type': 'organic', 'subtype': 'seaweed'},
    48: {'type': 'organic', 'subtype': 'tundra_moss'},
    49: {'type': 'stone', 'subtype': 'desert_rock'},
    
    # Fluids (50-54)
    50: {'type': 'fluid', 'subtype': 'water'},
    51: {'type': 'fluid', 'subtype': 'lava'},
    52: {'type': 'fluid', 'subtype': 'oil'},
    53: {'type': 'fluid', 'subtype': 'acid'},
    54: {'type': 'fluid', 'subtype': 'honey'},
    
    # Gases (55-59)
    55: {'type': 'fluid', 'subtype': 'steam'},
    56: {'type': 'fluid', 'subtype': 'toxic_gas'},
    57: {'type': 'fluid', 'subtype': 'natural_gas'},
    58: {'type': 'special', 'subtype': 'magical_mist'},
    59: {'type': 'fluid', 'subtype': 'smoke'},
    
    # Stone Processing (60-69)
    60: {'type': 'stone', 'subtype': 'stone_brick'},
    61: {'type': 'stone', 'subtype': 'granite_brick'},
    62: {'type': 'stone', 'subtype': 'marble_tile'},
    63: {'type': 'stone', 'subtype': 'sandstone_brick'},
    64: {'type': 'stone', 'subtype': 'slate_tile'},
    65: {'type': 'stone', 'subtype': 'cobblestone'},
    66: {'type': 'stone', 'subtype': 'smooth_stone'},
    67: {'type': 'stone', 'subtype': 'polished_granite'},
    68: {'type': 'stone', 'subtype': 'polished_marble'},
    69: {'type': 'stone', 'subtype': 'flagstone'},
    
    # Wood Processing (70-79)
    70: {'type': 'wood', 'subtype': 'oak_planks'},
    71: {'type': 'wood', 'subtype': 'pine_planks'},
    72: {'type': 'wood', 'subtype': 'birch_planks'},
    73: {'type': 'wood', 'subtype': 'mahogany_planks'},
    74: {'type': 'wood', 'subtype': 'oak_beam'},
    75: {'type': 'wood', 'subtype': 'pine_beam'},
    76: {'type': 'wood', 'subtype': 'hardwood_beam'},
    77: {'type': 'wood', 'subtype': 'bamboo_plank'},
    78: {'type': 'wood', 'subtype': 'cork'},
    79: {'type': 'special', 'subtype': 'charcoal_block'},
    
    # Metal Blocks (80-89)
    80: {'type': 'metal', 'subtype': 'iron'},
    81: {'type': 'metal', 'subtype': 'copper'},
    82: {'type': 'metal', 'subtype': 'bronze'},
    83: {'type': 'metal', 'subtype': 'steel'},
    84: {'type': 'metal', 'subtype': 'silver'},
    85: {'type': 'metal', 'subtype': 'gold'},
    86: {'type': 'metal', 'subtype': 'brass'},
    87: {'type': 'metal', 'subtype': 'pewter'},
    88: {'type': 'metal', 'subtype': 'mithril'},
    89: {'type': 'metal', 'subtype': 'adamantine'},
    
    # Clay & Ceramic (90-99)
    90: {'type': 'ceramic', 'subtype': 'clay_brick'},
    91: {'type': 'ceramic', 'subtype': 'terracotta'},
    92: {'type': 'ceramic', 'subtype': 'glazed_white'},
    93: {'type': 'ceramic', 'subtype': 'glazed_red'},
    94: {'type': 'ceramic', 'subtype': 'glazed_blue'},
    95: {'type': 'ceramic', 'subtype': 'glazed_green'},
    96: {'type': 'ceramic', 'subtype': 'porcelain'},
    97: {'type': 'ceramic', 'subtype': 'stoneware'},
    98: {'type': 'ceramic', 'subtype': 'earthenware'},
    99: {'type': 'ceramic', 'subtype': 'ceramic_tile'},
    
    # Functional blocks (100-149) - Use special generation
    **{i: {'type': 'special', 'subtype': 'functional'} for i in range(100, 150)},
    
    # Magical Materials (150-159)
    150: {'type': 'crystal', 'subtype': 'clear'},
    151: {'type': 'crystal', 'subtype': 'blue'},
    152: {'type': 'crystal', 'subtype': 'red'},
    153: {'type': 'crystal', 'subtype': 'green'},
    154: {'type': 'special', 'subtype': 'enchanted_stone'},
    155: {'type': 'special', 'subtype': 'runic_block'},
    156: {'type': 'crystal', 'subtype': 'ether'},
    157: {'type': 'special', 'subtype': 'void_stone'},
    158: {'type': 'stone', 'subtype': 'celestial_marble'},
    159: {'type': 'special', 'subtype': 'shadow_glass'},
    
    # Advanced Technology (160-169)
    **{i: {'type': 'special', 'subtype': 'technology'} for i in range(160, 170)},
    
    # Special Properties (170-179)
    **{i: {'type': 'special', 'subtype': 'magical'} for i in range(170, 180)},
    
    # Reserved for future (180-255) - placeholder
    **{i: {'type': 'special', 'subtype': 'placeholder'} for i in range(180, 256)}
}


def generate_modular_texture(block_id, size=32, face='all', seed=None):
    """
    Generate a texture using the modular system for a specific block ID.
    
    Args:
        block_id: The VoxelType ID (0-255)
        size: Texture size in pixels (default 16x16)
        face: Which face to generate ('top', 'side', 'bottom', 'all')
        seed: Random seed for reproducible generation
    
    Returns:
        PIL Image of the generated texture
    """
    if not MODULAR_SYSTEM_AVAILABLE:
        return None
    
    if seed is not None:
        random.seed(seed)
    
    # Get block classification
    block_info = BLOCK_MAPPING.get(block_id, {'type': 'special', 'subtype': 'placeholder'})
    block_type = block_info['type']
    subtype = block_info['subtype']
    
    try:
        if block_type == 'stone':
            if 'brick' in subtype or 'tile' in subtype or 'polished' in subtype or 'smooth' in subtype:
                return generate_processed_stone_texture(size, subtype)
            else:
                return generate_stone_texture(size, subtype)
        
        elif block_type == 'wood':
            if 'planks' in subtype or 'beam' in subtype:
                wood_species = subtype.split('_')[0]  # oak, pine, etc.
                return generate_plank_texture(size, wood_species)
            else:
                return generate_wood_texture(size, subtype)
        
        elif block_type == 'organic':
            # Handle leaves with per-face logic
            if 'leaves' in subtype:
                species = subtype.split('_')[0]  # oak, pine, etc.
                return generate_organic_texture('leaves_' + species, size, face)
            else:
                return generate_organic_texture(subtype, size, face)
        
        elif block_type == 'ore':
            return generate_ore_texture(subtype, size)
        
        elif block_type == 'crystal':
            return generate_crystal_texture(subtype, size)
        
        elif block_type == 'ceramic':
            return generate_ceramic_texture(subtype, size)
        
        elif block_type == 'metal':
            return generate_metal_texture(subtype, size)
        
        elif block_type == 'fluid':
            return generate_fluid_texture(subtype, size)
        
        elif block_type == 'special':
            return generate_special_texture(subtype, size)
        
        elif block_type == 'air':
            # Transparent texture
            return Image.new("RGBA", (size, size), (0, 0, 0, 0))
        
        else:
            # Fallback - purple placeholder
            img = Image.new("RGBA", (size, size), (128, 64, 128, 255))
            return img
            
    except Exception as e:
        print(f"Warning: Failed to generate modular texture for block {block_id} ({subtype}): {e}")
        # Return purple placeholder on error
        img = Image.new("RGBA", (size, size), (160, 32, 160, 255))
        return img


def should_use_per_face_textures(block_id):
    """
    Determine if a block should use different textures for different faces.
    Currently applies to grass, logs, and some special blocks.
    """
    block_info = BLOCK_MAPPING.get(block_id, {})
    subtype = block_info.get('subtype', '')
    
    # Grass has different top/side textures
    if subtype == 'grass':
        return True
    
    # Wood logs have different end/side textures  
    if block_info.get('type') == 'wood' and 'planks' not in subtype and 'beam' not in subtype:
        return True
    
    # Some organic materials benefit from per-face variation
    if subtype in ['cactus', 'mushroom_brown', 'mushroom_red']:
        return True
    
    return False


def get_texture_for_face(block_id, face='all', size=32, seed=None):
    """
    Get the appropriate texture for a specific face of a block.
    
    Args:
        block_id: VoxelType ID
        face: 'top', 'side', 'bottom', or 'all'
        size: Texture size in pixels
        seed: Random seed
    
    Returns:
        PIL Image
    """
    if MODULAR_SYSTEM_AVAILABLE:
        return generate_modular_texture(block_id, size, face, seed)
    else:
        # Fall back to legacy generation
        return generate_legacy_texture(block_id, size)


def generate_legacy_texture(block_id, size=32):
    """
    Generate textures using the legacy system for backwards compatibility.
    """
    # Legacy color definitions (preserving original mapping)
    legacy_colors = {
        0: (0, 0, 0, 0),          # AIR
        1: (128, 128, 128, 255),  # STONE
        2: (139, 69, 19, 255),    # DIRT
        3: (0, 128, 0, 255),      # GRASS
        4: (244, 164, 96, 255),   # SAND
        5: (128, 128, 128, 255),  # GRAVEL
        6: (139, 90, 43, 255),    # CLAY
        7: (32, 32, 32, 255),     # BEDROCK
        8: (101, 67, 33, 255),    # TOPSOIL
        9: (139, 90, 43, 255),    # SUBSOIL
        
        # Stone varieties with appropriate colors
        10: (169, 169, 169, 255), # GRANITE
        11: (240, 240, 240, 255), # LIMESTONE  
        12: (255, 255, 255, 255), # MARBLE
        13: (194, 178, 128, 255), # SANDSTONE
        14: (47, 79, 79, 255),    # SLATE
        15: (47, 79, 79, 255),    # BASALT
        16: (245, 245, 245, 255), # QUARTZITE
        17: (25, 25, 25, 255),    # OBSIDIAN
        18: (211, 211, 211, 255), # PUMICE
        19: (105, 105, 105, 255), # SHALE
        
        # Ores
        20: (64, 64, 64, 255),    # COAL_ORE
        21: (205, 133, 63, 255),  # IRON_ORE
        22: (184, 115, 51, 255),  # COPPER_ORE
        23: (192, 192, 192, 255), # TIN_ORE
        24: (192, 192, 192, 255), # SILVER_ORE
        25: (255, 215, 0, 255),   # GOLD_ORE
        26: (220, 20, 60, 255),   # GEM_RUBY
        27: (65, 105, 225, 255),  # GEM_SAPPHIRE
        28: (50, 205, 50, 255),   # GEM_EMERALD
        29: (185, 242, 255, 255), # GEM_DIAMOND
        
        # Wood
        30: (139, 69, 19, 255),   # WOOD_OAK
        31: (160, 82, 45, 255),   # WOOD_PINE
        32: (245, 245, 220, 255), # WOOD_BIRCH
        33: (117, 42, 42, 255),   # WOOD_MAHOGANY
        
        # Leaves
        34: (34, 139, 34, 255),   # LEAVES_OAK
        35: (0, 100, 0, 255),     # LEAVES_PINE
        36: (154, 205, 50, 255),  # LEAVES_BIRCH
        37: (46, 125, 50, 255),   # LEAVES_PALM
        
        # Fluids
        50: (30, 144, 255, 255),  # WATER
        51: (255, 69, 0, 255),    # LAVA
    }
    
    base_color = legacy_colors.get(block_id, (128, 64, 128, 255))  # Purple placeholder
    
    # Create simple colored texture
    img = Image.new("RGBA", (size, size), base_color)
    
    # Add basic patterns for specific blocks
    if block_id == 1:  # STONE - add speckles
        draw = ImageDraw.Draw(img)
        for _ in range(size // 2):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            darker = tuple(max(0, c-30) for c in base_color[:3]) + (255,)
            draw.point((x, y), darker)
    
    return img


def generate_texture_atlas(output_path="assets/textures/atlas_32x32.png", use_per_face=False):
    """
    Generate a comprehensive texture atlas for all 256 block types.
    
    Args:
        output_path: Where to save the atlas
        use_per_face: If True, generate separate atlases for different faces
    """
    atlas_size_px = 1024  # 1024x1024 atlas for 32x32 tiles
    tile_size_px = 32     # 32x32 pixel tiles for high detail
    tiles_per_row = atlas_size_px // tile_size_px  # 32x32 grid = 1024 total tiles
    
    print(f"Generating texture atlas with {tiles_per_row}x{tiles_per_row} = {tiles_per_row * tiles_per_row} tiles")
    print(f"Atlas size: {atlas_size_px}x{atlas_size_px}, Tile size: {tile_size_px}x{tile_size_px}")
    print(f"Using {'modular' if MODULAR_SYSTEM_AVAILABLE else 'legacy'} texture generation system")
    
    if use_per_face and MODULAR_SYSTEM_AVAILABLE:
        # Generate separate atlases for top, side, and bottom faces
        faces = ['top', 'side', 'bottom']
        for face in faces:
            face_output = output_path.replace('.png', f'_{face}.png')
            generate_single_atlas(face_output, face, atlas_size_px, tile_size_px, tiles_per_row)
    else:
        # Generate single unified atlas
        generate_single_atlas(output_path, 'all', atlas_size_px, tile_size_px, tiles_per_row)


def generate_single_atlas(output_path, face, atlas_size_px, tile_size_px, tiles_per_row):
    """Generate a single atlas for the specified face."""
    
    # Create atlas image
    atlas_image = Image.new("RGBA", (atlas_size_px, atlas_size_px), (0, 0, 0, 0))
    
    legacy_count = 0
    modular_count = 0
    placeholder_count = 0
    
    print(f"Generating {face} face atlas...")
    
    for tile_y_idx in range(tiles_per_row):
        for tile_x_idx in range(tiles_per_row):
            voxel_id = tile_y_idx * tiles_per_row + tile_x_idx
            
            # Calculate pixel coordinates for the tile
            x0 = tile_x_idx * tile_size_px
            y0 = tile_y_idx * tile_size_px
            
            # Generate texture using modular system
            try:
                # Use consistent seed based on block ID for reproducibility
                texture_seed = voxel_id * 12345 + hash(face) % 1000
                texture = get_texture_for_face(voxel_id, face, tile_size_px, texture_seed)
                
                if texture is not None:
                    atlas_image.paste(texture, (x0, y0))
                    
                    # Count generation method used
                    if voxel_id < 60:  # Legacy blocks
                        legacy_count += 1
                    elif voxel_id < 180:  # Defined modular blocks
                        modular_count += 1
                    else:  # Placeholder blocks
                        placeholder_count += 1
                else:
                    # Fallback for failed generation
                    create_placeholder_texture(atlas_image, x0, y0, tile_size_px, voxel_id)
                    placeholder_count += 1
                    
            except Exception as e:
                print(f"Error generating texture for block {voxel_id}: {e}")
                create_placeholder_texture(atlas_image, x0, y0, tile_size_px, voxel_id)
                placeholder_count += 1
    
    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    # Save the atlas
    atlas_image.save(output_path)
    
    print(f"✓ {face.capitalize()} atlas saved to {output_path}")
    print(f"  Legacy textures: {legacy_count}")
    print(f"  Modular textures: {modular_count}")
    print(f"  Placeholder textures: {placeholder_count}")
    print(f"  Total: {legacy_count + modular_count + placeholder_count}")


def create_placeholder_texture(atlas_image, x0, y0, tile_size_px, voxel_id):
    """Create a placeholder texture for blocks without specific generation."""
    # Purple checkered pattern with block ID
    checker_size = max(1, tile_size_px // 4)
    draw = ImageDraw.Draw(atlas_image)
    
    for cy in range(0, tile_size_px, checker_size):
        for cx in range(0, tile_size_px, checker_size):
            checker_x = (cx // checker_size) % 2
            checker_y = (cy // checker_size) % 2
            if (checker_x + checker_y) % 2 == 0:
                color = (128, 64, 128, 255)  # Purple
            else:
                color = (160, 32, 160, 255)  # Darker purple
            
            draw.rectangle([
                x0 + cx, y0 + cy, 
                x0 + cx + checker_size - 1, y0 + cy + checker_size - 1
            ], fill=color)
    
    # Add small block ID number if space allows
    if tile_size_px >= 8:
        try:
            # Simple text representation using pixels (for very small ID numbers)
            if voxel_id < 10:
                # Draw single digit
                draw_simple_digit(draw, x0 + 1, y0 + 1, voxel_id, (255, 255, 255, 255))
        except:
            pass  # Skip text if there are issues


def draw_simple_digit(draw, x, y, digit, color):
    """Draw a simple pixel representation of a digit."""
    # Very basic 3x5 pixel font for digits 0-9
    patterns = {
        0: [[1,1,1], [1,0,1], [1,0,1], [1,0,1], [1,1,1]],
        1: [[0,1,0], [1,1,0], [0,1,0], [0,1,0], [1,1,1]],
        2: [[1,1,1], [0,0,1], [1,1,1], [1,0,0], [1,1,1]],
        3: [[1,1,1], [0,0,1], [1,1,1], [0,0,1], [1,1,1]],
        4: [[1,0,1], [1,0,1], [1,1,1], [0,0,1], [0,0,1]],
        5: [[1,1,1], [1,0,0], [1,1,1], [0,0,1], [1,1,1]],
        6: [[1,1,1], [1,0,0], [1,1,1], [1,0,1], [1,1,1]],
        7: [[1,1,1], [0,0,1], [0,0,1], [0,0,1], [0,0,1]],
        8: [[1,1,1], [1,0,1], [1,1,1], [1,0,1], [1,1,1]],
        9: [[1,1,1], [1,0,1], [1,1,1], [0,0,1], [1,1,1]]
    }
    
    if digit in patterns:
        pattern = patterns[digit]
        for py, row in enumerate(pattern):
            for px, pixel in enumerate(row):
                if pixel:
                    draw.point((x + px, y + py), color)


if __name__ == "__main__":
    # Assuming the script is run from the project root
    project_root = "/home/system-x1/Projects/Voxel Castle/"
    output_file_path = os.path.join(project_root, "assets/textures/atlas_32x32.png")
    
    # Generate both single and per-face atlases with new 32x32 format
    print("=== Generating single unified atlas (32x32 tiles) ===")
    generate_texture_atlas(output_path=output_file_path, use_per_face=False)
    
    print("\n=== Generating per-face atlases (32x32 tiles) ===")
    generate_texture_atlas(output_path=output_file_path, use_per_face=True)
    
    print("\n✅ Atlas generation complete!")
    print(f"Legacy atlas preserved at: assets/textures/atlas.png")
    print(f"New 32x32 atlas created at: assets/textures/atlas_32x32.png")
