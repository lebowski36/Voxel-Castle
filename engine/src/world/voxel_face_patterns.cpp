#include "world/voxel_face_patterns.h"

namespace VoxelEngine {
namespace World {

/**
 * @brief Comprehensive face pattern mapping for all 256 block types
 * 
 * This mapping defines the texture requirements for each block type to enable
 * efficient atlas packing and minimize wasted texture slots.
 */
const std::unordered_map<VoxelType, FacePattern> BLOCK_FACE_PATTERNS = {
    
    // ========== CATEGORY 1: TERRAIN & NATURAL (0-49) ==========
    
    // Basic Terrain (0-9)
    {VoxelType::AIR, FacePattern::UNIFORM},                    // Transparent, no texture needed
    {VoxelType::STONE, FacePattern::UNIFORM},                  // Same texture all faces
    {VoxelType::DIRT, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::GRASS, FacePattern::ALL_DIFFERENT},            // Green top, dirt sides, dirt bottom
    {VoxelType::SAND, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::GRAVEL, FacePattern::UNIFORM},                 // Same texture all faces
    {VoxelType::CLAY, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::BEDROCK, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::TOPSOIL, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::SUBSOIL, FacePattern::UNIFORM},                // Same texture all faces
    
    // Stone Varieties (10-19)
    {VoxelType::GRANITE, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::LIMESTONE, FacePattern::UNIFORM},              // Same texture all faces
    {VoxelType::MARBLE, FacePattern::UNIFORM},                 // Same texture all faces
    {VoxelType::SANDSTONE, FacePattern::UNIFORM},              // Same texture all faces
    {VoxelType::SLATE, FacePattern::UNIFORM},                  // Same texture all faces
    {VoxelType::BASALT, FacePattern::UNIFORM},                 // Same texture all faces
    {VoxelType::QUARTZITE, FacePattern::UNIFORM},              // Same texture all faces
    {VoxelType::OBSIDIAN, FacePattern::UNIFORM},               // Same texture all faces
    {VoxelType::PUMICE, FacePattern::UNIFORM},                 // Same texture all faces
    {VoxelType::SHALE, FacePattern::UNIFORM},                  // Same texture all faces
    
    // Ores & Minerals (20-29)
    {VoxelType::COAL_ORE, FacePattern::UNIFORM},               // Same texture all faces
    {VoxelType::IRON_ORE, FacePattern::UNIFORM},               // Same texture all faces
    {VoxelType::COPPER_ORE, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::TIN_ORE, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::SILVER_ORE, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::GOLD_ORE, FacePattern::UNIFORM},               // Same texture all faces
    {VoxelType::GEM_RUBY, FacePattern::UNIFORM},               // Same texture all faces
    {VoxelType::GEM_SAPPHIRE, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::GEM_EMERALD, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::GEM_DIAMOND, FacePattern::UNIFORM},            // Same texture all faces
    
    // Organic Natural (30-39)
    {VoxelType::WOOD_OAK, FacePattern::TOP_BOTTOM_DIFFERENT},      // End grain top/bottom, bark sides
    {VoxelType::WOOD_PINE, FacePattern::TOP_BOTTOM_DIFFERENT},     // End grain top/bottom, bark sides
    {VoxelType::WOOD_BIRCH, FacePattern::TOP_BOTTOM_DIFFERENT},    // End grain top/bottom, bark sides
    {VoxelType::WOOD_MAHOGANY, FacePattern::TOP_BOTTOM_DIFFERENT}, // End grain top/bottom, bark sides
    {VoxelType::LEAVES_OAK, FacePattern::UNIFORM},                 // Same texture all faces
    {VoxelType::LEAVES_PINE, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::LEAVES_BIRCH, FacePattern::UNIFORM},               // Same texture all faces
    {VoxelType::LEAVES_PALM, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::MUSHROOM_BROWN, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::MUSHROOM_RED, FacePattern::UNIFORM},               // Same texture all faces
    
    // Biome Specific (40-49)
    {VoxelType::SNOW, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::ICE, FacePattern::UNIFORM},                    // Same texture all faces
    {VoxelType::PACKED_ICE, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::CACTUS, FacePattern::TOP_BOTTOM_DIFFERENT},    // Different top, special sides
    {VoxelType::JUNGLE_VINE, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::CORAL_PINK, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::CORAL_BLUE, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::SEAWEED, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::TUNDRA_MOSS, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::DESERT_ROCK, FacePattern::UNIFORM},            // Same texture all faces
    
    // ========== CATEGORY 2: FLUIDS & GASES (50-59) ==========
    
    // Fluids (50-54)
    {VoxelType::WATER, FacePattern::UNIFORM},                  // Same texture all faces
    {VoxelType::LAVA, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::OIL, FacePattern::UNIFORM},                    // Same texture all faces
    {VoxelType::ACID, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::HONEY, FacePattern::UNIFORM},                  // Same texture all faces
    
    // Gases (55-59)
    {VoxelType::STEAM, FacePattern::UNIFORM},                  // Same texture all faces
    {VoxelType::TOXIC_GAS, FacePattern::UNIFORM},              // Same texture all faces
    {VoxelType::NATURAL_GAS, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::MAGICAL_MIST, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::SMOKE, FacePattern::UNIFORM},                  // Same texture all faces
    
    // ========== CATEGORY 3: PROCESSED MATERIALS (60-99) ==========
    
    // Stone Processing (60-69)
    {VoxelType::STONE_BRICK, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::GRANITE_BRICK, FacePattern::UNIFORM},          // Same texture all faces
    {VoxelType::MARBLE_TILE, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::SANDSTONE_BRICK, FacePattern::UNIFORM},        // Same texture all faces
    {VoxelType::SLATE_TILE, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::COBBLESTONE, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::SMOOTH_STONE, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::POLISHED_GRANITE, FacePattern::UNIFORM},       // Same texture all faces
    {VoxelType::POLISHED_MARBLE, FacePattern::UNIFORM},        // Same texture all faces
    {VoxelType::FLAGSTONE, FacePattern::UNIFORM},              // Same texture all faces
    
    // Wood Processing (70-79)
    {VoxelType::OAK_PLANKS, FacePattern::UNIFORM},             // Processed lumber, uniform
    {VoxelType::PINE_PLANKS, FacePattern::UNIFORM},            // Processed lumber, uniform
    {VoxelType::BIRCH_PLANKS, FacePattern::UNIFORM},           // Processed lumber, uniform
    {VoxelType::MAHOGANY_PLANKS, FacePattern::UNIFORM},        // Processed lumber, uniform
    {VoxelType::OAK_BEAM, FacePattern::TOP_BOTTOM_DIFFERENT},  // Structural beam, end grain different
    {VoxelType::PINE_BEAM, FacePattern::TOP_BOTTOM_DIFFERENT}, // Structural beam, end grain different
    {VoxelType::HARDWOOD_BEAM, FacePattern::TOP_BOTTOM_DIFFERENT}, // Structural beam, end grain different
    {VoxelType::BAMBOO_PLANK, FacePattern::UNIFORM},           // Processed bamboo, uniform
    {VoxelType::CORK, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::CHARCOAL_BLOCK, FacePattern::UNIFORM},         // Same texture all faces
    
    // Metal Blocks (80-89)
    {VoxelType::IRON_BLOCK, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::COPPER_BLOCK, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::BRONZE_BLOCK, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::STEEL_BLOCK, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::SILVER_BLOCK, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::GOLD_BLOCK, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::BRASS_BLOCK, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::PEWTER_BLOCK, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::MITHRIL_BLOCK, FacePattern::UNIFORM},          // Same texture all faces
    {VoxelType::ADAMANTINE_BLOCK, FacePattern::UNIFORM},       // Same texture all faces
    
    // Clay & Ceramic (90-99)
    {VoxelType::CLAY_BRICK, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::TERRACOTTA, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::GLAZED_TILE_WHITE, FacePattern::UNIFORM},      // Same texture all faces
    {VoxelType::GLAZED_TILE_RED, FacePattern::UNIFORM},        // Same texture all faces
    {VoxelType::GLAZED_TILE_BLUE, FacePattern::UNIFORM},       // Same texture all faces
    {VoxelType::GLAZED_TILE_GREEN, FacePattern::UNIFORM},      // Same texture all faces
    {VoxelType::PORCELAIN, FacePattern::UNIFORM},              // Same texture all faces
    {VoxelType::STONEWARE, FacePattern::UNIFORM},              // Same texture all faces
    {VoxelType::EARTHENWARE, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::CERAMIC_TILE, FacePattern::UNIFORM},           // Same texture all faces
    
    // ========== CATEGORY 4: FUNCTIONAL BLOCKS (100-149) ==========
    
    // Doors & Openings (100-109) - Future directional support
    {VoxelType::DOOR_WOOD, FacePattern::DIRECTIONAL},          // Front/back different, sides different
    {VoxelType::DOOR_IRON, FacePattern::DIRECTIONAL},          // Front/back different, sides different
    {VoxelType::DOOR_REINFORCED, FacePattern::DIRECTIONAL},    // Front/back different, sides different
    {VoxelType::WINDOW_GLASS, FacePattern::UNIFORM},           // Same texture all faces (for now)
    {VoxelType::WINDOW_IRON_BARS, FacePattern::UNIFORM},       // Same texture all faces
    {VoxelType::PORTCULLIS, FacePattern::DIRECTIONAL},         // Oriented structure
    {VoxelType::DRAWBRIDGE, FacePattern::TOP_BOTTOM_DIFFERENT}, // Top surface different from sides
    {VoxelType::GATE_WOOD, FacePattern::DIRECTIONAL},          // Front/back different
    {VoxelType::GATE_IRON, FacePattern::DIRECTIONAL},          // Front/back different
    {VoxelType::HATCH_WOOD, FacePattern::TOP_BOTTOM_DIFFERENT}, // Top surface different
    
    // Storage & Containers (110-119)
    {VoxelType::CHEST_WOOD, FacePattern::DIRECTIONAL},         // Front face different (lock/handle)
    {VoxelType::CHEST_IRON, FacePattern::DIRECTIONAL},         // Front face different (lock/handle)
    {VoxelType::BARREL, FacePattern::TOP_BOTTOM_DIFFERENT},    // Top/bottom different from sides
    {VoxelType::CRATE, FacePattern::UNIFORM},                  // Same texture all faces
    {VoxelType::BIN_STORAGE, FacePattern::TOP_BOTTOM_DIFFERENT}, // Open top different
    {VoxelType::SHELF, FacePattern::DIRECTIONAL},              // Front face different (shelves)
    {VoxelType::CABINET, FacePattern::DIRECTIONAL},            // Front face different (doors)
    {VoxelType::STRONGBOX, FacePattern::DIRECTIONAL},          // Front face different (lock)
    {VoxelType::VAULT_DOOR, FacePattern::DIRECTIONAL},         // Front face different (door)
    {VoxelType::SAFE, FacePattern::DIRECTIONAL},               // Front face different (door)
    
    // Mechanisms (120-129)
    {VoxelType::LEVER, FacePattern::DIRECTIONAL},              // Directional mechanism
    {VoxelType::BUTTON, FacePattern::DIRECTIONAL},             // Front face different
    {VoxelType::PRESSURE_PLATE, FacePattern::TOP_BOTTOM_DIFFERENT}, // Top different from sides
    {VoxelType::GEAR_MECHANISM, FacePattern::UNIFORM},         // Same texture all faces
    {VoxelType::PULLEY, FacePattern::DIRECTIONAL},             // Oriented mechanism
    {VoxelType::ROPE, FacePattern::UNIFORM},                   // Same texture all faces
    {VoxelType::CHAIN, FacePattern::UNIFORM},                  // Same texture all faces
    {VoxelType::AXLE, FacePattern::TOP_BOTTOM_DIFFERENT},      // End faces different from sides
    {VoxelType::PUMP, FacePattern::DIRECTIONAL},               // Input/output faces different
    {VoxelType::VALVE, FacePattern::DIRECTIONAL},              // Flow direction matters
    
    // Lighting (130-139)
    {VoxelType::TORCH, FacePattern::DIRECTIONAL},              // Mounted on specific face
    {VoxelType::LANTERN, FacePattern::UNIFORM},                // Same texture all faces
    {VoxelType::CANDLE, FacePattern::TOP_BOTTOM_DIFFERENT},    // Flame on top, base on bottom
    {VoxelType::CHANDELIER, FacePattern::TOP_BOTTOM_DIFFERENT}, // Hanging from top
    {VoxelType::BRAZIER, FacePattern::TOP_BOTTOM_DIFFERENT},   // Fire bowl on top
    {VoxelType::CAMPFIRE, FacePattern::TOP_BOTTOM_DIFFERENT},  // Fire on top, logs/stones below
    {VoxelType::FURNACE, FacePattern::DIRECTIONAL},            // Front opening different
    {VoxelType::FORGE, FacePattern::DIRECTIONAL},              // Working surface different
    {VoxelType::KILN, FacePattern::DIRECTIONAL},               // Opening/door different
    {VoxelType::CRYSTAL_LIGHT, FacePattern::UNIFORM},          // Same texture all faces
    
    // Furniture (140-149)
    {VoxelType::TABLE_WOOD, FacePattern::TOP_BOTTOM_DIFFERENT}, // Table surface different
    {VoxelType::CHAIR_WOOD, FacePattern::DIRECTIONAL},         // Seat and back different faces
    {VoxelType::BED, FacePattern::TOP_BOTTOM_DIFFERENT},       // Sleeping surface different
    {VoxelType::THRONE, FacePattern::DIRECTIONAL},             // Front facing design
    {VoxelType::ALTAR, FacePattern::TOP_BOTTOM_DIFFERENT},     // Sacred surface on top
    {VoxelType::PODIUM, FacePattern::TOP_BOTTOM_DIFFERENT},    // Speaking surface on top
    {VoxelType::BENCH, FacePattern::TOP_BOTTOM_DIFFERENT},     // Seating surface different
    {VoxelType::STOOL, FacePattern::TOP_BOTTOM_DIFFERENT},     // Seating surface different
    {VoxelType::BOOKSHELF, FacePattern::DIRECTIONAL},          // Books visible on front
    {VoxelType::DESK, FacePattern::TOP_BOTTOM_DIFFERENT},      // Work surface different
    
    // ========== CATEGORY 5: ADVANCED & MAGICAL (150-179) ==========
    
    // Magical Materials (150-159)
    {VoxelType::CRYSTAL_CLEAR, FacePattern::UNIFORM},          // Same texture all faces
    {VoxelType::CRYSTAL_BLUE, FacePattern::UNIFORM},           // Same texture all faces
    {VoxelType::CRYSTAL_RED, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::CRYSTAL_GREEN, FacePattern::UNIFORM},          // Same texture all faces
    {VoxelType::ENCHANTED_STONE, FacePattern::UNIFORM},        // Same texture all faces
    {VoxelType::RUNIC_BLOCK, FacePattern::DIRECTIONAL},        // Runes oriented to specific faces
    {VoxelType::ETHER_CRYSTAL, FacePattern::UNIFORM},          // Same texture all faces
    {VoxelType::VOID_STONE, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::CELESTIAL_MARBLE, FacePattern::UNIFORM},       // Same texture all faces
    {VoxelType::SHADOW_GLASS, FacePattern::UNIFORM},           // Same texture all faces
    
    // Advanced Technology (160-169)
    {VoxelType::CONCRETE, FacePattern::UNIFORM},               // Same texture all faces
    {VoxelType::REINFORCED_CONCRETE, FacePattern::UNIFORM},    // Same texture all faces
    {VoxelType::STEEL_BEAM, FacePattern::TOP_BOTTOM_DIFFERENT}, // End faces different
    {VoxelType::GLASS_STEEL, FacePattern::UNIFORM},            // Same texture all faces
    {VoxelType::COMPOSITE_ARMOR, FacePattern::UNIFORM},        // Same texture all faces
    {VoxelType::ENERGY_CONDUIT, FacePattern::DIRECTIONAL},     // Flow direction matters
    {VoxelType::FORCE_FIELD_GEN, FacePattern::DIRECTIONAL},    // Emitter faces different
    {VoxelType::QUANTUM_BLOCK, FacePattern::UNIFORM},          // Same texture all faces
    {VoxelType::NANOCARBON, FacePattern::UNIFORM},             // Same texture all faces
    {VoxelType::PLASMA_CONTAINER, FacePattern::DIRECTIONAL},   // Containment interfaces different
    
    // Special Properties (170-179)
    {VoxelType::BLOCK_INVISIBLE, FacePattern::UNIFORM},        // Same texture all faces (if visible in editor)
    {VoxelType::BLOCK_INTANGIBLE, FacePattern::UNIFORM},       // Same texture all faces
    {VoxelType::BLOCK_ANTIGRAV, FacePattern::UNIFORM},         // Same texture all faces
    {VoxelType::BLOCK_MAGNETIC, FacePattern::UNIFORM},         // Same texture all faces
    {VoxelType::BLOCK_TEMPORAL, FacePattern::UNIFORM},         // Same texture all faces
    {VoxelType::BLOCK_DIMENSIONAL, FacePattern::DIRECTIONAL},  // Portal faces different
    {VoxelType::BLOCK_REGENERATING, FacePattern::UNIFORM},     // Same texture all faces
    {VoxelType::BLOCK_EXPLOSIVE, FacePattern::UNIFORM},        // Same texture all faces
    {VoxelType::BLOCK_ABSORBING, FacePattern::UNIFORM},        // Same texture all faces
    {VoxelType::BLOCK_AMPLIFYING, FacePattern::UNIFORM}        // Same texture all faces
    
    // Note: IDs 180-255 are reserved for future expansion
    // When adding new blocks, extend this mapping accordingly
};

FacePattern getFacePattern(VoxelType voxelType) {
    auto it = BLOCK_FACE_PATTERNS.find(voxelType);
    if (it != BLOCK_FACE_PATTERNS.end()) {
        return it->second;
    }
    
    // Default fallback for unmapped types (future blocks)
    return FacePattern::UNIFORM;
}

uint8_t getAtlasSlotCount(FacePattern pattern) {
    switch (pattern) {
        case FacePattern::UNIFORM:
            return 1;  // 1 slot in main atlas only
        case FacePattern::TOP_BOTTOM_DIFFERENT:
            return 2;  // 1 in main atlas (top/bottom), 1 in side atlas (sides)
        case FacePattern::ALL_DIFFERENT:
            return 3;  // 1 in main atlas (top), 1 in side atlas (sides), 1 in bottom atlas (bottom)
        case FacePattern::DIRECTIONAL:
            return 4;  // Variable, but minimum 4 for different orientations
        default:
            return 1;
    }
}

bool requiresSideAtlas(FacePattern pattern) {
    switch (pattern) {
        case FacePattern::TOP_BOTTOM_DIFFERENT:
        case FacePattern::ALL_DIFFERENT:
        case FacePattern::DIRECTIONAL:
            return true;
        case FacePattern::UNIFORM:
        default:
            return false;
    }
}

bool requiresBottomAtlas(FacePattern pattern) {
    switch (pattern) {
        case FacePattern::ALL_DIFFERENT:
        case FacePattern::DIRECTIONAL:
            return true;
        case FacePattern::UNIFORM:
        case FacePattern::TOP_BOTTOM_DIFFERENT:
        default:
            return false;
    }
}

} // namespace World
} // namespace VoxelEngine
