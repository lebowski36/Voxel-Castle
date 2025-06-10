# ContinuousField<T> API Reference

## Overview

The `ContinuousField<T>` template class provides smooth, organic interpolation for geological and spatial data. It's the mathematical foundation for all geological simulations in Voxel Castle Engine, offering seamless interpolation without grid artifacts.

**Key Features:**
- 🌊 **Smooth Interpolation**: Bicubic interpolation with Perlin noise enhancement
- 🌍 **Toroidal Topology**: Seamless world boundaries with wrapping support
- ⚡ **High Performance**: Sub-millisecond sample lookups
- 🧱 **Force Propagation**: Geological resistance modeling for realistic terrain
- 📏 **Multi-Scale**: Supports fields from meters to continental scales

## Quick Start

```cpp
#include "world/ContinuousField.h"

// Create a 100x100 field with 2m spacing between samples
auto elevationField = std::make_unique<ContinuousField<float>>(100, 100, 2.0f);

// Set sample values
elevationField->setSample(50, 50, 100.0f); // 100m elevation at center

// Sample at any world position with smooth interpolation
float height = elevationField->sampleAt(101.5f, 98.7f);
```

## Class Declaration

```cpp
template<typename T>
class ContinuousField {
public:
    // Construction and lifecycle
    ContinuousField(int width, int height, float spacing);
    ~ContinuousField() = default;
    
    // Core sampling and interpolation
    T sampleAt(float x, float z) const;
    
    // Data access and modification
    void setSample(int x, int z, const T& value);
    T getSample(int x, int z) const;
    void addToSample(int x, int z, const T& value);
    
    // Geological force propagation
    void propagateValue(const T& value, float x, float z, float range, 
                       std::function<float(float, float)> resistanceFunc);
    
    // Toroidal world support
    Point wrapCoordinates(float x, float z) const;
    float getToroidalDistance(float x1, float z1, float x2, float z2) const;
    
    // Utility operations
    void clear();
    void fill(const T& value);
    
    // Properties
    int getWidth() const;
    int getHeight() const;
    float getSampleSpacing() const;
    float getWorldWidth() const;
    float getWorldHeight() const;
};
```

## Constructor

### ContinuousField(int width, int height, float spacing)

Creates a new continuous field with the specified dimensions and sample spacing.

**Parameters:**
- `width` - Number of samples in X direction (minimum: 2)
- `height` - Number of samples in Z direction (minimum: 2)  
- `spacing` - Distance between samples in world units (meters)

**Performance:**
- **Time Complexity**: O(width × height)
- **Memory Usage**: `width × height × sizeof(T)` + overhead
- **Thread Safety**: Constructor is not thread-safe

**Example:**
```cpp
// Create a 1km² field with 10m resolution
auto field = std::make_unique<ContinuousField<float>>(100, 100, 10.0f);

// Create a continental-scale field (2048km² with 2km resolution)
auto continentalField = std::make_unique<ContinuousField<float>>(1024, 1024, 2000.0f);
```

## Core Sampling Methods

### T sampleAt(float x, float z) const

Samples the field at any world position using smooth bicubic interpolation enhanced with Perlin noise.

**Parameters:**
- `x` - World X coordinate in meters
- `z` - World Z coordinate in meters

**Returns:**
- Interpolated value at the specified position

**Performance:**
- **Time Complexity**: O(1) - constant time lookup
- **Target Performance**: < 1ms per sample
- **Thread Safety**: Read-only, thread-safe

**Interpolation Details:**
- Uses **bicubic interpolation** for smooth curves between samples
- Enhanced with **multi-octave Perlin noise** for organic variation
- **Toroidal wrapping** ensures seamless boundaries
- **No grid artifacts** - produces natural, continuous surfaces

**Example:**
```cpp
// Sample at exact grid coordinates
float exactSample = field->sampleAt(100.0f, 200.0f);

// Sample between grid points (smooth interpolation)
float interpolated = field->sampleAt(105.7f, 198.3f);

// Sample outside world bounds (automatically wraps)
float wrapped = field->sampleAt(-50.0f, 1500.0f);
```

### void setSample(int x, int z, const T& value)

Sets the value at a specific grid coordinate.

**Parameters:**
- `x` - Grid X index
- `z` - Grid Z index  
- `value` - Value to store

**Performance:**
- **Time Complexity**: O(1)
- **Thread Safety**: Not thread-safe for writes

**Automatic Wrapping:**
Grid coordinates are automatically wrapped to valid ranges:
```cpp
// These are equivalent if field is 100x100:
field->setSample(105, 78, 50.0f);  // Wraps to (5, 78)
field->setSample(5, 78, 50.0f);    // Direct access
```

### T getSample(int x, int z) const

Retrieves the exact value at a grid coordinate without interpolation.

**Parameters:**
- `x` - Grid X index  
- `z` - Grid Z index

**Returns:**
- Exact stored value at grid position

**Performance:**
- **Time Complexity**: O(1)
- **Thread Safety**: Read-only, thread-safe

## Force Propagation System

### void propagateValue(const T& value, float x, float z, float range, std::function<float(float, float)> resistanceFunc)

Propagates a geological force through the field using a resistance model. Essential for realistic terrain generation.

**Parameters:**
- `value` - Force value to propagate
- `x`, `z` - World coordinates of force origin
- `range` - Maximum propagation distance in meters
- `resistanceFunc` - Function returning resistance factor (1.0 = normal, >1.0 = high resistance)

**Performance:**
- **Time Complexity**: O(range²) for circular propagation area
- **Target Performance**: < 10ms for 1000m radius
- **Thread Safety**: Not thread-safe

**Resistance Function:**
```cpp
// Example: Variable rock hardness affects force propagation
auto rockResistance = [&](float x, float z) -> float {
    RockType rock = rockTypeField->sampleAt(x, z);
    switch (rock) {
        case RockType::IGNEOUS_GRANITE: return 5.0f;  // Very resistant
        case RockType::SEDIMENTARY_SANDSTONE: return 2.0f; // Moderate
        case RockType::SOIL_CLAY: return 0.5f; // Low resistance
        default: return 1.0f;
    }
};

// Propagate tectonic stress
float tectonicForce = 1000.0f;
float faultRadius = 2000.0f; // 2km radius
elevationField->propagateValue(tectonicForce, faultX, faultZ, faultRadius, rockResistance);
```

**Mathematical Model:**
- **Distance Falloff**: Exponential decay with distance
- **Resistance Integration**: Force reduced by local material resistance
- **Accumulation**: Multiple forces can accumulate at the same location

## Toroidal World Support

### Point wrapCoordinates(float x, float z) const

Converts world coordinates to wrapped coordinates for seamless boundaries.

**Parameters:**
- `x`, `z` - World coordinates (can be outside bounds)

**Returns:**
- `Point` structure with wrapped coordinates

**Example:**
```cpp
// Field is 1000m × 1000m
Point wrapped = field->wrapCoordinates(1250.0f, -300.0f);
// Result: wrapped.x = 250.0f, wrapped.z = 700.0f
```

### float getToroidalDistance(float x1, float z1, float x2, float z2) const

Calculates the shortest distance between two points considering toroidal wrapping.

**Parameters:**
- `x1`, `z1` - First point coordinates
- `x2`, `z2` - Second point coordinates  

**Returns:**
- Minimum distance considering world wrapping

**Use Case:**
Essential for force propagation and spatial queries in wrapped worlds:
```cpp
// Find shortest distance (may go "around the world")
float distance = field->getToroidalDistance(50.0f, 50.0f, 950.0f, 950.0f);
// Result: Much shorter than direct distance due to wrapping
```

## Utility Methods

### void clear()

Sets all samples to their default value (typically 0).

**Performance:**
- **Time Complexity**: O(width × height)
- **Use Case**: Resetting fields between simulations

### void fill(const T& value)

Sets all samples to the specified value.

**Parameters:**
- `value` - Value to fill the entire field with

**Performance:**
- **Time Complexity**: O(width × height)

### void addToSample(int x, int z, const T& value)

Adds a value to an existing sample (for arithmetic types).

**Parameters:**
- `x`, `z` - Grid coordinates
- `value` - Value to add

**Note:** Only works with arithmetic types (float, int, etc.). For complex types, this method sets the value instead.

## Performance Characteristics

### Memory Usage
| Field Size | Float Field | Double Field | RockType Field |
|------------|-------------|--------------|----------------|
| 100×100    | ~40 KB      | ~80 KB       | ~10 KB         |
| 1000×1000  | ~4 MB       | ~8 MB        | ~1 MB          |
| 4096×4096  | ~67 MB      | ~134 MB      | ~17 MB         |

### Sample Lookup Performance
| Operation | Target Time | Typical Time |
|-----------|-------------|--------------|
| getSample() | < 0.1μs | ~0.05μs |
| sampleAt() | < 1000μs | ~100-500μs |
| setSample() | < 0.1μs | ~0.05μs |
| propagateValue() | < 10ms (1km radius) | ~2-8ms |

### Optimization Tips

**For High-Frequency Sampling:**
```cpp
// Cache frequently used values
float cachedSpacing = field->getSampleSpacing();
float cachedWidth = field->getWorldWidth();

// Batch operations when possible
std::vector<Point> positions = generateSamplePositions();
std::vector<float> results;
results.reserve(positions.size());

for (const auto& pos : positions) {
    results.push_back(field->sampleAt(pos.x, pos.z));
}
```

**For Memory Efficiency:**
```cpp
// Use appropriate numeric types
ContinuousField<float> elevationField;    // 4 bytes per sample
ContinuousField<uint16_t> temperatureField; // 2 bytes per sample (scaled)

// Consider field resolution vs. quality trade-offs
// High resolution: 4096×4096 for maximum detail
// Balanced: 1024×1024 for good quality/performance
// Performance: 512×512 for fast generation
```

## Common Use Cases

### Geological Simulation
```cpp
// Create elevation field for continent-scale simulation
auto elevationField = std::make_unique<ContinuousField<float>>(2048, 2048, 1000.0f);

// Create rock hardness field for erosion simulation  
auto hardnessField = std::make_unique<ContinuousField<float>>(2048, 2048, 1000.0f);

// Simulate tectonic uplift
auto plateResistance = [&](float x, float z) -> float {
    return hardnessField->sampleAt(x, z);
};

elevationField->propagateValue(500.0f, plateX, plateZ, 50000.0f, plateResistance);
```

### Multi-Resolution Data
```cpp
// Coarse continental data (2km resolution)
auto continentalData = std::make_unique<ContinuousField<float>>(1024, 1024, 2000.0f);

// Fine regional data (100m resolution)  
auto regionalData = std::make_unique<ContinuousField<float>>(1024, 1024, 100.0f);

// Sample from appropriate resolution based on scale
float getElevation(float x, float z, float viewDistance) {
    if (viewDistance > 10000.0f) {
        return continentalData->sampleAt(x, z);
    } else {
        return regionalData->sampleAt(x, z);
    }
}
```

### Climate and Environmental Data
```cpp
// Temperature field with seasonal variation
auto temperatureField = std::make_unique<ContinuousField<float>>(512, 512, 2000.0f);

// Add temperature gradient from south to north
for (int z = 0; z < 512; ++z) {
    for (int x = 0; x < 512; ++x) {
        float latitude = (z / 512.0f) * 180.0f - 90.0f; // -90 to +90 degrees
        float baseTemp = 30.0f - (std::abs(latitude) * 0.7f); // Warmer at equator
        temperatureField->setSample(x, z, baseTemp);
    }
}
```

## Thread Safety

| Method | Thread Safety | Notes |
|--------|---------------|-------|
| sampleAt() | ✅ Safe | Read-only operations |
| getSample() | ✅ Safe | Read-only operations |
| setSample() | ❌ Not Safe | Requires external synchronization |
| propagateValue() | ❌ Not Safe | Modifies multiple samples |
| clear() / fill() | ❌ Not Safe | Modifies all samples |

**Recommended Pattern for Multithreading:**
```cpp
// Use read-write locks for safe concurrent access
std::shared_mutex fieldMutex;

// Multiple readers (sampling)
{
    std::shared_lock<std::shared_mutex> lock(fieldMutex);
    float value = field->sampleAt(x, z);
}

// Single writer (modification)
{
    std::unique_lock<std::shared_mutex> lock(fieldMutex);
    field->setSample(x, z, newValue);
}
```

## Error Handling

The ContinuousField class uses the following error handling strategies:

**Constructor Validation:**
```cpp
try {
    auto field = std::make_unique<ContinuousField<float>>(0, 0, 1.0f); // Invalid size
} catch (const std::invalid_argument& e) {
    // Handle invalid parameters
}
```

**Coordinate Wrapping:**
- Invalid coordinates are automatically wrapped rather than throwing exceptions
- This ensures robust operation in edge cases

**Memory Allocation:**
- Large field allocation may throw `std::bad_alloc`
- Consider catching and gracefully degrading quality

## Type Specializations

### Arithmetic Types (float, double, int)
- Full interpolation support with Perlin noise enhancement
- addToSample() performs actual addition
- Suitable for: elevation, temperature, pressure, density

### Enumeration Types (RockType, BiomeType)
- Nearest-neighbor sampling (no interpolation)
- addToSample() replaces value instead of adding
- Suitable for: material types, discrete classifications

### Custom Types
```cpp
// Custom geological data structure
struct GeologicalSample {
    float elevation;
    RockType primaryRock;
    float hardness;
    float porosity;
    
    // Must provide default constructor for ContinuousField
    GeologicalSample() : elevation(0.0f), primaryRock(RockType::IGNEOUS_GRANITE), 
                        hardness(1.0f), porosity(0.1f) {}
};

// Usage
auto geoField = std::make_unique<ContinuousField<GeologicalSample>>(1024, 1024, 500.0f);
```

## Migration and Compatibility

### From Previous Versions
- **v0.9**: `getValueAt()` renamed to `sampleAt()`
- **v0.8**: Constructor parameter order changed (width, height, spacing)
- **v0.7**: Thread safety guarantees added for read-only operations

### Best Practices Evolution
- **Preferred**: Use smart pointers for field ownership
- **Deprecated**: Raw pointer management
- **Future**: GPU acceleration for large fields (planned v1.1)

---

## See Also

- **[GeologicalSimulator](geological_simulator.md)** - High-level geological simulation using ContinuousField
- **[SeedWorldGenerator](seed_world_generator.md)** - World generation integration
- **[Performance Guide](../tutorials/performance_optimization.md)** - Optimization techniques
- **[Memory Management](../tutorials/memory_management.md)** - Efficient memory usage patterns
