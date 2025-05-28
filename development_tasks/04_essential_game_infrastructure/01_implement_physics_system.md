# Task: Implement Physics System

## Status: 🔄 IN PROGRESS

## Overview
Implement a dual-camera physics system with free-flying spectator mode and first-person physics-based player mode. Players can switch between modes, with the physics mode featuring gravity, collision detection, realistic movement, and jumping mechanics.

## Implementation Phases

### Phase 1: Dual Camera Mode System 🔄 IN PROGRESS
**Goal:** Implement camera mode switching and basic infrastructure for physics-based first-person mode.

- [x] **1.1: Camera Mode Management**
  - ✅ Add camera mode enumeration (FREE_FLYING, FIRST_PERSON)
  - ✅ Implement 'O' key handler to cycle between camera modes
  - ✅ Implement mode switching logic in Game class
  - ✅ Preserve current free-flying camera as default mode

- [x] **1.2: Player Entity Creation**
  - ✅ Create modular PlayerPhysics system
  - ✅ Design player collider (capsule: ~1.8m height × 0.6m diameter)
  - ✅ Set camera eye height at ~1.65m above ground level
  - ✅ Initialize player at current camera position when switching to first-person

- [x] **1.3: Input System Extension**
  - ✅ Extend input handling to support both camera modes
  - ✅ Preserve WASD + mouse controls for both modes
  - ✅ Add Space key for jumping in first-person mode
  - ✅ Maintain current free-flying controls (including up/down movement)
  - ✅ Implement Shift key for sprinting and Ctrl for crouching

### Phase 2: Basic Player Physics 🔄 IN PROGRESS
**Goal:** Implement fundamental physics for first-person player mode.

- [x] **2.1: Gravity System**
  - ✅ Implement gravity affecting player entity in first-person mode
  - ✅ Apply realistic gravity acceleration (~9.81 m/s²)
  - ✅ Handle terminal velocity to prevent excessive fall speeds
  - ✅ Ensure gravity only affects first-person mode, not free-flying

- [x] **2.2: Ground Detection & Collision**
  - ✅ Implement basic ground detection for floor collision
  - ✅ Handle player landing and stopping on solid voxel surfaces
  - ✅ Calculate proper ground position (player standing height above voxels)
  - ✅ Prevent player from falling through solid blocks

- [x] **2.3: Horizontal Movement Physics**
  - ✅ Implement WASD movement with arcade-style locomotion
  - ✅ Add realistic walking speed (~4-5 m/s) and sprint speed (~8-10 m/s)
  - ✅ Add directional movement based on camera orientation
  - ✅ Implement camera position synchronization with player

### Phase 3: Advanced Movement Mechanics ⏳ PENDING
**Goal:** Add sophisticated movement features for realistic first-person experience.

- [ ] **3.1: Step-Up Mechanism**
  - Implement automatic step-up for heights ≤ 0.75m (3/4 of a voxel)
  - Use forward collision detection to identify step-up opportunities
  - Smoothly elevate player position when stepping up
  - Ensure step-up doesn't interfere with jumping mechanics

- [ ] **3.2: Jumping System**
  - Implement Space key jumping with realistic physics
  - Set jump velocity for ~1.2m jump height (enough for 1-block obstacles)
  - Add ground detection to prevent infinite jumping
  - Handle jump timing and landing mechanics smoothly

- [ ] **3.3: Enhanced Movement Features**
  - Add sprint mode (Shift key for faster movement ~8-10 m/s)
  - Implement crouching (Ctrl key to reduce height to ~0.9m, slower movement ~2-3 m/s)
  - Add movement state tracking (standing, walking, running, jumping, falling, crouching)
  - Implement fall damage system based on fall velocity/height

- [ ] **3.4: Fall Damage System**
  - Calculate fall damage based on landing velocity
  - Set damage thresholds (e.g., no damage under 2m fall, scaling damage above)
  - Add player health component if not already present
  - Display damage feedback (visual effects, health reduction)

### Phase 4: Voxel World Integration ⏳ PENDING
**Goal:** Integrate player physics with dynamic voxel world.

- [ ] **4.1: Voxel Collision System**
  - Implement efficient player-voxel collision detection
  - Handle different voxel types (solid vs. non-solid blocks)
  - Update collision when world changes (voxel placement/removal)
  - Optimize collision detection for performance

- [ ] **4.2: Camera Synchronization**
  - Synchronize first-person camera position with player physics position
  - Handle smooth camera movement during physics interactions
  - Maintain camera orientation during mode switches
  - Ensure camera doesn't clip through geometry in first-person mode

- [ ] **4.3: Physics Optimization & Polish**
  - Implement spatial optimization for collision detection
  - Add physics interpolation for smooth visual movement
  - Handle edge cases (getting stuck, unusual terrain)
  - Performance profiling and optimization for physics system

## Design Decisions & Implementation Details

### Confirmed Design Choices
**Movement Features:**
- ✅ **Sprint Mode**: Shift key enables faster movement (~8-10 m/s vs normal 4-5 m/s)
- ✅ **Crouch Mode**: Ctrl key allows crouching (reduces height to ~0.9m, slower movement)
- ✅ **Fall Damage**: Implement damage when landing from high falls
- 🚫 **Sound Effects**: Deferred to separate audio system (not part of physics)

**Movement Feel:**
- ✅ **Arcade-style**: Instant response to input for responsive controls
- No momentum/acceleration - immediate velocity changes like current camera

**Collision Detection:**
- ✅ **Simple Box Collision**: Each solid voxel = box collider (fastest to implement)
- Ray-based detection for ground detection and step-up checks
- Focus on performance and simplicity for initial implementation

### Player Specifications
- **Player Collider**: Capsule shape, 1.8m height × 0.6m diameter
- **Eye Height**: 1.65m above ground (camera position)
- **Crouch Height**: 0.9m total height when crouching
- **Step Height**: 0.75m maximum automatic step-up
- **Jump Height**: 1.2m maximum jump
- **Walking Speed**: 4-5 m/s normal, 8-10 m/s sprinting, 2-3 m/s crouching

## Success Criteria
- [ ] Successfully switch between free-flying and first-person camera modes with 'O' key
- [ ] Free-flying mode preserves all current functionality (WASD + mouse + up/down movement)
- [ ] First-person mode features realistic gravity and ground collision
- [ ] Player can walk smoothly on terrain using WASD controls
- [ ] Automatic step-up works for heights ≤ 0.75m without jumping
- [ ] Space key jumping provides ~1.2m jump height for obstacle clearing
- [ ] Camera positioning feels natural at human eye level (~1.65m above ground)
- [ ] Physics system performs well without impacting frame rate
- [ ] Smooth transitions between camera modes
- [ ] Player cannot clip through voxel geometry in first-person mode

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
