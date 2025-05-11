# Physics System

## Overview
The Voxel Fortress physics system provides realistic physical simulation within our voxel-based world, handling everything from basic object motion to complex structural integrity calculations. Built on a modified physics engine, our implementation features specialized extensions for voxel environments, including block-based collision detection, structural analysis, and fluid dynamics. The system balances simulation fidelity with performance requirements, employing a variety of optimization techniques to maintain responsiveness even in complex scenarios with thousands of simulated objects.

> **Related Documents:**
> - [Engine Architecture](./Engine%20Architecture.md)
> - [Entity Component System](./Entity%20Component%20System.md)
> - [Voxel Data Structures](./Voxel%20Data%20Structures.md)
> - [Physics Systems](../Game%20Concept/Physics%20Systems.md)

## Core Architecture

### Physics Foundation

#### Physics Engine Integration
- **Core Physics Engine:** Based on a mature C++ physics library
- **Custom Extensions:** Voxel-specific modifications to the base engine
- **Simulation Pipeline:** Clear stages of collision detection, resolution, and integration
- **Fixed Timestep:** Physics updates on a consistent time interval independent of frame rate
- **Deterministic Design:** Reliable, reproducible physics behavior

#### World Representation
- **Spatial Partitioning:** Optimized collision detection through space subdivision
- **Collision Geometry:** Multiple representation levels for physical objects
- **Material Properties:** Physical characteristics of different block types
- **Physics Proxies:** Simplified representations for complex objects
- **Scale Management:** Handling different scales from micro-blocks to large structures

#### Coordinate System
- **Physics Space:** Dedicated coordinate system for physics calculations
- **Transformation Pipeline:** Converting between world and physics coordinates
- **Precision Management:** Strategies for maintaining accuracy across large distances
- **Origin Shifting:** Dynamic repositioning to maintain precision near the player
- **Scale Factors:** Appropriate scaling between visual and physical representation

### Block Physics

#### Block Collision System
- **Block Shapes:** Different collision shapes for various block types
- **Compound Shapes:** Combining simple shapes for complex blocks
- **Tight vs. Loose Bounds:** Different precision levels based on context
- **Shape Caching:** Reusing collision shapes for similar blocks
- **Contact Generation:** Efficient detection of block-to-entity collisions

#### Block Physical Properties
- **Density & Mass:** Weight characteristics affecting physics behavior
- **Friction Coefficients:** Surface interaction properties
- **Restitution:** Bounciness of different materials
- **Hardness:** Resistance to damage and breaking
- **Custom Properties:** Special physical behaviors for unique block types

#### Block State Changes
- **Breaking Mechanics:** How blocks respond to excessive forces
- **Damage Models:** Progressive deterioration of block integrity
- **State Transitions:** Changing physical properties based on block state
- **Block Connections:** How blocks physically interact with neighbors
- **Physical Triggers:** Block changes initiated by physical events

### Structural Integrity

#### Support Analysis
- **Load Calculation:** Determining weight distribution through structures
- **Support Propagation:** How load transfers between connected blocks
- **Overhang Limits:** Maximum unsupported extension
- **Material Strength:** Different weight bearing capabilities
- **Connection Types:** How different blocks attach to each other

#### Structural Failure
- **Failure Conditions:** When structures collapse
- **Progressive Collapse:** Cascading failure simulation
- **Fracture Lines:** Natural break points in structures
- **Dynamic Response:** How remaining structure reacts to partial collapse
- **Debris Generation:** Creating physically simulated rubble from collapses

#### Engineering Challenges
- **Arch Mechanics:** Proper simulation of self-supporting arches
- **Cantilever Physics:** Realistic behavior of extended structures
- **Tension & Compression:** Different forces in structural elements
- **Structural Reinforcement:** Materials that enhance structural strength
- **Foundation Requirements:** How ground support affects buildings

### Fluid Dynamics

#### Fluid Simulation
- **Cellular Automata Model:** Block-based fluid simulation
- **Flow Rates:** Variable speed based on conditions
- **Pressure System:** How fluid pressure affects behavior
- **Source & Sink Blocks:** Where fluids originate and drain
- **Fluid Mixing:** Interactions between different fluid types

#### Fluid-Block Interaction
- **Erosion Effects:** Fluids wearing away certain blocks
- **Buoyancy:** How objects float in fluids
- **Flooding Mechanics:** Spread of fluids through spaces
- **Current Forces:** How flowing fluids push entities
- **Block State Changes:** How fluids affect block properties (e.g., wetting, extinguishing)

#### Fluid-Entity Interaction
- **Swimming Physics:** Entity movement through fluids
- **Drowning Mechanics:** Effects of submersion on entities
- **Current Effects:** Entities being pushed by flowing fluids
- **Splashing Effects:** Visual and physical responses to entering fluids
- **Fluid Drag:** Resistance to movement in different fluids

## Advanced Systems

### Entity Physics

#### Entity Body Types
- **Rigid Bodies:** Standard solid object physics
- **Soft Bodies:** Deformable objects with internal connections
- **Character Controllers:** Specialized physics for player and NPCs
- **Ragdolls:** Multi-body systems for realistic character impacts
- **Vehicles:** Compound bodies with special movement characteristics

#### Collision Detection
- **Broad Phase:** Quick elimination of distant object pairs
- **Narrow Phase:** Precise collision detection between proximate objects
- **Continuous Collision:** Preventing fast objects from tunneling through obstacles
- **Shape Types:** Boxes, spheres, capsules, convex hulls, compound shapes
- **Collision Layers:** Selective collision between different object categories

#### Constraint Systems
- **Joint Types:** Various connections between physics bodies
- **Constraint Solver:** Maintaining physical relationships between objects
- **Breakable Constraints:** Joints that fail under sufficient force
- **Constraint Chains:** Networks of connected bodies
- **Mechanical Systems:** Simulating gears, pulleys, hinges, etc.

### Specialized Physics Features

#### Destructible Environments
- **Block Destruction:** Physics of breaking individual blocks
- **Cascading Destruction:** Chain reactions of destruction
- **Structural Response:** How remaining structure behaves after partial destruction
- **Debris Physics:** Behavior of fragments after destruction
- **Performance Management:** Limiting physics complexity during mass destruction

#### Explosive Physics
- **Blast Wave Propagation:** How explosion forces spread
- **Line of Sight Damage:** Blocking of explosive force by obstacles
- **Force Application:** Converting explosions to physical impulses
- **Material Response:** Different block reactions to explosions
- **Chain Reactions:** Secondary explosions triggered by primary ones

#### Weather Effects
- **Wind Forces:** Physical push on entities and certain blocks
- **Precipitation Physics:** Rain and snow accumulation
- **Temperature Effects:** Freezing, thawing, and related state changes
- **Lightning Strikes:** Impact physics and resulting block changes
- **Natural Disasters:** Earthquakes, floods, avalanches, etc.

### Physics-Based Gameplay

#### Interactive Machinery
- **Simple Machines:** Levers, pulleys, gears, etc.
- **Power Transmission:** Mechanical energy transfer between components
- **Mechanical Puzzles:** Physics-based gameplay challenges
- **Feedback Systems:** Physical response to player interaction
- **Failure States:** How machinery breaks or malfunctions

#### Combat Physics
- **Projectile Ballistics:** Realistic trajectory simulation
- **Impact Forces:** Effects of weapons on targets
- **Knockback Mechanics:** Physical response to attacks
- **Shield Physics:** Deflection and protection simulation
- **Armor Penetration:** How attacks overcome protective layers

#### Resource Collection
- **Mining Mechanics:** Physical process of block extraction
- **Tool Physics:** How different tools interact with materials
- **Resource Drop Behavior:** Physical movement of harvested items
- **Machine Interaction:** How devices collect and process resources
- **Physics-Based Challenges:** Making collection require physical skill

## Technical Implementation

### Performance Optimization

#### Simulation Levels
- **Full Simulation:** Detailed physics for nearby/important objects
- **Simplified Physics:** Reduced complexity for distant objects
- **Sleep States:** Deactivating static objects until disturbed
- **Priority System:** More computational resources for critical elements
- **Distance-Based Detail:** Physics fidelity decreasing with distance

#### Spatial Optimization
- **Dynamic AABB Trees:** Efficient broad phase collision detection
- **Island Management:** Independent simulation of disconnected physics groups
- **Grid-Based Culling:** Quick elimination of distant interactions
- **Swept Tests:** Efficient moving object collision detection
- **Custom Broadphase:** Specialized for voxel environments

#### Computational Efficiency
- **SIMD Utilization:** Vectorized physics calculations
- **GPU Acceleration:** Offloading appropriate calculations to GPU
- **Warm Starting:** Using previous solutions to accelerate convergence
- **Constraint Reduction:** Simplifying complex constraint systems
- **Approximation Methods:** Using faster calculations where precision isn't critical

### Multithreading Design

#### Parallel Physics Pipeline
- **Job-Based Physics:** Breaking physics work into parallel tasks
- **Island Parallelism:** Processing disconnected physical systems in parallel
- **Pipeline Stages:** Different phases of physics processing in parallel
- **Data Dependency Management:** Minimizing synchronization points
- **Load Balancing:** Distributing physics work evenly across cores

#### Synchronization Strategy
- **Read/Write Phases:** Clear separation of data access patterns
- **Lock-Free Approaches:** Avoiding contention in hot paths
- **Double Buffering:** Allowing parallel read/write operations
- **Atomic Operations:** Thread-safe updates without locks
- **Conflict Resolution:** Handling overlapping physics operations

#### Predictive Physics
- **Speculative Execution:** Processing likely outcomes in advance
- **Result Caching:** Reusing calculations when inputs haven't changed
- **Incremental Updates:** Only recalculating what's necessary
- **Priority Processing:** Most important physics first
- **Anticipatory Loading:** Preparing physics data before it's needed

### Integration With Game Systems

#### Rendering Synchronization
- **Physics-Visual Alignment:** Keeping rendered objects aligned with physics
- **Interpolation:** Smooth visual representation between physics steps
- **Prediction:** Visual anticipation of physics outcomes
- **Animation Blending:** Transitioning between physics and animation control
- **Visual Effects:** Triggering effects based on physics events

#### Gameplay Integration
- **Physics Queries:** Game systems gathering physics information
- **Physics-Based Interactions:** Gameplay mechanics using physics simulation
- **Event System:** Notifying game systems of physics events
- **Physics Scripting:** Controlled physics for specific gameplay moments
- **Player Feedback:** Communicating physics state to players

#### Sound System Connection
- **Physics-Based Audio:** Generating sound events from physics interactions
- **Impact Sounds:** Different sounds based on collision parameters
- **Material Sound Properties:** How different materials sound when interacting
- **Environmental Audio:** Physics-influenced ambient sounds
- **Positional Audio:** Accurate sound placement based on physics events

### Debug & Development Tools

#### Physics Visualization
- **Collision Shape Display:** Visual representation of physics shapes
- **Force Visualization:** Showing forces acting on objects
- **Constraint Display:** Visualizing connections between bodies
- **Velocity Vectors:** Displaying movement direction and speed
- **Contact Points:** Showing where objects are touching

#### Analysis Tools
- **Stability Testing:** Checking structures for potential collapse
- **Performance Profiling:** Measuring physics simulation costs
- **Anomaly Detection:** Finding unusual or problematic physics behavior
- **Stress Testing:** Pushing physics systems to their limits
- **Comparison Tools:** Before/after views of physics changes

#### Physics Sandbox
- **Interactive Testing:** Directly manipulating physics objects
- **Parameter Adjustment:** Real-time modification of physics properties
- **Scenario Creation:** Setting up specific physics situations for testing
- **Record & Playback:** Capturing and reviewing physics interactions
- **Physics Debugging:** Special modes to diagnose issues

## Technical Implementation Considerations (C++)

Adapting and implementing this physics system in C++ involves choosing appropriate libraries, leveraging C++ performance features, and ensuring tight integration with other C++-based engine systems.

*   **Core Physics Engine Choice:**
    *   **Existing C++ Libraries:** Evaluate mature C++ physics libraries:
        *   **PhysX (NVIDIA):** Feature-rich, widely used, good performance, but can have a more complex API.
        *   **Jolt Physics:** Modern, open-source, designed for performance and ease of use, gaining popularity.
        *   **Bullet Physics:** Long-standing, open-source, very flexible, used in many projects.
        *   **ReactPhysics3D:** Open-source, relatively lightweight, focused on 3D simulation.
    *   **Custom Extensions:** Regardless of the chosen library, significant custom extensions will be needed for voxel-specific phenomena like block-based collision, detailed structural integrity, and cellular automata-based fluid dynamics.

*   **C++ Language & Performance:**
    *   **Performance-Critical Code:** Physics calculations are computationally intensive. Use modern C++ (C++17/20/23) for optimized code. Profile frequently.
    *   **Memory Management:** Physics engines often create/destroy many small objects (collision shapes, contacts). Custom allocators (pools, arenas) for these objects can significantly improve performance. `std::pmr` can be a basis.
    *   **SIMD:** Leverage SIMD instructions (e.g., via intrinsics or libraries like `xsimd` or Eigen for vector math) for vector and matrix operations common in physics (e.g., transformations, collision checks, solver stages).
    *   **Data Structures:** Optimize data structures used for spatial partitioning (e.g., AABB trees, grids) and broad-phase collision detection for cache efficiency.

*   **Voxel-Specific Implementation:**
    *   **Block Collision:** Develop efficient collision geometry generation for voxel blocks. This might involve creating convex hulls for simple blocks or groups of blocks, or specialized voxel-aware collision algorithms.
    *   **Structural Integrity:** This will likely be a custom system. It might involve graph-based representations of structures, where nodes are blocks and edges are connections, with forces propagated through the graph.
    *   **Fluid Dynamics:** Cellular automata for fluids can be implemented efficiently in C++, potentially using grid-based computations that map well to cache-friendly access patterns and SIMD.

*   **Multithreading & Concurrency:**
    *   **Parallel Pipeline:** Most modern C++ physics engines support multithreading. Design the physics pipeline (broadphase, narrowphase, solver, integration) to exploit parallelism using a job system (e.g., Intel TBB, a custom fiber scheduler, or `std::async` with a thread pool).
    *   **Island-Based Parallelism:** Simulate disconnected groups of interacting objects (islands) in parallel.
    *   **Thread Safety:** Ensure data structures accessed by multiple threads are properly synchronized using `std::mutex`, `std::atomic`, or lock-free techniques where appropriate.

*   **Integration with ECS (C++):**
    *   **Physics Components:** Represent rigid bodies, colliders, joints, etc., as components in the C++ ECS (e.g., using `EnTT` or `Flecs`).
    *   **Synchronization:** Efficiently synchronize transforms between the ECS and the physics engine. This often involves a buffer or direct updates at specific points in the game loop.
    *   **Event System:** Propagate collision events (begin contact, end contact, etc.) from the physics engine back to the ECS for game logic to react.

*   **Debugging & Development Tools:**
    *   **Visualization:** Use a library like ImGui to create in-engine debug visualizations for collision shapes, contact points, forces, AABBs, and structural stress.
    *   **Profilers:** Integrate with C++ profilers (Tracy, Perf, VTune) to identify physics bottlenecks.
    *   **Physics Sandbox:** Essential for testing and tuning. Allow real-time modification of physics properties and spawning/manipulation of objects.

*   **Removing Rust-Specific References:**
    *   The primary reference to **Rapier** needs to be replaced with the chosen C++ physics library or the concept of a C++-based physics foundation.

By selecting a suitable C++ physics library as a base and building specialized voxel systems on top, leveraging C++'s performance capabilities, a robust and efficient physics simulation can be achieved.

## Implementation Progress

### Current Status
- Core physics integration established
- Basic voxel collision system implemented
- Initial entity physics working
- Simple block physics for gravity and support
- Fundamental fluid simulation prototype

### Next Steps
- Structural integrity system implementation
- Advanced fluid dynamics development
- Physics optimization for large-scale structures
- Multithreading enhancements
- Special effects from physics events

## References & Resources

### Academic & Industry Resources
- "Physically Based Modeling" course materials
- "Real-Time Collision Detection" by Christer Ericson
- "Game Physics Engine Development" by Ian Millington
- Research papers on voxel-based structural analysis

### Inspiration Sources
- Minecraft physics mods (structural integrity systems)
- Teardown (destructible voxel environments)
- Bridge Constructor (structural engineering challenges)
- Noita (fluid and particle physics)
- Besiege (mechanical construction physics)