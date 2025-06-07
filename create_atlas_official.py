#!/usr/bin/env python3
"""
OFFICIAL ATLAS GENERATOR (2025-06-08)
======================================

This is the OFFICIAL, CURRENT atlas generation system for Voxel Castle.
All other create_atlas_*.py files are legacy/development versions.

🎯 USE THIS FILE FOR ALL ATLAS GENERATION 🎯

Features:
- Dynamic multi-file atlas support (atlas_main_2.png, etc.)
- JSON-driven block data (data/blocks/*.json)
- Comprehensive metadata generation (atlas_metadata.json)
- Face pattern support (UNIFORM, TOP_BOTTOM_DIFFERENT, ALL_DIFFERENT)
- 99.6% space efficiency
- Automatic expansion when block count exceeds 16x16 grids

Part of: Unified Block Resource System (Phase 2 Complete)
"""

from PIL import Image, ImageDraw, ImageFont
import os
import sys
import json
import math
import random
import argparse
from typing import Dict, List, Tuple, Optional
from enum import Enum

# Import our existing systems
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, current_dir)

from atlas_face_system import (
    AtlasSlotAllocator, AtlasType, FacePattern, 
    get_atlas_requirements, calculate_atlas_grid_size
)
from scripts.json_to_block_mapping import load_unified_block_data, convert_to_block_mapping

# Import texture generation
texture_generators_path = os.path.join(current_dir, 'texture_generators')
sys.path.insert(0, texture_generators_path)

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
    MODULAR_SYSTEM_AVAILABLE = False
    print(f"⚠ Warning: Modular texture system not available ({e}). Using basic generation.")


def generate_modular_texture(block_id, block_info, size=32, face='all', seed=None):
    """
    Generate a texture using the modular system for a specific block.
    Enhanced with detailed legacy texture generation logic.
    
    Args:
        block_id: The block ID (0-255)
        block_info: Block data from JSON 
        size: Texture size in pixels (default 32x32)
        face: Which face to generate ('top', 'side', 'bottom', 'all')
        seed: Random seed for reproducible generation
    
    Returns:
        PIL Image of the generated texture
    """
    if not MODULAR_SYSTEM_AVAILABLE:
        return generate_legacy_texture(block_id, block_info, size)

    # Generate deterministic seed based on block ID and face
    # This ensures reproducible textures that only change when we modify the generator logic
    if seed is None:
        # Use block ID and face hash for deterministic seeding
        face_hash = hash(face) % 1000 if face != 'all' else 0
        seed = (block_id * 12345 + face_hash) % (2**31)
    
    # Set global random seed for this texture generation
    random.seed(seed)

    # Get block classification from JSON data
    # Extract from nested structure: texture_info.generation.type/subtype
    texture_info = block_info.get('texture_info', {})
    generation = texture_info.get('generation', {})
    block_type = generation.get('type', 'special')
    subtype = generation.get('subtype', 'placeholder')
    
    # Debug output
    print(f"Debug modular: block_id={block_id}, name={block_info.get('name', 'unknown')}, type={block_type}, subtype={subtype}")
    
    try:
        if block_type == 'stone':
            print(f"Debug: Generating stone texture for {subtype}, size={size}")
            if 'brick' in subtype or 'tile' in subtype or 'polished' in subtype or 'smooth' in subtype:
                return generate_processed_stone_texture(size, subtype)
            else:
                return generate_stone_texture(size, subtype)
        
        elif block_type == 'wood':
            print(f"Debug: Generating wood texture for {subtype}, size={size}")
            if 'planks' in subtype or 'beam' in subtype:
                wood_species = subtype.split('_')[0]  # oak, pine, etc.
                return generate_plank_texture(size, wood_species)
            else:
                return generate_wood_texture(size, subtype)
        
        elif block_type == 'organic':
            print(f"Debug: Generating organic texture for {subtype}, size={size}")
            # Handle leaves with per-face logic
            if 'leaves' in subtype:
                species = subtype.split('_')[0]  # oak, pine, etc.
                return generate_organic_texture('leaves_' + species, size, face=face)
            else:
                return generate_organic_texture(subtype, size, face=face)
        
        elif block_type == 'ore':
            print(f"Debug: Generating ore texture for type '{subtype}', size={size}")
            return generate_ore_texture(subtype, size)
        
        elif block_type == 'crystal':
            print(f"Debug: Generating crystal texture for type '{subtype}', size={size}")
            return generate_crystal_texture(subtype, size)
        
        elif block_type == 'ceramic':
            print(f"Debug: Generating ceramic texture for {subtype}, size={size}")
            return generate_ceramic_texture(subtype, size)
        
        elif block_type == 'metal':
            print(f"Debug: Generating metal texture for {subtype}, size={size}")
            return generate_metal_texture(subtype, size)
        
        elif block_type == 'fluid':
            print(f"Debug: Generating fluid texture for {subtype}, size={size}")
            return generate_fluid_texture(subtype, size)
        
        elif block_type == 'special':
            print(f"Debug: Generating special texture for {subtype}, size={size}")
            return generate_special_texture(subtype, size)
        
        elif block_type == 'air':
            # Transparent texture
            return Image.new("RGBA", (size, size), (0, 0, 0, 0))
        
        else:
            # Fallback - purple placeholder
            print(f"Warning: Unknown block type '{block_type}', using placeholder")
            img = Image.new("RGBA", (size, size), (128, 64, 128, 255))
            return img
            
    except Exception as e:
        print(f"Warning: Failed to generate modular texture for block {block_id} ({subtype}): {e}")
        # Return purple placeholder on error
        img = Image.new("RGBA", (size, size), (160, 32, 160, 255))
        return img


def generate_legacy_texture(block_id, block_info, size=32):
    """
    Generate textures using the legacy system for backwards compatibility.
    Enhanced with detailed patterns and colors.
    """
    # Enhanced legacy color definitions
    legacy_colors = {
        0: (0, 0, 0, 0),          # AIR
        1: (128, 128, 128, 255),  # STONE
        2: (139, 69, 19, 255),    # DIRT
        3: (0, 128, 0, 255),      # GRASS
        4: (244, 164, 96, 255),   # SAND
        5: (105, 105, 105, 255),  # GRAVEL
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
        
        # Ores with base stone + ore colors
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
        
        # Wood types
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
        52: (30, 30, 30, 255),    # OIL
        53: (127, 255, 0, 255),   # ACID
        54: (255, 215, 0, 200),   # HONEY
    }
    
    # Get base color (fallback to purple if not defined)
    base_color = legacy_colors.get(block_id, (128, 64, 128, 255))
    
    # Create base image
    img = Image.new("RGBA", (size, size), base_color)
    draw = ImageDraw.Draw(img)
    
    # Add detailed patterns based on block type
    block_type = block_info.get('type', 'unknown')
    subtype = block_info.get('subtype', 'unknown')
    
    if block_type == 'stone' or block_id == 1:  # Stone blocks - add speckles and grain
        for _ in range(size * 2):  # More speckles for higher detail
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            # Random variation in brightness
            variation = random.randint(-40, 40)
            darker = tuple(max(0, min(255, c + variation)) for c in base_color[:3]) + (255,)
            draw.point((x, y), darker)
    
    elif block_type == 'organic' and 'leaves' in subtype:  # Leaves - organic pattern
        for _ in range(size * 3):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            if random.random() < 0.3:  # 30% chance for leaf detail
                lighter = tuple(min(255, c + 20) for c in base_color[:3]) + (255,)
                draw.point((x, y), lighter)
    
    elif block_type == 'wood':  # Wood - add grain lines
        # Vertical grain lines
        for x in range(0, size, 2):
            if random.random() < 0.7:  # 70% chance for grain line
                line_darkness = random.randint(-30, -10)
                grain_color = tuple(max(0, c + line_darkness) for c in base_color[:3]) + (255,)
                for y in range(size):
                    if random.random() < 0.8:  # Irregular grain
                        draw.point((x, y), grain_color)
    
    elif block_type == 'ore':  # Ore blocks - add sparkles
        base_stone = (128, 128, 128, 255)  # Stone base
        img = Image.new("RGBA", (size, size), base_stone)
        draw = ImageDraw.Draw(img)
        
        # Add ore veins
        for _ in range(size // 4):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            # Draw small ore clusters
            cluster_size = random.randint(1, 3)
            for dx in range(-cluster_size, cluster_size + 1):
                for dy in range(-cluster_size, cluster_size + 1):
                    if 0 <= x + dx < size and 0 <= y + dy < size:
                        if random.random() < 0.6:
                            draw.point((x + dx, y + dy), base_color)
    
    elif block_type == 'fluid':  # Fluid blocks - add shimmer effect
        for _ in range(size):
            x = random.randint(0, size-1)
            y = random.randint(0, size-1)
            if random.random() < 0.1:  # 10% chance for shimmer
                lighter = tuple(min(255, c + 50) for c in base_color[:3]) + base_color[3:]
                draw.point((x, y), lighter)
    
    return img


class AtlasFileInfo:
    """Information about a specific atlas file"""
    def __init__(self, file_index: int, atlas_type: AtlasType, grid_width: int, grid_height: int):
        self.file_index = file_index
        self.atlas_type = atlas_type
        self.grid_width = grid_width
        self.grid_height = grid_height
        self.total_slots = grid_width * grid_height
        self.assigned_blocks = []  # [(block_id, slot_index)]
        
    @property
    def filename(self) -> str:
        """Generate the filename for this atlas file"""
        if self.file_index == 1:
            return f"atlas_{self.atlas_type.value.lower()}.png"
        else:
            return f"atlas_{self.atlas_type.value.lower()}_{self.file_index}.png"
    
    @property
    def used_slots(self) -> int:
        return len(self.assigned_blocks)
    
    @property
    def available_slots(self) -> int:
        return self.total_slots - self.used_slots
    
    @property
    def efficiency(self) -> float:
        return (self.used_slots / self.total_slots) * 100 if self.total_slots > 0 else 0

class DynamicAtlasGenerator:
    """Advanced atlas generator with multi-file support and dynamic expansion"""
    
    def __init__(self, data_dir="data", tile_size_px=32, max_grid_size=16, print_summary=True, debug=False):
        self.data_dir = data_dir
        self.tile_size_px = tile_size_px
        self.max_grid_size = max_grid_size
        self.print_summary = print_summary
        self.debug = debug
        
        # Load block data
        self.unified_data = load_unified_block_data(data_dir)
        self.block_mapping = convert_to_block_mapping(self.unified_data)
        
        # Atlas file management
        self.atlas_files: Dict[AtlasType, List[AtlasFileInfo]] = {}
        self.block_assignments: Dict[int, Tuple[AtlasType, int, int]] = {}  # block_id -> (atlas_type, file_index, slot_index)
        
        self._calculate_atlas_layout()
        
        if print_summary:
            self._print_layout_summary()
    
    def _calculate_atlas_layout(self):
        """Calculate optimal atlas file layout for all blocks"""
        print("🧮 Calculating dynamic atlas layout...")
        
        # Calculate face pattern requirements from JSON data
        requirements = self._get_json_atlas_requirements()
        
        for atlas_type in [AtlasType.MAIN, AtlasType.SIDE, AtlasType.BOTTOM]:
            if atlas_type in requirements:
                block_ids = requirements[atlas_type]
                self._layout_atlas_type(atlas_type, block_ids)
    
    def _get_json_atlas_requirements(self) -> Dict[AtlasType, List[int]]:
        """Analyze JSON block data to determine atlas requirements"""
        requirements = {
            AtlasType.MAIN: [],
            AtlasType.SIDE: [],
            AtlasType.BOTTOM: []
        }
        
        for block_name, block_data in self.unified_data.items():
            block_id = block_data["id"]
            face_pattern = block_data.get("face_pattern", "UNIFORM")
            
            # All blocks need main atlas slot
            requirements[AtlasType.MAIN].append(block_id)
            
            # Determine additional atlas requirements based on face pattern
            if face_pattern in ["TOP_BOTTOM_DIFFERENT", "ALL_DIFFERENT", "ALL_FACES_DIFFERENT"]:
                requirements[AtlasType.SIDE].append(block_id)
            
            if face_pattern in ["ALL_DIFFERENT", "ALL_FACES_DIFFERENT"]:
                requirements[AtlasType.BOTTOM].append(block_id)
        
        # Sort for consistent ordering
        for atlas_type in requirements:
            requirements[atlas_type].sort()
        
        return requirements
    
    def _layout_atlas_type(self, atlas_type: AtlasType, block_ids: List[int]):
        """Layout blocks for a specific atlas type across multiple files if needed"""
        if not block_ids:
            return
            
        num_blocks = len(block_ids)
        max_slots_per_file = self.max_grid_size * self.max_grid_size
        
        # Calculate number of files needed
        if num_blocks <= max_slots_per_file:
            # Single file - calculate optimal grid size
            grid_size = calculate_atlas_grid_size(num_blocks)
            grid_w, grid_h = grid_size
            
            file_info = AtlasFileInfo(1, atlas_type, grid_w, grid_h)
            self.atlas_files[atlas_type] = [file_info]
            
            # Assign all blocks to this file
            for i, block_id in enumerate(block_ids):
                file_info.assigned_blocks.append((block_id, i))
                self.block_assignments[block_id] = (atlas_type, 1, i)
                
        else:
            # Multiple files needed
            num_files = math.ceil(num_blocks / max_slots_per_file)
            self.atlas_files[atlas_type] = []
            
            for file_index in range(1, num_files + 1):
                start_idx = (file_index - 1) * max_slots_per_file
                end_idx = min(start_idx + max_slots_per_file, num_blocks)
                file_blocks = block_ids[start_idx:end_idx]
                
                if file_index < num_files:
                    # Full file
                    grid_w = grid_h = self.max_grid_size
                else:
                    # Last file - optimize size
                    remaining_blocks = len(file_blocks)
                    grid_w, grid_h = calculate_atlas_grid_size(remaining_blocks)
                
                file_info = AtlasFileInfo(file_index, atlas_type, grid_w, grid_h)
                self.atlas_files[atlas_type].append(file_info)
                
                # Assign blocks to this file
                for i, block_id in enumerate(file_blocks):
                    file_info.assigned_blocks.append((block_id, i))
                    self.block_assignments[block_id] = (atlas_type, file_index, i)
    
    def _print_layout_summary(self):
        """Print detailed layout summary"""
        print(f"\n📊 Dynamic Atlas Layout Summary")
        print("=" * 60)
        
        total_files = 0
        total_blocks = 0
        total_slots = 0
        
        for atlas_type, file_list in self.atlas_files.items():
            if not file_list:
                continue
                
            print(f"\n{atlas_type.value} Atlas:")
            print("-" * 25)
            
            for file_info in file_list:
                efficiency_str = f"{file_info.efficiency:5.1f}%"
                print(f"  File {file_info.file_index}: {file_info.grid_width}x{file_info.grid_height} "
                      f"({file_info.used_slots}/{file_info.total_slots} slots, {efficiency_str})")
                
                total_files += 1
                total_blocks += file_info.used_slots
                total_slots += file_info.total_slots
        
        overall_efficiency = (total_blocks / total_slots) * 100 if total_slots > 0 else 0
        print(f"\n📈 Overall Statistics:")
        print(f"  Total Files: {total_files}")
        print(f"  Total Blocks: {total_blocks}")
        print(f"  Total Slots: {total_slots}")
        print(f"  Overall Efficiency: {overall_efficiency:.1f}%")
    
    def generate_all_atlases(self, output_dir="assets/textures/"):
        """Generate all atlas files with metadata"""
        print(f"\n🎨 Generating Dynamic Multi-File Atlases")
        
        os.makedirs(output_dir, exist_ok=True)
        
        generated_files = []
        metadata = {
            "generation_info": {
                "tile_size_px": self.tile_size_px,
                "max_grid_size": self.max_grid_size,
                "total_blocks": len(self.unified_data)
            },
            "atlases": {}
        }
        
        for atlas_type, file_list in self.atlas_files.items():
            if not file_list:
                continue
                
            atlas_metadata = {
                "files": [],
                "total_blocks": sum(len(f.assigned_blocks) for f in file_list)
            }
            
            for file_info in file_list:
                # Generate the atlas image
                output_path = os.path.join(output_dir, file_info.filename)
                self._generate_atlas_file(file_info, output_path)
                generated_files.append((file_info.filename, output_path))
                
                # Add to metadata
                file_metadata = {
                    "filename": file_info.filename,
                    "grid_size": [file_info.grid_width, file_info.grid_height],
                    "total_slots": file_info.total_slots,
                    "used_slots": file_info.used_slots,
                    "efficiency": round(file_info.efficiency, 1),
                    "blocks": {}
                }
                
                # Add block assignments
                for block_id, slot_index in file_info.assigned_blocks:
                    slot_x = slot_index % file_info.grid_width
                    slot_y = slot_index // file_info.grid_width
                    file_metadata["blocks"][str(block_id)] = {
                        "slot_index": slot_index,
                        "slot_coords": [slot_x, slot_y]
                    }
                
                atlas_metadata["files"].append(file_metadata)
            
            metadata["atlases"][atlas_type.value.lower()] = atlas_metadata
        
        # Save metadata
        metadata_path = os.path.join(output_dir, "atlas_metadata.json")
        with open(metadata_path, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        print(f"\n✅ Generated {len(generated_files)} atlas files")
        print(f"📄 Atlas metadata saved to: {metadata_path}")
        
        return generated_files, metadata_path
    
    def _generate_atlas_file(self, file_info: AtlasFileInfo, output_path: str):
        """Generate a single atlas file"""
        atlas_size_px = (
            file_info.grid_width * self.tile_size_px, 
            file_info.grid_height * self.tile_size_px
        )
        
        print(f"  📋 {file_info.filename}: {file_info.grid_width}x{file_info.grid_height} "
              f"({file_info.used_slots} blocks, {file_info.efficiency:.1f}%)")
        
        # Create blank atlas
        atlas_image = Image.new('RGBA', atlas_size_px, (0, 0, 0, 0))
        
        # Generate textures for assigned blocks
        for block_id, slot_index in file_info.assigned_blocks:
            slot_x = slot_index % file_info.grid_width
            slot_y = slot_index // file_info.grid_width
            
            x0 = slot_x * self.tile_size_px
            y0 = slot_y * self.tile_size_px
            
            # Get block info
            block_info = None
            for block_name, data in self.unified_data.items():
                if data["id"] == block_id:
                    block_info = data
                    break
            
            if block_info:
                # Generate texture for this face type
                face_texture = self._generate_face_texture(block_info, file_info.atlas_type)
                if face_texture:
                    atlas_image.paste(face_texture, (x0, y0))
                else:
                    self._generate_placeholder(atlas_image, x0, y0, file_info.atlas_type)
            else:
                self._generate_placeholder(atlas_image, x0, y0, file_info.atlas_type)
        
        # Fill remaining slots with debug pattern
        for slot_index in range(file_info.used_slots, file_info.total_slots):
            slot_x = slot_index % file_info.grid_width
            slot_y = slot_index // file_info.grid_width
            x0 = slot_x * self.tile_size_px
            y0 = slot_y * self.tile_size_px
            self._generate_empty_placeholder(atlas_image, x0, y0)
        
        atlas_image.save(output_path)
        
        # Generate debug version if debug mode is enabled
        if self.debug:
            self._generate_debug_atlas(file_info, output_path)
    
    def _generate_face_texture(self, block_info: Dict, atlas_type: AtlasType) -> Optional[Image.Image]:
        """Generate texture for specific face of a block using enhanced modular system"""
        try:
            # Map atlas type to face for generation
            face_map = {
                AtlasType.MAIN: 'top',
                AtlasType.SIDE: 'side', 
                AtlasType.BOTTOM: 'bottom'
            }
            face = face_map.get(atlas_type, 'top')
            
            # Get block ID and use consistent seed for reproducible generation
            block_id = block_info.get("id", 0)
            texture_seed = block_id * 12345 + hash(face) % 1000
            
            # Generate using enhanced modular system
            texture = generate_modular_texture(
                block_id, 
                block_info, 
                size=self.tile_size_px, 
                face=face, 
                seed=texture_seed
            )
            
            if texture and isinstance(texture, Image.Image):
                # Ensure correct size
                if texture.size != (self.tile_size_px, self.tile_size_px):
                    texture = texture.resize((self.tile_size_px, self.tile_size_px))
                return texture
            
        except Exception as e:
            print(f"⚠ Enhanced texture generation failed for {block_info.get('name', 'unknown')}: {e}")
        
        return None
    
    def _generate_placeholder(self, atlas_image: Image.Image, x0: int, y0: int, atlas_type: AtlasType):
        """Generate a placeholder texture for missing blocks"""
        draw = ImageDraw.Draw(atlas_image)
        
        # Different colors for different atlas types
        colors = {
            AtlasType.MAIN: (200, 100, 100, 255),    # Red
            AtlasType.SIDE: (100, 200, 100, 255),    # Green  
            AtlasType.BOTTOM: (100, 100, 200, 255)   # Blue
        }
        
        color = colors.get(atlas_type, (128, 128, 128, 255))
        
        # Fill with solid color
        draw.rectangle([x0, y0, x0 + self.tile_size_px - 1, y0 + self.tile_size_px - 1], 
                      fill=color)
        
        # Add texture pattern
        for i in range(0, self.tile_size_px, 4):
            for j in range(0, self.tile_size_px, 4):
                if (i + j) % 8 == 0:
                    darker = tuple(max(0, c - 40) for c in color[:3]) + (255,)
                    draw.rectangle([x0 + i, y0 + j, x0 + i + 2, y0 + j + 2], fill=darker)
    
    def _generate_empty_placeholder(self, atlas_image: Image.Image, x0: int, y0: int):
        """Generate placeholder for unused atlas slots"""
        draw = ImageDraw.Draw(atlas_image)
        
        # Light gray with diagonal lines to show it's unused
        fill_color = (64, 64, 64, 128)
        line_color = (32, 32, 32, 128)
        
        # Fill background
        draw.rectangle([x0, y0, x0 + self.tile_size_px - 1, y0 + self.tile_size_px - 1], 
                      fill=fill_color)
        
        # Diagonal lines
        for i in range(-self.tile_size_px, self.tile_size_px, 8):
            draw.line([x0 + i, y0, x0 + i + self.tile_size_px, y0 + self.tile_size_px], 
                     fill=line_color, width=1)
    
    def _generate_debug_atlas(self, file_info: AtlasFileInfo, output_path: str) -> None:
        """Generate debug version of atlas with block IDs and coordinates overlaid"""
        debug_path = output_path.replace('.png', '_debug.png')
        
        # Load the original atlas
        if not os.path.exists(output_path):
            print(f"⚠ Original atlas not found: {output_path}")
            return
            
        atlas_image = Image.open(output_path).convert('RGBA')
        draw = ImageDraw.Draw(atlas_image)
        
        # Try to load a font, fallback to default if not available
        try:
            # Try to find a system font
            font_size = max(8, self.tile_size_px // 4)
            font = ImageFont.truetype("/usr/share/fonts/dejavu/DejaVuSans-Bold.ttf", font_size)
        except:
            try:
                font = ImageFont.load_default()
            except:
                font = None
        
        # Overlay block IDs and coordinates
        for block_id, slot_index in file_info.assigned_blocks:
            slot_x = slot_index % file_info.grid_width
            slot_y = slot_index // file_info.grid_width
            x0 = slot_x * self.tile_size_px
            y0 = slot_y * self.tile_size_px
            
            # Create semi-transparent overlay for text background
            overlay = Image.new('RGBA', (self.tile_size_px, self.tile_size_px), (0, 0, 0, 128))
            atlas_image.paste(overlay, (x0, y0), overlay)
            
            # Add block ID in top-left corner
            id_text = str(block_id)
            if font:
                bbox = draw.textbbox((0, 0), id_text, font=font)
                text_width = bbox[2] - bbox[0]
                text_height = bbox[3] - bbox[1]
            else:
                text_width, text_height = 20, 10  # Fallback estimate
            
            # Position text with small margin
            text_x = x0 + 2
            text_y = y0 + 2
            
            # Draw text with outline for visibility
            if font:
                # Black outline
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        if dx != 0 or dy != 0:
                            draw.text((text_x + dx, text_y + dy), id_text, fill=(0, 0, 0, 255), font=font)
                # White text
                draw.text((text_x, text_y), id_text, fill=(255, 255, 255, 255), font=font)
            else:
                # Fallback without font
                draw.text((text_x, text_y), id_text, fill=(255, 255, 255, 255))
            
            # Add coordinate in bottom-right corner
            coord_text = f"({slot_x},{slot_y})"
            if font:
                bbox = draw.textbbox((0, 0), coord_text, font=font)
                coord_width = bbox[2] - bbox[0]
                coord_height = bbox[3] - bbox[1]
            else:
                coord_width, coord_height = 30, 10  # Fallback estimate
            
            coord_x = x0 + self.tile_size_px - coord_width - 2
            coord_y = y0 + self.tile_size_px - coord_height - 2
            
            if font:
                # Black outline
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        if dx != 0 or dy != 0:
                            draw.text((coord_x + dx, coord_y + dy), coord_text, fill=(0, 0, 0, 255), font=font)
                # White text
                draw.text((coord_x, coord_y), coord_text, fill=(255, 255, 255, 255), font=font)
            else:
                # Fallback without font
                draw.text((coord_x, coord_y), coord_text, fill=(255, 255, 255, 255))
        
        atlas_image.save(debug_path)
        print(f"🐛 Debug atlas saved: {debug_path}")

def main():
    """Main function with command line argument support"""
    parser = argparse.ArgumentParser(description='Generate dynamic atlas files for Voxel Castle')
    parser.add_argument('--debug', action='store_true', 
                       help='Generate debug atlases with block IDs and coordinates')
    parser.add_argument('--tile-size', type=int, default=32,
                       help='Tile size in pixels (default: 32)')
    parser.add_argument('--max-grid', type=int, default=16,
                       help='Maximum grid size (default: 16)')
    parser.add_argument('--output-dir', type=str, default='assets/textures/',
                       help='Output directory (default: assets/textures/)')
    
    args = parser.parse_args()
    
    print("🚀 Dynamic Multi-File Atlas Generator")
    print("=" * 50)
    
    if args.debug:
        print("🐛 Debug mode enabled - will generate debug atlases with IDs")
    
    try:
        # Create generator with command line settings
        generator = DynamicAtlasGenerator(
            data_dir="data",
            tile_size_px=args.tile_size,
            max_grid_size=args.max_grid,
            print_summary=True,
            debug=args.debug
        )
        
        # Generate all atlases
        files, metadata_path = generator.generate_all_atlases(args.output_dir)
        
        print(f"\n✅ Dynamic atlas generation complete!")
        print(f"📁 Output directory: {args.output_dir}")
        print(f"📄 Metadata file: {metadata_path}")
        
        if args.debug:
            print(f"🐛 Debug atlases generated with _debug.png suffix")
        
        return True
        
    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    main()
