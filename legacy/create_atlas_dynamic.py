#!/usr/bin/env python3
"""
Dynamic Multi-File Atlas Generator
Enhanced atlas system that supports automatic expansion to multiple files.
Phase 2 of the Unified Block Resource System.
"""

from PIL import Image, ImageDraw
import os
import sys
import json
import math
from typing import Dict, List, Tuple, Optional
from enum import Enum

# Import our existing systems
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)  # Go up one level from legacy/
sys.path.insert(0, parent_dir)

from atlas_face_system import (
    AtlasSlotAllocator, AtlasType, FacePattern, 
    get_atlas_requirements, calculate_atlas_grid_size
)
from scripts.json_to_block_mapping import load_unified_block_data, convert_to_block_mapping

# Import texture generation
texture_generators_path = os.path.join(parent_dir, 'texture_generators')
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
except ImportError:
    MODULAR_SYSTEM_AVAILABLE = False
    print("⚠ Warning: Modular texture system not available. Using basic generation.")

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
    
    def __init__(self, data_dir="data", tile_size_px=32, max_grid_size=16, print_summary=True):
        self.data_dir = data_dir
        self.tile_size_px = tile_size_px
        self.max_grid_size = max_grid_size
        self.print_summary = print_summary
        
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
    
    def _generate_face_texture(self, block_info: Dict, atlas_type: AtlasType) -> Optional[Image.Image]:
        """Generate texture for specific face of a block"""
        if not MODULAR_SYSTEM_AVAILABLE:
            return None
        
        try:
            # Use the modular texture system logic (similar to original atlas generator)
            block_type = block_info.get('type', 'unknown')
            subtype = block_info.get('subtype', 'unknown')
            
            # Map atlas type to face
            face_map = {
                AtlasType.MAIN: 'top',
                AtlasType.SIDE: 'side', 
                AtlasType.BOTTOM: 'bottom'
            }
            face = face_map.get(atlas_type, 'top')
            
            # Generate using modular system
            coordinator = SimpleTextureCoordinator()
            texture = coordinator.generate_texture_for_face(
                block_type, subtype, face, self.tile_size_px
            )
            
            if texture and isinstance(texture, Image.Image):
                return texture.resize((self.tile_size_px, self.tile_size_px))
            
        except Exception as e:
            print(f"⚠ Texture generation failed for {block_info.get('name', 'unknown')}: {e}")
        
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

def main():
    """Generate dynamic multi-file atlases"""
    print("🚀 Dynamic Multi-File Atlas Generator")
    print("=" * 50)
    
    try:
        # Create generator with default settings
        generator = DynamicAtlasGenerator(
            data_dir="data",
            tile_size_px=32,
            max_grid_size=16,
            print_summary=True
        )
        
        # Generate all atlases
        files, metadata_path = generator.generate_all_atlases("assets/textures/")
        
        print(f"\n✅ Dynamic atlas generation complete!")
        print(f"📁 Output directory: assets/textures/")
        print(f"📄 Metadata file: {metadata_path}")
        
        return True
        
    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    main()
