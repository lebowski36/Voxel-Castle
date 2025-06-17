#!/usr/bin/env python3
"""
ProceduralTerrain Visualization Tool
====================================

Enhanced visualization tool for the ProceduralTerrain world generation system.
Uses C++ backend for accurate terrain generation matching the actual game engine.

Features:
- Heightmap visualization with meter scale legend
- Climate system visualization (temperature, humidity, precipitation)
- River network and water flow visualization
- Large-scale terrain previews (8km+ regions)
- Multiple visualization modes and export formats

Usage:
    python3 worldgen_visualizer.py --seed 12345 --size 512 --output heightmap.png
    python3 worldgen_visualizer.py --seed 12345 --mode climate --size 1024
    python3 worldgen_visualizer.py --seed 12345 --mode terrain --region -2000,-2000,4000,4000
    python3 worldgen_visualizer.py --seed 12345 --mode combined --scale 16000

Author: AI Assistant & User
Date: June 2025
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.patches as patches
import argparse
import sys
import os
from PIL import Image
import math
import progressbar
import os

# Add the directory containing worldgen_cpp to the Python path
# This assumes the script is run from the project root directory
project_root = os.getcwd()
cpp_wrapper_dir = os.path.join(project_root, 'scripts', 'cpp_wrapper')
if cpp_wrapper_dir not in sys.path:
    sys.path.insert(0, cpp_wrapper_dir)

try:
    import worldgen_cpp
except ImportError as e:
    print(f"Error: Failed to import 'worldgen_cpp'.")
    print(f"Details: {e}")
    print(f"Please ensure the C++ backend is compiled correctly in: {cpp_wrapper_dir}")
    sys.exit(1)

class WorldVisualizer:
    def __init__(self, seed):
        print(f"Initializing world generator with seed {seed}...")
        self.seed = seed

    def generate_heightmap(self, x_min, y_min, width, height, resolution):
        """Generates a heightmap for a given region using the C++ backend."""
        
        # Ensure width and height are integers
        width = int(width)
        height = int(height)
        
        # Calculate actual sampling dimensions based on resolution
        sample_width = int(width * resolution)
        sample_height = int(height * resolution)
        
        print(f"Sampling {sample_width}x{sample_height} points for {width}x{height} world units...")
        
        # Create coordinate arrays with proper sampling
        x_coords = np.linspace(x_min, x_min + width, sample_width, dtype=np.float32)
        y_coords = np.linspace(y_min, y_min + height, sample_height, dtype=np.float32)
        xx, yy = np.meshgrid(x_coords, y_coords)
        
        # Flatten arrays for the C++ function
        x_flat = xx.flatten()
        y_flat = yy.flatten()

        print("Generating heightmap with C++ backend...")
        
        # Call the C++ batch generation function
        height_values = worldgen_cpp.generate_terrain_heightmap(x_flat, y_flat, self.seed)
        
        # Reshape the flat array back into a 2D map
        heightmap = height_values.reshape((sample_height, sample_width))
        
        print("Heightmap generation complete.")
        return heightmap

    def generate_climate_map(self, x_min, y_min, width, height, resolution=1.0, mode='temperature'):
        """Generate climate map (temperature or precipitation)"""
        x_coords = np.linspace(x_min, x_min + width, int(width * resolution))
        y_coords = np.linspace(y_min, y_min + height, int(height * resolution))
        
        climate_map = np.zeros((len(y_coords), len(x_coords)))
        
        for i, y in enumerate(y_coords):
            for j, x in enumerate(x_coords):
                elevation = self.terrain.get_elevation(x, y)
                temperature = self.climate.get_temperature(x, y, elevation)
                
                if mode == 'temperature':
                    climate_map[i, j] = temperature
                else:  # precipitation
                    climate_map[i, j] = self.climate.get_precipitation(x, y, temperature, elevation)
                    
        return climate_map
    
    def generate_biome_map(self, x_min, y_min, width, height, resolution=1.0):
        """Generate biome classification map"""
        x_coords = np.linspace(x_min, x_min + width, int(width * resolution))
        y_coords = np.linspace(y_min, y_min + height, int(height * resolution))
        
        biome_map = np.zeros((len(y_coords), len(x_coords), 3))  # RGB colors
        
        for i, y in enumerate(y_coords):
            for j, x in enumerate(x_coords):
                elevation = self.terrain.get_elevation(x, y)
                temperature = self.climate.get_temperature(x, y, elevation)
                precipitation = self.climate.get_precipitation(x, y, temperature, elevation)
                
                biome = self.biomes.classify_biome(temperature, precipitation, elevation)
                biome_map[i, j] = self.biomes.BIOMES[biome]['color']
                
        return biome_map
    
    def generate_rivers_map(self, x_min, y_min, width, height, resolution=1.0):
        """Generate river network map"""
        x_coords = np.linspace(x_min, x_min + width, int(width * resolution))
        y_coords = np.linspace(y_min, y_min + height, int(height * resolution))
        
        river_map = np.zeros((len(y_coords), len(x_coords)))
        
        for i, y in enumerate(y_coords):
            for j, x in enumerate(x_coords):
                river_width = self.rivers.get_river_width(x, y)
                river_map[i, j] = river_width
                
        return river_map
    
    def generate_overlay_map(self, x_min, y_min, width, height, resolution=1.0):
        """Generate combined heightmap with rivers and biome overlay"""
        x_coords = np.linspace(x_min, x_min + width, int(width * resolution))
        y_coords = np.linspace(y_min, y_min + height, int(height * resolution))
        
        # Start with biome base layer
        overlay_map = np.zeros((len(y_coords), len(x_coords), 3))  # RGB
        
        for i, y in enumerate(y_coords):
            for j, x in enumerate(x_coords):
                elevation = self.terrain.get_elevation(x, y)
                temperature = self.climate.get_temperature(x, y, elevation)
                precipitation = self.climate.get_precipitation(x, y, temperature, elevation)
                
                # Base biome color
                biome = self.biomes.classify_biome(temperature, precipitation, elevation)
                base_color = np.array(self.biomes.BIOMES[biome]['color'])
                
                # Modify color based on elevation (shading effect)
                elevation_factor = (elevation + 2048) / 4096.0  # Normalize to 0-1
                shaded_color = base_color * (0.5 + elevation_factor * 0.5)
                
                # Add rivers (bright blue overlay)
                river_width = self.rivers.get_river_width(x, y)
                if river_width > 0:
                    river_intensity = min(1.0, river_width / 10.0)  # Normalize river width
                    river_color = np.array([0.2, 0.6, 1.0])  # Bright blue
                    shaded_color = shaded_color * (1 - river_intensity) + river_color * river_intensity
                
                overlay_map[i, j] = np.clip(shaded_color, 0, 1)
                
        return overlay_map

def main():
    parser = argparse.ArgumentParser(description='ProceduralTerrain Visualization Tool')
    parser.add_argument('--seed', type=int, default=12345, help='World seed')
    parser.add_argument('--mode', choices=['heightmap', 'temperature', 'precipitation', 'biomes', 'rivers', 'overlay'], 
                       default='heightmap', help='Visualization mode')
    parser.add_argument('--size', type=int, default=512, help='Map size in world units (creates square map)')
    parser.add_argument('--region', type=str, default=None, 
                       help='Override region as x_min,y_min,width,height (if not specified, uses --size centered at origin)')
    parser.add_argument('--output', type=str, default=None, help='Output filename')
    parser.add_argument('--resolution', type=float, default=0.5, help='Sampling resolution (samples per world unit)')
    
    args = parser.parse_args()
    
    # Calculate region based on size or use custom region
    if args.region is not None:
        # Use custom region
        try:
            x_min, y_min, width, height = map(float, args.region.split(','))
        except:
            print("Error: Invalid region format. Use: x_min,y_min,width,height")
            return 1
    else:
        # Use size parameter to create centered region
        half_size = args.size / 2
        x_min, y_min = -half_size, -half_size
        width, height = args.size, args.size
    
    # Create visualizer
    print(f"Initializing world generator with seed {args.seed}...")
    visualizer = WorldVisualizer(args.seed)
    
    # Calculate actual world region (accounting for 25cm voxel size)
    real_width_m = width * 0.25
    real_height_m = height * 0.25
    real_x_min_m = x_min * 0.25
    real_y_min_m = y_min * 0.25
    real_x_max_m = real_x_min_m + real_width_m
    real_y_max_m = real_y_min_m + real_height_m
    
    print(f"Generating {args.mode} map for region ({real_x_min_m:.1f}, {real_y_min_m:.1f}) to ({real_x_max_m:.1f}, {real_y_max_m:.1f}) meters...")
    print(f"World coordinates: ({x_min:.0f}, {y_min:.0f}) to ({x_min + width:.0f}, {y_min + height:.0f}) (1 unit = 25cm)")
    print(f"Resolution: {args.resolution} samples per world unit")
    
    # Generate output filename if not provided
    if args.output is None:
        args.output = f"worldgen_{args.mode}_seed{args.seed}_size{args.size}.png"
    
    # Generate data based on mode
    if args.mode == 'heightmap':
        data = visualizer.generate_heightmap(x_min, y_min, width, height, args.resolution)
        plt.figure(figsize=(12, 10))
        
        # Create custom colormap with sea level at center
        from matplotlib.colors import TwoSlopeNorm
        import matplotlib.colors as mcolors
        
        # Define sea-level centered normalization
        norm = TwoSlopeNorm(vmin=-2048, vcenter=0, vmax=2048)
        
        # Create custom colormap: blue for below sea level, terrain colors for above
        colors_below = plt.cm.Blues_r(np.linspace(0.3, 1, 128))  # Deep blue to light blue
        colors_above = plt.cm.terrain(np.linspace(0.35, 1, 128))  # Green to brown to white
        all_colors = np.vstack((colors_below, colors_above))
        sea_level_cmap = mcolors.LinearSegmentedColormap.from_list('sea_level', all_colors)
        
        # Use fixed color scale for ±2048m range with sea level centered coloring
        plt.imshow(data, cmap=sea_level_cmap, extent=[x_min, x_min+width, y_min+height, y_min], 
                  norm=norm)
        cbar = plt.colorbar(label='Elevation (m)')
        cbar.ax.axhline(y=0, color='white', linewidth=2)
        plt.title(f'Heightmap - Seed {args.seed} (Sea Level = 0m)')
        
        # Print statistics with fixed reference
        print(f"\nHeightmap Statistics:")
        print(f"  Min elevation: {np.min(data):.1f}m")
        print(f"  Max elevation: {np.max(data):.1f}m") 
        print(f"  Mean elevation: {np.mean(data):.1f}m")
        print(f"  Std deviation: {np.std(data):.1f}m")
        print(f"  Sea level (0m) coverage: {np.sum(data < 0) / data.size * 100:.1f}%")
        
    elif args.mode == 'temperature':
        data = visualizer.generate_climate_map(x_min, y_min, width, height, args.resolution, 'temperature')
        plt.figure(figsize=(12, 10))
        plt.imshow(data, cmap='coolwarm', extent=[x_min, x_min+width, y_min+height, y_min])
        plt.colorbar(label='Temperature (°C)')
        plt.title(f'Temperature Map - Seed {args.seed}')
        
    elif args.mode == 'precipitation':
        data = visualizer.generate_climate_map(x_min, y_min, width, height, args.resolution, 'precipitation')
        plt.figure(figsize=(12, 10))
        plt.imshow(data, cmap='Blues', extent=[x_min, x_min+width, y_min+height, y_min])
        plt.colorbar(label='Precipitation (mm/year)')
        plt.title(f'Precipitation Map - Seed {args.seed}')
        
    elif args.mode == 'biomes':
        data = visualizer.generate_biome_map(x_min, y_min, width, height, args.resolution)
        plt.figure(figsize=(12, 10))
        plt.imshow(data, extent=[x_min, x_min+width, y_min+height, y_min])
        plt.title(f'Biome Map - Seed {args.seed}')
        
        # Add biome legend
        legend_elements = []
        for biome_name, biome_data in visualizer.biomes.BIOMES.items():
            legend_elements.append(plt.Rectangle((0,0),1,1, facecolor=biome_data['color'], 
                                               label=biome_name.replace('_', ' ').title()))
        plt.legend(handles=legend_elements, loc='upper right', bbox_to_anchor=(1.15, 1))
    
    elif args.mode == 'rivers':
        data = visualizer.generate_rivers_map(x_min, y_min, width, height, args.resolution)
        plt.figure(figsize=(12, 10))
        plt.imshow(data, cmap='Blues', extent=[x_min, x_min+width, y_min+height, y_min])
        plt.colorbar(label='River Width (m)')
        plt.title(f'River Network - Seed {args.seed}')
        
    elif args.mode == 'overlay':
        data = visualizer.generate_overlay_map(x_min, y_min, width, height, args.resolution)
        plt.figure(figsize=(12, 10))
        plt.imshow(data, extent=[x_min, x_min+width, y_min+height, y_min])
        plt.title(f'Combined Terrain Overview - Seed {args.seed}')
        plt.text(0.02, 0.98, 'Biomes (color) + Elevation (shading) + Rivers (blue)', 
                transform=plt.gca().transAxes, verticalalignment='top', 
                bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    plt.xlabel('X (world coordinates)')
    plt.ylabel('Y (world coordinates)')
    plt.tight_layout()
    
    # Save the image
    print(f"Saving to {args.output}...")
    plt.savefig(args.output, dpi=150, bbox_inches='tight')
    print(f"Visualization saved as {args.output}")
    
    # Also show statistics
    if args.mode == 'heightmap':
        print(f"\nHeightmap Statistics:")
        print(f"  Min elevation: {np.min(data):.1f}m")
        print(f"  Max elevation: {np.max(data):.1f}m")
        print(f"  Mean elevation: {np.mean(data):.1f}m")
        print(f"  Std deviation: {np.std(data):.1f}m")
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
