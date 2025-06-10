# Subtask 6: Chunk Management System

**Priority:** Performance & Streaming
**Estimated Time:** 3-4 days
**Dependencies:** Hybrid Detail Generator, SeedWorldGenerator Integration

## Overview

Implement the intelligent multi-tier caching and streaming system that manages geological chunk generation, storage, and loading for seamless world exploration with optimal performance.

## Technical Requirements

### Core Chunk Management Architecture
```cpp
class GeologicalChunkManager {
private:
    // Multi-tier caching system
    LRUCache<ChunkCoord, VoxelChunk> activeChunks;          // Tier 1: In memory
    DiskCache<ChunkCoord, CompressedChunk> diskCache;       // Tier 2: On disk
    ContinuousField<GeologicalSample> baseGeology;          // Tier 3: Base data
    
    // Boundary smoothing system
    HashMap<ChunkCoord, BorderVoxels> boundaryBuffer;       // Edge overlap
    
    // Predictive loading system
    std::unique_ptr<PredictiveLoader> predictiveLoader;
    
    // Quality fallback system
    QualityFallbackManager fallbackManager;
    
    GeologicalConfig config;
    
public:
    // Core chunk operations
    VoxelChunk getChunk(int chunkX, int chunkZ);
    void preloadChunks(const std::vector<ChunkCoord>& coords);
    void unloadDistantChunks(int playerX, int playerZ, float unloadDistance);
    
    // Streaming management
    void updatePlayerPosition(float playerX, float playerZ);
    void setLoadRadius(float radius);
    
    // Performance monitoring
    PerformanceMetrics getPerformanceMetrics() const;
    void enablePerformanceMonitoring(bool enabled);
};
```

### Multi-Tier Caching Strategy

#### Tier 1: Active Chunks (Memory)
- **Capacity:** 64 chunks (~2GB memory)
- **Access Time:** < 1ms
- **Scope:** Player vicinity (±4 chunk radius)
- **Eviction:** LRU based on player distance

#### Tier 2: Generated Chunks (Disk)
- **Capacity:** Unlimited (compressed storage)
- **Access Time:** 10-50ms (disk I/O + decompression)
- **Scope:** Previously explored areas
- **Storage:** ~32KB per compressed chunk

#### Tier 3: Geological Base Data (Memory)
- **Size:** ~500MB (always loaded)
- **Access Time:** < 0.1ms
- **Scope:** Entire world geological foundation
- **Usage:** Generate new chunks on-demand

## Implementation Steps

### Phase 1: Core Caching System (Day 1)

#### LRU Active Chunk Cache
```cpp
class LRUChunkCache {
private:
    struct CacheEntry {
        VoxelChunk chunk;
        ChunkCoord coord;
        std::chrono::steady_clock::time_point lastAccess;
        size_t memorySize;
    };
    
    std::unordered_map<ChunkCoord, std::list<CacheEntry>::iterator> chunkMap;
    std::list<CacheEntry> lruList;
    size_t maxMemoryUsage;
    size_t currentMemoryUsage;
    
public:
    bool contains(const ChunkCoord& coord) const;
    VoxelChunk get(const ChunkCoord& coord);
    void put(const ChunkCoord& coord, const VoxelChunk& chunk);
    void evictOldChunks();
    
    // Performance monitoring
    float getHitRate() const;
    size_t getMemoryUsage() const;
};
```

#### Disk-Based Chunk Storage
```cpp
class DiskChunkCache {
private:
    std::string worldPath;
    std::unordered_map<ChunkCoord, ChunkMetadata> chunkIndex;
    CompressionSystem compressor;
    
public:
    bool chunkExists(const ChunkCoord& coord) const;
    VoxelChunk loadChunk(const ChunkCoord& coord);
    void saveChunk(const ChunkCoord& coord, const VoxelChunk& chunk);
    
    // Batch operations for performance
    void preloadChunks(const std::vector<ChunkCoord>& coords);
    void batchSaveChunks(const std::vector<std::pair<ChunkCoord, VoxelChunk>>& chunks);
};
```

### Phase 2: Boundary Smoothing System (Day 2)

#### Seamless Chunk Boundaries
```cpp
class BoundarySmoothing {
private:
    static constexpr int OVERLAP_SIZE = 1; // 1-voxel overlap
    
    struct BorderVoxels {
        std::array<VoxelData, CHUNK_SIZE> northBorder;
        std::array<VoxelData, CHUNK_SIZE> southBorder;
        std::array<VoxelData, CHUNK_SIZE> eastBorder;
        std::array<VoxelData, CHUNK_SIZE> westBorder;
    };
    
    HashMap<ChunkCoord, BorderVoxels> borderCache;
    
public:
    void cacheBorderVoxels(const ChunkCoord& coord, const VoxelChunk& chunk);
    void smoothChunkBoundaries(VoxelChunk& chunk, const ChunkCoord& coord);
    
private:
    BorderVoxels extractBorderVoxels(const VoxelChunk& chunk);
    void applyBorderSmoothing(VoxelChunk& chunk, const BorderVoxels& neighbors);
};
```

#### Geological Continuity Enforcement
```cpp
void BoundarySmoothing::smoothChunkBoundaries(VoxelChunk& chunk, const ChunkCoord& coord) {
    // Get neighboring border data
    auto northBorder = getBorderVoxels(ChunkCoord(coord.x, coord.z + 1), SOUTH);
    auto southBorder = getBorderVoxels(ChunkCoord(coord.x, coord.z - 1), NORTH);
    auto eastBorder = getBorderVoxels(ChunkCoord(coord.x + 1, coord.z), WEST);
    auto westBorder = getBorderVoxels(ChunkCoord(coord.x - 1, coord.z), EAST);
    
    // Apply geological continuity constraints
    enforceRockTypeContinuity(chunk, northBorder, southBorder, eastBorder, westBorder);
    enforceElevationContinuity(chunk, northBorder, southBorder, eastBorder, westBorder);
    enforceJointSystemContinuity(chunk, neighboringJointSystems);
}
```

### Phase 3: Predictive Loading System (Day 3)

#### Player Movement Prediction
```cpp
class PredictiveLoader {
private:
    struct MovementHistory {
        std::vector<Vector3> positions;
        std::vector<std::chrono::steady_clock::time_point> timestamps;
        Vector3 predictedVelocity;
        Vector3 predictedDirection;
    };
    
    MovementHistory playerMovement;
    GeologicalChunkManager* chunkManager;
    float predictionTime = 5.0f; // Predict 5 seconds ahead
    
public:
    void updatePlayerPosition(const Vector3& position);
    std::vector<ChunkCoord> getPredictiveLoadList(int maxChunks = 9);
    
private:
    Vector3 calculatePredictedPosition(float timeAhead);
    std::vector<ChunkCoord> getChunksInRadius(const Vector3& center, float radius);
};
```

#### Asynchronous Chunk Loading
```cpp
class AsyncChunkLoader {
private:
    std::thread loaderThread;
    std::queue<ChunkCoord> loadQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> running;
    
    GeologicalChunkManager* chunkManager;
    
public:
    void startAsyncLoading();
    void stopAsyncLoading();
    void queueChunkLoad(const ChunkCoord& coord);
    
private:
    void loaderThreadFunction();
    void processLoadQueue();
};
```

### Phase 4: Quality Fallback System (Day 4)

#### Performance Monitoring
```cpp
class PerformanceMonitor {
private:
    std::chrono::steady_clock::time_point lastFrameTime;
    std::deque<float> frameTimeHistory;
    std::deque<float> chunkGenerationTimes;
    size_t memoryUsage;
    
public:
    void recordFrameTime();
    void recordChunkGenerationTime(float time);
    void updateMemoryUsage(size_t usage);
    
    // Performance metrics
    float getAverageFrameTime() const;
    float getAverageChunkGenerationTime() const;
    bool isPerformanceStressed() const;
    
    PerformanceLevel recommendQualityLevel() const;
};
```

#### Dynamic Quality Fallback
```cpp
class QualityFallbackManager {
private:
    GeologicalPreset currentQuality;
    GeologicalPreset fallbackQuality;
    PerformanceMonitor performanceMonitor;
    
    // Fallback thresholds
    float maxChunkGenerationTime = 100.0f; // 100ms
    float maxMemoryUsage = 0.8f;           // 80% of available memory
    
public:
    GeologicalPreset getCurrentQuality() const { return currentQuality; }
    bool shouldFallback() const;
    void applyFallback();
    void restoreQuality();
    
private:
    GeologicalPreset calculateFallbackQuality();
    void notifyUser(const std::string& message);
};
```

## Streaming Performance Optimization

### Batch Operations
```cpp
// Batch load multiple chunks for better I/O performance
void batchLoadChunks(const std::vector<ChunkCoord>& coords) {
    // Group by disk location for sequential reads
    auto groupedCoords = groupByDiskLocation(coords);
    
    for (const auto& group : groupedCoords) {
        // Load chunks sequentially from same disk region
        loadChunkGroup(group);
    }
}
```

### Memory Pool Management
```cpp
class ChunkMemoryPool {
private:
    std::vector<VoxelChunk> chunkPool;
    std::queue<VoxelChunk*> availableChunks;
    std::mutex poolMutex;
    
public:
    VoxelChunk* acquireChunk();
    void releaseChunk(VoxelChunk* chunk);
    
    // Pre-allocate chunks to avoid memory fragmentation
    void preallocateChunks(size_t count);
};
```

## Integration Points

### World Generator Integration
```cpp
// Integrate with SeedWorldGenerator
VoxelChunk SeedWorldGenerator::generateChunkOnDemand(int chunkX, int chunkZ) {
    // Check if chunk manager has cached version
    if (chunkManager->hasChunk(chunkX, chunkZ)) {
        return chunkManager->getChunk(chunkX, chunkZ);
    }
    
    // Generate new chunk with detail generator
    GeologicalSample geoSample = geologicalSim->getSampleAt(chunkX * 8, chunkZ * 8);
    VoxelChunk chunk = detailGen->generateChunkDetail(chunkX, chunkZ, geoSample);
    
    // Store in chunk manager
    chunkManager->storeChunk(chunkX, chunkZ, chunk);
    
    return chunk;
}
```

### Game Engine Integration
```cpp
// Integrate with game world system
void WorldRenderer::updateChunks() {
    Vector3 playerPos = getPlayerPosition();
    
    // Update chunk manager with player position
    chunkManager->updatePlayerPosition(playerPos.x, playerPos.z);
    
    // Get visible chunks
    auto visibleChunks = chunkManager->getVisibleChunks(playerPos, renderDistance);
    
    // Render chunks
    for (const auto& chunk : visibleChunks) {
        renderChunk(chunk);
    }
}
```

## Files to Create/Modify

### New Files
- `engine/include/world/GeologicalChunkManager.h`
- `engine/src/world/GeologicalChunkManager.cpp`
- `engine/src/world/chunk/LRUChunkCache.cpp`
- `engine/src/world/chunk/DiskChunkCache.cpp`
- `engine/src/world/chunk/BoundarySmoothing.cpp`
- `engine/src/world/chunk/PredictiveLoader.cpp`
- `engine/src/world/chunk/QualityFallbackManager.cpp`
- `engine/src/world/chunk/PerformanceMonitor.cpp`

### Modified Files
- `engine/src/world/seed_world_generator.cpp` - Chunk manager integration
- `engine/src/rendering/WorldRenderer.cpp` - Chunk streaming integration
- `engine/src/world/World.cpp` - Chunk loading integration

### Supporting Files
- `engine/include/world/ChunkTypes.h` - Data structures
- `tests/performance/ChunkLoadingBenchmark.cpp` - Performance tests

## Performance Targets

### Memory Usage
- **Active Chunks:** 64 chunks × 32MB = ~2GB maximum
- **Boundary Buffer:** ~100MB for edge smoothing
- **Base Geology:** ~500MB (always loaded)
- **Total Peak:** ~2.6GB during normal gameplay

### Loading Performance
- **Cache Hit:** < 1ms chunk access
- **Disk Load:** < 50ms chunk loading from disk
- **Generation:** 15ms-200ms new chunk generation (quality dependent)
- **Predictive Loading:** 95% cache hit rate for player movement

### Streaming Performance
- **Load Radius:** 8 chunks (256m) around player
- **Unload Distance:** 16 chunks (512m) from player
- **Batch Size:** 9 chunks per predictive load batch
- **Update Rate:** 10Hz position updates for smooth streaming

## Testing Strategy

### Unit Tests
- LRU cache eviction logic
- Boundary smoothing algorithms
- Predictive loading accuracy

### Performance Tests
- Memory usage under various loads
- Chunk loading performance benchmarks
- Cache hit rate optimization

### Integration Tests
- Complete chunk streaming during gameplay simulation
- Quality fallback system under stress
- Boundary continuity verification

## Success Criteria

- [ ] Seamless chunk loading with no visible boundaries
- [ ] 95%+ cache hit rate for normal player movement
- [ ] Memory usage stays within target limits
- [ ] Quality fallback system prevents performance issues
- [ ] Predictive loading reduces wait times
- [ ] Disk storage efficiently compressed
- [ ] All performance targets met

## Notes

This system is critical for gameplay performance. Players should never experience lag or visible chunk boundaries when exploring. The predictive loading should stay ahead of player movement, and the quality fallback should gracefully handle performance stress without jarring quality changes.
