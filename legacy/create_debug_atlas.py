#!/usr/bin/env python3
"""
Debug Atlas Generator
====================
Creates debug versions of texture atlases with block IDs, names and coordinates overlaid.
Uses higher resolution to ensure text is readable while preserving texture visibility.
"""

from PIL import Image, ImageDraw, ImageFont
import os
import sys
import importlib
import inspect
from create_atlas_efficient import EfficientAtlasGenerator, AtlasType

def load_dynamic_block_mapping():
    """Dynamically loads the BLOCK_MAPPING from create_atlas_efficient.py"""
    try:
        # Use importlib to ensure we get the latest version
        spec = importlib.util.find_spec('create_atlas_efficient')
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        
        # Return the BLOCK_MAPPING
        return module.BLOCK_MAPPING
    except Exception as e:
        print(f"Error dynamically loading BLOCK_MAPPING: {e}")
        return {}

def create_debug_atlas(atlas_path, output_path, atlas_type, tile_size=32, scale_factor=2):
    """
    Creates a debug version of the atlas with block information overlaid.
    
    Args:
        atlas_path: Path to the original atlas image
        output_path: Path to save the debug atlas
        atlas_type: AtlasType enum value (MAIN, SIDE, BOTTOM)
        tile_size: Size of each texture tile in pixels
        scale_factor: How much to scale up the debug atlas for better readability
    """
    if not os.path.exists(atlas_path):
        print(f"Error: Source atlas not found at {atlas_path}")
        return False
    
    # Load the atlas and get dimensions
    original_atlas = Image.open(atlas_path).convert('RGBA')
    width, height = original_atlas.size
    grid_size = width // tile_size
    
    # Create a larger debug atlas for better text visibility
    debug_tile_size = tile_size * scale_factor
    debug_width = width * scale_factor
    debug_height = height * scale_factor
    debug_atlas = Image.new('RGBA', (debug_width, debug_height), (0, 0, 0, 0))
    
    # Dynamically load the latest BLOCK_MAPPING
    block_mapping = load_dynamic_block_mapping()
    
    # Create a drawing context
    draw = ImageDraw.Draw(debug_atlas)
    
    # Try to load a font, fallback to default if not available
    try:
        # Larger font size for better readability
        font_size = max(12, debug_tile_size // 6)
        font = ImageFont.truetype("DejaVuSansMono.ttf", font_size)
    except IOError:
        try:
            # Try another common font
            font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", font_size)
        except:
            try:
                # Fall back to default
                font = ImageFont.load_default()
                print("Warning: Using default font, text may be small")
            except:
                print("Warning: Could not load font, text may not be visible")
                font = None
    
    # Create a mapping of position to block ID for this atlas
    blocks_in_atlas = {}
    generator = EfficientAtlasGenerator(tile_size)
    
    # For each block in the mapping
    for block_id, block_info in block_mapping.items():
        # Get the slot position for this block in this atlas type
        slot = None
        if atlas_type == AtlasType.MAIN:
            slot = block_id  # Main atlas uses block_id directly
        elif atlas_type == AtlasType.SIDE:
            try:
                # Only TOP_BOTTOM_DIFFERENT and ALL_DIFFERENT blocks have side textures
                pattern = generator.atlas_face_system.get_block_face_pattern(block_id)
                if pattern in [2, 3]:  # TOP_BOTTOM_DIFFERENT, ALL_DIFFERENT
                    slot = generator.atlas_face_system.get_side_atlas_slot(block_id)
            except Exception as e:
                pass
        elif atlas_type == AtlasType.BOTTOM:
            try:
                # Only ALL_DIFFERENT blocks have bottom textures 
                pattern = generator.atlas_face_system.get_block_face_pattern(block_id)
                if pattern == 3:  # ALL_DIFFERENT
                    slot = generator.atlas_face_system.get_bottom_atlas_slot(block_id)
            except:
                pass
        
        if slot is not None:
            # Convert slot to grid position
            x = slot % grid_size
            y = slot // grid_size
            
            # Get block name
            block_type = block_info.get('type', 'unknown')
            block_subtype = block_info.get('subtype', '')
            block_name = f"{block_type}"
            if block_subtype:
                block_name += f"_{block_subtype}"
            
            # Get face pattern name
            try:
                pattern_value = generator.atlas_face_system.get_block_face_pattern(block_id)
                # Map numeric pattern values to readable abbreviated names
                pattern_names = {
                    0: "UNIFORM",
                    1: "SIDES_DIFF",
                    2: "TOP_BOT_DIFF", 
                    3: "ALL_DIFF",
                    4: "DIRECT"
                }
                pattern_name = pattern_names.get(pattern_value, f"UNK:{pattern_value}")
            except Exception as e:
                pattern_name = "UNKNOWN"
            
            # Store the block info at this position
            blocks_in_atlas[(x, y)] = (block_id, block_name, pattern_name)
            
            # Copy and scale the texture tile to the debug atlas
            src_box = (x * tile_size, y * tile_size, (x + 1) * tile_size, (y + 1) * tile_size)
            dst_box = (x * debug_tile_size, y * debug_tile_size, (x + 1) * debug_tile_size, (y + 1) * debug_tile_size)
            
            # Extract the tile and scale it up
            tile = original_atlas.crop(src_box)
            scaled_tile = tile.resize((debug_tile_size, debug_tile_size), Image.NEAREST)
            debug_atlas.paste(scaled_tile, (dst_box[0], dst_box[1]))
    
    # Now draw the debug information on each block tile
    for (x, y), (block_id, block_name, pattern_name) in blocks_in_atlas.items():
        # Calculate pixel coordinates in the scaled atlas
        px = x * debug_tile_size
        py = y * debug_tile_size
        
        # No overlay background or indicators, just text with a subtle outline for visibility
        
        # Draw text with shadow outline effect for better readability without backgrounds
        if font:
            # Draw block ID and coordinates
            id_text = f"ID:{block_id}"
            coord_text = f"({x},{y})"
            
            # Function to draw text with outline/shadow for better visibility on any background
            def draw_outlined_text(x, y, text, fill_color=(255, 255, 255, 255)):
                # Draw shadow/outline first
                shadow_color = (0, 0, 0, 200)
                offset = 1
                draw.text((x+offset, y+offset), text, fill=shadow_color, font=font)
                # Draw main text
                draw.text((x, y), text, fill=fill_color, font=font)
            
            # Draw all text with outlines
            draw_outlined_text(px + 5, py + 2, id_text)
            draw_outlined_text(px + 5, py + font_size + 2, coord_text)
            draw_outlined_text(px + 5, py + font_size*2 + 2, block_name[:15])
            
            # Add atlas type prefix to differentiate between atlas types
            atlas_prefixes = {
                AtlasType.MAIN: "M",
                AtlasType.SIDE: "S",
                AtlasType.BOTTOM: "B"
            }
            atlas_prefix = atlas_prefixes.get(atlas_type, "?")
            
            # Add pattern type in bottom right with smaller font
            pattern_font_size = max(8, font_size // 2)
            pattern_font = None
            try:
                pattern_font = ImageFont.truetype("DejaVuSansMono.ttf", pattern_font_size)
            except:
                pattern_font = font
                
            if pattern_font:
                # Add pattern info at the bottom of the tile
                pattern_y = py + debug_tile_size - pattern_font_size - 5
                pattern_text = f"{atlas_prefix}:{pattern_name}"
                
                # Draw with outline for visibility
                shadow_color = (0, 0, 0, 200)
                offset = 1
                draw.text((px + 5 + offset, pattern_y + offset), pattern_text, fill=shadow_color, font=pattern_font)
                draw.text((px + 5, pattern_y), pattern_text, fill=(255, 255, 0, 255), font=pattern_font)
    
    # Save the debug atlas
    debug_atlas.save(output_path)
    print(f"✓ Debug atlas saved to {output_path} ({debug_width}x{debug_height}px)")
    return True

def create_all_debug_atlases(scale_factor=2):
    """Creates debug versions of all atlases.
    
    Args:
        scale_factor: How much to scale up the debug atlas (2x, 3x, etc.)
    """
    print(f"🔍 Generating debug atlases with block information (scaled {scale_factor}x)...")
    
    # Define atlas paths
    atlas_dir = "assets/textures"
    debug_dir = "assets/textures/debug"
    
    # Create debug directory if it doesn't exist
    if not os.path.exists(debug_dir):
        os.makedirs(debug_dir)
    
    # Load atlas names dynamically from directory
    main_atlas = None
    side_atlas = None
    bottom_atlas = None
    
    for filename in os.listdir(atlas_dir):
        if filename.startswith("atlas_main_"):
            main_atlas = filename
        elif filename.startswith("atlas_side_"):
            side_atlas = filename
        elif filename.startswith("atlas_bottom_"):
            bottom_atlas = filename
    
    if not main_atlas:
        print("Main atlas not found. Generating atlas first...")
        import create_atlas_efficient
        create_atlas_efficient.main()
        
        # Try again after generation
        for filename in os.listdir(atlas_dir):
            if filename.startswith("atlas_main_"):
                main_atlas = filename
            elif filename.startswith("atlas_side_"):
                side_atlas = filename
            elif filename.startswith("atlas_bottom_"):
                bottom_atlas = filename
    
    # Create debug versions of all atlases
    atlases = []
    if main_atlas:
        atlases.append((main_atlas, f"debug_atlas_main_{scale_factor}x.png", AtlasType.MAIN))
    if side_atlas: 
        atlases.append((side_atlas, f"debug_atlas_side_{scale_factor}x.png", AtlasType.SIDE))
    if bottom_atlas:
        atlases.append((bottom_atlas, f"debug_atlas_bottom_{scale_factor}x.png", AtlasType.BOTTOM))
    
    for src_name, dst_name, atlas_type in atlases:
        src_path = os.path.join(atlas_dir, src_name)
        dst_path = os.path.join(debug_dir, dst_name)
        
        if create_debug_atlas(src_path, dst_path, atlas_type, scale_factor=scale_factor):
            print(f"  ✓ Created {dst_name}")
        else:
            print(f"  ✗ Failed to create {dst_name}")

if __name__ == "__main__":
    # Get scale factor from command line if provided
    scale_factor = 2  # Default scale factor
    if len(sys.argv) > 1:
        try:
            scale_factor = int(sys.argv[1])
            if scale_factor < 1:
                print("Scale factor must be >= 1, using default of 2")
                scale_factor = 2
        except ValueError:
            print("Invalid scale factor, using default of 2")
    
    # Create debug atlases
    create_all_debug_atlases(scale_factor)
    
    print(f"✅ Debug atlas generation complete! Check assets/textures/debug/ folder.")
    print(f"   Debug atlases are scaled {scale_factor}x for better text visibility.")
