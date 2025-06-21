#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import worldgen_cpp
from elevation_colormap import apply_standardized_elevation_coloring, get_elevation_stats

def test_new_river_system():
    print("Testing new realistic river system...")
    
    # Test at different scales with higher resolution for better detail
    test_areas = [
        {"name": "25km_area", "size": 25000, "resolution": 0.04},   # 1000x1000 samples (was 500x500)
        {"name": "50km_area", "size": 50000, "resolution": 0.03},   # 1500x1500 samples (was 750x750)
        {"name": "100km_area", "size": 100000, "resolution": 0.02}, # 2000x2000 samples (was 1000x1000)
    ]
    
    seed = 12345
    
    for area in test_areas:
        print(f"\n=== Testing {area['name']} ===")
        size = area['size']
        resolution = area['resolution']
        
        # Create coordinate arrays
        samples = int(size * resolution)
        print(f"Generating {samples}x{samples} samples for {size/1000:.1f}km area")
        
        x_coords = np.linspace(-size/2, size/2, samples)
        z_coords = np.linspace(-size/2, size/2, samples)
        x_grid, z_grid = np.meshgrid(x_coords, z_coords)
        
        x_flat = x_grid.flatten()
        z_flat = z_grid.flatten()
        
        # Generate regular terrain
        print("Generating regular terrain...")
        regular_terrain = worldgen_cpp.generate_terrain_heightmap(x_flat, z_flat, seed)
        regular_grid = regular_terrain.reshape(samples, samples)
        
        # Generate river-carved terrain
        print("Generating river-carved terrain...")
        carved_terrain = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, z_flat, seed)
        carved_grid = carved_terrain.reshape(samples, samples)
        
        # Generate river flow data
        print("Generating river flow data...")
        river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_flat, z_flat, seed)
        river_grid = river_flow.reshape(samples, samples)
        
        # Calculate carving depth
        carving_depth = regular_grid - carved_grid
        
        # Analyze river statistics
        river_threshold = 1.0   # Any river strength > 1.0
        rivers_mask = river_grid >= river_threshold
        river_count = np.sum(rivers_mask)
        river_percentage = (river_count / (samples * samples)) * 100
        
        # Get terrain statistics using standardized system
        terrain_stats = get_elevation_stats(carved_grid)
        
        print(f"Results:")
        print(f"  Total samples: {samples * samples:,}")
        print(f"  Rivers found: {river_count:,}")
        print(f"  River coverage: {river_percentage:.2f}%")
        print(f"  Terrain elevation range: {terrain_stats['min_elevation']:.1f}m to {terrain_stats['max_elevation']:.1f}m")
        print(f"  Land coverage: {terrain_stats['land_percentage']:.1f}%")
        print(f"  Sea coverage: {terrain_stats['sea_level_percentage']:.1f}%")
        print(f"  Average carving depth: {np.mean(carving_depth):.2f}m")
        print(f"  Max carving depth: {np.max(carving_depth):.2f}m")
        
        if river_count > 0:
            river_carving = carving_depth[rivers_mask]
            print(f"  Average carving in river areas: {np.mean(river_carving):.2f}m")
            print(f"  Max carving in river areas: {np.max(river_carving):.2f}m")
        
        # Generate comprehensive visualization
        plt.figure(figsize=(20, 10))
        
        # Use absolute game world elevation range for consistent coloring
        GAME_MIN_ELEVATION = -2048.0  # Game world minimum
        GAME_MAX_ELEVATION = 2048.0   # Game world maximum
        
        # Regular terrain
        plt.subplot(2, 4, 1)
        apply_standardized_elevation_coloring(regular_grid, plt.gca(), f'Regular Terrain - {area["name"]}')
        
        # River-carved terrain
        plt.subplot(2, 4, 2)
        apply_standardized_elevation_coloring(carved_grid, plt.gca(), f'River-Carved Terrain - {area["name"]}')
        
        # Carving depth map
        plt.subplot(2, 4, 3)
        plt.imshow(carving_depth, cmap='Blues', origin='lower')
        plt.title(f'River Carving Depth - {area["name"]}')
        plt.colorbar(label='Carving Depth (m)')
        
        # Rivers overlay on carved terrain
        plt.subplot(2, 4, 4)
        apply_standardized_elevation_coloring(carved_grid, plt.gca(), f'Rivers on Carved Terrain ({river_count} rivers)')
        if river_count > 0:
            river_masked = np.ma.masked_where(river_grid < river_threshold, river_grid)
            plt.imshow(river_masked, cmap='Blues', origin='lower', alpha=0.6)
        
        # Cross-sections showing carving effects
        mid_row = samples // 2
        mid_col = samples // 2
        
        # Horizontal cross-section
        plt.subplot(2, 4, 5)
        x_line = x_coords
        regular_line = regular_grid[mid_row, :]
        carved_line = carved_grid[mid_row, :]
        plt.plot(x_line, regular_line, 'b-', label='Regular Terrain', alpha=0.7)
        plt.plot(x_line, carved_line, 'g-', label='River-Carved Terrain')
        plt.fill_between(x_line, regular_line, carved_line, alpha=0.3, color='blue', label='Carving')
        plt.xlabel('Distance (m)')
        plt.ylabel('Elevation (m)')
        plt.title('Horizontal Cross-Section')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Vertical cross-section
        plt.subplot(2, 4, 6)
        z_line = z_coords
        regular_line = regular_grid[:, mid_col]
        carved_line = carved_grid[:, mid_col]
        plt.plot(z_line, regular_line, 'b-', label='Regular Terrain', alpha=0.7)
        plt.plot(z_line, carved_line, 'g-', label='River-Carved Terrain')
        plt.fill_between(z_line, regular_line, carved_line, alpha=0.3, color='blue', label='Carving')
        plt.xlabel('Distance (m)')
        plt.ylabel('Elevation (m)')
        plt.title('Vertical Cross-Section')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Carving vs elevation scatter plot
        plt.subplot(2, 4, 7)
        if river_count > 0:
            river_elevations = carved_grid[rivers_mask]
            river_carvings = carving_depth[rivers_mask]
            plt.scatter(river_elevations, river_carvings, alpha=0.5, s=1)
            plt.xlabel('Elevation (m)')
            plt.ylabel('Carving Depth (m)')
            plt.title('Carving vs Elevation')
            plt.grid(True, alpha=0.3)
        
        # River strength histogram
        plt.subplot(2, 4, 8)
        if river_count > 0:
            river_values = river_grid[rivers_mask]
            plt.hist(river_values, bins=50, alpha=0.7, color='blue')
            plt.xlabel('River Strength')
            plt.ylabel('Count')
            plt.title('River Strength Distribution')
            plt.yscale('log')
            plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(f'new_river_system_{area["name"]}.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"  Saved: new_river_system_{area['name']}.png")

if __name__ == "__main__":
    test_new_river_system()
