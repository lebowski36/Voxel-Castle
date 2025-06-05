# World Generation System
*Created: 2025-06-05 22:59*
*Last Updated: 2025-06-05 23:30*

## Overview
This document outlines the implementation plan for Voxel Castle's world generation system. The goal is to transition from the current legacy world generation to a more flexible, feature-rich seed-based generation system that supports both small detailed worlds and massive continent-scale worlds.

## World Generation Philosophy

**Inspired by Dwarf Fortress + Minecraft Hybrid:**
- **World Creation Phase**: Full world pre-generation with historical simulation (Dwarf Fortress style)
- **Runtime Phase**: Detailed chunk voxelization on-demand (Minecraft style)
- **Scale Flexibility**: From 10km² starter worlds to 25,000,000km² epic worlds (quarter Earth-size)

### Two-Tier Generation Strategy

#### Tier 1: World Creation (Pre-Generation)
- **Scope**: Entire world at region/biome scale (e.g., 1km² regions)
- **Data**: Climate patterns, geological structure, biome placement, civilization sites, major resource deposits, historical events timeline
- **Timeline**: Seconds to hours depending on world size and historical simulation depth
- **Output**: "World Template" - comprehensive region-level database
- **Storage**: Persistent disk storage for large worlds (streaming from disk)

#### Tier 2: Runtime Voxelization (On-Demand)  
- **Scope**: Individual 32x32x32 voxel chunks around active player areas
- **Data**: Detailed terrain heightmaps, structure placement, precise ore veins, cave networks
- **Timeline**: < 16ms per chunk for smooth 60fps gameplay
- **Input**: World template data for the chunk's region + deterministic seed algorithms
- **Caching**: In-memory cache for recently accessed chunks

### Scalable World Sizes

#### Available Options:
1. **Starter Worlds**: 10km² - 100km² (Immediate availability)
2. **Regional Worlds**: 1,000km² - 10,000km² (Standard gameplay)
3. **Continental Worlds**: 100,000km² - 1,000,000km² (Epic campaigns)
4. **Massive Worlds**: 5,000,000km² - 25,000,000km² (Quarter Earth-size, initially grayed out)

#### Technical Considerations:
- **RAM Usage**: Adaptive based on world size and active area
- **Disk Streaming**: Essential for worlds > 1,000km²
- **Generation Time**: Configurable historical simulation depth
- **Optimization Gates**: Large worlds unlock after performance optimization

## Design Requirements Integration

*This section integrates requirements from `/docs/design_specifications/Game Concept/World Generation.md` into the 08c implementation plan.*

### Core Design Goals

**From Design Document:**
1. **Exploration & Discovery**: Create a sense of wonder and reward player curiosity
2. **Geological & Ecological Plausibility**: Follow logical patterns that players can learn
3. **Meaningful Player Choice**: Settlement location decisions should have significant consequences
4. **Narrative Foundation**: Provide rich foundation for emergent stories
5. **Performance Balance**: Balance detail with acceptable generation times

### Comprehensive Feature Requirements

#### 1. Terrain Generation Features
- **Multi-Scale Height Maps**: Multi-octave noise functions creating varied elevations
- **Erosion Simulation**: Water and wind-based erosion for realistic landforms
- **Geological Processes**: Plate tectonics simulation for mountain ranges and rifts
- **Scale Control**: Parameters for world size, vertical scale, and feature size
- **Underground Generation**: Cave systems, layer stratification, chasms, ancient structures
- **Water Systems**: Oceans, rivers, lakes, underground water, wetlands

#### 2. Biome System Requirements
- **Climate System**: Temperature and precipitation gradients
- **Biome Types**: Forests, grasslands, deserts, tundra, jungles, swamps
- **Transition Zones**: Natural blending between adjacent biomes
- **Altitude Effects**: How elevation affects biome characteristics
- **Seasonal Changes**: Biome transformation through seasonal cycles

#### 3. Resource Distribution Requirements
- **Mineral Deposits**: Ore veins following geological principles
- **Gem Clusters**: Rare valuable resources in specific geological contexts
- **Stone Types**: Different stone varieties with unique properties
- **Surface Resources**: Forest types, plant distribution, natural materials
- **Geological Accuracy**: Minerals in plausible formations and depths
- **Scarcity Balance**: Valuable resources challenging but possible to find

#### 4. World Features Requirements
- **Landmarks**: Natural wonders, unique geological formations
- **Ruins & Monuments**: Remnants of previous civilizations
- **Strategic Locations**: Natural chokepoints, defensible positions
- **Resource Hotspots**: Areas exceptionally rich in specific resources
- **Dynamic Features**: Quicksand, unstable ice, lava flows
- **Weather-Sensitive Areas**: Regions affected by storms, floods

#### 5. Historical Generation Requirements
- **World History Simulation**: Civilizations rise & fall, historical events
- **Faction Generation**: Culture types, relationship networks, territory claims
- **Artifact Creation**: Legendary items, ancient knowledge, cultural treasures
- **Archaeological Context**: How and where artifacts are discovered

### Configurable Historical Simulation

#### Simulation Depth Options:
1. **None**: Pure geological/biome generation only (fastest)
2. **Basic**: Major geological events, basic climate history  
3. **Standard**: Add civilization placement, basic territorial history
4. **Detailed**: Full historical simulation with events, wars, cultural development
5. **Epic**: Deep simulation with linguistic evolution, technological progression

#### Simulation Duration:
- **Short**: 100-500 years of simulated history
- **Medium**: 1,000-2,000 years (standard fantasy timespan)
- **Long**: 5,000-10,000 years (deep historical layers)
- **Epic**: 25,000+ years (geological time scales)

#### Parallel Region Processing:
- **Small Worlds**: Single-threaded regional simulation
- **Large Worlds**: Parallel simulation of distant regions
- **Overarching Events**: Climate changes, migrations affecting multiple regions
- **Regional Independence**: Local events contained to specific areas

#### Performance Considerations:
- **Time Budget**: User-configurable maximum generation time
- **Progress Feedback**: Real-time progress bars and stage descriptions
- **Interruption Support**: Ability to pause/resume generation for large worlds
- **Quality Fallbacks**: Reduce simulation complexity if time limits exceeded

#### 6. World Customization Requirements
- **Seed System**: Numeric seeds for world recreation
- **Customization Options**: Player-adjustable parameters
- **World Size Options**: From small areas to vast continents
- **Challenge Factors**: Resource abundance, climate harshness adjustments

#### 7. World Viewer Requirements
- **Initial Survey**: Tools for examining world before settlement
- **Resource Overlay**: Highlighting known or visible resources
- **Danger Assessment**: Information about hazardous areas
- **Settlement Suitability**: Analysis of promising colony locations
- **Site Selection**: Tools for choosing initial settlement area

#### 8. Advanced Systems Requirements
- **Climate System**: Weather patterns, seasonal cycles, extreme weather
- **Ecosystem Simulation**: Flora dynamics, fauna behavior, food chains
- **Natural Disasters**: Geological events, weather disasters, ecological crises
- **Procedural Structures**: Ruin layouts, settlement designs, dungeon systems
- **Cultural Content**: Language creation, art generation, historical documents

#### 9. World Persistence Requirements
- **Permanent Alterations**: How colony development changes the world
- **Environmental Response**: Long-term effects of resource extraction
- **Historical Recording**: Player actions becoming part of world history
- **World State Tracking**: Modified chunks, dynamic feature updates

#### 10. Technical Implementation Requirements
- **Noise Functions**: Layered noise, domain warping, feature-based generation
- **Chunking System**: Generation order, lazy generation, level of detail
- **Performance Optimization**: Parallel generation, caching, progressive detail
- **Data Structures**: Spatial organization, chunk storage, multi-layer data

### Integration with 08c Implementation

These design requirements will be distributed across the 08c subtasks as follows:

- **08c.1 (Core Seed System)**: Implements seed system requirements and foundation
- **08c.2 (Generation Parameters)**: Implements customization and parameter systems
- **08c.3 (Feature Parity)**: Ensures legacy compatibility while adding basic terrain features
- **08c.4 (Biome System)**: Implements climate, biome, and ecosystem requirements
- **08c.5 (Structure Generation)**: Implements ruins, landmarks, and procedural structures
- **08c.6 (Cave Generation)**: Implements underground systems and cave networks
- **08c.7 (Ore Generation)**: Implements resource distribution and geological accuracy
- **08c.8 (UI Integration)**: Implements world viewer and site selection tools
- **08c.9 (Performance)**: Implements technical optimization requirements
- **08c.10 (Legacy Transition)**: Manages transition and world persistence requirements
- **08c.6 (Cave Generation)**: Implements underground systems and cave networks
- **08c.7 (Ore Generation)**: Implements resource distribution and geological accuracy
- **08c.8 (UI Integration)**: Implements world viewer and site selection tools
- **08c.9 (Performance)**: Implements technical optimization requirements
- **08c.10 (Legacy Transition)**: Manages transition and world persistence requirements

### Testability Requirements for Each Feature

Each feature must include:
- **Visual Debug Overlays**: Real-time visualization of generation parameters
- **Parameter Tuning UI**: In-game controls for adjusting generation in real-time
- **Seed Comparison Tools**: Side-by-side comparison of different seeds
- **Feature-Specific Viewers**: Specialized tools for each generation component
- **Performance Monitoring**: Real-time generation performance metrics
- **User Feedback Systems**: Mechanisms for reporting generation issues with seed data

### Priority Implementation Order

1. **Foundation** (08c.1-08c.3): Core seed system, parameters, and basic terrain
2. **Environmental** (08c.4): Biome and climate systems
3. **Features** (08c.5-08c.7): Structures, caves, and resources
4. **Interface** (08c.8): World viewer and selection tools
5. **Optimization** (08c.9-08c.10): Performance and transition planning

---

## Objectives

1. **Feature Parity**: Ensure the new seed-based generation system provides all functionality of the legacy system
2. **Dual System Support**: Maintain "Resume Game" with legacy generation while implementing new system for "Load World"/"Create New World"
3. **Extensibility**: Design for future enhancements (biomes, structures, caves, etc.)
4. **Integration**: Properly integrate with the world persistence system planned in `08a_save_file_architecture.md`
5. **User Experience**: Provide a seamless experience for users regardless of which system they use
6. **Visual Testability**: Each component must be visually testable in-game with immediate feedback
7. **Incremental Development**: Enable testing of individual features before the entire system is complete

## Implementation Strategy

### Phase 1: Analysis and Documentation
- [ ] **Review Current Generation System**: Document existing functionality and requirements
- [ ] **Research Documentation**: Collect information on planned features from project documentation
- [ ] **Define Technical Requirements**: Create specifications for the new system
- [ ] **Integration Planning**: Define how this integrates with the Save/Load architecture
- [ ] **Testability Planning**: Design testing tools and visualization systems for all components

### Phase 2: Core World Generation System
- [ ] **Basic Implementation**: Build core seed-based generation functionality (see `08c.1_core_seed_system.md` for details)
- [ ] **Parameter System**: Create system for world generation parameters (see `08c.2_generation_parameters.md`)
- [ ] **Feature Parity**: Ensure all existing functionality works with new system (see `08c.3_feature_parity.md`)
- [ ] **Debug UI Components**: Implement basic visualization and testing tools for core system
- [ ] **Seed Testing Tools**: Create tools for comparing worlds with different seeds

### Phase 3: Extended Features
- [ ] **Biome System**: Implement biome-based world generation (see `08c.4_biome_system.md`)
- [ ] **Structure Generation**: Add support for structures (see `08c.5_structure_generation.md`)
- [ ] **Cave System**: Generate cave networks (see `08c.6_cave_generation.md`)
- [ ] **Ore Distribution**: Implement realistic ore placement (see `08c.7_ore_generation.md`)
- [ ] **Feature Visualization**: Add feature-specific visualization tools for testing each component
- [ ] **Parameter Tuning UI**: Create in-game controls for adjusting generation parameters in real-time

### Phase 4: Integration and Testing
- [ ] **UI Integration**: Connect world generation to UI elements (see `08c.8_ui_integration.md`)
- [ ] **Performance Testing**: Verify generation speed and optimize (see `08c.9_performance_optimization.md`)
- [ ] **Legacy System Transition**: Plan for full transition away from legacy system (see `08c.10_legacy_transition.md`)
- [ ] **Comprehensive Test Suite**: Finalize testing tools for the complete system
- [ ] **User Feedback Systems**: Add mechanisms for users to report generation issues with seed information

## Current Tasks

### Task 1: Document Current World Generation (COMPLETED)

### Legacy System Analysis

**Status**: ✅ COMPLETED
**Completion Date**: 2025-06-05 18:30

#### Core Files
- **Generator**: `engine/src/world/world_generator.cpp` + `engine/include/world/world_generator.h`
- **Noise**: `engine/include/util/noise.h`
- **Manager**: `engine/src/world/world_manager.cpp`
- **Chunk**: `engine/src/world/chunk_segment.cpp`

#### Current Generation Process

**1. Entry Point**
- `WorldGenerator::generateChunkSegment(ChunkSegment& segment, int worldX, int worldY, int worldZ)`
- Called by `WorldManager::updateActiveChunks()` when chunks are needed
- Thread-safe with mutex locks for generation

**2. Coordinate System**
- **World Coordinates**: `worldX`, `worldY`, `worldZ` are segment indices (not voxel coordinates)
- **Global Coordinates**: `globalX = worldX + x`, `globalZ = worldZ + z` for noise input
- **Chunk Dimensions**: 32x32x32 voxels per segment (`ChunkSegment::CHUNK_WIDTH/HEIGHT/DEPTH`)

**3. Noise-Based Height Generation**
```cpp
// Current parameters (in generateChunkSegment)
const float noiseInputScale = 0.02f;         // Horizontal frequency
const float terrainAmplitude = height * 1.5f; // Vertical scale (48 for 32-height)
const float baseTerrainOffset = height / 8.0f; // Base height offset (4)

// Height calculation
float nx = globalX * noiseInputScale;
float nz = globalZ * noiseInputScale;
float noise_val = VoxelEngine::Util::smoothValueNoise(nx, 0.0f, nz);
int columnHeight = static_cast<int>(noise_val * terrainAmplitude) + static_cast<int>(baseTerrainOffset);
```

**4. Voxel Placement Logic**
```cpp
if (globalY <= columnHeight) {
    if (globalY == columnHeight) {
        type = VoxelType::GRASS;           // Surface layer
    } else if (globalY > columnHeight - 3) {
        type = VoxelType::DIRT;            // Sub-surface (3 layers)
    } else {
        type = VoxelType::STONE;           // Deep layers
    }
} else {
    type = VoxelType::AIR;                 // Above surface
}
```

#### Current Noise System

**Noise Implementation** (`engine/include/util/noise.h`):
- Custom value noise with trilinear interpolation
- Hash function: `hash(x,y,z) = (x*374761393 + y*668265263 + z*2147483647) ^ operations`
- Range: [0,1] output from `smoothValueNoise()`
- 3D interpolation between 8 corner values

**Characteristics**:
- Simple but effective for basic terrain
- No multi-octave/fractal noise
- No domain warping or advanced features
- Fixed Y=0 for 2D heightmap generation

#### Seed System

**Current Implementation**:
- `WorldGenerator` has `worldSeed_` (uint64_t) and `rng_` (std::mt19937_64)
- `setSeed(uint64_t)` and `setSeedFromString(string)` methods available
- Per-block seeds: `getBlockSeed(x,y,z)` combines position hash with world seed
- Static fallback: `staticGetBlockSeed()` uses default seed when no instance available

**Issues**:
- Noise system doesn't use the world seed (uses deterministic hash only)
- No biome-based seed variation
- Limited parameterization

#### Performance Characteristics

**Generation Speed**: Fast - simple noise lookup per voxel column
**Memory Usage**: Generates chunk segments on-demand (32³ voxels)
**Thread Safety**: Mutex-protected in `WorldManager::updateActiveChunks()`

**Logging/Debug**:
- Extensive debug logging with timestamps
- Height value logging for first 20 terrain columns
- Coordinate mapping verification logs

#### Current Limitations

1. **Single Biome**: No biome system - uniform grass/dirt/stone everywhere
2. **Basic Noise**: Simple value noise, no fractal/multi-octave complexity
3. **No Structures**: No villages, dungeons, or generated features
4. **No Caves**: Solid terrain below surface
5. **No Water**: No oceans, rivers, or lakes
6. **Fixed Parameters**: No world-type variation or customization
7. **Heightmap Only**: Pure 2D heightmap, no 3D features

#### Integration Points

**With WorldManager**:
- Called from `updateActiveChunks()` when segments need generation
- Triggered when player movement requires new chunks
- Uses `ChunkSegment::setGenerated(true)` to mark completion

**With Save System**:
- Generated chunks can be saved/loaded via current SaveManager system
- `Resume Game` preserves legacy-generated terrain
- New chunks use same generator when loading existing worlds

#### Benchmarking Results

**Generation Rate**: ~1-2ms per 32³ chunk segment (measured via debug logs)
**Coordinate Range**: Tested working for segments (-2,-2,-2) to (2,2,2)
**Terrain Quality**: Produces rolling hills with reasonable variation

**Sample Output**:
- Height range: ~4-52 (with current parameters)
- Noise variation: Smooth transitions, no sharp edges
- Block distribution: Appropriate grass/dirt/stone layers

### Next Steps for New System

Based on this analysis, the new system should:

1. **Preserve Simplicity**: Keep the basic noise → height → voxel placement flow
2. **Add Extensibility**: Modular biome, structure, and feature systems
3. **Enhance Noise**: Multi-octave, domain-warped, seed-integrated noise
4. **Maintain Performance**: Similar generation speeds with more features
5. **Ensure Compatibility**: Legacy worlds continue working via "Resume Game"

---

### Task 2: Design Core Seed System (IN PROGRESS)
