# World Generation Architecture Documentation

*Created: 2025-06-08 21:00*  
*Last Updated: 2025-06-08 21:00*  

## Dual-Path Architecture Overview

The Voxel Castle project maintains **two separate world generation systems** to ensure stability during development:

### Legacy World Generation System

**Files:**
- Primary: `engine/src/world/world_generator.cpp`
- Header: `engine/include/world/world_generator.h`

**Purpose:**
- Maintains existing "Resume Game" functionality
- Provides stable, working reference implementation
- Simple heightmap-based generation with basic biomes

**Status:** 
- **PRESERVED** - Modifications should be minimal and only for critical bugs
- Used by main menu "Resume Game" button
- Must remain functional at all times

**Features:**
- 2D Perlin noise heightmap generation
- Basic biome assignment (grass, stone, snow)
- Simple cave generation
- Established voxel placement logic

### New Advanced World Generation System

**Files:**
- Primary: `engine/src/world/seed_world_generator.cpp`
- Biome System: `engine/src/world/biome/` (biome_types.cpp, biome_data.cpp, biome_registry.cpp)
- Headers: `engine/include/world/` (corresponding .h files)

**Purpose:**
- Advanced world generation following `docs/BIOME_IMPLEMENTATION_PLAN.md`
- Realistic geological and ecological features
- Climate simulation, erosion, river networks
- Complex biome system with environmental data

**Status:**
- **ACTIVE DEVELOPMENT** - Following phased implementation plan
- Will be used by main menu "Create World" button (future)
- Currently in Phase 1: Advanced Foundation

**Planned Features:**
- Multi-tier generation (Template + Runtime Voxelization)
- Geological simulation (erosion, tectonics)
- Hydrological systems (rivers, watersheds)
- Climate modeling (temperature, precipitation)
- Advanced biome assignment (multi-factor logic)
- Historical simulation (future phase)

## Integration Points

### Game Menu System
- **"Resume Game"** → Legacy system (`world_generator.cpp`)
- **"Create World"** → New system (`seed_world_generator.cpp` + biome system)

### Save/Load System
- Legacy saves: Use legacy world generator for continued compatibility
- New saves: Use advanced system with environmental database

### Development Workflow
1. **Legacy System**: Only modify for critical bugs or compatibility
2. **New System**: Active development following implementation plan
3. **Testing**: Both systems must build and run successfully
4. **Migration**: Future consideration after new system reaches feature parity

## Critical Rules

1. **Never break the legacy system** - It's our stable reference
2. **New features go in the advanced system only**
3. **Both systems must compile and run at all times**
4. **Document any changes to either system**
5. **Test both paths after any significant changes**

## File Organization

```
engine/
├── src/world/
│   ├── world_generator.cpp          # Legacy system (PRESERVE)
│   ├── seed_world_generator.cpp     # New system (ACTIVE DEV)
│   └── biome/                       # New biome system
│       ├── biome_types.cpp
│       ├── biome_data.cpp
│       └── biome_registry.cpp
└── include/world/
    ├── world_generator.h            # Legacy system headers
    ├── seed_world_generator.h       # New system headers
    └── biome/                       # New biome system headers
        ├── biome_types.h
        ├── biome_data.h
        └── biome_registry.h
```

This architecture ensures we always have a working game while developing advanced features.
