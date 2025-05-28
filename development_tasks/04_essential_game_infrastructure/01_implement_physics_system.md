# Task: Implement Physics System

## Status: 🔄 IN PROGRESS

## Overview
Implement a basic physics system to handle entity movement, collision detection, and basic world physics interactions. This will provide the foundation for player movement, falling objects, and entity-world interactions.

## Implementation Phases

### Phase 1: Core Physics Infrastructure ⏳ PENDING
**Goal:** Set up the basic physics system architecture and integration with ECS.

- [ ] **1.1: Choose Physics Engine**
  - Research and select appropriate physics library (Bullet Physics, Jolt Physics, or custom lightweight solution)
  - Consider voxel-specific needs: discrete collision detection, high entity count, minimal dependencies
  - Document choice with rationale for voxel castle's specific requirements

- [ ] **1.2: Physics Component Integration**
  - Create physics-related ECS components (RigidBodyComponent, ColliderComponent, etc.)
  - Design component structure to work with chosen physics engine
  - Ensure components integrate cleanly with existing PositionComponent and VelocityComponent

- [ ] **1.3: Physics System Setup**
  - Create PhysicsSystem class that integrates with Flecs ECS
  - Set up physics world/space initialization
  - Implement basic simulation step integration with game loop
  - Add physics system to main game initialization

### Phase 2: Basic Collision Detection 🔄 PENDING
**Goal:** Implement collision detection between entities and the voxel world.

- [ ] **2.1: Voxel World Collision**
  - Implement collision detection between entities and voxel blocks
  - Create efficient voxel-based collision mesh or use discrete collision checks
  - Handle solid vs. non-solid voxel types (AIR vs. STONE, DIRT, etc.)
  - Test basic collision with simple entity (player camera bounds)

- [ ] **2.2: Entity-Entity Collision**
  - Implement basic collision detection between entities
  - Handle simple shapes (sphere, box colliders)
  - Add collision response (bounce, stop, overlap detection)
  - Test with multiple entities in the world

- [ ] **2.3: Collision Response System**
  - Implement proper collision response for entity-world interactions
  - Handle velocity changes, position corrections
  - Add basic friction and restitution properties
  - Ensure entities don't clip through voxel geometry

### Phase 3: Basic Entity Physics 🔄 PENDING
**Goal:** Add fundamental physics behaviors for entities.

- [ ] **3.1: Gravity System**
  - Implement global gravity affecting entities
  - Add gravity component to control which entities are affected
  - Test falling entities and ground collision
  - Ensure proper terminal velocity and realistic fall behavior

- [ ] **3.2: Movement Physics**
  - Add forces and impulses for entity movement
  - Implement proper acceleration/deceleration for smooth movement
  - Add air resistance and ground friction
  - Replace direct velocity manipulation with force-based movement

- [ ] **3.3: Basic Player Physics**
  - Integrate player movement with physics system
  - Add proper ground detection for jumping
  - Implement walking, running, and jumping physics
  - Test camera collision and movement smoothness

### Phase 4: World Physics Integration 🔄 PENDING
**Goal:** Connect physics system with voxel world interactions.

- [ ] **4.1: Voxel Modification Physics**
  - Handle physics when voxels are added/removed
  - Update collision mesh when world changes
  - Ensure entities respond to world modifications
  - Test dynamic world changes with physics entities

- [ ] **4.2: Basic Block Physics**
  - Implement simple falling block physics for affected voxel types (sand, gravel)
  - Add support detection for blocks
  - Create cascade effects for unsupported blocks
  - Test with sand/gravel falling when support is removed

- [ ] **4.3: Physics Optimization**
  - Implement spatial partitioning for efficient collision detection
  - Add physics object culling based on distance
  - Optimize voxel collision mesh generation
  - Profile and optimize physics performance

## Technical Requirements

### Physics Engine Integration
- **Lightweight Solution**: Prefer minimal overhead for voxel world
- **Discrete Collision**: Handle blocky, non-smooth voxel geometry well
- **High Entity Count**: Support many small entities efficiently
- **Thread Safety**: Compatible with potential multi-threading

### ECS Integration
- **Component Design**: Physics components work seamlessly with existing ECS
- **System Integration**: Physics system coordinates with rendering and game logic
- **Data Consistency**: Ensure physics state stays synchronized with ECS state

### Performance Considerations
- **Voxel Collision**: Efficient collision detection with large voxel worlds
- **Spatial Optimization**: Use spatial data structures for collision culling
- **Memory Management**: Minimize physics memory overhead
- **Update Frequency**: Balance physics accuracy with performance

## Success Criteria
- [ ] Entities properly collide with voxel world geometry
- [ ] Basic gravity and movement physics work smoothly
- [ ] Player movement feels responsive and realistic
- [ ] Physics system integrates cleanly with existing ECS architecture
- [ ] Performance remains stable with multiple physics entities
- [ ] Basic world-physics interactions (falling blocks) function correctly

## Implementation Notes

### Existing Infrastructure
- **ECS System**: Flecs ECS already in place with PositionComponent, VelocityComponent
- **Movement System**: Basic MovementSystem exists but needs physics integration
- **Voxel World**: WorldManager provides voxel data access for collision detection
- **Component Architecture**: Clean component system ready for physics components

### Integration Points
- **Game Loop**: Physics system needs integration with main update loop
- **Rendering**: Physics state must synchronize with rendering positions
- **Input**: Player input should drive physics forces rather than direct velocity
- **World**: Physics must respond to dynamic voxel world changes

### Testing Strategy
- **Entity Spawning**: Create test entities to verify physics behavior
- **Movement Testing**: Test player physics with various input combinations
- **Collision Testing**: Verify collision detection with different voxel configurations
- **Performance Testing**: Monitor physics impact on frame rate and memory

## Key Files to Modify/Create
- `engine/include/ecs/components/physics_components.h` - Physics-related components
- `engine/include/ecs/systems/physics_system.h` - Main physics system
- `engine/src/ecs/systems/physics_system.cpp` - Physics system implementation
- `engine/src/ecs/systems/movement_system.cpp` - Update to use physics forces
- `game/src/main.cpp` - Add physics system initialization
- Physics engine integration files (depending on chosen library)

## Dependencies
- Existing ECS system (Flecs)
- WorldManager for voxel collision data
- Existing MovementSystem and position/velocity components
- Selected physics engine library (to be determined in Phase 1.1)

## Completion Notes
**This task will be marked complete when all phases are implemented and success criteria are met. The physics system should provide a solid foundation for more advanced physics features like structural integrity and fluid dynamics in future tasks.**
