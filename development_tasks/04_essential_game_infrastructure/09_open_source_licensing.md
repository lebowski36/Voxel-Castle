# Open Source Licensing Research and Decision
*Created: 2025-06-05 23:42*
*Last Updated: 2025-06-05 23:42*

## Overview
This document outlines the research and decision process for choosing an appropriate open source license for the Voxel Castle project. Given that this is a game engine/game project built using multiple third-party libraries, it's crucial to understand our licensing obligations and make an informed choice.

## Current Project Dependencies

### Major Dependencies
Based on our CMakeLists.txt and external libraries:

1. **SDL3** - Zlib License (very permissive)
2. **OpenGL** - Open standard (no licensing restrictions for usage)
3. **GLAD** - MIT License or Public Domain (depending on generation options)
4. **Flecs ECS** - MIT License
5. **GLM (OpenGL Mathematics)** - MIT License or The Happy Bunny License
6. **STB Image** - MIT License or Public Domain

### Licensing Compatibility Analysis

**Key Questions to Research:**
- [ ] What are the exact licenses of each dependency?
- [ ] Are there any copyleft requirements (GPL, LGPL)?
- [ ] What are the attribution requirements?
- [ ] Are there any restrictions on commercial use?
- [ ] What licenses are compatible with each other?

## License Options to Consider

### 1. MIT License
**Pros:**
- Very permissive, allows commercial use
- Compatible with most dependencies
- Simple and well-understood
- Minimal attribution requirements

**Cons:**
- No patent protection
- No copyleft protection (derivative works can be proprietary)

### 2. Apache License 2.0
**Pros:**
- Permissive like MIT
- Includes patent protection
- Well-suited for larger projects
- Commercial-friendly

**Cons:**
- More complex than MIT
- Longer license text

### 3. GPL v3
**Pros:**
- Strong copyleft protection
- Ensures derivatives remain open source
- Patent protection

**Cons:**
- May not be compatible with some dependencies
- Could limit commercial adoption
- Complex compliance requirements

### 4. LGPL v3
**Pros:**
- Copyleft for the library itself
- Allows linking with proprietary code
- Good for engine development

**Cons:**
- More complex than permissive licenses
- May limit some use cases

## Research Tasks

### Phase 1: Dependency License Audit
- [ ] **SDL3 License Verification**
  - [ ] Check exact license version and requirements
  - [ ] Verify if static linking affects licensing
  - [ ] Document attribution requirements

- [ ] **GLAD License Verification**
  - [ ] Confirm generation options used
  - [ ] Check if MIT or Public Domain applies
  - [ ] Document any attribution needs

- [ ] **Flecs ECS License Review**
  - [ ] Verify MIT license terms
  - [ ] Check for any additional requirements
  - [ ] Review commercial use permissions

- [ ] **GLM License Review**
  - [ ] Determine which license applies (MIT vs Happy Bunny)
  - [ ] Check compatibility requirements
  - [ ] Document attribution needs

- [ ] **STB Image License Review**
  - [ ] Verify Public Domain/MIT status
  - [ ] Check usage requirements
  - [ ] Document any attribution needs

### Phase 2: License Compatibility Matrix
- [ ] Create compatibility matrix for all dependencies
- [ ] Identify any conflicting requirements
- [ ] Document minimum attribution requirements
- [ ] Check for any copyleft contamination risks

### Phase 3: Project Goals Assessment
- [ ] **Commercial Use Intentions**
  - [ ] Determine if commercial licensing is desired
  - [ ] Consider dual-licensing options
  - [ ] Evaluate business model implications

- [ ] **Community Contributions**
  - [ ] Consider how license affects contributors
  - [ ] Evaluate CLA (Contributor License Agreement) needs
  - [ ] Plan for community involvement

- [ ] **Fork and Derivative Policies**
  - [ ] Decide on copyleft vs permissive approach
  - [ ] Consider engine vs game licensing separately
  - [ ] Plan for asset licensing (separate from code)

### Phase 4: License Implementation
- [ ] **License File Creation**
  - [ ] Create LICENSE file in project root
  - [ ] Add SPDX license identifiers to source files
  - [ ] Create NOTICE file for attributions

- [ ] **Documentation Updates**
  - [ ] Update README with license information
  - [ ] Document third-party licenses
  - [ ] Create contribution guidelines

- [ ] **Build System Integration**
  - [ ] Add license checks to build process
  - [ ] Ensure proper attribution in distributions
  - [ ] Plan for license compliance in releases

## Recommendation Framework

### Factors to Consider
1. **Simplicity** - How easy is compliance?
2. **Commercial Viability** - Does it support business goals?
3. **Community Growth** - Does it encourage contributions?
4. **Legal Protection** - Does it provide adequate protections?
5. **Compatibility** - Does it work with all dependencies?

### Decision Matrix
| License | Simplicity | Commercial | Community | Protection | Compatibility | Score |
|---------|------------|------------|-----------|------------|---------------|-------|
| MIT     | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ | ?/25 |
| Apache 2.0 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ?/25 |
| GPL v3  | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ?/25 |
| LGPL v3 | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ?/25 |

*Scoring to be completed during research phase*

## Consultation Resources

### Legal Resources
- [ ] Open Source Initiative (OSI) license documentation
- [ ] Choose A License (choosealicense.com) guidance
- [ ] GitHub licensing documentation
- [ ] Software Freedom Law Center resources

### Technical Resources
- [ ] SPDX License List for standard license identifiers
- [ ] REUSE specification for license compliance
- [ ] FOSSology for automated license scanning
- [ ] LicenseFinder for dependency license analysis

### Community Resources
- [ ] Game development community practices
- [ ] Similar open source game engines (Godot, etc.)
- [ ] Engine vs game licensing precedents
- [ ] Indie developer licensing experiences

## Timeline and Milestones

### Week 1: Research Phase
- [ ] Complete dependency license audit
- [ ] Research license options thoroughly
- [ ] Consult community resources

### Week 2: Analysis Phase
- [ ] Create compatibility matrix
- [ ] Score decision matrix
- [ ] Identify top 2-3 license candidates

### Week 3: Decision Phase
- [ ] Make final license decision
- [ ] Plan implementation strategy
- [ ] Prepare documentation

### Week 4: Implementation Phase
- [ ] Add license files to project
- [ ] Update all documentation
- [ ] Implement build system checks
- [ ] Announce licensing decision

## Action Items for User

**Immediate Questions for User:**
1. **Commercial Intentions**: Do you plan to potentially commercialize this game/engine, or is it purely for learning/portfolio purposes?

2. **Community Goals**: Do you want to encourage community contributions and forks, or prefer more control over derivatives?

3. **Business Model**: If commercial, would you consider dual licensing (open source + commercial licenses)?

4. **Risk Tolerance**: Are you comfortable with copyleft licenses that require derivatives to also be open source?

5. **Legal Resources**: Do you have access to legal counsel for license review, or should we focus on well-established, simple licenses?

**Next Steps:**
- Please provide guidance on the above questions
- I'll begin the dependency license audit
- We'll work through the research systematically
- Final decision will be made collaboratively based on your goals and the research findings

## Notes and Considerations

### Game vs Engine Licensing
- Consider licensing the engine separately from game assets
- Game assets (art, audio, levels) may need different licensing
- Engine code could be more permissive than game content

### International Considerations
- Most licenses are written for US law
- Consider international contributors and users
- EU GDPR considerations for any data collection

### Future Flexibility
- Choose licenses that allow future changes if needed
- Consider how license choice affects dependencies you might add later
- Plan for potential license upgrades (e.g., GPL v3 to v4)

**This document will be updated as research progresses and decisions are made.**
