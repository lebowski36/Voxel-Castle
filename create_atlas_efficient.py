#!/usr/bin/env python3
"""
Efficient Face-Based Atlas Generator
Updated create_atlas.py that uses the new efficient face-based atlas system.
Generates optimally packed atlases with minimal wasted space.
"""

from PIL import Image, ImageDraw
import os
import sys
import random
import math

# Import our new atlas face system
from atlas_face_system import (
    AtlasSlotAllocator, AtlasType, FacePattern, 
    get_atlas_requirements, calculate_atlas_grid_size
)

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

# Block type classification for texture generation (same as before)
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
    
    # Continue with all blocks... (truncated for brevity)
    # Wood Processing (70-79)
    70: {'type': 'wood', 'subtype': 'oak_planks'},
    71: {'type': 'wood', 'subtype': 'pine_planks'},
    72: {'type': 'wood', 'subtype': 'birch_planks'},
    73: {'type': 'wood', 'subtype': 'mahogany_planks'},
    74: {'type': 'wood', 'subtype': 'oak_beam'},
    75: {'type': 'wood', 'subtype': 'pine_beam'},
    76: {'type': 'wood', 'subtype': 'hardwood_beam'},
    
    # Metal Blocks (80-89)
    80: {'type': 'metal', 'subtype': 'iron'},
    81: {'type': 'metal', 'subtype': 'copper'},
    82: {'type': 'metal', 'subtype': 'bronze'},
    83: {'type': 'metal', 'subtype': 'steel'},
    84: {'type': 'metal', 'subtype': 'silver'},
    85: {'type': 'metal', 'subtype': 'gold'},
}

# Fill in remaining blocks with placeholders
for i in range(256):
    if i not in BLOCK_MAPPING:
        BLOCK_MAPPING[i] = {'type': 'special', 'subtype': 'placeholder'}

class EfficientAtlasGenerator:
    """
    Generates efficient face-based atlases using the new slot allocation system.
    """
    
    def __init__(self, tile_size_px=32, print_summary=False):
        self.tile_size_px = tile_size_px
        self.allocator = AtlasSlotAllocator()
        # Use the silent calculation method to avoid repeated output
        self.allocator._calculate_requirements()
        
        # Get requirements for each atlas type (using our existing allocator)
        self.requirements = {
            AtlasType.MAIN: self.allocator.stats['main_slots_used'],
            AtlasType.SIDE: self.allocator.stats['side_slots_used'],
            AtlasType.BOTTOM: self.allocator.stats['bottom_slots_used']
        }
        
        # Print allocation summary only if requested
        if print_summary:
            self.allocator._print_allocation_summary()
            
            # Calculate and print grid sizes for each atlas
            for atlas_type, num_slots in self.requirements.items():
                if num_slots > 0:
                    grid_w, grid_h = calculate_atlas_grid_size(num_slots)
                    print(f"  {atlas_type.value:6s} atlas: {num_slots:3d} slots -> {grid_w}x{grid_h} grid ({grid_w*tile_size_px}x{grid_h*tile_size_px}px)")
        
        # Calculate grid sizes for each atlas (always needed)
        self.atlas_grids = {}
        for atlas_type, num_slots in self.requirements.items():
            if num_slots > 0:
                grid_w, grid_h = calculate_atlas_grid_size(num_slots)
                self.atlas_grids[atlas_type] = (grid_w, grid_h)
        
        # Statistics
        self.stats = {
            'textures_generated': 0,
            'textures_failed': 0,
            'generation_methods': {'modular': 0, 'legacy': 0, 'placeholder': 0}
        }
    
    def generate_all_atlases(self, output_dir="assets/textures/"):
        """Generate all required atlases for the efficient face-based system."""
        print(f"\n🎨 Generating Efficient Face-Based Atlases (32x32 tiles)")
        
        # Ensure output directory exists
        os.makedirs(output_dir, exist_ok=True)
        
        # Generate each required atlas
        atlases_generated = []
        
        if AtlasType.MAIN in self.atlas_grids:
            main_path = os.path.join(output_dir, "atlas_main_16x16.png")
            self.generate_atlas(AtlasType.MAIN, main_path)
            atlases_generated.append(("Main", main_path))
        
        if AtlasType.SIDE in self.atlas_grids:
            side_path = os.path.join(output_dir, "atlas_side_3x3.png")
            self.generate_atlas(AtlasType.SIDE, side_path)
            atlases_generated.append(("Side", side_path))
        
        if AtlasType.BOTTOM in self.atlas_grids:
            bottom_path = os.path.join(output_dir, "atlas_bottom_1x1.png")
            self.generate_atlas(AtlasType.BOTTOM, bottom_path)
            atlases_generated.append(("Bottom", bottom_path))
        
        self.print_generation_summary(atlases_generated)
        return atlases_generated
    
    def generate_atlas(self, atlas_type: AtlasType, output_path: str):
        """Generate a specific atlas type."""
        if atlas_type not in self.atlas_grids:
            print(f"⚠ Skipping {atlas_type.value} atlas (no slots required)")
            return
        
        grid_w, grid_h = self.atlas_grids[atlas_type]
        atlas_size_px = (grid_w * self.tile_size_px, grid_h * self.tile_size_px)
        
        print(f"📋 Generating {atlas_type.value} atlas ({grid_w}x{grid_h} grid, {atlas_size_px[0]}x{atlas_size_px[1]}px)...")
        
        # Create atlas image
        atlas_image = Image.new("RGBA", atlas_size_px, (0, 0, 0, 0))
        
        # Get all voxel IDs that need this atlas type
        voxel_ids_for_atlas = self.get_voxel_ids_for_atlas(atlas_type)
        
        # Generate textures for each required slot
        for voxel_id in voxel_ids_for_atlas:
            slot = self.allocator.get_texture_slot(voxel_id, atlas_type)
            if slot is not None:
                self.generate_texture_for_slot(atlas_image, voxel_id, atlas_type, slot, grid_w)
        
        # Save the atlas
        atlas_image.save(output_path)
        print(f"✓ {atlas_type.value} atlas saved to {output_path}")
    
    def get_voxel_ids_for_atlas(self, atlas_type: AtlasType) -> list:
        """Get all voxel IDs that need textures in this atlas type."""
        voxel_ids = []
        
        for voxel_id in range(256):
            if self.allocator.get_texture_slot(voxel_id, atlas_type) is not None:
                voxel_ids.append(voxel_id)
        
        return voxel_ids
    
    def generate_texture_for_slot(self, atlas_image: Image.Image, voxel_id: int, 
                                atlas_type: AtlasType, slot: int, grid_w: int):
        """Generate a texture for a specific atlas slot."""
        # Calculate pixel coordinates for the slot
        slot_x = slot % grid_w
        slot_y = slot // grid_w
        x0 = slot_x * self.tile_size_px
        y0 = slot_y * self.tile_size_px
        
        # Determine which face type to generate
        face_type = self.get_face_type_for_atlas(atlas_type, voxel_id)
        
        try:
            # Use consistent seed based on voxel ID and face type for reproducibility
            texture_seed = voxel_id * 12345 + hash(face_type) % 1000
            
            # Generate texture using modular system
            texture = self.get_texture_for_face(voxel_id, face_type, texture_seed)
            
            if texture is not None:
                atlas_image.paste(texture, (x0, y0))
                self.stats['textures_generated'] += 1
                
                # Count generation method
                if MODULAR_SYSTEM_AVAILABLE:
                    self.stats['generation_methods']['modular'] += 1
                else:
                    self.stats['generation_methods']['legacy'] += 1
            else:
                raise Exception("Texture generation returned None")
                
        except Exception as e:
            print(f"⚠ Error generating texture for voxel {voxel_id} ({face_type}): {e}")
            self.create_placeholder_texture(atlas_image, x0, y0, voxel_id)
            self.stats['textures_failed'] += 1
            self.stats['generation_methods']['placeholder'] += 1
    
    def get_face_type_for_atlas(self, atlas_type: AtlasType, voxel_id: int) -> str:
        """Determine which face type to generate for this atlas."""
        if atlas_type == AtlasType.MAIN:
            return 'top'  # Main atlas contains top/primary faces
        elif atlas_type == AtlasType.SIDE:
            return 'side'
        elif atlas_type == AtlasType.BOTTOM:
            return 'bottom'
        return 'top'
    
    def get_texture_for_face(self, block_id: int, face: str, seed: int = None) -> Image.Image:
        """Generate texture for a specific face of a block."""
        if MODULAR_SYSTEM_AVAILABLE:
            return self.generate_modular_texture(block_id, face, seed)
        else:
            return self.generate_legacy_texture(block_id)
    
    def generate_modular_texture(self, block_id: int, face: str, seed: int = None) -> Image.Image:
        """Generate texture using the modular system."""
        if seed is not None:
            random.seed(seed)
        
        # Get block classification
        block_info = BLOCK_MAPPING.get(block_id, {'type': 'special', 'subtype': 'placeholder'})
        block_type = block_info['type']
        subtype = block_info['subtype']
        
        try:
            if block_type == 'stone':
                if 'brick' in subtype or 'tile' in subtype or 'polished' in subtype or 'smooth' in subtype:
                    return generate_processed_stone_texture(self.tile_size_px, subtype)
                else:
                    return generate_stone_texture(self.tile_size_px, subtype)
            
            elif block_type == 'wood':
                if 'planks' in subtype or 'beam' in subtype:
                    wood_species = subtype.split('_')[0]  # oak, pine, etc.
                    return generate_plank_texture(self.tile_size_px, wood_species)
                else:
                    return generate_wood_texture(self.tile_size_px, subtype, face)
            
            elif block_type == 'organic':
                # Handle leaves and grass with per-face logic
                if 'leaves' in subtype:
                    species = subtype.split('_')[0]  # oak, pine, etc.
                    return generate_organic_texture('leaves_' + species, self.tile_size_px, face)
                elif subtype == 'grass':
                    return generate_organic_texture(subtype, self.tile_size_px, face)
                else:
                    return generate_organic_texture(subtype, self.tile_size_px, face)
            
            elif block_type == 'ore':
                return generate_ore_texture(subtype, self.tile_size_px)
            
            elif block_type == 'crystal':
                return generate_crystal_texture(subtype, self.tile_size_px)
            
            elif block_type == 'ceramic':
                return generate_ceramic_texture(subtype, self.tile_size_px)
            
            elif block_type == 'metal':
                return generate_metal_texture(subtype, self.tile_size_px)
            
            elif block_type == 'fluid':
                return generate_fluid_texture(subtype, self.tile_size_px)
            
            elif block_type == 'special':
                return generate_special_texture(subtype, self.tile_size_px)
            
            elif block_type == 'air':
                # Transparent texture
                return Image.new("RGBA", (self.tile_size_px, self.tile_size_px), (0, 0, 0, 0))
            
            else:
                # Fallback - purple placeholder
                return Image.new("RGBA", (self.tile_size_px, self.tile_size_px), (128, 64, 128, 255))
                
        except Exception as e:
            print(f"Warning: Failed to generate modular texture for block {block_id} ({subtype}): {e}")
            return Image.new("RGBA", (self.tile_size_px, self.tile_size_px), (160, 32, 160, 255))
    
    def generate_legacy_texture(self, block_id: int) -> Image.Image:
        """Generate texture using legacy system."""
        # Simple color-based textures for basic compatibility
        legacy_colors = {
            0: (0, 0, 0, 0),          # AIR
            1: (128, 128, 128, 255),  # STONE
            2: (139, 69, 19, 255),    # DIRT
            3: (0, 128, 0, 255),      # GRASS
            4: (244, 164, 96, 255),   # SAND
        }
        
        base_color = legacy_colors.get(block_id, (128, 64, 128, 255))  # Purple placeholder
        return Image.new("RGBA", (self.tile_size_px, self.tile_size_px), base_color)
    
    def create_placeholder_texture(self, atlas_image: Image.Image, x0: int, y0: int, voxel_id: int):
        """Create a placeholder texture for failed generation."""
        checker_size = max(1, self.tile_size_px // 4)
        draw = ImageDraw.Draw(atlas_image)
        
        for cy in range(0, self.tile_size_px, checker_size):
            for cx in range(0, self.tile_size_px, checker_size):
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
    
    def print_generation_summary(self, atlases_generated: list):
        """Print a summary of atlas generation."""
        print(f"\n✅ Atlas Generation Complete!")
        print(f"📁 Generated Files:")
        for name, path in atlases_generated:
            file_size = os.path.getsize(path) if os.path.exists(path) else 0
            print(f"  {name:6s}: {path} ({file_size:,} bytes)")
        
        print(f"\n📊 Generation Statistics:")
        print(f"  Textures generated: {self.stats['textures_generated']}")
        print(f"  Textures failed:    {self.stats['textures_failed']}")
        print(f"  Modular textures:   {self.stats['generation_methods']['modular']}")
        print(f"  Legacy textures:    {self.stats['generation_methods']['legacy']}")
        print(f"  Placeholder textures: {self.stats['generation_methods']['placeholder']}")
        
        total_pixels = sum((grid_w * grid_h * self.tile_size_px * self.tile_size_px) 
                          for grid_w, grid_h in self.atlas_grids.values())
        print(f"  Total atlas pixels: {total_pixels:,}")

def generate_legacy_atlas_for_compatibility(output_path="assets/textures/atlas_legacy_16x16.png"):
    """Generate a legacy-compatible atlas for backwards compatibility."""
    print(f"\n🔄 Generating Legacy Compatibility Atlas...")
    
    # Use old 16x16 format for compatibility
    atlas_size_px = 512  # 16x16 grid of 32x32 tiles
    tile_size_px = 32
    tiles_per_row = 16
    
    atlas_image = Image.new("RGBA", (atlas_size_px, atlas_size_px), (0, 0, 0, 0))
    
    # Generate textures for the first 256 blocks (legacy format)
    for tile_y_idx in range(tiles_per_row):
        for tile_x_idx in range(tiles_per_row):
            voxel_id = tile_y_idx * tiles_per_row + tile_x_idx
            
            x0 = tile_x_idx * tile_size_px
            y0 = tile_y_idx * tile_size_px
            
            # Generate texture (use 'top' face as default)
            try:
                generator = EfficientAtlasGenerator(tile_size_px)
                texture = generator.get_texture_for_face(voxel_id, 'top', voxel_id * 12345)
                if texture:
                    atlas_image.paste(texture, (x0, y0))
                else:
                    raise Exception("No texture generated")
            except Exception as e:
                # Create placeholder
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
    
    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    atlas_image.save(output_path)
    print(f"✓ Legacy atlas saved to {output_path}")

if __name__ == "__main__":
    print("🎨 Efficient Face-Based Atlas Generator")
    print("=" * 50)
    
    # Generate the new efficient atlases
    generator = EfficientAtlasGenerator(tile_size_px=32, print_summary=True)
    atlases = generator.generate_all_atlases()
    
    # Also generate legacy compatibility atlas
    generate_legacy_atlas_for_compatibility()
    
    print(f"\n🎯 Atlas generation complete! Check assets/textures/ for results.")
