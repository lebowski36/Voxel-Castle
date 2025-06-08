# Biome System Design Document

*Created: 2025-06-08*
*Last Updated: 2025-06-08*

## Overview

This document outlines the design decisions and architecture for Voxel Castle's biome system. The biome system will leverage the newly completed 257-block unified resource system to create diverse, believable ecosystems across the game world.

## Core Design Decisions

### 1. Biome Storage Strategy: Hybrid Approach

**Decision: Hybrid Strategy - Persistent Regional Biome Data + On-Demand Detail Generation**

This approach matches our existing two-tier world generation architecture:
1. **Regional biome data** (temperature, humidity, biome type) will be pre-generated and stored persistently
2. **Chunk-level details** will be generated on-demand using the regional data as constraints

**Pros:**
- Consistent with our existing two-tier generation philosophy
- Enables massive worlds (up to 25,000,000km²) with manageable storage
- Quick access to biome data for any world location
- Supports historical simulation

**Cons:**
- Requires regional biome data format design
- Slightly more complex than pure on-demand generation
- Initial generation time for large worlds

**Implementation Details:**
- Store lightweight biome metadata (type, temperature, humidity) in regional database
- Generate detailed block placement, microclimate effects, and transitions at runtime
- Cache recently accessed biome data for performance

### 2. Biome Transition Approach: Gradient-Based Transitions with Special Ecotones

**Decision: Gradient-Based Transitions with Transition Zone Enhancement**

This hybrid approach creates smooth transitions naturally while also defining special transition zones for certain biome pairs:

1. **Base Layer**: Use gradient blending of temperature/humidity
2. **Enhanced Layer**: Define special transition biomes for certain biome pairs (e.g., forest-desert becomes savanna)

**Pros:**
- Creates truly seamless transitions as specified in our design
- Avoids harsh terrain changes between biomes
- Still allows for unique ecotone biomes in transition areas
- Performs well with large-scale world plans

**Cons:**
- More complex than simple hard biome boundaries
- Requires careful tuning of transition parameters

**Implementation Details:**
- Use distance fields to detect proximity to biome boundaries
- Apply gradual terrain blending proportional to boundary proximity
- Introduce special transition blocks near boundaries (e.g., sparse trees between forest and plains)
- Define specific ecotone biomes for certain biome pairs (savanna between forest and desert)

### 3. Visualization Integration: Toggle-able Debug Overlays

**Decision: Integrated Toggle-able Debug Visualization System**

Biome visualization tools will be integrated directly into the gameplay but will be toggle-able via debug commands.

**Implementation Details:**
- Add toggle-able debug visualizations accessible via console commands/debug menu
- Include color-coded biome overlay, temperature/humidity heatmaps
- Implement biome inspector for clicking to see detailed biome data
- Make all visualization features accessible during normal gameplay

### 4. Initial Biome Count: Focus on Core Five

**Decision: Start with 5 Core Biomes**

We'll implement a focused initial set of biomes to ensure the system works properly before expanding:

1. **Plains**: Grassland with occasional trees
2. **Forest**: Dense tree coverage with varied heights
3. **Desert**: Sandy terrain with cacti
4. **Mountains**: High elevation with stone outcrops
5. **Ocean**: Water bodies with underwater features and depth zones

These provide good environmental variety and testing of various biome features while keeping implementation manageable.

### 5. Noise Algorithm: Extend Existing EnhancedNoise System

**Decision: Extend Existing EnhancedNoise System**

The current system is well-designed for our needs. For biome generation specifically:
1. Use the **octaveNoise** method for smooth climate gradients
2. Apply **ridgedNoise** for mountain biome boundaries
3. Use **warpedNoise** for natural-looking biome transitions

This leverages existing code and maintains consistency with our architecture.

## Technical Implementation Plan

### 1. Core Components

#### BiomeType Enum
```cpp
enum class BiomeType {
    PLAINS,
    FOREST,
    DESERT,
    MOUNTAINS,
    OCEAN,
    // Future expansion:
    TUNDRA,
    JUNGLE,
    SAVANNA,
    WETLANDS,
    VOLCANIC
};
```

#### BiomeData Structure
```cpp
struct BiomeData {
    BiomeType type;
    float baseTemperature;
    float baseHumidity;
    float terrainRoughness;
    float terrainScale;
    int baseHeight;
    int heightVariation;
    // Block types for this biome
    BlockType surfaceBlock;
    BlockType subsurfaceBlock;
    BlockType deepBlock;
    // Special feature blocks
    BlockType featureBlocks[4];
    // Temperature/humidity transition thresholds
    float minTemperature;
    float maxTemperature;
    float minHumidity;
    float maxHumidity;
};
```

#### BiomeRegistry
Central repository for all biome data and characteristics:
```cpp
class BiomeRegistry {
public:
    static void initialize();
    static BiomeData getBiomeData(BiomeType type);
    static BiomeType getBiomeFromTemperatureHumidity(float temperature, float humidity);
    // ...
};
```

### 2. Two-Tier System Components

#### RegionalBiomeGenerator
```cpp
class RegionalBiomeGenerator {
public:
    RegionalBiomeGenerator(const WorldSeed& seed, const WorldParameters& params);
    
    BiomeType getBiomeType(int64_t regionX, int64_t regionZ);
    float getTemperature(int64_t regionX, int64_t regionZ);
    float getHumidity(int64_t regionX, int64_t regionZ);
    
    void generateRegionalBiomes(int64_t startX, int64_t startZ, int64_t width, int64_t height);
    void saveRegionalData(const std::string& filename);
    void loadRegionalData(const std::string& filename);
    
private:
    const WorldSeed& worldSeed_;
    const WorldParameters& worldParams_;
    EnhancedNoise climateNoise_;
    
    // Climate mapping functions
    float calculateBaseTemperature(int64_t x, int64_t z);
    float calculateBaseHumidity(int64_t x, int64_t z);
    
    // Regional data cache
    std::unordered_map<int64_t, std::unordered_map<int64_t, BiomeType>> biomeCache_;
    std::unordered_map<int64_t, std::unordered_map<int64_t, float>> temperatureCache_;
    std::unordered_map<int64_t, std::unordered_map<int64_t, float>> humidityCache_;
};
```

#### ChunkBiomeProcessor
```cpp
class ChunkBiomeProcessor {
public:
    ChunkBiomeProcessor(const RegionalBiomeGenerator& regionalGenerator);
    
    void processChunk(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    
    // Biome-specific generation methods
    void applyPlainsGeneration(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    void applyForestGeneration(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    void applyDesertGeneration(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    void applyMountainsGeneration(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    void applyOceanGeneration(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    
    // Specialized feature placement
    void placeVegetation(ChunkSegment& segment, int worldX, int worldY, int worldZ, BiomeType biomeType);
    void createTransitions(ChunkSegment& segment, int worldX, int worldY, int worldZ);
    
private:
    const RegionalBiomeGenerator& regionalGenerator_;
    
    // Local chunk-level processing
    int getAdjustedHeight(int baseHeight, float localNoise, BiomeType biomeType);
    BiomeType getTransitionBiome(BiomeType primary, BiomeType secondary);
    float getBiomeBlendFactor(int x, int z, BiomeType primary, BiomeType secondary);
};
```

#### BiomeVisualizer
```cpp
class BiomeVisualizer {
public:
    BiomeVisualizer(const RegionalBiomeGenerator& regionalGenerator);
    
    void renderBiomeOverlay();
    void renderTemperatureMap();
    void renderHumidityMap();
    void renderTransitionZones();
    
    void showBiomeInfo(int x, int z);
    void toggleVisualizationMode(VisualizationMode mode);
    
private:
    const RegionalBiomeGenerator& regionalGenerator_;
    VisualizationMode currentMode_;
    bool isVisible_;
    
    glm::vec3 getBiomeColor(BiomeType type);
    glm::vec3 getTemperatureColor(float temperature);
    glm::vec3 getHumidityColor(float humidity);
};
```

### 3. UI Integration

We'll integrate with the WorldCreationDialog to allow biome customization:

```cpp
// Added to WorldCreationDialog
void WorldCreationDialog::addBiomeSection() {
    // Biome density options (more forests, more desert, etc.)
    addSectionHeader("Biome Settings");
    
    // Add biome density sliders
    forestDensitySlider_ = createStyledSlider("Forest Density", getNextElementY(), 0, 100, 50);
    desertDensitySlider_ = createStyledSlider("Desert Size", getNextElementY(), 0, 100, 50);
    mountainDensitySlider_ = createStyledSlider("Mountain Coverage", getNextElementY(), 0, 100, 50);
    
    // Add terrain scale slider (how varied the terrain is)
    terrainScaleSlider_ = createStyledSlider("Terrain Variation", getNextElementY(), 0, 100, 50);
}
```

### 4. Block Assignments for Biomes

Each biome will use specific blocks from our 257-block palette:

#### Plains Biome
- Surface: Grass Block
- Subsurface: Dirt Block
- Deep: Stone Block
- Features: Tall Grass, Flowers, Occasional Oak Trees

#### Forest Biome
- Surface: Grass Block
- Subsurface: Dirt Block
- Deep: Stone Block
- Features: Oak Trees, Birch Trees, Pine Trees, Bushes

#### Desert Biome
- Surface: Sand Block
- Subsurface: Sandstone Block
- Deep: Stone Block
- Features: Cacti, Dead Bush, Sandstone Formations

#### Mountains Biome
- Surface: Stone Block, Mountain Stone Block
- Subsurface: Stone Block
- Deep: Deepslate Block
- Features: Ore Deposits, Snow (at high elevation), Gravel

#### Ocean Biome
- Surface: Water Block
- Subsurface: Sand Block, Clay Block
- Deep: Stone Block
- Features: Seagrass, Coral, Underwater Caves

## Next Steps and Implementation Timeline

### Phase 1: Core Framework (First Week)
1. Implement BiomeType enum and BiomeData structs
2. Create BiomeRegistry with initial 5 biomes
3. Implement RegionalBiomeGenerator core functionality
4. Add temperature/humidity generation

### Phase 2: Integration (Second Week)
1. Connect to SeedWorldGenerator
2. Implement ChunkBiomeProcessor
3. Create basic visualization tools
4. Add UI controls in WorldCreationDialog

### Phase 3: Biome-Specific Features (Third Week)
1. Implement block selection for each biome
2. Create height modification based on biome
3. Add biome-specific features (vegetation, etc.)
4. Implement transition blending

### Phase 4: Testing and Refinement (Fourth Week)
1. Add detailed visualization and analysis tools
2. Test with various seeds and world sizes
3. Fine-tune biome parameters and transitions
4. Document the completed system

## References

- [08c.4_biome_system.md](/home/system-x1/Projects/Voxel Castle/development_tasks/04_essential_game_infrastructure/08c.4_biome_system.md) - Original task specification
- [08c_world_generation_system.md](/home/system-x1/Projects/Voxel Castle/development_tasks/04_essential_game_infrastructure/08c_world_generation_system.md) - World Generation System overview
- [World Generation.md](/home/system-x1/Projects/Voxel Castle/docs/design_specifications/Game%20Concept/World%20Generation.md) - Game concept design specification
