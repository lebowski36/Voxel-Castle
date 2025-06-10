#!/usr/bin/env python3
"""
Tectonic Elevation Scaling Calculator
=====================================

Purpose: Calculate proper scaling values for tectonic elevation modifiers to ensure
terrain stays within the ±2048m world boundaries while providing dramatic geological features.

World Constraints:
- Total world height: 4096m (from -2048m to +2048m)
- Voxel size: 0.25m × 0.25m × 0.25m
- Target: Use 80% of range for safety margin = ±1638m for tectonic features
"""

import math

# World constraints
WORLD_HEIGHT_RANGE = 4096  # meters (-2048 to +2048)
WORLD_MIN = -2048
WORLD_MAX = +2048
SAFETY_MARGIN = 0.8  # Use 80% of range to leave room for base terrain + other effects

# Tectonic target range (with safety margin)
TECTONIC_MAX = WORLD_MAX * SAFETY_MARGIN  # +1638m
TECTONIC_MIN = WORLD_MIN * SAFETY_MARGIN  # -1638m
TECTONIC_RANGE = TECTONIC_MAX - TECTONIC_MIN  # 3276m total

print("=== WORLD CONSTRAINTS ===")
print(f"World height range: {WORLD_MIN}m to {WORLD_MAX}m ({WORLD_HEIGHT_RANGE}m total)")
print(f"Safety margin: {SAFETY_MARGIN*100}%")
print(f"Tectonic target range: {TECTONIC_MIN}m to {TECTONIC_MAX}m ({TECTONIC_RANGE}m total)")
print()

# Current tectonic simulation parameters
MAX_STRESS = 1.0  # From boundary.stress clamping in CalculateStressAccumulation
MAX_INFLUENCE = 1.0  # influence starts at 1.0 and decreases with distance
MAX_LOCAL_STRESS = MAX_STRESS * MAX_INFLUENCE  # localStress = boundary.stress * influence

print("=== CURRENT SIMULATION PARAMETERS ===")
print(f"Maximum boundary stress: {MAX_STRESS}")
print(f"Maximum influence: {MAX_INFLUENCE}")
print(f"Maximum localStress: {MAX_LOCAL_STRESS}")
print()

# Calculate scaling factors for each boundary type
print("=== BOUNDARY TYPE SCALING CALCULATIONS ===")

# CONVERGENT BOUNDARIES (Mountain ranges)
# Goal: localStress=1.0 should produce +1638m elevation
convergent_scale = TECTONIC_MAX / MAX_LOCAL_STRESS
print(f"CONVERGENT (Mountains):")
print(f"  Target: 0 to +{TECTONIC_MAX}m")
print(f"  Formula: localStress * {convergent_scale:.1f}")
print(f"  Test: localStress=1.0 → {1.0 * convergent_scale:.1f}m")
print(f"  Test: localStress=0.5 → {0.5 * convergent_scale:.1f}m")
print()

# DIVERGENT BOUNDARIES (Ocean trenches/rifts)
# Goal: localStress=1.0 should produce -1638m elevation
divergent_scale = abs(TECTONIC_MIN) / MAX_LOCAL_STRESS
print(f"DIVERGENT (Rifts/Trenches):")
print(f"  Target: 0 to {TECTONIC_MIN}m")
print(f"  Formula: -localStress * {divergent_scale:.1f}")
print(f"  Test: localStress=1.0 → {-1.0 * divergent_scale:.1f}m")
print(f"  Test: localStress=0.5 → {-0.5 * divergent_scale:.1f}m")
print()

# TRANSFORM BOUNDARIES (Lateral movement, moderate elevation changes)
# Goal: ±400m for lateral shear effects (about 25% of mountain height)
transform_scale = (TECTONIC_MAX * 0.25) / MAX_LOCAL_STRESS
print(f"TRANSFORM (Lateral shear):")
print(f"  Target: ±{transform_scale:.1f}m")
print(f"  Formula: localStress * {transform_scale:.1f} * (random ±1)")
print(f"  Test: localStress=1.0 → ±{1.0 * transform_scale:.1f}m")
print(f"  Test: localStress=0.5 → ±{0.5 * transform_scale:.1f}m")
print()

# STABLE AREAS (Small random variation)
# Goal: ±50m for geological noise
stable_variation = 50.0
print(f"STABLE AREAS (Geological noise):")
print(f"  Target: ±{stable_variation}m")
print(f"  Formula: random(-{stable_variation}, +{stable_variation})")
print()

print("=== CODE VALUES TO IMPLEMENT ===")
print("// Mountain ranges (convergent boundaries)")
print(f"elevationMod = localStress * {convergent_scale:.1f}f;")
print()
print("// Ocean trenches/rifts (divergent boundaries)")  
print(f"elevationMod = -localStress * {divergent_scale:.1f}f;")
print()
print("// Transform faults (lateral shear)")
print(f"elevationMod = localStress * {transform_scale:.1f}f * transformDirection;")
print()
print("// Stable areas (geological noise)")
print(f"elevationMod = uniformDist(rng) * {stable_variation:.1f}f;")
print()

print("=== VERIFICATION ===")
max_mountain = 1.0 * convergent_scale
max_trench = -1.0 * divergent_scale
max_transform = 1.0 * transform_scale
max_stable = stable_variation

total_max = max_mountain + max_stable
total_min = max_trench - max_stable

print(f"Maximum possible elevation: {max_mountain:.1f}m + {max_stable:.1f}m = {total_max:.1f}m")
print(f"Minimum possible elevation: {max_trench:.1f}m - {max_stable:.1f}m = {total_min:.1f}m")

if total_max <= WORLD_MAX and total_min >= WORLD_MIN:
    print("✅ SAFE: All values within world boundaries!")
else:
    print("❌ UNSAFE: Values exceed world boundaries!")
    if total_max > WORLD_MAX:
        print(f"  ⚠️  Maximum {total_max:.1f}m exceeds world limit {WORLD_MAX}m")
    if total_min < WORLD_MIN:
        print(f"  ⚠️  Minimum {total_min:.1f}m below world limit {WORLD_MIN}m")
