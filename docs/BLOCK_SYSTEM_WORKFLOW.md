# Block System Workflow - Technical Documentation

This document provides detailed technical information about the Voxel Fortress unified block resource system.

## System Overview

The Voxel Fortress block system is a **metadata-driven code generation pipeline** that eliminates manual synchronization between texture generation (Python) and game logic (C++). All block-related resources are generated from a single source of truth: JSON block definitions.

## Architecture

```
JSON Block Definitions
         ↓
    ID Registry System (assigns stable IDs)
         ↓
   ┌─────────────┬─────────────┬─────────────┐
   ↓             ↓             ↓             ↓
C++ Headers   C++ Source   Python Maps   Texture Atlas
voxel_types_  voxel_types_  block_mapping_  atlas_main.png
generated.h   generated.cpp generated.py   + metadata
```

## File Structure

### Data Sources (Human-Edited)
```
data/blocks/
├── terrain.json      # Natural blocks (stone, dirt, ores)
├── fluids.json       # Liquids and gases
├── processed.json    # Crafted/manufactured blocks  
├── functional.json   # Interactive blocks (doors, chests)
├── advanced.json     # High-tech/magical blocks
└── placeholder.json  # Development placeholders
```

### Generated Files (Auto-Created)
```
data/
└── id_registry.json                          # Stable ID assignments

engine/include/generated/
└── voxel_types_generated.h                   # C++ VoxelType enum

engine/src/generated/
└── voxel_types_generated.cpp                 # C++ property tables

block_mapping_generated.py                    # Python texture mappings

assets/textures/
├── atlas_main.png                            # Primary texture atlas
├── atlas_side.png                            # Side face atlas
├── atlas_bottom.png                          # Bottom face atlas
└── atlas_metadata.json                       # Atlas coordinate data
```

### Generation Scripts
```
scripts/generators/
├── generate_all.py      # Master orchestrator
├── id_manager.py        # Stable ID assignment
├── cpp_generator.py     # C++ code generation
└── python_generator.py  # Python code generation

create_atlas_official.py # Texture atlas generation
```

## Block Definition Format

### Complete Block Schema
```json
{
  "BLOCK_NAME": {
    "name": "Human Readable Name",
    "category": "terrain|fluids|processed|functional|advanced|placeholder",
    "type": "stone|wood|metal|organic|fluid|crystal|ceramic|ore|special",
    "subtype": "granite|oak|iron|grass|water|ruby|clay|coal|snow",
    "face_pattern": "uniform|grass|log|functional",
    "properties": {
      "solid": true|false,
      "transparent": true|false,
      "hardness": 0-10,
      "flammable": true|false,
      "liquid": true|false,
      "light_level": 0-15
    },
    "crafting": {
      "recipe": {...},
      "tool_required": "pickaxe|axe|shovel",
      "skill_level": 1-10
    }
  }
}
```

### Minimal Block Definition
```json
{
  "SIMPLE_STONE": {
    "name": "Simple Stone",
    "category": "terrain",
    "type": "stone",
    "subtype": "stone"
  }
}
```

## ID Assignment System

### Explicit ID Approach (Current Implementation)
- **All blocks have explicit `"id"` fields in their JSON definitions**
- **Block name serves as the unique identifier and dictionary key**
- **ID registry serves as backup/validation and migration tracking**
- **New blocks can be added without ID initially, system auto-assigns and writes back**

### Stability Guarantee
- **Once assigned, block IDs NEVER change**
- **Save game compatibility is preserved forever**
- **New blocks get the next available ID automatically**

### Assignment Algorithm
```python
# IDs are assigned in category order:
CATEGORY_ORDER = ["terrain", "fluids", "processed", "functional", "advanced", "placeholder"]

# Within each category, blocks with explicit IDs keep them
# Blocks without IDs get auto-assigned the next available ID
# IDs are written back to JSON files for consistency
```

### ID Registry Format
```json
{
  "version": "1.1",
  "last_generated": "2025-06-08T15:59:43.875037",
  "assignments": {
    "AIR": 0,
    "STONE": 1,
    "DIRT": 2,
    "GRASS": 3
  },
  "next_available_id": 257,
  "deprecated_blocks": {},
  "permanently_reserved_ids": [100],
  "notes": {
    "AIR": "Reserved ID 0 - special air block"
  }
}
```

### Block Definition Format
```json
{
  "STONE": {
    "id": 1,
    "name": "Stone",
    "description": "Basic stone block",
    "category": "terrain",
    "tags": ["solid", "natural"],
    "face_pattern": "uniform",
    "texture_info": {
      "generation": {
        "type": "stone",
        "subtype": "granite"
      }
    }
  }
}
```

## Developer Workflow

### Adding a New Block

#### Method 1: With Explicit ID (Recommended)
```json
{
  "MY_NEW_BLOCK": {
    "id": 999,  
    "name": "My New Block",
    "description": "A custom block for testing",
    "category": "terrain",
    "tags": ["solid", "custom"],
    "physical_properties": {
      "solid": true,
      "transparent": false,
      "hardness": 5.0
    },
    "texture_info": {
      "face_pattern": "uniform",
      "generation": {
        "type": "stone",
        "subtype": "granite"
      }
    },
    "world_generation": {
      "natural_generation": false
    }
  }
}
```

#### Method 2: Auto-Assigned ID (For Quick Testing)
```json
{
  "MY_TEST_BLOCK": {
    "name": "My Test Block",
    "description": "Quick test block - ID will be auto-assigned",
    "category": "terrain",
    "texture_info": {
      "face_pattern": "uniform",
      "generation": {
        "type": "stone",
        "subtype": "granite"
      }
    }
  }
}
```

#### Steps to Add a Block:

1. **Choose appropriate category file** (terrain, fluids, processed, functional, advanced, placeholder)

2. **Add block definition** to the chosen JSON file
   - Include explicit `"id"` field if you want a specific ID
   - Omit `"id"` field for auto-assignment (system will write it back)

3. **Run the migration command** (if you omitted the ID):
   ```bash
   python scripts/generators/id_manager.py --write-back-ids
   ```

4. **Generate all resources**:
   ```bash
   python scripts/generators/generate_all.py
   ```

5. **Build and test**:
   ```bash
   cd build && make
   ./bin/game
   ```

### Editing Existing Blocks

1. **Never change the `"id"` field** of existing blocks
2. **Block name is the unique identifier** - changing it requires migration
3. **All other fields can be freely modified**
4. **Re-run generation after changes**

### Block Removal/Deprecation

#### Safe Removal (Recommended)
```bash
# Mark block as deprecated with fallback
python scripts/generators/id_manager.py --deprecate OLD_BLOCK --reason "Replaced by improved version" --fallback NEW_BLOCK

# List all deprecated blocks
python scripts/generators/id_manager.py --list-deprecated
```

#### Direct Removal (Not Recommended)
- Only for test blocks that never shipped
- Can break save games if used in production

### Registry Management

#### Check Current Status
```bash
python scripts/generators/id_manager.py --summary
```

#### Write Back Missing IDs
```bash
python scripts/generators/id_manager.py --write-back-ids
```

### Best Practices

✅ **DO:**
- Use descriptive block names (e.g., `OAK_PLANKS`, not `WOOD1`)
- Include explicit IDs for production blocks
- Test texture generation before committing
- Run full generation before building
- Use deprecation for removing blocks

❌ **DON'T:**
- Change existing block IDs
- Remove blocks without deprecation
- Use auto-assigned IDs for production content
- Forget to run generation after JSON changes
