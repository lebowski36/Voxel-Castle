#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import worldgen_cpp

def test_realistic_river_density():
    print("Testing realistic river density...")
    
    # Test different scales to show river distribution - zoomed out more
    test_areas = [
        {"name": "25km_area", "size": 25000, "resolution": 0.08},  # 200x200 samples
        {"name": "50km_area", "size": 50000, "resolution": 0.04},  # 200x200 samples  
        {"name": "100km_area", "size": 100000, "resolution": 0.02}, # 200x200 samples
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
        
        # Generate river data
        print("Generating river flow data...")
        river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_flat, z_flat, seed)
        river_grid = river_flow.reshape(samples, samples)
        
        # Analyze river statistics with proper thresholds
        river_threshold = 1.0   # Any river strength > 1.0
        rivers_mask = river_grid >= river_threshold
        river_count = np.sum(rivers_mask)
        river_percentage = (river_count / (samples * samples)) * 100
        
        print(f"Results:")
        print(f"  Total samples: {samples * samples:,}")
        print(f"  Rivers found: {river_count:,}")
        print(f"  River coverage: {river_percentage:.2f}%")
        print(f"  Expected rivers per km²: {river_count / (size/1000)**2:.1f}")
        
        # Analyze river strengths
        river_values = river_grid[rivers_mask]
        if len(river_values) > 0:
            print(f"  River strength stats:")
            print(f"    Min: {np.min(river_values):.1f}")
            print(f"    Max: {np.max(river_values):.1f}")
            print(f"    Mean: {np.mean(river_values):.1f}")
            
            # Count different river types with more realistic thresholds
            major_rivers = np.sum(river_values >= 500)   # Major rivers (500+)
            regional_rivers = np.sum((river_values >= 100) & (river_values < 500))  # Regional (100-500)
            small_rivers = np.sum((river_values >= 30) & (river_values < 100))   # Small rivers (30-100)
            streams = np.sum((river_values >= 10) & (river_values < 30))          # Streams (10-30)
            creeks = np.sum((river_values >= 1) & (river_values < 10))            # Creeks (1-10)
            
            print(f"  River type distribution:")
            print(f"    Major rivers: {major_rivers} ({major_rivers/(size/1000)**2:.2f} per km²)")
            print(f"    Regional rivers: {regional_rivers} ({regional_rivers/(size/1000)**2:.2f} per km²)")
            print(f"    Small rivers: {small_rivers} ({small_rivers/(size/1000)**2:.2f} per km²)")
            print(f"    Streams: {streams} ({streams/(size/1000)**2:.2f} per km²)")
            print(f"    Creeks: {creeks} ({creeks/(size/1000)**2:.2f} per km²)")
        
        # Generate visualization comparing regular vs river-carved terrain
        plt.figure(figsize=(15, 5))
        
        # Regular heightmap
        plt.subplot(1, 3, 1)
        heightmap = worldgen_cpp.generate_terrain_heightmap(x_flat, z_flat, seed)
        heightmap_grid = heightmap.reshape(samples, samples)
        plt.imshow(heightmap_grid, cmap='terrain', origin='lower')
        plt.title(f'Terrain - {area["name"]}')
        plt.colorbar(label='Elevation (m)')
        
        # River-carved heightmap
        plt.subplot(1, 3, 2)
        carved_heightmap = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, z_flat, seed)
        carved_heightmap_grid = carved_heightmap.reshape(samples, samples)
        plt.imshow(carved_heightmap_grid, cmap='terrain', origin='lower')
        plt.title(f'River-Carved Terrain - {area["name"]}')
        plt.colorbar(label='Elevation (m)')
        
        # Rivers overlay on carved terrain
        plt.subplot(1, 3, 3)
        plt.imshow(carved_heightmap_grid, cmap='terrain', origin='lower', alpha=0.7)
        
        # Show rivers with different colors by type
        if len(river_values) > 0:
            river_display = np.zeros_like(river_grid)
            river_display[river_grid >= 500] = 5      # Major rivers - red
            river_display[(river_grid >= 100) & (river_grid < 500)] = 4  # Regional - orange
            river_display[(river_grid >= 30) & (river_grid < 100)] = 3    # Small - yellow
            river_display[(river_grid >= 10) & (river_grid < 30)] = 2      # Streams - light blue
            river_display[(river_grid >= 1) & (river_grid < 10)] = 1       # Creeks - blue
            
            river_masked = np.ma.masked_where(river_display == 0, river_display)
            plt.imshow(river_masked, cmap='cool', origin='lower', alpha=0.8, vmin=1, vmax=5)
        
        plt.title(f'Rivers - {area["name"]} ({river_count} rivers)')
        plt.colorbar(label='River Type', ticks=[1,2,3,4,5], 
                    format=plt.FuncFormatter(lambda x, p: ['','Creeks','Streams','Small','Regional','Major'][int(x)]))
        
        plt.tight_layout()
        plt.savefig(f'river_density_test_{area["name"]}.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"  Saved: river_density_test_{area['name']}.png")

if __name__ == "__main__":
    test_realistic_river_density()
