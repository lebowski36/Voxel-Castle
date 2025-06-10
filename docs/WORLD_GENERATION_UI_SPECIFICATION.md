# World Generation UI Specification

*Created: 2025-06-08*  
*Last Updated: 2025-06-10*  
*Status: ACTIVE IMPLEMENTATION - Phase 4B*

## Overview

This document specifies the user interface and experience for world generation in Voxel Castle, inspired by Dwarf Fortress's legendary world generation visualization and process.

**IMPORTANT CLARIFICATION**: Unlike Dwarf Fortress, **the entire world remains fully explorable**. "Active areas" refers only to performance optimization - areas where players are present get detailed voxel data loaded, while the complete world template database exists for the entire world and enables seamless exploration anywhere.

**Related Documentation:**
- [World Generation Architecture](WORLD_GENERATION_ARCHITECTURE.md) - Technical implementation
- [Biome Implementation Plan](BIOME_IMPLEMENTATION_PLAN.md) - Generation systems
- [Implementation Progress](../development_tasks/04_essential_game_infrastructure/08f_world_generation_ui.md) - Current status

## Architecture Overview

### **Dual-UI System (Implemented)**
The system is split into two specialized UIs following the Dwarf Fortress approach:

1. **WorldConfigurationUI**: Parameter selection **without preview** (follows DF configuration approach)
2. **WorldSimulationUI**: Real-time world generation visualization with progress tracking

### **Three-Phase User Experience**
1. **Configuration Phase**: Parameter selection only (no preview) ✅ **IMPLEMENTED**
2. **Generation Phase**: Full world map visualization during simulation 🔄 **IN PROGRESS**  
3. **Embark Phase**: Browse completed world to select spawn location (future)

## Implementation Status & Current Reality

### ✅ **What's Currently Working**
- **World Map Visualization**: 512x512 heightmap rendering correctly positioned
- **Generation Phases**: All 6 phases execute (Tectonics → Erosion → Water → Climate → Biomes → Civilization)
- **Tectonic Simulation**: Real geological simulation (15-21 plates, collision detection, stress calculation)
- **UI Integration**: WorldMapRenderer connected to WorldSimulationUI
- **Progress Tracking**: Phase progression with progress bars and time estimates
- **World Persistence**: Generated worlds saved to `./worlds/` directory

### ❌ **Current Implementation Gaps**
- **Static Visualization**: Same heightmap shown throughout all phases (elevation range: 29-380m unchanging)
- **Missing Water**: No blue areas for rivers, lakes, or oceans
- **No Phase-Specific Colors**: Always shows elevation mode instead of switching per phase
- **Tectonic Disconnect**: Tectonic simulation results not applied to heightmap
- **No Progressive Changes**: Terrain doesn't visually evolve during generation

## Vision: Expected Behavior vs Current Reality

### **What Users Should See (Target Implementation)**

#### **Phase 1: Tectonic Formation**
- **Expected**: Dramatic terrain changes, mountains rising, valleys forming
- **Current**: Static elevation map (29-380m range unchanged)
- **Needed**: Apply tectonic stress/collision results to heightmap data

#### **Phase 2: Erosion & Weathering**  
- **Expected**: Smoothed peaks, carved valleys, realistic erosion patterns
- **Current**: Same static heightmap from Phase 1
- **Needed**: Erosion algorithms that modify terrain data

#### **Phase 3: Water Systems**
- **Expected**: **BLUE AREAS** for rivers, lakes, oceans with water depth visualization
- **Current**: No water visualization, only elevation colors
- **Needed**: Water body generation + blue color mapping

#### **Phase 4: Climate Simulation**
- **Expected**: Temperature/precipitation gradients, climate zone visualization
- **Current**: Only elevation colors shown  
- **Needed**: Auto-switch to temperature/precipitation visualization modes

#### **Phase 5: Biome Assignment**
- **Expected**: **GREEN forests, YELLOW deserts, WHITE tundra** based on climate
- **Current**: Still elevation colors only
- **Needed**: Auto-switch to biome visualization mode with realistic colors

#### **Phase 6: Civilization History**
- **Expected**: Settlement markers, roads, areas of human influence
- **Current**: No civilization features visible
- **Needed**: Civilization data generation + visualization overlay

### **Core Philosophy**
Players should witness and understand the **story of their world's creation** through:
- **Real-time visualization** of geological, hydrological, and climatic processes
- **Progressive detail revelation** from continental scale to local features  
- **Interactive parameter control** to customize world characteristics
- **Historical narrative generation** showing how the world evolved

## User Experience Flow

### Phase 1: World Configuration Screen ✅ **IMPLEMENTED**
**Layout**: Parameter selection interface **without preview** (Dwarf Fortress approach)
- **World Size**: Small (1024km²) to Massive (2048km²) ✅ Working
- **Simulation Depth**: Fast to Epic (affects tectonic plate count) ✅ Working  
- **Climate Type**: Arctic, Temperate, Tropical, Desert ✅ Working
- **Geological Activity**: Stable, Moderate, Highly Active ✅ Working
- **Hydrology**: Arid, Normal, Water-rich ✅ Working
- **World Name**: Text input with validation ✅ Working

**Status**: ✅ **COMPLETE** - All controls functional, no preview (follows DF pattern)

### Phase 2: World Generation Visualization 🔄 **IN PROGRESS**
**Layout**: Full-screen generation viewer with progress panels

**Main Visualization Area** (Large central panel): ✅ **Partially Working**
- **Real-time 2D world map**: ✅ 512x512 heightmap rendering correctly  
- **Color-coded visualization**: ❌ **MISSING** - Only elevation mode, no phase-specific colors
- **Progressive changes**: ❌ **MISSING** - Same data throughout all phases
- **Auto-mode switching**: ❌ **MISSING** - Doesn't switch visualization per phase

**Expected Phase-Specific Visualization**:
- **Tectonic Phase**: Elevation changes showing mountain formation
- **Erosion Phase**: Smoothed terrain with valley carving
- **Water Phase**: **Blue tones** for rivers, lakes, watersheds  
- **Climate Phase**: Temperature/precipitation color gradients
- **Biome Phase**: **Multi-colored** biome assignments (green/brown/white)
- **Civilization Phase**: Settlement and infrastructure markers

**Generation Progress Panel** (Right side): ✅ **Working**
- **Current Phase Indicator**: ✅ Shows "Tectonic Formation", "Erosion & Weathering", etc.
- **Progress Bars**: ✅ Phase and overall progress tracking functional
- **Generation Statistics**: 🔄 Basic stats working, needs enhancement
- **Time Estimates**: ✅ Phase and total time estimation working

**Generation Log Panel** (Bottom): ✅ **Working**
- **Scrolling text log**: ✅ Phase transitions logged
- **Geological events**: 🔄 Basic logging, needs narrative enhancement

**Interactive Controls** (Left side): ✅ **Working**  
- **Visualization Mode Toggle**: ✅ Buttons implemented (Elevation, Temperature, Precipitation, Biomes, Hydrology)
- **Mode Switching**: ❌ **NOT CONNECTED** - Buttons don't affect visualization yet
- **Pause/Resume**: ✅ Functional
- **Speed Control**: ✅ Functional

### Phase 3: World Summary & Exploration ✅ **IMPLEMENTED**
**Layout**: Generated world overview with statistics and notable features

**World Statistics**: ✅ **Working**
- **Geological Summary**: ✅ Mount Skyreach height, basic terrain stats
- **World Persistence**: ✅ Worlds saved to `./worlds/` directory  
- **Action Buttons**: ✅ Back button functional, "Play World" ready for implementation

## Critical Implementation Issues & Solutions

### **Issue 1: Static Visualization Data**
**Problem**: Same elevation range (29-380m) shown throughout all phases
**Root Cause**: WorldMapRenderer generates static noise-based heightmap, ignores simulation results
**Solution Needed**: Connect tectonic simulation output to visualization data

### **Issue 2: Missing Water Visualization**  
**Problem**: No blue areas for water bodies despite "Water Systems" phase
**Root Cause**: No water generation in WorldMapRenderer, only elevation data
**Solution Needed**: Add water body detection + blue color mapping for low elevation areas

### **Issue 3: No Auto-Mode Switching**
**Problem**: Always shows elevation colors regardless of generation phase  
**Root Cause**: WorldMapRenderer doesn't automatically switch visualization modes per phase
**Solution Needed**: Phase-driven automatic mode switching in renderWorldMap()

### **Issue 4: Tectonic Results Ignored**
**Problem**: Tectonic simulation runs correctly but results don't affect heightmap
**Root Cause**: No data flow from TectonicSimulator results to WorldMapRenderer
**Solution Needed**: Apply tectonic stress/elevation changes to terrain data
## Technical Implementation Roadmap

### **Priority 1: Connect Simulation Results to Visualization** 🔥 **HIGH PRIORITY**
**Files**: `engine/src/ui/WorldMapRenderer.cpp`, `engine/src/world/seed_world_generator.cpp`
**Goal**: Make terrain visually change based on actual simulation results

**Tasks**:
1. **Apply Tectonic Results**: Connect `TectonicSimulator` elevation changes to heightmap
2. **Progressive Data Updates**: Update world map data after each phase completes  
3. **Elevation Range Changes**: Show increasing terrain complexity (e.g., 0m→600m+ range)

### **Priority 2: Implement Water Visualization** 🔥 **HIGH PRIORITY**  
**Files**: `engine/src/ui/WorldMapRenderer.cpp`
**Goal**: Show blue areas for water bodies during Water Systems phase

**Tasks**:
1. **Water Body Generation**: Detect low-elevation areas for water placement
2. **Blue Color Mapping**: Add water-specific color scheme (blue tones)
3. **Depth Visualization**: Different blue shades for water depth variation

### **Priority 3: Auto-Mode Switching** 🔥 **HIGH PRIORITY**
**Files**: `engine/src/ui/WorldSimulationUI.cpp`, `engine/src/ui/WorldMapRenderer.cpp`  
**Goal**: Automatically switch visualization modes per generation phase

**Tasks**:
1. **Phase-Driven Mode Selection**: Auto-switch visualization based on current phase
2. **Mode Implementation**: Ensure Temperature, Precipitation, Biome modes work correctly
3. **Smooth Transitions**: Visual transitions between different visualization modes

### **Priority 4: Enhanced Generation Statistics** 🔶 **MEDIUM PRIORITY**
**Files**: `engine/src/ui/WorldSimulationUI.cpp`
**Goal**: Show meaningful generation statistics instead of placeholders

**Tasks**:
1. **Real Data Integration**: Connect actual generation data to statistics display
2. **Feature Counting**: Track mountains, rivers, biomes created during generation
3. **Dynamic Updates**: Update statistics as features are generated

## Implementation Dependencies & Architecture

### **Current Working Components** ✅
- **UI Framework**: WorldSimulationUI layout and controls functional  
- **Rendering Pipeline**: WorldMapRenderer successfully renders textures
- **Tectonic Simulation**: Real geological simulation with 15-21 plates
- **Phase Progression**: All 6 generation phases execute correctly
- **World Persistence**: Generated worlds saved and retrievable

### **Data Flow Architecture**
```
SeedWorldGenerator → [MISSING CONNECTION] → WorldMapRenderer → WorldSimulationUI
     ↓                                            ↓                ↓
TectonicSimulator    [Simulation Results]    Visualization    UI Display
```

**Current Gap**: Simulation results not connected to visualization data

### **File Structure**
```
engine/src/ui/
├── WorldConfigurationUI.cpp     ✅ Complete (parameter selection)
├── WorldSimulationUI.cpp        🔄 Needs data integration  
└── WorldMapRenderer.cpp         🔄 Needs simulation connection

engine/src/world/
├── seed_world_generator.cpp     ✅ Simulation working
└── tectonic_simulator.cpp       ✅ Results available but unused
```

## Future Enhancements (Post-Core Implementation)

### **Advanced Visualization Features**
- **3D World Preview**: Optional 3D terrain view during generation
- **Historical Playback**: Replay world evolution from any point  
- **Cross-Section Views**: See geological layers and underground features
- **Interactive Zoom/Pan**: Explore different areas during generation

### **Enhanced Generation Features**  
- **Manual Intervention**: Allow users to guide generation at key points
- **Disaster Simulation**: Add volcanic eruptions, meteor impacts
- **Civilization Interaction**: Influence historical civilization development
- **Real-time Parameter Adjustment**: Modify parameters during generation

## Success Criteria

### **Phase 4B Completion Criteria**
1. ✅ **Visual Changes During Simulation**: Terrain dramatically changes between phases
2. ✅ **Water Visualization**: Blue areas visible during Water Systems phase  
3. ✅ **Auto-Mode Switching**: Different visualizations per phase automatically
4. ✅ **Progressive Terrain**: Elevation range increases during Tectonic phase

### **User Experience Validation**
- **Dramatic Visual Storytelling**: Users can see their world's creation story unfold
- **Intuitive Phase Understanding**: Clear visual distinction between generation phases  
- **Engaging Process**: Generation feels dynamic and interesting, not static
- **Technical Accuracy**: Visualization accurately represents simulation results

This specification reflects the current reality while providing a clear roadmap for completing the Dwarf Fortress-inspired world generation visualization experience.
