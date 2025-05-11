# Control Systems

## Overview
The Control Systems framework establishes how players interact with and command the Voxel Fortress game world, creating a unique hybrid approach that combines direct character control with strategic colony management. This system bridges the immediacy of first-person voxel manipulation with the depth of colony simulation, providing players with multiple complementary ways to shape their fortress and society.

> **Related Documents:**
> - [Player Interaction](./Player%20Interaction.md)
> - [Colony Simulation](./Colony%20Simulation.md)
> - [UI Design](./UI%20Design.md) (Planned)
> - [Main Game Concept](../Voxel%20Fortress%20Game%20Concept%20Details.md)

## Core Components

### Control Modes

#### Avatar Control
- **Character Embodiment:** Controlling a specific colonist from first or third-person perspective
- **Selection Mechanism:** Methods for choosing which colonist to control
- **Handoff Process:** Smooth transitions when switching controlled characters
- **Identity Indicators:** Clear visual cues showing which colonist is player-controlled
- **Ability Access:** How to utilize the controlled colonist's specific skills and traits

#### Strategic Overseer Mode
- **Camera Controls:** Free camera movement independent of any colonist
- **View Transitions:** Seamless shifts between close, medium, and distant views
- **Selection Mechanics:** Methods for selecting individuals, groups, or zones
- **Command Interface:** Issuing orders without being tied to a specific colonist's position
- **Information Display:** Enhanced data visualization in this mode

#### Mode Switching
- **Transition Controls:** Hotkeys or UI elements for switching between modes
- **Context Retention:** Preserving selections and commands across mode changes
- **Situational Awareness:** Providing appropriate information during transitions
- **State Memory:** Remembering camera positions and selections when returning to a mode
- **Automation During Absence:** How colonists behave when not directly controlled

### Direct Control Mechanics

#### Movement & Navigation
- **Basic Movement:** WASD/controller movement appropriate to perspective
- **Enhanced Mobility:** Jumping, climbing, swimming mechanics
- **Navigation Assistance:** Pathfinding help for complex environments
- **Obstacle Handling:** How controlled characters deal with barriers
- **Speed Factors:** Effects of encumbrance, terrain, and character attributes

#### Environmental Interaction
- **Block Manipulation:** Mining, placing, and modifying blocks directly
- **Object Interaction:** Using furniture, doors, mechanisms, and special objects
- **Tool Usage:** Equipping and employing tools for specific interactions
- **Context Sensitivity:** Adaptive interactions based on target and situation
- **Reach Limitations:** Realistic constraints on interaction distance

#### Character Actions
- **Basic Actions:** Common activities like eating, sleeping, equipping items
- **Skill-Based Actions:** Specialized activities based on character proficiencies
- **Social Interactions:** Direct conversation and relationship building
- **Combat Control:** Attack targeting, defensive positioning, weapon use
- **Emergency Actions:** Quick responses to threats or hazards

#### First-Person Experience
- **Immersive Elements:** Viewpoint bobbing, breathing, footsteps
- **Condition Feedback:** Visual/audio cues reflecting character state
- **Environmental Awareness:** Perception based on character's capabilities
- **Skill Expression:** How character expertise affects first-person gameplay
- **Personal Inventory:** Managing carried items and equipment

### Indirect Control Systems

#### Order System
- **Command Types:** Different categories of instructions (work, move, prioritize)
- **Order Specificity:** General vs. precise instructions
- **Priority Levels:** Urgency settings for different commands
- **Order Queue:** Managing and reviewing pending instructions
- **Feedback Loop:** Information on order receipt, progress, and completion

#### Designation Interface
- **Area Selection:** Tools for marking regions for specific purposes
- **Activity Zones:** Designating areas for mining, farming, stockpiles, etc.
- **Structure Planning:** Blueprint placement and construction management
- **Restriction Zones:** Controlling where colonists can or cannot go
- **Mass Designation:** Tools for efficient large-scale planning

#### Automation & Policies
- **Standing Orders:** Persistent instructions that apply continuously
- **Conditional Rules:** "If-then" automation for routine decisions
- **Priority Framework:** Global system for resolving competing tasks
- **Schedule Management:** Time-based activity planning and shifts
- **Emergency Protocols:** Pre-configured responses to threats

#### Population Management
- **Role Assignment:** Setting professional roles and responsibilities
- **Group Organization:** Creating and managing teams or squads
- **Individual Specialization:** Focusing colonists on specific skill development
- **Hierarchy Setting:** Establishing leadership and authority structures
- **Draft System:** Temporary direct control for military operations

### Hybrid Integration

#### Seamless Workflows
- **Complementary Actions:** How direct and indirect controls work together
- **Task Continuity:** Maintaining workflow when switching between modes
- **Control Handoffs:** Transitioning tasks between player and AI control
- **Context Recognition:** System adapts to player control preferences
- **Nested Control:** Issuing strategic commands while in direct control

#### Colonist AI Interaction
- **Autonomy Spectrum:** Settings for how much initiative colonists take
- **Override System:** Player authority over AI decisions
- **Suggestion Framework:** AI colonists propose actions for player consideration
- **Feedback Mechanisms:** How colonists respond to player control patterns
- **Learning Components:** AI adaptation to player management style

#### Responsive Simulation
- **Time Control:** Pausing, slowing, or accelerating simulation during control modes
- **Detail Scaling:** Simulation depth adjustment based on control context
- **Reaction Systems:** How uncontrolled elements respond to player actions
- **Causal Clarity:** Making cause-effect relationships clear across control modes
- **Intervention Points:** Critical moments that prompt mode switching

## Interface Design

### Mode-Specific UI

#### Avatar Mode Interface
- **Minimalist HUD:** Essential information only when directly controlling
- **Context Actions:** Dynamic control prompts based on situation
- **Status Indicators:** Health, stamina, equipped items for controlled character
- **Environmental Information:** Temperature, hazards, interactable objects
- **Quick Commands:** Limited strategic options while in direct control

#### Strategic Mode Interface
- **Command Panels:** Comprehensive management tools and information
- **Overview Displays:** Colony-wide status summaries
- **Selection Details:** In-depth information about selected entities or areas
- **Alert System:** Notifications requiring attention
- **Planning Tools:** Long-term strategy and design interfaces

#### Shared UI Elements
- **Mode Indicator:** Clear visual cue of current control mode
- **Quick Switch Controls:** Easy access to mode transition
- **Persistent Information:** Critical data visible in all modes
- **Notification Framework:** Priority-based alert system
- **Time Controls:** Simulation speed adjustment in all modes

### Information Flow

#### Contextual Guidance
- **Control Hints:** Suggestions based on current situation
- **Tutorial Elements:** Progressive introduction of control mechanisms
- **Opportunity Highlighting:** Drawing attention to useful actions
- **Error Prevention:** Warning about potentially problematic commands
- **Learning Assistance:** Helping players discover control synergies

#### Command Feedback
- **Action Confirmation:** Clear indication of successful commands
- **Execution Updates:** Progress information on ongoing actions
- **Failure Notification:** Explanations when commands cannot be completed
- **Chain Reactions:** Visibility of consequences from actions
- **Historical Record:** Log of significant commands and outcomes

#### Information Layers
- **Filter System:** Toggling different information overlays
- **Detail Levels:** Adjusting information density based on player preference
- **Spatial Representation:** Map-based vs. list-based information
- **Temporal Data:** Historical trends and future projections
- **Relationship Visualization:** Connections between different data points

## Advanced Systems

### Specialized Control Contexts

#### Combat Control
- **Tactical Mode:** Specialized interface for combat situations
- **Unit Positioning:** Explicit placement and formation controls
- **Target Prioritization:** Systems for directing attack focus
- **Defensive Planning:** Setting up protective positions and fallbacks
- **Real-time/Turn-based Options:** Flexibility in combat pacing

#### Construction Management
- **Blueprint System:** Planning structures before building
- **Material Allocation:** Directing specific resources to projects
- **Worker Assignment:** Controlling which colonists handle construction
- **Quality Control:** Influencing the finished quality of structures
- **Renovation Controls:** Systems for modifying existing structures

#### Crisis Management
- **Emergency Mode:** Streamlined interface during critical situations
- **Triage System:** Prioritizing responses during multiple crises
- **Resource Diversion:** Quickly reallocating materials and personnel
- **Evacuation Controls:** Managing colonist movement during dangers
- **Recovery Direction:** Post-crisis cleanup and rebuilding

### Advanced Control Techniques

#### Macros & Templates
- **Command Sequences:** Recording series of actions for reuse
- **Design Templates:** Saving and applying building designs
- **Workflow Patterns:** Reusable work assignment configurations
- **Production Presets:** Saved manufacturing specifications
- **Custom Control Sets:** Player-defined command groupings

#### Delegation Systems
- **Manager Roles:** Assigning colonists to oversee specific areas
- **Authority Levels:** Setting how much discretion managers have
- **Approval Requirements:** Decisions requiring player confirmation
- **Management Hierarchy:** Chains of command among colonists
- **Performance Review:** Evaluating and adjusting delegation effectiveness

#### Scenario-Specific Controls
- **Expedition Interface:** Specialized controls for exploration parties
- **Trade Controls:** Dedicated interface for merchant interactions
- **Diplomatic Systems:** Controls for inter-faction communication
- **Research Direction:** Specialized interface for knowledge development
- **Festival Management:** Controls for organizing social events

### Extended Control Options

#### Remote Viewing & Control
- **Observer Mode:** Viewing distant areas without direct presence
- **Remote Orders:** Issuing commands to distant parts of the colony
- **Map-Based Control:** Strategic interface for regional management
- **Scout Integration:** Using explorer reports for informed remote decisions
- **Communication Limits:** Realistic constraints on long-distance control

#### Temporal Control
- **Planning Queue:** Scheduling actions for future execution
- **Conditional Timing:** "When X happens, do Y" instructions
- **Calendar System:** Long-term planning across seasons/years
- **Historical Review:** Examining and learning from past decisions
- **Predictive Tools:** Forecasting outcomes of potential decisions

#### Multiplayer Considerations (If Applicable)
- **Control Division:** How multiple players share control responsibilities
- **Permission System:** Managing what different players can control
- **Collaborative Tools:** Features facilitating teamwork
- **Conflict Resolution:** Systems for handling contradictory commands
- **Synchronization:** Maintaining consistent control across network conditions

## Technical Implementation Considerations

### Input Handling Architecture
- **Control Mapping:** Flexible system for associating inputs with actions. *(Consider using C++ libraries like SDL, SFML, or a custom input manager for handling diverse input devices and remapping.)*
- **Context Sensitivity:** Input interpretation based on current state and mode
- **Input Buffering:** Handling rapid or complex input sequences
- **Error Tolerance:** Graceful handling of ambiguous or problematic inputs
- **Accessibility Options:** Alternative control schemes for different needs

### Command Processing
- **Command Queue:** Efficient processing of pending instructions. *(C++ standard library containers like `std::queue` or `std::deque` can be used, or custom lock-free queues for multithreaded scenarios.)*
- **Priority Resolution:** Algorithms for determining action order
- **Validation System:** Checking commands for feasibility before execution
- **Cancellation Handling:** Clean abortion of in-progress actions
- **Dependency Chains:** Managing actions that require prerequisites

### Simulation Interface
- **State Synchronization:** Keeping UI and simulation data consistent. *(C++ allows for direct memory manipulation, which can be leveraged for efficient state updates, but requires careful management to avoid race conditions or stale data. Consider event-based systems or observer patterns.)*
- **Update Optimization:** Efficient information flow between systems
- **Event Communication:** Notifying control systems of relevant changes
- **Prediction Models:** Anticipating outcomes of potential commands
- **Time Scale Management:** Maintaining control responsiveness at different simulation speeds

## Design Goals & Principles

### Intuitive Duality
The hybrid control system should feel natural and complementary, not like two separate games bolted together.

### Depth Without Complexity
Advanced control options should emerge from combining simple, understandable mechanics.

### Contextual Appropriateness
The right control method should be available at the right time without unnecessary mode switching.

### Player Expression
Control systems should support diverse play styles from micromanagement to high-level direction.

### Consistent Logic
Control behaviors should follow predictable patterns that players can learn and master.

## References & Inspiration

- **Dwarf Fortress:** Deep indirect control systems and designation tools
- **Minecraft:** Direct block manipulation and first-person interaction
- **The Sims:** Character control and needs management
- **RimWorld:** Work prioritization and colony management
- **Dungeon Keeper:** Avatar possession mechanic within management game
- **Evil Genius:** Room designation and minion management systems
- **Oxygen Not Included:** Priority-based task assignment and overlay systems

## Development Milestones

### Phase 1: Core Mode Framework
- Basic implementation of direct and strategic control modes
- Simple mode switching mechanism
- Fundamental camera systems for both perspectives
- Essential UI elements for each mode

### Phase 2: Direct Control Refinement
- Polished first-person controls and interaction
- Character embodiment features
- Environmental manipulation tools
- Direct action feedback systems

### Phase 3: Strategic Control Depth
- Comprehensive designation system
- Advanced order and priority framework
- Colony management interfaces
- Information visualization tools

### Phase 4: Complete Integration
- Seamless mode transitions
- Context-sensitive control suggestions
- Specialized control scenarios
- Advanced delegation and automation systems
- Performance optimization and polish