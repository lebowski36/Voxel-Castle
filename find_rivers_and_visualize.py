#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import worldgen_cpp
from elevation_colormap import apply_standardized_elevation_coloring

def find_rivers_and_visualize():
    """Find regions with rivers and create proper heightmap + river visualization"""
    print("=== Finding Rivers and Creating Visualization ===")
    seed = 12345
    
    # Step 1: Search for regions with rivers
    print("Step 1: Searching for regions with rivers...")
    regions_to_test = [
        (-1, -1), (-1, 0), (-1, 1),
        (0, -1),  (0, 0),  (0, 1),
        (1, -1),  (1, 0),  (1, 1)
    ]
    
    region_with_rivers = None
    for region_x, region_z in regions_to_test:
        # Test a few points in this region to see if any have rivers
        center_x = region_x * 25000 + 12500  # Center of 25km region
        center_z = region_z * 25000 + 12500
        
        # Test multiple points in this region
        river_found = False
        for dx in range(-5000, 6000, 2500):  # Test points within the region
            for dz in range(-5000, 6000, 2500):
                test_x = center_x + dx
                test_z = center_z + dz
                
                river_strength = worldgen_cpp.query_river_at_point(test_x, test_z, seed)
                if river_strength > 0:
                    print(f"  Found river in region ({region_x},{region_z}) at ({test_x:.0f},{test_z:.0f}) strength={river_strength:.2f}")
                    region_with_rivers = (region_x, region_z, center_x, center_z)
                    river_found = True
                    break
            if river_found:
                break
        if river_found:
            break
    
    if not region_with_rivers:
        print("ERROR: No rivers found in any tested regions!")
        return
    
    region_x, region_z, center_x, center_z = region_with_rivers
    print(f"Using region ({region_x},{region_z}) centered at ({center_x:.0f},{center_z:.0f})")
    
    # Step 2: Generate detailed visualization around the river region
    print("\nStep 2: Generating detailed visualization...")
    view_size = 3000  # 3km area around the rivers
    samples = worldgen_cpp.get_optimal_sample_count(view_size, "rivers")
    
    print(f"Creating {view_size}m x {view_size}m visualization with {samples}x{samples} samples")
    print(f"Resolution: {view_size/samples:.1f}m per pixel")
    
    # Create coordinate arrays centered on the river region
    half_size = view_size / 2
    x_coords = np.linspace(center_x - half_size, center_x + half_size, samples)
    z_coords = np.linspace(center_z - half_size, center_z + half_size, samples)
    x_grid, z_grid = np.meshgrid(x_coords, z_coords)
    
    x_flat = x_grid.flatten()
    z_flat = z_grid.flatten()
    
    print("Generating terrain data...")
    regular_terrain = worldgen_cpp.generate_terrain_heightmap(x_flat, z_flat, seed)
    carved_terrain = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, z_flat, seed)
    river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_flat, z_flat, seed)
    
    # Reshape to grids
    regular_grid = regular_terrain.reshape(samples, samples)
    carved_grid = carved_terrain.reshape(samples, samples)
    river_grid = river_flow.reshape(samples, samples)
    
    # Calculate results
    carving_depth = regular_grid - carved_grid
    
    # River analysis
    river_threshold = 0.1
    rivers_mask = river_grid >= river_threshold
    river_count = np.sum(rivers_mask)
    river_percentage = (river_count / (samples * samples)) * 100
    
    print(f"\nResults:")
    print(f"  Total samples: {samples * samples:,}")
    print(f"  Rivers found: {river_count:,}")
    print(f"  River coverage: {river_percentage:.2f}%")
    print(f"  Terrain range: {np.min(carved_grid):.1f}m to {np.max(carved_grid):.1f}m")
    print(f"  Max carving: {np.max(carving_depth):.2f}m")
    
    if river_count > 0:
        river_values = river_grid[rivers_mask]
        print(f"  River strength range: {np.min(river_values):.3f} to {np.max(river_values):.3f}")
    
    # Step 3: Create comprehensive visualization
    print("\nStep 3: Creating visualization...")
    plt.figure(figsize=(20, 12))
    
    # Terrain with rivers overlay
    plt.subplot(2, 3, 1)
    apply_standardized_elevation_coloring(carved_grid, plt.gca(), f'Terrain + Rivers (Region {region_x},{region_z})')
    if river_count > 0:
        # Create blue overlay for rivers
        river_masked = np.ma.masked_where(river_grid < river_threshold, river_grid)
        plt.imshow(river_masked, cmap='Blues', origin='lower', alpha=0.7, vmin=river_threshold)
        plt.colorbar(label='River Strength', shrink=0.8)
    
    # Regular terrain (no carving)
    plt.subplot(2, 3, 2)
    apply_standardized_elevation_coloring(regular_grid, plt.gca(), 'Regular Terrain (No Rivers)')
    
    # River carving depth
    plt.subplot(2, 3, 3)
    plt.imshow(carving_depth, cmap='Blues', origin='lower')
    plt.title('River Carving Depth')
    plt.colorbar(label='Carving Depth (m)', shrink=0.8)
    
    # River strength map
    plt.subplot(2, 3, 4)
    plt.imshow(river_grid, cmap='Blues', origin='lower', vmin=0, vmax=np.max(river_grid) if river_count > 0 else 1)
    plt.title('River Strength Map')
    plt.colorbar(label='River Strength', shrink=0.8)
    
    # Cross-section through center
    plt.subplot(2, 3, 5)
    mid_row = samples // 2
    x_line = np.linspace(-half_size, half_size, samples)
    regular_line = regular_grid[mid_row, :]
    carved_line = carved_grid[mid_row, :]
    river_line = river_grid[mid_row, :]
    
    plt.plot(x_line, regular_line, 'b-', label='Regular Terrain', alpha=0.7)
    plt.plot(x_line, carved_line, 'g-', label='Carved Terrain', linewidth=2)
    plt.fill_between(x_line, regular_line, carved_line, alpha=0.3, color='blue', label='River Carving')
    
    # Mark river locations
    if river_count > 0:
        river_points = river_line >= river_threshold
        if np.any(river_points):
            plt.scatter(x_line[river_points], carved_line[river_points], c='red', s=20, label='Rivers', zorder=5)
    
    plt.xlabel('Distance from Center (m)')
    plt.ylabel('Elevation (m)')
    plt.title('Cross-Section Through Center')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Statistics
    plt.subplot(2, 3, 6)
    plt.text(0.1, 0.9, f"Region: ({region_x}, {region_z})", transform=plt.gca().transAxes)
    plt.text(0.1, 0.8, f"Center: ({center_x:.0f}, {center_z:.0f})", transform=plt.gca().transAxes)
    plt.text(0.1, 0.7, f"Area: {view_size}m x {view_size}m", transform=plt.gca().transAxes)
    plt.text(0.1, 0.6, f"Resolution: {view_size/samples:.1f}m/pixel", transform=plt.gca().transAxes)
    plt.text(0.1, 0.5, f"Rivers: {river_count:,} points ({river_percentage:.2f}%)", transform=plt.gca().transAxes)
    plt.text(0.1, 0.4, f"Elevation: {np.min(carved_grid):.1f} to {np.max(carved_grid):.1f}m", transform=plt.gca().transAxes)
    plt.text(0.1, 0.3, f"Max Carving: {np.max(carving_depth):.2f}m", transform=plt.gca().transAxes)
    plt.title('Visualization Statistics')
    plt.gca().set_xlim(0, 1)
    plt.gca().set_ylim(0, 1)
    plt.gca().axis('off')
    
    plt.tight_layout()
    plt.savefig(f'river_visualization_region_{region_x}_{region_z}.png', dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"Saved: river_visualization_region_{region_x}_{region_z}.png")

if __name__ == "__main__":
    find_rivers_and_visualize()
