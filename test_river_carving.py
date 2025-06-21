#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import worldgen_cpp

def test_river_carving_effect():
    """Test to specifically examine river carving effects on terrain"""
    print("Testing river carving effects...")
    
    # Focus on a smaller area where we can see details
    size = 5000  # 5km area
    resolution = 0.2  # High resolution for detail
    samples = int(size * resolution)  # 1000x1000 samples
    seed = 12345
    
    print(f"Generating {samples}x{samples} samples for {size/1000:.1f}km area")
    
    # Create coordinate arrays centered on origin
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
    
    # Analyze results
    print("\n=== River Carving Analysis ===")
    print(f"Regular terrain range: {np.min(regular_grid):.1f}m to {np.max(regular_grid):.1f}m")
    print(f"Carved terrain range: {np.min(carved_grid):.1f}m to {np.max(carved_grid):.1f}m")
    print(f"Carving depth range: {np.min(carving_depth):.1f}m to {np.max(carving_depth):.1f}m")
    print(f"Average carving depth: {np.mean(carving_depth):.2f}m")
    
    # Find areas with significant rivers
    river_threshold = 30.0  # C++ RIVER_THRESHOLD
    rivers_mask = river_grid >= river_threshold
    river_count = np.sum(rivers_mask)
    
    if river_count > 0:
        river_carving_depth = carving_depth[rivers_mask]
        print(f"Rivers found: {river_count}")
        print(f"Average carving in river areas: {np.mean(river_carving_depth):.2f}m")
        print(f"Max carving in river areas: {np.max(river_carving_depth):.2f}m")
        
        # Find strongest rivers
        river_values = river_grid[rivers_mask]
        strong_rivers = river_values >= 100
        if np.sum(strong_rivers) > 0:
            strong_river_carving = river_carving_depth[strong_rivers]
            print(f"Strong rivers (>100): {np.sum(strong_rivers)}")
            print(f"Average carving in strong river areas: {np.mean(strong_river_carving):.2f}m")
    
    # Create comprehensive visualization
    plt.figure(figsize=(20, 12))
    
    # Regular terrain
    plt.subplot(2, 4, 1)
    plt.imshow(regular_grid, cmap='terrain', origin='lower')
    plt.title('Regular Terrain')
    plt.colorbar(label='Elevation (m)')
    
    # River-carved terrain
    plt.subplot(2, 4, 2)
    plt.imshow(carved_grid, cmap='terrain', origin='lower')
    plt.title('River-Carved Terrain')
    plt.colorbar(label='Elevation (m)')
    
    # Carving depth map
    plt.subplot(2, 4, 3)
    plt.imshow(carving_depth, cmap='Blues', origin='lower')
    plt.title('River Carving Depth')
    plt.colorbar(label='Carving Depth (m)')
    
    # River flow overlay
    plt.subplot(2, 4, 4)
    plt.imshow(carved_grid, cmap='terrain', origin='lower', alpha=0.7)
    river_masked = np.ma.masked_where(river_grid < river_threshold, river_grid)
    plt.imshow(river_masked, cmap='Blues', origin='lower', alpha=0.8)
    plt.title(f'Rivers on Carved Terrain ({river_count} rivers)')
    plt.colorbar(label='River Strength')
    
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
    
    # Carving vs river strength scatter plot
    plt.subplot(2, 4, 7)
    if river_count > 0:
        river_strengths = river_grid[rivers_mask]
        river_carvings = carving_depth[rivers_mask]
        plt.scatter(river_strengths, river_carvings, alpha=0.5, s=1)
        plt.xlabel('River Strength')
        plt.ylabel('Carving Depth (m)')
        plt.title('Carving vs River Strength')
        plt.grid(True, alpha=0.3)
    
    # River strength histogram
    plt.subplot(2, 4, 8)
    if river_count > 0:
        plt.hist(river_values, bins=50, alpha=0.7, color='blue')
        plt.xlabel('River Strength')
        plt.ylabel('Count')
        plt.title('River Strength Distribution')
        plt.yscale('log')
        plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('river_carving_analysis.png', dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"\nSaved: river_carving_analysis.png")

if __name__ == "__main__":
    test_river_carving_effect()
