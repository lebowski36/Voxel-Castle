#!/usr/bin/env python3
"""
Enhanced River Visualizer - Shows terrain with and without river carving
Demonstrates the ProceduralTerrain system with integrated river networks
"""

import sys
import os
sys.path.append('/home/system-x1/Projects/Voxel Castle/scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors
from matplotlib.patches import Rectangle
import worldgen_cpp

def generate_terrain_comparison(center_x, center_z, size_km, resolution, seed):
    """Generate terrain data for comparison visualization"""
    
    # Convert km to coordinate units (1 unit = 0.25m, so 1km = 4000 units)
    size_units = int(size_km * 4000)
    half_size = size_units // 2
    
    # Create coordinate grids
    x_coords = np.linspace(center_x - half_size, center_x + half_size, resolution, dtype=np.float32)
    z_coords = np.linspace(center_z - half_size, center_z + half_size, resolution, dtype=np.float32)
    
    # Create 2D coordinate arrays
    X, Z = np.meshgrid(x_coords, z_coords)
    x_flat = X.flatten()
    z_flat = Z.flatten()
    
    print(f"Generating terrain data for {size_km}km x {size_km}km region...")
    print(f"Resolution: {resolution}x{resolution} points")
    print(f"Coordinate range: ({center_x - half_size}, {center_z - half_size}) to ({center_x + half_size}, {center_z + half_size})")
    
    # Generate normal terrain
    terrain_normal = worldgen_cpp.generate_terrain_heightmap(x_flat, z_flat, seed)
    terrain_normal = terrain_normal.reshape(resolution, resolution)
    
    # Generate terrain with river carving
    terrain_with_rivers = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, z_flat, seed)
    terrain_with_rivers = terrain_with_rivers.reshape(resolution, resolution)
    
    # Calculate river carving effect
    river_carving = terrain_with_rivers - terrain_normal
    
    # Generate river flow data
    elevation_flat = terrain_normal.flatten()
    climate_data = []
    for i in range(len(x_flat)):
        climate = worldgen_cpp.generate_climate_data(x_flat[i], z_flat[i], seed, elevation_flat[i])
        climate_data.append(climate['precipitation'])
    
    precipitation_flat = np.array(climate_data, dtype=np.float32)
    river_flow = worldgen_cpp.generate_river_flow(x_flat, z_flat, elevation_flat, precipitation_flat, seed)
    river_flow = river_flow.reshape(resolution, resolution)
    
    return {
        'x_coords': x_coords,
        'z_coords': z_coords,
        'terrain_normal': terrain_normal,
        'terrain_with_rivers': terrain_with_rivers,
        'river_carving': river_carving,
        'river_flow': river_flow,
        'size_km': size_km,
        'seed': seed
    }

def create_river_comparison_plot(data):
    """Create a comprehensive comparison plot showing river effects"""
    
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle(f'ProceduralTerrain River System - {data["size_km"]}km Region (Seed: {data["seed"]})', 
                 fontsize=16, fontweight='bold')
    
    # Convert coordinates to kilometers for display
    x_km = (data['x_coords'] * 0.25) / 1000  # Convert to km
    z_km = (data['z_coords'] * 0.25) / 1000
    
    # 1. Normal terrain (top-left)
    ax1 = axes[0, 0]
    terrain_min, terrain_max = data['terrain_normal'].min(), data['terrain_normal'].max()
    im1 = ax1.imshow(data['terrain_normal'], extent=[x_km[0], x_km[-1], z_km[0], z_km[-1]], 
                     cmap='terrain', origin='lower')
    ax1.set_title('Normal Terrain (No Rivers)', fontweight='bold')
    ax1.set_xlabel('Distance (km)')
    ax1.set_ylabel('Distance (km)')
    cbar1 = plt.colorbar(im1, ax=ax1, shrink=0.8)
    cbar1.set_label('Elevation (m)')
    ax1.text(0.02, 0.98, f'Range: {terrain_min:.1f}m to {terrain_max:.1f}m', 
             transform=ax1.transAxes, verticalalignment='top', 
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    # 2. Terrain with rivers (top-right)
    ax2 = axes[0, 1]
    river_min, river_max = data['terrain_with_rivers'].min(), data['terrain_with_rivers'].max()
    im2 = ax2.imshow(data['terrain_with_rivers'], extent=[x_km[0], x_km[-1], z_km[0], z_km[-1]], 
                     cmap='terrain', origin='lower')
    ax2.set_title('Terrain with River Carving', fontweight='bold')
    ax2.set_xlabel('Distance (km)')
    ax2.set_ylabel('Distance (km)')
    cbar2 = plt.colorbar(im2, ax=ax2, shrink=0.8)
    cbar2.set_label('Elevation (m)')
    ax2.text(0.02, 0.98, f'Range: {river_min:.1f}m to {river_max:.1f}m', 
             transform=ax2.transAxes, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    # 3. River carving effect (bottom-left)
    ax3 = axes[1, 0]
    carving_min, carving_max = data['river_carving'].min(), data['river_carving'].max()
    # Use a diverging colormap centered at 0 for carving
    carving_norm = colors.TwoSlopeNorm(vmin=carving_min, vcenter=0, vmax=carving_max)
    im3 = ax3.imshow(data['river_carving'], extent=[x_km[0], x_km[-1], z_km[0], z_km[-1]], 
                     cmap='RdBu_r', norm=carving_norm, origin='lower')
    ax3.set_title('River Carving Effect', fontweight='bold')
    ax3.set_xlabel('Distance (km)')
    ax3.set_ylabel('Distance (km)')
    cbar3 = plt.colorbar(im3, ax=ax3, shrink=0.8)
    cbar3.set_label('Carving Depth (m)')
    
    # Count river-carved areas
    river_areas = np.sum(data['river_carving'] < -0.1)  # Areas carved by more than 10cm
    total_areas = data['river_carving'].size
    river_percentage = (river_areas / total_areas) * 100
    
    ax3.text(0.02, 0.98, f'River Coverage: {river_percentage:.1f}%\nMax Carving: {abs(carving_min):.1f}m', 
             transform=ax3.transAxes, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    # 4. River flow accumulation (bottom-right)
    ax4 = axes[1, 1]
    flow_min, flow_max = data['river_flow'].min(), data['river_flow'].max()
    im4 = ax4.imshow(data['river_flow'], extent=[x_km[0], x_km[-1], z_km[0], z_km[-1]], 
                     cmap='Blues', origin='lower')
    ax4.set_title('River Flow Accumulation', fontweight='bold')
    ax4.set_xlabel('Distance (km)')
    ax4.set_ylabel('Distance (km)')
    cbar4 = plt.colorbar(im4, ax=ax4, shrink=0.8)
    cbar4.set_label('Flow Accumulation')
    
    # Count significant flow areas
    flow_areas = np.sum(data['river_flow'] > 0.3)  # Areas with significant flow
    flow_percentage = (flow_areas / total_areas) * 100
    
    ax4.text(0.02, 0.98, f'Flow Areas: {flow_percentage:.1f}%\nMax Flow: {flow_max:.2f}', 
             transform=ax4.transAxes, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    plt.tight_layout()
    return fig

def main():
    """Generate river comparison visualizations"""
    
    print("🌊 Enhanced River Visualizer")
    print("=" * 50)
    
    # Test different scales and seeds
    test_cases = [
        {'center_x': 2000, 'center_z': 2000, 'size_km': 5, 'resolution': 256, 'seed': 12345, 'name': '5km_rivers'},
        {'center_x': 5000, 'center_z': 5000, 'size_km': 10, 'resolution': 256, 'seed': 54321, 'name': '10km_rivers'},
        {'center_x': 1000, 'center_z': 3000, 'size_km': 2, 'resolution': 256, 'seed': 98765, 'name': '2km_detailed_rivers'},
    ]
    
    for case in test_cases:
        print(f"\n📊 Generating {case['name']} visualization...")
        
        # Generate terrain data
        data = generate_terrain_comparison(
            case['center_x'], case['center_z'], 
            case['size_km'], case['resolution'], case['seed']
        )
        
        # Create visualization
        fig = create_river_comparison_plot(data)
        
        # Save the plot
        filename = f"{case['name']}_comparison.png"
        plt.savefig(filename, dpi=150, bbox_inches='tight')
        print(f"✅ Saved: {filename}")
        
        # Calculate statistics
        river_carved_area = np.sum(data['river_carving'] < -0.1)
        total_area = data['river_carving'].size
        avg_carving = np.mean(data['river_carving'][data['river_carving'] < -0.1])
        max_carving = abs(data['river_carving'].min())
        
        print(f"   📈 Statistics:")
        print(f"   - River-carved area: {(river_carved_area/total_area)*100:.1f}%")
        print(f"   - Average carving depth: {avg_carving:.2f}m")
        print(f"   - Maximum carving depth: {max_carving:.2f}m")
        print(f"   - Elevation range: {data['terrain_with_rivers'].min():.1f}m to {data['terrain_with_rivers'].max():.1f}m")
        
        plt.close(fig)
    
    print(f"\n🎉 River visualization complete!")
    print("Generated files show:")
    print("  • Normal terrain vs terrain with river carving")
    print("  • River carving depth effects")
    print("  • River flow accumulation patterns")
    print("  • Statistical analysis of river coverage")

if __name__ == "__main__":
    main()
