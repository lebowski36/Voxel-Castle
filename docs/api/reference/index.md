# API Reference Index

## Quick Navigation

### 🌍 World Generation
- **[ContinuousField<T>](../world_generation/continuous_field.md)** - Smooth field interpolation and geological data
- **[GeologicalSimulator](../world_generation/geological_simulator.md)** - Advanced terrain generation
- **[SeedWorldGenerator](../world_generation/seed_world_generator.md)** - Seed-based world creation
- **[TectonicSimulator](../world_generation/tectonic_simulator.md)** - Plate tectonics simulation

### 🎮 Core Engine
- **[WorldSeed](../engine_core/world_seed.md)** - Deterministic random number generation
- **[WorldParameters](../engine_core/world_parameters.md)** - World configuration management
- **[ChunkSegment](../engine_core/chunk_segment.md)** - Voxel data containers
- **[RegionalDatabase](../engine_core/regional_database.md)** - Large world data storage

### 🎨 User Interface
- **[WorldConfigurationUI](../ui/world_configuration_ui.md)** - World creation interface
- **[WorldSimulationUI](../ui/world_simulation_ui.md)** - Generation progress visualization
- **[WorldMapRenderer](../ui/world_map_renderer.md)** - 2D world visualization

### 🔧 Utilities
- **[PerformanceBenchmark](../utilities/performance_benchmark.md)** - Performance testing framework
- **[NoiseGeneration](../utilities/noise_generation.md)** - Procedural noise functions
- **[BiomeSystem](../utilities/biome_system.md)** - Climate and biome management

## Complete Alphabetical Index

### A
- `addToSample()` - [ContinuousField](../world_generation/continuous_field.md#addtosample)

### B
- `BiomeType` - [BiomeSystem](../utilities/biome_system.md#biometype)
- `BenchmarkResults` - [PerformanceBenchmark](../utilities/performance_benchmark.md)

### C
- `clear()` - [ContinuousField](../world_generation/continuous_field.md#clear)
- `ContinuousField<T>` - [ContinuousField](../world_generation/continuous_field.md)
- `ContinuousFieldBenchmark` - [PerformanceBenchmark](../utilities/performance_benchmark.md)

### D
- `detectWorldType()` - [WorldGenerator](../world_generation/world_generator.md)

### E
- `exportGeologicalData()` - [GeologicalSimulator](../world_generation/geological_simulator.md)

### F
- `fill()` - [ContinuousField](../world_generation/continuous_field.md#fill)

### G
- `generateChunkSegment()` - [SeedWorldGenerator](../world_generation/seed_world_generator.md)
- `generateWorldMap()` - [WorldMapRenderer](../ui/world_map_renderer.md)
- `GeologicalConfig` - [GeologicalSimulator](../world_generation/geological_simulator.md)
- `GeologicalSimulator` - [GeologicalSimulator](../world_generation/geological_simulator.md)
- `getSample()` - [ContinuousField](../world_generation/continuous_field.md#getsample)
- `getSampleAt()` - [ContinuousField](../world_generation/continuous_field.md#sampleat)
- `getToroidalDistance()` - [ContinuousField](../world_generation/continuous_field.md#gettoroidaldistance)

### H
- `HeightMap` - [WorldGenerator](../world_generation/world_generator.md)

### I
- `initialize()` - [GeologicalSimulator](../world_generation/geological_simulator.md)
- `initializeGeologicalSimulation()` - [SeedWorldGenerator](../world_generation/seed_world_generator.md)

### L
- `loadChunk()` - [ChunkManager](../engine_core/chunk_manager.md)

### M
- `MineralType` - [GeologicalData](../world_generation/geological_data.md)

### P
- `PhaseInfo` - [GeologicalSimulator](../world_generation/geological_simulator.md)
- `Point` - [ContinuousField](../world_generation/continuous_field.md#point)
- `propagateValue()` - [ContinuousField](../world_generation/continuous_field.md#propagatevalue)

### R
- `RegionalData` - [RegionalDatabase](../engine_core/regional_database.md)
- `RockType` - [GeologicalData](../world_generation/geological_data.md)
- `runFullSimulation()` - [GeologicalSimulator](../world_generation/geological_simulator.md)

### S
- `sampleAt()` - [ContinuousField](../world_generation/continuous_field.md#sampleat)
- `SeedWorldGenerator` - [SeedWorldGenerator](../world_generation/seed_world_generator.md)
- `setSample()` - [ContinuousField](../world_generation/continuous_field.md#setsample)
- `simulateErosionPhase()` - [GeologicalSimulator](../world_generation/geological_simulator.md)
- `simulateTectonicPhase()` - [GeologicalSimulator](../world_generation/geological_simulator.md)

### T
- `TectonicSimulator` - [TectonicSimulator](../world_generation/tectonic_simulator.md)

### W
- `WorldConfigurationUI` - [WorldConfigurationUI](../ui/world_configuration_ui.md)
- `WorldGenerator` - [WorldGenerator](../world_generation/world_generator.md)
- `WorldMapRenderer` - [WorldMapRenderer](../ui/world_map_renderer.md)
- `WorldParameters` - [WorldParameters](../engine_core/world_parameters.md)
- `WorldSeed` - [WorldSeed](../engine_core/world_seed.md)
- `WorldSimulationUI` - [WorldSimulationUI](../ui/world_simulation_ui.md)
- `wrapCoordinates()` - [ContinuousField](../world_generation/continuous_field.md#wrapcoordinates)

## By Category

### Data Structures
- `ContinuousField<T>` - Core spatial data container
- `Point` - 2D coordinate structure
- `GeologicalConfig` - Simulation configuration
- `PhaseInfo` - Progress tracking data
- `RegionalData` - Large-scale world data

### Enumerations
- `RockType` - Types of geological materials
- `MineralType` - Mineral and ore types
- `BiomeType` - Climate zones and biomes
- `GeologicalPhase` - Simulation phases
- `GeologicalPreset` - Quality presets

### Core Classes
- `GeologicalSimulator` - Main geological simulation
- `SeedWorldGenerator` - World generation orchestrator
- `TectonicSimulator` - Plate tectonics (legacy)
- `WorldMapRenderer` - 2D visualization
- `PerformanceBenchmark` - Testing framework

### User Interface
- `WorldConfigurationUI` - World creation parameters
- `WorldSimulationUI` - Generation progress display
- `BaseMenu` - UI framework base class

### Utilities
- `WorldSeed` - Random number generation
- `NoiseGeneration` - Procedural functions
- `PerformanceTracker` - Monitoring tools

## Performance Reference

### Time Complexity
| Operation | Complexity | Target Time |
|-----------|------------|-------------|
| `sampleAt()` | O(1) | < 1ms |
| `setSample()` | O(1) | < 0.1μs |
| `propagateValue()` | O(range²) | < 10ms (1km) |
| `fill()` | O(n²) | Depends on size |
| `wrapCoordinates()` | O(1) | < 0.1μs |

### Memory Usage
| Field Size | Float Field | Memory |
|------------|-------------|--------|
| 512×512 | 1MB | ✅ Efficient |
| 1024×1024 | 4MB | ✅ Good |
| 2048×2048 | 17MB | ⚠️ Large |
| 4096×4096 | 67MB | ❌ Very Large |

### Thread Safety
| Operation | Thread Safe | Notes |
|-----------|-------------|-------|
| `sampleAt()` | ✅ Yes | Read-only |
| `getSample()` | ✅ Yes | Read-only |
| `setSample()` | ❌ No | Requires synchronization |
| `propagateValue()` | ❌ No | Modifies multiple samples |

## Common Patterns

### Initialization
```cpp
// Create field
auto field = std::make_unique<ContinuousField<float>>(1024, 1024, 1000.0f);

// Initialize with data
field->fill(0.0f);
for (int z = 0; z < 1024; ++z) {
    for (int x = 0; x < 1024; ++x) {
        float value = calculateInitialValue(x, z);
        field->setSample(x, z, value);
    }
}
```

### Sampling
```cpp
// Single sample
float height = elevationField->sampleAt(worldX, worldZ);

// Batch sampling
std::vector<float> heights;
for (const auto& position : positions) {
    heights.push_back(elevationField->sampleAt(position.x, position.z));
}
```

### Force Application
```cpp
// Define resistance
auto rockResistance = [&](float x, float z) -> float {
    RockType rock = rockField->sampleAt(x, z);
    return getRockHardness(rock);
};

// Apply tectonic force
elevationField->propagateValue(1000.0f, faultX, faultZ, 2000.0f, rockResistance);
```

### Performance Monitoring
```cpp
#include "world/ContinuousFieldBenchmark.h"

// Run benchmarks
VoxelCastle::World::Performance::runContinuousFieldBenchmarks();
```

---

## See Also

- **[Getting Started Guide](../tutorials/getting_started.md)** - First steps with the API
- **[Performance Optimization](../tutorials/performance_optimization.md)** - Speed and memory tips
- **[Best Practices](../tutorials/best_practices.md)** - Recommended patterns
- **[Migration Guide](../migration/README.md)** - Upgrading from previous versions
