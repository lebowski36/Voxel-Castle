# Subtask 2: Geological Simulation Engine

**Priority:** Core System
**Estimated Time:** 5-6 days
**Dependencies:** ContinuousField Implementation

## Overview

Implement the three-phase geological simulation system that transforms continental-scale geological processes into realistic terrain over multiple timescales.

## Technical Requirements

### Core Simulation Architecture
```cpp
class GeologicalSimulator {
private:
    // Continuous geological fields
    ContinuousField<float> mantleStress;
    ContinuousField<float> crustStress;
    ContinuousField<float> elevationField;
    ContinuousField<float> rockHardness;
    ContinuousField<float> waterFlow;
    ContinuousField<RockType> rockTypes;
    
    GeologicalConfig config;
    
public:
    // Three-phase simulation pipeline
    void simulateTectonicPhase(float durationMillionYears);
    void simulateErosionPhase(float durationThousandYears);
    void simulateDetailPhase(float durationHundredYears);
    
    // Visual progress callbacks
    void setProgressCallback(std::function<void(PhaseInfo)> callback);
    
    // Data export for chunk generation
    GeologicalSample getSampleAt(float x, float z) const;
};
```

### Three-Phase Timeline System

#### Phase 1: Continental Tectonics (Millions of Years)
- **Duration:** 2 minutes, 30 visual updates
- **Processes:** Mantle convection, plate movement, mountain building
- **Visual Updates:** Every 100,000 simulated years (4 seconds real-time)

#### Phase 2: Regional Erosion (Thousands of Years)  
- **Duration:** 2 minutes, 20 visual updates
- **Processes:** Weathering, river systems, glacial carving
- **Visual Updates:** Every 1,000 simulated years (6 seconds real-time)

#### Phase 3: Local Detail (Hundreds of Years)
- **Duration:** 1 minute, 10 visual updates
- **Processes:** Micro-weathering, sediment deposition, vegetation effects
- **Visual Updates:** Every 100 simulated years (6 seconds real-time)

## Implementation Steps

### Phase 1: Core Structure (Day 1)
- [ ] Create GeologicalSimulator class
- [ ] Initialize all ContinuousField instances
- [ ] Implement basic phase progression framework
- [ ] Add progress callback system

### Phase 2: Tectonic Simulation (Day 2)
- [ ] Implement mantle convection simulation
- [ ] Add plate stress propagation
- [ ] Create mountain building algorithms
- [ ] Test continental-scale deformation

### Phase 3: Erosion Simulation (Day 3)
- [ ] Implement chemical weathering processes
- [ ] Add physical erosion algorithms
- [ ] Create river system formation
- [ ] Add glacial carving (optional)

### Phase 4: Detail Simulation (Day 4)
- [ ] Implement micro-weathering processes
- [ ] Add sediment transport and deposition
- [ ] Create local water flow patterns
- [ ] Add vegetation weathering effects

### Phase 5: Integration & Optimization (Day 5-6)
- [ ] Optimize phase transitions and data flow
- [ ] Add geological sample export system
- [ ] Implement quality presets (Performance/Balanced/Quality/Ultra)
- [ ] Performance profiling and optimization

## Geological Process Details

### Mantle Convection
```cpp
void simulateMantleConvection(float timeStep) {
    for (each point in mantleField) {
        // Deep Earth heat flow creates convection cells
        float heatFlow = calculateHeatFlow(point);
        float convectionForce = heatFlow * convectionStrength;
        
        // Propagate stress to surface through geological resistance
        propagateStressToSurface(point, convectionForce);
    }
}
```

### Mountain Building
```cpp
void simulateMountainBuilding(float timeStep) {
    for (each point in crustField) {
        float stress = crustStress.sampleAt(point.x, point.z);
        float rockResistance = rockHardness.sampleAt(point.x, point.z);
        
        if (stress > compressionThreshold) {
            // Uplift based on stress and rock type
            float uplift = calculateUplift(stress, rockResistance, timeStep);
            elevationField.addValue(point.x, point.z, uplift);
        }
    }
}
```

### Erosion Processes
```cpp
void simulateErosion(float timeStep) {
    // Chemical weathering
    for (each point in elevationField) {
        float weatheringRate = getWeatheringRate(rockType, climate);
        float erosion = weatheringRate * timeStep;
        elevationField.subtractValue(point.x, point.z, erosion);
    }
    
    // Water erosion along flow paths
    simulateWaterErosion(timeStep);
}
```

## Files to Create/Modify

### New Files
- `engine/include/world/GeologicalSimulator.h`
- `engine/src/world/GeologicalSimulator.cpp`
- `engine/include/world/GeologicalTypes.h`
- `engine/src/world/geological/TectonicSimulation.cpp`
- `engine/src/world/geological/ErosionSimulation.cpp`
- `engine/src/world/geological/DetailSimulation.cpp`

### Supporting Files
- `engine/include/world/GeologicalConfig.h`
- `tests/integration/GeologicalSimulationTest.cpp`

## Quality Presets Implementation

### Performance Preset (~1-2 minutes)
- Simplified physics models
- Lower resolution fields (2048×2048)
- Basic erosion algorithms

### Balanced Preset (~3-5 minutes) - Default
- Realistic physics models
- Medium resolution fields (4096×4096)
- Multiple erosion types

### Quality Preset (~8-12 minutes)
- Advanced physics models
- High resolution fields (8192×8192)
- Complex weathering simulation

### Ultra-Realism Preset (~20-30 minutes)
- Research-grade physics
- Maximum resolution fields (16384×16384)
- All geological processes enabled

## Visual Progress Integration

### Progress Information Structure
```cpp
struct PhaseInfo {
    GeologicalPhase currentPhase;
    float phaseProgress;        // 0.0 - 1.0
    float totalProgress;        // 0.0 - 1.0
    string currentProcess;      // "Mountain Building", "River Carving", etc.
    float timeRemaining;        // Estimated seconds
    PerformanceMetrics metrics;
};
```

## Testing Strategy

### Unit Tests
- Individual process algorithms
- Field value propagation
- Phase transition logic

### Integration Tests
- Complete simulation runs
- Performance benchmarking
- Memory usage validation

### Visual Validation
- Export height maps at each phase
- Visual diff comparison
- Geological accuracy review

## Success Criteria

- [ ] Three-phase simulation completes in target time
- [ ] Realistic geological features emerge
- [ ] Smooth transitions between phases
- [ ] All quality presets work correctly
- [ ] Visual progress updates work smoothly
- [ ] Geological samples export correctly

## Performance Targets

- **Balanced Preset:** 3-5 minutes total
- **Memory Usage:** < 500MB for geological fields
- **Visual Updates:** 60 smooth transitions total
- **Sample Export:** < 1ms per geological sample

## Notes

This is the heart of the geological realism system. Focus on realistic geological processes while maintaining performance targets. The visual timeline should be mesmerizing to watch.
