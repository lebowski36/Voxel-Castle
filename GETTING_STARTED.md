# Getting Started with Voxel Fortress

## Quick Overview

Voxel Fortress is a **voxel-based colony simulation game** built with a custom C++ engine. Think Minecraft's creative building meets Dwarf Fortress's deep simulation systems.

## What Works Right Now

The engine is currently in **active development** with these working features:

### ✅ Playable Features
- **3D Voxel World**: Procedurally generated terrain with chunk-based loading
- **Free Camera**: Fly around the world with smooth mouse and keyboard controls  
- **Block Building**: Place and remove different block types in real-time
- **Real-time Rendering**: 60 FPS performance with optimized mesh generation

### 🎮 Current Controls
- **Mouse**: Look around (auto-captured)
- **WASD**: Move camera
- **Space/Shift**: Up/Down movement
- **Escape**: Release mouse cursor
- **Left Click**: Place blocks
- **Right Click**: Remove blocks  
- **Mouse Wheel**: Change block type

## 5-Minute Setup

### Dependencies

All external dependencies are included in the repository under the `external/` directory:

- **SDL3**: Window management and input handling
- **GLM**: Mathematics library for graphics
- **GLAD**: OpenGL loader
- **Flecs**: Entity Component System framework

You don't need to download or install these separately.

### Building the Project

```bash
# 1. Navigate to project
cd "/path/to/Voxel Castle"

# 2. Create build directory if it doesn't exist
mkdir -p build && cd build

# 3. Configure with CMake
cmake ..

# 4. Build the engine
cmake --build .

# 5. Run the game
./bin/VoxelFortressGame
```

You should see a 3D world with terrain. Use the mouse to look around and WASD to fly!

> **System Dependencies**: You'll still need a C++ compiler, CMake, and OpenGL development libraries installed on your system. See [BUILD_GUIDE.md](BUILD_GUIDE.md) for detailed instructions for your specific operating system.

## What You'll See

- **Procedural Terrain**: Hills, valleys, and basic landscape
- **Chunk Loading**: World loads around you as you move
- **Debug Output**: Console shows frame rates and chunk information
- **Block Interaction**: Point at surfaces to place blocks, or at blocks to remove them

## Next Steps for Developers

### Understanding the Codebase
- **`engine/`**: Core systems (rendering, world management, ECS)
- **`game/`**: Game-specific logic and main executable
- **`development_tasks/`**: Detailed feature development tracking

### Current Development Focus
We're currently working on:
- **UI System**: Block selection interface
- **Player Physics**: First-person character controller
- **Game States**: Menu system and state management

### Contributing
1. Check `CURRENT_TODO.md` for active tasks
2. Review `development_tasks/` for detailed sub-tasks
3. Follow the established C++ coding patterns
4. Test thoroughly and update documentation

## Architecture Highlights

- **Custom C++ Engine**: Built for performance and control
- **ECS Architecture**: Using Flecs for entity management
- **Chunk-based World**: 32³ voxel chunks for efficient processing
- **Greedy Meshing**: Optimized rendering for block worlds
- **Smart Pointers**: Modern C++ memory management

## Vision

While currently a tech demo, Voxel Fortress aims to become a deep colony simulation where you:
- Build and manage thriving settlements
- Design complex production chains  
- Navigate emergent narrative through character interactions
- Explore vast procedurally generated worlds

The current engine work is building the foundation for these ambitious simulation systems.

---

**Questions?** Check the main `README.md` for detailed build instructions or `DEVELOPMENT.md` for architectural details.
