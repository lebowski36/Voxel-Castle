# New World Generation Plan

*Created: 2025-06-21*
*Status: Proposed*

## 1. Overview & Philosophy

This document outlines a revised, pragmatic plan for developing the Voxel Castle world generation system. We are pivoting away from the complex, simulation-based approach and embracing the robust, procedural generation system defined in `PROCEDURAL_TERRAIN_DESIGN.md`.

**Core Principles:**
- **Focus on Playability:** Prioritize getting a functional, interesting, and performant world running.
- **Incremental Development:** Build features step-by-step, starting with a solid foundation and adding complexity gradually.
- **User-Driven Biome Design:** Each biome will be developed in a tight feedback loop with the user.
- **Defer Complexity:** Advanced, high-risk features like realistic river networks and geological simulation are postponed until the core system is stable and feature-rich.

## 2. The Dual-Path Approach

As per `CURRENT_TODO.md`, we will maintain two separate generation paths to ensure stability:

1.  **Legacy Path (`world_generator.cpp`):**
    *   **UI:** "Resume Game (Legacy)" button.
    *   **Functionality:** Remains untouched. Provides a stable baseline.

2.  **New Procedural Path (`seed_world_generator.cpp`):**
    *   **UI:** "Create World" and "Load World" buttons.
    *   **Functionality:** All new development will happen here. This will become the primary way to experience Voxel Castle.

## 3. Development Roadmap

### Phase 1: Foundation & Feature Parity

The goal of this phase is to create a new world generation system that is at least as good as the legacy one, but with a better foundation for future expansion, improved performance, and some initial variety.

**Task 1.1: UI Overhaul**
- [ ] Modify the main menu UI.
- [ ] Rename the existing "Resume Game" to "Resume Game (Legacy)".
- [ ] Add a new "Create World" button (will trigger the new generator).
- [ ] Add a new "Load World" button (will load worlds created by the new generator).

**Task 1.2: Foundational Terrain Generation**
- [ ] Implement the 4-scale noise system (`Continental`, `Regional`, `Local`, `Micro`) as described in `PROCEDURAL_TERRAIN_DESIGN.md`.
- [ ] This will be implemented in `engine/src/world/seed_world_generator.cpp`.
- [ ] The output should be basic continents, oceans, and varied elevation, achieving feature parity with the legacy generator's noise.

**Task 1.3: Core Biome System**
- [ ] Create a `Biome` base class and a `BiomeRegistry`.
- [ ] Implement a simple `BiomeSelector` that uses elevation and a single climate noise map (e.g., temperature) to assign biomes.
- [ ] **Initial Biomes:**
    - [ ] `OceanBiome`: For areas below sea level.
    - [ ] `PlainsBiome`: For low-lying, flat-ish land.
    - [ ] `ForestBiome`: For mid-elevation areas.
    - [ ] `HillsBiome`: For more rugged terrain.
    - [ ] `MountainsBiome`: For high-elevation peaks.

**Task 1.4: Chunk Management & Performance**
- [ ] Investigate the current chunk loading and management system.
- [ ] Implement optimizations to support a wider visible area of chunks (e.g., 16-32 chunks view distance).
- [ ] Profile and optimize the chunk generation pipeline to ensure smooth performance.

### Phase 2: Iterative Biome Development

This phase will be a continuous loop of development and user feedback. For each new biome, we will follow this process:

1.  **Discuss:** Clarify the user's vision for the biome (aesthetics, features, rarity).
2.  **Implement:** Add the biome to the `BiomeRegistry` and implement its specific features (terrain modifications, vegetation, decorations, etc.).
3.  **Review:** Present the result to the user for feedback and iteration.

**Biome Wishlist (To be prioritized with user):**
- Desert
- Jungle
- Taiga
- Savanna
- Swamp
- Tundra
- Fantasy/Magical Biomes (e.g., Crystal Caves, Floating Islands)

### Phase 3: Advanced Features (Postponed)

Once the world is diverse and stable, we can revisit these complex features.

- **River Networks:** A simplified version might be tackled first.
- **Cave Systems:** A dedicated procedural system for underground networks.
- **Advanced Climate Simulation:** Incorporating wind, moisture, and seasons.
- **Structures & Points of Interest:** Villages, dungeons, etc.
