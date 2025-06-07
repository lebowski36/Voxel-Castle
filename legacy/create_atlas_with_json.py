#!/usr/bin/env python3
"""
Create Atlas with JSON Data
Combines the efficient face-based atlas system with our new JSON block data.
Generates properly sized main, side, and bottom atlases.
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

# Import the JSON-to-legacy bridge
from scripts.json_to_block_mapping import load_unified_block_data, convert_to_block_mapping

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

class JsonAtlasGenerator:
    """Atlas generator that uses JSON block data and efficient multi-atlas system."""
    
    def __init__(self, data_dir="data", tile_size_px=32, print_summary=True):
        self.data_dir = data_dir
        self.tile_size_px = tile_size_px
        self.print_summary = print_summary
        
        # Load block data from JSON
        print("🔄 Loading block data from JSON...")
        self.blocks_data = load_unified_block_data(data_dir)
        self.block_mapping, self.face_patterns = convert_to_block_mapping(self.blocks_data)
        print(f"   📖 Loaded {len(self.blocks_data)} blocks from JSON data")
        
        # Initialize atlas system
        self.atlas_grids = {}
        
        # Calculate required atlases
        self._setup_atlas_grids()
        
        # Initialize texture coordinator
        if MODULAR_SYSTEM_AVAILABLE:
            self.texture_coordinator = SimpleTextureCoordinator(
                tile_size=tile_size_px
            )
    
    def _setup_atlas_grids(self):
        """Calculate required atlas grid sizes based on face patterns."""
        # Count blocks by atlas type
        main_blocks = []
        side_blocks = []
        bottom_blocks = []
        
        for block_id, face_pattern in self.face_patterns.items():
            # All blocks need main atlas
            main_blocks.append(block_id)
            
            # Blocks with different side faces need side atlas
            if face_pattern in ['ALL_DIFFERENT', 'TOP_BOTTOM_DIFFERENT']:
                side_blocks.append(block_id)
            
            # Blocks with different bottom faces need bottom atlas
            if face_pattern == 'ALL_DIFFERENT':
                bottom_blocks.append(block_id)
        
        # Calculate grid sizes
        if main_blocks:
            self.atlas_grids[AtlasType.MAIN] = calculate_atlas_grid_size(len(main_blocks))
        if side_blocks:
            self.atlas_grids[AtlasType.SIDE] = calculate_atlas_grid_size(len(side_blocks))
        if bottom_blocks:
            self.atlas_grids[AtlasType.BOTTOM] = calculate_atlas_grid_size(len(bottom_blocks))
        
        if self.print_summary:
            print(f"   📊 MAIN: {self.atlas_grids.get(AtlasType.MAIN, (0,0))[0]}x{self.atlas_grids.get(AtlasType.MAIN, (0,0))[1]} grid ({len(main_blocks)} blocks)")
            if side_blocks:
                print(f"   📊 SIDE: {self.atlas_grids.get(AtlasType.SIDE, (0,0))[0]}x{self.atlas_grids.get(AtlasType.SIDE, (0,0))[1]} grid ({len(side_blocks)} blocks)")
            if bottom_blocks:
                print(f"   📊 BOTTOM: {self.atlas_grids.get(AtlasType.BOTTOM, (0,0))[0]}x{self.atlas_grids.get(AtlasType.BOTTOM, (0,0))[1]} grid ({len(bottom_blocks)} blocks)")
    
    def generate_all_atlases(self, output_dir="assets/textures/"):
        """Generate all required atlases using JSON data."""
        print(f"\n🎨 Generating Face-Based Atlases from JSON Data ({self.tile_size_px}x{self.tile_size_px} tiles)")
        
        # Ensure output directory exists
        os.makedirs(output_dir, exist_ok=True)
        
        # Generate each required atlas
        atlases_generated = []
        
        if AtlasType.MAIN in self.atlas_grids:
            grid_w, grid_h = self.atlas_grids[AtlasType.MAIN]
            main_path = os.path.join(output_dir, f"atlas_main.png")
            self.generate_atlas(AtlasType.MAIN, main_path)
            atlases_generated.append(("Main", main_path, f"{grid_w}x{grid_h}"))
        
        if AtlasType.SIDE in self.atlas_grids:
            grid_w, grid_h = self.atlas_grids[AtlasType.SIDE]
            side_path = os.path.join(output_dir, f"atlas_side.png")
            self.generate_atlas(AtlasType.SIDE, side_path)
            atlases_generated.append(("Side", side_path, f"{grid_w}x{grid_h}"))
        
        if AtlasType.BOTTOM in self.atlas_grids:
            grid_w, grid_h = self.atlas_grids[AtlasType.BOTTOM]
            bottom_path = os.path.join(output_dir, f"atlas_bottom.png")
            self.generate_atlas(AtlasType.BOTTOM, bottom_path)
            atlases_generated.append(("Bottom", bottom_path, f"{grid_w}x{grid_h}"))
        
        self.print_generation_summary(atlases_generated)
        return atlases_generated
    
    def generate_atlas(self, atlas_type: AtlasType, output_path: str):
        """Generate a specific atlas type using JSON block data."""
        if atlas_type not in self.atlas_grids:
            print(f"⚠ Skipping {atlas_type.value} atlas (no slots required)")
            return
        
        grid_w, grid_h = self.atlas_grids[atlas_type]
        atlas_size_px = (grid_w * self.tile_size_px, grid_h * self.tile_size_px)
        
        print(f"📋 Generating {atlas_type.value} atlas ({grid_w}x{grid_h} grid, {atlas_size_px[0]}x{atlas_size_px[1]}px)...")
        
        # Create the atlas image
        atlas_image = Image.new('RGBA', atlas_size_px, (0, 0, 0, 0))
        
        # Get blocks that need this atlas type
        blocks_for_atlas = self._get_blocks_for_atlas_type(atlas_type)
        
        legacy_count = 0
        modular_count = 0
        placeholder_count = 0
        
        # Generate textures for each slot
        for i, block_id in enumerate(blocks_for_atlas):
            if i >= grid_w * grid_h:
                break  # Don't exceed atlas capacity
            
            # Calculate position in atlas
            row = i // grid_w
            col = i % grid_w
            x0 = col * self.tile_size_px
            y0 = row * self.tile_size_px
            
            # Generate texture for this block
            if block_id in self.block_mapping:
                block_info = self.block_mapping[block_id]
                texture = self.generate_block_texture(block_info, self.tile_size_px)
                
                if texture:
                    atlas_image.paste(texture, (x0, y0))
                    if hasattr(texture, '_generated_by') and texture._generated_by == 'modular':
                        modular_count += 1
                    else:
                        legacy_count += 1
                else:
                    # Generate placeholder
                    self._generate_placeholder(atlas_image, x0, y0, self.tile_size_px)
                    placeholder_count += 1
            else:
                # Generate placeholder for unknown blocks
                self._generate_placeholder(atlas_image, x0, y0, self.tile_size_px)
                placeholder_count += 1
        
        # Fill remaining slots with placeholders
        total_slots = grid_w * grid_h
        used_slots = len(blocks_for_atlas)
        remaining_slots = total_slots - used_slots
        placeholder_count += remaining_slots
        
        for i in range(used_slots, total_slots):
            row = i // grid_w
            col = i % grid_w
            x0 = col * self.tile_size_px
            y0 = row * self.tile_size_px
            self._generate_placeholder(atlas_image, x0, y0, self.tile_size_px)
        
        # Save the atlas
        atlas_image.save(output_path)
        print(f"✓ {atlas_type.value.title()} atlas saved to {output_path}")
        if self.print_summary:
            print(f"  Legacy textures: {legacy_count}")
            print(f"  Modular textures: {modular_count}")
            print(f"  Placeholder textures: {placeholder_count}")
            print(f"  Total: {total_slots}")
    
    def generate_block_texture(self, block_info, size):
        """Generate texture for a block using the sophisticated modular system."""
        if not MODULAR_SYSTEM_AVAILABLE:
            return None
        
        block_type = block_info.get('type', 'unknown')
        subtype = block_info.get('subtype', 'default')
        
        try:
            # Use the sophisticated routing logic from the old system
            if block_type == 'stone':
                if 'brick' in subtype or 'tile' in subtype or 'polished' in subtype or 'smooth' in subtype:
                    texture = generate_processed_stone_texture(size, subtype)
                else:
                    texture = generate_stone_texture(size, subtype)
            
            elif block_type == 'wood':
                if 'planks' in subtype or 'beam' in subtype:
                    wood_species = subtype.split('_')[0]  # oak, pine, etc.
                    texture = generate_plank_texture(size, wood_species)
                else:
                    texture = generate_wood_texture(size, subtype)
            
            elif block_type == 'organic':
                texture = generate_organic_texture(size, subtype)
            
            elif block_type == 'special':
                texture = generate_special_texture(size, subtype)
            
            elif block_type == 'crystal':
                texture = generate_crystal_texture(size, subtype)
            
            elif block_type == 'ceramic':
                texture = generate_ceramic_texture(size, subtype)
            
            elif block_type == 'metal':
                texture = generate_metal_texture(size, subtype)
            
            elif block_type == 'fluid':
                texture = generate_fluid_texture(size, subtype)
            
            elif block_type == 'ore':
                texture = generate_ore_texture(size, subtype)
            
            else:
                # Fallback to coordinator for unknown types
                texture = self.texture_coordinator.generate_block_texture(f"{block_type}/{subtype}", 'all', size)
            
            if texture:
                texture._generated_by = 'modular'
                return texture
            else:
                return None
                
        except Exception as e:
            if self.print_summary:
                print(f"  ⚠ Error generating {block_type}/{subtype}: {e}")
            return None
    
    def _generate_placeholder(self, atlas_image, x0, y0, tile_size):
        """Generate a purple checkerboard placeholder."""
        checker_size = max(1, tile_size // 4)
        draw = ImageDraw.Draw(atlas_image)
        
        for cy in range(0, tile_size, checker_size):
            for cx in range(0, tile_size, checker_size):
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
    
    def _get_blocks_for_atlas_type(self, atlas_type: AtlasType):
        """Get list of block IDs that need this atlas type."""
        blocks = []
        
        for block_id, face_pattern in self.face_patterns.items():
            if atlas_type == AtlasType.MAIN:
                # All blocks need main atlas
                blocks.append(block_id)
            elif atlas_type == AtlasType.SIDE:
                # Only blocks with different side faces
                if face_pattern in ['ALL_DIFFERENT', 'TOP_BOTTOM_DIFFERENT']:
                    blocks.append(block_id)
            elif atlas_type == AtlasType.BOTTOM:
                # Only blocks with different bottom faces
                if face_pattern == 'ALL_DIFFERENT':
                    blocks.append(block_id)
        
        return sorted(blocks)  # Sort for consistent ordering
    
    def print_generation_summary(self, atlases_generated):
        """Print summary of generated atlases."""
        if not self.print_summary:
            return
        
        print(f"\n📊 Atlas Generation Summary:")
        for name, path, grid in atlases_generated:
            rel_path = os.path.relpath(path)
            print(f"  ✓ {name}: {rel_path} ({grid} grid)")

if __name__ == "__main__":
    print("🎨 JSON-Based Atlas Generator")
    print("=" * 50)
    
    # Generate the atlases from JSON data
    generator = JsonAtlasGenerator(data_dir="data", tile_size_px=32, print_summary=True)
    atlases = generator.generate_all_atlases()
    
    print(f"\n🎯 Atlas generation complete! Check assets/textures/ for results.")
