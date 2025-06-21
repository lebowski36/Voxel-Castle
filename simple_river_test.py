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
    
    # Use smaller area with reasonable resolution for testing
    view_size = 500   # 500m x 500m area (much smaller)
    samples = 50      # 50x50 = 2.5k samples (very manageable)
    resolution = samples / view_size
    
    print(f"Testing {view_size}m x {view_size}m area with {samples}x{samples} samples ({samples*samples:,} total)")
    print(f"Resolution: {view_size/samples:.1f}m per pixel")
    
    # Test area where rivers were actually generated (from tracing output)
    # Pick a specific river source location we saw in the output
    # For example: "Tracing river from (-24750, -24750) elevation: 105.873m"
    center_x, center_z = -24750.0, -24750.0
    
    # Create coordinate arrays
    half_size = view_size / 2
    x_coords = np.linspace(center_x - half_size, center_x + half_size, samples)
    z_coords = np.linspace(center_z - half_size, center_z + half_size, samples)
    x_grid, z_grid = np.meshgrid(x_coords, z_coords)
    
    print(f"Query area: X=[{x_coords[0]:.1f}, {x_coords[-1]:.1f}], Z=[{z_coords[0]:.1f}, {z_coords[-1]:.1f}]")
    
    # Show which region this corresponds to (assuming 25km regions)
    region_size = 25000  # 25km regions as mentioned in output
    min_region_x = int(np.floor(x_coords[0] / region_size))
    max_region_x = int(np.floor(x_coords[-1] / region_size))
    min_region_z = int(np.floor(z_coords[0] / region_size))
    max_region_z = int(np.floor(z_coords[-1] / region_size))
    print(f"Querying regions: X=[{min_region_x}, {max_region_x}], Z=[{min_region_z}, {max_region_z}]")
    
    print(f"Query region: X=({x_coords[0]:.1f} to {x_coords[-1]:.1f}), Z=({z_coords[0]:.1f} to {z_coords[-1]:.1f})")
    
    x_flat = x_grid.flatten()
    z_flat = z_grid.flatten()
    
    print("Generating terrain...")
    sys.stdout.flush()  # Ensure this message appears before C++ output
    # Generate terrain data (this should be fast for 1km area)
    regular_terrain = worldgen_cpp.generate_terrain_heightmap(x_flat, z_flat, seed)
    
    print("Generating carved terrain with rivers...")
    sys.stdout.flush()  # Ensure this message appears before C++ output
    carved_terrain = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, z_flat, seed)
    
    print("Querying cached river network...")
    print(f"About to query {len(x_flat):,} points for river data...")
    sys.stdout.flush()  # Ensure this message appears before C++ output
    
    # Query river network using the cached data (batch function with progress bar)
    river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_flat, z_flat, seed)
    print("River querying complete!")
    sys.stdout.flush()
    
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
    plt.figure(figsize=(15, 10))
    
    # Terrain with rivers overlay - use local elevation range for better contrast
    plt.subplot(2, 3, 1)
    # Use local terrain range instead of global range for better visualization
    terrain_min, terrain_max = np.min(carved_grid), np.max(carved_grid)
    im1 = plt.imshow(carved_grid, cmap='terrain', origin='lower', vmin=terrain_min, vmax=terrain_max)
    plt.colorbar(im1, label='Elevation (m)')
    if river_count > 0:
        river_masked = np.ma.masked_where(river_grid < river_threshold, river_grid)
        plt.imshow(river_masked, cmap='Blues', origin='lower', alpha=0.8, vmin=river_threshold, vmax=np.max(river_grid))
    plt.title(f'Terrain + Rivers ({river_count} river points)')
    plt.xlabel('Distance (10m per pixel)')
    plt.ylabel('Distance (10m per pixel)')

    # Regular terrain (no carving)
    plt.subplot(2, 3, 2)
    regular_min, regular_max = np.min(regular_grid), np.max(regular_grid)
    im_reg = plt.imshow(regular_grid, cmap='terrain', origin='lower', vmin=regular_min, vmax=regular_max)
    plt.colorbar(im_reg, label='Elevation (m)')
    plt.title('Original Terrain (No Rivers)')
    plt.xlabel('Distance (10m per pixel)')
    plt.ylabel('Distance (10m per pixel)')
    
    # Carving depth
    plt.subplot(2, 3, 3)
    plt.imshow(carving_depth, cmap='Blues', origin='lower')
    plt.title('River Carving Depth')
    plt.colorbar(label='Depth (m)')
    plt.xlabel('Distance (10m per pixel)')
    plt.ylabel('Distance (10m per pixel)')
    
    # River strength
    plt.subplot(2, 3, 4)
    if river_count > 0:
        plt.imshow(river_grid, cmap='viridis', origin='lower', vmin=0, vmax=np.max(river_grid))
    else:
        plt.imshow(river_grid, cmap='viridis', origin='lower')
    plt.title('River Flow Strength')
    plt.colorbar(label='Flow Strength')
    plt.xlabel('Distance (10m per pixel)')
    plt.ylabel('Distance (10m per pixel)')
    
    # River network overlay (binary)
    plt.subplot(2, 3, 5)
    river_binary = (river_grid > river_threshold).astype(float)
    plt.imshow(river_binary, cmap='Blues_r', origin='lower')
    plt.title('River Network (Binary)')
    plt.colorbar(label='River (1) / Land (0)')
    plt.xlabel('Distance (10m per pixel)')
    plt.ylabel('Distance (10m per pixel)')
    
    # Cross-section
    plt.subplot(2, 3, 6)
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
