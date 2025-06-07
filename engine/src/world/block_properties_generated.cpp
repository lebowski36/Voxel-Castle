#include "world/block_properties_generated.h"
#include <iostream>

namespace VoxelEngine {
namespace World {

void BlockPropertiesGenerator::initializeFromUnifiedData(BlockPropertiesManager& manager) {
    std::cout << "[BlockPropertiesGenerator] Initializing properties from unified JSON data..." << std::endl;
    registerAllBlocks(manager);
    std::cout << "[BlockPropertiesGenerator] Initialized properties for 256 block types." << std::endl;
}

void BlockPropertiesGenerator::registerAllBlocks(BlockPropertiesManager& manager) {
    // Auto-generated from unified JSON block data
    // Generated on: 2025-06-08T00:03:37.152817
    
    // Air (ID: 0)
    {
        BlockProperties props;
        props.name = "Air";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = false;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::AIR, props);
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
        
        registerBlockProperties(VoxelType::STONE, props);
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
        
        registerBlockProperties(VoxelType::DIRT, props);
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
        
        registerBlockProperties(VoxelType::GRASS, props);
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
        
        registerBlockProperties(VoxelType::SAND, props);
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
        
        registerBlockProperties(VoxelType::GRAVEL, props);
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
        
        registerBlockProperties(VoxelType::CLAY, props);
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
        
        registerBlockProperties(VoxelType::BEDROCK, props);
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
        
        registerBlockProperties(VoxelType::TOPSOIL, props);
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
        
        registerBlockProperties(VoxelType::SUBSOIL, props);
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
        
        registerBlockProperties(VoxelType::GRANITE, props);
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
        
        registerBlockProperties(VoxelType::LIMESTONE, props);
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
        
        registerBlockProperties(VoxelType::MARBLE, props);
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
        
        registerBlockProperties(VoxelType::SANDSTONE, props);
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
        
        registerBlockProperties(VoxelType::SLATE, props);
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
        
        registerBlockProperties(VoxelType::BASALT, props);
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
        
        registerBlockProperties(VoxelType::QUARTZITE, props);
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
        
        registerBlockProperties(VoxelType::OBSIDIAN, props);
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
        
        registerBlockProperties(VoxelType::PUMICE, props);
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
        
        registerBlockProperties(VoxelType::SHALE, props);
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
        
        registerBlockProperties(VoxelType::COAL_ORE, props);
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
        
        registerBlockProperties(VoxelType::IRON_ORE, props);
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
        
        registerBlockProperties(VoxelType::COPPER_ORE, props);
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
        
        registerBlockProperties(VoxelType::TIN_ORE, props);
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
        
        registerBlockProperties(VoxelType::SILVER_ORE, props);
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
        
        registerBlockProperties(VoxelType::GOLD_ORE, props);
    }

    // Ruby (ID: 26)
    {
        BlockProperties props;
        props.name = "Ruby";
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
        if (VoxelType::RUBY != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::RUBY;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::RUBY, props);
    }

    // Sapphire (ID: 27)
    {
        BlockProperties props;
        props.name = "Sapphire";
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
        if (VoxelType::SAPPHIRE != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::SAPPHIRE;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::SAPPHIRE, props);
    }

    // Emerald (ID: 28)
    {
        BlockProperties props;
        props.name = "Emerald";
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
        if (VoxelType::EMERALD != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::EMERALD;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::EMERALD, props);
    }

    // Diamond (ID: 29)
    {
        BlockProperties props;
        props.name = "Diamond";
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
        if (VoxelType::DIAMOND != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::DIAMOND;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::DIAMOND, props);
    }

    // Oak Wood (ID: 30)
    {
        BlockProperties props;
        props.name = "Oak Wood";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::OAK_WOOD != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::OAK_WOOD;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::OAK_WOOD, props);
    }

    // Pine Wood (ID: 31)
    {
        BlockProperties props;
        props.name = "Pine Wood";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::PINE_WOOD != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::PINE_WOOD;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::PINE_WOOD, props);
    }

    // Birch Wood (ID: 32)
    {
        BlockProperties props;
        props.name = "Birch Wood";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BIRCH_WOOD != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BIRCH_WOOD;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BIRCH_WOOD, props);
    }

    // Mahogany Wood (ID: 33)
    {
        BlockProperties props;
        props.name = "Mahogany Wood";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::MAHOGANY_WOOD != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::MAHOGANY_WOOD;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::MAHOGANY_WOOD, props);
    }

    // Oak Leaves (ID: 34)
    {
        BlockProperties props;
        props.name = "Oak Leaves";
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
        if (VoxelType::OAK_LEAVES != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::OAK_LEAVES;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::OAK_LEAVES, props);
    }

    // Pine Leaves (ID: 35)
    {
        BlockProperties props;
        props.name = "Pine Leaves";
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
        if (VoxelType::PINE_LEAVES != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::PINE_LEAVES;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::PINE_LEAVES, props);
    }

    // Birch Leaves (ID: 36)
    {
        BlockProperties props;
        props.name = "Birch Leaves";
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
        if (VoxelType::BIRCH_LEAVES != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BIRCH_LEAVES;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BIRCH_LEAVES, props);
    }

    // Palm Leaves (ID: 37)
    {
        BlockProperties props;
        props.name = "Palm Leaves";
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
        if (VoxelType::PALM_LEAVES != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::PALM_LEAVES;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::PALM_LEAVES, props);
    }

    // Brown Mushroom (ID: 38)
    {
        BlockProperties props;
        props.name = "Brown Mushroom";
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
        if (VoxelType::BROWN_MUSHROOM != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BROWN_MUSHROOM;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BROWN_MUSHROOM, props);
    }

    // Red Mushroom (ID: 39)
    {
        BlockProperties props;
        props.name = "Red Mushroom";
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
        if (VoxelType::RED_MUSHROOM != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::RED_MUSHROOM;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::RED_MUSHROOM, props);
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
        
        registerBlockProperties(VoxelType::SNOW, props);
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
        
        registerBlockProperties(VoxelType::ICE, props);
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
        
        registerBlockProperties(VoxelType::PACKED_ICE, props);
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
        
        registerBlockProperties(VoxelType::CACTUS, props);
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
        
        registerBlockProperties(VoxelType::JUNGLE_VINE, props);
    }

    // Pink Coral (ID: 45)
    {
        BlockProperties props;
        props.name = "Pink Coral";
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
        if (VoxelType::PINK_CORAL != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::PINK_CORAL;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::PINK_CORAL, props);
    }

    // Blue Coral (ID: 46)
    {
        BlockProperties props;
        props.name = "Blue Coral";
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
        if (VoxelType::BLUE_CORAL != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLUE_CORAL;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLUE_CORAL, props);
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
        
        registerBlockProperties(VoxelType::SEAWEED, props);
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
        
        registerBlockProperties(VoxelType::TUNDRA_MOSS, props);
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
        
        registerBlockProperties(VoxelType::DESERT_ROCK, props);
    }

    // Water (ID: 50)
    {
        BlockProperties props;
        props.name = "Water";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::WATER, props);
    }

    // Lava (ID: 51)
    {
        BlockProperties props;
        props.name = "Lava";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::LAVA, props);
    }

    // Oil (ID: 52)
    {
        BlockProperties props;
        props.name = "Oil";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::OIL, props);
    }

    // Acid (ID: 53)
    {
        BlockProperties props;
        props.name = "Acid";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::ACID, props);
    }

    // Honey (ID: 54)
    {
        BlockProperties props;
        props.name = "Honey";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::HONEY, props);
    }

    // Steam (ID: 55)
    {
        BlockProperties props;
        props.name = "Steam";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::STEAM, props);
    }

    // Toxic Gas (ID: 56)
    {
        BlockProperties props;
        props.name = "Toxic Gas";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::TOXIC_GAS, props);
    }

    // Natural Gas (ID: 57)
    {
        BlockProperties props;
        props.name = "Natural Gas";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::NATURAL_GAS, props);
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
        
        registerBlockProperties(VoxelType::MAGICAL_MIST, props);
    }

    // Smoke (ID: 59)
    {
        BlockProperties props;
        props.name = "Smoke";
        props.is_solid = false;
        props.is_transparent = true;
        props.is_fluid = true;
        props.is_walkable = false;
        props.hardness = 0f;
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
        
        registerBlockProperties(VoxelType::SMOKE, props);
    }

    // Block 60 (ID: 60)
    {
        BlockProperties props;
        props.name = "Block 60";
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
        if (VoxelType::BLOCK_60 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_60;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_60, props);
    }

    // Block 61 (ID: 61)
    {
        BlockProperties props;
        props.name = "Block 61";
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
        if (VoxelType::BLOCK_61 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_61;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_61, props);
    }

    // Block 62 (ID: 62)
    {
        BlockProperties props;
        props.name = "Block 62";
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
        if (VoxelType::BLOCK_62 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_62;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_62, props);
    }

    // Block 63 (ID: 63)
    {
        BlockProperties props;
        props.name = "Block 63";
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
        if (VoxelType::BLOCK_63 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_63;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_63, props);
    }

    // Block 64 (ID: 64)
    {
        BlockProperties props;
        props.name = "Block 64";
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
        if (VoxelType::BLOCK_64 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_64;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_64, props);
    }

    // Block 65 (ID: 65)
    {
        BlockProperties props;
        props.name = "Block 65";
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
        if (VoxelType::BLOCK_65 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_65;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_65, props);
    }

    // Block 66 (ID: 66)
    {
        BlockProperties props;
        props.name = "Block 66";
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
        if (VoxelType::BLOCK_66 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_66;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_66, props);
    }

    // Block 67 (ID: 67)
    {
        BlockProperties props;
        props.name = "Block 67";
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
        if (VoxelType::BLOCK_67 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_67;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_67, props);
    }

    // Block 68 (ID: 68)
    {
        BlockProperties props;
        props.name = "Block 68";
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
        if (VoxelType::BLOCK_68 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_68;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_68, props);
    }

    // Block 69 (ID: 69)
    {
        BlockProperties props;
        props.name = "Block 69";
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
        if (VoxelType::BLOCK_69 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_69;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_69, props);
    }

    // Block 70 (ID: 70)
    {
        BlockProperties props;
        props.name = "Block 70";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_70 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_70;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_70, props);
    }

    // Block 71 (ID: 71)
    {
        BlockProperties props;
        props.name = "Block 71";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_71 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_71;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_71, props);
    }

    // Block 72 (ID: 72)
    {
        BlockProperties props;
        props.name = "Block 72";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_72 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_72;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_72, props);
    }

    // Block 73 (ID: 73)
    {
        BlockProperties props;
        props.name = "Block 73";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_73 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_73;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_73, props);
    }

    // Block 74 (ID: 74)
    {
        BlockProperties props;
        props.name = "Block 74";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_74 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_74;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_74, props);
    }

    // Block 75 (ID: 75)
    {
        BlockProperties props;
        props.name = "Block 75";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_75 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_75;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_75, props);
    }

    // Block 76 (ID: 76)
    {
        BlockProperties props;
        props.name = "Block 76";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_76 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_76;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_76, props);
    }

    // Block 77 (ID: 77)
    {
        BlockProperties props;
        props.name = "Block 77";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_77 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_77;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_77, props);
    }

    // Block 78 (ID: 78)
    {
        BlockProperties props;
        props.name = "Block 78";
        props.is_solid = true;
        props.is_transparent = false;
        props.is_fluid = false;
        props.is_walkable = true;
        props.hardness = 1.0f;
        props.blast_resistance = 3.0f;
        props.required_tool = BlockProperties::ToolType::AXE;
        props.required_tool_level = 1;
        props.is_flammable = false;
        props.fire_spread_chance = 0.0f;
        props.emits_light = false;
        props.light_level = 0;
        props.friction = 0.6f;
        
        // Default drops - set to the block itself for most blocks
        if (VoxelType::BLOCK_78 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_78;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_78, props);
    }

    // Block 79 (ID: 79)
    {
        BlockProperties props;
        props.name = "Block 79";
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
        if (VoxelType::BLOCK_79 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_79;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_79, props);
    }

    // Block 80 (ID: 80)
    {
        BlockProperties props;
        props.name = "Block 80";
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
        if (VoxelType::BLOCK_80 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_80;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_80, props);
    }

    // Block 81 (ID: 81)
    {
        BlockProperties props;
        props.name = "Block 81";
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
        if (VoxelType::BLOCK_81 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_81;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_81, props);
    }

    // Block 82 (ID: 82)
    {
        BlockProperties props;
        props.name = "Block 82";
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
        if (VoxelType::BLOCK_82 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_82;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_82, props);
    }

    // Block 83 (ID: 83)
    {
        BlockProperties props;
        props.name = "Block 83";
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
        if (VoxelType::BLOCK_83 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_83;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_83, props);
    }

    // Block 84 (ID: 84)
    {
        BlockProperties props;
        props.name = "Block 84";
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
        if (VoxelType::BLOCK_84 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_84;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_84, props);
    }

    // Block 85 (ID: 85)
    {
        BlockProperties props;
        props.name = "Block 85";
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
        if (VoxelType::BLOCK_85 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_85;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_85, props);
    }

    // Block 86 (ID: 86)
    {
        BlockProperties props;
        props.name = "Block 86";
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
        if (VoxelType::BLOCK_86 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_86;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_86, props);
    }

    // Block 87 (ID: 87)
    {
        BlockProperties props;
        props.name = "Block 87";
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
        if (VoxelType::BLOCK_87 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_87;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_87, props);
    }

    // Block 88 (ID: 88)
    {
        BlockProperties props;
        props.name = "Block 88";
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
        if (VoxelType::BLOCK_88 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_88;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_88, props);
    }

    // Block 89 (ID: 89)
    {
        BlockProperties props;
        props.name = "Block 89";
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
        if (VoxelType::BLOCK_89 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_89;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_89, props);
    }

    // Block 90 (ID: 90)
    {
        BlockProperties props;
        props.name = "Block 90";
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
        if (VoxelType::BLOCK_90 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_90;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_90, props);
    }

    // Block 91 (ID: 91)
    {
        BlockProperties props;
        props.name = "Block 91";
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
        if (VoxelType::BLOCK_91 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_91;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_91, props);
    }

    // Block 92 (ID: 92)
    {
        BlockProperties props;
        props.name = "Block 92";
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
        if (VoxelType::BLOCK_92 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_92;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_92, props);
    }

    // Block 93 (ID: 93)
    {
        BlockProperties props;
        props.name = "Block 93";
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
        if (VoxelType::BLOCK_93 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_93;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_93, props);
    }

    // Block 94 (ID: 94)
    {
        BlockProperties props;
        props.name = "Block 94";
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
        if (VoxelType::BLOCK_94 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_94;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_94, props);
    }

    // Block 95 (ID: 95)
    {
        BlockProperties props;
        props.name = "Block 95";
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
        if (VoxelType::BLOCK_95 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_95;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_95, props);
    }

    // Block 96 (ID: 96)
    {
        BlockProperties props;
        props.name = "Block 96";
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
        if (VoxelType::BLOCK_96 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_96;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_96, props);
    }

    // Block 97 (ID: 97)
    {
        BlockProperties props;
        props.name = "Block 97";
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
        if (VoxelType::BLOCK_97 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_97;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_97, props);
    }

    // Block 98 (ID: 98)
    {
        BlockProperties props;
        props.name = "Block 98";
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
        if (VoxelType::BLOCK_98 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_98;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_98, props);
    }

    // Block 99 (ID: 99)
    {
        BlockProperties props;
        props.name = "Block 99";
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
        if (VoxelType::BLOCK_99 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_99;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_99, props);
    }

    // Block 100 (ID: 100)
    {
        BlockProperties props;
        props.name = "Block 100";
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
        if (VoxelType::BLOCK_100 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_100;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_100, props);
    }

    // Block 101 (ID: 101)
    {
        BlockProperties props;
        props.name = "Block 101";
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
        if (VoxelType::BLOCK_101 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_101;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_101, props);
    }

    // Block 102 (ID: 102)
    {
        BlockProperties props;
        props.name = "Block 102";
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
        if (VoxelType::BLOCK_102 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_102;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_102, props);
    }

    // Block 103 (ID: 103)
    {
        BlockProperties props;
        props.name = "Block 103";
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
        if (VoxelType::BLOCK_103 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_103;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_103, props);
    }

    // Block 104 (ID: 104)
    {
        BlockProperties props;
        props.name = "Block 104";
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
        if (VoxelType::BLOCK_104 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_104;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_104, props);
    }

    // Block 105 (ID: 105)
    {
        BlockProperties props;
        props.name = "Block 105";
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
        if (VoxelType::BLOCK_105 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_105;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_105, props);
    }

    // Block 106 (ID: 106)
    {
        BlockProperties props;
        props.name = "Block 106";
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
        if (VoxelType::BLOCK_106 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_106;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_106, props);
    }

    // Block 107 (ID: 107)
    {
        BlockProperties props;
        props.name = "Block 107";
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
        if (VoxelType::BLOCK_107 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_107;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_107, props);
    }

    // Block 108 (ID: 108)
    {
        BlockProperties props;
        props.name = "Block 108";
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
        if (VoxelType::BLOCK_108 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_108;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_108, props);
    }

    // Block 109 (ID: 109)
    {
        BlockProperties props;
        props.name = "Block 109";
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
        if (VoxelType::BLOCK_109 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_109;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_109, props);
    }

    // Block 110 (ID: 110)
    {
        BlockProperties props;
        props.name = "Block 110";
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
        if (VoxelType::BLOCK_110 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_110;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_110, props);
    }

    // Block 111 (ID: 111)
    {
        BlockProperties props;
        props.name = "Block 111";
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
        if (VoxelType::BLOCK_111 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_111;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_111, props);
    }

    // Block 112 (ID: 112)
    {
        BlockProperties props;
        props.name = "Block 112";
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
        if (VoxelType::BLOCK_112 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_112;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_112, props);
    }

    // Block 113 (ID: 113)
    {
        BlockProperties props;
        props.name = "Block 113";
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
        if (VoxelType::BLOCK_113 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_113;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_113, props);
    }

    // Block 114 (ID: 114)
    {
        BlockProperties props;
        props.name = "Block 114";
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
        if (VoxelType::BLOCK_114 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_114;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_114, props);
    }

    // Block 115 (ID: 115)
    {
        BlockProperties props;
        props.name = "Block 115";
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
        if (VoxelType::BLOCK_115 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_115;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_115, props);
    }

    // Block 116 (ID: 116)
    {
        BlockProperties props;
        props.name = "Block 116";
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
        if (VoxelType::BLOCK_116 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_116;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_116, props);
    }

    // Block 117 (ID: 117)
    {
        BlockProperties props;
        props.name = "Block 117";
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
        if (VoxelType::BLOCK_117 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_117;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_117, props);
    }

    // Block 118 (ID: 118)
    {
        BlockProperties props;
        props.name = "Block 118";
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
        if (VoxelType::BLOCK_118 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_118;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_118, props);
    }

    // Block 119 (ID: 119)
    {
        BlockProperties props;
        props.name = "Block 119";
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
        if (VoxelType::BLOCK_119 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_119;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_119, props);
    }

    // Block 120 (ID: 120)
    {
        BlockProperties props;
        props.name = "Block 120";
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
        if (VoxelType::BLOCK_120 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_120;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_120, props);
    }

    // Block 121 (ID: 121)
    {
        BlockProperties props;
        props.name = "Block 121";
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
        if (VoxelType::BLOCK_121 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_121;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_121, props);
    }

    // Block 122 (ID: 122)
    {
        BlockProperties props;
        props.name = "Block 122";
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
        if (VoxelType::BLOCK_122 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_122;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_122, props);
    }

    // Block 123 (ID: 123)
    {
        BlockProperties props;
        props.name = "Block 123";
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
        if (VoxelType::BLOCK_123 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_123;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_123, props);
    }

    // Block 124 (ID: 124)
    {
        BlockProperties props;
        props.name = "Block 124";
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
        if (VoxelType::BLOCK_124 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_124;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_124, props);
    }

    // Block 125 (ID: 125)
    {
        BlockProperties props;
        props.name = "Block 125";
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
        if (VoxelType::BLOCK_125 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_125;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_125, props);
    }

    // Block 126 (ID: 126)
    {
        BlockProperties props;
        props.name = "Block 126";
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
        if (VoxelType::BLOCK_126 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_126;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_126, props);
    }

    // Block 127 (ID: 127)
    {
        BlockProperties props;
        props.name = "Block 127";
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
        if (VoxelType::BLOCK_127 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_127;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_127, props);
    }

    // Block 128 (ID: 128)
    {
        BlockProperties props;
        props.name = "Block 128";
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
        if (VoxelType::BLOCK_128 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_128;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_128, props);
    }

    // Block 129 (ID: 129)
    {
        BlockProperties props;
        props.name = "Block 129";
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
        if (VoxelType::BLOCK_129 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_129;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_129, props);
    }

    // Block 130 (ID: 130)
    {
        BlockProperties props;
        props.name = "Block 130";
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
        if (VoxelType::BLOCK_130 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_130;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_130, props);
    }

    // Block 131 (ID: 131)
    {
        BlockProperties props;
        props.name = "Block 131";
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
        if (VoxelType::BLOCK_131 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_131;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_131, props);
    }

    // Block 132 (ID: 132)
    {
        BlockProperties props;
        props.name = "Block 132";
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
        if (VoxelType::BLOCK_132 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_132;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_132, props);
    }

    // Block 133 (ID: 133)
    {
        BlockProperties props;
        props.name = "Block 133";
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
        if (VoxelType::BLOCK_133 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_133;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_133, props);
    }

    // Block 134 (ID: 134)
    {
        BlockProperties props;
        props.name = "Block 134";
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
        if (VoxelType::BLOCK_134 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_134;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_134, props);
    }

    // Block 135 (ID: 135)
    {
        BlockProperties props;
        props.name = "Block 135";
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
        if (VoxelType::BLOCK_135 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_135;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_135, props);
    }

    // Block 136 (ID: 136)
    {
        BlockProperties props;
        props.name = "Block 136";
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
        if (VoxelType::BLOCK_136 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_136;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_136, props);
    }

    // Block 137 (ID: 137)
    {
        BlockProperties props;
        props.name = "Block 137";
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
        if (VoxelType::BLOCK_137 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_137;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_137, props);
    }

    // Block 138 (ID: 138)
    {
        BlockProperties props;
        props.name = "Block 138";
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
        if (VoxelType::BLOCK_138 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_138;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_138, props);
    }

    // Block 139 (ID: 139)
    {
        BlockProperties props;
        props.name = "Block 139";
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
        if (VoxelType::BLOCK_139 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_139;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_139, props);
    }

    // Block 140 (ID: 140)
    {
        BlockProperties props;
        props.name = "Block 140";
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
        if (VoxelType::BLOCK_140 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_140;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_140, props);
    }

    // Block 141 (ID: 141)
    {
        BlockProperties props;
        props.name = "Block 141";
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
        if (VoxelType::BLOCK_141 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_141;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_141, props);
    }

    // Block 142 (ID: 142)
    {
        BlockProperties props;
        props.name = "Block 142";
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
        if (VoxelType::BLOCK_142 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_142;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_142, props);
    }

    // Block 143 (ID: 143)
    {
        BlockProperties props;
        props.name = "Block 143";
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
        if (VoxelType::BLOCK_143 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_143;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_143, props);
    }

    // Block 144 (ID: 144)
    {
        BlockProperties props;
        props.name = "Block 144";
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
        if (VoxelType::BLOCK_144 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_144;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_144, props);
    }

    // Block 145 (ID: 145)
    {
        BlockProperties props;
        props.name = "Block 145";
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
        if (VoxelType::BLOCK_145 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_145;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_145, props);
    }

    // Block 146 (ID: 146)
    {
        BlockProperties props;
        props.name = "Block 146";
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
        if (VoxelType::BLOCK_146 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_146;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_146, props);
    }

    // Block 147 (ID: 147)
    {
        BlockProperties props;
        props.name = "Block 147";
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
        if (VoxelType::BLOCK_147 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_147;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_147, props);
    }

    // Block 148 (ID: 148)
    {
        BlockProperties props;
        props.name = "Block 148";
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
        if (VoxelType::BLOCK_148 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_148;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_148, props);
    }

    // Block 149 (ID: 149)
    {
        BlockProperties props;
        props.name = "Block 149";
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
        if (VoxelType::BLOCK_149 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_149;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_149, props);
    }

    // Block 150 (ID: 150)
    {
        BlockProperties props;
        props.name = "Block 150";
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
        if (VoxelType::BLOCK_150 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_150;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_150, props);
    }

    // Block 151 (ID: 151)
    {
        BlockProperties props;
        props.name = "Block 151";
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
        if (VoxelType::BLOCK_151 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_151;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_151, props);
    }

    // Block 152 (ID: 152)
    {
        BlockProperties props;
        props.name = "Block 152";
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
        if (VoxelType::BLOCK_152 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_152;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_152, props);
    }

    // Block 153 (ID: 153)
    {
        BlockProperties props;
        props.name = "Block 153";
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
        if (VoxelType::BLOCK_153 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_153;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_153, props);
    }

    // Block 154 (ID: 154)
    {
        BlockProperties props;
        props.name = "Block 154";
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
        if (VoxelType::BLOCK_154 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_154;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_154, props);
    }

    // Block 155 (ID: 155)
    {
        BlockProperties props;
        props.name = "Block 155";
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
        if (VoxelType::BLOCK_155 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_155;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_155, props);
    }

    // Block 156 (ID: 156)
    {
        BlockProperties props;
        props.name = "Block 156";
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
        if (VoxelType::BLOCK_156 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_156;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_156, props);
    }

    // Block 157 (ID: 157)
    {
        BlockProperties props;
        props.name = "Block 157";
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
        if (VoxelType::BLOCK_157 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_157;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_157, props);
    }

    // Block 158 (ID: 158)
    {
        BlockProperties props;
        props.name = "Block 158";
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
        if (VoxelType::BLOCK_158 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_158;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_158, props);
    }

    // Block 159 (ID: 159)
    {
        BlockProperties props;
        props.name = "Block 159";
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
        if (VoxelType::BLOCK_159 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_159;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_159, props);
    }

    // Block 160 (ID: 160)
    {
        BlockProperties props;
        props.name = "Block 160";
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
        if (VoxelType::BLOCK_160 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_160;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_160, props);
    }

    // Block 161 (ID: 161)
    {
        BlockProperties props;
        props.name = "Block 161";
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
        if (VoxelType::BLOCK_161 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_161;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_161, props);
    }

    // Block 162 (ID: 162)
    {
        BlockProperties props;
        props.name = "Block 162";
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
        if (VoxelType::BLOCK_162 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_162;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_162, props);
    }

    // Block 163 (ID: 163)
    {
        BlockProperties props;
        props.name = "Block 163";
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
        if (VoxelType::BLOCK_163 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_163;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_163, props);
    }

    // Block 164 (ID: 164)
    {
        BlockProperties props;
        props.name = "Block 164";
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
        if (VoxelType::BLOCK_164 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_164;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_164, props);
    }

    // Block 165 (ID: 165)
    {
        BlockProperties props;
        props.name = "Block 165";
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
        if (VoxelType::BLOCK_165 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_165;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_165, props);
    }

    // Block 166 (ID: 166)
    {
        BlockProperties props;
        props.name = "Block 166";
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
        if (VoxelType::BLOCK_166 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_166;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_166, props);
    }

    // Block 167 (ID: 167)
    {
        BlockProperties props;
        props.name = "Block 167";
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
        if (VoxelType::BLOCK_167 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_167;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_167, props);
    }

    // Block 168 (ID: 168)
    {
        BlockProperties props;
        props.name = "Block 168";
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
        if (VoxelType::BLOCK_168 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_168;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_168, props);
    }

    // Block 169 (ID: 169)
    {
        BlockProperties props;
        props.name = "Block 169";
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
        if (VoxelType::BLOCK_169 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_169;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_169, props);
    }

    // Block 170 (ID: 170)
    {
        BlockProperties props;
        props.name = "Block 170";
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
        if (VoxelType::BLOCK_170 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_170;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_170, props);
    }

    // Block 171 (ID: 171)
    {
        BlockProperties props;
        props.name = "Block 171";
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
        if (VoxelType::BLOCK_171 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_171;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_171, props);
    }

    // Block 172 (ID: 172)
    {
        BlockProperties props;
        props.name = "Block 172";
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
        if (VoxelType::BLOCK_172 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_172;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_172, props);
    }

    // Block 173 (ID: 173)
    {
        BlockProperties props;
        props.name = "Block 173";
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
        if (VoxelType::BLOCK_173 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_173;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_173, props);
    }

    // Block 174 (ID: 174)
    {
        BlockProperties props;
        props.name = "Block 174";
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
        if (VoxelType::BLOCK_174 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_174;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_174, props);
    }

    // Block 175 (ID: 175)
    {
        BlockProperties props;
        props.name = "Block 175";
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
        if (VoxelType::BLOCK_175 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_175;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_175, props);
    }

    // Block 176 (ID: 176)
    {
        BlockProperties props;
        props.name = "Block 176";
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
        if (VoxelType::BLOCK_176 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_176;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_176, props);
    }

    // Block 177 (ID: 177)
    {
        BlockProperties props;
        props.name = "Block 177";
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
        if (VoxelType::BLOCK_177 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_177;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_177, props);
    }

    // Block 178 (ID: 178)
    {
        BlockProperties props;
        props.name = "Block 178";
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
        if (VoxelType::BLOCK_178 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_178;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_178, props);
    }

    // Block 179 (ID: 179)
    {
        BlockProperties props;
        props.name = "Block 179";
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
        if (VoxelType::BLOCK_179 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_179;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_179, props);
    }

    // Block 180 (ID: 180)
    {
        BlockProperties props;
        props.name = "Block 180";
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
        if (VoxelType::BLOCK_180 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_180;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_180, props);
    }

    // Block 181 (ID: 181)
    {
        BlockProperties props;
        props.name = "Block 181";
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
        if (VoxelType::BLOCK_181 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_181;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_181, props);
    }

    // Block 182 (ID: 182)
    {
        BlockProperties props;
        props.name = "Block 182";
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
        if (VoxelType::BLOCK_182 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_182;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_182, props);
    }

    // Block 183 (ID: 183)
    {
        BlockProperties props;
        props.name = "Block 183";
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
        if (VoxelType::BLOCK_183 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_183;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_183, props);
    }

    // Block 184 (ID: 184)
    {
        BlockProperties props;
        props.name = "Block 184";
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
        if (VoxelType::BLOCK_184 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_184;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_184, props);
    }

    // Block 185 (ID: 185)
    {
        BlockProperties props;
        props.name = "Block 185";
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
        if (VoxelType::BLOCK_185 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_185;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_185, props);
    }

    // Block 186 (ID: 186)
    {
        BlockProperties props;
        props.name = "Block 186";
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
        if (VoxelType::BLOCK_186 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_186;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_186, props);
    }

    // Block 187 (ID: 187)
    {
        BlockProperties props;
        props.name = "Block 187";
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
        if (VoxelType::BLOCK_187 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_187;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_187, props);
    }

    // Block 188 (ID: 188)
    {
        BlockProperties props;
        props.name = "Block 188";
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
        if (VoxelType::BLOCK_188 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_188;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_188, props);
    }

    // Block 189 (ID: 189)
    {
        BlockProperties props;
        props.name = "Block 189";
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
        if (VoxelType::BLOCK_189 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_189;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_189, props);
    }

    // Block 190 (ID: 190)
    {
        BlockProperties props;
        props.name = "Block 190";
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
        if (VoxelType::BLOCK_190 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_190;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_190, props);
    }

    // Block 191 (ID: 191)
    {
        BlockProperties props;
        props.name = "Block 191";
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
        if (VoxelType::BLOCK_191 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_191;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_191, props);
    }

    // Block 192 (ID: 192)
    {
        BlockProperties props;
        props.name = "Block 192";
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
        if (VoxelType::BLOCK_192 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_192;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_192, props);
    }

    // Block 193 (ID: 193)
    {
        BlockProperties props;
        props.name = "Block 193";
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
        if (VoxelType::BLOCK_193 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_193;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_193, props);
    }

    // Block 194 (ID: 194)
    {
        BlockProperties props;
        props.name = "Block 194";
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
        if (VoxelType::BLOCK_194 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_194;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_194, props);
    }

    // Block 195 (ID: 195)
    {
        BlockProperties props;
        props.name = "Block 195";
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
        if (VoxelType::BLOCK_195 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_195;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_195, props);
    }

    // Block 196 (ID: 196)
    {
        BlockProperties props;
        props.name = "Block 196";
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
        if (VoxelType::BLOCK_196 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_196;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_196, props);
    }

    // Block 197 (ID: 197)
    {
        BlockProperties props;
        props.name = "Block 197";
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
        if (VoxelType::BLOCK_197 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_197;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_197, props);
    }

    // Block 198 (ID: 198)
    {
        BlockProperties props;
        props.name = "Block 198";
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
        if (VoxelType::BLOCK_198 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_198;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_198, props);
    }

    // Block 199 (ID: 199)
    {
        BlockProperties props;
        props.name = "Block 199";
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
        if (VoxelType::BLOCK_199 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_199;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_199, props);
    }

    // Block 200 (ID: 200)
    {
        BlockProperties props;
        props.name = "Block 200";
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
        if (VoxelType::BLOCK_200 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_200;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_200, props);
    }

    // Block 201 (ID: 201)
    {
        BlockProperties props;
        props.name = "Block 201";
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
        if (VoxelType::BLOCK_201 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_201;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_201, props);
    }

    // Block 202 (ID: 202)
    {
        BlockProperties props;
        props.name = "Block 202";
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
        if (VoxelType::BLOCK_202 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_202;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_202, props);
    }

    // Block 203 (ID: 203)
    {
        BlockProperties props;
        props.name = "Block 203";
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
        if (VoxelType::BLOCK_203 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_203;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_203, props);
    }

    // Block 204 (ID: 204)
    {
        BlockProperties props;
        props.name = "Block 204";
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
        if (VoxelType::BLOCK_204 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_204;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_204, props);
    }

    // Block 205 (ID: 205)
    {
        BlockProperties props;
        props.name = "Block 205";
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
        if (VoxelType::BLOCK_205 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_205;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_205, props);
    }

    // Block 206 (ID: 206)
    {
        BlockProperties props;
        props.name = "Block 206";
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
        if (VoxelType::BLOCK_206 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_206;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_206, props);
    }

    // Block 207 (ID: 207)
    {
        BlockProperties props;
        props.name = "Block 207";
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
        if (VoxelType::BLOCK_207 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_207;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_207, props);
    }

    // Block 208 (ID: 208)
    {
        BlockProperties props;
        props.name = "Block 208";
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
        if (VoxelType::BLOCK_208 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_208;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_208, props);
    }

    // Block 209 (ID: 209)
    {
        BlockProperties props;
        props.name = "Block 209";
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
        if (VoxelType::BLOCK_209 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_209;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_209, props);
    }

    // Block 210 (ID: 210)
    {
        BlockProperties props;
        props.name = "Block 210";
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
        if (VoxelType::BLOCK_210 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_210;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_210, props);
    }

    // Block 211 (ID: 211)
    {
        BlockProperties props;
        props.name = "Block 211";
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
        if (VoxelType::BLOCK_211 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_211;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_211, props);
    }

    // Block 212 (ID: 212)
    {
        BlockProperties props;
        props.name = "Block 212";
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
        if (VoxelType::BLOCK_212 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_212;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_212, props);
    }

    // Block 213 (ID: 213)
    {
        BlockProperties props;
        props.name = "Block 213";
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
        if (VoxelType::BLOCK_213 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_213;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_213, props);
    }

    // Block 214 (ID: 214)
    {
        BlockProperties props;
        props.name = "Block 214";
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
        if (VoxelType::BLOCK_214 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_214;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_214, props);
    }

    // Block 215 (ID: 215)
    {
        BlockProperties props;
        props.name = "Block 215";
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
        if (VoxelType::BLOCK_215 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_215;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_215, props);
    }

    // Block 216 (ID: 216)
    {
        BlockProperties props;
        props.name = "Block 216";
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
        if (VoxelType::BLOCK_216 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_216;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_216, props);
    }

    // Block 217 (ID: 217)
    {
        BlockProperties props;
        props.name = "Block 217";
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
        if (VoxelType::BLOCK_217 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_217;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_217, props);
    }

    // Block 218 (ID: 218)
    {
        BlockProperties props;
        props.name = "Block 218";
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
        if (VoxelType::BLOCK_218 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_218;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_218, props);
    }

    // Block 219 (ID: 219)
    {
        BlockProperties props;
        props.name = "Block 219";
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
        if (VoxelType::BLOCK_219 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_219;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_219, props);
    }

    // Block 220 (ID: 220)
    {
        BlockProperties props;
        props.name = "Block 220";
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
        if (VoxelType::BLOCK_220 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_220;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_220, props);
    }

    // Block 221 (ID: 221)
    {
        BlockProperties props;
        props.name = "Block 221";
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
        if (VoxelType::BLOCK_221 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_221;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_221, props);
    }

    // Block 222 (ID: 222)
    {
        BlockProperties props;
        props.name = "Block 222";
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
        if (VoxelType::BLOCK_222 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_222;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_222, props);
    }

    // Block 223 (ID: 223)
    {
        BlockProperties props;
        props.name = "Block 223";
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
        if (VoxelType::BLOCK_223 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_223;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_223, props);
    }

    // Block 224 (ID: 224)
    {
        BlockProperties props;
        props.name = "Block 224";
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
        if (VoxelType::BLOCK_224 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_224;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_224, props);
    }

    // Block 225 (ID: 225)
    {
        BlockProperties props;
        props.name = "Block 225";
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
        if (VoxelType::BLOCK_225 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_225;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_225, props);
    }

    // Block 226 (ID: 226)
    {
        BlockProperties props;
        props.name = "Block 226";
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
        if (VoxelType::BLOCK_226 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_226;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_226, props);
    }

    // Block 227 (ID: 227)
    {
        BlockProperties props;
        props.name = "Block 227";
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
        if (VoxelType::BLOCK_227 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_227;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_227, props);
    }

    // Block 228 (ID: 228)
    {
        BlockProperties props;
        props.name = "Block 228";
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
        if (VoxelType::BLOCK_228 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_228;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_228, props);
    }

    // Block 229 (ID: 229)
    {
        BlockProperties props;
        props.name = "Block 229";
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
        if (VoxelType::BLOCK_229 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_229;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_229, props);
    }

    // Block 230 (ID: 230)
    {
        BlockProperties props;
        props.name = "Block 230";
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
        if (VoxelType::BLOCK_230 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_230;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_230, props);
    }

    // Block 231 (ID: 231)
    {
        BlockProperties props;
        props.name = "Block 231";
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
        if (VoxelType::BLOCK_231 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_231;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_231, props);
    }

    // Block 232 (ID: 232)
    {
        BlockProperties props;
        props.name = "Block 232";
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
        if (VoxelType::BLOCK_232 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_232;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_232, props);
    }

    // Block 233 (ID: 233)
    {
        BlockProperties props;
        props.name = "Block 233";
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
        if (VoxelType::BLOCK_233 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_233;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_233, props);
    }

    // Block 234 (ID: 234)
    {
        BlockProperties props;
        props.name = "Block 234";
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
        if (VoxelType::BLOCK_234 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_234;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_234, props);
    }

    // Block 235 (ID: 235)
    {
        BlockProperties props;
        props.name = "Block 235";
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
        if (VoxelType::BLOCK_235 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_235;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_235, props);
    }

    // Block 236 (ID: 236)
    {
        BlockProperties props;
        props.name = "Block 236";
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
        if (VoxelType::BLOCK_236 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_236;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_236, props);
    }

    // Block 237 (ID: 237)
    {
        BlockProperties props;
        props.name = "Block 237";
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
        if (VoxelType::BLOCK_237 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_237;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_237, props);
    }

    // Block 238 (ID: 238)
    {
        BlockProperties props;
        props.name = "Block 238";
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
        if (VoxelType::BLOCK_238 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_238;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_238, props);
    }

    // Block 239 (ID: 239)
    {
        BlockProperties props;
        props.name = "Block 239";
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
        if (VoxelType::BLOCK_239 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_239;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_239, props);
    }

    // Block 240 (ID: 240)
    {
        BlockProperties props;
        props.name = "Block 240";
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
        if (VoxelType::BLOCK_240 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_240;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_240, props);
    }

    // Block 241 (ID: 241)
    {
        BlockProperties props;
        props.name = "Block 241";
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
        if (VoxelType::BLOCK_241 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_241;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_241, props);
    }

    // Block 242 (ID: 242)
    {
        BlockProperties props;
        props.name = "Block 242";
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
        if (VoxelType::BLOCK_242 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_242;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_242, props);
    }

    // Block 243 (ID: 243)
    {
        BlockProperties props;
        props.name = "Block 243";
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
        if (VoxelType::BLOCK_243 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_243;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_243, props);
    }

    // Block 244 (ID: 244)
    {
        BlockProperties props;
        props.name = "Block 244";
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
        if (VoxelType::BLOCK_244 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_244;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_244, props);
    }

    // Block 245 (ID: 245)
    {
        BlockProperties props;
        props.name = "Block 245";
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
        if (VoxelType::BLOCK_245 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_245;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_245, props);
    }

    // Block 246 (ID: 246)
    {
        BlockProperties props;
        props.name = "Block 246";
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
        if (VoxelType::BLOCK_246 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_246;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_246, props);
    }

    // Block 247 (ID: 247)
    {
        BlockProperties props;
        props.name = "Block 247";
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
        if (VoxelType::BLOCK_247 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_247;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_247, props);
    }

    // Block 248 (ID: 248)
    {
        BlockProperties props;
        props.name = "Block 248";
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
        if (VoxelType::BLOCK_248 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_248;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_248, props);
    }

    // Block 249 (ID: 249)
    {
        BlockProperties props;
        props.name = "Block 249";
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
        if (VoxelType::BLOCK_249 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_249;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_249, props);
    }

    // Block 250 (ID: 250)
    {
        BlockProperties props;
        props.name = "Block 250";
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
        if (VoxelType::BLOCK_250 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_250;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_250, props);
    }

    // Block 251 (ID: 251)
    {
        BlockProperties props;
        props.name = "Block 251";
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
        if (VoxelType::BLOCK_251 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_251;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_251, props);
    }

    // Block 252 (ID: 252)
    {
        BlockProperties props;
        props.name = "Block 252";
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
        if (VoxelType::BLOCK_252 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_252;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_252, props);
    }

    // Block 253 (ID: 253)
    {
        BlockProperties props;
        props.name = "Block 253";
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
        if (VoxelType::BLOCK_253 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_253;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_253, props);
    }

    // Block 254 (ID: 254)
    {
        BlockProperties props;
        props.name = "Block 254";
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
        if (VoxelType::BLOCK_254 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_254;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_254, props);
    }

    // Block 255 (ID: 255)
    {
        BlockProperties props;
        props.name = "Block 255";
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
        if (VoxelType::BLOCK_255 != VoxelType::AIR) {
            props.possible_drops[0] = VoxelType::BLOCK_255;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(VoxelType::BLOCK_255, props);
    }

}}

} // namespace World
} // namespace VoxelEngine
