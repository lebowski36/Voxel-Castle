# World Generation UI Specification

*Created: 2025-06-08*  
*Status: SPECIFICATION - TO BE IMPLEMENTED*

## Overview

This document specifies the user interface and experience for world generation in Voxel Castle, inspired by Dwarf Fortress's legendary world generation visualization and process.

**IMPORTANT CLARIFICATION**: Unlike Dwarf Fortress, **the entire world remains fully explorable**. "Active areas" refers only to performance optimization - areas where players are present get detailed voxel data loaded, while the complete world template database exists for the entire world and enables seamless exploration anywhere.

**Related Documentation:**
- [World Generation Architecture](WORLD_GENERATION_ARCHITECTURE.md) - Technical implementation
- [Biome Implementation Plan](BIOME_IMPLEMENTATION_PLAN.md) - Generation systems

## Vision: Dwarf Fortress-Inspired World Generation Experience

### Core Philosophy
Players should witness and understand the **story of their world's creation** through:
- **Real-time visualization** of geological, hydrological, and climatic processes
- **Progressive detail revelation** from continental scale to local features  
- **Interactive parameter control** to customize world characteristics
- **Historical narrative generation** showing how the world evolved

### User Experience Flow

#### Phase 1: World Configuration Screen
**Layout**: Parameter selection interface with preview
- **World Size**: Small (512x512 regions) to Massive (4096x4096 regions)
- **Simulation Depth**: Fast (basic) to Epic (full geological simulation)
- **Climate Type**: Temperate, Tropical, Arctic, Desert-dominated, etc.
- **Geological Activity**: Stable, Moderate, Highly Volcanic/Seismic
- **Hydrology**: Arid, Normal, Water-rich
- **Advanced Seed Controls**: Custom seeds for different generation phases

#### Phase 2: World Generation Visualization
**Layout**: Full-screen generation viewer with progress panels

**Main Visualization Area** (Large central panel):
- **Real-time 2D world map** showing generation progress
- **Color-coded visualization** of current generation phase:
  - **Blue tones**: Tectonic/geological phase (elevation, rock types)
  - **Brown/Green tones**: Erosion phase (valleys, sediment)
  - **Cyan/Blue tones**: Hydrological phase (rivers, lakes, watersheds)
  - **Multi-colored**: Climate/biome assignment phase
- **Progressive zoom capability** - start continental, zoom to regional detail
- **Time-lapse effect** - show changes over geological time

**Generation Progress Panel** (Right side):
- **Current Phase Indicator**: "Simulating Tectonic Activity..."
- **Detailed Progress Bars**:
  - Overall Progress: 34%
  - Current Phase: "Hydraulic Erosion" - 67%
  - Sub-process: "River Network Formation" - 23%
- **Generation Statistics**:
  - Mountain Ranges Created: 12
  - Major Rivers: 47
  - Biomes Identified: 8
  - Simulation Years Elapsed: 50,000
- **Time Estimates**:
  - Current Phase: 2m 30s remaining
  - Total Generation: 8m 15s remaining

**Generation Log Panel** (Bottom):
- **Scrolling text log** similar to Dwarf Fortress:
  ```
  [Year 45,231] The Dragonspine Mountains rise from tectonic collision
  [Year 47,892] The Whisperflow River carves through the eastern valleys
  [Year 48,156] Massive lake 'Mirrormere' forms in central depression
  [Year 49,834] Dense forests spread across the humid northern regions
  [Year 50,000] Ancient civilization 'The Stone Builders' establishes settlements
  ```

**Interactive Controls** (Left side):
- **Pause/Resume Generation** button
- **Visualization Mode Toggle**:
  - Elevation
  - Temperature
  - Precipitation  
  - Biomes
  - Rivers/Hydrology
  - Geological Composition
- **Speed Control**: 1x to 16x generation speed
- **Layer Toggles**: Show/hide specific features
- **Export Progress**: Save intermediate world states

#### Phase 3: World Summary & Exploration
**Layout**: Generated world overview with statistics and notable features

**World Overview Map** (Large panel):
- **Final generated world** with all systems integrated
- **Interactive exploration**: Click regions to see details
- **Notable locations highlighted**: Unique geological features, ideal settlement sites
- **Zoom levels**: Continental → Regional → Local preview

**World Statistics Panel**:
- **Geological Summary**:
  - Highest Peak: "Mount Skyreach" (2,847m)
  - Deepest Valley: "Shadowrift Gorge" (-127m)
  - Largest Lake: "Mirrormere" (15.7 km²)
  - Longest River: "The Lifeflow" (342 km)
- **Climate Summary**:
  - Temperature Range: -23°C to 41°C
  - Driest Region: "Sunscorch Desert" (12mm/year)
  - Wettest Region: "Mistral Rainforest" (3,200mm/year)
- **Biome Distribution**:
  - Forests: 34%
  - Plains: 28%
  - Mountains: 18%
  - Desert: 12%
  - Other: 8%

**Notable Features Panel**:
- **Unique Locations**: Auto-generated points of interest
  - "The Crystal Caves of Deepholm"
  - "Stormwind Pass - The Highest Mountain Trail"
  - "The Floating Islands of Aethermoor"
- **Optimal Settlement Locations**: AI-identified ideal building sites
- **Resource Hotspots**: Rich mineral deposits, rare materials
- **Natural Wonders**: Waterfalls, hot springs, unique geological formations

**Action Buttons**:
- **Begin Game**: Start playing in this world
- **Export World**: Save world template for sharing
- **Regenerate**: Create new world with same parameters
- **Modify**: Return to parameter selection with current world as base

### Technical Implementation Notes

#### Visualization Rendering
- **2D Heightmap Rendering**: Real-time colored heightmap display
- **Progressive Detail Loading**: Show continental outline → regional features → local details
- **Smooth Transitions**: Animated changes during generation phases
- **Multiple Data Layer Support**: Switch between elevation, temperature, biome, etc.

#### Performance Considerations
- **Background Generation**: World generation runs on separate thread
- **Visualization Updates**: Refresh visualization every 100ms during generation
- **Memory Management**: Stream visualization data to avoid memory bloat
- **Cancellation Support**: Allow users to cancel generation and return to parameters

#### UI Framework Integration
- **Immediate Mode GUI**: Use existing ImGui system for panels and controls
- **Custom Rendering**: Direct OpenGL rendering for world visualization
- **Event System**: Handle user interactions with world map
- **State Management**: Preserve UI state during generation process

### Future Enhancements

#### Advanced Visualization Features
- **3D World Preview**: Optional 3D terrain view during generation
- **Historical Playback**: Replay world evolution from any point
- **Cross-Section Views**: See geological layers and underground features
- **Comparative Views**: Side-by-side comparison of different generation parameters

#### Interactive Generation
- **Manual Intervention**: Allow users to guide generation at key points
- **Disaster Simulation**: Add volcanic eruptions, meteor impacts, etc.
- **Civilization Interaction**: Influence historical civilization development
- **Real-time Parameter Adjustment**: Modify parameters during generation

#### Sharing & Community
- **World Sharing**: Upload/download world templates
- **Generation Replays**: Share time-lapse videos of world creation
- **Community Challenges**: Generate worlds meeting specific criteria
- **World Rating System**: Community rating of generated worlds

## Implementation Priority

**Phase 1 (Essential)**: Basic generation visualization with progress tracking
**Phase 2 (Important)**: Interactive controls and detailed statistics  
**Phase 3 (Polish)**: Advanced visualization modes and world exploration
**Phase 4 (Future)**: Community features and advanced interactions

This specification ensures that Voxel Castle provides an engaging, informative, and beautiful world generation experience that rivals or exceeds Dwarf Fortress while fitting our modern 3D voxel architecture.
