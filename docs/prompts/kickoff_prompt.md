# 🚀 Kickoff Prompt for New Features

Use this template when starting work on a new system:

```
@workspace I'm implementing [FEATURE_NAME] for Voxel Castle. Please review:

1. Current architecture in relevant files
2. Our 32³ chunk system and 0.25m voxel constraints  
3. Performance requirements (60 FPS target)
4. Integration with Flecs ECS
5. OpenGL 3.3+ rendering pipeline

Generate code that:
- Follows our C++17 standards
- Maintains chunk size consistency
- Uses existing logging/profiling systems
- Integrates with current mesh generation
- Respects our coordinate system (Y-up)

Context: [Brief description of what you're building]
```
