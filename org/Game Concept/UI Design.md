# UI Design

## Overview
The UI Design system serves as the critical interface between the player and Voxel Fortress's complex simulation systems. Its primary goal is to transform the game's deep mechanics into intuitive, accessible interactions without sacrificing depth or control. This system addresses one of the project's core challenges: making a Dwarf Fortress-inspired simulation approachable while preserving the rich gameplay that emerges from interacting with complex, interconnected systems.

> **Related Documents:**
> - [Control Systems](./Control%20Systems.md)
> - [Player Interaction](./Player%20Interaction.md)
> - [Colony Simulation](./Colony%20Simulation.md)
> - [Performance Optimization](./Performance%20Optimization.md)
> - [Main Game Concept](../Voxel%20Fortress%20Game%20Concept%20Details.md)

## Core Components

### UI Architecture

#### Design Philosophy
- **Progressive Disclosure:** Revealing complexity gradually as players need it
- **Context Sensitivity:** UI adapts to current player activity and environment
- **Consistent Language:** Uniform visual and interaction patterns across systems
- **Information Hierarchy:** Most critical data emphasized visually and positionally
- **Direct Manipulation:** Whenever possible, interact with game elements directly rather than through abstract menus

#### UI Layers
- **Persistent HUD:** Always-visible core information and quick access controls
- **Context Panels:** Information and controls relevant to current selection/activity
- **Management Interfaces:** Deeper systems for detailed colony oversight
- **Modal Windows:** Focused interfaces for specific complex tasks
- **Tooltip/Help System:** On-demand explanation of UI elements and mechanics

#### Responsive Design
- **Screen Scaling:** Adapting to different resolutions and aspect ratios
- **UI Density Options:** Adjustable information density for player preference
- **Size Controls:** User-adjustable text and element sizing
- **Layout Flexibility:** Rearrangeable interface components for customization
- **Platform Adaptation:** Considerations for different input devices (mouse/keyboard, controller, touch)

### Core Interface Elements

#### Selection System
- **Entity Selection:** Click, drag-select, double-click conventions
- **Selection Memory:** Remembering previous selections for quick recall
- **Group Selection:** Creating and managing saved entity groups
- **Filter Selection:** Selecting entities based on properties or conditions
- **Selection Visualization:** Clear indicators showing what is currently selected

#### Command Interface
- **Context Commands:** Actions available for the current selection
- **Command Categories:** Logical grouping of related functions
- **Command History:** Record of recent actions with undo capabilities
- **Command Queue:** Visualization of pending/in-progress actions
- **Keyboard Shortcuts:** Comprehensive hotkey system for efficiency

#### Information Display
- **Entity Details:** Showing properties of selected entities
- **Status Indicators:** Visual representation of important conditions
- **Alert System:** Tiered notification system for events requiring attention
- **Resource Tracking:** Clear visualization of available and needed resources
- **Comparative Information:** Tools for comparing different entities or states

#### Navigation Controls
- **Camera Management:** Controls for view position, angle, zoom
- **Map System:** Minimap, region map, and world map at different scales
- **Location Bookmarks:** Saving and quickly accessing important locations
- **Search Functionality:** Finding specific entities or locations by name/type
- **Focus Controls:** Quickly centering view on relevant activity or alerts

### Management Systems

#### Colony Dashboard
- **Overview Panel:** At-a-glance colony health and status
- **Population Management:** Tools for reviewing and organizing colonists
- **Resource Summary:** Current stores and consumption/production rates
- **Priority Management:** Global settings for work importance
- **Alert Management:** Centralized system for reviewing and addressing issues

#### Work Management
- **Job Assignment:** Interface for assigning duties to colonists
- **Job Queue:** Visualization and manipulation of pending tasks
- **Priority System:** Setting importance levels for different activities
- **Automation Tools:** Setting up standing orders and conditions
- **Schedule Management:** Planning work periods and shifts

#### Construction Interface
- **Blueprint System:** Tools for designing structures
- **Material Selection:** Choosing components for planned buildings
- **Progress Tracking:** Visual feedback on construction status
- **Modification Tools:** Editing existing structures
- **Template System:** Saving and reusing designs

#### Resource Management
- **Stockpile Configuration:** Setting storage parameters and priorities
- **Production Chains:** Visualizing and managing resource processing
- **Trading Interface:** Tools for managing exchange with other factions
- **Consumption Controls:** Setting usage policies for different resources
- **Resource Filters:** Specifying what items go where

### Specialized Interfaces

#### Military & Security
- **Alert Levels:** Setting colony-wide security status
- **Squad Management:** Organizing and controlling defensive units
- **Tactical Map:** Strategic overview of threats and defenses
- **Equipment Assignment:** Managing military gear and loadouts
- **Training Schedule:** Organizing combat practice and skill development

#### Social & Health Management
- **Relationship Viewer:** Visualizing social connections between colonists
- **Mood Management:** Tools for addressing psychological needs
- **Medical Interface:** Health tracking and treatment assignment
- **Social Event Planning:** Organizing gatherings and celebrations
- **Individual Development:** Training and skill advancement planning

#### Research & Development
- **Technology Tree:** Visual representation of advancement options
- **Project Management:** Assigning and tracking research efforts
- **Knowledge Base:** Accessing discovered information and blueprints
- **Experiment Interface:** Tools for conducting and monitoring tests
- **Skills Development:** Planning education and training programs

#### Logistics Network
- **Transportation Overview:** Visualization of material movement
- **Route Management:** Setting and optimizing delivery paths
- **Bottleneck Analysis:** Identifying logistical problems
- **Vehicle Assignment:** Managing carts, animals, or other transport
- **Supply Chain Visualization:** End-to-end resource flow tracking

## Information Visualization

### Data Representation

#### Textual Information
- **Typography Hierarchy:** Different weights, sizes, and styles for information importance
- **Color Coding:** Consistent use of colors for different information types
- **Numerical Representation:** Appropriate display of statistics (raw numbers vs. bars/charts)
- **Abbreviated Views:** Condensed information with expansion options
- **Search & Filter:** Tools for finding specific information in large datasets

#### Graphical Elements
- **Icon Design:** Clear, distinctive icons for different items, actions, and statuses
- **Progress Indicators:** Visual representation of completion status
- **State Visualization:** Clear indicators of entity and system conditions
- **Comparative Graphics:** Visual tools for contrasting different options
- **Animation Usage:** Strategic movement to draw attention or show change

#### Charts & Graphs
- **Historical Trends:** Tracking changes in important metrics over time
- **Distribution Displays:** Showing how resources or attributes are spread
- **Relationship Networks:** Visual representation of connections between entities
- **Heat Maps:** Spatial representation of data intensity
- **Comparative Bars:** Side-by-side comparison of different values

### Spatial Information

#### World Map System
- **Multi-Scale Display:** Seamless zooming between detail levels
- **Information Layers:** Toggleable overlays showing different data sets
- **Region Highlighting:** Drawing attention to areas of interest
- **Navigation Tools:** Logical movement between different locations
- **Annotation System:** Adding notes and markers to locations

#### Overlay Systems
- **Zone Visualization:** Clear indicators of designated areas
- **Resource Highlighting:** Showing where materials/items are located
- **Danger Zones:** Marking areas with specific hazards
- **Temperature Map:** Visual representation of environmental conditions
- **Activity Tracking:** Showing where colonists are working/moving

#### Pathfinding Visualization
- **Route Display:** Showing planned paths for entities
- **Accessibility Mapping:** Indicating what areas can be reached
- **Traffic Analysis:** Highlighting congested or heavily used paths
- **Distance Indicators:** Visual representation of travel time/difficulty
- **Blockage Warnings:** Clearly marking inaccessible areas

## Advanced UI Systems

### Contextual Help & Learning

#### Tutorial Systems
- **Guided Introduction:** Step-by-step walkthrough of core mechanics
- **Context Tutorials:** Just-in-time instruction when accessing new features
- **Practice Scenarios:** Structured challenges to learn specific systems
- **Complexity Progression:** Gated introduction of advanced features
- **Reference Library:** Comprehensive searchable game information

#### In-Game Assistance
- **Tooltips:** On-hover explanations of interface elements
- **Contextual Hints:** Suggestions based on current game state
- **Problem Indicators:** Highlighting specific issues with suggestions
- **Question System:** Direct help for specific player queries
- **Mentor Messages:** Periodic tips based on play patterns

#### Interface Guidance
- **Highlighting:** Drawing attention to relevant controls
- **Breadcrumbs:** Showing navigation path through complex interfaces
- **Search Functionality:** Finding specific controls or information
- **Simplified Modes:** Optional streamlined interfaces for new players
- **Guided Workflows:** Step-by-step assistance for complex tasks

### UI Customization

#### Personal Configuration
- **Layout Adjustment:** Moving and resizing interface elements
- **Visibility Options:** Showing/hiding different information components
- **Color Schemes:** Preset and custom UI color options
- **Preset Configurations:** Ready-made UI setups for different playstyles
- **Saved Profiles:** Multiple UI configurations for different situations

#### Notification Management
- **Alert Priority:** Setting importance levels for different notification types
- **Notification Filtering:** Controlling which alerts appear and how
- **Digest Options:** Grouping similar notifications to reduce interruption
- **Timing Control:** Setting when and how alerts are delivered
- **Follow-up Tracking:** Marking and monitoring addressed issues

#### Interface Extensions
- **Custom Panels:** Creating specialized information displays
- **Query Tools:** Building custom filters and searches
- **Macro System:** Recording and playing back UI interactions
- **Dashboard Creation:** Combining relevant information into personal views
- **Bookmark System:** Creating quick access to frequently used interfaces

### Specialized Visualization Tools

#### Time-Based Visualization
- **History Timeline:** Scrollable record of colony events
- **Scheduling Interface:** Planning future activities on a calendar
- **Cycle Tracking:** Visualizing patterns across day/night and seasons
- **Projection Tools:** Forecasting future resource levels and needs
- **Playback Controls:** Reviewing past events in detail

#### Relationship Mapping
- **Social Network:** Interactive visualization of colonist relationships
- **Faction Diagram:** Diplomatic connections with external groups
- **Production Chains:** Complete visualization of resource transformations
- **Dependency Maps:** What systems rely on which others
- **Influence Networks:** How events and decisions ripple through systems

#### Comparison Tools
- **Before/After Views:** Contrasting states across time
- **Side-by-Side Analysis:** Comparing different entities or options
- **Scenario Testing:** Projecting outcomes of different decisions
- **Benchmark Reference:** Comparing current state to ideal targets
- **Historical Context:** Current metrics in relation to past performance

## Technical Implementation Considerations

### UI Framework Architecture

#### Component System
- **Reusable Elements:** Library of standard UI components (buttons, sliders, text boxes, etc.). In C++, this could be a custom-built library or leverage parts of a framework like Dear ImGui, Qt (if a heavier solution is acceptable for tools), or custom classes for in-game UI elements.
- **Inheritance Structure:** Building complex elements from simpler ones using C++ class inheritance or composition.
- **Style Application:** Consistent visual treatment across components, possibly managed by a C++ styling system or theme data loaded from files.
- **Event Handling:** Clean separation of visual elements and logic. C++ event systems (e.g., signals/slots, observer pattern, function callbacks) would connect UI events to game logic.
- **Rendering Optimization:** Efficient drawing of interface elements, often managed by the chosen C++ UI library or custom rendering code using graphics APIs (Vulkan, OpenGL, DirectX).

#### Data Binding
- **Model-View Separation:** Clean division between data (C++ game state) and presentation (UI elements). Patterns like Model-View-ViewModel (MVVM) or Model-View-Presenter (MVP) can be adapted in C++.
- **Update Mechanisms:** Efficient refreshing when data changes. This might involve C++ observer patterns, event notifications, or dirty flagging systems to trigger UI updates.
- **Lazy Loading:** Only generating UI components when needed.
- **View Recycling:** Reusing UI elements for similar data (common in list views).
- **Batch Updates:** Grouping multiple C++ state changes to reduce rendering passes for the UI.

#### Input Management
- **Device Abstraction:** Handling different input methods consistently via a C++ input layer (e.g., using SDL, GLFW, or custom input handling).
- **Focus System:** Tracking which UI element is currently active, managed by C++ UI framework logic.
- **Input Priority:** Resolving conflicts between different handlers (e.g., game input vs. UI input).
- **Gesture Recognition:** Interpreting complex input patterns, potentially with C++ libraries or custom algorithms.
- **Accessibility Inputs:** Support for alternative control methods.

### Performance Optimization

#### Rendering Efficiency
- **Draw Call Batching:** Minimizing rendering state changes. Critical for custom C++ UI renderers using graphics APIs.
- **Clipping & Culling:** Only rendering visible UI elements.
- **Layer Management:** Strategic organization to minimize redraws.
- **Resolution Independence:** Efficient scaling for different displays, handled by C++ layout and rendering code.
- **Asset Management:** Smart loading and unloading of UI resources (textures, fonts) in C++.

#### Update Throttling
- **Change Detection:** Only updating what has actually changed.
- **Importance-Based Updates:** More frequent refreshes for critical info.
- **Deferred Updates:** Grouping non-critical changes.
- **Animation Optimization:** Efficient handling of moving elements, possibly using C++ tweening libraries or custom animation curves.
- **Background Processing:** Heavy computations off the main UI thread using C++ threading (`std::thread`, `std::async`).

#### Memory Management
- **Asset Pooling:** Reusing common UI elements and resources. Custom C++ memory pool allocators can be beneficial.
- **View Virtualization:** Only instantiating visible portions of large lists/grids.
- **Cache Strategy:** Smart retention of frequently accessed UI data.
- **Garbage Collection Awareness:** While C++ is not garbage collected by default, smart pointers (`std::unique_ptr`, `std::shared_ptr`) must be used correctly to manage memory. For UI systems with many small objects, custom allocators or arena allocators can improve performance over frequent heap allocations.
- **Memory Budgeting:** Enforcing limits on UI resource usage through C++ tracking mechanisms.

## Design Goals & Principles

### Clarity Over Decoration
UI elements should prioritize clear communication over ornate design, though aesthetic considerations remain important.

### Context Over Comprehensiveness
Show what's relevant to the current situation rather than overwhelming with all possible information.

### Consistency Builds Mastery
Use consistent patterns across all systems so skills learned in one area transfer to others.

### Direct Manipulation
Favor interaction with game objects over abstract menus whenever possible.

### Scaling Complexity
Start simple and reveal depth gradually as players demonstrate readiness.

## References & Inspiration

- **Dwarf Fortress (Steam):** Successful modernization of complex simulation UI
- **RimWorld:** Accessible colony simulation with clean information hierarchy
- **Oxygen Not Included:** Clear visualization of complex systems and overlays
- **Factorio:** Excellent production chain management and logical organization
- **Cities: Skylines:** Intuitive handling of many interconnected systems
- **Paradox Grand Strategy Games:** Approaches to presenting enormous amounts of data
- **Minecraft (with mods):** Various UI approaches to block-based world interaction

## Development Milestones

### Phase 1: Core Interface Framework
- Base UI component library
- Selection and command systems
- Basic information display
- Fundamental navigation controls

### Phase 2: Management Interfaces
- Colony dashboard
- Work management system
- Construction interface
- Resource management tools

### Phase 3: Advanced Visualization
- Data representation systems
- Spatial information tools
- Specialized interfaces for different subsystems
- Help and tutorial foundations

### Phase 4: Complete Integration
- Full customization capabilities
- Advanced comparison and analysis tools
- Comprehensive tutorial and help systems
- Performance optimization and polish
- Accessibility features and options