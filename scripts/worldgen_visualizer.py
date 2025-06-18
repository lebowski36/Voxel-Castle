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
        total_samples = sample_width * sample_height
        
        # Additional safety check (should have been caught in main, but double-check)
        if total_samples > 10_000_000:
            raise ValueError(f"Too many samples requested: {total_samples:,}. Maximum is 10,000,000. Use lower resolution.")
        
        print(f"Sampling {sample_width}x{sample_height} points for {width}x{height} world units...")
        
        try:
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
            
        except MemoryError as e:
            print(f"ERROR: Out of memory when generating {sample_width}x{sample_height} heightmap!")
            print(f"Try using a lower resolution (current: {resolution})")
            print(f"Suggested resolution: {0.5 * resolution:.3f} or lower")
            raise
        except Exception as e:
            print(f"ERROR: Failed to generate heightmap: {e}")
            raise

    def generate_heightmap_with_rivers(self, x_min, y_min, width, height, resolution):
        """Generates a heightmap with river carving using the C++ backend."""
        
        # Ensure width and height are integers
        width = int(width)
        height = int(height)
        
        # Calculate actual sampling dimensions based on resolution
        sample_width = int(width * resolution)
        sample_height = int(height * resolution)
        total_samples = sample_width * sample_height
        
        # Additional safety check
        if total_samples > 10_000_000:
            raise ValueError(f"Too many samples requested: {total_samples:,}. Maximum is 10,000,000. Use lower resolution.")
        
        print(f"Sampling {sample_width}x{sample_height} points for heightmap with rivers...")
        
        try:
            # Create coordinate arrays with proper sampling
            x_coords = np.linspace(x_min, x_min + width, sample_width, dtype=np.float32)
            y_coords = np.linspace(y_min, y_min + height, sample_height, dtype=np.float32)
            xx, yy = np.meshgrid(x_coords, y_coords)
            
            # Flatten arrays for the C++ function
            x_flat = xx.flatten()
            y_flat = yy.flatten()

            print("Generating heightmap with rivers using C++ backend...")
            
            # Call the C++ function that includes river carving
            height_values = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, y_flat, self.seed)
            
            # Reshape the flat array back into a 2D map
            heightmap = height_values.reshape((sample_height, sample_width))
            
            print("Heightmap with rivers generation complete.")
            return heightmap
            
        except MemoryError as e:
            print(f"ERROR: Out of memory when generating {sample_width}x{sample_height} heightmap with rivers!")
            print(f"Try using a lower resolution (current: {resolution})")
            print(f"Suggested resolution: {0.5 * resolution:.3f} or lower")
            raise
        except Exception as e:
            print(f"ERROR: Failed to generate heightmap with rivers: {e}")
            raise

    def generate_climate_map(self, x_min, y_min, width, height, resolution=1.0, mode='temperature'):
        """Generate climate map using C++ backend"""
        # Ensure width and height are integers
        width = int(width)
        height = int(height)
        
        # Calculate actual sampling dimensions based on resolution
        sample_width = int(width * resolution)
        sample_height = int(height * resolution)
        
        print(f"Generating {mode} map for {sample_width}x{sample_height} points...")
        
        # Create coordinate arrays with proper sampling
        x_coords = np.linspace(x_min, x_min + width, sample_width, dtype=np.float32)
        y_coords = np.linspace(y_min, y_min + height, sample_height, dtype=np.float32)
        xx, yy = np.meshgrid(x_coords, y_coords)
        
        # Flatten arrays for the C++ function
        x_flat = xx.flatten()
        y_flat = yy.flatten()

        print("Generating heightmap for climate calculation...")
        # First get elevations using the C++ backend
        elevations = worldgen_cpp.generate_terrain_heightmap(x_flat, y_flat, self.seed)
        
        print(f"Generating {mode} data with C++ backend...")
        # Generate climate data
        if mode == 'temperature':
            climate_data = worldgen_cpp.generate_climate_temperature(x_flat, y_flat, elevations, self.seed)
        elif mode == 'humidity':
            climate_data = worldgen_cpp.generate_climate_humidity(x_flat, y_flat, elevations, self.seed)
        elif mode == 'precipitation':
            climate_data = worldgen_cpp.generate_climate_precipitation(x_flat, y_flat, elevations, self.seed)
        else:
            raise ValueError(f"Unknown climate mode: {mode}")
        
        # Reshape back to 2D array
        result = climate_data.reshape((sample_height, sample_width))
        
        print(f"{mode.capitalize()} map generation complete.")
        return result
    
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
        """Generate river network map using the new hierarchical fractal river system"""
        # Ensure width and height are integers
        width = int(width)
        height = int(height)
        
        # Calculate actual sampling dimensions based on resolution
        sample_width = int(width * resolution)
        sample_height = int(height * resolution)
        total_samples = sample_width * sample_height
        
        # Additional safety check
        if total_samples > 10_000_000:
            raise ValueError(f"Too many samples requested: {total_samples:,}. Maximum is 10,000,000. Use lower resolution.")
        
        print(f"Generating river network for {sample_width}x{sample_height} points...")
        
        try:
            # Create coordinate arrays with proper sampling
            x_coords = np.linspace(x_min, x_min + width, sample_width, dtype=np.float32)
            y_coords = np.linspace(y_min, y_min + height, sample_height, dtype=np.float32)
            xx, yy = np.meshgrid(x_coords, y_coords)
            
            # Flatten arrays for the C++ function
            x_flat = xx.flatten()
            y_flat = yy.flatten()

            print("Generating river flow data with new hierarchical fractal system...")
            
            # Use the new comprehensive river flow function
            river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_flat, y_flat, self.seed)
            
            # Get river widths as well for better visualization
            river_widths = worldgen_cpp.generate_river_width(x_flat, y_flat, self.seed)
            
            # Reshape both arrays back to 2D
            flow_map = river_flow.reshape((sample_height, sample_width))
            width_map = river_widths.reshape((sample_height, sample_width))
            
            # Combine flow and width for better visualization
            # Use flow strength as the base, but enhance with width information
            river_map = flow_map.copy()
            
            # Apply river threshold - only show significant rivers
            river_threshold = 30.0  # Matches RIVER_THRESHOLD from C++
            river_map[river_map < river_threshold] = 0
            
            print("River network generation complete.")
            return river_map
            
        except MemoryError as e:
            print(f"ERROR: Out of memory when generating {sample_width}x{sample_height} river map!")
            print(f"Try using a lower resolution (current: {resolution})")
            print(f"Suggested resolution: {0.5 * resolution:.3f} or lower")
            raise
        except Exception as e:
            print(f"ERROR: Failed to generate river map: {e}")
            raise
    
    def generate_overlay_map(self, x_min, y_min, width, height, resolution=1.0):
        """Generate combined heightmap with rivers overlay using new river system"""
        # Ensure width and height are integers
        width = int(width)
        height = int(height)
        
        # Calculate actual sampling dimensions based on resolution
        sample_width = int(width * resolution)
        sample_height = int(height * resolution)
        
        print(f"Generating combined overlay map for {sample_width}x{sample_height} points...")
        
        # Create coordinate arrays with proper sampling
        x_coords = np.linspace(x_min, x_min + width, sample_width, dtype=np.float32)
        y_coords = np.linspace(y_min, y_min + height, sample_height, dtype=np.float32)
        xx, yy = np.meshgrid(x_coords, y_coords)
        
        # Flatten arrays for the C++ function
        x_flat = xx.flatten()
        y_flat = yy.flatten()

        print("Generating base heightmap...")
        # Get heightmap using C++ backend
        heightmap = worldgen_cpp.generate_terrain_heightmap(x_flat, y_flat, self.seed)
        heightmap_2d = heightmap.reshape((sample_height, sample_width))
        
        print("Generating river network overlay...")
        # Get river data using new system
        river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_flat, y_flat, self.seed)
        river_flow_2d = river_flow.reshape((sample_height, sample_width))
        
        # Create RGB overlay map
        overlay_map = np.zeros((sample_height, sample_width, 3))
        
        # Base terrain coloring based on elevation
        elevation_normalized = (heightmap_2d + 2048) / 4096.0  # Normalize to 0-1
        elevation_normalized = np.clip(elevation_normalized, 0, 1)
        
        # Create terrain color gradient (brown to white for mountains)
        for i in range(sample_height):
            for j in range(sample_width):
                elev = elevation_normalized[i, j]
                
                # Terrain base color (brown lowlands to white peaks)
                if elev < 0.3:  # Lowlands - green
                    base_color = np.array([0.2 + elev*0.4, 0.6 + elev*0.3, 0.1 + elev*0.2])
                elif elev < 0.7:  # Hills - brown
                    base_color = np.array([0.5 + elev*0.3, 0.4 + elev*0.2, 0.2 + elev*0.1])
                else:  # Mountains - gray to white
                    base_color = np.array([0.6 + elev*0.4, 0.6 + elev*0.4, 0.6 + elev*0.4])
                
                # Apply river overlay (bright blue)
                river_strength = river_flow_2d[i, j]
                if river_strength > 30.0:  # River threshold
                    river_intensity = min(1.0, np.log(river_strength) / 8.0)  # Logarithmic scaling
                    river_color = np.array([0.1, 0.4, 1.0])  # Bright blue
                    base_color = base_color * (1 - river_intensity) + river_color * river_intensity
                
                overlay_map[i, j] = np.clip(base_color, 0, 1)
        
        print("Overlay map generation complete.")
        return overlay_map
    
    def add_scale_legend(self, ax, width, height, units='m'):
        """Add a scale legend showing real-world distances to the map"""
        # Calculate appropriate scale bar length (about 1/8 of the map width)
        scale_length = width / 8
        
        # Round to nice numbers
        if scale_length >= 1000:
            # Use kilometers
            scale_km = round(scale_length / 1000)
            if scale_km < 1:
                scale_km = 1
            scale_length = scale_km * 1000
            scale_text = f"{scale_km} km"
        else:
            # Use meters
            scale_m = round(scale_length)
            if scale_m < 100:
                scale_m = 100
            scale_length = scale_m
            scale_text = f"{scale_m} m"
        
        # Position the scale bar in the bottom-left corner (overlaid on map)
        x_pos = ax.get_xlim()[0] + width * 0.05  # 5% from left edge
        y_pos = ax.get_ylim()[0] + height * 0.08  # 8% from bottom edge (overlaid)
        
        # Draw the scale bar
        ax.plot([x_pos, x_pos + scale_length], [y_pos, y_pos], 
                'k-', linewidth=3, solid_capstyle='butt')
        
        # Add end caps
        bar_height = height * 0.01  # 1% of map height
        ax.plot([x_pos, x_pos], [y_pos - bar_height/2, y_pos + bar_height/2], 
                'k-', linewidth=3)
        ax.plot([x_pos + scale_length, x_pos + scale_length], 
                [y_pos - bar_height/2, y_pos + bar_height/2], 'k-', linewidth=3)
        
        # Add text label (overlaid on map with better background)
        ax.text(x_pos + scale_length/2, y_pos + bar_height*3, scale_text, 
                ha='center', va='bottom', fontsize=10, fontweight='bold',
                bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.9, edgecolor='black'))
        
        print(f"Added scale legend: {scale_text}")

    def generate_noise_map(self, x_min, y_min, width, height, resolution=1.0, scale='continental'):
        """Generate noise map for a specific scale"""
        # Ensure width and height are integers
        width = int(width)
        height = int(height)
        
        # Calculate actual sampling dimensions based on resolution
        sample_width = int(width * resolution)
        sample_height = int(height * resolution)
        
        print(f"Generating {scale} noise map for {sample_width}x{sample_height} points...")
        
        # Create coordinate arrays
        x_coords = np.linspace(x_min, x_min + width, sample_width, dtype=np.float32)
        y_coords = np.linspace(y_min, y_min + height, sample_height, dtype=np.float32)
        xx, yy = np.meshgrid(x_coords, y_coords)
        
        # Flatten arrays for the C++ function
        x_flat = xx.flatten()
        y_flat = yy.flatten()

        # Generate noise data
        if scale == 'continental':
            noise_data = worldgen_cpp.generate_continental_noise_batch(x_flat, y_flat, self.seed)
        elif scale == 'regional':
            noise_data = worldgen_cpp.generate_regional_noise_batch(x_flat, y_flat, self.seed)
        else:
            raise ValueError(f"Unknown noise scale: {scale}")
        
        # Reshape back to 2D array
        result = noise_data.reshape((sample_height, sample_width))
        
        print(f"{scale.capitalize()} noise map generation complete.")
        return result

    def generate_slope_map(self, x_min, y_min, width, height, resolution=1.0):
        """Generate slope/gradient map from heightmap"""
        # Get heightmap first
        heightmap = self.generate_heightmap(x_min, y_min, width, height, resolution)
        
        # Calculate gradients
        grad_y, grad_x = np.gradient(heightmap)
        slope = np.sqrt(grad_x**2 + grad_y**2)
        
        print("Slope map generation complete.")
        return slope

    def generate_aspect_map(self, x_min, y_min, width, height, resolution=1.0):
        """Generate aspect (slope direction) map from heightmap"""
        # Get heightmap first
        heightmap = self.generate_heightmap(x_min, y_min, width, height, resolution)
        
        # Calculate gradients
        grad_y, grad_x = np.gradient(heightmap)
        aspect = np.arctan2(grad_y, grad_x) * 180 / np.pi
        
        print("Aspect map generation complete.")
        return aspect

    def generate_comprehensive_analysis(self, x_min, y_min, width, height, resolution=1.0):
        """Generate comprehensive multi-panel terrain analysis"""
        print("=== COMPREHENSIVE TERRAIN ANALYSIS ===")
        
        # Generate all available data
        heightmap = self.generate_heightmap(x_min, y_min, width, height, resolution)
        heightmap_with_rivers = self.generate_heightmap_with_rivers(x_min, y_min, width, height, resolution)
        rivers = self.generate_rivers_map(x_min, y_min, width, height, resolution)
        temperature = self.generate_climate_map(x_min, y_min, width, height, resolution, 'temperature')
        humidity = self.generate_climate_map(x_min, y_min, width, height, resolution, 'humidity')
        precipitation = self.generate_climate_map(x_min, y_min, width, height, resolution, 'precipitation')
        continental_noise = self.generate_noise_map(x_min, y_min, width, height, resolution, 'continental')
        regional_noise = self.generate_noise_map(x_min, y_min, width, height, resolution, 'regional')
        slope = self.generate_slope_map(x_min, y_min, width, height, resolution)
        
        return {
            'heightmap': heightmap,
            'heightmap_with_rivers': heightmap_with_rivers,
            'rivers': rivers,
            'temperature': temperature,
            'humidity': humidity,
            'precipitation': precipitation,
            'continental_noise': continental_noise,
            'regional_noise': regional_noise,
            'slope': slope
        }

    def plot_comprehensive_analysis(self, data_dict, x_min, y_min, width, height, seed, output_filename):
        """Create comprehensive multi-panel visualization"""
        # Create figure with subplots (4x2 grid)
        fig, axes = plt.subplots(3, 3, figsize=(24, 18))
        fig.suptitle(f'Comprehensive Terrain Analysis - Seed {seed}', fontsize=20, y=0.98)
        
        # Extent for all plots
        extent = [x_min, x_min+width, y_min+height, y_min]
        
        # 1. Heightmap with Rivers (top-left) - This is the main terrain with river carving
        ax = axes[0, 0]
        from matplotlib.colors import TwoSlopeNorm
        import matplotlib.colors as mcolors
        norm = TwoSlopeNorm(vmin=-2048, vcenter=0, vmax=2048)
        colors_below = plt.cm.Blues_r(np.linspace(0.3, 1, 128))
        colors_above = plt.cm.terrain(np.linspace(0.35, 1, 128))
        all_colors = np.vstack((colors_below, colors_above))
        sea_level_cmap = mcolors.LinearSegmentedColormap.from_list('sea_level', all_colors)
        
        im1 = ax.imshow(data_dict['heightmap_with_rivers'], cmap=sea_level_cmap, extent=extent, norm=norm)
        ax.set_title('Terrain with River Carving', fontsize=14, fontweight='bold')
        cbar1 = plt.colorbar(im1, ax=ax, shrink=0.8)
        cbar1.set_label('Elevation (m)')
        cbar1.ax.axhline(y=0, color='white', linewidth=2)
        self.add_scale_legend(ax, width, height)
        
        # 2. Temperature (top-center)
        ax = axes[0, 1]
        im2 = ax.imshow(data_dict['temperature'], cmap='coolwarm', extent=extent)
        ax.set_title('Temperature Distribution', fontsize=14, fontweight='bold')
        cbar2 = plt.colorbar(im2, ax=ax, shrink=0.8)
        cbar2.set_label('Temperature (°C)')
        
        # 3. Humidity (top-right)
        ax = axes[0, 2]
        im3 = ax.imshow(data_dict['humidity'], cmap='Blues', extent=extent)
        ax.set_title('Humidity Distribution', fontsize=14, fontweight='bold')
        cbar3 = plt.colorbar(im3, ax=ax, shrink=0.8)
        cbar3.set_label('Humidity (0-1)')
        
        # 4. Precipitation (middle-left)
        ax = axes[1, 0]
        im4 = ax.imshow(data_dict['precipitation'], cmap='YlGnBu', extent=extent)
        ax.set_title('Annual Precipitation', fontsize=14, fontweight='bold')
        cbar4 = plt.colorbar(im4, ax=ax, shrink=0.8)
        cbar4.set_label('Precipitation (mm/year)')
        
        # 5. Continental Noise (middle-center)
        ax = axes[1, 1]
        im5 = ax.imshow(data_dict['continental_noise'], cmap='RdGy_r', extent=extent)
        ax.set_title('Continental-Scale Noise', fontsize=14, fontweight='bold')
        cbar5 = plt.colorbar(im5, ax=ax, shrink=0.8)
        cbar5.set_label('Noise Value')
        
        # 6. Regional Noise (middle-right)
        ax = axes[1, 2]
        im6 = ax.imshow(data_dict['regional_noise'], cmap='RdGy_r', extent=extent)
        ax.set_title('Regional-Scale Noise', fontsize=14, fontweight='bold')
        cbar6 = plt.colorbar(im6, ax=ax, shrink=0.8)
        cbar6.set_label('Noise Value')
        
        # 7. Slope (bottom-left)
        ax = axes[2, 0]
        im7 = ax.imshow(data_dict['slope'], cmap='plasma', extent=extent)
        ax.set_title('Terrain Slope', fontsize=14, fontweight='bold')
        cbar7 = plt.colorbar(im7, ax=ax, shrink=0.8)
        cbar7.set_label('Slope (gradient magnitude)')
        
        # 8. Rivers Network (bottom-center)
        ax = axes[2, 1]
        # Create a visualization that shows rivers clearly
        river_vis = np.copy(data_dict['heightmap_with_rivers'])
        # Highlight areas where rivers have carved the terrain
        river_strength = data_dict['rivers']
        # Make river areas more visible by darkening them
        river_mask = river_strength > 30.0  # River threshold from the design
        river_vis[river_mask] = river_vis[river_mask] - 50  # Darken river areas
        
        im8 = ax.imshow(river_vis, cmap=sea_level_cmap, extent=extent, norm=norm)
        ax.set_title('River Networks (Highlighted)', fontsize=14, fontweight='bold')
        cbar8 = plt.colorbar(im8, ax=ax, shrink=0.8)
        cbar8.set_label('Elevation (m) - Rivers Darkened')
        
        # 9. Statistics Summary (bottom-right)
        ax = axes[2, 2]
        ax.axis('off')  # Turn off axis for text display
        
        # Calculate statistics
        river_flow_data = data_dict['rivers']
        rivers_present = np.sum(river_flow_data > 30.0)  # Using river threshold from design
        total_points = river_flow_data.size
        river_coverage = rivers_present / total_points * 100
        
        stats_text = f"""TERRAIN STATISTICS
        
ELEVATION:
• Min: {np.min(data_dict['heightmap']):.1f}m
• Max: {np.max(data_dict['heightmap']):.1f}m  
• Mean: {np.mean(data_dict['heightmap']):.1f}m
• Std Dev: {np.std(data_dict['heightmap']):.1f}m
• Sea Level Coverage: {np.sum(data_dict['heightmap'] < 0) / data_dict['heightmap'].size * 100:.1f}%

RIVERS:
• River Coverage: {river_coverage:.2f}%
• Max Flow Strength: {np.max(river_flow_data):.1f}
• Mean Flow (rivers only): {np.mean(river_flow_data[river_flow_data > 30.0]) if rivers_present > 0 else 0:.1f}
• Total River Points: {rivers_present:,}

CLIMATE:
• Temperature Range: {np.min(data_dict['temperature']):.1f}°C to {np.max(data_dict['temperature']):.1f}°C
• Mean Temperature: {np.mean(data_dict['temperature']):.1f}°C
• Mean Humidity: {np.mean(data_dict['humidity']):.2f}
• Mean Precipitation: {np.mean(data_dict['precipitation']):.0f}mm/year

TERRAIN COMPLEXITY:
• Mean Slope: {np.mean(data_dict['slope']):.2f}
• Max Slope: {np.max(data_dict['slope']):.2f}
• Continental Noise Range: {np.min(data_dict['continental_noise']):.2f} to {np.max(data_dict['continental_noise']):.2f}
• Regional Noise Range: {np.min(data_dict['regional_noise']):.2f} to {np.max(data_dict['regional_noise']):.2f}

MAP INFO:
• Seed: {seed}
• Region: {width/1000:.1f}km × {height/1000:.1f}km
• Resolution: {len(data_dict['heightmap'][0])}×{len(data_dict['heightmap'])} points"""
        
        ax.text(0.05, 0.95, stats_text, transform=ax.transAxes, fontsize=11, 
                verticalalignment='top', fontfamily='monospace',
                bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgray', alpha=0.8))
        
        # Add axis labels to all plots
        for i in range(3):
            for j in range(2):  # Skip the stats panel
                if i == 2 and j == 1:  # Skip stats panel
                    continue
                axes[i, j].set_xlabel('X (world coordinates)')
                axes[i, j].set_ylabel('Y (world coordinates)')
        
        # Adjust layout
        plt.tight_layout(rect=[0, 0, 1, 0.97])
        
        # Save the comprehensive analysis
        print(f"Saving comprehensive analysis to {output_filename}...")
        plt.savefig(output_filename, dpi=150, bbox_inches='tight')
        print(f"Comprehensive terrain analysis saved!")
        
        return fig

def main():
    parser = argparse.ArgumentParser(description='ProceduralTerrain Visualization Tool')
    parser.add_argument('--seed', type=int, default=12345, help='World seed')
    parser.add_argument('--mode', choices=['heightmap', 'heightmap_rivers', 'temperature', 'humidity', 'precipitation', 'biomes', 'rivers', 'overlay', 'comprehensive'], 
                       default='comprehensive', help='Visualization mode')
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
    
    # CRITICAL: Validate resolution and memory usage to prevent crashes
    sample_width = int(width * args.resolution)
    sample_height = int(height * args.resolution)
    total_samples = sample_width * sample_height
    
    # Calculate estimated memory usage (rough estimate: 4 bytes per float + overhead)
    estimated_memory_mb = (total_samples * 4 * 3) / (1024 * 1024)  # 3 arrays (coords + result)
    
    # Set reasonable limits
    MAX_SAMPLES = 10_000_000  # 10 million samples max
    MAX_MEMORY_MB = 500  # 500MB max memory usage
    
    if total_samples >= MAX_SAMPLES:
        # Calculate suggested resolution with safety margin to account for int() rounding
        suggested_resolution = math.sqrt((MAX_SAMPLES - 1) / (width * height))
        print(f"ERROR: Resolution too high!")
        print(f"  Requested: {sample_width}x{sample_height} = {total_samples:,} samples")
        print(f"  Maximum allowed: {MAX_SAMPLES:,} samples")
        print(f"  Current resolution: {args.resolution}")
        print(f"  Suggested max resolution: {suggested_resolution:.3f}")
        print()
        print(f"TIP: Use resolution {suggested_resolution:.3f} or lower (e.g., --resolution {suggested_resolution:.3f})")
        return 1
    
    if estimated_memory_mb > MAX_MEMORY_MB:
        # Calculate suggested resolution with safety margin
        suggested_resolution = math.sqrt((MAX_MEMORY_MB * 1024 * 1024 - 1024) / (width * height * 4 * 3))
        print(f"ERROR: Estimated memory usage too high!")
        print(f"  Estimated memory: {estimated_memory_mb:.1f} MB")
        print(f"  Maximum allowed: {MAX_MEMORY_MB} MB")
        print(f"  Suggested max resolution: {suggested_resolution:.3f}")
        print()
        print(f"TIP: Use resolution {suggested_resolution:.3f} or lower (e.g., --resolution {suggested_resolution:.3f})")
        return 1
    
    # Warn if resolution might be slow
    if total_samples > 1_000_000:  # 1 million samples
        print(f"WARNING: Large resolution detected ({sample_width}x{sample_height} = {total_samples:,} samples)")
        print(f"         Estimated memory: {estimated_memory_mb:.1f} MB")
        print(f"         This may take several minutes...")
        print()
    
    # Show sampling info
    print(f"Sampling resolution: {sample_width}x{sample_height} points ({total_samples:,} total)")
    print(f"Memory estimate: {estimated_memory_mb:.1f} MB")
    
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
        
        # Add scale legend
        visualizer.add_scale_legend(plt.gca(), width, height)
        
        # Print statistics with fixed reference
        print(f"\nHeightmap Statistics:")
        print(f"  Min elevation: {np.min(data):.1f}m")
        print(f"  Max elevation: {np.max(data):.1f}m") 
        print(f"  Mean elevation: {np.mean(data):.1f}m")
        print(f"  Std deviation: {np.std(data):.1f}m")
        print(f"  Sea level (0m) coverage: {np.sum(data < 0) / data.size * 100:.1f}%")
        
    elif args.mode == 'heightmap_rivers':
        # Generate heightmap with river carving integrated
        width = int(width)
        height = int(height)
        sample_width = int(width * args.resolution)
        sample_height = int(height * args.resolution)
        
        print(f"Generating heightmap with river carving for {sample_width}x{sample_height} points...")
        
        # Create coordinate arrays
        x_coords = np.linspace(x_min, x_min + width, sample_width, dtype=np.float32)
        y_coords = np.linspace(y_min, y_min + height, sample_height, dtype=np.float32)
        xx, yy = np.meshgrid(x_coords, y_coords)
        x_flat = xx.flatten()
        y_flat = yy.flatten()
        
        # Generate heightmap with river carving using C++ backend
        data = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_flat, y_flat, args.seed)
        data = data.reshape((sample_height, sample_width))
        
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
        plt.title(f'Heightmap with River Carving - Seed {args.seed} (Sea Level = 0m)')
        
        # Add scale legend
        visualizer.add_scale_legend(plt.gca(), width, height)
        
        # Print statistics
        print(f"\nHeightmap with River Carving Statistics:")
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
        visualizer.add_scale_legend(plt.gca(), width, height)
        
    elif args.mode == 'humidity':
        data = visualizer.generate_climate_map(x_min, y_min, width, height, args.resolution, 'humidity')
        plt.figure(figsize=(12, 10))
        plt.imshow(data, cmap='viridis', extent=[x_min, x_min+width, y_min+height, y_min])
        plt.colorbar(label='Humidity (0-1)')
        plt.title(f'Humidity Map - Seed {args.seed}')
        visualizer.add_scale_legend(plt.gca(), width, height)
        
    elif args.mode == 'precipitation':
        data = visualizer.generate_climate_map(x_min, y_min, width, height, args.resolution, 'precipitation')
        plt.figure(figsize=(12, 10))
        plt.imshow(data, cmap='Blues', extent=[x_min, x_min+width, y_min+height, y_min])
        plt.colorbar(label='Precipitation (mm/year)')
        plt.title(f'Precipitation Map - Seed {args.seed}')
        visualizer.add_scale_legend(plt.gca(), width, height)
        
    elif args.mode == 'biomes':
        data = visualizer.generate_biome_map(x_min, y_min, width, height, args.resolution)
        plt.figure(figsize=(12, 10))
        plt.imshow(data, extent=[x_min, x_min+width, y_min+height, y_min])
        plt.title(f'Biome Map - Seed {args.seed}')
        visualizer.add_scale_legend(plt.gca(), width, height)
        
        # Add biome legend
        legend_elements = []
        for biome_name, biome_data in visualizer.biomes.BIOMES.items():
            legend_elements.append(plt.Rectangle((0,0),1,1, facecolor=biome_data['color'], 
                                               label=biome_name.replace('_', ' ').title()))
        plt.legend(handles=legend_elements, loc='upper right', bbox_to_anchor=(1.15, 1))
    
    elif args.mode == 'rivers':
        data = visualizer.generate_rivers_map(x_min, y_min, width, height, args.resolution)
        plt.figure(figsize=(12, 10))
        
        # Use logarithmic scale for better river visualization
        data_log = np.log10(data + 1)  # Add 1 to avoid log(0)
        
        # Create custom colormap for rivers (white background, blue rivers)
        river_cmap = plt.cm.Blues.copy()
        river_cmap.set_under('white')  # Set background to white
        
        plt.imshow(data_log, cmap=river_cmap, extent=[x_min, x_min+width, y_min+height, y_min], 
                  vmin=np.log10(30), vmax=np.max(data_log))  # Start from river threshold
        
        # Create custom colorbar with original values
        cbar = plt.colorbar(label='River Flow Strength (log scale)')
        
        # Update colorbar ticks to show original values
        log_ticks = cbar.get_ticks()
        original_ticks = [f"{10**tick:.0f}" if tick > 0 else "0" for tick in log_ticks]
        cbar.set_ticklabels(original_ticks)
        
        plt.title(f'River Network (New Hierarchical Fractal System) - Seed {args.seed}')
        visualizer.add_scale_legend(plt.gca(), width, height)
        
        # Print river statistics
        river_count = np.sum(data > 30)  # Count river pixels above threshold
        total_pixels = data.size
        river_coverage = river_count / total_pixels * 100
        print(f"\nRiver Network Statistics:")
        print(f"  River pixels (>30 strength): {river_count}")
        print(f"  Total pixels: {total_pixels}")
        print(f"  River coverage: {river_coverage:.2f}%")
        print(f"  Max river strength: {np.max(data):.1f}")
        print(f"  Mean river strength (rivers only): {np.mean(data[data > 30]):.1f}")
        
    elif args.mode == 'overlay':
        data = visualizer.generate_overlay_map(x_min, y_min, width, height, args.resolution)
        plt.figure(figsize=(12, 10))
        plt.imshow(data, extent=[x_min, x_min+width, y_min+height, y_min])
        plt.title(f'Combined Terrain Overview - Seed {args.seed}')
        visualizer.add_scale_legend(plt.gca(), width, height)
        plt.text(0.02, 0.98, 'Biomes (color) + Elevation (shading) + Rivers (blue)', 
                transform=plt.gca().transAxes, verticalalignment='top', 
                bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    elif args.mode == 'comprehensive':
        # Generate comprehensive analysis
        data_dict = visualizer.generate_comprehensive_analysis(x_min, y_min, width, height, args.resolution)
        
        # Create comprehensive visualization
        fig = visualizer.plot_comprehensive_analysis(data_dict, x_min, y_min, width, height, args.seed, args.output)
        
        # Print comprehensive statistics
        print(f"\n=== COMPREHENSIVE TERRAIN ANALYSIS COMPLETE ===")
        print(f"Generated 8 different visualizations:")
        print(f"  1. Heightmap with sea-level coloring")
        print(f"  2. Temperature distribution") 
        print(f"  3. Humidity distribution")
        print(f"  4. Annual precipitation")
        print(f"  5. Continental-scale noise patterns")
        print(f"  6. Regional-scale noise patterns")
        print(f"  7. Terrain slope analysis")
        print(f"  8. Slope aspect (direction) analysis")
        print(f"  9. Statistical summary panel")
        
        # Early return to skip the individual plot formatting
        return 0
    
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
