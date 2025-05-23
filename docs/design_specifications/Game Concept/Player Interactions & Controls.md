# Player Interaction & Controls

## Overview
The Player Interaction system defines how players engage with and control the Voxel Fortress game world. This system bridges the player's intent with in-game actions through intuitive controls, clear feedback, and flexible interaction modes. By balancing direct involvement with strategic management, the interaction model aims to make complex colony simulation accessible without sacrificing depth.

> **Related Documents:**
> - [Visual Design](./Visual%20Design.md)
> - [Colony Simulation](./Colony%20Simulation.md)
> - [Control Systems](./Control%20Systems.md) (Planned)
> - [Main Game Concept](../Voxel%20Fortress%20Game%20Concept%20Details.md)

## Core Components

### Interaction Modes

#### First-Person Perspective
- **Player Avatar:** Controlling a specific colonist from their viewpoint
- **Direct Interaction:** Physically interacting with blocks, objects, and characters
- **Immersive Experience:** Experiencing the colony from ground level
- **Skill Expression:** Using player skill for tasks like combat or precision building
- **Perspective Switch:** Ability to inhabit different colonists for different roles

#### Strategic Overview
- **Management View:** Camera positioned above the colony
- **Variable Height:** Adjustable zoom from close-in to bird's-eye view
- **Free Navigation:** Camera movement independent of any specific colonist
- **Information Overlays:** Access to colony-wide data visualizations
- **Multi-Tasking:** Managing several operations across the colony simultaneously

#### Hybrid System
- **Seamless Transition:** Quick switching between modes with a dedicated key/button
- **Context Sensitivity:** Automatically suggested mode based on current activity
- **Split Attention:** Managing colony while controlling an avatar during critical moments
- **Companion Mode:** Controlling one colonist directly while managing others indirectly
- **Preference Settings:** Allowing players to emphasize their preferred interaction style

### Control Schemes

#### Keyboard & Mouse Controls
- **Movement:** WASD for camera/character movement
- **Camera Control:** Mouse for looking/aiming in first-person; rotation, pan, and zoom in overhead view
- **Context Actions:** Right-click for context menu, left-click for primary action
- **Hotkeys:** Customizable shortcuts for frequently used commands
- **Selection System:** Click, drag-select, and modifier keys for entity selection
- **Command Modifiers:** Shift/Ctrl/Alt combinations for extended functions

#### Controller Support
- **Dual Analog:** Left stick for movement, right for camera
- **Context Sensitivity:** Button functions that change based on current selection/mode
- **Radial Menus:** Quick access to common actions via controller-friendly UI
- **Cursor Control:** Precision mode for detailed selection when needed
- **Haptic Feedback:** Appropriate vibration for different actions and events

#### Touch/Mobile Considerations (Optional)
- **Gesture Support:** Pinch to zoom, swipe to rotate, tap to select
- **On-screen Controls:** Virtual joysticks or context-sensitive buttons
- **Simplified Interface:** Larger touch targets and streamlined commands
- **Size Optimization:** UI scaling for different screen dimensions

### Direct Interaction Systems

#### Block Manipulation
- **Block Breaking:** Mining/demolishing existing blocks with appropriate tools
- **Block Placement:** Adding new blocks from inventory or available resources
- **Block Modification:** Changing block properties or appearance (e.g., carving, painting)
- **Build Reach:** Reasonable distance limitations for block interaction
- **Physics Awareness:** Feedback when placed blocks might collapse or cause issues

#### Entity Interaction
- **Colonist Communication:** Direct conversation, orders, information requests
- **Item Handling:** Picking up, dropping, using, or transferring items
- **Device Operation:** Activating mechanisms, operating machinery
- **Creature Interaction:** Dealing with animals (domesticated or wild) and other beings
- **Combat Engagement:** Direct control during combat situations

#### Environment Interaction
- **Resource Gathering:** Harvesting resources from environment
- **Navigation Aids:** Climbing, swimming, potentially jumping between levels
- **Environmental Hazards:** Direct response to immediate dangers
- **Discovery:** Examining and interacting with points of interest
- **Tool Usage:** Using specific tools for specialized interactions

### Designation & Management Systems

#### Blueprint System
- **Structure Planning:** Laying out buildings before construction
- **Template Usage:** Applying saved designs to new locations
- **Material Selection:** Choosing specific materials for planned structures
- **Modification Tools:** Editing existing blueprints
- **Validation Feedback:** Warnings about potential issues with designs

#### Zone Designation
- **Area Types:** Marking regions for specific purposes (farming, storage, residence)
- **Permission Settings:** Controlling who can access different areas
- **Priority Levels:** Setting importance for development or resource allocation
- **Visual Clarity:** Clear differentiation between zone types
- **Overlapping Considerations:** Rules for handling intersecting designations

#### Order Management
- **Task Assignment:** Giving direct orders to individuals or groups
- **Priority System:** Setting urgency levels for different tasks
- **Condition-Based Orders:** "Do X when Y happens" instructions
- **Scheduling:** Planning activities for different times of day
- **Order Chaining:** Creating sequences of related tasks

#### Resource Allocation
- **Material Reservation:** Setting aside resources for specific projects
- **Distribution Priorities:** Directing where resources should go first
- **Consumption Policies:** Rules for using different quality/types of resources
- **Stockpile Management:** Organizing storage areas and contents
- **Production Quotas:** Setting target amounts for created items

## Information Systems

### User Interface

#### HUD Elements
- **Status Indicators:** Health, hunger, equipped items for controlled colonist
- **Minimap:** Local surroundings awareness
- **Alert System:** Notifications of important events
- **Context Actions:** Available interactions based on current selection
- **Quick Access:** Frequently used tools and commands

#### Management Panels
- **Colony Overview:** Population, resources, threats at a glance
- **Detailed Information:** Deep-dive into specific colonists, buildings, or systems
- **Organization Tools:** Filtering, sorting, and grouping information
- **Tabbed Interface:** Logical separation of different management aspects
- **Scaling Options:** Adjustable UI size and density

#### Information Visualization
- **Resource Graphs:** Tracking production and consumption over time
- **Relationship Networks:** Visual representation of colonist interactions
- **Heat Maps:** Showing activity, danger, or resource concentration
- **Workflow Diagrams:** Production chains and resource movement
- **Status Overlays:** Visual indicators projected onto the game world

### Feedback Systems

#### Visual Feedback
- **Selection Highlighting:** Clear indicators of selected entities or blocks
- **Action Confirmation:** Visual cues for successful or failed actions
- **Progress Indicators:** Showing completion status of ongoing tasks
- **State Changes:** Visual transformation of affected objects or entities
- **Warning Signals:** Clear indicators of potential problems

#### Audio Feedback
- **Interaction Sounds:** Distinct audio for different actions
- **Ambient Information:** Sound cues indicating nearby activities
- **Alert System:** Audio notifications of important events
- **Spatial Audio:** Direction and distance information through sound
- **Success/Failure Cues:** Different sounds based on outcome

#### Haptic Feedback (For Controllers)
- **Action Confirmation:** Subtle vibration for completed actions
- **Environmental Effects:** Feedback for different terrain types
- **Resistance Simulation:** Variable vibration for different materials
- **Alert Patterns:** Distinct vibration patterns for different warnings
- **Intensity Scaling:** Stronger feedback for more significant events

## Advanced Systems

### Accessibility Features

#### Control Customization
- **Rebindable Controls:** Complete control remapping
- **Alternative Schemes:** Preset options for different playstyles or needs
- **Simplified Modes:** Options to reduce control complexity
- **Adaptive Controls:** Support for specialized input devices
- **Timing Adjustments:** Options to modify interaction timing requirements

#### Visual Accessibility
- **Text Scaling:** Adjustable text size throughout the UI
- **High Contrast Mode:** Enhanced visual distinction between elements
- **Color Blind Options:** Alternative color schemes for different types of color blindness
- **Screen Reader Support:** Text-to-speech for UI elements
- **Visual Noise Reduction:** Options to minimize non-essential visual elements

#### Cognitive Accessibility
- **Tutorial Systems:** Contextual guidance for game mechanics
- **Complexity Scaling:** Options to gradually introduce advanced features
- **Task Reminders:** Optional prompts for ongoing objectives
- **Pause Functionality:** Ability to freeze time for decision making
- **Automation Options:** Configurable AI assistance for routine tasks

### Advanced Interaction Techniques

#### Tool Systems
- **Specialized Tools:** Different tools for different interaction types
- **Tool Upgrading:** Improving tool capabilities over time
- **Quick Switching:** Fast access to appropriate tools for current tasks
- **Tool Combinations:** Using multiple tools together for complex actions
- **Custom Tools:** Creating specialized tools for specific colony needs

#### Mass Selection & Commands
- **Selection Patterns:** Different methods for selecting multiple entities
- **Group Management:** Creating and managing saved groups
- **Batch Orders:** Applying the same command to multiple targets
- **Filter Selection:** Selecting entities based on properties or conditions
- **Chain Commands:** Creating sequences of actions for selected entities

#### Macro & Automation Systems
- **Recorded Sequences:** Creating reusable action patterns
- **Conditional Rules:** Setting up if-then scenarios for colonist behavior
- **Production Automations:** Setting up self-sustaining production workflows
- **Scheduled Actions:** Planning tasks for future execution
- **Templates & Presets:** Saving and applying complex configurations

### Specialized Interaction Contexts

#### Combat Control
- **Tactical View:** Specialized interface for combat situations
- **Unit Positioning:** Controlling colonist formations and positions
- **Target Selection:** Designating priority enemies or objectives
- **Combat Orders:** Special commands for combat scenarios
- **Real-time vs. Pause-based:** Options for different combat control styles

#### Expedition & Exploration
- **Scouting Controls:** Managing explorers in uncharted territory
- **Map Marking:** Adding notes and indicators to discovered areas
- **Navigation Tools:** Pathfinding assistance for difficult terrain
- **Discovery Interface:** Special controls for examining and documenting findings
- **Return Planning:** Setting up extraction routes and contingencies

#### Crisis Management
- **Emergency Interface:** Streamlined controls during critical situations
- **Priority Overrides:** Quick reprioritization of colony resources and personnel
- **Rapid Response Tools:** Fast access to essential crisis functions
- **Status Monitoring:** Clear visualization of developing situation
- **Aftermath Controls:** Special interface elements for recovery operations

## Technical Implementation Considerations

### Input System Architecture
- **Input Abstraction:** Separation of physical inputs from game actions.
- **Action Mapping:** Flexible association of inputs to in-game functions.
- **Context Sensitivity:** Input interpretation based on game state.
- **Input Buffering:** Handling rapid or concurrent inputs appropriately.
- **Device Management:** Supporting multiple input devices simultaneously.
- **C++ Libraries:** Consider using libraries like SDL, GLFW for cross-platform input handling, or platform-specific APIs (e.g., WinAPI for Windows, X11 for Linux) for more direct control if needed. Event-driven architectures are common.

### UI Implementation
- **Layout Engine:** Adaptable UI positioning and scaling.
- **Rendering Pipeline:** Efficient UI drawing with minimal performance impact.
- **State Management:** Clean separation of UI state and game state.
- **Animation System:** Smooth transitions between UI states.
- **Theme Support:** Consistent visual styling with customization options.
- **C++ UI Frameworks:** For in-game UI, a custom solution built with a graphics library (like SFML Graphics, or directly via OpenGL/Vulkan/DirectX) is common. For development tools or complex editor-like interfaces, Dear ImGui is a very popular and effective choice. Consider how UI events will be processed, potentially through a dedicated UI event loop or integrated into the main game loop.

### Performance Optimization
- **Input Prioritization:** Ensuring responsive controls even under system load.
- **UI Batching:** Minimizing draw calls for interface elements.
- **Event Throttling:** Preventing excessive UI updates.
- **Asynchronous Processing:** Handling intensive operations without blocking input.
- **Memory Management:** Efficient resource usage for UI assets.
- **C++ Event Systems:** A robust event system or message queue in C++ will be crucial for decoupling input handling, UI updates, and game logic, ensuring responsiveness and maintainability. This could involve observer patterns, delegates, or more complex event bus implementations.

## Design Goals & Principles

### Intuitive Discovery
Controls should feel natural and allow players to discover functionality through experimentation.

### Consistent Feedback
Players should always understand what they're selecting and what will happen when they act.

### Scalable Complexity
Simple actions should be simple to perform, while complex actions can require more steps but remain manageable.

### Input Flexibility
Support multiple valid ways to accomplish the same task to accommodate different player preferences.

### Progressive Disclosure
Introduce interface elements gradually as they become relevant rather than overwhelming with options.

## References & Inspiration

- **Dwarf Fortress:** Deep management systems and designation tools
- **Minecraft:** Direct block interaction and first-person navigation
- **RimWorld:** Clean colony management interface and prioritization systems
- **The Sims:** Character control and environmental interaction
- **Factorio:** Blueprint system and large-scale management tools
- **City Skylines:** Information overlays and zoning systems

## Development Milestones

### Phase 1: Core Controls
- Basic movement and camera systems
- Fundamental block interaction
- Simple UI framework
- Essential feedback systems

### Phase 2: Management Tools
- Blueprint and designation systems
- Order and priority management
- Resource allocation interface
- Information visualization

### Phase 3: Mode Integration
- Seamless first-person/strategic view transitions
- Context-sensitive control schemes
- Group selection and commands
- Advanced feedback systems

### Phase 4: Complete Experience
- Full accessibility features
- Advanced automation tools
- Specialized context interfaces
- Performance optimization and polish