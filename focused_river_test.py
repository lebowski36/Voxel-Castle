#!/usr/bin/env python3
"""
Focused River Visualization Test
Creates a high-resolution visualization of a smaller area to properly show rivers
"""

import sys
import os
sys.path.append('scripts')

import numpy as np
import matplotlib.pyplot as plt
from cpp_wrapper import worldgen_cpp
import colorsys

def create_focused_river_map():
    """Create a high-resolution river map of a small area to see river details"""
    
    # Test parameters - much smaller area, higher resolution
    area_size = 10000  # 10km area instead of 100km
    resolution = 0.5   # Higher resolution: 0.5 samples per world unit = 2m per pixel
    seed = 12345
    
    # Calculate sampling parameters
    sample_width = int(area_size * resolution)
    sample_height = int(area_size * resolution)
    total_samples = sample_width * sample_height
    
    print(f"=== FOCUSED RIVER ANALYSIS ===")
    print(f"Area: {area_size/1000:.1f}km × {area_size/1000:.1f}km")
    print(f"Resolution: {resolution} samples per world unit ({1/resolution:.1f}m per pixel)")
    print(f"Sampling: {sample_width}x{sample_height} = {total_samples:,} samples")
    print(f"World coordinates: (-{area_size//2}, -{area_size//2}) to ({area_size//2}, {area_size//2})")
    print()
    
    # Create coordinate arrays
    x_coords = np.linspace(-area_size//2, area_size//2, sample_width, dtype=np.float32)
    z_coords = np.linspace(-area_size//2, area_size//2, sample_height, dtype=np.float32)
    X, Z = np.meshgrid(x_coords, z_coords, indexing='ij')
    
    # Flatten for C++ processing
    x_flat = X.flatten()
    z_flat = Z.flatten()
    
    print("Generating heightmap...")
    heightmap_flat = worldgen_cpp.generate_terrain_heightmap(x_flat, z_flat, seed)
    heightmap = heightmap_flat.reshape((sample_width, sample_height))
    
    print("Generating heightmap with rivers...")
    heightmap_rivers_flat = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, z_flat, seed)
    heightmap_rivers = heightmap_rivers_flat.reshape((sample_width, sample_height))
    
    print("Generating river flow data...")
    # For rivers, we need elevation and precipitation data for the old API
    elevations = heightmap_flat
    precipitations = np.full_like(elevations, 800.0)  # Assume 800mm precipitation
    
    river_flow_flat = worldgen_cpp.generate_river_flow(x_flat, z_flat, elevations, precipitations, seed)
    river_flow = river_flow_flat.reshape((sample_width, sample_height))
    
    print("Analyzing river data...")
    
    # Analysis
    river_threshold = 30.0
    river_mask = river_flow > river_threshold
    river_count = np.sum(river_mask)
    river_percentage = (river_count / total_samples) * 100
    
    # River carving analysis
    carving_difference = heightmap - heightmap_rivers
    max_carving = np.max(carving_difference)
    mean_carving = np.mean(carving_difference[carving_difference > 0.1])
    
    print(f"River Statistics:")
    print(f"  Points with rivers (>{river_threshold}): {river_count:,} ({river_percentage:.1f}%)")
    print(f"  Max river strength: {np.max(river_flow):.1f}")
    print(f"  Max river carving depth: {max_carving:.1f}m")
    print(f"  Mean carving depth: {mean_carving:.1f}m")
    print()
    
    # Sample some river values at different locations
    print("Sample river values:")
    for i in range(0, len(river_flow_flat), len(river_flow_flat)//10):
        x_world = x_flat[i]
        z_world = z_flat[i]
        river_val = river_flow_flat[i]
        if river_val > 10:  # Only show notable values
            print(f"  ({x_world:6.0f}, {z_world:6.0f}): river_flow={river_val:.1f}")
    
    # Create visualization
    fig, axes = plt.subplots(2, 2, figsize=(16, 16))
    fig.suptitle(f'Focused River Analysis - {area_size/1000:.1f}km Area (Seed: {seed})', fontsize=16)
    
    # Heightmap
    im1 = axes[0,0].imshow(heightmap.T, origin='lower', cmap='terrain', 
                          extent=[-area_size//2, area_size//2, -area_size//2, area_size//2])
    axes[0,0].set_title('Base Heightmap')
    axes[0,0].set_xlabel('World X (meters)')
    axes[0,0].set_ylabel('World Z (meters)')
    plt.colorbar(im1, ax=axes[0,0], label='Elevation (m)')
    
    # Heightmap with river carving
    im2 = axes[0,1].imshow(heightmap_rivers.T, origin='lower', cmap='terrain',
                          extent=[-area_size//2, area_size//2, -area_size//2, area_size//2])
    axes[0,1].set_title('Heightmap with River Carving')
    axes[0,1].set_xlabel('World X (meters)')
    axes[0,1].set_ylabel('World Z (meters)')
    plt.colorbar(im2, ax=axes[0,1], label='Elevation (m)')
    
    # River flow visualization
    # Use log scale for better river visibility
    river_flow_log = np.log10(river_flow + 1)  # +1 to avoid log(0)
    im3 = axes[1,0].imshow(river_flow_log.T, origin='lower', cmap='Blues',
                          extent=[-area_size//2, area_size//2, -area_size//2, area_size//2])
    axes[1,0].set_title('River Flow (Log Scale)')
    axes[1,0].set_xlabel('World X (meters)')
    axes[1,0].set_ylabel('World Z (meters)')
    plt.colorbar(im3, ax=axes[1,0], label='Log10(Flow + 1)')
    
    # River carving depth
    carving_display = np.where(carving_difference > 0.1, carving_difference, np.nan)
    im4 = axes[1,1].imshow(carving_display.T, origin='lower', cmap='Reds',
                          extent=[-area_size//2, area_size//2, -area_size//2, area_size//2])
    axes[1,1].set_title('River Carving Depth')
    axes[1,1].set_xlabel('World X (meters)')
    axes[1,1].set_ylabel('World Z (meters)')
    plt.colorbar(im4, ax=axes[1,1], label='Carving Depth (m)')
    
    # Add grid lines to show scale
    for ax in axes.flat:
        ax.grid(True, alpha=0.3)
        # Add 1km grid lines
        for km in range(-area_size//2000, area_size//2000 + 1):
            if km != 0:
                ax.axvline(km * 1000, color='white', alpha=0.5, linewidth=0.5)
                ax.axhline(km * 1000, color='white', alpha=0.5, linewidth=0.5)
    
    plt.tight_layout()
    
    output_filename = f'focused_river_analysis_{area_size//1000}km.png'
    plt.savefig(output_filename, dpi=150, bbox_inches='tight')
    print(f"Saved visualization: {output_filename}")
    
    return river_flow, heightmap, heightmap_rivers

if __name__ == "__main__":
    try:
        river_flow, heightmap, heightmap_rivers = create_focused_river_map()
        print("Focused river analysis complete!")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
