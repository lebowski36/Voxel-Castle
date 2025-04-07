# Resource Management

## Overview
The Resource Management system is a fundamental component of Voxel Fortress that drives the colony economy, production chains, and player decision-making. This system governs how materials are collected, processed, stored, and utilized throughout the game, creating meaningful challenges and strategic depth.

> **Related Documents:**
> - [Colony Simulation](./Colony%20Simulation.md)
> - [Skills & Professions](./Skills%20%26%20Professions.md)
> - [Main Game Concept](../Voxel%20Fortress%20Game%20Concept%20Details.md)

## Core Components

### Resource Types

#### Raw Materials
- **Organic Materials**
  - **Wood Types:** Different trees yield different wood with varying properties
  - **Plants:** Food crops, medicinal herbs, textile fibers, decorative plants
  - **Animal Products:** Meat, hides, bones, wool, milk, eggs
  - **Wild Resources:** Berries, mushrooms, wild plants with special properties
  
- **Inorganic Materials**
  - **Stone Types:** Granite, limestone, marble, etc. with different properties
  - **Minerals:** Coal, sulfur, saltpeter, gems, etc.
  - **Metals Ores:** Iron, copper, tin, gold, silver, etc.
  - **Clay:** For pottery and brick-making
  - **Sand:** For glass-making
  
- **Water Resources**
  - **Clean Water:** For drinking, brewing, farming
  - **Salt Water:** For salt production
  - **Special Waters:** Mineral-rich, magic-infused (if applicable)

#### Processed Materials
- **Refined Materials**
  - **Lumber:** Processed wood for construction and crafting
  - **Blocks:** Cut stone blocks for higher-quality construction
  - **Metal Ingots:** Smelted metals ready for smithing
  - **Cloth:** Woven from plant fibers or animal wool
  - **Leather:** Processed from animal hides
  - **Glass:** Made from sand in furnaces
  - **Pottery:** Clay items fired in kilns

- **Components**
  - **Metal Parts:** Nails, hinges, fittings, mechanisms
  - **Wooden Components:** Handles, furniture parts, barrels, etc.
  - **Building Components:** Doors, windows, support beams
  - **Specialized Items:** Gears, springs, pipes, etc.

#### Consumables
- **Food**
  - **Basic Foods:** Raw ingredients, simple meals
  - **Complex Foods:** Advanced recipes, higher quality meals
  - **Preserved Foods:** Smoked, pickled, dried, etc.
  
- **Beverages**
  - **Water:** Basic hydration
  - **Alcohol:** Beer, wine, spirits with varying effects
  - **Medicinal Drinks:** Teas, tonics, etc.
  
- **Medicine**
  - **Basic Medicine:** Simple bandages, herbal remedies
  - **Advanced Medicine:** Specialized treatments, complex compounds
  - **Emergency Supplies:** Antidotes, surgical tools

#### Finished Goods
- **Tools & Equipment**
  - **Work Tools:** Mining picks, hammers, axes, etc.
  - **Farming Equipment:** Hoes, plows, irrigation components
  - **Specialized Tools:** Medical instruments, research equipment

- **Furniture & Décor**
  - **Functional Furniture:** Beds, tables, chairs, storage
  - **Decorative Items:** Statues, tapestries, paintings
  - **Luxury Items:** High-quality versions with better effects

- **Clothing & Armor**
  - **Basic Clothing:** Everyday wear for colonists
  - **Specialized Outfits:** Profession-specific attire
  - **Armor Types:** Light, medium, heavy protection
  - **Accessories:** Belts, pouches, jewelry

- **Weapons**
  - **Melee Weapons:** Swords, axes, hammers, etc.
  - **Ranged Weapons:** Bows, crossbows, possible firearms
  - **Ammunition:** Arrows, bolts, etc.

### Resource Properties

#### Physical Properties
- **Weight:** Affects carrying capacity and transport efficiency
- **Volume:** Determines storage space requirements
- **Durability:** How well items withstand use and time
- **Flammability:** Risk of burning in fires or high temperatures
- **Decay Rate:** How quickly items deteriorate if not properly stored

#### Quality & Variation
- **Quality Levels:** Poor, normal, good, excellent, masterwork affecting effectiveness
- **Material Variations:** Same item types but made from different materials
- **Crafting Quality:** Affected by crafter's skill and workshop conditions
- **Wear & Damage:** Items degrade with use, can be repaired

#### Value & Rarity
- **Base Value:** Intrinsic worth of resources and items
- **Rarity Factors:** Some materials naturally scarce or hard to access
- **Demand Fluctuations:** Value changes based on colony needs and trader interests
- **Luxury Premium:** Items with aesthetic/status value beyond practical use

### Gathering & Production

#### Resource Collection
- **Mining:** Extracting stone, ores, gems from terrain
- **Woodcutting:** Harvesting trees for wood
- **Farming:** Cultivating crops in dedicated areas
- **Hunting/Trapping:** Acquiring animal resources
- **Foraging:** Collecting wild plants and resources
- **Water Collection:** Wells, rivers, rain collection

#### Processing Chains
- **Metal Production Pipeline:** Ore → Smelting → Ingots → Smithing → Items
- **Wood Processing:** Trees → Lumber → Carpentry → Furniture/Buildings
- **Food Production:** Raw Ingredients → Processing → Cooking → Meals
- **Textile Chain:** Fibers/Wool → Spinning → Weaving → Tailoring → Clothing

#### Workshop System
- **Specialized Facilities:** Different workshops for different crafting types
- **Equipment Quality:** Affects efficiency and product quality
- **Workshop Layout:** Impacts productivity and colonist satisfaction
- **Resource Proximity:** Benefits from having inputs nearby

### Storage & Logistics

#### Storage System
- **Stockpiles:** Designated areas for storing specific resource types
- **Storage Buildings:** More efficient specialized storage (barns, warehouses)
- **Storage Containers:** Barrels, crates, etc. with different properties
- **Storage Conditions:** Temperature, humidity affecting preservation
- **Capacity Management:** Limited space requiring prioritization

#### Transport System
- **Manual Transport:** Colonists carrying resources
- **Animal Transport:** Beasts of burden for heavier loads
- **Infrastructure:** Roads, bridges improving transport efficiency
- **Mechanical Systems:** Potential carts, pulleys, conveyor systems
- **Automation:** Later-game mechanical or magical automation options

#### Resource Flow
- **Supply Chains:** Ensuring resources move efficiently between production stages
- **Job Priorities:** Determining which resources get moved first
- **Bottleneck Analysis:** Tools for identifying logistics problems
- **Distance Factors:** Longer paths require more time and labor

### Resource Management Interface

#### Resource Monitoring
- **Colony Inventory:** Overview of all available resources
- **Production Statistics:** Tracking input/output rates
- **Consumption Rates:** Monitoring how quickly resources are being used
- **Projections:** Estimates of future needs based on current trends

#### Planning Tools
- **Production Orders:** Setting up what to craft and in what quantities
- **Conditional Orders:** Automatic production based on inventory levels
- **Priority System:** Determining which orders get fulfilled first
- **Resource Allocation:** Reserving materials for specific projects

#### Alert System
- **Shortage Warnings:** Notifications when critical resources run low
- **Production Alerts:** Information about production problems
- **Storage Alerts:** Warnings about stockpiles nearing capacity
- **Expiration Notifications:** Alerts for perishable goods at risk

## Advanced Systems

### Economy & Trade

#### Trading System
- **Merchant Caravans:** Periodic visitors with various goods
- **Trade Relations:** Reputation affecting prices and available items
- **Export Focus:** Specializing in high-value goods for trade
- **Import Dependencies:** Relying on trade for difficult-to-produce items

#### Currency & Bartering
- **Direct Bartering:** Trading goods for goods
- **Value Metrics:** Systems for determining fair exchange rates
- **Possible Currencies:** Coins or standardized valuable items

#### Market Factors
- **Supply & Demand:** Dynamic pricing based on rarity and need
- **Faction Relations:** Affecting trade availability and terms
- **Distance Factors:** Rarer goods from more distant trading partners
- **Seasonal Variations:** Different goods available at different times

### Sustainability & Ecology

#### Resource Regeneration
- **Renewable Resources:** Trees regrow, animals repopulate
- **Regeneration Rates:** Different speeds for different resource types
- **Depletion Risk:** Over-exploitation leading to permanent loss
- **Cultivation vs. Extraction:** Strategic choices for long-term planning

#### Environmental Impact
- **Pollution:** Production processes affecting surroundings
- **Erosion & Degradation:** Terrain changes from resource extraction
- **Ecosystem Balance:** Wildlife and plant life responding to colony actions
- **Climate Effects:** Potential local climate changes from deforestation, etc.

### Optimization & Efficiency

#### Production Efficiency
- **Specialization Benefits:** Dedicated production facilities and workers
- **Scale Advantages:** Larger operations producing more efficiently
- **Research Benefits:** Technological improvements to production methods
- **Expert Bonuses:** Master craftspeople creating more with less

#### Resource Conservation
- **Recycling Systems:** Reclaiming materials from used items
- **By-product Utilization:** Finding uses for production waste
- **Efficiency Technologies:** Research to reduce resource consumption
- **Alternative Materials:** Substitutions when primary resources are scarce

#### Crisis Management
- **Emergency Reserves:** Stockpiling critical resources for hard times
- **Rationing Systems:** Controlling consumption during shortages
- **Alternative Production:** Backup methods requiring different inputs
- **Triage Protocols:** Deciding what to save when not everything can be

## Technical Implementation Considerations

### Data Structures
- **Resource Database:** Efficient storage and querying of inventory data
- **Production Recipe System:** Modular definition of crafting requirements
- **Quality Calculation Models:** Algorithms for determining output quality
- **Pathfinding Integration:** For optimal resource movement routes

### Simulation Systems
- **Production Tick Rate:** How frequently production processes update
- **Resource Flow Simulation:** Modeling movement through the colony
- **Decay & Spoilage:** Time-based deterioration of certain resources
- **Supply/Demand Modeling:** For internal economy and trading system

### Performance Optimization
- **Batch Processing:** Updating similar resources together
- **Caching Resource Paths:** Remembering optimal routes for repeated tasks
- **Simplified Distant Simulation:** Less detailed modeling for far-away processes
- **Update Prioritization:** More frequent updates for critical resources

## Design Goals & Principles

### Meaningful Complexity
Complex enough to create interesting decisions but not overwhelming or busywork.

### Visible Systems
Resource flows and production chains should be observable and understandable.

### Strategic Depth
Player choices about what to produce and how to allocate resources should have significant impact.

### Emergent Problem-Solving
Resource constraints should create interesting problems with multiple possible solutions.

## References & Inspiration

- **Dwarf Fortress:** Complex production chains and resource variety
- **Factorio:** Visible resource flows and production optimization
- **RimWorld:** Resource prioritization and crisis management
- **Minecraft:** Different material properties and crafting progression
- **Anno Series:** Production chain management and efficiency balancing

## Development Milestones

### Phase 1: Basic Resources
- Core resource types implementation
- Simple gathering mechanics
- Basic stockpile system

### Phase 2: Production Chains
- Workshop implementation
- Multi-stage crafting recipes
- Quality variation system

### Phase 3: Advanced Economy
- Trading system
- Complex resource properties
- Storage specialization

### Phase 4: Complete Integration
- Full ecological impacts
- Economic fluctuations
- Advanced logistics solutions