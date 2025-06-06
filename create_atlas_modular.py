"""
New Modular Atlas Generator
==========================

Generates texture atlas using the modular texture generation system.
This replaces create_atlas.py with support for 256+ block types and per-face textures.
"""

import os
import sys
from PIL import Image
from typing import Dict, List, Tuple

# Add texture_generators to path
sys.path.append(os.path.join(os.path.dirname(__file__), 'texture_generators'))

from texture_generators.texture_coordinator import TextureCoordinator

def create_modular_atlas(tile_size: int = 32, output_path: str = "assets/textures/block_atlas.png", 
                        seed: int = 42) -> None:
    """
    Create texture atlas using the modular generation system.
    
    Args:
        tile_size: Size of each texture tile in pixels
        output_path: Path to save the generated atlas
        seed: Random seed for reproducible generation
    """
    
    print(f"Generating modular texture atlas with tile size {tile_size}x{tile_size}")
    print(f"Using seed: {seed}")
    
    # Initialize texture coordinator
    coordinator = TextureCoordinator(tile_size=tile_size, seed=seed)
    
    # Get all available voxel types
    voxel_types = list(coordinator.texture_map.keys())
    voxel_types.sort()  # Ensure consistent ordering
    
    print(f"Generating textures for {len(voxel_types)} voxel types")
    
    # Calculate atlas dimensions
    # We need 3 faces per voxel type (top, bottom, side)
    # For now, let's arrange in a grid that fits all textures
    total_textures = len(voxel_types) * 3  # 3 faces per type
    
    # Calculate grid dimensions (try to make it roughly square)
    import math
    grid_width = math.ceil(math.sqrt(total_textures))
    grid_height = math.ceil(total_textures / grid_width)
    
    print(f"Atlas grid: {grid_width}x{grid_height} = {grid_width * grid_height} slots")
    print(f"Atlas size: {grid_width * tile_size}x{grid_height * tile_size} pixels")
    
    # Create atlas image
    atlas_width = grid_width * tile_size
    atlas_height = grid_height * tile_size
    atlas = Image.new('RGBA', (atlas_width, atlas_height), (0, 0, 0, 0))
    
    # Generate mapping file data
    mapping_data = []
    mapping_data.append("# Modular Block Texture Atlas Mapping")
    mapping_data.append(f"# Generated with tile_size={tile_size}, seed={seed}")
    mapping_data.append(f"# Atlas size: {atlas_width}x{atlas_height}")
    mapping_data.append(f"# Format: voxel_type_id:face:x:y:width:height")
    mapping_data.append("")
    
    # Generate textures and place them in atlas
    current_slot = 0
    
    for voxel_type_id in voxel_types:
        print(f"Generating textures for voxel type {voxel_type_id}...")
        
        # Generate all three faces
        faces = ['top', 'bottom', 'side']
        
        for face in faces:
            if current_slot >= grid_width * grid_height:
                print("Warning: Ran out of atlas slots!")
                break
                
            # Calculate position in atlas
            grid_x = current_slot % grid_width
            grid_y = current_slot // grid_width
            atlas_x = grid_x * tile_size
            atlas_y = grid_y * tile_size
            
            # Generate texture for this face
            try:
                texture = coordinator.generate_texture(voxel_type_id, face)
                
                # Paste into atlas
                atlas.paste(texture, (atlas_x, atlas_y))
                
                # Add to mapping
                mapping_data.append(f"{voxel_type_id}:{face}:{atlas_x}:{atlas_y}:{tile_size}:{tile_size}")
                
            except Exception as e:
                print(f"Error generating texture for type {voxel_type_id}, face {face}: {e}")
                # Create error texture (bright red)
                error_texture = Image.new('RGBA', (tile_size, tile_size), (255, 0, 0, 255))
                atlas.paste(error_texture, (atlas_x, atlas_y))
                mapping_data.append(f"{voxel_type_id}:{face}:{atlas_x}:{atlas_y}:{tile_size}:{tile_size} # ERROR")
            
            current_slot += 1
    
    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    # Save atlas
    atlas.save(output_path)
    print(f"Atlas saved to: {output_path}")
    
    # Save mapping file
    mapping_path = output_path.replace('.png', '_mapping.txt')
    with open(mapping_path, 'w') as f:
        f.write('\n'.join(mapping_data))
    print(f"Mapping saved to: {mapping_path}")
    
    # Print summary
    print(f"\nAtlas generation complete!")
    print(f"- Generated {current_slot} textures")
    print(f"- Atlas size: {atlas_width}x{atlas_height}")
    print(f"- Tile size: {tile_size}x{tile_size}")
    print(f"- Seed used: {seed}")

def create_preview_sheet(tile_size: int = 64, output_path: str = "assets/textures/preview_sheet.png",
                        seed: int = 42) -> None:
    """
    Create a preview sheet showing all textures with labels.
    Useful for visual verification of the texture generation.
    """
    from PIL import ImageFont, ImageDraw
    
    print(f"Generating preview sheet with tile size {tile_size}x{tile_size}")
    
    # Initialize coordinator
    coordinator = TextureCoordinator(tile_size=tile_size, seed=seed)
    
    # Get voxel types
    voxel_types = list(coordinator.texture_map.keys())[:60]  # Limit for preview
    voxel_types.sort()
    
    # Calculate grid (6 columns for preview)
    cols = 6
    rows = (len(voxel_types) + cols - 1) // cols
    
    # Calculate sheet size (add space for labels)
    label_height = 20
    cell_width = tile_size + 10  # padding
    cell_height = tile_size + label_height + 10
    
    sheet_width = cols * cell_width
    sheet_height = rows * cell_height
    
    # Create preview sheet
    sheet = Image.new('RGB', (sheet_width, sheet_height), (255, 255, 255))
    draw = ImageDraw.Draw(sheet)
    
    # Try to load a font
    try:
        font = ImageFont.truetype("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 12)
    except:
        font = ImageFont.load_default()
    
    # Generate preview textures
    for i, voxel_type_id in enumerate(voxel_types):
        row = i // cols
        col = i % cols
        
        x = col * cell_width + 5
        y = row * cell_height + 5
        
        try:
            # Generate 'side' face as representative texture
            texture = coordinator.generate_texture(voxel_type_id, 'side')
            sheet.paste(texture, (x, y))
            
            # Add label
            label_y = y + tile_size + 2
            draw.text((x, label_y), f"ID:{voxel_type_id}", fill=(0, 0, 0), font=font)
            
        except Exception as e:
            # Draw error indicator
            draw.rectangle([x, y, x + tile_size - 1, y + tile_size - 1], fill=(255, 0, 0))
            draw.text((x, y + tile_size + 2), f"ID:{voxel_type_id} ERR", fill=(255, 0, 0), font=font)
    
    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    # Save preview
    sheet.save(output_path)
    print(f"Preview sheet saved to: {output_path}")

if __name__ == "__main__":
    # Default generation
    print("Generating modular texture atlas...")
    
    # Create main atlas
    create_modular_atlas(
        tile_size=32,
        output_path="assets/textures/block_atlas_modular.png",
        seed=42
    )
    
    # Create preview sheet
    create_preview_sheet(
        tile_size=64,
        output_path="assets/textures/block_preview_modular.png",
        seed=42
    )
    
    print("All textures generated successfully!")
