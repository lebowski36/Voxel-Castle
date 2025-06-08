# World Database Investigation Report

*Created: 2025-06-08 21:15*  
*Last Updated: 2025-06-08 21:20*  
*Status: ✅ COMPLETED - CUSTOM BINARY FORMAT SELECTED*

> **⚠️ IMPORTANT UPDATE ⚠️**
> 
> This report has been **superseded** by the analysis in [08c.4.1_world_database_format_investigation.md](../development_tasks/04_essential_game_infrastructure/08c.4.1_world_database_format_investigation.md).
> 
> **DECISION**: Custom chunked binary format selected over SQLite for optimal spatial data access patterns.
> 
> This file is kept for historical reference of the investigation process.

## Executive Summary

~~This report investigates database format options for Voxel Castle's advanced world generation system, focusing on regional data storage for biomes, climate, geology, and hydrology data.~~

**UPDATE**: After thorough analysis, **Custom Chunked Binary Format** was selected as the optimal solution for our spatial data access patterns. SQLite was determined to be over-engineering for our use case.

## Current Architecture Analysis

### Existing File I/O Patterns

**Current Save System**: 
- Uses JSON for metadata and configuration
- Binary files for chunk/voxel data  
- File-based organization with std::filesystem
- Supports streaming load/save of world chunks

**Current Storage Structure**:
```
saves/
├── world_metadata.json
├── chunks/
│   ├── chunk_x_z.dat (binary voxel data)
└── player_data.json
```

**Access Patterns**:
- **Read**: Frequent chunk loading during gameplay (~16ms target)
- **Write**: Periodic saves during auto-save intervals 
- **Streaming**: Sequential chunk loading as player moves

### Integration Points

1. **WorldManager** - Manages chunk loading/unloading
2. **SaveManager** - Handles persistence and file I/O
3. **WorldGenerator/SeedWorldGenerator** - Need regional data during generation

## Requirements Assessment

### Data Volume Estimates

**Regional Scale**: 1km² regions for large worlds
- **Small World** (100km²): ~100 regions = ~100KB-1MB data
- **Medium World** (1,000km²): ~1,000 regions = ~1-10MB data  
- **Large World** (25,000km²): ~25,000 regions = ~25-250MB data

**Data Per Region**:
```cpp
struct RegionalData {
    BiomeType biome;              // 1 byte
    float temperature;            // 4 bytes  
    float humidity;               // 4 bytes
    float elevation;              // 4 bytes
    // Future: geological, hydrological data
    // Estimated: 20-50 bytes per region
};
```

### Performance Requirements

- **Read Access**: < 16ms per region (gameplay critical)
- **Write Access**: Acceptable during world generation (seconds)
- **Streaming**: Efficient loading of adjacent regions
- **Memory**: Reasonable memory usage for large worlds

## Options Analysis

### 1. SQLite ⭐ (RECOMMENDED)

**Pros**:
- Single file storage (easy saves/distribution)
- ACID compliance and reliability
- Excellent tooling and debugging support
- Spatial indices (R-Tree extension) for regional queries
- Cross-platform, no external dependencies
- Mature C++ integration

**Cons**: 
- Small overhead for simple queries
- May be overkill for pure key-value access

**Schema Design**:
```sql
CREATE TABLE regions (
    x INTEGER,
    z INTEGER, 
    biome_type INTEGER,
    temperature REAL,
    humidity REAL,
    elevation REAL,
    -- Future expansion fields
    PRIMARY KEY (x, z)
);

CREATE INDEX region_spatial ON regions(x, z);
```

**Performance Profile**: Excellent for complex queries, good for streaming

### 2. Custom Binary Format + Index

**Pros**:
- Maximum performance and efficiency
- Complete control over format
- No external dependencies
- Ideal for simple record access

**Cons**:
- Custom implementation complexity
- Need to implement indexing/searching
- Versioning challenges
- No existing tooling

**Format Design**:
```
world_regions.idx    (index file)
world_regions.dat    (packed binary records)
```

**Performance Profile**: Highest performance for direct access

### 3. Chunked Files (Current Model Extended)

**Pros**:
- Natural fit with existing architecture
- Simple streaming implementation  
- Good file-level organization
- Easy to understand and debug

**Cons**:
- Many small files for large worlds
- Complex directory management
- No built-in indexing

**File Structure**:
```
world_data/
├── regions_0_0.bin    (covers x:0-31, z:0-31)
├── regions_32_0.bin   (covers x:32-63, z:0-31)
└── regions.idx        (master index)
```

## Benchmark Plan

### Test Implementation

I will implement simple prototypes of the top 2 options (SQLite and Custom Binary) with:

1. **Write Performance**: Insert 10,000 regional records
2. **Read Performance**: Random access to regional data
3. **Range Queries**: Load regions in player vicinity  
4. **Storage Efficiency**: File size comparison

### Test Data

```cpp
struct TestRegionalData {
    int32_t x, z;                    // 8 bytes
    BiomeType biome;                 // 1 byte  
    float temperature, humidity;     // 8 bytes
    float elevation;                 // 4 bytes
    // Total: 21 bytes per region
};
```

## Preliminary Recommendation: SQLite

**Rationale**:
1. **Reliability**: Battle-tested database with ACID properties
2. **Tooling**: Excellent debugging with DB browsers  
3. **Scalability**: Handles millions of records efficiently
4. **Spatial Queries**: Built-in support for regional lookups
5. **Integration**: Simple C++ integration, single file
6. **Future-Proof**: Easily extensible for complex historical data

**Implementation Strategy**:
1. Single SQLite file per world: `world_name/world_data.db`
2. Regional data table with spatial indexing
3. Future tables for structures, history, etc.
4. Integration with existing SaveManager

## Next Steps

1. ✅ **Current**: Complete architecture analysis  
2. 🔄 **Next**: Implement SQLite prototype
3. **Then**: Benchmark performance vs custom binary
4. **Finally**: Final recommendation and implementation plan

## Legacy System Preservation

**CRITICAL**: The new database system will be used only for the **"Create World"** path. The **"Resume Game"** path will continue using the existing save system, ensuring we maintain a working reference implementation.
