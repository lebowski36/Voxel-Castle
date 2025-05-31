# 🏰 Voxel Fortress 🏰

## Welcome to the Voxel Fortress Project!

**Voxel Fortress** is an ambitious colony simulation game that combines the depth of Dwarf Fortress with the accessibility and creative freedom of voxel-based worlds.

![Active Development](https://img.shields.io/badge/Status-Active%20Development-brightgreen) ![Engine](https://img.shields.io/badge/Engine-Custom%20C++-blue) ![Version](https://img.shields.io/badge/Version-0.1.0%20Alpha-orange)

> "Build, dig, survive, and watch stories unfold in a vast, procedurally generated voxel world."

## 🎮 Game Concept

In Voxel Fortress, you'll:

- 👥 **Manage a thriving colony** of unique individuals with personalities, needs, and skills
- 🏗️ **Build epic structures** by digging into mountains or building on the surface
- 🌋 **Explore a vast procedurally generated world** with varied biomes and hidden secrets
- ⚔️ **Defend against threats** from the wilderness and rival factions
- 📈 **Create complex production chains** to craft advanced items and grow your fortress
- 📜 **Experience emergent storytelling** through the interactions of deep simulation systems

## 🧩 Core Features

- **Deep Colony Simulation** - Characters with unique traits, skills, relationships, and needs
- **Voxel-based Building** - Intuitive yet powerful construction system
- **Advanced LOD System** - See for miles across your procedurally generated world
- **Rich Simulation** - From fluid dynamics to complex social interactions
- **Emergent Gameplay** - No two playthroughs will ever be the same

## 📁 Project Documentation

Explore our design and development documentation:

- **[Getting Started Guide](GETTING_STARTED.md)** - Quick setup and overview for new users
- **[Development Guide](DEVELOPMENT.md)** - Architecture overview and development status  
- **[Current Tasks](CURRENT_TODO.md)** - Active development tracking and progress
- [Game Concept Details](docs/design_specifications/Voxel%20Fortress%20Game%20Concept%20Details.md) - Core gameplay pillars and vision
- [Technical Stack Documentation](docs/design_specifications/Voxel%20Fortress%20Tech%20Stack%20(C++%20Custom%20Engine%20Approach).md) - Detailed technical approach
- [Colony Simulation Design](docs/design_specifications/Game%20Concept/Colony%20Simulation.md) - Future gameplay mechanics

## 🚧 Development Status

**Current Phase: Core Engine Development (Active Development)**

Voxel Fortress is currently in active development as a custom C++ engine. The core foundational systems are operational, and we're building towards a playable prototype.

### ✅ Completed Systems
- **Core Engine Foundation**: Window management, OpenGL rendering pipeline, input handling
- **Entity Component System**: Flecs ECS integration for game object management  
- **Voxel World System**: Chunk-based world representation with dynamic loading/unloading
- **Mesh Generation**: Greedy meshing algorithm for efficient voxel rendering
- **Camera System**: Free-flying spectator camera with mouse and keyboard controls
- **World Generation**: Basic procedural terrain generation
- **Physics System**: Collision detection and basic player physics
- **Texture System**: Texture atlas management for block rendering

### 🔄 Currently Working On  
- **Block Placement System**: Interactive block placement and removal (debugging phase)
- **UI Framework**: Block selection UI and game interface elements
- **Game State Management**: Menu system and state transitions

### 🎯 Next Milestones
- Complete interactive block modification system
- Implement basic lighting and shading
- Add player movement and collision in first-person mode
- Expand world generation features

## 💡 Vision & Future

**Current Reality**: Voxel Fortress is a **working voxel engine prototype** with real-time world interaction, providing the foundation for more ambitious features.

**Future Vision**: Voxel Fortress aims to create a perfect blend between:
- The depth and emergent storytelling of Dwarf Fortress
- The accessibility and creativity of Minecraft  
- Modern rendering techniques for breathtaking vistas

### Long-term Goals
- **Deep Colony Simulation**: Complex AI citizens with personalities, needs, and relationships
- **Emergent Storytelling**: Narratives that emerge from system interactions rather than scripts
- **Massive Worlds**: Level-of-detail systems enabling exploration across vast distances
- **Complex Systems**: Realistic physics, economy, and social dynamics

### Development Philosophy  
We believe in creating games where players create their own stories through interactions with complex, interwoven systems. Every technical decision made in the engine development serves this ultimate vision.

**Current Focus**: Building robust foundational systems that can support future complexity while maintaining excellent performance and user experience.

## 🛠️ Building and Running Voxel Fortress

### Quick Start (if you have dependencies)
```bash
# Navigate to project directory
cd "/path/to/Voxel Castle"

# Build and run
cmake --build build && ./build/bin/VoxelFortressGame
```

### First Time Setup
**You'll need**: C++ compiler, CMake 3.20+, and OpenGL drivers

For detailed setup instructions for your operating system, see **[BUILD_GUIDE.md](BUILD_GUIDE.md)**

### Current Features
- **Free-flying Camera**: Explore the procedurally generated voxel world
- **Interactive Blocks**: Place and remove different block types
- **Real-time Rendering**: 60 FPS target with optimized chunk-based rendering
- **Dynamic World**: Chunks load and unload based on camera position

### Controls
- **Mouse**: Look around | **WASD**: Move camera | **Space/Shift**: Up/Down
- **Escape**: Toggle mouse capture | **Left Click**: Place blocks | **Right Click**: Remove blocks
- **Mouse Wheel**: Cycle through block types

> **First time building?** Check [BUILD_GUIDE.md](BUILD_GUIDE.md) for platform-specific setup instructions.

## 🔧 Technology Stack

**Voxel Fortress** is built with a custom C++ engine designed for high performance and control:

### Core Technologies
- **Language**: C++17 for performance, memory control, and mature ecosystem
- **Build System**: CMake for cross-platform compilation
- **Window Management**: SDL3 for cross-platform window and input handling
- **Graphics**: OpenGL 3.3+ with modern rendering techniques
- **Mathematics**: GLM for vector and matrix operations

### Engine Architecture  
- **Entity Component System**: Flecs high-performance ECS for game object management
- **Rendering Pipeline**: Custom voxel rendering with greedy meshing algorithms
- **World Management**: Chunk-based world system with dynamic loading/unloading
- **Memory Management**: Smart pointers and RAII for automatic resource management
- **Multithreading**: Background mesh generation and world loading

### Voxel Technology
- **Chunk System**: 32³ voxel chunks (8m³ world volumes) for efficient processing
- **Mesh Generation**: Greedy meshing for optimized triangle count
- **Level of Detail**: Distance-based chunk management for performance
- **Texture Atlas**: Efficient texture management for all block types

### Development Tools
- **Version Control**: Git with structured development task tracking
- **Debugging**: Comprehensive logging system with performance metrics
- **Profiling**: Frame time tracking and mesh generation statistics

This technology stack provides the foundation for complex simulation systems while maintaining the 60 FPS performance target essential for smooth voxel world interaction.

---

*More details coming soon as development progresses!*