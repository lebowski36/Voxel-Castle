#!/usr/bin/env python3
"""
Find guaranteed river locations by directly accessing the river network data.
This script will scan the generated river networks and identify small areas
that are guaranteed to contain rivers for high-resolution visualization.
"""

import sys
import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import random

# Add the cpp_wrapper to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'scripts', 'cpp_wrapper'))

try:
    import worldgen_cpp
    print("✓ Successfully imported worldgen_cpp module")
except ImportError as e:
    print(f"✗ Failed to import worldgen_cpp: {e}")
    print("Make sure the C++ extension is built. Run:")
    print("cd scripts/cpp_wrapper && python setup.py build_ext --inplace")
    sys.exit(1)

# Import the standardized elevation colormap
from elevation_colormap import get_elevation_colormap

def scan_for_river_locations(seed=12345, scan_regions=9, area_size=512):
    """
    Scan multiple regions to find river locations by directly checking
    the generated river network data.
    
    Args:
        seed: Random seed for terrain generation
        scan_regions: Number of regions to scan (9 = 3x3 grid)
        area_size: Size of each scan area in meters
    
    Returns:
        List of (x, z, river_count) tuples for areas with rivers
    """
    print(f"Scanning {scan_regions} regions for river locations...")
    
    river_locations = []
    region_size = 2048  # Size of each region to scan
    
    # Scan a grid of regions centered around origin
    grid_size = int(np.sqrt(scan_regions))
    start_offset = -(grid_size // 2) * region_size
    
    for i in range(grid_size):
        for j in range(grid_size):
            center_x = start_offset + i * region_size
            center_z = start_offset + j * region_size
            
            print(f"Scanning region {i*grid_size + j + 1}/{scan_regions}: center ({center_x}, {center_z})")
            
            # Sample multiple points in this region to find rivers
            sample_points = 32  # 32x32 grid within each region
            step = region_size // sample_points
            
            river_count = 0
            sample_locations = []
            
            for x_idx in range(sample_points):
                for z_idx in range(sample_points):
                    world_x = center_x - region_size//2 + x_idx * step
                    world_z = center_z - region_size//2 + z_idx * step
                    
                    # Query river at this point
                    try:
                        river_data = worldgen_cpp.query_river_at_point(world_x, world_z, seed)
                        if river_data['has_river']:
                            river_count += 1
                            sample_locations.append((world_x, world_z, river_data['river_width']))
                    except Exception as e:
                        # Skip points that cause errors
                        continue
            
            print(f"  Found {river_count} river points in region ({center_x}, {center_z})")
            
            if river_count > 0:
                # Pick a river location from this region
                location = random.choice(sample_locations)
                river_locations.append((location[0], location[1], river_count, location[2]))
    
    return river_locations
    
    # Generate terrain and climate data first
    print("📊 Generating terrain elevation data...")
    elevations = worldgen_cpp.generate_terrain_heightmap(X_flat, Z_flat, seed)
    
    print("🌡️ Generating climate data...")
    temperatures = worldgen_cpp.generate_climate_temperature(X_flat, Z_flat, elevations, seed)
    precipitations = worldgen_cpp.generate_climate_precipitation(X_flat, Z_flat, elevations, seed)
    
    # Generate river flow data
    print("🌊 Analyzing river flow patterns...")
    river_flows = worldgen_cpp.generate_comprehensive_river_flow(X_flat, Z_flat, seed)
    
    # Find locations with significant rivers
    river_locations = []
    
    # Different thresholds for different river types
    thresholds = {
        'major_river': 500.0,      # Major rivers (very visible)
        'regional_river': 150.0,   # Regional rivers (clearly visible)
        'local_river': 50.0,       # Local rivers (visible at close zoom)
        'stream': 20.0             # Streams (visible at very close zoom)
    }
    
    for i, (x, z, flow, elev) in enumerate(zip(X_flat, Z_flat, river_flows, elevations)):
        if flow > thresholds['stream']:
            # Determine river type
            river_type = 'stream'
            if flow > thresholds['local_river']:
                river_type = 'local_river'
            if flow > thresholds['regional_river']:
                river_type = 'regional_river'
            if flow > thresholds['major_river']:
                river_type = 'major_river'
            
            # Convert back to kilometers for display
            x_km = x * 0.25 / 1000
            z_km = z * 0.25 / 1000
            
            river_locations.append({
                'x': x,
                'z': z,
                'x_km': x_km,
                'z_km': z_km,
                'flow_strength': flow,
                'elevation': elev,
                'river_type': river_type
            })
    
    # Sort by flow strength (strongest rivers first)
    river_locations.sort(key=lambda r: r['flow_strength'], reverse=True)
    
    print(f"🌊 Found {len(river_locations)} river locations:")
    for i, river in enumerate(river_locations[:10]):  # Show top 10
        print(f"  {i+1}. {river['river_type'].replace('_', ' ').title()}: "
              f"({river['x_km']:.1f}km, {river['z_km']:.1f}km) "
              f"flow={river['flow_strength']:.1f} elev={river['elevation']:.1f}m")
    
    return river_locations

def suggest_preview_coordinates(seed, target_river_types=['regional_river', 'major_river']):
    """
    Suggest good coordinates and zoom levels for world preview centered on rivers.
    
    Args:
        seed: World seed to analyze
        target_river_types: Types of rivers to look for
    
    Returns:
        Dictionary with suggested preview parameters
    """
    print(f"🎯 Finding preview coordinates for seed {seed}...")
    
    # Scan multiple regions to find good rivers
    scan_centers = [
        (0, 0),           # Origin
        (50000, 0),       # 50km east
        (0, 50000),       # 50km north
        (-50000, 0),      # 50km west
        (0, -50000),      # 50km south
        (50000, 50000),   # Northeast
        (-50000, -50000), # Southwest
    ]
    
    all_rivers = []
    
    for center_x, center_z in scan_centers:
        rivers = scan_for_rivers(seed, center_x, center_z, scan_radius_km=30, resolution=100)
        all_rivers.extend(rivers)
    
    # Filter for target river types
    target_rivers = [r for r in all_rivers if r['river_type'] in target_river_types]
    
    if not target_rivers:
        print("⚠️ No target rivers found, using any rivers available...")
        target_rivers = all_rivers
    
    if not target_rivers:
        print("❌ No rivers found in scanned area!")
        return None
    
    # Sort by flow strength and pick the best one
    target_rivers.sort(key=lambda r: r['flow_strength'], reverse=True)
    best_river = target_rivers[0]
    
    # Determine appropriate zoom level based on river type
    zoom_suggestions = {
        'major_river': {'area_km': 100, 'resolution': 800},      # 100km x 100km at high res
        'regional_river': {'area_km': 50, 'resolution': 600},    # 50km x 50km
        'local_river': {'area_km': 20, 'resolution': 400},       # 20km x 20km
        'stream': {'area_km': 5, 'resolution': 200}              # 5km x 5km
    }
    
    zoom = zoom_suggestions.get(best_river['river_type'], zoom_suggestions['regional_river'])
    
    suggestion = {
        'seed': seed,
        'center_x_km': best_river['x_km'],
        'center_z_km': best_river['z_km'],
        'center_x_world': best_river['x'],
        'center_z_world': best_river['z'],
        'area_km': zoom['area_km'],
        'resolution': zoom['resolution'],
        'river_type': best_river['river_type'],
        'flow_strength': best_river['flow_strength'],
        'elevation': best_river['elevation']
    }
    
    print(f"\n🎯 BEST PREVIEW SUGGESTION:")
    print(f"   Seed: {seed}")
    print(f"   Center: ({suggestion['center_x_km']:.1f}km, {suggestion['center_z_km']:.1f}km)")
    print(f"   World Coords: ({suggestion['center_x_world']:.0f}, {suggestion['center_z_world']:.0f})")
    print(f"   Area: {suggestion['area_km']}km x {suggestion['area_km']}km")
    print(f"   Resolution: {suggestion['resolution']}x{suggestion['resolution']}")
    print(f"   River Type: {suggestion['river_type'].replace('_', ' ').title()}")
    print(f"   Flow Strength: {suggestion['flow_strength']:.1f}")
    print(f"   Elevation: {suggestion['elevation']:.1f}m")
    
    return suggestion

def generate_preview_with_rivers(suggestion):
    """
    Generate a world preview using the suggested coordinates with rivers visible.
    """
    if not suggestion:
        print("❌ No suggestion provided")
        return
    
    print(f"\n🖼️ Generating preview with rivers for seed {suggestion['seed']}...")
    
    # Calculate coordinate ranges
    half_area_km = suggestion['area_km'] / 2
    center_x_km = suggestion['center_x_km']
    center_z_km = suggestion['center_z_km']
    
    # Convert to world coordinates
    x_min_world = (center_x_km - half_area_km) * 1000 / 0.25
    x_max_world = (center_x_km + half_area_km) * 1000 / 0.25
    z_min_world = (center_z_km - half_area_km) * 1000 / 0.25
    z_max_world = (center_z_km + half_area_km) * 1000 / 0.25
    
    # Create coordinate grids
    resolution = suggestion['resolution']
    x_coords = np.linspace(x_min_world, x_max_world, resolution)
    z_coords = np.linspace(z_min_world, z_max_world, resolution)
    X, Z = np.meshgrid(x_coords, z_coords)
    
    # Generate terrain with river carving
    print("🏔️ Generating terrain with river carving...")
    heightmap = worldgen_cpp.generate_terrain_heightmap_with_rivers(
        X.flatten(), Z.flatten(), suggestion['seed']
    ).reshape((resolution, resolution))
    
    # Generate river flow data
    print("🌊 Generating river overlay...")
    river_flow = worldgen_cpp.generate_comprehensive_river_flow(
        X.flatten(), Z.flatten(), suggestion['seed']
    ).reshape((resolution, resolution))
    
    # Create visualization
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(18, 6))
    
    # Set up coordinate extent for all plots
    extent = [center_x_km - half_area_km, center_x_km + half_area_km,
              center_z_km - half_area_km, center_z_km + half_area_km]
    
    # Heightmap with realistic elevation coloring
    im1 = ax1.imshow(heightmap, origin='lower', extent=extent)
    apply_elevation_coloring(heightmap, ax1, 
                           f'Terrain Heightmap with River Carving\nSeed: {suggestion["seed"]}')
    ax1.set_xlabel('X (km)')
    ax1.set_ylabel('Z (km)')
    
    # River flow overlay on terrain
    river_mask = river_flow > 10.0  # Only show significant rivers
    im2 = ax2.imshow(heightmap, origin='lower', alpha=0.7, extent=extent)
    apply_elevation_coloring(heightmap, ax2, 
                           f'Rivers Overlay\n{suggestion["river_type"].replace("_", " ").title()} Visible')
    river_overlay = ax2.imshow(np.where(river_mask, river_flow, np.nan), 
                              cmap='Blues', origin='lower', alpha=0.8, extent=extent)
    ax2.set_xlabel('X (km)')
    ax2.set_ylabel('Z (km)')
    plt.colorbar(river_overlay, ax=ax2, label='River Flow Strength')
    
    # River flow intensity
    im3 = ax3.imshow(river_flow, cmap='Blues', origin='lower', extent=extent)
    ax3.set_title(f'River Flow Intensity\nMax Flow: {np.max(river_flow):.1f}')
    ax3.set_xlabel('X (km)')
    ax3.set_ylabel('Z (km)')
    plt.colorbar(im3, ax=ax3, label='Flow Strength')
    
    plt.tight_layout()
    
    # Save the preview
    filename = f'river_preview_seed{suggestion["seed"]}_x{suggestion["center_x_km"]:.0f}_z{suggestion["center_z_km"]:.0f}.png'
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    print(f"💾 Saved preview: {filename}")
    
    plt.show()

def main():
    """Main function to find rivers and generate preview"""
    # Test with a few different seeds
    test_seeds = [12345, 54321, 98765, 11111, 99999]
    
    print("🌊 RIVER LOCATION SCANNER for Voxel Castle")
    print("=" * 50)
    
    best_suggestions = []
    
    for seed in test_seeds:
        print(f"\n📍 Testing seed {seed}...")
        suggestion = suggest_preview_coordinates(seed)
        if suggestion:
            best_suggestions.append(suggestion)
    
    if not best_suggestions:
        print("❌ No rivers found in any tested seeds!")
        return
    
    # Pick the best suggestion (highest flow strength)
    best_suggestion = max(best_suggestions, key=lambda s: s['flow_strength'])
    
    print(f"\n🏆 GENERATING PREVIEW FOR BEST RIVER:")
    print(f"   Seed {best_suggestion['seed']} with {best_suggestion['river_type'].replace('_', ' ').title()}")
    print(f"   Flow strength: {best_suggestion['flow_strength']:.1f}")
    
    # Generate the preview
    generate_preview_with_rivers(best_suggestion)
    
    # Output coordinates for use in worldgen_visualizer.py
    print(f"\n📋 COORDINATES FOR WORLDGEN_VISUALIZER.PY:")
    print(f"   python scripts/worldgen_visualizer.py --seed {best_suggestion['seed']} "
          f"--center_x {best_suggestion['center_x_km']:.1f} --center_z {best_suggestion['center_z_km']:.1f} "
          f"--area {best_suggestion['area_km']} --resolution {best_suggestion['resolution']}")

if __name__ == "__main__":
    main()
