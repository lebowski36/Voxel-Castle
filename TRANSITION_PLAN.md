# Transition Plan: From Bevy to Custom Engine Components

## Overview

Based on our project requirements and the challenges encountered with Bevy 0.15.3's UI system, we're transitioning to a more custom approach using lower-level components. This aligns better with the original "Custom Engine" philosophy outlined in our technical documentation.

## Rationale

1. **API Stability Issues**: Bevy's rapid development pace has led to breaking changes and deprecations that impact our development workflow.
2. **Specialized Requirements**: Our voxel-based colony simulation has unique needs for rendering, LOD systems, and large-scale world management.
3. **Control and Flexibility**: A more custom approach gives us greater control over performance optimizations crucial for voxel worlds.
4. **Alignment with Project Vision**: Our technical documentation already outlines plans for custom systems built on core low-level libraries.

## Technical Approach

Instead of using Bevy as a complete engine, we'll adopt a composable architecture using these foundational components:

1. **Graphics/Rendering**: wgpu (directly rather than through Bevy)
2. **Window Management**: winit
3. **ECS Framework**: Legion or hecs (alternatives to bevy_ecs with more stable APIs)
4. **Physics**: Rapier (as originally planned)
5. **Math**: glam or nalgebra
6. **Custom Systems**:
   - Voxel data structures and management
   - Specialized LOD system for voxel rendering
   - Colony simulation modules
   - Custom UI framework tailored to our needs

## Migration Strategy

Rather than a complete rewrite, we'll implement a phased transition:

1. **Prototype Phase**: Create a minimal working prototype with the new stack
2. **Parallel Development**: Continue limited development in the current Bevy project while building the new foundation
3. **Component Migration**: Move components one by one from Bevy to our custom framework
4. **Full Transition**: Complete the transition once core functionality is working in the new system

## Expected Benefits

1. **Increased Stability**: Reduced dependency on rapidly changing APIs
2. **Better Performance**: More control over rendering and memory usage for voxel-specific optimizations
3. **Customized Solutions**: Systems designed specifically for our simulation and voxel rendering needs
4. **Cleaner Architecture**: More explicit component relationships without working around engine limitations

## Risks and Mitigations

| Risk | Mitigation |
|------|------------|
| Increased development time | Focus on core systems first, incremental implementation |
| Missing engine features | Evaluate and implement only essential features |
| Reinventing the wheel | Leverage existing libraries where appropriate |
| Knowledge gaps | Research and small prototypes before full implementation |

This transition aligns with our technical roadmap and will provide a more suitable foundation for the ambitious scope of Voxel Fortress.
