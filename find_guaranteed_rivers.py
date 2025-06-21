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
from elevation_colormap import create_voxel_castle_elevation_colormap, apply_standardized_elevation_coloring

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

def visualize_river_area(center_x, center_z, area_size, seed=12345, resolution=2.0):
    """
    Generate a high-resolution visualization of a specific area known to contain rivers.
    
    Args:
        center_x, center_z: Center coordinates of the area
        area_size: Size of the area to visualize (in meters)
        seed: Random seed for terrain generation
        resolution: Resolution in meters per pixel
    """
    print(f"Generating high-resolution visualization of area at ({center_x}, {center_z})")
    print(f"Area size: {area_size}m × {area_size}m, Resolution: {resolution}m/pixel")
    
    # Calculate grid dimensions
    half_size = area_size // 2
    pixels = int(area_size / resolution)
    
    print(f"Grid size: {pixels} × {pixels} pixels")
    
    # Generate coordinate arrays
    x_coords = np.linspace(center_x - half_size, center_x + half_size, pixels)
    z_coords = np.linspace(center_z - half_size, center_z + half_size, pixels)
    
    # Generate elevation and river data
    print("Generating terrain data...")
    elevations = np.zeros((pixels, pixels))
    rivers = np.zeros((pixels, pixels))
    river_widths = np.zeros((pixels, pixels))
    
    river_point_count = 0
    
    for i, world_z in enumerate(z_coords):
        if i % (pixels // 10) == 0:
            print(f"Progress: {i}/{pixels} rows ({100*i//pixels}%)")
        
        for j, world_x in enumerate(x_coords):
            # Get elevation
            elevation = worldgen_cpp.get_elevation(world_x, world_z, seed)
            elevations[i, j] = elevation
            
            # Check for rivers
            try:
                river_data = worldgen_cpp.query_river_at_point(world_x, world_z, seed)
                if river_data['has_river']:
                    rivers[i, j] = 1.0
                    river_widths[i, j] = river_data['river_width']
                    river_point_count += 1
            except:
                pass
    
    print(f"Found {river_point_count} river points in the visualization area")
    
    # Create the visualization
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))
    
    # Get the standardized elevation colormap
    elevation_cmap = create_voxel_castle_elevation_colormap()
    
    # Plot 1: Raw elevation
    apply_standardized_elevation_coloring(elevations, axes[0], f'Elevation\n({area_size}m × {area_size}m area)')
    axes[0].set_xlabel('X (meters)')
    axes[0].set_ylabel('Z (meters)')
    
    # Plot 2: Elevation with river overlay
    apply_standardized_elevation_coloring(elevations, axes[1], f'Elevation + Rivers\n({river_point_count} river points)')
    
    # Overlay rivers in bright red
    river_mask = rivers > 0
    if np.any(river_mask):
        axes[1].contour(x_coords, z_coords, rivers, levels=[0.5], colors=['red'], linewidths=2)
        
        # Also show river points as red dots
        river_x, river_z = np.meshgrid(x_coords, z_coords)
        river_points_x = river_x[river_mask]
        river_points_z = river_z[river_mask]
        axes[1].scatter(river_points_x, river_points_z, c='red', s=1, alpha=0.8)
    
    axes[1].set_xlabel('X (meters)')
    axes[1].set_ylabel('Z (meters)')
    
    # Plot 3: River widths only
    if np.any(river_mask):
        im3 = axes[2].imshow(river_widths, extent=[center_x - half_size, center_x + half_size, 
                                                 center_z - half_size, center_z + half_size], 
                            cmap='Blues', origin='lower', vmin=0, vmax=np.max(river_widths))
        axes[2].set_title(f'River Widths\n(Max: {np.max(river_widths):.1f}m)')
        plt.colorbar(im3, ax=axes[2], label='River Width (m)')
    else:
        axes[2].text(0.5, 0.5, 'No Rivers\nFound', ha='center', va='center', 
                    transform=axes[2].transAxes, fontsize=16)
        axes[2].set_title('River Widths')
    
    axes[2].set_xlabel('X (meters)')
    axes[2].set_ylabel('Z (meters)')
    
    plt.tight_layout()
    
    # Save the visualization
    filename = f'river_area_visualization_x{center_x}_z{center_z}_{area_size}m.png'
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    print(f"✓ Saved visualization: {filename}")
    
    return river_point_count > 0

def main():
    """Main function to find and visualize river locations."""
    seed = 12345
    
    print("=== River Location Scanner ===")
    print(f"Using seed: {seed}")
    
    # Step 1: Scan for river locations
    print("\n1. Scanning for river locations...")
    river_locations = scan_for_river_locations(seed=seed, scan_regions=9, area_size=512)
    
    if not river_locations:
        print("✗ No river locations found! This suggests an issue with river generation or detection.")
        return
    
    print(f"\n✓ Found {len(river_locations)} regions with rivers:")
    for i, (x, z, count, width) in enumerate(river_locations):
        print(f"  {i+1}. Location ({x:5.0f}, {z:5.0f}): {count:3d} rivers, max width {width:.1f}m")
    
    # Step 2: Visualize the best river location
    print("\n2. Visualizing the area with most rivers...")
    
    # Pick the location with the most rivers
    best_location = max(river_locations, key=lambda x: x[2])
    center_x, center_z, river_count, max_width = best_location
    
    print(f"Selected location: ({center_x}, {center_z}) with {river_count} river points")
    
    # Generate visualization at multiple scales
    for area_size, resolution in [(512, 2.0), (256, 1.0), (128, 0.5)]:
        print(f"\nGenerating {area_size}m × {area_size}m visualization at {resolution}m/pixel...")
        success = visualize_river_area(center_x, center_z, area_size, seed, resolution)
        
        if success:
            print(f"✓ Successfully visualized rivers at {area_size}m scale")
        else:
            print(f"✗ No rivers found in {area_size}m area (may need larger search area)")
    
    print(f"\n=== Complete ===")
    print(f"Found and visualized river area at ({center_x}, {center_z})")
    print("Check the generated PNG files for detailed river visualizations.")

if __name__ == "__main__":
    main()
