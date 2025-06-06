#ifndef VOXEL_ENGINE_BLOCK_PROPERTIES_H
#define VOXEL_ENGINE_BLOCK_PROPERTIES_H

#include "world/voxel_types.h"
#include <unordered_map>
#include <string>
#include <array>

namespace VoxelEngine {
namespace World {

/**
 * @struct BlockProperties
 * @brief Defines the properties and behavior characteristics of each block type.
 */
struct BlockProperties {
    // Basic Properties
    std::string name;           ///< Display name of the block
    bool is_solid;              ///< Whether the block has collision
    bool is_transparent;        ///< Whether light passes through
    bool is_fluid;              ///< Whether the block behaves as a fluid
    bool is_walkable;           ///< Whether entities can walk on this block
    
    // Interaction Properties
    float hardness;             ///< Difficulty to break (0.0 = unbreakable, 1.0 = very easy)
    float blast_resistance;     ///< Resistance to explosions
    bool is_flammable;          ///< Whether the block can catch fire
    float fire_spread_chance;   ///< Probability of spreading fire (0.0-1.0)
    
    // Visual Properties
    bool emits_light;           ///< Whether the block produces light
    uint8_t light_level;        ///< Light level emitted (0-15)
    bool is_animated;           ///< Whether the texture animates
    float animation_speed;      ///< Animation speed multiplier
    
    // Physics Properties
    bool affected_by_gravity;   ///< Whether the block falls when unsupported
    bool can_be_pushed;         ///< Whether the block can be moved by pistons
    float friction;             ///< Surface friction for entities walking on it
    
    // Crafting & Drops
    std::array<VoxelType, 4> possible_drops; ///< What this block drops when broken
    std::array<uint8_t, 4> drop_chances;     ///< Chance (0-255) for each drop
    uint8_t drop_count_min;     ///< Minimum number of items dropped
    uint8_t drop_count_max;     ///< Maximum number of items dropped
    
    // Tool Requirements
    enum class ToolType : uint8_t {
        NONE = 0,       ///< No tool required
        PICKAXE = 1,    ///< Requires pickaxe
        AXE = 2,        ///< Requires axe
        SHOVEL = 3,     ///< Requires shovel
        SHEARS = 4      ///< Requires shears
    };
    ToolType required_tool;     ///< Tool type needed to harvest efficiently
    uint8_t required_tool_level; ///< Tool tier required (0=wood, 1=stone, 2=iron, etc.)
    
    // Environmental
    bool weathering_enabled;    ///< Whether the block changes over time
    VoxelType weathered_form;   ///< What this block becomes when weathered
    float weathering_rate;      ///< Rate of weathering (blocks per hour)
    
    // Advanced Properties
    bool conducts_redstone;     ///< Whether redstone signals pass through
    bool is_redstone_source;    ///< Whether the block emits redstone signal
    uint8_t redstone_power;     ///< Redstone power level (0-15)
    
    // Default constructor for uninitialized blocks
    BlockProperties() 
        : name("Unknown"), is_solid(true), is_transparent(false), is_fluid(false), 
          is_walkable(true), hardness(1.0f), blast_resistance(1.0f), is_flammable(false),
          fire_spread_chance(0.0f), emits_light(false), light_level(0), is_animated(false),
          animation_speed(1.0f), affected_by_gravity(false), can_be_pushed(true),
          friction(0.6f), possible_drops{VoxelType::AIR, VoxelType::AIR, VoxelType::AIR, VoxelType::AIR},
          drop_chances{0, 0, 0, 0}, drop_count_min(0), drop_count_max(0),
          required_tool(ToolType::NONE), required_tool_level(0), weathering_enabled(false),
          weathered_form(VoxelType::AIR), weathering_rate(0.0f), conducts_redstone(false),
          is_redstone_source(false), redstone_power(0) {}
};

/**
 * @class BlockPropertiesManager
 * @brief Manages block properties for all voxel types in the game.
 * 
 * This class provides a centralized system for accessing block properties,
 * ensuring consistent behavior across the game systems.
 */
class BlockPropertiesManager {
public:
    static BlockPropertiesManager& getInstance();
    
    // Core access methods
    const BlockProperties& getProperties(VoxelType type) const;
    bool isRegistered(VoxelType type) const;
    
    // Convenience methods for common queries
    bool isSolid(VoxelType type) const;
    bool isTransparent(VoxelType type) const;
    bool isFluid(VoxelType type) const;
    bool isWalkable(VoxelType type) const;
    bool emitsLight(VoxelType type) const;
    uint8_t getLightLevel(VoxelType type) const;
    float getHardness(VoxelType type) const;
    bool isFlammable(VoxelType type) const;
    
    // Advanced queries
    bool requiresTool(VoxelType type) const;
    BlockProperties::ToolType getRequiredTool(VoxelType type) const;
    bool canHarvestWith(VoxelType block, BlockProperties::ToolType tool, uint8_t tool_level) const;
    
    // Environmental queries
    bool isWeathering(VoxelType type) const;
    VoxelType getWeatheredForm(VoxelType type) const;
    
private:
    BlockPropertiesManager();
    ~BlockPropertiesManager() = default;
    
    // Prevent copying
    BlockPropertiesManager(const BlockPropertiesManager&) = delete;
    BlockPropertiesManager& operator=(const BlockPropertiesManager&) = delete;
    
    void initializeDefaultProperties();
    void registerBlockProperties(VoxelType type, const BlockProperties& properties);
    
    std::unordered_map<VoxelType, BlockProperties> m_properties;
    BlockProperties m_default_properties; ///< Fallback for unregistered blocks
};

} // namespace World
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_BLOCK_PROPERTIES_H
