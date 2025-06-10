# Voxel Castle Engine API Documentation

Welcome to the comprehensive API documentation for Voxel Castle Engine. This documentation follows modern game engine documentation best practices, providing clear examples, performance notes, and practical usage guidance.

## 📖 Documentation Structure

### Core Systems
- **[World Generation API](world_generation/README.md)** - Comprehensive world creation and management systems
- **[Engine Core API](engine_core/README.md)** - Fundamental engine systems and utilities
- **[Rendering API](rendering/README.md)** - Graphics, shaders, and visual effects
- **[Physics API](physics/README.md)** - Physics simulation and collision detection
- **[Audio API](audio/README.md)** - Sound effects, music, and 3D audio
- **[UI API](ui/README.md)** - User interface and menu systems

### Specialized Systems
- **[Geological Simulation API](world_generation/geological_simulation.md)** - Advanced terrain generation
- **[Voxel API](voxel_system/README.md)** - Voxel manipulation and chunk management
- **[Networking API](networking/README.md)** - Multiplayer and networking features
- **[Mod API](modding/README.md)** - Game modification and extension systems

## 🎯 Documentation Philosophy

### For Game Developers
Our API documentation is designed for **practical game development**:
- **Example-First**: Every API call includes working code examples
- **Performance-Aware**: Performance characteristics and optimization tips included
- **Use-Case Driven**: Organized by what you want to accomplish, not just class hierarchy
- **Best Practices**: Recommended patterns and common pitfalls highlighted

### Documentation Standards
Following industry best practices from Unity, Unreal Engine, and modern game engines:
- **Consistent Format**: All APIs follow the same documentation template
- **Version Tracking**: API changes and deprecation notices clearly marked
- **Cross-References**: Extensive linking between related systems
- **Search-Friendly**: Organized for easy navigation and search

## 🚀 Quick Start Guides

### New to Voxel Castle?
1. **[Engine Overview](overview/engine_architecture.md)** - Understanding the engine structure
2. **[Your First World](tutorials/first_world.md)** - Create and explore a basic world
3. **[Basic Scripting](tutorials/basic_scripting.md)** - Core programming concepts
4. **[Common Patterns](tutorials/common_patterns.md)** - Frequently used code patterns

### Migrating from Other Engines?
- **[From Minecraft Modding](migration/from_minecraft.md)** - Familiar concepts and differences
- **[From Unity](migration/from_unity.md)** - Component systems and scene management
- **[From Unreal](migration/from_unreal.md)** - Blueprint equivalents and C++ patterns

## 📚 Reference Categories

### By Experience Level
- **[Beginner](reference/beginner/README.md)** - Essential APIs for getting started
- **[Intermediate](reference/intermediate/README.md)** - Advanced features and optimization
- **[Expert](reference/expert/README.md)** - Engine internals and custom systems

### By Development Phase
- **[Prototyping](workflows/prototyping.md)** - Rapid iteration and testing
- **[Production](workflows/production.md)** - Optimized, stable development
- **[Debugging](workflows/debugging.md)** - Troubleshooting and profiling tools

## 🔧 Development Tools

### API Browser
- **[Class Browser](tools/class_browser.md)** - Complete class hierarchy and relationships
- **[Function Reference](tools/function_reference.md)** - Alphabetical function listing
- **[Event System](tools/event_system.md)** - Custom events and messaging

### Testing and Profiling
- **[Unit Testing Guide](tools/testing.md)** - Writing tests for your game logic
- **[Performance Profiling](tools/profiling.md)** - Optimization tools and techniques
- **[Memory Management](tools/memory.md)** - Best practices for memory usage

## 🎮 Game-Specific APIs

### Voxel Castle Features
- **[Civilization System](game_features/civilizations.md)** - AI civilizations and interactions
- **[Economic System](game_features/economics.md)** - Trade, resources, and economy
- **[Building System](game_features/building.md)** - Construction and architecture
- **[Exploration System](game_features/exploration.md)** - Discovery and progression

### Modding and Extensions
- **[Mod Framework](modding/framework.md)** - Creating and distributing mods
- **[Custom Blocks](modding/custom_blocks.md)** - Adding new voxel types
- **[Custom UI](modding/custom_ui.md)** - Creating mod interfaces
- **[Asset Pipeline](modding/assets.md)** - Custom models, textures, and sounds

## 📋 API Standards and Conventions

### Naming Conventions
```cpp
// Classes: PascalCase
class WorldGenerator;

// Functions: camelCase
void generateChunk();

// Constants: UPPER_SNAKE_CASE
const int MAX_CHUNK_SIZE = 32;

// Members: camelCase with underscore suffix
private:
    float sampleSpacing_;
```

### Error Handling
```cpp
// Use exceptions for unrecoverable errors
if (!isValid()) {
    throw std::runtime_error("Invalid world state");
}

// Use return values for expected failures
std::optional<Chunk> loadChunk(int x, int z);
```

### Performance Documentation
Every performance-critical API includes:
- **Time Complexity**: O(n) notation where applicable
- **Memory Usage**: Typical and maximum memory requirements
- **Thread Safety**: Concurrent usage guidelines
- **Optimization Tips**: Best practices for performance

## 🔍 Search and Navigation

### Finding What You Need
- **[API Index](reference/index.md)** - Alphabetical listing of all APIs
- **[Topic Index](reference/topics.md)** - APIs organized by functionality
- **[Example Index](examples/index.md)** - Code examples by use case
- **[Troubleshooting](support/troubleshooting.md)** - Common issues and solutions

### Version Information
- **Current Version**: 1.0.0-beta
- **API Stability**: Beta (subject to changes)
- **Backward Compatibility**: Maintained for minor versions
- **Migration Guides**: Available for breaking changes

---

## 🤝 Contributing to Documentation

We welcome contributions to improve this documentation:
- **[Documentation Guidelines](contributing/documentation.md)** - Style guide and standards
- **[Example Contributions](contributing/examples.md)** - Adding code examples
- **[Translation](contributing/translation.md)** - Multi-language support

---

*This documentation is generated and maintained as part of the Voxel Castle Engine project. For the latest updates, visit our [GitHub repository](https://github.com/voxelcastle/engine).*
