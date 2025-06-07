#include "world/block_properties_generated.h"
#include <iostream>

namespace VoxelEngine {
namespace World {

void BlockPropertiesGenerator::initializeFromUnifiedData(BlockPropertiesManager& manager) {
    std::cout << "[BlockPropertiesGenerator] Initializing properties from unified JSON data..." << std::endl;
    registerAllBlocks(manager);
    std::cout << "[BlockPropertiesGenerator] Initialized properties for 44 block types." << std::endl;
}

void BlockPropertiesGenerator::registerAllBlocks(BlockPropertiesManager& manager) {
    // Auto-generated from unified JSON block data
    // Generated on: 2025-06-08T00:14:48.797519
    // Blocks with matching VoxelType enum entries: 44 of 256
    
    // Air (ID: 0)
    {
        BlockProperties props;
        props.name = "Air";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = false;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::AIR != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::AIR;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::AIR, props);
    }

    // Stone (ID: 1)
    {
        BlockProperties props;
        props.name = "Stone";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::STONE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::STONE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::STONE, props);
    }

    // Dirt (ID: 2)
    {
        BlockProperties props;
        props.name = "Dirt";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::DIRT != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::DIRT;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::DIRT, props);
    }

    // Grass (ID: 3)
    {
        BlockProperties props;
        props.name = "Grass";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::GRASS != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::GRASS;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::GRASS, props);
    }

    // Sand (ID: 4)
    {
        BlockProperties props;
        props.name = "Sand";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SAND != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SAND;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SAND, props);
    }

    // Gravel (ID: 5)
    {
        BlockProperties props;
        props.name = "Gravel";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::GRAVEL != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::GRAVEL;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::GRAVEL, props);
    }

    // Clay (ID: 6)
    {
        BlockProperties props;
        props.name = "Clay";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::CLAY != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::CLAY;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::CLAY, props);
    }

    // Bedrock (ID: 7)
    {
        BlockProperties props;
        props.name = "Bedrock";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BEDROCK != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BEDROCK;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::BEDROCK, props);
    }

    // Topsoil (ID: 8)
    {
        BlockProperties props;
        props.name = "Topsoil";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::TOPSOIL != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::TOPSOIL;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::TOPSOIL, props);
    }

    // Subsoil (ID: 9)
    {
        BlockProperties props;
        props.name = "Subsoil";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SUBSOIL != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SUBSOIL;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SUBSOIL, props);
    }

    // Granite (ID: 10)
    {
        BlockProperties props;
        props.name = "Granite";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::GRANITE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::GRANITE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::GRANITE, props);
    }

    // Limestone (ID: 11)
    {
        BlockProperties props;
        props.name = "Limestone";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::LIMESTONE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::LIMESTONE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::LIMESTONE, props);
    }

    // Marble (ID: 12)
    {
        BlockProperties props;
        props.name = "Marble";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::MARBLE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::MARBLE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::MARBLE, props);
    }

    // Sandstone (ID: 13)
    {
        BlockProperties props;
        props.name = "Sandstone";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SANDSTONE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SANDSTONE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SANDSTONE, props);
    }

    // Slate (ID: 14)
    {
        BlockProperties props;
        props.name = "Slate";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SLATE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SLATE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SLATE, props);
    }

    // Basalt (ID: 15)
    {
        BlockProperties props;
        props.name = "Basalt";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BASALT != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BASALT;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::BASALT, props);
    }

    // Quartzite (ID: 16)
    {
        BlockProperties props;
        props.name = "Quartzite";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::QUARTZITE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::QUARTZITE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::QUARTZITE, props);
    }

    // Obsidian (ID: 17)
    {
        BlockProperties props;
        props.name = "Obsidian";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::OBSIDIAN != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::OBSIDIAN;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::OBSIDIAN, props);
    }

    // Pumice (ID: 18)
    {
        BlockProperties props;
        props.name = "Pumice";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::PUMICE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::PUMICE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::PUMICE, props);
    }

    // Shale (ID: 19)
    {
        BlockProperties props;
        props.name = "Shale";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SHALE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SHALE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SHALE, props);
    }

    // Coal Ore (ID: 20)
    {
        BlockProperties props;
        props.name = "Coal Ore";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::COAL_ORE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::COAL_ORE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::COAL_ORE, props);
    }

    // Iron Ore (ID: 21)
    {
        BlockProperties props;
        props.name = "Iron Ore";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::IRON_ORE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::IRON_ORE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::IRON_ORE, props);
    }

    // Copper Ore (ID: 22)
    {
        BlockProperties props;
        props.name = "Copper Ore";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::COPPER_ORE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::COPPER_ORE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::COPPER_ORE, props);
    }

    // Tin Ore (ID: 23)
    {
        BlockProperties props;
        props.name = "Tin Ore";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::TIN_ORE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::TIN_ORE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::TIN_ORE, props);
    }

    // Silver Ore (ID: 24)
    {
        BlockProperties props;
        props.name = "Silver Ore";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SILVER_ORE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SILVER_ORE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SILVER_ORE, props);
    }

    // Gold Ore (ID: 25)
    {
        BlockProperties props;
        props.name = "Gold Ore";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::GOLD_ORE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::GOLD_ORE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::GOLD_ORE, props);
    }

    // Snow (ID: 40)
    {
        BlockProperties props;
        props.name = "Snow";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SNOW != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SNOW;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SNOW, props);
    }

    // Ice (ID: 41)
    {
        BlockProperties props;
        props.name = "Ice";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::ICE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::ICE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::ICE, props);
    }

    // Packed Ice (ID: 42)
    {
        BlockProperties props;
        props.name = "Packed Ice";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::PACKED_ICE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::PACKED_ICE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::PACKED_ICE, props);
    }

    // Cactus (ID: 43)
    {
        BlockProperties props;
        props.name = "Cactus";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::CACTUS != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::CACTUS;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::CACTUS, props);
    }

    // Jungle Vine (ID: 44)
    {
        BlockProperties props;
        props.name = "Jungle Vine";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::JUNGLE_VINE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::JUNGLE_VINE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::JUNGLE_VINE, props);
    }

    // Seaweed (ID: 47)
    {
        BlockProperties props;
        props.name = "Seaweed";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SEAWEED != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SEAWEED;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SEAWEED, props);
    }

    // Tundra Moss (ID: 48)
    {
        BlockProperties props;
        props.name = "Tundra Moss";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 0.5f;
        props.blast_resistance = 1.5f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::TUNDRA_MOSS != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::TUNDRA_MOSS;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::TUNDRA_MOSS, props);
    }

    // Desert Rock (ID: 49)
    {
        BlockProperties props;
        props.name = "Desert Rock";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.5f;
        props.blast_resistance = 4.5f;
        props.required_tool = BlockProperties::ToolType::PICKAXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::DESERT_ROCK != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::DESERT_ROCK;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::DESERT_ROCK, props);
    }

    // Water (ID: 50)
    {
        BlockProperties props;
        props.name = "Water";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::WATER != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::WATER;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::WATER, props);
    }

    // Lava (ID: 51)
    {
        BlockProperties props;
        props.name = "Lava";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::LAVA != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::LAVA;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::LAVA, props);
    }

    // Oil (ID: 52)
    {
        BlockProperties props;
        props.name = "Oil";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::OIL != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::OIL;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::OIL, props);
    }

    // Acid (ID: 53)
    {
        BlockProperties props;
        props.name = "Acid";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::ACID != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::ACID;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::ACID, props);
    }

    // Honey (ID: 54)
    {
        BlockProperties props;
        props.name = "Honey";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::HONEY != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::HONEY;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::HONEY, props);
    }

    // Steam (ID: 55)
    {
        BlockProperties props;
        props.name = "Steam";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::STEAM != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::STEAM;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::STEAM, props);
    }

    // Toxic Gas (ID: 56)
    {
        BlockProperties props;
        props.name = "Toxic Gas";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::TOXIC_GAS != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::TOXIC_GAS;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::TOXIC_GAS, props);
    }

    // Natural Gas (ID: 57)
    {
        BlockProperties props;
        props.name = "Natural Gas";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::NATURAL_GAS != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::NATURAL_GAS;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::NATURAL_GAS, props);
    }

    // Magical Mist (ID: 58)
    {
        BlockProperties props;
        props.name = "Magical Mist";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::SHOVEL;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::MAGICAL_MIST != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::MAGICAL_MIST;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::MAGICAL_MIST, props);
    }

    // Smoke (ID: 59)
    {
        BlockProperties props;
        props.name = "Smoke";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0.0f;
        props.blast_resistance = 0.0f;
        props.required_tool = BlockProperties::ToolType::NONE;
        props.required_tool_level = 0;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.1f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::SMOKE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SMOKE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        manager.registerBlockProperties(VoxelType::SMOKE, props);
    }

}

} // namespace World
} // namespace VoxelEngine
