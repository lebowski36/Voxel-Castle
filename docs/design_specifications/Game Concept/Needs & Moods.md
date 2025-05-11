# Needs & Moods

## Overview
The Needs & Moods system is a fundamental pillar of Voxel Fortress that drives colonist behavior, satisfaction, and productivity. This comprehensive system simulates the physical, emotional, and social requirements of your colonists, creating dynamic challenges and storytelling opportunities.

> **Related Documents:**
> - [Colony Simulation](./Colony%20Simulation.md)
> - [Skills & Professions](./Skills%20%26%20Professions.md)
> - [Main Game Concept](../Voxel%20Fortress%20Game%20Concept%20Details.md)

## Core Components

### Physical Needs

#### Hunger & Thirst
- **Hunger Levels:** From well-fed to starving with multiple stages
- **Food Quality:** Different foods satisfy hunger differently and provide mood bonuses/penalties
- **Food Variety:** Colonists prefer diverse diets; repetitive meals cause diminishing satisfaction
- **Thirst System:** Separate from hunger, requires access to clean water sources
- **Alcohol & Substances:** Optional consumables that affect mood, productivity, and social behavior

#### Rest & Energy
- **Sleep Cycle:** Colonists require regular sleep based on individual traits
- **Sleep Quality:** Determined by bed quality, room conditions, recent experiences
- **Exhaustion States:** Tiredness affects work speed, social interactions, and accident likelihood
- **Napping:** Short rest periods during the day may help prevent full exhaustion

#### Health & Safety
- **Injuries:** From minor cuts to life-threatening wounds, requiring different levels of care
- **Diseases:** Contagious and non-contagious ailments with various symptoms and treatments
- **Environmental Hazards:** Temperature extremes, radiation, toxic gases
- **Security:** Need to feel safe from external threats and internal conflicts

### Psychological Needs

#### Privacy & Personal Space
- **Bedroom Quality:** Private vs. shared, size, decoration
- **Downtime:** Need for periods away from work and social obligations
- **Personal Territory:** Space viewed as "theirs" that others shouldn't intrude upon

#### Beauty & Comfort
- **Environmental Beauty:** Visual appeal of surroundings, affected by architecture, decorations, natural features
- **Comfort:** Quality of furniture, appropriate clothing, temperature control
- **Noise Levels:** Disruptive sounds affect sleep and concentration
- **Cleanliness:** Environment free from dirt, blood, refuse

#### Recreation & Entertainment
- **Variety:** Different recreation types (intellectual, physical, social, etc.)
- **Novelty:** Entertainment facilities become less effective with repeated use
- **Skill-Based Preferences:** Smart colonists prefer intellectual activities, strong ones physical activities
- **Cultural Activities:** Concerts, exhibitions, competitions to break monotony

### Social Needs

#### Belonging & Affiliation
- **Social Integration:** Sense of being part of the colony's social fabric
- **Cultural Identity:** Connection to cultural roots and practices
- **Faction Membership:** Belonging to internal subgroups that share values/interests

#### Relationships
- **Social Interaction:** Regular communication with compatible individuals
- **Intimacy:** Close bonds with partners, family members, or close friends
- **Conflict Resolution:** Need to resolve interpersonal tensions
- **Grief Processing:** Time and support needed after losing close relations

#### Status & Recognition
- **Respect:** Acknowledgment from peers based on contributions
- **Rank & Position:** Designated role in colony hierarchy
- **Achievements:** Recognition for notable accomplishments
- **Leadership Opportunities:** For those with ambitious traits

### Spiritual & Self-Actualization

#### Meaning & Purpose
- **Meaningful Work:** Jobs aligned with personal values and interests
- **Life Goals:** Long-term aspirations unique to each colonist
- **Value Alignment:** Colony decisions that match individual ethics

#### Self-Development
- **Skill Advancement:** Opportunities to learn and grow
- **Challenge:** Appropriate difficulty level in assigned tasks
- **Creative Expression:** For artistically inclined colonists

#### Spiritual Practice
- **Religious Observance:** For spiritually-minded colonists
- **Meditation:** Quiet reflection time
- **Rituals:** Community ceremonies marking important events

## Mood System

### Mood States
- **Euphoric:** Exceptionally happy, highly productive but potentially impulsive
- **Happy:** Content and working efficiently
- **Content:** Baseline satisfaction
- **Dissatisfied:** Minor complaints, slight productivity drop
- **Unhappy:** Significant productivity impact, may break minor rules
- **Distressed:** Prone to social conflicts, work refusal
- **Breaking Point:** Risk of major mental breaks

### Mental Breaks
- **Minor:** Sad wandering, binge eating/drinking, social withdrawal
- **Major:** Tantrums, fights, destruction of property
- **Extreme:** Dangerous breaks like berserk rage, catatonia, or abandoning the colony
- **Recovery:** Time and conditions needed to stabilize after a break

### Mood Factors
- **Weighted System:** Different needs impact different colonists based on their traits
- **Temporary vs. Long-term:** Distinction between transient feelings and deeper satisfaction
- **Memory System:** Significant events create memories that affect mood over time
- **Expectations:** Higher wealth/development raises colonist expectations

## UI & Player Interaction

### Need Monitoring
- **Individual View:** Detailed breakdown of a colonist's needs and mood factors
- **Colony Overview:** Aggregate view of overall colony satisfaction and urgent issues
- **Alert System:** Warnings for colonists approaching critical need levels

### Satisfaction Management
- **Direct Intervention:** Player can prioritize certain actions to address urgent needs
- **Policy Settings:** Colony-wide directives affecting how needs are met
- **Infrastructure Planning:** Building facilities that efficiently satisfy multiple needs

### Behavioral Indicators
- **Animation Cues:** Visual indicators of colonist states (slouching when tired, etc.)
- **Social Signals:** Interactions that reveal mood and need states
- **Environmental Feedback:** Visual cues in the environment showing quality/problems

## Technical Implementation Considerations

<!-- C++ Note: The simulation of colonist needs and moods will require efficient C++ data structures for tracking numerous individual states and their modifiers. Performance will be critical, especially with a large number of colonists. Consider event-driven updates and optimized algorithms for calculating mood changes and triggering behavioral responses. -->

### Need Calculation Systems
- **Continuous vs. Discrete:** Some needs track continuously, others update at intervals
- **Need Decay Rates:** Different needs deteriorate at different speeds
- **Satisfaction Sources:** Multiple ways to satisfy the same need with varying effectiveness

### AI Decision Making
- **Need Prioritization:** How colonists decide which needs to address first
- **Path Planning:** Finding optimal routes to need satisfaction sources
- **Group Coordination:** Sharing limited resources for need fulfillment

### Performance Optimization
- **Update Frequency:** Needs calculated at appropriate intervals based on importance
- **Simplified Models:** Less critical colonists use simpler need models when far from player focus
- **Batched Processing:** Similar needs calculated together to improve performance

## Design Goals & Principles

### Comprehensible Complexity
Complex enough to create interesting dynamics but transparent enough for players to understand and address.

### Meaningful Trade-offs
No perfect solution to satisfy all needs simultaneously, requiring strategic choices.

### Individual Variation
Colonists should feel like individuals with unique need profiles rather than interchangeable units.

### Story Generation
Need-driven behaviors should create memorable narratives and unexpected situations.

## References & Inspiration

- **Dwarf Fortress:** Complex need interactions and mental break systems
-