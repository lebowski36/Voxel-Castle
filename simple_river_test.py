#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import worldgen_cpp
from elevation_colormap import apply_standardized_elevation_coloring

def simple_river_test():
    """Very simple test with minimal area to find and visualize rivers"""
    print("=== Simple River Test ===")
    seed = 12345
    
    # Use 2km area with DYNAMIC resolution calculated by C++ engine
    view_size = 2000  # 2km x 2km (good size for rivers)
    samples = worldgen_cpp.get_optimal_sample_count(view_size, "rivers")  # Dynamic resolution from C++ engine
    resolution = samples / view_size
    
    print(f"Testing {view_size}m x {view_size}m area with {samples}x{samples} samples ({samples*samples:,} total)")
    print(f"Resolution: {view_size/samples:.1f}m per pixel (dynamically calculated)")
    
    # Center the view well within a single region (25km = 25000m regions)
    # Place center at (5000, 5000) to be well within the (0,0) region
    center_x, center_z = 5000.0, 5000.0
    
    # Create coordinate arrays
    half_size = view_size / 2
    x_coords = np.linspace(center_x - half_size, center_x + half_size, samples)
    z_coords = np.linspace(center_z - half_size, center_z + half_size, samples)
    x_grid, z_grid = np.meshgrid(x_coords, z_coords)
    
    x_flat = x_grid.flatten()
    z_flat = z_grid.flatten()
    
    print("Generating terrain...")
    # Generate terrain data (this should be fast for 1km area)
    regular_terrain = worldgen_cpp.generate_terrain_heightmap(x_flat, z_flat, seed)
    carved_terrain = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, z_flat, seed)
    river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_flat, z_flat, seed)
    
    # Reshape to grids
    regular_grid = regular_terrain.reshape(samples, samples)
    carved_grid = carved_terrain.reshape(samples, samples)
    river_grid = river_flow.reshape(samples, samples)
    
    # Calculate results
    carving_depth = regular_grid - carved_grid
    
    # Detailed river analysis
    print(f"River data statistics:")
    print(f"  Min river value: {np.min(river_grid):.3f}")
    print(f"  Max river value: {np.max(river_grid):.3f}")
    print(f"  Mean river value: {np.mean(river_grid):.3f}")
    print(f"  Values > 0: {np.sum(river_grid > 0):,}")
    print(f"  Values > 0.1: {np.sum(river_grid > 0.1):,}")
    print(f"  Values > 1.0: {np.sum(river_grid > 1.0):,}")
    
    river_threshold = 0.1  # Lower threshold to catch any rivers
    rivers_mask = river_grid >= river_threshold
    river_count = np.sum(rivers_mask)
    
    print(f"\nResults:")
    print(f"  Rivers found: {river_count} points")
    print(f"  River coverage: {river_count/(samples*samples)*100:.2f}%")
    print(f"  Max carving: {np.max(carving_depth):.2f}m")
    print(f"  Terrain range: {np.min(carved_grid):.1f}m to {np.max(carved_grid):.1f}m")
    
    # Simple visualization
    plt.figure(figsize=(12, 8))
    
    # Terrain with rivers overlay
    plt.subplot(2, 2, 1)
    apply_standardized_elevation_coloring(carved_grid, plt.gca(), 'Terrain with Rivers')
    if river_count > 0:
        river_masked = np.ma.masked_where(river_grid < river_threshold, river_grid)
        plt.imshow(river_masked, cmap='Blues', origin='lower', alpha=0.8)
    plt.title(f'Terrain + Rivers ({river_count} river points)')
    
    # Carving depth
    plt.subplot(2, 2, 2)
    plt.imshow(carving_depth, cmap='Blues', origin='lower')
    plt.title('River Carving Depth')
    plt.colorbar(label='Depth (m)')
    
    # River strength
    plt.subplot(2, 2, 3)
    plt.imshow(river_grid, cmap='Blues', origin='lower')
    plt.title('River Strength')
    plt.colorbar(label='Flow Strength')
    
    # Cross-section
    plt.subplot(2, 2, 4)
    mid_row = samples // 2
    distance = x_coords - center_x
    regular_line = regular_grid[mid_row, :]
    carved_line = carved_grid[mid_row, :]
    
    plt.plot(distance, regular_line, 'b-', alpha=0.7, label='Regular')
    plt.plot(distance, carved_line, 'g-', linewidth=2, label='Carved')
    plt.fill_between(distance, regular_line, carved_line, alpha=0.3, color='blue')
    plt.xlabel('Distance (m)')
    plt.ylabel('Elevation (m)')
    plt.title('Cross-Section')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(f'simple_river_test_{view_size}m.png', dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"Saved: simple_river_test_{view_size}m.png")
    
    # If no rivers found, suggest searching other areas
    if river_count == 0:
        print("\nNo rivers found in this area. Try running with different coordinates.")
        print("You can modify center_x and center_z in the script to search other areas.")

if __name__ == "__main__":
    simple_river_test()
