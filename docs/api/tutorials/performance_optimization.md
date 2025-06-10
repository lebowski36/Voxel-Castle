# Performance Optimization Guide

## Overview

This guide covers performance optimization techniques for Voxel Castle Engine, with special focus on the geological simulation and world generation systems.

## ContinuousField Performance

### Understanding Performance Characteristics

The `ContinuousField<T>` class is the foundation of geological simulation and must maintain high performance across different use cases:

**Performance Targets:**
- **Sample Lookup**: < 1ms per operation
- **Force Propagation**: < 10ms for 1000m radius
- **Memory Usage**: < 100MB for 1000×1000 field
- **Boundary Operations**: No performance penalty

### Optimization Strategies

#### 1. Field Size Selection

Choose appropriate field resolution based on your use case:

```cpp
// Continental scale: 2km resolution for plate tectonics
auto continentalField = std::make_unique<ContinuousField<float>>(1024, 1024, 2000.0f);

// Regional scale: 100m resolution for detailed terrain
auto regionalField = std::make_unique<ContinuousField<float>>(1024, 1024, 100.0f);

// Local scale: 1m resolution for high-detail areas
auto localField = std::make_unique<ContinuousField<float>>(512, 512, 1.0f);
```

**Memory Usage Guidelines:**
- 512×512: ~1MB (good for local details)
- 1024×1024: ~4MB (balanced for regional data)
- 2048×2048: ~17MB (continental scale)
- 4096×4096: ~67MB (maximum detail, use sparingly)

#### 2. Batch Operations

When performing many operations, batch them to reduce overhead:

```cpp
// ❌ Inefficient: Individual operations
for (const auto& position : positions) {
    float value = field->sampleAt(position.x, position.z);
    processValue(value);
}

// ✅ Efficient: Batch with cached properties
float spacing = field->getSampleSpacing();
float worldWidth = field->getWorldWidth();
float worldHeight = field->getWorldHeight();

std::vector<float> results;
results.reserve(positions.size());

for (const auto& position : positions) {
    results.push_back(field->sampleAt(position.x, position.z));
}

// Process all results together
processBatch(results);
```

#### 3. Smart Caching

Cache frequently accessed data:

```cpp
class TerrainSampler {
private:
    std::shared_ptr<ContinuousField<float>> elevationField_;
    
    // Cache for frequently used properties
    float cachedSpacing_;
    float cachedWorldWidth_;
    float cachedWorldHeight_;
    
    // LRU cache for sample results
    mutable std::unordered_map<uint64_t, float> sampleCache_;
    static constexpr size_t MAX_CACHE_SIZE = 1000;
    
public:
    TerrainSampler(std::shared_ptr<ContinuousField<float>> field) 
        : elevationField_(field) {
        // Cache properties at construction
        cachedSpacing_ = field->getSampleSpacing();
        cachedWorldWidth_ = field->getWorldWidth();
        cachedWorldHeight_ = field->getWorldHeight();
    }
    
    float getElevation(float x, float z) const {
        // Create cache key from coordinates
        uint64_t key = hashCoordinates(x, z);
        
        auto it = sampleCache_.find(key);
        if (it != sampleCache_.end()) {
            return it->second; // Cache hit
        }
        
        // Cache miss - compute and store
        float result = elevationField_->sampleAt(x, z);
        
        if (sampleCache_.size() >= MAX_CACHE_SIZE) {
            // Simple eviction: clear half the cache
            auto halfway = std::next(sampleCache_.begin(), MAX_CACHE_SIZE / 2);
            sampleCache_.erase(sampleCache_.begin(), halfway);
        }
        
        sampleCache_[key] = result;
        return result;
    }
    
private:
    uint64_t hashCoordinates(float x, float z) const {
        // Simple spatial hash for caching
        int gridX = static_cast<int>(x / cachedSpacing_);
        int gridZ = static_cast<int>(z / cachedSpacing_);
        return (static_cast<uint64_t>(gridX) << 32) | static_cast<uint64_t>(gridZ);
    }
};
```

#### 4. Multithreaded Access Patterns

Use read-write locks for safe concurrent access:

```cpp
#include <shared_mutex>

class ThreadSafeFieldAccess {
private:
    std::shared_ptr<ContinuousField<float>> field_;
    mutable std::shared_mutex mutex_;
    
public:
    ThreadSafeFieldAccess(std::shared_ptr<ContinuousField<float>> field) 
        : field_(field) {}
    
    // Multiple threads can read simultaneously
    float sampleAt(float x, float z) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return field_->sampleAt(x, z);
    }
    
    // Only one thread can write at a time
    void setSample(int x, int z, float value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        field_->setSample(x, z, value);
    }
    
    // Batch read operations for better performance
    std::vector<float> sampleBatch(const std::vector<std::pair<float, float>>& positions) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        
        std::vector<float> results;
        results.reserve(positions.size());
        
        for (const auto& pos : positions) {
            results.push_back(field_->sampleAt(pos.first, pos.second));
        }
        
        return results;
    }
};
```

## Force Propagation Optimization

### Understanding Force Propagation Costs

Force propagation is computationally expensive because it affects multiple samples:

**Cost Factors:**
- **Range**: O(range²) - larger ranges are exponentially more expensive
- **Resistance Function**: Complex functions add overhead per affected sample
- **Field Resolution**: Higher resolution means more samples to update

### Optimization Techniques

#### 1. Intelligent Range Selection

```cpp
// ❌ Expensive: Very large propagation range
field->propagateValue(1000.0f, x, z, 10000.0f, resistanceFunc); // 10km radius!

// ✅ Efficient: Reasonable range based on geological scale
float effectiveRange = std::min(2000.0f, field->getSampleSpacing() * 50.0f);
field->propagateValue(1000.0f, x, z, effectiveRange, resistanceFunc);
```

#### 2. Optimized Resistance Functions

```cpp
// ❌ Expensive: Complex resistance calculation
auto expensiveResistance = [&](float x, float z) -> float {
    float temp = temperatureField->sampleAt(x, z);
    float humidity = humidityField->sampleAt(x, z);
    float rockHardness = rockField->sampleAt(x, z);
    
    // Complex calculation involving multiple field samples
    return calculateComplexResistance(temp, humidity, rockHardness);
};

// ✅ Efficient: Pre-computed resistance field
class OptimizedResistance {
private:
    std::shared_ptr<ContinuousField<float>> precomputedResistance_;
    
public:
    OptimizedResistance(/* source fields */) {
        // Pre-compute resistance values once
        precomputedResistance_ = std::make_unique<ContinuousField<float>>(1024, 1024, 2000.0f);
        
        for (int z = 0; z < 1024; ++z) {
            for (int x = 0; x < 1024; ++x) {
                float worldX = x * 2000.0f;
                float worldZ = z * 2000.0f;
                
                float temp = temperatureField->sampleAt(worldX, worldZ);
                float humidity = humidityField->sampleAt(worldX, worldZ);
                float rockHardness = rockField->sampleAt(worldX, worldZ);
                
                float resistance = calculateComplexResistance(temp, humidity, rockHardness);
                precomputedResistance_->setSample(x, z, resistance);
            }
        }
    }
    
    // Fast resistance lookup
    float operator()(float x, float z) const {
        return precomputedResistance_->sampleAt(x, z);
    }
};
```

#### 3. Progressive Force Application

For very large forces, apply them progressively:

```cpp
void applyLargeTectonicForce(ContinuousField<float>& field, float x, float z, float totalForce) {
    const float maxRangePerStep = 1000.0f;
    const int numSteps = 5;
    const float forcePerStep = totalForce / numSteps;
    
    auto resistance = [](float x, float z) -> float { return 1.0f; };
    
    for (int step = 0; step < numSteps; ++step) {
        float currentRange = maxRangePerStep * (step + 1);
        field.propagateValue(forcePerStep, x, z, currentRange, resistance);
        
        // Allow other operations between steps
        std::this_thread::yield();
    }
}
```

## Memory Optimization

### Memory Usage Patterns

Monitor memory usage for different field configurations:

```cpp
void analyzeMemoryUsage() {
    struct FieldConfig {
        int size;
        float spacing;
        std::string description;
    };
    
    std::vector<FieldConfig> configs = {
        {512, 1000.0f, "Regional detail (512km²)"},
        {1024, 2000.0f, "Continental scale (2048km²)"},
        {2048, 1000.0f, "High-detail continental (2048km²)"},
        {4096, 500.0f, "Ultra-high detail (2048km²)"}
    };
    
    for (const auto& config : configs) {
        size_t memoryBytes = config.size * config.size * sizeof(float);
        double memoryMB = memoryBytes / (1024.0 * 1024.0);
        
        std::cout << config.description << ": " 
                  << memoryMB << " MB" << std::endl;
        
        if (memoryMB > 100.0) {
            std::cout << "  ⚠️  Exceeds 100MB target - consider optimization" << std::endl;
        }
    }
}
```

### Memory-Efficient Alternatives

For very large worlds, consider hierarchical approaches:

```cpp
class HierarchicalField {
private:
    // Multiple resolution levels
    std::unique_ptr<ContinuousField<float>> level0_; // 4km resolution
    std::unique_ptr<ContinuousField<float>> level1_; // 1km resolution  
    std::unique_ptr<ContinuousField<float>> level2_; // 250m resolution
    
public:
    float sampleAt(float x, float z, float detailLevel) const {
        if (detailLevel < 1000.0f && level2_) {
            return level2_->sampleAt(x, z);
        } else if (detailLevel < 5000.0f && level1_) {
            return level1_->sampleAt(x, z);
        } else {
            return level0_->sampleAt(x, z);
        }
    }
};
```

## Performance Monitoring

### Built-in Performance Tracking

```cpp
class PerformanceTracker {
private:
    std::unordered_map<std::string, std::vector<double>> timings_;
    
public:
    template<typename Func>
    auto timeOperation(const std::string& name, Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        
        double timeUs = std::chrono::duration<double, std::micro>(end - start).count();
        timings_[name].push_back(timeUs);
        
        return result;
    }
    
    void printStatistics() const {
        for (const auto& [name, times] : timings_) {
            if (times.empty()) continue;
            
            double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
            double minTime = *std::min_element(times.begin(), times.end());
            double maxTime = *std::max_element(times.begin(), times.end());
            
            std::cout << name << ":" << std::endl;
            std::cout << "  Average: " << avg << " μs" << std::endl;
            std::cout << "  Range: " << minTime << " - " << maxTime << " μs" << std::endl;
            std::cout << "  Samples: " << times.size() << std::endl;
        }
    }
};

// Usage example
PerformanceTracker tracker;

float elevation = tracker.timeOperation("sample_lookup", [&]() {
    return elevationField->sampleAt(x, z);
});
```

### Profiling Integration

Use the built-in benchmark system for regular performance validation:

```cpp
#include "world/ContinuousFieldBenchmark.h"

// Run benchmarks during development
void validatePerformance() {
    VoxelCastle::World::Performance::runContinuousFieldBenchmarks();
}

// Automated performance regression testing
bool checkPerformanceRegression() {
    // This would be called in your test suite
    // Returns false if performance has degraded
    return true; // Implement based on benchmark results
}
```

## Best Practices Summary

### Do's ✅
- **Cache frequently used properties** (spacing, world dimensions)
- **Use appropriate field resolutions** for your scale requirements
- **Batch operations** when processing multiple samples
- **Pre-compute complex resistance functions** for force propagation
- **Monitor memory usage** and stay within targets
- **Use read-write locks** for thread safety
- **Run performance benchmarks** regularly during development

### Don'ts ❌
- **Don't use oversized fields** without justification
- **Don't perform complex calculations** in resistance functions
- **Don't ignore thread safety** in multithreaded environments
- **Don't propagate forces** over unnecessarily large ranges
- **Don't skip performance validation** before releasing
- **Don't assume linear scaling** - test actual performance
- **Don't mix resolution levels** without careful consideration

### Performance Checklist

Before deploying ContinuousField-based systems:

- [ ] Sample lookup averages < 1ms
- [ ] Force propagation completes < 10ms for 1000m radius
- [ ] Memory usage < 100MB for 1000×1000 fields
- [ ] Thread safety implemented where needed
- [ ] Caching strategy implemented for repeated access
- [ ] Batch operations used for bulk processing
- [ ] Performance benchmarks pass all targets
- [ ] Memory usage monitored and optimized

---

## See Also

- **[ContinuousField API Reference](../world_generation/continuous_field.md)** - Complete API documentation
- **[Memory Management Guide](memory_management.md)** - General memory optimization
- **[Multithreading Best Practices](multithreading.md)** - Thread safety patterns
