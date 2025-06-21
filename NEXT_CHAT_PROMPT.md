# NEW CHAT PROMPT: Fix Voxel Castle River Network System

## CONTEXT
The Voxel Castle ProceduralTerrain system has a broken river network implementation that needs critical architectural fixes. The system currently generates scattered noise-based "river effects" instead of realistic connected river networks.

## CURRENT STATE
- **Engine**: C++ backend with disconnected river systems (flow accumulation vs river networks)
- **Wrapper**: Python worldgen_cpp module built and working
- **Problem**: River visualization shows 100% terrain as "rivers" or 0% depending on thresholds
- **Root Issue**: Two separate river systems don't communicate, creating noise patterns instead of linear river channels

## CRITICAL TASK
Fix the river network architecture following the detailed analysis in `docs/RIVER_NETWORK_CRITICAL_FIXES.md`. The main issues are:

1. **Disconnected Systems**: Flow accumulation (carving) vs River networks (paths) operate separately
2. **Region Mismatch**: 100km river regions vs 25km test areas don't align  
3. **No Linear Rivers**: System creates scattered points, not connected river paths
4. **Broken Visualization**: Rivers should appear as blue curves on terrain, currently invisible

## REQUIRED DELIVERABLE
Working river system that generates **realistic connected river networks** visible as **blue curved channels** on terrain visualizations with **proper valley carving**.

## FILES TO EXAMINE
- `docs/RIVER_NETWORK_CRITICAL_FIXES.md` - Complete analysis and fix requirements
- `engine/src/world/procedural_terrain/features/river_networks.cpp` - Main river implementation  
- `test_new_river_system.py` - Test script showing current broken state
- `debug_rivers.py` - Shows rivers detected at every point (100% coverage issue)

## SUCCESS CRITERIA
- Rivers appear as connected blue curves (not scattered points)
- River carving creates visible valleys following river paths
- Realistic river density (1-5% coverage, not 0% or 100%)
- Rivers flow downhill from sources to oceans/lakes

Start by examining the critical fixes document and implementing the unified river system architecture.
