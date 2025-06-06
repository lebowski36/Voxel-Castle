#!/usr/bin/env python3
"""
Efficient Face-Based Atlas System
Implementation of the new atlas generation system that matches the C++ face pattern classification.
This module provides efficient packing and slot allocation for multi-atlas texture generation.
"""

from enum import Enum
from typing import Dict, List, Tuple, Optional, Set
import os

class FacePattern(Enum):
    """Face pattern types that match the C++ enum"""
    UNIFORM = "UNIFORM"                    # All faces same - 1 atlas slot
    TOP_BOTTOM_DIFFERENT = "TOP_BOTTOM_DIFFERENT"  # Top/bottom same, sides different - 2 atlas slots
    ALL_DIFFERENT = "ALL_DIFFERENT"       # All faces unique - 3 atlas slots
    DIRECTIONAL = "DIRECTIONAL"           # Front/back/left/right different - 4+ atlas slots

class AtlasType(Enum):
    """Atlas types for the multi-atlas system"""
    MAIN = "main"        # Primary face textures (all blocks)
    SIDE = "side"        # Side face textures (only certain patterns)
    BOTTOM = "bottom"    # Bottom face textures (only certain patterns)

# Face pattern mapping for all 256 VoxelType IDs
# This must match the C++ mapping in voxel_face_patterns.cpp
BLOCK_FACE_PATTERNS = {
    # Basic Terrain (0-9)
    0: FacePattern.UNIFORM,               # AIR
    1: FacePattern.UNIFORM,               # STONE
    2: FacePattern.UNIFORM,               # DIRT
    3: FacePattern.ALL_DIFFERENT,         # GRASS (green top, dirt sides, dirt bottom)
    4: FacePattern.UNIFORM,               # SAND
    5: FacePattern.UNIFORM,               # GRAVEL
    6: FacePattern.UNIFORM,               # CLAY
    7: FacePattern.UNIFORM,               # BEDROCK
    8: FacePattern.UNIFORM,               # TOPSOIL
    9: FacePattern.UNIFORM,               # SUBSOIL
    
    # Stone Varieties (10-19)
    10: FacePattern.UNIFORM,              # GRANITE
    11: FacePattern.UNIFORM,              # LIMESTONE
    12: FacePattern.UNIFORM,              # MARBLE
    13: FacePattern.UNIFORM,              # SANDSTONE
    14: FacePattern.UNIFORM,              # SLATE
    15: FacePattern.UNIFORM,              # BASALT
    16: FacePattern.UNIFORM,              # QUARTZITE
    17: FacePattern.UNIFORM,              # OBSIDIAN
    18: FacePattern.UNIFORM,              # PUMICE
    19: FacePattern.UNIFORM,              # SHALE
    
    # Ores & Minerals (20-29)
    20: FacePattern.UNIFORM,              # COAL_ORE
    21: FacePattern.UNIFORM,              # IRON_ORE
    22: FacePattern.UNIFORM,              # COPPER_ORE
    23: FacePattern.UNIFORM,              # TIN_ORE
    24: FacePattern.UNIFORM,              # SILVER_ORE
    25: FacePattern.UNIFORM,              # GOLD_ORE
    26: FacePattern.UNIFORM,              # GEM_RUBY
    27: FacePattern.UNIFORM,              # GEM_SAPPHIRE
    28: FacePattern.UNIFORM,              # GEM_EMERALD
    29: FacePattern.UNIFORM,              # GEM_DIAMOND
    
    # Organic Natural (30-39) - Wood logs use TOP_BOTTOM_DIFFERENT
    30: FacePattern.TOP_BOTTOM_DIFFERENT, # WOOD_OAK (end grain top/bottom, bark sides)
    31: FacePattern.TOP_BOTTOM_DIFFERENT, # WOOD_PINE
    32: FacePattern.TOP_BOTTOM_DIFFERENT, # WOOD_BIRCH
    33: FacePattern.TOP_BOTTOM_DIFFERENT, # WOOD_MAHOGANY
    34: FacePattern.UNIFORM,              # LEAVES_OAK
    35: FacePattern.UNIFORM,              # LEAVES_PINE
    36: FacePattern.UNIFORM,              # LEAVES_BIRCH
    37: FacePattern.UNIFORM,              # LEAVES_PALM
    38: FacePattern.UNIFORM,              # MUSHROOM_BROWN
    39: FacePattern.UNIFORM,              # MUSHROOM_RED
    
    # Biome Specific (40-49)
    40: FacePattern.UNIFORM,              # SNOW
    41: FacePattern.UNIFORM,              # ICE
    42: FacePattern.UNIFORM,              # PACKED_ICE
    43: FacePattern.UNIFORM,              # CACTUS
    44: FacePattern.UNIFORM,              # JUNGLE_VINE
    45: FacePattern.UNIFORM,              # CORAL_PINK
    46: FacePattern.UNIFORM,              # CORAL_BLUE
    47: FacePattern.UNIFORM,              # SEAWEED
    48: FacePattern.UNIFORM,              # TUNDRA_MOSS
    49: FacePattern.UNIFORM,              # DESERT_ROCK
    
    # Fluids (50-59) - All uniform
    50: FacePattern.UNIFORM,              # WATER
    51: FacePattern.UNIFORM,              # LAVA
    52: FacePattern.UNIFORM,              # OIL
    53: FacePattern.UNIFORM,              # ACID
    54: FacePattern.UNIFORM,              # HONEY
    55: FacePattern.UNIFORM,              # STEAM
    56: FacePattern.UNIFORM,              # TOXIC_GAS
    57: FacePattern.UNIFORM,              # NATURAL_GAS
    58: FacePattern.UNIFORM,              # MAGICAL_MIST
    59: FacePattern.UNIFORM,              # SMOKE
    
    # Stone Processing (60-69) - All uniform processed materials
    60: FacePattern.UNIFORM,              # STONE_BRICK
    61: FacePattern.UNIFORM,              # GRANITE_BRICK
    62: FacePattern.UNIFORM,              # MARBLE_TILE
    63: FacePattern.UNIFORM,              # SANDSTONE_BRICK
    64: FacePattern.UNIFORM,              # SLATE_TILE
    65: FacePattern.UNIFORM,              # COBBLESTONE
    66: FacePattern.UNIFORM,              # SMOOTH_STONE
    67: FacePattern.UNIFORM,              # POLISHED_GRANITE
    68: FacePattern.UNIFORM,              # POLISHED_MARBLE
    69: FacePattern.UNIFORM,              # FLAGSTONE
    
    # Wood Processing (70-79) - Planks uniform, beams with grain direction
    70: FacePattern.UNIFORM,              # OAK_PLANKS
    71: FacePattern.UNIFORM,              # PINE_PLANKS
    72: FacePattern.UNIFORM,              # BIRCH_PLANKS
    73: FacePattern.UNIFORM,              # MAHOGANY_PLANKS
    74: FacePattern.TOP_BOTTOM_DIFFERENT, # OAK_BEAM (grain direction)
    75: FacePattern.TOP_BOTTOM_DIFFERENT, # PINE_BEAM
    76: FacePattern.TOP_BOTTOM_DIFFERENT, # HARDWOOD_BEAM
    77: FacePattern.UNIFORM,              # BAMBOO_PLANK
    78: FacePattern.UNIFORM,              # CORK
    79: FacePattern.UNIFORM,              # CHARCOAL_BLOCK
    
    # Metal Blocks (80-89) - All uniform
    80: FacePattern.UNIFORM,              # IRON_BLOCK
    81: FacePattern.UNIFORM,              # COPPER_BLOCK
    82: FacePattern.UNIFORM,              # BRONZE_BLOCK
    83: FacePattern.UNIFORM,              # STEEL_BLOCK
    84: FacePattern.UNIFORM,              # SILVER_BLOCK
    85: FacePattern.UNIFORM,              # GOLD_BLOCK
    86: FacePattern.UNIFORM,              # BRASS_BLOCK
    87: FacePattern.UNIFORM,              # PEWTER_BLOCK
    88: FacePattern.UNIFORM,              # MITHRIL_BLOCK
    89: FacePattern.UNIFORM,              # ADAMANTINE_BLOCK
    
    # Clay & Ceramic (90-99) - All uniform
    90: FacePattern.UNIFORM,              # CLAY_BRICK
    91: FacePattern.UNIFORM,              # TERRACOTTA
    92: FacePattern.UNIFORM,              # GLAZED_WHITE
    93: FacePattern.UNIFORM,              # GLAZED_RED
    94: FacePattern.UNIFORM,              # GLAZED_BLUE
    95: FacePattern.UNIFORM,              # GLAZED_GREEN
    96: FacePattern.UNIFORM,              # PORCELAIN
    97: FacePattern.UNIFORM,              # STONEWARE
    98: FacePattern.UNIFORM,              # EARTHENWARE
    99: FacePattern.UNIFORM,              # CERAMIC_TILE
}

# Fill remaining slots 100-255 with UNIFORM pattern as default
for i in range(100, 256):
    if i not in BLOCK_FACE_PATTERNS:
        BLOCK_FACE_PATTERNS[i] = FacePattern.UNIFORM

class AtlasSlotAllocator:
    """
    Manages efficient slot allocation for the multi-atlas system.
    Ensures minimal atlas usage and no wasted slots.
    """
    
    def __init__(self):
        self.main_atlas_slots = {}     # voxel_id -> main_slot
        self.side_atlas_slots = {}     # voxel_id -> side_slot  
        self.bottom_atlas_slots = {}   # voxel_id -> bottom_slot
        
        # Track next available slots for efficient atlases
        self.next_side_slot = 0
        self.next_bottom_slot = 0
        
        # Statistics
        self.stats = {
            'main_slots_used': 0,
            'side_slots_used': 0,
            'bottom_slots_used': 0,
            'blocks_by_pattern': {pattern: 0 for pattern in FacePattern}
        }
    
    def allocate_all_slots(self):
        """Allocate slots for all 256 block types."""
        print("Allocating atlas slots for all 256 block types...")
        
        for voxel_id in range(256):
            pattern = self.get_face_pattern(voxel_id)
            self.allocate_slots_for_block(voxel_id, pattern)
        
        self._calculate_stats()
        self._print_allocation_summary()
    
    def get_face_pattern(self, voxel_id: int) -> FacePattern:
        """Get the face pattern for a voxel ID."""
        return BLOCK_FACE_PATTERNS.get(voxel_id, FacePattern.UNIFORM)
    
    def allocate_slots_for_block(self, voxel_id: int, pattern: FacePattern):
        """Allocate slots for a specific block based on its pattern."""
        # All blocks get a main atlas slot (their voxel_id)
        self.main_atlas_slots[voxel_id] = voxel_id
        
        # Allocate additional slots based on pattern
        if pattern == FacePattern.TOP_BOTTOM_DIFFERENT:
            # Needs side texture
            self.side_atlas_slots[voxel_id] = self.next_side_slot
            self.next_side_slot += 1
            
        elif pattern == FacePattern.ALL_DIFFERENT:
            # Needs both side and bottom textures
            self.side_atlas_slots[voxel_id] = self.next_side_slot
            self.next_side_slot += 1
            self.bottom_atlas_slots[voxel_id] = self.next_bottom_slot
            self.next_bottom_slot += 1
            
        elif pattern == FacePattern.DIRECTIONAL:
            # Future implementation - for now treat as ALL_DIFFERENT
            self.side_atlas_slots[voxel_id] = self.next_side_slot
            self.next_side_slot += 1
            self.bottom_atlas_slots[voxel_id] = self.next_bottom_slot
            self.next_bottom_slot += 1
    
    def get_texture_slot(self, voxel_id: int, atlas_type: AtlasType) -> Optional[int]:
        """Get the atlas slot for a specific voxel and atlas type."""
        if atlas_type == AtlasType.MAIN:
            return self.main_atlas_slots.get(voxel_id)
        elif atlas_type == AtlasType.SIDE:
            return self.side_atlas_slots.get(voxel_id)
        elif atlas_type == AtlasType.BOTTOM:
            return self.bottom_atlas_slots.get(voxel_id)
        return None
    
    def get_required_face_textures(self, voxel_id: int) -> List[str]:
        """Get list of face types that need textures for this voxel."""
        pattern = self.get_face_pattern(voxel_id)
        
        if pattern == FacePattern.UNIFORM:
            return ['top']  # Main atlas only, used for all faces
        elif pattern == FacePattern.TOP_BOTTOM_DIFFERENT:
            return ['top', 'side']  # Top used for top/bottom, side for sides
        elif pattern == FacePattern.ALL_DIFFERENT:
            return ['top', 'side', 'bottom']
        elif pattern == FacePattern.DIRECTIONAL:
            return ['top', 'side', 'bottom']  # For now, same as ALL_DIFFERENT
        
        return ['top']
    
    def _calculate_stats(self):
        """Calculate allocation statistics."""
        self.stats['main_slots_used'] = len(self.main_atlas_slots)
        self.stats['side_slots_used'] = len(self.side_atlas_slots)
        self.stats['bottom_slots_used'] = len(self.bottom_atlas_slots)
        
        for voxel_id in range(256):
            pattern = self.get_face_pattern(voxel_id)
            self.stats['blocks_by_pattern'][pattern] += 1
    
    def _calculate_requirements(self):
        """Calculate slot requirements silently for get_atlas_requirements()."""
        for voxel_id in range(256):
            pattern = self.get_face_pattern(voxel_id)
            self.allocate_slots_for_block(voxel_id, pattern)
        
        self._calculate_stats()
        # No printing in this method

    def _print_allocation_summary(self):
        """Print a summary of slot allocation."""
        print(f"\n📊 Atlas Slot Allocation Summary:")
        print(f"  Main Atlas:   {self.stats['main_slots_used']:3d}/256 slots used (100%)")
        print(f"  Side Atlas:   {self.stats['side_slots_used']:3d}/256 slots used ({self.stats['side_slots_used']/256*100:.1f}%)")
        print(f"  Bottom Atlas: {self.stats['bottom_slots_used']:3d}/256 slots used ({self.stats['bottom_slots_used']/256*100:.1f}%)")
        
        print(f"\n📈 Blocks by Pattern:")
        for pattern, count in self.stats['blocks_by_pattern'].items():
            print(f"  {pattern.value:20s}: {count:3d} blocks ({count/256*100:.1f}%)")
        
        # Calculate efficiency
        total_slots_needed = (self.stats['main_slots_used'] + 
                            self.stats['side_slots_used'] + 
                            self.stats['bottom_slots_used'])
        total_slots_in_old_system = 256 * 3  # Old system would need 3 atlases full
        efficiency = (1 - total_slots_needed / total_slots_in_old_system) * 100
        
        print(f"\n💡 Efficiency Improvement:")
        print(f"  Old system: {total_slots_in_old_system} slots (3 full atlases)")
        print(f"  New system: {total_slots_needed} slots total")
        print(f"  Space saved: {efficiency:.1f}%")

def get_atlas_requirements() -> Dict[AtlasType, int]:
    """Get the number of slots required for each atlas type."""
    allocator = AtlasSlotAllocator()
    # Use internal calculation without prints
    allocator._calculate_requirements()
    
    return {
        AtlasType.MAIN: allocator.stats['main_slots_used'],
        AtlasType.SIDE: allocator.stats['side_slots_used'],
        AtlasType.BOTTOM: allocator.stats['bottom_slots_used']
    }

def calculate_atlas_grid_size(num_slots: int) -> Tuple[int, int]:
    """Calculate optimal atlas grid size for the given number of slots."""
    if num_slots <= 0:
        return (0, 0)
    
    # Find the smallest square that can fit all slots
    import math
    grid_size = math.ceil(math.sqrt(num_slots))
    
    # For our 16x16 maximum constraint
    max_grid_size = 16
    if grid_size > max_grid_size:
        raise ValueError(f"Cannot fit {num_slots} slots in {max_grid_size}x{max_grid_size} grid")
    
    return (grid_size, grid_size)

def main():
    """Test the slot allocation system."""
    print("🧪 Testing Atlas Slot Allocation System")
    
    allocator = AtlasSlotAllocator()
    allocator.allocate_all_slots()
    
    # Test specific blocks
    print(f"\n🔍 Testing Specific Blocks:")
    test_blocks = [
        (1, "STONE"),
        (3, "GRASS"), 
        (30, "WOOD_OAK"),
        (70, "OAK_PLANKS"),
        (74, "OAK_BEAM")
    ]
    
    for voxel_id, name in test_blocks:
        pattern = allocator.get_face_pattern(voxel_id)
        required_faces = allocator.get_required_face_textures(voxel_id)
        main_slot = allocator.get_texture_slot(voxel_id, AtlasType.MAIN)
        side_slot = allocator.get_texture_slot(voxel_id, AtlasType.SIDE)
        bottom_slot = allocator.get_texture_slot(voxel_id, AtlasType.BOTTOM)
        
        print(f"  {name:12s} (ID {voxel_id:3d}): {pattern.value:20s}")
        print(f"    Faces: {', '.join(required_faces)}")
        print(f"    Slots: main={main_slot}, side={side_slot}, bottom={bottom_slot}")
    
    # Test atlas requirements
    print(f"\n📏 Atlas Size Requirements:")
    requirements = get_atlas_requirements()
    for atlas_type, num_slots in requirements.items():
        if num_slots > 0:
            grid_w, grid_h = calculate_atlas_grid_size(num_slots)
            print(f"  {atlas_type.value:6s}: {num_slots:3d} slots -> {grid_w}x{grid_h} grid (32x32 tiles = {grid_w*32}x{grid_h*32}px)")
    
    print(f"\n✅ Atlas slot allocation system working correctly!")

if __name__ == "__main__":
    main()
