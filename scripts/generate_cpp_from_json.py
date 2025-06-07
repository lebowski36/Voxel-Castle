#!/usr/bin/env python3
"""
C++ Code Generator for Unified Block System

This script reads the unified JSON block data and generates C++ initialization code
for the BlockPropertiesManager. This provides a bridge between the unified JSON system
and the C++ engine until we can add proper JSON parsing to the C++ side.

Generated files:
- block_properties_generated.cpp: Generated initialization code
- block_properties_generated.h: Generated header with declarations
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, Any, List

def load_all_block_data() -> Dict[str, Any]:
    """Load all block data from JSON files."""
    data_dir = Path(__file__).parent.parent / "data" / "blocks"
    
    all_blocks = {}
    
    # Load all JSON files in the blocks directory
    for json_file in data_dir.glob("*.json"):
        if json_file.name == "block_schema_proposal.json":
            continue
            
        print(f"Loading {json_file.name}...")
        with open(json_file, 'r') as f:
            data = json.load(f)
            
        # Extract blocks from this file
        if "blocks" in data:
            for block_name, block_data in data["blocks"].items():
                all_blocks[block_name] = block_data
                
    print(f"Loaded {len(all_blocks)} blocks total")
    return all_blocks

def map_tool_type(tool_str: str) -> str:
    """Map JSON tool string to C++ enum value."""
    tool_map = {
        "none": "BlockProperties::ToolType::NONE",
        "pickaxe": "BlockProperties::ToolType::PICKAXE", 
        "axe": "BlockProperties::ToolType::AXE",
        "shovel": "BlockProperties::ToolType::SHOVEL",
        "shears": "BlockProperties::ToolType::SHEARS"
    }
    return tool_map.get(tool_str.lower(), "BlockProperties::ToolType::NONE")

def get_valid_voxel_types() -> set:
    """Get all valid VoxelType enum values from the C++ header."""
    # Define the known VoxelTypes based on the current enum
    # This should match the VoxelType enum in voxel_types.h
    valid_types = {
        'AIR', 'STONE', 'DIRT', 'GRASS', 'SAND', 'GRAVEL', 'CLAY', 'BEDROCK', 'TOPSOIL', 'SUBSOIL',
        'GRANITE', 'LIMESTONE', 'MARBLE', 'SANDSTONE', 'SLATE', 'BASALT', 'QUARTZITE', 'OBSIDIAN', 'PUMICE', 'SHALE',
        'COAL_ORE', 'IRON_ORE', 'COPPER_ORE', 'TIN_ORE', 'SILVER_ORE', 'GOLD_ORE', 'GEM_RUBY', 'GEM_SAPPHIRE', 'GEM_EMERALD', 'GEM_DIAMOND',
        'WOOD_OAK', 'WOOD_PINE', 'WOOD_BIRCH', 'WOOD_MAHOGANY', 'LEAVES_OAK', 'LEAVES_PINE', 'LEAVES_BIRCH', 'LEAVES_PALM', 'MUSHROOM_BROWN', 'MUSHROOM_RED',
        'SNOW', 'ICE', 'PACKED_ICE', 'CACTUS', 'JUNGLE_VINE', 'CORAL_PINK', 'CORAL_BLUE', 'SEAWEED', 'TUNDRA_MOSS', 'DESERT_ROCK',
        'WATER', 'LAVA', 'OIL', 'ACID', 'HONEY', 'STEAM', 'TOXIC_GAS', 'NATURAL_GAS', 'MAGICAL_MIST', 'SMOKE',
        'STONE_BRICK', 'GRANITE_BRICK', 'MARBLE_TILE', 'SANDSTONE_BRICK', 'SLATE_TILE', 'COBBLESTONE', 'SMOOTH_STONE', 'POLISHED_GRANITE', 'POLISHED_MARBLE', 'FLAGSTONE',
        'OAK_PLANKS', 'PINE_PLANKS', 'BIRCH_PLANKS', 'MAHOGANY_PLANKS', 'OAK_BEAM', 'PINE_BEAM', 'HARDWOOD_BEAM', 'BAMBOO_PLANK', 'CORK', 'CHARCOAL_BLOCK',
        'IRON_BLOCK', 'COPPER_BLOCK', 'BRONZE_BLOCK', 'STEEL_BLOCK', 'SILVER_BLOCK', 'GOLD_BLOCK', 'BRASS_BLOCK', 'PEWTER_BLOCK', 'MITHRIL_BLOCK', 'ADAMANTINE_BLOCK',
        'CLAY_BRICK', 'TERRACOTTA', 'GLAZED_TILE_WHITE', 'GLAZED_TILE_RED', 'GLAZED_TILE_BLUE', 'GLAZED_TILE_GREEN', 'PORCELAIN', 'STONEWARE', 'EARTHENWARE', 'CERAMIC_TILE',
        'DOOR_WOOD', 'DOOR_IRON', 'DOOR_REINFORCED', 'WINDOW_GLASS', 'WINDOW_IRON_BARS', 'PORTCULLIS', 'DRAWBRIDGE', 'GATE_WOOD', 'GATE_IRON', 'HATCH_WOOD',
        'CHEST_WOOD', 'CHEST_IRON', 'BARREL', 'CRATE', 'BIN_STORAGE', 'SHELF', 'CABINET', 'STRONGBOX', 'VAULT_DOOR', 'SAFE',
        'LEVER', 'BUTTON', 'PRESSURE_PLATE', 'GEAR_MECHANISM', 'PULLEY', 'ROPE', 'CHAIN', 'AXLE', 'PUMP', 'VALVE',
        'TORCH', 'LANTERN', 'CANDLE', 'CHANDELIER', 'BRAZIER', 'CAMPFIRE', 'FURNACE', 'FORGE', 'KILN', 'CRYSTAL_LIGHT',
        'TABLE_WOOD', 'CHAIR_WOOD', 'BED', 'THRONE', 'ALTAR', 'PODIUM', 'BENCH', 'STOOL', 'BOOKSHELF', 'DESK',
        'CRYSTAL_CLEAR', 'CRYSTAL_BLUE', 'CRYSTAL_RED', 'CRYSTAL_GREEN', 'ENCHANTED_STONE', 'RUNIC_BLOCK', 'ETHER_CRYSTAL', 'VOID_STONE', 'CELESTIAL_MARBLE', 'SHADOW_GLASS',
        'CONCRETE', 'REINFORCED_CONCRETE', 'STEEL_BEAM', 'GLASS_STEEL', 'COMPOSITE_ARMOR', 'ENERGY_CONDUIT', 'FORCE_FIELD_GEN', 'QUANTUM_BLOCK', 'NANOCARBON', 'PLASMA_CONTAINER',
        'BLOCK_INVISIBLE', 'BLOCK_INTANGIBLE', 'BLOCK_ANTIGRAV', 'BLOCK_MAGNETIC', 'BLOCK_TEMPORAL', 'BLOCK_DIMENSIONAL', 'BLOCK_REGENERATING', 'BLOCK_EXPLOSIVE', 'BLOCK_ABSORBING', 'BLOCK_AMPLIFYING'
    }
    return valid_types

def generate_cpp_props(block_name: str, block_data: Dict[str, Any]) -> str:
    """Generate C++ code for one block's properties."""
    
    # Check if this block type exists in the VoxelType enum
    valid_types = get_valid_voxel_types()
    if block_name not in valid_types:
        print(f"⚠️  Skipping {block_name} - not found in VoxelType enum")
        return ""
    
    # Extract data with defaults
    name = block_data.get("name", block_name.title())
    physical = block_data.get("physical_properties", {})
    gameplay = block_data.get("gameplay_properties", {})
    
    # Physical properties
    is_solid = physical.get("solid", True)
    is_transparent = physical.get("transparent", False)
    is_fluid = physical.get("fluid", False)
    hardness = physical.get("hardness", 1.0)
    
    # Gameplay properties
    tool_req = gameplay.get("tool_requirements", {})
    required_tool = tool_req.get("required_tool", "none")
    tool_level = tool_req.get("minimum_tier", 0)
    
    lighting = gameplay.get("lighting", {})
    emits_light = lighting.get("emits_light", False)
    light_level = lighting.get("light_level", 0)
    
    flammability = gameplay.get("flammability", {})
    is_flammable = flammability.get("flammable", False)
    fire_spread_rate = flammability.get("fire_spread_rate", 0.0)
    
    # Generate C++ initialization code
    cpp_code = f"""    // {name} (ID: {block_data.get('id', 'unknown')})
    {{
        BlockProperties props;
        props.name = "{name}";
        props.is_solid = {str(is_solid).lower()};
        props.is_transparent = {str(is_transparent).lower()};
        props.is_fluid = {str(is_fluid).lower()};
        props.is_walkable = {str(is_solid and not is_fluid).lower()};
        props.hardness = {hardness:.1f}f;
        props.blast_resistance = {hardness * 3.0:.1f}f;
        props.required_tool = {map_tool_type(required_tool)};
        props.required_tool_level = {tool_level};
        props.is_flammable = {str(is_flammable).lower()};
        props.fire_spread_chance = {fire_spread_rate:.1f}f;
        props.emits_light = {str(emits_light).lower()};
        props.light_level = {light_level};
        props.friction = {0.1 if is_fluid else 0.6:.1f}f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::{block_name} != VoxelType::AIR) {{
            props.possible_drops[0] = VoxelType::{block_name};
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }}
        
        manager.registerBlockProperties(VoxelType::{block_name}, props);
    }}
"""
    return cpp_code

def generate_header_file(blocks: Dict[str, Any]) -> str:
    """Generate the header file content."""
    header = """#ifndef VOXEL_ENGINE_BLOCK_PROPERTIES_GENERATED_H
#define VOXEL_ENGINE_BLOCK_PROPERTIES_GENERATED_H

#include "world/block_properties.h"

namespace VoxelEngine {
namespace World {

class BlockPropertiesGenerator {
public:
    /**
     * @brief Initialize all block properties from unified JSON data.
     * @param manager The BlockPropertiesManager to populate.
     */
    static void initializeFromUnifiedData(BlockPropertiesManager& manager);
    
private:
    static void registerAllBlocks(BlockPropertiesManager& manager);
};

} // namespace World
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_BLOCK_PROPERTIES_GENERATED_H
"""
    return header

def generate_implementation_file(blocks: Dict[str, Any]) -> str:
    """Generate the implementation file content."""
    
    # Sort blocks by ID to maintain order
    sorted_blocks = sorted(blocks.items(), key=lambda x: x[1].get('id', 999))
    
    # Filter out blocks that don't exist in VoxelType enum
    valid_types = get_valid_voxel_types()
    filtered_blocks = [(name, data) for name, data in sorted_blocks if name in valid_types]
    
    print(f"📊 Generating code for {len(filtered_blocks)} blocks (out of {len(blocks)} total)")
    
    impl = f"""#include "world/block_properties_generated.h"
#include <iostream>

namespace VoxelEngine {{
namespace World {{

void BlockPropertiesGenerator::initializeFromUnifiedData(BlockPropertiesManager& manager) {{
    std::cout << "[BlockPropertiesGenerator] Initializing properties from unified JSON data..." << std::endl;
    registerAllBlocks(manager);
    std::cout << "[BlockPropertiesGenerator] Initialized properties for {len(filtered_blocks)} block types." << std::endl;
}}

void BlockPropertiesGenerator::registerAllBlocks(BlockPropertiesManager& manager) {{
    // Auto-generated from unified JSON block data
    // Generated on: {__import__('datetime').datetime.now().isoformat()}
    // Blocks with matching VoxelType enum entries: {len(filtered_blocks)} of {len(blocks)}
    
"""
    
    # Generate code for each valid block
    for block_name, block_data in filtered_blocks:
        block_code = generate_cpp_props(block_name, block_data)
        if block_code:  # Only add if code was generated
            impl += block_code
            impl += "\n"
    
    impl += """}

} // namespace World
} // namespace VoxelEngine
"""
    
    return impl

def main():
    """Main function to generate C++ code from JSON data."""
    print("🔧 Generating C++ code from unified JSON block data...")
    
    # Load all block data
    try:
        blocks = load_all_block_data()
    except Exception as e:
        print(f"❌ Error loading block data: {e}")
        return 1
    
    if not blocks:
        print("❌ No block data found!")
        return 1
    
    # Generate C++ files
    try:
        header_content = generate_header_file(blocks)
        impl_content = generate_implementation_file(blocks)
        
        # Write files to engine include/src directories
        project_root = Path(__file__).parent.parent
        
        header_path = project_root / "engine" / "include" / "world" / "block_properties_generated.h"
        impl_path = project_root / "engine" / "src" / "world" / "block_properties_generated.cpp"
        
        # Ensure directories exist
        header_path.parent.mkdir(parents=True, exist_ok=True)
        impl_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Write header file
        with open(header_path, 'w') as f:
            f.write(header_content)
        print(f"✅ Generated header: {header_path}")
        
        # Write implementation file
        with open(impl_path, 'w') as f:
            f.write(impl_content)
        print(f"✅ Generated implementation: {impl_path}")
        
        print(f"🎉 Successfully generated C++ code for {len(blocks)} blocks!")
        print("📝 Next steps:")
        print("   1. Add the generated files to your CMakeLists.txt")
        print("   2. Call BlockPropertiesGenerator::initializeFromUnifiedData() in your BlockPropertiesManager constructor")
        print("   3. Build and test the unified system")
        
        return 0
        
    except Exception as e:
        print(f"❌ Error generating C++ code: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
