# Subtask 3: Hybrid Detail Generator

**Priority:** Core Feature
**Estimated Time:** 6-7 days
**Dependencies:** ContinuousField, Geological Simulation Engine

## Overview

Implement the ultra-realistic chunk generation system that creates detailed 25cm-scale geological features from 32m simulation data using hybrid geological modeling.

## Technical Requirements

### Core Detail Generator Architecture
```cpp
class HybridDetailGenerator {
private:
    GeologicalConfig config;
    std::unique_ptr<PerlinNoiseGenerator> noiseGen;
    std::unique_ptr<MicroPhysicsSimulator> microSim;
    
public:
    // Main chunk generation entry point
    VoxelChunk generateChunkDetail(int chunkX, int chunkZ, 
                                  const GeologicalSample& geoSample);
    
    // Five-phase detail generation pipeline
    RockFormation generateRockFormation(RockType rockType, 
                                       int chunkX, int chunkZ);
    MicroGeologyResult simulateMicroGeology(const RockFormation& base, 
                                           const GeologicalSample& sample);
    FractalDetail generateFractalDetail(const MicroGeologyResult& microGeo, 
                                       float roughness);
    ErosionDetail simulateChunkErosion(const FractalDetail& fractal, 
                                      float waterFlow);
    VoxelChunk combineDetailLayers(const RockFormation& rock,
                                  const MicroGeologyResult& micro,
                                  const FractalDetail& fractal,
                                  const ErosionDetail& erosion);
};
```

### Ultra-Realistic Geological Features

#### Granite Chunk Features
- **Joint Systems:** Natural fracture patterns every 1-3 meters
- **Weathering Spheroids:** Rounded granite boulders from chemical weathering
- **Exfoliation Sheets:** Curved fracture planes parallel to surface
- **Quartz Veins:** White mineral veins cutting through granite
- **Scree Slopes:** Angular rock fragments at cliff bases

#### Limestone Chunk Features
- **Solution Pits:** Small dissolved depressions (0.5-2m diameter)
- **Karst Features:** Cave entrances, sinkholes, underground streams
- **Bedding Planes:** Horizontal rock layers visible in cliff faces
- **Flowstone Formations:** Smooth calcium carbonate deposits
- **Underground Caverns:** Multi-level cave systems

#### Sandstone Chunk Features
- **Cross-Bedding:** Diagonal layering patterns from ancient dunes
- **Alcoves and Arches:** Natural erosion features
- **Desert Varnish:** Dark mineral coating on exposed surfaces
- **Slot Canyons:** Narrow, deep erosion channels
- **Hoodoos:** Tall, thin spires of resistant rock

#### Volcanic Chunk Features
- **Columnar Jointing:** Hexagonal rock columns from cooling lava
- **Lava Tubes:** Underground tunnels from flowing lava
- **Volcanic Bombs:** Large rounded rocks ejected from eruptions
- **Pyroclastic Layers:** Distinct ash and pumice layers
- **Thermal Features:** Hot springs, fumaroles, geysers

## Implementation Steps

### Phase 1: Rock Formation Generation (Day 1-2)
- [ ] Implement geological type-specific formation algorithms
- [ ] Create granite joint system generation
- [ ] Add limestone bedding plane simulation
- [ ] Implement sandstone cross-bedding patterns
- [ ] Add volcanic columnar jointing

### Phase 2: Micro-Physics Simulation (Day 3-4)
- [ ] Create micro-weathering simulation system
- [ ] Implement micro-erosion along water flow paths
- [ ] Add sediment transport and deposition
- [ ] Create jointing and fracturing simulation
- [ ] Implement stress-based feature formation

### Phase 3: Fractal Detail Enhancement (Day 5)
- [ ] Add multi-octave Perlin noise system
- [ ] Create geological-type-specific noise parameters
- [ ] Implement seamless noise across chunk boundaries
- [ ] Add surface roughness variation

### Phase 4: Erosion Detail Simulation (Day 6)
- [ ] Implement chunk-scale water flow modeling
- [ ] Create erosion pattern algorithms
- [ ] Add overhang and alcove formation
- [ ] Simulate cave system generation
- [ ] Create scree slope formation

### Phase 5: Integration & Optimization (Day 7)
- [ ] Implement layer combination algorithms
- [ ] Optimize chunk generation performance
- [ ] Add quality preset implementations
- [ ] Performance profiling and final optimization

## Detailed Implementation Examples

### Granite Formation Generation
```cpp
RockFormation generateGraniteFormation(int chunkX, int chunkZ) {
    RockFormation formation;
    
    // Generate realistic granite jointing patterns
    generateJointSystems(formation, chunkX, chunkZ);
    
    // Add weathering spheroids based on climate exposure
    generateWeatheringFeatures(formation, getClimateData(chunkX, chunkZ));
    
    // Create quartz vein networks based on geological stress
    generateQuartzVeins(formation, getStressField(chunkX, chunkZ));
    
    return formation;
}

void generateJointSystems(RockFormation& formation, int chunkX, int chunkZ) {
    // Granite forms joint systems perpendicular to cooling surfaces
    float primaryJointSpacing = 2.0f + perlinNoise(chunkX, chunkZ) * 1.0f; // 2-3m
    float secondaryJointSpacing = 1.0f + perlinNoise(chunkX*2, chunkZ*2) * 0.5f; // 1-1.5m
    
    // Create orthogonal joint sets
    addJointSet(formation, 0.0f, primaryJointSpacing);      // North-South joints
    addJointSet(formation, 90.0f, secondaryJointSpacing);   // East-West joints
    addJointSet(formation, 45.0f, primaryJointSpacing*1.5f); // Diagonal stress joints
}
```

### Limestone Cave Generation
```cpp
void generateCaveSystems(VoxelChunk& chunk, RockType rockType) {
    if (rockType == LIMESTONE) {
        // Limestone dissolves to create cave systems
        for (each water flow zone) {
            if (hasAcidicWater(zone)) {
                // Follow bedding planes for cave development
                followBeddingPlanes(zone, cave_direction);
                
                // Create phreatic (water-filled) cave passages
                if (belowWaterTable(zone)) {
                    createPhreaticPassage(zone, waterFlow);
                }
                
                // Create vadose (air-filled) canyons above water table
                if (aboveWaterTable(zone)) {
                    createVadoseCanyon(zone, waterFlow);
                }
                
                // Add speleothems (stalactites/stalagmites)
                addSpeleothems(zone, age, waterChemistry);
            }
        }
    }
}
```

### Micro-Physics Simulation
```cpp
void simulateMicroWeathering(MicroGeologyResult& result, float hardness, int timeStep) {
    // Chemical weathering - affects softer minerals first
    for (each voxel in chunk) {
        if (voxel.mineralType == FELDSPAR) {
            // Feldspar weathers to clay faster than quartz
            float weatheringRate = getWeatheringRate(voxel, climate);
            voxel.hardness -= weatheringRate * timeStep;
            
            // Create clay alteration zones
            if (voxel.hardness < weatheringThreshold) {
                convertToClayMinerals(voxel);
            }
        }
    }
}

void simulateMicroErosion(MicroGeologyResult& result, float waterFlow, int timeStep) {
    // Water erosion - follows flow paths and removes loose material
    for (each voxel in waterFlow path) {
        if (voxel.isLoose() && waterFlow > erosionThreshold) {
            // Calculate erosion based on water velocity and rock resistance
            float erosionRate = waterFlow * (1.0f / voxel.resistance);
            
            removeVoxel(voxel);
            addSedimentToFlow(voxel.material, waterFlow);
            
            // Create undercuts and overhangs
            if (hasWeakUnderlayer(voxel)) {
                createOverhang(voxel, erosionRate);
            }
        }
    }
}
```

## Files to Create/Modify

### New Files
- `engine/include/world/HybridDetailGenerator.h`
- `engine/src/world/HybridDetailGenerator.cpp`
- `engine/include/world/geological/RockFormation.h`
- `engine/src/world/geological/GraniteFormation.cpp`
- `engine/src/world/geological/LimestoneFormation.cpp`
- `engine/src/world/geological/SandstoneFormation.cpp`
- `engine/src/world/geological/VolcanicFormation.cpp`
- `engine/src/world/geological/MicroPhysicsSimulator.cpp`
- `engine/src/world/geological/ErosionDetailSimulator.cpp`

### Supporting Files
- `engine/include/world/VoxelChunk.h`
- `engine/include/world/GeologicalSample.h`
- `tests/integration/ChunkGenerationTest.cpp`

## Quality Preset Variations

### Performance Preset (~5ms per chunk)
- Simplified rock formation algorithms
- Basic fractal noise only
- No micro-physics simulation
- Simple erosion patterns

### Balanced Preset (~15ms per chunk) - Default
- Moderate geological detail
- Basic micro-physics simulation
- Simplified cave generation
- Realistic surface features

### Quality Preset (~50ms per chunk)
- Detailed geological features
- Full micro-physics simulation
- Complex cave systems
- Advanced erosion modeling

### Ultra-Realism Preset (~200ms per chunk)
- Research-grade geological accuracy
- 1000-step micro-physics simulation
- Complex multi-level cave systems
- All geological processes enabled

## Performance Optimization Strategies

### Memory Optimization
- Stream geological templates from disk
- Use object pooling for temporary calculations
- Compress intermediate results

### Computation Optimization
- Vectorized noise generation
- Parallel micro-physics simulation
- Cache frequently used geological patterns

### Quality Scaling
- Reduce simulation steps for lower quality
- Simplify geological processes
- Use pre-computed templates when possible

## Testing Strategy

### Unit Tests
- Individual rock formation algorithms
- Micro-physics simulation accuracy
- Fractal noise generation quality

### Visual Validation
- Export chunk visualizations
- Compare with real geological photos
- Verify realistic feature formation

### Performance Testing
- Chunk generation timing
- Memory usage monitoring
- Quality preset validation

## Success Criteria

- [ ] Ultra-realistic geological features at 25cm scale
- [ ] Minecraft-level familiarity with higher realism
- [ ] Distinct appearance for different rock types
- [ ] Natural overhangs and cave systems
- [ ] Performance targets met for all quality presets
- [ ] Seamless integration with chunk streaming

## Performance Targets

- **Balanced Quality:** ~15ms per chunk generation
- **Memory Usage:** < 32MB per chunk during generation
- **Feature Accuracy:** Geologically realistic at 25cm scale
- **Boundary Seamlessness:** No visible chunk edges

## Notes

This system generates the actual terrain players will explore. Focus on visual quality and geological accuracy while maintaining reasonable performance. Each rock type should have a distinctly different appearance and behavior.
