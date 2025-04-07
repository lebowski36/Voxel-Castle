# Skills & Professions

## Overview
The Skills & Professions system forms a core component of Voxel Fortress's simulation depth, defining how colonists develop abilities, specialize in roles, and contribute to the colony's growth. This system creates meaningful progression for individual characters while giving players strategic choices in workforce management.

> **Related Documents:**
> - [Colony Simulation](./Colony%20Simulation.md)
> - [Needs & Moods](./Needs%20%26%20Moods.md)
> - [Main Game Concept](../Voxel%20Fortress%20Game%20Concept%20Details.md)

## Core Components

### Skill System

#### Skill Categories
- **Physical Skills:** Mining, construction, combat, farming, hunting, etc.
- **Crafting Skills:** Smithing, woodworking, cooking, brewing, tailoring, etc.
- **Intellectual Skills:** Medicine, research, engineering, artistic pursuits
- **Social Skills:** Negotiation, leadership, entertainment, teaching

#### Skill Progression
- **Experience-Based:** Skills improve through practice, not arbitrary point allocation
- **Learning Curve:** Initial gains come quickly, mastery requires dedicated practice
- **Passive Observation:** Limited learning by watching others perform tasks
- **Study:** Dedicated learning activities (reading books, training sessions) boost skill gain
- **Mentorship:** Skilled colonists can teach others, accelerating their learning

#### Skill Levels
- **Numeric Scale:** 0-20 scale for precise tracking of advancement
- **Named Tiers:** Novice, Amateur, Competent, Skilled, Expert, Master, Legendary
- **Specializations:** Sub-skills that unlock at higher tiers (e.g., "Blacksmith" splits into "Weaponsmith" and "Armorsmith")

#### Skill Effects
- **Quality:** Higher skills produce better quality items and work
- **Speed:** Skilled colonists work more efficiently at their specialties
- **Resource Efficiency:** Less waste of materials at higher skill levels
- **Special Techniques:** Unique abilities unlocked at expert/master levels
- **Failure Rates:** Lower chance of accidents, injuries, or botched work

### Profession System

#### Job Assignment
- **Manual Assignment:** Players directly assign colonists to jobs/roles
- **Priority Settings:** Define priorities for each colonist across job categories
- **Auto-Assignment:** Optional AI distribution of labor based on skills and priorities
- **Scheduling:** Time allocation for different activities throughout the day

#### Career Paths
- **Specialization Tracks:** Related professions forming career advancement paths
- **Certification/Title System:** Formal recognition of skill mastery (titles, ceremonies)
- **Professional Identity:** Long-term attachment to certain roles affects colonist satisfaction
- **Transitioning:** Costs/benefits of switching career paths mid-game

#### Workplaces & Tools
- **Specialized Facilities:** Different jobs require appropriate workstations
- **Tool Quality:** Better tools improve work outcomes and satisfaction
- **Ergonomics:** Workplace design affects efficiency and colonist health
- **Proximity Effects:** Benefits from related workshops being near each other

#### Job Types

##### Resource Gathering
- **Miners:** Extract stone, ore, and gems from the environment
- **Woodcutters:** Harvest trees for lumber
- **Farmers:** Cultivate crops and manage fields
- **Hunters/Trappers:** Pursue wild animals for meat, hide, and other materials
- **Foragers:** Collect wild plants, mushrooms, and other natural resources

##### Production & Crafting
- **Smiths:** Work metal into tools, weapons, armor, and components
- **Carpenters:** Create wooden structures, furniture, and items
- **Masons:** Build and carve stone structures and objects
- **Tailors:** Create clothing, bedding, and fabric items
- **Cooks:** Prepare meals with varying complexity and quality
- **Brewers:** Create beverages with morale and trade value

##### Support & Services
- **Medics:** Treat injuries and illnesses, perform surgery
- **Teachers:** Improve skill learning rates for others
- **Entertainers:** Boost colony morale through performances
- **Researchers:** Develop new technologies and techniques
- **Caretakers:** Look after children, elderly, and injured colonists

##### Administrative & Leadership
- **Managers:** Optimize workflow and resource allocation
- **Diplomats:** Handle relations with other factions and traders
- **Military Officers:** Lead and train defense forces
- **Governors:** Administer colony policies and justice

##### Hybrid Roles
- **Explorers:** Combination of combat, survival, and reconnaissance skills
- **Artificers:** Merge crafting with research for unique creations
- **Wardens:** Security and prisoner management

### Integration with Other Systems

#### Aptitude & Learning
- **Traits:** Innate characteristics that affect skill learning rates
- **Age Factors:** Peak learning periods and natural decline
- **Intelligence:** General learning ability across multiple fields
- **Interests:** Personal preferences making certain skills more enjoyable

#### Satisfaction & Mood
- **Job Satisfaction:** Matching skills and interests to assigned work
- **Variety Need:** Some colonists need job rotation to stay happy
- **Achievement:** Pride in completing difficult work or reaching skill milestones
- **Recognition:** Social benefits from being skilled in valued professions

#### Social Dynamics
- **Mentorship Relations:** Bonds formed through teaching
- **Professional Respect:** Status earned from skill mastery
- **Guild-like Groups:** Specialists forming socio-professional communities
- **Competition:** Rivalry between similarly skilled colonists

## Technical Implementation Considerations

### Data Architecture
- **Skill Trees:** Hierarchical organization of related skills and specializations
- **Experience Tracking:** Systems for measuring appropriate practice activities
- **Work Process Models:** How tasks break down into steps requiring specific skills

### AI Systems
- **Task Selection:** How colonists choose what work to do based on priorities
- **Learning Simulation:** Models for realistic skill development curves
- **Quality Determination:** Algorithms for how skill affects work outcomes

### Balancing Considerations
- **Generalists vs. Specialists:** Ensuring both approaches are viable
- **Learning Speed:** Paced to feel rewarding but not trivial
- **Skill Ceiling Effects:** Preventing single highly-skilled colonists from trivializing gameplay

## Design Goals & Principles

### Meaningful Progression
Skills should provide a sense of character development and accomplishment over time.

### Strategic Depth
Players should face interesting decisions about how to develop their workforce.

### Organic Specialization
Colonists should naturally gravitate toward roles that suit them rather than being completely interchangeable.

### Interdependence
No single profession should be able to operate fully independently; encourage interaction between specialists.

### Skill Expression
High skill levels should be visibly apparent in the results produced, not just numerical improvements.

## References & Inspiration

- **Dwarf Fortress:** Deep skill system with quality levels and moods
- **RimWorld:** Work priority system and specialized workplaces
- **The Guild 2:** Career progression and professional identity
- **Factorio:** Production chain specialization and efficiency
- **Kingdom Come: Deliverance:** Practice-based skill improvement

## Development Milestones

### Phase 1: Basic Framework
- Core skill list implementation
- Simple experience gain system
- Basic job assignment interface

### Phase 2: Profession Development
- Specialized workstations and tools
- Quality outcome system
- Initial career paths

### Phase 3: Advanced Features
- Teaching and mentorship
- Special techniques for masters
- Professional titles and recognition

### Phase 4: Complete Integration
- Full interdependence with all game systems
- Social dynamics of professional life
- Cultural impact of valued professions