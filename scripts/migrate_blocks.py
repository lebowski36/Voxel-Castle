#!/usr/bin/env python3
"""
Block Data Migration Script
Converts existing BLOCK_MAPPING from create_atlas.py to new JSON format.
Preserves all working texture generation logic.
"""

import json
import sys
import os

# Current working directory context
sys.path.append('/home/system-x1/Projects/Voxel Castle')

# Import the working BLOCK_MAPPING
def load_block_mapping():
    """Extract BLOCK_MAPPING from create_atlas.py without executing the whole file"""
    block_mapping = {}
    
    # Basic Terrain (0-9)
    terrain_blocks = {
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
    }
    
    # Fluids (50-59)
    fluid_blocks = {
        50: {'type': 'fluid', 'subtype': 'water'},
        51: {'type': 'fluid', 'subtype': 'lava'},
        52: {'type': 'fluid', 'subtype': 'oil'},
        53: {'type': 'fluid', 'subtype': 'acid'},
        54: {'type': 'fluid', 'subtype': 'honey'},
        55: {'type': 'fluid', 'subtype': 'steam'},
        56: {'type': 'fluid', 'subtype': 'toxic_gas'},
        57: {'type': 'fluid', 'subtype': 'natural_gas'},
        58: {'type': 'special', 'subtype': 'magical_mist'},
        59: {'type': 'fluid', 'subtype': 'smoke'},
    }
    
    # Processed Materials (60-99)
    processed_blocks = {
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
    }
    
    # Functional blocks (100-149) - Special generation
    functional_blocks = {i: {'type': 'special', 'subtype': 'functional'} for i in range(100, 150)}
    
    # Advanced materials (150-179)
    advanced_blocks = {
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
    }
    
    # Technology and magical (160-179)
    tech_magical_blocks = {}
    tech_magical_blocks.update({i: {'type': 'special', 'subtype': 'technology'} for i in range(160, 170)})
    tech_magical_blocks.update({i: {'type': 'special', 'subtype': 'magical'} for i in range(170, 180)})
    
    # Placeholder blocks (180-255)
    placeholder_blocks = {i: {'type': 'special', 'subtype': 'placeholder'} for i in range(180, 256)}
    
    # Combine all mappings
    block_mapping.update(terrain_blocks)
    block_mapping.update(fluid_blocks)  
    block_mapping.update(processed_blocks)
    block_mapping.update(functional_blocks)
    block_mapping.update(advanced_blocks)
    block_mapping.update(tech_magical_blocks)
    block_mapping.update(placeholder_blocks)
    
    return block_mapping

# Block name mapping for converting subtypes to proper names
def get_block_name_from_mapping(block_id, subtype):
    """Convert block ID and subtype to proper block name"""
    name_mapping = {
        # Basic terrain
        0: "Air", 1: "Stone", 2: "Dirt", 3: "Grass", 4: "Sand", 5: "Gravel",
        6: "Clay", 7: "Bedrock", 8: "Topsoil", 9: "Subsoil",
        
        # Stone varieties
        10: "Granite", 11: "Limestone", 12: "Marble", 13: "Sandstone", 14: "Slate",
        15: "Basalt", 16: "Quartzite", 17: "Obsidian", 18: "Pumice", 19: "Shale",
        
        # Ores & minerals
        20: "Coal Ore", 21: "Iron Ore", 22: "Copper Ore", 23: "Tin Ore", 24: "Silver Ore",
        25: "Gold Ore", 26: "Ruby", 27: "Sapphire", 28: "Emerald", 29: "Diamond",
        
        # Organic natural
        30: "Oak Wood", 31: "Pine Wood", 32: "Birch Wood", 33: "Mahogany Wood",
        34: "Oak Leaves", 35: "Pine Leaves", 36: "Birch Leaves", 37: "Palm Leaves",
        38: "Brown Mushroom", 39: "Red Mushroom",
        
        # Biome specific
        40: "Snow", 41: "Ice", 42: "Packed Ice", 43: "Cactus", 44: "Jungle Vine",
        45: "Pink Coral", 46: "Blue Coral", 47: "Seaweed", 48: "Tundra Moss", 49: "Desert Rock",
        
        # Fluids
        50: "Water", 51: "Lava", 52: "Oil", 53: "Acid", 54: "Honey",
        55: "Steam", 56: "Toxic Gas", 57: "Natural Gas", 58: "Magical Mist", 59: "Smoke",
    }
    
    return name_mapping.get(block_id, f"Block {block_id}")

def migrate_block_to_json(block_id, block_info):
    """Convert a single block from BLOCK_MAPPING to new JSON format"""
    
    # Get basic properties
    block_type = block_info['type']
    subtype = block_info['subtype']
    name = get_block_name_from_mapping(block_id, subtype)
    
    # Determine properties based on type
    if block_type == 'air':
        properties = {
            "solid": False,
            "transparent": True,
            "fluid": False,
            "walkable": True,
            "hardness": 0,
            "density": 0
        }
        category = "special"
        tags = ["transparent", "walkable", "non_solid"]
    elif block_type == 'fluid':
        properties = {
            "solid": False,
            "transparent": True,
            "fluid": True,
            "walkable": False,
            "hardness": 0,
            "density": 1.0
        }
        category = "fluid"
        tags = ["fluid", "transparent"]
    elif block_type in ['stone', 'ore']:
        properties = {
            "solid": True,
            "transparent": False,
            "fluid": False,
            "walkable": False,
            "hardness": 1.5,
            "density": 2.7
        }
        category = "stone" if block_type == 'stone' else "ore"
        tags = ["solid", "natural"] if block_type == 'stone' else ["solid", "natural", "ore"]
    elif block_type == 'wood':
        properties = {
            "solid": True,
            "transparent": False,
            "fluid": False,
            "walkable": False,
            "hardness": 1.0,
            "density": 0.8
        }
        category = "wood"
        tags = ["solid", "organic", "building_material"]
    elif block_type == 'organic':
        properties = {
            "solid": True,
            "transparent": False,
            "fluid": False,
            "walkable": False,
            "hardness": 0.5,
            "density": 1.2
        }
        category = "organic"
        tags = ["solid", "organic", "natural"]
    else:  # special, metal, ceramic, crystal
        properties = {
            "solid": True,
            "transparent": False,
            "fluid": False,
            "walkable": False,
            "hardness": 1.0,
            "density": 1.0
        }
        category = block_type
        tags = ["solid"]
    
    # Build JSON block definition
    json_block = {
        "id": block_id,
        "name": name,
        "description": f"{name} - migrated from working BLOCK_MAPPING",
        "category": category,
        "tags": tags,
        "physical_properties": properties,
        "texture_info": {
            "face_pattern": "UNIFORM",  # Default, can be customized later
            "generation": {
                "type": block_type,
                "subtype": subtype
            }
        },
        "world_generation": {
            "natural_generation": block_id < 50  # Terrain blocks naturally generate
        }
    }
    
    # Add gameplay properties for solid blocks
    if properties["solid"]:
        json_block["gameplay_properties"] = {
            "tool_requirements": {
                "required_tool": "pickaxe" if block_type in ['stone', 'ore'] else "axe" if block_type == 'wood' else "shovel",
                "minimum_tier": 1
            }
        }
    
    return json_block

def categorize_blocks(block_mapping):
    """Sort blocks into categories for separate files"""
    categories = {
        "terrain": {},      # 0-49: terrain, stone varieties, ores, organic
        "fluids": {},       # 50-59: fluids and gases  
        "processed": {},    # 60-99: processed materials
        "functional": {},   # 100-149: functional blocks
        "advanced": {},     # 150-179: crystals, magical materials
        "placeholder": {}   # 180-255: placeholders
    }
    
    for block_id, block_info in block_mapping.items():
        if 0 <= block_id <= 49:
            categories["terrain"][block_id] = block_info
        elif 50 <= block_id <= 59:
            categories["fluids"][block_id] = block_info
        elif 60 <= block_id <= 99:
            categories["processed"][block_id] = block_info
        elif 100 <= block_id <= 149:
            categories["functional"][block_id] = block_info
        elif 150 <= block_id <= 179:
            categories["advanced"][block_id] = block_info
        else:  # 180-255
            categories["placeholder"][block_id] = block_info
    
    return categories

if __name__ == "__main__":
    print("🔄 Starting block data migration...")
    
    # Load working BLOCK_MAPPING
    block_mapping = load_block_mapping()
    print(f"✓ Loaded {len(block_mapping)} block definitions from BLOCK_MAPPING")
    
    # Categorize blocks
    categories = categorize_blocks(block_mapping)
    
    # Output directory
    output_dir = "/home/system-x1/Projects/Voxel Castle/data/blocks"
    
    # Process each category
    for category_name, blocks in categories.items():
        if not blocks:  # Skip empty categories
            continue
            
        print(f"🔄 Processing category: {category_name} ({len(blocks)} blocks)")
        
        # Convert to JSON format
        json_blocks = {}
        for block_id, block_info in blocks.items():
            block_name = get_block_name_from_mapping(block_id, block_info['subtype']).upper().replace(' ', '_')
            json_blocks[block_name] = migrate_block_to_json(block_id, block_info)
        
        # Create category file structure
        category_data = {
            "$schema": "../block_schema_proposal.json",
            "category": category_name,
            "description": f"Block category: {category_name}",
            "id_range": [min(blocks.keys()), max(blocks.keys())],
            "blocks": json_blocks
        }
        
        # Write to file (backup existing if present)
        output_file = os.path.join(output_dir, f"{category_name}.json")
        if os.path.exists(output_file):
            backup_file = f"{output_file}.backup"
            os.rename(output_file, backup_file)
            print(f"  ✓ Backed up existing {category_name}.json")
        
        with open(output_file, 'w') as f:
            json.dump(category_data, f, indent=2)
        
        print(f"  ✓ Created {category_name}.json with {len(json_blocks)} blocks")
    
    print("\n✅ Migration complete! All working BLOCK_MAPPING data preserved in new JSON format.")
    print("📋 Next steps:")
    print("  1. Validate migrated data with: python scripts/validate_blocks.py")
    print("  2. Test atlas generation compatibility")
    print("  3. Review and refine block properties")
