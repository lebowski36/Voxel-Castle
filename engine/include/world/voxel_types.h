
#ifndef VOXELCASTLE_VOXEL_TYPES_H
#define VOXELCASTLE_VOXEL_TYPES_H

// Standard library includes first
#include <cstdint>

namespace VoxelEngine {
namespace World {

/**
 * @enum VoxelType
 * @brief Defines the different types of voxels available in the game world.
 *
 * Each voxel type is represented by an 8-bit unsigned integer. This enum provides
 * named constants for all voxel types, organized by category for clarity.
 * The underlying type is `uint8_t`, allowing for exactly 256 distinct voxel types.
 */
enum class VoxelType : uint8_t {
    // ========== CATEGORY 1: TERRAIN & NATURAL (0-49) ==========
    
    // Basic Terrain (0-9)
    AIR = 0,        /**< Empty space, transparent and non-collidable */
    STONE = 1,      /**< Basic stone, common solid building material */
    DIRT = 2,       /**< Natural dirt material */
    GRASS = 3,      /**< Grass surface blocks */
    SAND = 4,       /**< Granular sand material */
    GRAVEL = 5,     /**< Loose rock fragments */
    CLAY = 6,       /**< Clay material for ceramics */
    BEDROCK = 7,    /**< Unbreakable base layer */
    TOPSOIL = 8,    /**< Rich farming soil */
    SUBSOIL = 9,    /**< Nutrient-poor deeper soil */
    
    // Stone Varieties (10-19)
    GRANITE = 10,     /**< Hard igneous rock */
    LIMESTONE = 11,   /**< Sedimentary rock, good for lime/cement */
    MARBLE = 12,      /**< Metamorphic rock, decorative */
    SANDSTONE = 13,   /**< Compressed sand formation */
    SLATE = 14,       /**< Good for roofing and tiles */
    BASALT = 15,      /**< Dark volcanic rock */
    QUARTZITE = 16,   /**< Very hard, decorative stone */
    OBSIDIAN = 17,    /**< Volcanic glass, sharp tools */
    PUMICE = 18,      /**< Lightweight volcanic rock */
    SHALE = 19,       /**< Sedimentary rock, oil-bearing */
    
    // Ores & Minerals (20-29)
    COAL_ORE = 20,      /**< Coal deposits for fuel */
    IRON_ORE = 21,      /**< Iron ore for metalworking */
    COPPER_ORE = 22,    /**< Copper ore for alloys */
    TIN_ORE = 23,       /**< Tin ore for bronze */
    SILVER_ORE = 24,    /**< Precious silver deposits */
    GOLD_ORE = 25,      /**< Precious gold deposits */
    GEM_RUBY = 26,      /**< Red precious gems */
    GEM_SAPPHIRE = 27,  /**< Blue precious gems */
    GEM_EMERALD = 28,   /**< Green precious gems */
    GEM_DIAMOND = 29,   /**< Hardest natural material */
    
    // Organic Natural (30-39)
    WOOD_OAK = 30,      /**< Oak wood (replaces old WOOD) */
    WOOD_PINE = 31,     /**< Pine wood, lighter color */
    WOOD_BIRCH = 32,    /**< Birch wood, pale color */
    WOOD_MAHOGANY = 33, /**< Dark, rich mahogany */
    LEAVES_OAK = 34,    /**< Oak leaves (replaces old LEAVES) */
    LEAVES_PINE = 35,   /**< Pine needles */
    LEAVES_BIRCH = 36,  /**< Birch leaves */
    LEAVES_PALM = 37,   /**< Tropical palm fronds */
    MUSHROOM_BROWN = 38, /**< Brown mushrooms */
    MUSHROOM_RED = 39,  /**< Red mushrooms */
    
    // Biome Specific (40-49)
    SNOW = 40,          /**< Snow layers */
    ICE = 41,           /**< Regular ice */
    PACKED_ICE = 42,    /**< Harder, denser ice */
    CACTUS = 43,        /**< Desert cactus */
    JUNGLE_VINE = 44,   /**< Hanging jungle vines */
    CORAL_PINK = 45,    /**< Pink coral formations */
    CORAL_BLUE = 46,    /**< Blue coral formations */
    SEAWEED = 47,       /**< Ocean vegetation */
    TUNDRA_MOSS = 48,   /**< Arctic moss cover */
    DESERT_ROCK = 49,   /**< Weathered desert stone */
    
    // ========== CATEGORY 2: FLUIDS & GASES (50-59) ==========
    
    // Fluids (50-54)
    WATER = 50,         /**< Water (moved from old ID 6) */
    LAVA = 51,          /**< Molten rock */
    OIL = 52,           /**< Crude oil deposits */
    ACID = 53,          /**< Corrosive acid */
    HONEY = 54,         /**< Viscous honey */
    
    // Gases (55-59)
    STEAM = 55,         /**< Water vapor */
    TOXIC_GAS = 56,     /**< Dangerous gas */
    NATURAL_GAS = 57,   /**< Flammable natural gas */
    MAGICAL_MIST = 58,  /**< Ethereal magical vapor */
    SMOKE = 59,         /**< Combustion smoke */
    
    // ========== CATEGORY 3: PROCESSED MATERIALS (60-99) ==========
    
    // Stone Processing (60-69)
    STONE_BRICK = 60,       /**< Processed stone bricks */
    GRANITE_BRICK = 61,     /**< Granite masonry */
    MARBLE_TILE = 62,       /**< Polished marble tiles */
    SANDSTONE_BRICK = 63,   /**< Sandstone blocks */
    SLATE_TILE = 64,        /**< Slate roofing tiles */
    COBBLESTONE = 65,       /**< Rounded stone cobbles */
    SMOOTH_STONE = 66,      /**< Polished stone */
    POLISHED_GRANITE = 67,  /**< Finished granite */
    POLISHED_MARBLE = 68,   /**< Finished marble */
    FLAGSTONE = 69,         /**< Flat paving stones */
    
    // Wood Processing (70-79)
    OAK_PLANKS = 70,        /**< Oak lumber */
    PINE_PLANKS = 71,       /**< Pine lumber */
    BIRCH_PLANKS = 72,      /**< Birch lumber */
    MAHOGANY_PLANKS = 73,   /**< Mahogany lumber */
    OAK_BEAM = 74,          /**< Structural oak beam */
    PINE_BEAM = 75,         /**< Structural pine beam */
    HARDWOOD_BEAM = 76,     /**< Strong hardwood beam */
    BAMBOO_PLANK = 77,      /**< Processed bamboo */
    CORK = 78,              /**< Insulating cork */
    CHARCOAL_BLOCK = 79,    /**< Compressed charcoal */
    
    // Metal Blocks (80-89)
    IRON_BLOCK = 80,        /**< Iron metal block */
    COPPER_BLOCK = 81,      /**< Copper metal block */
    BRONZE_BLOCK = 82,      /**< Bronze alloy block */
    STEEL_BLOCK = 83,       /**< Steel alloy block */
    SILVER_BLOCK = 84,      /**< Silver metal block */
    GOLD_BLOCK = 85,        /**< Gold metal block */
    BRASS_BLOCK = 86,       /**< Brass alloy block */
    PEWTER_BLOCK = 87,      /**< Pewter alloy block */
    MITHRIL_BLOCK = 88,     /**< Magical metal block */
    ADAMANTINE_BLOCK = 89,  /**< Legendary metal block */
    
    // Clay & Ceramic (90-99)
    CLAY_BRICK = 90,        /**< Fired clay bricks */
    TERRACOTTA = 91,        /**< Fired clay pottery */
    GLAZED_TILE_WHITE = 92, /**< White ceramic tiles */
    GLAZED_TILE_RED = 93,   /**< Red ceramic tiles */
    GLAZED_TILE_BLUE = 94,  /**< Blue ceramic tiles */
    GLAZED_TILE_GREEN = 95, /**< Green ceramic tiles */
    PORCELAIN = 96,         /**< Fine ceramic material */
    STONEWARE = 97,         /**< Durable ceramic */
    EARTHENWARE = 98,       /**< Basic ceramic */
    CERAMIC_TILE = 99,      /**< General ceramic tiles */
    
    // ========== CATEGORY 4: FUNCTIONAL BLOCKS (100-149) ==========
    
    // Doors & Openings (100-109)
    DOOR_WOOD = 100,        /**< Wooden door */
    DOOR_IRON = 101,        /**< Iron door */
    DOOR_REINFORCED = 102,  /**< Reinforced door */
    WINDOW_GLASS = 103,     /**< Glass window */
    WINDOW_IRON_BARS = 104, /**< Iron bar window */
    PORTCULLIS = 105,       /**< Castle gate */
    DRAWBRIDGE = 106,       /**< Movable bridge */
    GATE_WOOD = 107,        /**< Wooden gate */
    GATE_IRON = 108,        /**< Iron gate */
    HATCH_WOOD = 109,       /**< Wooden hatch */
    
    // Storage & Containers (110-119)
    CHEST_WOOD = 110,       /**< Wooden storage chest */
    CHEST_IRON = 111,       /**< Iron storage chest */
    BARREL = 112,           /**< Storage barrel */
    CRATE = 113,            /**< Wooden crate */
    BIN_STORAGE = 114,      /**< Storage bin */
    SHELF = 115,            /**< Storage shelf */
    CABINET = 116,          /**< Storage cabinet */
    STRONGBOX = 117,        /**< Secure storage */
    VAULT_DOOR = 118,       /**< Vault entrance */
    SAFE = 119,             /**< Small secure storage */
    
    // Mechanisms (120-129)
    LEVER = 120,            /**< Mechanical lever */
    BUTTON = 121,           /**< Push button */
    PRESSURE_PLATE = 122,   /**< Floor pressure sensor */
    GEAR_MECHANISM = 123,   /**< Gear assembly */
    PULLEY = 124,           /**< Rope pulley system */
    ROPE = 125,             /**< Hemp rope */
    CHAIN = 126,            /**< Metal chain */
    AXLE = 127,             /**< Rotating shaft */
    PUMP = 128,             /**< Fluid pump */
    VALVE = 129,            /**< Flow control valve */
    
    // Lighting (130-139)
    TORCH = 130,            /**< Burning torch */
    LANTERN = 131,          /**< Oil lantern */
    CANDLE = 132,           /**< Wax candle */
    CHANDELIER = 133,       /**< Hanging light fixture */
    BRAZIER = 134,          /**< Fire brazier */
    CAMPFIRE = 135,         /**< Open campfire */
    FURNACE = 136,          /**< Smelting furnace */
    FORGE = 137,            /**< Metalworking forge */
    KILN = 138,             /**< Ceramic kiln */
    CRYSTAL_LIGHT = 139,    /**< Magical light crystal */
    
    // Furniture (140-149)
    TABLE_WOOD = 140,       /**< Wooden table */
    CHAIR_WOOD = 141,       /**< Wooden chair */
    BED = 142,              /**< Sleeping bed */
    THRONE = 143,           /**< Royal throne */
    ALTAR = 144,            /**< Religious altar */
    PODIUM = 145,           /**< Speaking podium */
    BENCH = 146,            /**< Wooden bench */
    STOOL = 147,            /**< Simple stool */
    BOOKSHELF = 148,        /**< Book storage */
    DESK = 149,             /**< Writing desk */
    
    // ========== CATEGORY 5: ADVANCED & MAGICAL (150-199) ==========
    
    // Magical Materials (150-159)
    CRYSTAL_CLEAR = 150,    /**< Clear magical crystal */
    CRYSTAL_BLUE = 151,     /**< Blue magical crystal */
    CRYSTAL_RED = 152,      /**< Red magical crystal */
    CRYSTAL_GREEN = 153,    /**< Green magical crystal */
    ENCHANTED_STONE = 154,  /**< Magically enhanced stone */
    RUNIC_BLOCK = 155,      /**< Stone with carved runes */
    ETHER_CRYSTAL = 156,    /**< Ethereal crystal */
    VOID_STONE = 157,       /**< Dark void material */
    CELESTIAL_MARBLE = 158, /**< Heavenly marble */
    SHADOW_GLASS = 159,     /**< Dark translucent material */
    
    // Advanced Technology (160-169)
    CONCRETE = 160,           /**< Modern concrete */
    REINFORCED_CONCRETE = 161, /**< Steel-reinforced concrete */
    STEEL_BEAM = 162,         /**< Structural steel */
    GLASS_STEEL = 163,        /**< Transparent metal */
    COMPOSITE_ARMOR = 164,    /**< Advanced armor plating */
    ENERGY_CONDUIT = 165,     /**< Energy transmission */
    FORCE_FIELD_GEN = 166,    /**< Force field generator */
    QUANTUM_BLOCK = 167,      /**< Quantum material */
    NANOCARBON = 168,         /**< Carbon nanotube material */
    PLASMA_CONTAINER = 169,   /**< Plasma containment */
    
    // Special Properties (170-179)
    BLOCK_INVISIBLE = 170,    /**< Invisible blocks for secrets */
    BLOCK_INTANGIBLE = 171,   /**< Walkthrough blocks */
    BLOCK_ANTIGRAV = 172,     /**< Anti-gravity blocks */
    BLOCK_MAGNETIC = 173,     /**< Magnetic attraction blocks */
    BLOCK_TEMPORAL = 174,     /**< Time-affecting blocks */
    BLOCK_DIMENSIONAL = 175,  /**< Portal material */
    BLOCK_REGENERATING = 176, /**< Self-healing blocks */
    BLOCK_EXPLOSIVE = 177,    /**< Destructible explosive */
    BLOCK_ABSORBING = 178,    /**< Energy dampening */
    BLOCK_AMPLIFYING = 179,   /**< Energy boosting */
    
    // ========== RESERVED FOR FUTURE EXPANSION (180-255) ==========
    // Keep remaining IDs available for future features
    
    // Legacy compatibility aliases (will be phased out)
    WOOD = WOOD_OAK,        /**< @deprecated Use WOOD_OAK instead */
    LEAVES = LEAVES_OAK     /**< @deprecated Use LEAVES_OAK instead */
};

} // namespace World
} // namespace VoxelEngine

#endif //VOXELCASTLE_VOXEL_TYPES_H
