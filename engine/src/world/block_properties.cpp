#include "world/block_properties.h"
#include <iostream>

namespace VoxelEngine {
namespace World {

BlockPropertiesManager& BlockPropertiesManager::getInstance() {
    static BlockPropertiesManager instance;
    return instance;
}

BlockPropertiesManager::BlockPropertiesManager() {
    initializeDefaultProperties();
}

const BlockProperties& BlockPropertiesManager::getProperties(VoxelType type) const {
    auto it = m_properties.find(type);
    if (it != m_properties.end()) {
        return it->second;
    }
    return m_default_properties;
}

bool BlockPropertiesManager::isRegistered(VoxelType type) const {
    return m_properties.find(type) != m_properties.end();
}

// Convenience methods
bool BlockPropertiesManager::isSolid(VoxelType type) const {
    return getProperties(type).is_solid;
}

bool BlockPropertiesManager::isTransparent(VoxelType type) const {
    return getProperties(type).is_transparent;
}

bool BlockPropertiesManager::isFluid(VoxelType type) const {
    return getProperties(type).is_fluid;
}

bool BlockPropertiesManager::isWalkable(VoxelType type) const {
    return getProperties(type).is_walkable;
}

bool BlockPropertiesManager::emitsLight(VoxelType type) const {
    return getProperties(type).emits_light;
}

uint8_t BlockPropertiesManager::getLightLevel(VoxelType type) const {
    return getProperties(type).light_level;
}

float BlockPropertiesManager::getHardness(VoxelType type) const {
    return getProperties(type).hardness;
}

bool BlockPropertiesManager::isFlammable(VoxelType type) const {
    return getProperties(type).is_flammable;
}

bool BlockPropertiesManager::requiresTool(VoxelType type) const {
    return getProperties(type).required_tool != BlockProperties::ToolType::NONE;
}

BlockProperties::ToolType BlockPropertiesManager::getRequiredTool(VoxelType type) const {
    return getProperties(type).required_tool;
}

bool BlockPropertiesManager::canHarvestWith(VoxelType block, BlockProperties::ToolType tool, uint8_t tool_level) const {
    const auto& props = getProperties(block);
    if (props.required_tool == BlockProperties::ToolType::NONE) {
        return true; // No tool required
    }
    return tool == props.required_tool && tool_level >= props.required_tool_level;
}

bool BlockPropertiesManager::isWeathering(VoxelType type) const {
    return getProperties(type).weathering_enabled;
}

VoxelType BlockPropertiesManager::getWeatheredForm(VoxelType type) const {
    return getProperties(type).weathered_form;
}

void BlockPropertiesManager::registerBlockProperties(VoxelType type, const BlockProperties& properties) {
    m_properties[type] = properties;
}

void BlockPropertiesManager::initializeDefaultProperties() {
    // Helper lambda to create properties more easily
    auto createProps = [this](VoxelType type, const std::string& name, bool solid, bool transparent, 
                             bool fluid, float hardness, BlockProperties::ToolType tool = BlockProperties::ToolType::NONE,
                             uint8_t tool_level = 0, bool flammable = false, uint8_t light = 0) {
        BlockProperties props;
        props.name = name;
        props.is_solid = solid;
        props.is_transparent = transparent;
        props.is_fluid = fluid;
        props.is_walkable = solid && !fluid;
        props.hardness = hardness;
        props.blast_resistance = hardness * 3.0f; // Default blast resistance
        props.required_tool = tool;
        props.required_tool_level = tool_level;
        props.is_flammable = flammable;
        props.emits_light = light > 0;
        props.light_level = light;
        props.friction = solid ? 0.6f : 0.1f; // Solid blocks have more friction
        
        // Set default drops to the block itself (most common case)
        if (type != VoxelType::AIR) {
            props.possible_drops[0] = type;
            props.drop_chances[0] = 255; // 100% chance
            props.drop_count_min = 1;
            props.drop_count_max = 1;
        }
        
        registerBlockProperties(type, props);
    };

    // ========== CATEGORY 1: TERRAIN & NATURAL (0-49) ==========
    
    // Basic Terrain (0-9)
    createProps(VoxelType::AIR, "Air", false, true, false, 0.0f);
    createProps(VoxelType::STONE, "Stone", true, false, false, 1.5f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::DIRT, "Dirt", true, false, false, 0.5f, BlockProperties::ToolType::SHOVEL, 0);
    createProps(VoxelType::GRASS, "Grass Block", true, false, false, 0.6f, BlockProperties::ToolType::SHOVEL, 0);
    createProps(VoxelType::SAND, "Sand", true, false, false, 0.5f, BlockProperties::ToolType::SHOVEL, 0);
    createProps(VoxelType::GRAVEL, "Gravel", true, false, false, 0.6f, BlockProperties::ToolType::SHOVEL, 0);
    createProps(VoxelType::CLAY, "Clay", true, false, false, 0.6f, BlockProperties::ToolType::SHOVEL, 0);
    createProps(VoxelType::BEDROCK, "Bedrock", true, false, false, 0.0f); // Unbreakable
    
    // Set special properties for gravity-affected blocks
    {
        auto& sand_props = m_properties[VoxelType::SAND];
        sand_props.affected_by_gravity = true;
        auto& gravel_props = m_properties[VoxelType::GRAVEL];
        gravel_props.affected_by_gravity = true;
    }

    // Stone Variants (10-19) - Continue reading from voxel_types.h for accurate mapping
    createProps(VoxelType::GRANITE, "Granite", true, false, false, 1.8f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::MARBLE, "Marble", true, false, false, 1.6f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::SANDSTONE, "Sandstone", true, false, false, 1.2f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::LIMESTONE, "Limestone", true, false, false, 1.3f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::SLATE, "Slate", true, false, false, 1.4f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::OBSIDIAN, "Obsidian", true, false, false, 50.0f, BlockProperties::ToolType::PICKAXE, 3);
    createProps(VoxelType::PUMICE, "Pumice", true, false, false, 0.8f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::BASALT, "Basalt", true, false, false, 1.7f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::QUARTZITE, "Quartzite", true, false, false, 1.9f, BlockProperties::ToolType::PICKAXE, 1);
    
    // Ores & Gems (20-29)
    createProps(VoxelType::COAL_ORE, "Coal Ore", true, false, false, 3.0f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::IRON_ORE, "Iron Ore", true, false, false, 3.5f, BlockProperties::ToolType::PICKAXE, 1);
    createProps(VoxelType::COPPER_ORE, "Copper Ore", true, false, false, 3.2f, BlockProperties::ToolType::PICKAXE, 1);
    createProps(VoxelType::TIN_ORE, "Tin Ore", true, false, false, 3.3f, BlockProperties::ToolType::PICKAXE, 1);
    createProps(VoxelType::SILVER_ORE, "Silver Ore", true, false, false, 4.0f, BlockProperties::ToolType::PICKAXE, 2);
    createProps(VoxelType::GOLD_ORE, "Gold Ore", true, false, false, 4.5f, BlockProperties::ToolType::PICKAXE, 2);
    createProps(VoxelType::GEM_RUBY, "Ruby Ore", true, false, false, 5.0f, BlockProperties::ToolType::PICKAXE, 3);
    createProps(VoxelType::GEM_SAPPHIRE, "Sapphire Ore", true, false, false, 5.0f, BlockProperties::ToolType::PICKAXE, 3);
    createProps(VoxelType::GEM_EMERALD, "Emerald Ore", true, false, false, 5.0f, BlockProperties::ToolType::PICKAXE, 3);
    createProps(VoxelType::GEM_DIAMOND, "Diamond Ore", true, false, false, 6.0f, BlockProperties::ToolType::PICKAXE, 3);
    
    // Organic Natural (30-39)
    createProps(VoxelType::WOOD_OAK, "Oak Wood", true, false, false, 2.0f, BlockProperties::ToolType::AXE, 0, true);
    createProps(VoxelType::WOOD_PINE, "Pine Wood", true, false, false, 1.8f, BlockProperties::ToolType::AXE, 0, true);
    createProps(VoxelType::WOOD_BIRCH, "Birch Wood", true, false, false, 1.9f, BlockProperties::ToolType::AXE, 0, true);
    createProps(VoxelType::WOOD_MAHOGANY, "Mahogany Wood", true, false, false, 2.2f, BlockProperties::ToolType::AXE, 0, true);
    createProps(VoxelType::LEAVES_OAK, "Oak Leaves", true, true, false, 0.2f, BlockProperties::ToolType::SHEARS, 0, true);
    createProps(VoxelType::LEAVES_PINE, "Pine Needles", true, true, false, 0.2f, BlockProperties::ToolType::SHEARS, 0, true);
    createProps(VoxelType::LEAVES_BIRCH, "Birch Leaves", true, true, false, 0.2f, BlockProperties::ToolType::SHEARS, 0, true);
    createProps(VoxelType::LEAVES_PALM, "Palm Fronds", true, true, false, 0.2f, BlockProperties::ToolType::SHEARS, 0, true);
    createProps(VoxelType::MUSHROOM_BROWN, "Brown Mushroom", true, true, false, 0.1f);
    createProps(VoxelType::MUSHROOM_RED, "Red Mushroom", true, true, false, 0.1f);
    
    // Set special properties for wood and leaves
    for (auto wood_type : {VoxelType::WOOD_OAK, VoxelType::WOOD_PINE, VoxelType::WOOD_BIRCH, VoxelType::WOOD_MAHOGANY}) {
        auto& props = m_properties[wood_type];
        props.fire_spread_chance = 0.8f;
    }
    for (auto leaf_type : {VoxelType::LEAVES_OAK, VoxelType::LEAVES_PINE, VoxelType::LEAVES_BIRCH, VoxelType::LEAVES_PALM}) {
        auto& props = m_properties[leaf_type];
        props.fire_spread_chance = 0.9f;
    }
    
    // Biome Specific (40-49)
    createProps(VoxelType::SNOW, "Snow", true, false, false, 0.1f, BlockProperties::ToolType::SHOVEL, 0);
    createProps(VoxelType::ICE, "Ice", true, true, false, 0.5f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::PACKED_ICE, "Packed Ice", true, false, false, 1.0f, BlockProperties::ToolType::PICKAXE, 0);
    createProps(VoxelType::CACTUS, "Cactus", true, false, false, 0.4f);
    createProps(VoxelType::JUNGLE_VINE, "Jungle Vine", false, true, false, 0.2f, BlockProperties::ToolType::SHEARS, 0, true);
    createProps(VoxelType::CORAL_PINK, "Pink Coral", true, false, false, 0.3f);
    createProps(VoxelType::CORAL_BLUE, "Blue Coral", true, false, false, 0.3f);
    createProps(VoxelType::SEAWEED, "Seaweed", false, true, false, 0.1f);
    createProps(VoxelType::TUNDRA_MOSS, "Tundra Moss", true, false, false, 0.2f);
    createProps(VoxelType::DESERT_ROCK, "Desert Rock", true, false, false, 1.8f, BlockProperties::ToolType::PICKAXE, 0);
    
    // ========== CATEGORY 2: FLUIDS & GASES (50-59) ==========
    
    // Fluids (50-54)
    createProps(VoxelType::WATER, "Water", false, true, true, 0.0f);
    createProps(VoxelType::LAVA, "Lava", false, false, true, 0.0f, BlockProperties::ToolType::NONE, 0, false, 15);
    createProps(VoxelType::OIL, "Oil", false, true, true, 0.0f, BlockProperties::ToolType::NONE, 0, true);
    createProps(VoxelType::ACID, "Acid", false, true, true, 0.0f);
    createProps(VoxelType::HONEY, "Honey", false, true, true, 0.0f);
    
    // Gases (55-59)
    createProps(VoxelType::STEAM, "Steam", false, true, true, 0.0f);
    createProps(VoxelType::TOXIC_GAS, "Toxic Gas", false, true, true, 0.0f);
    createProps(VoxelType::NATURAL_GAS, "Natural Gas", false, true, true, 0.0f, BlockProperties::ToolType::NONE, 0, true);
    createProps(VoxelType::MAGICAL_MIST, "Magical Mist", false, true, true, 0.0f, BlockProperties::ToolType::NONE, 0, false, 5);
    createProps(VoxelType::SMOKE, "Smoke", false, true, true, 0.0f);
    
    // Set special fluid properties
    {
        auto& water_props = m_properties[VoxelType::WATER];
        water_props.friction = 0.1f;
        water_props.drop_count_min = 0; // Water doesn't drop items
        water_props.drop_count_max = 0;
        water_props.drop_chances[0] = 0;
        
        auto& lava_props = m_properties[VoxelType::LAVA];
        lava_props.friction = 0.2f;
        lava_props.drop_count_min = 0;
        lava_props.drop_count_max = 0;
        lava_props.drop_chances[0] = 0;
    }
    
    // For now, we'll initialize the most commonly used blocks (0-59)
    // The remaining blocks (60-255) will use default properties until explicitly defined
    // This allows the system to work immediately while being extensible
    
    std::cout << "[BlockPropertiesManager] Initialized properties for " << m_properties.size() << " block types." << std::endl;
}

} // namespace World
} // namespace VoxelEngine
