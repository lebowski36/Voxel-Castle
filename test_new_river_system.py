#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import worldgen_cpp
from elevation_colormap import apply_standardized_elevation_coloring, get_elevation_stats

def find_river_locations(seed, search_size=50000, sample_spacing=1000):
    """Find locations that contain rivers by sparse sampling"""
    print(f"Searching for rivers in {search_size/1000:.1f}km area with {sample_spacing}m spacing...")
    
    river_locations = []
    samples_per_side = int(search_size / sample_spacing)
    
    for i in range(samples_per_side):
        for j in range(samples_per_side):
            x = -search_size/2 + i * sample_spacing
            z = -search_size/2 + j * sample_spacing
            
            # Quick river check
            try:
                river_data = worldgen_cpp.query_river_at_point(x, z, seed)
                if river_data > 0:  # Has river
                    river_locations.append((x, z, river_data))
                    print(f"  Found river at ({x:.0f}, {z:.0f}) strength={river_data:.2f}")
                    
                    # Stop after finding a few rivers
                    if len(river_locations) >= 5:
                        break
            except:
                continue
        
        if len(river_locations) >= 5:
            break
    
    print(f"Found {len(river_locations)} river locations")
    return river_locations

def test_new_river_system():
    print("Testing new realistic river system...")
    
    seed = 12345
    
    # Step 1: Find actual river locations
    river_locations = find_river_locations(seed)
    
    if not river_locations:
        print("ERROR: No rivers found in search area!")
        return
    
    # Step 2: Test each found river location with focused view
    for i, (river_x, river_z, river_strength) in enumerate(river_locations):
        print(f"\n=== Testing River {i+1} at ({river_x:.0f}, {river_z:.0f}) ===")
        
        # Small focused area around the river (2km x 2km)
        view_size = 2000  # 2km
        samples = 200     # 200x200 = 40,000 samples (manageable)
        
        print(f"Generating {samples}x{samples} samples for {view_size}m focused view")
        
        # Create coordinate arrays centered on the river
        x_coords = np.linspace(river_x - view_size/2, river_x + view_size/2, samples)
        z_coords = np.linspace(river_z - view_size/2, river_z + view_size/2, samples)
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
        river_threshold = 0.5   # Lower threshold for better detection
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
        print(f"  Average carving depth: {np.mean(carving_depth):.2f}m")
        print(f"  Max carving depth: {np.max(carving_depth):.2f}m")
        
        if river_count > 0:
            river_carving = carving_depth[rivers_mask]
            print(f"  Average carving in river areas: {np.mean(river_carving):.2f}m")
            print(f"  Max carving in river areas: {np.max(river_carving):.2f}m")
        
        # Generate focused visualization
        plt.figure(figsize=(16, 12))
        
        # Regular terrain
        plt.subplot(2, 3, 1)
        apply_standardized_elevation_coloring(regular_grid, plt.gca(), f'Regular Terrain - River {i+1}')
        
        # River-carved terrain
        plt.subplot(2, 3, 2)
        apply_standardized_elevation_coloring(carved_grid, plt.gca(), f'River-Carved Terrain - River {i+1}')
        
        # Rivers overlay on carved terrain (this is the key visualization!)
        plt.subplot(2, 3, 3)
        apply_standardized_elevation_coloring(carved_grid, plt.gca(), f'Rivers on Terrain ({river_count} detected)')
        if river_count > 0:
            # Create blue overlay for rivers
            river_masked = np.ma.masked_where(river_grid < river_threshold, river_grid)
            plt.imshow(river_masked, cmap='Blues', origin='lower', alpha=0.8, vmin=river_threshold, vmax=np.max(river_grid))
            plt.colorbar(label='River Strength')
        
        # Carving depth map
        plt.subplot(2, 3, 4)
        plt.imshow(carving_depth, cmap='Blues', origin='lower')
        plt.title(f'River Carving Depth - River {i+1}')
        plt.colorbar(label='Carving Depth (m)')
        
        # Cross-section through river center
        plt.subplot(2, 3, 5)
        mid_row = samples // 2
        x_line = x_coords
        regular_line = regular_grid[mid_row, :]
        carved_line = carved_grid[mid_row, :]
        river_line = river_grid[mid_row, :]
        
        plt.plot(x_line, regular_line, 'b-', label='Regular Terrain', alpha=0.7)
        plt.plot(x_line, carved_line, 'g-', label='River-Carved Terrain', linewidth=2)
        plt.fill_between(x_line, regular_line, carved_line, alpha=0.3, color='blue', label='River Carving')
        
        # Mark river locations
        river_points = river_line >= river_threshold
        if np.any(river_points):
            plt.scatter(x_line[river_points], carved_line[river_points], c='red', s=20, label='River Points', zorder=5)
        
        plt.xlabel('Distance (m)')
        plt.ylabel('Elevation (m)')
        plt.title(f'Cross-Section Through River {i+1}')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # River strength vs carving correlation
        plt.subplot(2, 3, 6)
        if river_count > 0:
            river_strengths = river_grid[rivers_mask]
            river_carvings = carving_depth[rivers_mask]
            plt.scatter(river_strengths, river_carvings, alpha=0.6, s=2)
            plt.xlabel('River Strength')
            plt.ylabel('Carving Depth (m)')
            plt.title('River Strength vs Carving')
            plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(f'focused_river_test_river_{i+1}.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"  Saved: focused_river_test_river_{i+1}.png")
        
        # Only test first 2 rivers to avoid too many images
        if i >= 1:
            break

if __name__ == "__main__":
    test_new_river_system()
