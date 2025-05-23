# Physics Systems

## Overview
The Physics Systems in Voxel Fortress govern how objects interact with each other and the environment, creating a believable and dynamic world. From basic block physics to complex fluid dynamics, these systems add depth to gameplay while creating challenges and opportunities for colony management. The physics implementation balances realism with gameplay needs and computational constraints.

> **Related Documents:**
> - [Building & Logistics](./Building%20%26%20Logistics.md)
> - [World Generation](./World%20Generation.md)
> - [Threats & Challenges](./Threats%20%26%20Challenges.md)
> - [Main Game Concept](../Voxel%20Fortress%20Game%20Concept%20Details.md)

## Core Components

### Block Physics

#### Gravity & Support
- **Affected Materials:** Select materials (sand, gravel, soil) affected by gravity
- **Support Rules:** Blocks must be supported from below or attached to sides
- **Cascading Collapses:** One falling block can trigger others to fall
- **Falling Behavior:** Realistic acceleration and landing patterns
- **Entity Interactions:** Falling blocks can damage entities or trigger mechanisms

#### Material Properties
- **Density:** Affects weight, buoyancy, and structural requirements
- **Hardness:** Determines mining time and resistance to damage
- **Brittleness:** How likely the material is to shatter rather than deform
- **Flammability:** Susceptibility to fire and burning characteristics
- **Conductivity:** Heat and electrical transmission properties

#### Natural Weathering
- **Erosion:** Gradual degradation from water and weather
- **Oxidation:** Metal rusting and patina development
- **Decay:** Organic materials rotting over time
- **Wear Patterns:** High-traffic areas showing signs of use

### Structural Integrity

#### Load-Bearing Mechanics
- **Weight Distribution:** Heavier blocks require proper support
- **Support Chains:** How weight is transferred through connected structures
- **Material Strength:** Different materials can support different loads
- **Compression vs. Tension:** Different structural behavior under different forces

#### Structural Failure
- **Warning Signs:** Cracks, creaking, or dust indicating imminent collapse
- **Partial Failure:** Portions collapsing rather than entire structures
- **Failure Cascades:** How one failure can trigger others
- **Environmental Triggers:** Vibrations, water damage, or temperature changes affecting stability

#### Architectural Elements
- **Arches & Buttresses:** Special structural elements with enhanced support capabilities
- **Reinforcement:** Methods to strengthen existing structures
- **Spanning Elements:** Bridges, beams, and other structures requiring specialized support
- **Cantilevering:** Rules for unsupported projections

#### Subsidence & Terrain Stability
- **Ground Types:** Different terrain has different stability properties
- **Underground Hollowing:** Effects of extensive mining on surface stability
- **Foundation Requirements:** Necessary support for different building types
- **Terrain Compression:** Settling of soil under heavy structures over time

### Fluid Dynamics

#### Water Physics
- **Flow Patterns:** Water seeks lowest level and spreads horizontally
- **Pressure Systems:** Depth affects pressure and flow rate
- **Source Blocks:** Springs and renewable water sources
- **Interaction with World:** Erosion, plant growth, and material softening
- **States:** Freezing in cold temperatures, evaporation in heat

#### Lava Physics
- **Heat Effects:** Damage to entities, ignition of flammable materials
- **Solidification:** Cooling into stone when exposed to water or air
- **Flow Properties:** Slower movement than water, different viscosity
- **Light Emission:** Natural lighting source
- **Material Transformation:** Converting certain blocks it flows over

#### Other Fluids
- **Oil:** Flammable, floating on water, fuel source
- **Mud:** Slowing movement, gradually drying
- **Magic Fluids:** Optional fantasy elements with special properties
- **Gas Pockets:** Dangerous underground gases with unique behaviors

#### Fluid Containment & Control
- **Vessels & Pipes:** Transportation and storage systems
- **Pumps & Sluices:** Mechanical control of flow
- **Dams & Reservoirs:** Large-scale water management
- **Pressure Systems:** Utilizing fluid pressure for mechanical work

### Entity Physics

#### Movement & Collision
- **Pathfinding Constraints:** Physical obstacles affect movement possibilities
- **Entity Size:** Different entities require different spaces for movement
- **Push Effects:** Large entities can potentially move smaller objects
- **Climbing/Swimming:** Special movement types based on entity capabilities

#### Object Interactions
- **Item Throwing/Dropping:** Trajectory, bounce, and damage calculations
- **Stack Stability:** How high objects can be stacked before toppling
- **Friction Effects:** Objects sliding on different surfaces
- **Collision Response:** How objects react when bumped or struck

#### Environmental Effects
- **Wind Forces:** Effects on flying entities, lightweight objects
- **Current Effects:** Water/lava flows affecting movement
- **Temperature Impact:** Entity speed and behavior changes in different temperatures
- **Weather Physics:** Rain, snow accumulation, ice formation

### Mechanical Systems

#### Simple Machines
- **Levers & Pulleys:** Basic force multiplication
- **Gears & Belts:** Transferring rotational energy
- **Counterweights:** Balance-based mechanisms
- **Springs & Tension:** Storing and releasing energy

#### Power Transfer
- **Axles & Driveshafts:** Direct power transmission
- **Water Wheels:** Converting fluid flow to mechanical energy
- **Windmills:** Harnessing wind power
- **Animal Power:** Treadmills and directed animal labor

#### Advanced Mechanisms
- **Logic Gates:** Mechanical computation elements
- **Timing Systems:** Water clocks, pendulums
- **Automation Chains:** Connected mechanisms performing complex tasks
- **Feedback Systems:** Self-regulating mechanical processes

## Advanced Systems

### Environmental Physics

#### Weather Effects
- **Wind Dynamics:** Directional force affecting objects, fires, flying entities
- **Precipitation:** Water accumulation, snow buildup, flooding potential
- **Temperature Transfer:** Heat flow between objects and environment
- **Lightning:** Strike patterns, fire ignition, damage calculations

#### Natural Disasters
- **Earthquakes:** Vibrations causing structural stresses and potential collapse
- **Landslides:** Mass movement of terrain under certain conditions
- **Floods:** Rising water levels with associated pressure and damage
- **Volcanic Activity:** Lava flows, ash clouds, terrain deformation

#### Ecological Systems
- **Plant Growth Physics:** Support requirements, space competition
- **Erosion Cycles:** Long-term terrain changes from water and wind
- **Soil Mechanics:** Fertility, water retention, compaction
- **Climate Feedback:** How colony activities might affect local environment

### Combustion & Temperature

#### Fire Propagation
- **Ignition Requirements:** Heat sources, flammable materials
- **Spread Patterns:** How fire moves between adjacent materials
- **Oxygen Dependency:** Confined spaces affecting burn rate
- **Extinguishing Mechanics:** Water, smothering, fire breaks

#### Heat Transfer
- **Conduction:** Heat moving through connected materials
- **Convection:** Air/fluid heated by sources affecting nearby objects
- **Radiation:** Direct heat effects at a distance
- **Thermal Mass:** Materials heating and cooling at different rates

#### Smoke & Gas
- **Diffusion Patterns:** How gases spread through available space
- **Buoyancy:** Rising in air based on temperature and density
- **Toxicity Effects:** Impact on entity health and consciousness
- **Ventilation Mechanics:** Air flow and gas dispersion

#### Freezing & Thawing
- **State Changes:** Water to ice and back based on temperature
- **Expansion Effects:** Potential damage from freezing water
- **Insulation Properties:** How different materials retain heat
- **Cold-Weather Mechanics:** Effects on machinery, fluids, and entities

### Combat & Destruction Physics

#### Damage Models
- **Material Response:** Different blocks react differently to damage
- **Progressive Destruction:** Multi-stage breakdown rather than binary states
- **Structural Consequences:** Damage affecting integrity of surrounding blocks
- **Repair Requirements:** Physics considerations for fixing damaged structures

#### Projectile Physics
- **Ballistic Trajectories:** Realistic arcs affected by gravity
- **Penetration Mechanics:** Ability to pass through different materials
- **Ricochet Calculations:** Deflection angles based on surface and impact
- **Momentum Transfer:** Knockback and movement effects

#### Explosion Mechanics
- **Blast Waves:** Force diminishing with distance
- **Fragmentation:** Secondary projectiles from destroyed materials
- **Environmental Impact:** Crater formation, fire ignition, structural damage
- **Chain Reactions:** Explosions triggering other explosions or collapses

### Special Physics Systems

#### Magic/Technology Integration (Optional)
- **Field Effects:** Area-based modifications to standard physics
- **Anti-Gravity Zones:** Areas where normal gravity is altered
- **Teleportation Mechanics:** Mass conservation and momentum considerations
- **Dimensional Phasing:** Objects temporarily ignoring certain physical constraints

#### Time Effects
- **Decay Timers:** How long various processes take to complete
- **Seasonal Influence:** How time of year affects various physics systems
- **Day/Night Cycle:** Temperature fluctuations and lighting changes
- **Aging Effects:** Long-term physical changes to structures and materials

## Technical Implementation Considerations

<!-- C++ Note: The physics simulation can be built using a C++ physics engine (e.g., Bullet Physics, PhysX, Jolt Physics) or a custom solution if specific voxel-based interactions are paramount. C++ allows for fine-grained memory management and performance optimization crucial for large-scale physics. Multi-threading (std::thread, tasking systems) will be essential for parallelizing physics calculations (e.g., collision detection, rigid body dynamics, fluid simulation). Integration with an Entity Component System (ECS), possibly using libraries like EnTT, would be a common C++ approach for managing physics properties and behaviors of game entities. -->

### Physics Simulation Architecture

#### Simulation Levels
- **Full Simulation:** Active areas receive complete physics calculations
- **Partial Simulation:** Distant areas use simplified models
- **Suspended Physics:** Very distant areas have physics "frozen" until needed
- **Threshold System:** Dynamic adjustment based on importance and processing availability

#### Update Frequency
- **Critical Systems:** Updated every tick (structural integrity near colonists)
- **Standard Physics:** Regular update cycle (most active blocks and entities)
- **Background Effects:** Less frequent updates (distant or less crucial elements)
- **Event-Triggered:** Some physics only calculated when relevant events occur

#### Optimization Techniques
- **Chunking:** Dividing physics calculations by spatial regions
- **Caching Results:** Storing calculations that don't need frequent updates
- **Instancing:** Using same physics calculations for identical objects
- **Priority Queuing:** Managing which physics calculations happen first

### Physics Integration

#### Block Data Structure
- **Physical Properties Storage:** How material properties are encoded
- **State Tracking:** Monitoring stress, damage, temperature
- **Connection Information:** How blocks relate physically to neighbors
- **Update Requirements:** Flags indicating need for physics recalculation

#### Entity-Physics Interaction
- **Collision Detection:** Efficient algorithms for entity-world interaction
- **Movement Resolution:** Handling entity movement through physical space
- **Weight & Force:** How entity mass affects and is affected by physics
- **Special Abilities:** Entity-specific physics exceptions

#### Multi-Threading Approach
- **Parallel Calculations:** Which physics systems can run concurrently
- **Thread Safety:** Ensuring consistent state across parallel operations
- **Synchronization Points:** When physics systems must wait for others
- **Work Distribution:** Balancing physics workload across available processors

## Design Goals & Principles

### Visibility & Feedback
Physics should be visually apparent, giving players clear feedback about causes and effects.

### Dynamic Complexity
Create emergent challenges and opportunities from the interaction of simple physics rules.

### Meaningful Constraints
Physical limitations should create interesting gameplay decisions rather than just frustrations.

### Performance Consciousness
Physics depth must be balanced against computational cost, focusing detail where it matters most.

### Intuitive Behavior
Even complex physics should follow rules that players can learn and predict with experience.

## References & Inspiration

- **Dwarf Fortress:** Structural collapse and fluid systems
- **Minecraft:** Basic block physics and fluid flow
- **Oxygen Not Included:** Gas diffusion and state changes
- **Noita:** Detailed material interaction and physics
- **The Legend of Zelda: Breath of the Wild:** Physics-based puzzles and interactions
- **Real-world Civil Engineering:** Structural principles and material properties

## Development Milestones

### Phase 1: Core Block Physics
- Basic gravity for unsupported blocks
- Simple water flow mechanics
- Fundamental material properties
- Initial collision systems

### Phase 2: Structural Integrity
- Advanced support requirements
- Structural stress calculation
- Partial collapse mechanics
- Warning indicators for players

### Phase 3: Advanced Fluid & Environmental
- Complete water and lava systems
- Temperature and fire propagation
- Weather effects and natural disasters
- Gas and smoke simulation

### Phase 4: Complete Integration
- Mechanical systems and power transfer
- Full environmental physics
- Combat and destruction refinement
- Performance optimization for large-scale physics