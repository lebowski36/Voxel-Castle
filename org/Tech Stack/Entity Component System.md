# Entity Component System

## Overview
The Entity Component System (ECS) architecture forms the backbone of Voxel Fortress's simulation systems, enabling the simulation of thousands of interacting entities with complex behaviors while maintaining high performance. Our implementation leverages the core principles of ECS design—separating data from behavior and organizing components for optimal cache utilization—while incorporating custom optimizations specific to our voxel-based colony simulation requirements.

> **Related Documents:**
> - [Engine Architecture](./Engine%20Architecture.md)
> - [AI Architecture](./AI%20Architecture.md)
> - [Data Structures](../Game%20Concept/Data%20Structures.md)
> - [Colony Simulation](../Game%20Concept/Colony%20Simulation.md)

## Core Architecture

### Architectural Principles

#### Data-Oriented Design
Our ECS implementation follows strict data-oriented design principles:
- Components are pure data with no behavior
- Similar components are stored contiguously in memory
- Systems operate on streams of components rather than individual entities
- Memory layout optimized for cache coherency
- Minimal indirection to reduce pointer chasing

#### Component-Entity Relationship
- Entities are lightweight identifiers (essentially just IDs)
- Components are associated with entities but stored separately
- No inheritance hierarchy for entities or components
- Relationships between entities represented explicitly through components
- Component addition/removal is dynamic at runtime

#### System Design
- Systems contain all behavior logic
- Each system operates on specific component combinations
- Systems run in a deterministic order with explicit dependencies
- Systems can be enabled/disabled dynamically
- Specialized systems for different update frequencies

### Core Implementation

#### Entity Management
- **Entity Registry:** Central registry maintaining entity validity and lifecycle
- **Sparse Set Implementation:** Efficient entity lookup and iteration
- **Version Handling:** Entity reuse with version checking to prevent dangling references
- **Entity Batching:** Creation and destruction in batches for better performance
- **Lifecycle Hooks:** Notification system for entity creation/destruction events

#### Component Storage
- **Archetype-Based Storage:** Grouping entities with identical component sets
- **Component Arrays:** Contiguous arrays of same-type components
- **Chunk-Based Memory:** Fixed-size memory blocks for better memory management
- **Component Metadata:** Type information and serialization capabilities
- **Optional Components:** Efficient handling of optional/sparse components

#### System Execution
- **System Scheduler:** Dependency-aware execution ordering
- **Parallel Execution:** Automatic parallelization of compatible systems
- **Job-Based Processing:** Breaking work into distributable tasks
- **Batch Processing:** Operating on multiple entities in a single system invocation
- **System Groups:** Logical grouping of related systems

## Specialized Features

### Query System

#### Component Queries
- **Query Builder Pattern:** Expressive API for defining component combinations
- **Filter Conditions:** Additional constraints beyond component presence
- **Cached Queries:** Optimization for frequently used queries
- **Archetype Filtering:** Quick elimination of irrelevant archetypes
- **Read/Write Access Control:** Explicit declaration of access patterns

#### Iteration Options
- **Entity Iteration:** Processing all matching entities
- **Chunk Iteration:** Operating on groups of entities with identical components
- **Parallel Iteration:** Distributing work across multiple threads
- **Sparse Iteration:** Efficiently handling widely scattered entities
- **Sorted Iteration:** Processing entities in a specific order

#### Query Optimization
- **Query Caching:** Reusing query results between frames
- **Incremental Updates:** Only reprocessing entities that changed
- **Index Acceleration:** Fast lookup structures for common queries
- **Component Access Patterns:** Optimization based on access frequency
- **Query Merging:** Combining similar queries to reduce overhead

### Spatial Partitioning Integration

#### Spatial Component System
- **Position Components:** Efficient representation of entity locations
- **Spatial Hash Integration:** Quick lookup of entities in an area
- **Automatic Region Registration:** Entities tracked in spatial structures
- **Multiple Granularity:** Different precision levels for different needs
- **Spatial Query Specialization:** Optimized queries for spatial operations

#### Visibility Integration
- **View Frustum Filtering:** Culling entities outside camera view
- **Distance-Based Processing:** Different processing levels based on distance
- **Visibility Component Tagging:** Tracking what entities are currently visible
- **LOD Integration:** Connection to rendering level-of-detail system
- **Occlusion Awareness:** Knowledge of what entities are hidden by others

#### Chunk-Entity Relationship
- **Chunk Binding:** Associating entities with world chunks
- **Cross-Chunk Entity Handling:** Managing entities spanning multiple chunks
- **Migration Management:** Moving entities between chunks as they move
- **Chunk Loading Integration:** Automatic entity loading/unloading with chunks
- **Cellular Automation Integration:** Entity interaction with world blocks

### Event & Messaging System

#### Event Propagation
- **Component Change Events:** Notifications when components are modified
- **Entity Lifecycle Events:** Creation, destruction, and transformation events
- **System Communication:** Structured data passing between systems
- **Deferred Events:** Events processed at specific points in the frame
- **Event Batching:** Combining similar events for processing efficiency

#### Observer Patterns
- **Component Monitors:** Watching for specific component changes
- **Entity Trackers:** Following specific entities of interest
- **System Result Observation:** Capturing and responding to system outputs
- **Query Result Differencing:** Detecting changes in query results
- **Interest Management:** Registering for only relevant event types

#### Command Pattern Implementation
- **Entity Commands:** Deferred modifications to entities
- **Command Buffers:** Collecting commands for later execution
- **Transaction System:** All-or-nothing groups of commands
- **Command Validation:** Ensuring commands maintain system integrity
- **Replay Capability:** Recording and replaying command sequences

## Technical Implementation

### Memory Management

#### Component Memory Layout
- **Structure of Arrays:** Components grouped by type not entity
- **Cache Line Optimization:** Data arranged for optimal cache usage
- **Memory Alignment:** Proper alignment for SIMD operations
- **Fixed vs. Dynamic Storage:** Different strategies for different component types
- **Component Packing:** Efficient storage of small components

#### Allocation Strategy
- **Custom Allocators:** Purpose-built allocators for component arrays
- **Pool Allocation:** Reuse of component memory
- **Chunk-Based Growth:** Allocation in fixed-size blocks
- **Memory Mapping:** Efficient virtual memory usage for large worlds
- **Reference Management:** Handling components with owned resources

#### Memory Recycling
- **Component Freelist:** Reusing component slots from destroyed entities
- **Deferred Cleanup:** Batching destruction operations
- **Archetype Compaction:** Defragmenting sparsely used archetypes
- **Memory Pressure Handling:** Adjusting behavior under low memory conditions
- **Resource Claiming:** Prioritizing memory for critical systems

### Concurrency Model

#### Read-Write Access Control
- **Access Declaration:** Systems explicitly declare component access patterns
- **Conflict Detection:** Compile-time and runtime detection of access conflicts
- **Read-Only Sharing:** Multiple systems reading the same components simultaneously
- **Exclusive Access:** Guaranteeing exclusive access when writing
- **Component-Level Granularity:** Locking at the component type level not globally

#### Parallel System Execution
- **Dependency Graph Analysis:** Determining which systems can run in parallel
- **Work Division:** Splitting entity processing across worker threads
- **Thread Pool Integration:** Using global thread pool for system execution
- **Load Balancing:** Distributing work evenly across available cores
- **SIMD Utilization:** Vectorized processing of component data

#### Lock-Free Approaches
- **Atomic Operations:** Lock-free updates where possible
- **Double Buffering:** Reading from one buffer while writing to another
- **Version Counters:** Detecting concurrent modifications
- **Immutable Design:** Favoring immutable data to avoid synchronization
- **Thread-Local Staging:** Collecting changes locally before committing

### Integration With Other Systems

#### Physics System Integration
- **Physics Body Components:** Representation of physical properties
- **Collision Event Propagation:** From physics to ECS
- **Transform Synchronization:** Keeping ECS and physics positions aligned
- **Physics Proxies:** Simplified representations for distant entities
- **Selective Physics:** Activating detailed physics only when necessary

#### Rendering System Integration
- **Render Component Extraction:** Converting ECS data to render-friendly format
- **Instance Data Generation:** Creating GPU-compatible instance data
- **Visibility Determination:** Deciding what entities to render
- **Animation System Bridge:** Connecting ECS with skeletal animation
- **Material Property Components:** Controlling entity appearance

#### AI System Integration
- **Behavior Components:** Storing AI state and decision information
- **Perception System:** AI awareness of environment via ECS queries
- **Action Execution:** AI decisions resulting in component changes
- **Goal Components:** Representing entity objectives
- **Knowledge Representation:** Entity understanding of world state

#### Serialization Integration
- **Component Serialization:** Converting components to persistent format
- **Entity Reconstruction:** Rebuilding entities from serialized data
- **Reference Handling:** Maintaining entity references through serialization
- **Versioning Support:** Handling component schema changes
- **Selective Serialization:** Saving only necessary components

## Performance Optimization

### Compilation & Code Generation

#### Type-Based Optimization
- **Component Type Analysis:** Specialized handling based on component characteristics
- **Static Dispatch:** Using compile-time polymorphism where possible
- **Const Generics:** Size optimizations for fixed-size components
- **Template Specialization:** Different implementations for different component types

#### Macro System
- **Component Registration Macros:** Reducing boilerplate for component definition
- **System Declaration Macros:** Automatically deriving access patterns
- **Query Construction Helpers:** Simplifying complex query creation
- **Reflection Generation:** Automatic implementation of serialization
- **Code Generation:** Specialized hot-path optimizations

#### Compiler Cooperation
- **Inlining Directives:** Guiding compiler optimization decisions
- **Branch Prediction Hints:** Improving instruction pipeline efficiency
- **Prefetch Instructions:** Strategic data prefetching
- **Link-Time Optimization:** Cross-module optimization
- **Profile-Guided Optimization:** Using runtime data to inform compilation

### Runtime Optimization

#### Hot Path Optimization
- **Archetype Sorting:** Most frequently accessed archetypes processed first
- **Component Reordering:** Most accessed components first in memory layout
- **Branch Reduction:** Minimizing conditional logic in inner loops
- **Loop Unrolling:** Manual and compiler unrolling for critical paths
- **Constant Propagation:** Pre-computation of constant values

#### Batching & Vectorization
- **SIMD Operations:** Vectorized component processing
- **Batch Size Tuning:** Optimal sizes for different operation types
- **Instruction Pipelining:** Operations arranged for CPU pipeline efficiency
- **Cache Warming:** Prefetching data before processing
- **Memory Access Patterns:** Stride optimization for linear traversal

#### Profiling-Driven Optimization
- **System Timing:** Detailed performance metrics per system
- **Hot Spot Identification:** Finding performance bottlenecks
- **Entity Processing Rates:** Tracking entities processed per millisecond
- **Memory Access Analysis:** Identifying cache misses and memory stalls
- **Optimization Feedback Loop:** Continuous improvement based on profiling

## Implementation Progress

### Current Status
- Core ECS architecture implemented
- Basic component storage and entity management
- Simple query system operational
- Initial system scheduler with dependency handling
- Fundamental component types defined

### Next Steps
- Parallel execution system implementation
- Advanced query filtering and caching
- Spatial integration optimization
- Event system development
- Serialization framework completion

## References & Resources

### Academic & Industry Resources
- "Data-Oriented Design and C++" by Mike Acton
- "Overwatch Gameplay Architecture and Netcode" by Timothy Ford
- "Building a Data-Oriented Entity System" by Niklas Gray
- "Practical Examples in Data Oriented Design" by Dice

### Open Source Inspirations
- EnTT - Archetype design and sparse sets
- Flecs - System dependencies and relationship handling
- Articles by Mike Acton - C++ DOD/ECS insights

### Voxel-Specific Adaptations
- Minecraft entity systems - Chunk-entity relationship examples
- Factorio technical blog - Entity optimization techniques
- Vintage Story - Entity-block interaction approaches
- Dwarf Fortress - Simulation depth reference points

---

## Technical Implementation Considerations (C++)

Implementing this ECS architecture in C++ requires careful attention to performance, memory management, and API design. Here are key C++ specific considerations:

*   **Core Language & Performance:**
    *   **Modern C++ Standards:** Utilize C++17/20/23 features. `if constexpr` can be powerful for compile-time decisions in generic ECS code. Concepts can constrain template parameters for systems and components.
    *   **Memory Layout & Cache Coherency:** This is paramount. C++ gives direct control over memory. Ensure component arrays are tightly packed. Use `alignas` for SIMD-friendly alignment.
    *   **Pointer Indirection:** Minimize pointer chasing. Store components directly in arrays within archetypes/chunks where possible.
    *   **Templates & Metaprogramming:** Heavily use templates for generic component storage, system definitions, and query systems. Compile-time type information is key to an efficient C++ ECS.

*   **Entity & Component Management:**
    *   **Entity IDs:** `uint32_t` or `uint64_t` for entity IDs, possibly with a generation/version counter packed in to detect stale IDs.
    *   **Component Storage:**
        *   **Archetypes:** Store components in contiguous arrays per archetype. `std::vector<ComponentType>` or custom contiguous containers can be used for each component type within an archetype.
        *   **Sparse Sets:** Often used for mapping entity IDs to their row index within an archetype's component arrays. This provides O(1) lookup.
        *   **Memory Allocation:** Custom allocators (arena, pool) for component data within archetypes to control memory layout and reduce fragmentation. `std::pmr` can be a foundation.
    *   **Type Erasure (Carefully):** While C++ ECS often relies on static typing, some form of type erasure might be needed for very dynamic scenarios or tooling, but it should be avoided in hot paths if it impacts performance.

*   **System Implementation:**
    *   **System Signature:** Systems typically define the set of components they operate on (e.g., via template parameters or a tuple of component types).
    *   **Iteration:** Systems iterate over entities matching their component signature. This often involves iterating through relevant archetypes and then their component arrays.
    *   **Function Pointers/`std::function`:** Can be used to store system update logic, though direct templated calls are often faster.

*   **Query System:**
    *   **Compile-Time Queries:** Leverage C++ templates and type traits to define and resolve queries at compile time for maximum performance.
    *   **Query DSL:** Potentially create a Domain Specific Language (DSL) using C++ expression templates or chained function calls to build queries (e.g., `world.query<Position, Velocity>().without<Static>()`).
    *   **Query Caching:** Cache archetype matching for queries.

*   **Concurrency & Parallelism:**
    *   **System Dependencies:** Clearly define read/write dependencies for each system on component types. A graph of systems can then be used to schedule parallel execution.
    *   **Parallel Iteration:** For systems that can run in parallel, divide the work of iterating over entities/archetypes among threads. Libraries like Intel TBB or custom job schedulers are essential.
    *   **Atomic Operations / Lock-Free Structures:** Use `std::atomic` for simple flags or counters. Lock-free data structures for command buffers or event queues can be beneficial but are complex to implement correctly.
    *   **Thread-Local Storage:** For temporary data used during system updates to avoid contention.

*   **Event & Messaging System:**
    *   **Event Types:** Define event types as simple structs.
    *   **Event Queues:** Use thread-safe queues (potentially lock-free) for inter-system communication or for deferred event processing.
    *   **Observer Pattern:** `std::function` or signal/slot mechanisms (e.g., from Boost.Signals2 or a custom implementation) can implement observer patterns for component changes.

*   **Serialization:**
    *   **Reflection:** A C++ reflection system (e.g., `rttr`, or a custom one using macros and templates) is invaluable for automatically serializing components.
    *   **Binary Serialization:** For performance, prefer binary serialization formats (e.g., Cereal, Boost.Serialization, or a custom format).

*   **Debugging & Tooling:**
    *   **Debug Names:** Store debug names for entities and components (can be stripped in release builds).
    *   **Visualization:** Use ImGui or similar to create in-engine inspectors for entity components and system states.

*   **Rust-Specific Points to Adapt/Remove:**
    *   **Monomorphization Benefits (Rust):** While C++ templates also lead to specialized code, the term "monomorphization" is very specific to Rust's generics. The underlying principle of generating specialized code for specific types is shared.
    *   **Trait Specialization (Rust):** C++ has template specialization and SFINAE/concepts which serve similar purposes for providing different implementations based on types.
    *   **`bevy_ecs`, `Legion`, `specs` (Rust ECS libraries):** These are inspirations. For C++, look at `EnTT`, `Flecs`, or articles by people like Mike Acton for C++ DOD/ECS insights.

Popular C++ ECS libraries like `EnTT` or `Flecs` provide excellent examples of how these concepts are implemented and can serve as strong references or be used directly.