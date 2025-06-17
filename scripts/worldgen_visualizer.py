#!/usr/bin/env python3
"""
ProceduralTerrain Visualization Tool
====================================

This tool generates preview images of the new ProceduralTerrain world generation system.
It helps visualize and test heightmaps, climate zones, biomes, cave systems, etc.

Usage:
    python3 worldgen_visualizer.py --seed 12345 --size 512 --output heightmap.png
    python3 worldgen_visualizer.py --seed 12345 --mode climate --size 1024
    python3 worldgen_visualizer.py --seed 12345 --mode biomes --region -500,-500,1000,1000

Author: AI Assistant & User
Date: June 2025
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import argparse
import sys
import os
from PIL import Image
import math

class WorldSeed:
    """Manages world seed and provides deterministic noise generation"""
    
    def __init__(self, seed=12345):
        self.master_seed = seed
        self.noise_seeds = {}
        
    def get_noise_seed(self, name):
        """Get deterministic seed for a specific noise layer"""
        if name not in self.noise_seeds:
            # Simple hash function to generate consistent sub-seeds
            self.noise_seeds[name] = hash((self.master_seed, name)) % (2**31)
        return self.noise_seeds[name]

class PerlinNoise:
    """Simple Perlin noise implementation for terrain generation"""
    
    def __init__(self, seed=0):
        self.seed = seed
        np.random.seed(seed)
        # Generate permutation table
        self.p = np.random.permutation(256).tolist()
        self.p += self.p  # Duplicate for easier indexing
        
    def fade(self, t):
        return t * t * t * (t * (t * 6 - 15) + 10)
    
    def lerp(self, t, a, b):
        return a + t * (b - a)
    
    def grad(self, hash_val, x, y):
        h = hash_val & 15
        u = x if h < 8 else y
        v = y if h < 4 else (x if h == 12 or h == 14 else 0)
        return (u if (h & 1) == 0 else -u) + (v if (h & 2) == 0 else -v)
    
    def noise(self, x, y):
        X = int(x) & 255
        Y = int(y) & 255
        
        x -= int(x)
        y -= int(y)
        
        u = self.fade(x)
        v = self.fade(y)
        
        A = self.p[X] + Y
        B = self.p[X + 1] + Y
        
        return self.lerp(v, 
                        self.lerp(u, self.grad(self.p[A], x, y),
                                    self.grad(self.p[B], x - 1, y)),
                        self.lerp(u, self.grad(self.p[A + 1], x, y - 1),
                                    self.grad(self.p[B + 1], x - 1, y - 1)))

class ClimateSystem:
    """Climate calculation system for temperature and precipitation"""
    
    # Voxel scale: 1 world coordinate = 0.25m (25cm voxel size)
    VOXEL_SIZE = 0.25  # meters per voxel
    
    def __init__(self, world_seed):
        self.world_seed = world_seed
        self.temp_noise = PerlinNoise(world_seed.get_noise_seed("temperature"))
        self.precip_noise = PerlinNoise(world_seed.get_noise_seed("precipitation"))
        
    def get_temperature(self, x, y, elevation=0):
        """Calculate temperature at world coordinates
        
        Args:
            x, y: World coordinates (1 unit = 0.25m)
            elevation: Elevation in meters
        """
        # Convert world coordinates to real-world meters
        real_x = x * self.VOXEL_SIZE
        real_y = y * self.VOXEL_SIZE
        
        # Base temperature from latitude (assuming y is north-south)
        latitude_factor = abs(real_y) / 10000.0  # Scale factor for latitude effect
        base_temp = 25.0 - (latitude_factor * 30.0)  # 25°C at equator, -5°C at poles
        
        # Elevation effect (-6.5°C per 1000m)
        elevation_effect = -elevation * 0.0065
        
        # Noise variation for climate complexity
        noise_variation = self.temp_noise.noise(real_x / 5000.0, real_y / 5000.0) * 15.0
        
        return base_temp + elevation_effect + noise_variation
    
    def get_precipitation(self, x, y, temperature, elevation=0):
        """Calculate precipitation at world coordinates
        
        Args:
            x, y: World coordinates (1 unit = 0.25m)
            temperature: Temperature in Celsius
            elevation: Elevation in meters
        """
        # Convert world coordinates to real-world meters
        real_x = x * self.VOXEL_SIZE
        real_y = y * self.VOXEL_SIZE
        
        # Base precipitation from noise
        base_precip = (self.precip_noise.noise(real_x / 8000.0, real_y / 8000.0) + 1) * 1000.0
        
        # Temperature effect on precipitation
        temp_factor = 1.0 if temperature > 0 else 0.3  # Less precip in cold areas
        
        # Elevation effect (orographic precipitation)
        elevation_factor = 1.0 + (elevation / 2000.0) * 0.5
        
        return base_precip * temp_factor * elevation_factor

class TerrainGenerator:
    """Heightmap and terrain feature generation"""
    
    # Voxel scale: 1 world coordinate = 0.25m (25cm voxel size)
    VOXEL_SIZE = 0.25  # meters per voxel
    MAX_ELEVATION_VOXELS = 8192  # ±8192 voxels = ±2048m
    
    def __init__(self, world_seed):
        self.world_seed = world_seed
        self.continental_noise = PerlinNoise(world_seed.get_noise_seed("continental"))
        self.mountain_noise = PerlinNoise(world_seed.get_noise_seed("mountains"))
        self.hill_noise = PerlinNoise(world_seed.get_noise_seed("hills"))
        self.detail_noise = PerlinNoise(world_seed.get_noise_seed("detail"))
        
    def get_elevation(self, x, y):
        """Calculate elevation at world coordinates with realistic distribution
        
        Args:
            x, y: World coordinates (1 unit = 0.25m)
        Returns:
            Elevation in meters (for display purposes)
        """
        # Convert world coordinates to real-world meters for noise sampling
        real_x = x * self.VOXEL_SIZE  # Convert to meters
        real_y = y * self.VOXEL_SIZE  # Convert to meters
        
        # Continental shape (large scale landmasses vs ocean basins)
        # Scale for continental features: ~50km wavelength
        continental = self.continental_noise.noise(real_x / 50000.0, real_y / 50000.0)
        
        # Mountain ranges (creates major elevation features)
        # Scale for mountain ranges: ~15km wavelength
        mountains = self.mountain_noise.noise(real_x / 15000.0, real_y / 15000.0)
        
        # Hills and valleys (regional variation)
        # Scale for hills: ~5km wavelength
        hills = self.hill_noise.noise(real_x / 5000.0, real_y / 5000.0)
        
        # Fine detail (local surface roughness)
        # Scale for details: ~1km wavelength
        detail = self.detail_noise.noise(real_x / 1000.0, real_y / 1000.0)
        
        # Realistic elevation layering (in voxels first, then convert to meters)
        # Base elevation from continental structure (-3200 to +3200 voxels)
        base_elevation_voxels = continental * 3200.0
        
        # Mountain layer - only significant where continental noise supports it
        mountain_factor = max(0, continental + 0.3)  # Mountains form on continents
        
        # Create sharp peaks using ridged noise for realistic mountain shapes
        mountain_ridged = abs(mountains) * 2.0 - 1.0  # Convert to ridged noise (-1 to 1)
        mountain_peaks = np.power(abs(mountain_ridged), 0.6) * np.sign(mountain_ridged)  # Sharp peaks, steep valleys
        mountain_elevation_voxels = mountain_peaks * mountain_factor * 4800.0  # Sharp mountain features
        
        # Hill layer - moderate everywhere
        hill_elevation_voxels = hills * 1000.0
        
        # Detail layer - fine surface variation
        detail_elevation_voxels = detail * 120.0
        
        # Combine layers with realistic scaling
        total_elevation_voxels = base_elevation_voxels + mountain_elevation_voxels + hill_elevation_voxels + detail_elevation_voxels
        
        # Soft clamping to avoid flat areas at extremes
        if total_elevation_voxels > 7200:  # Most terrain should be within ±1800m
            excess = total_elevation_voxels - 7200
            total_elevation_voxels = 7200 + excess * 0.3  # Compress extreme heights
        elif total_elevation_voxels < -7200:
            excess = total_elevation_voxels + 7200
            total_elevation_voxels = -7200 + excess * 0.3  # Compress extreme depths
        
        # Final hard clamp to voxel limits
        total_elevation_voxels = max(-self.MAX_ELEVATION_VOXELS, min(self.MAX_ELEVATION_VOXELS, total_elevation_voxels))
        
        # Convert back to meters for display
        return total_elevation_voxels * self.VOXEL_SIZE

class RiverGenerator:
    """Generate realistic river networks based on elevation and precipitation"""
    
    def __init__(self, world_seed, terrain_generator, climate_system):
        self.world_seed = world_seed
        self.terrain = terrain_generator
        self.climate = climate_system
        self.river_noise = PerlinNoise(world_seed.get_noise_seed("rivers"))
        
    def is_river(self, x, y):
        """Determine if there should be a river at this location"""
        elevation = self.terrain.get_elevation(x, y)
        
        # Rivers don't form on high mountains or below sea level
        if elevation > 1000 or elevation < -10:
            return False
            
        # Get precipitation (more rain = more likely rivers)
        temperature = self.climate.get_temperature(x, y, elevation)
        precipitation = self.climate.get_precipitation(x, y, temperature, elevation)
        
        # River likelihood based on precipitation and elevation
        precip_factor = min(1.0, precipitation / 1000.0)  # Normalize precipitation
        elevation_factor = max(0, 1000 - abs(elevation)) / 1000.0  # Favor mid-elevations
        
        # Use noise to create winding river patterns
        river_noise = abs(self.river_noise.noise(x * 0.25 * 0.001, y * 0.25 * 0.001))  # Scale for 25cm voxels
        
        # Rivers form in narrow bands (valleys)
        river_threshold = 0.95 - (precip_factor * elevation_factor * 0.3)
        
        return river_noise > river_threshold
    
    def get_river_width(self, x, y):
        """Get river width at location (0 = no river, >0 = river width in meters)"""
        if not self.is_river(x, y):
            return 0.0
            
        # River width based on elevation and precipitation
        elevation = self.terrain.get_elevation(x, y)
        temperature = self.climate.get_temperature(x, y, elevation)
        precipitation = self.climate.get_precipitation(x, y, temperature, elevation)
        
        # Wider rivers in areas with more precipitation
        base_width = 2.0 + (precipitation / 1000.0) * 8.0  # 2-10 meter width
        
        # Rivers get wider at lower elevations (accumulation effect)
        elevation_factor = max(0.1, (500 - elevation) / 500.0)
        
        return base_width * elevation_factor

class BiomeClassifier:
    """Biome classification based on temperature, precipitation, and elevation"""
    
    BIOMES = {
        'tundra': {'temp_min': -20, 'temp_max': 5, 'precip_min': 0, 'precip_max': 500, 'color': (0.7, 0.8, 0.9)},
        'taiga': {'temp_min': -10, 'temp_max': 15, 'precip_min': 300, 'precip_max': 1000, 'color': (0.2, 0.5, 0.3)},
        'temperate_forest': {'temp_min': 5, 'temp_max': 25, 'precip_min': 600, 'precip_max': 1500, 'color': (0.3, 0.7, 0.2)},
        'grassland': {'temp_min': 0, 'temp_max': 30, 'precip_min': 200, 'precip_max': 800, 'color': (0.6, 0.8, 0.3)},
        'desert': {'temp_min': 15, 'temp_max': 50, 'precip_min': 0, 'precip_max': 300, 'color': (0.9, 0.8, 0.4)},
        'tropical_forest': {'temp_min': 20, 'temp_max': 35, 'precip_min': 1200, 'precip_max': 3000, 'color': (0.1, 0.6, 0.1)},
        'savanna': {'temp_min': 20, 'temp_max': 35, 'precip_min': 500, 'precip_max': 1200, 'color': (0.8, 0.7, 0.3)},
        'alpine': {'temp_min': -15, 'temp_max': 10, 'precip_min': 500, 'precip_max': 2000, 'color': (0.5, 0.5, 0.6)},
        'ocean': {'temp_min': -5, 'temp_max': 30, 'precip_min': 0, 'precip_max': 3000, 'color': (0.1, 0.3, 0.8)},
    }
    
    def classify_biome(self, temperature, precipitation, elevation):
        """Classify biome based on climate and elevation"""
        # Ocean biome for areas below sea level
        if elevation < -50:
            return 'ocean'
            
        # Alpine biome for high elevations
        if elevation > 1500:
            return 'alpine'
            
        # Find best matching biome
        best_biome = 'grassland'  # Default fallback
        best_score = float('inf')
        
        for biome_name, biome_data in self.BIOMES.items():
            if biome_name in ['ocean', 'alpine']:
                continue
                
            # Calculate how well this biome matches the conditions
            temp_score = 0
            if temperature < biome_data['temp_min']:
                temp_score = biome_data['temp_min'] - temperature
            elif temperature > biome_data['temp_max']:
                temp_score = temperature - biome_data['temp_max']
                
            precip_score = 0
            if precipitation < biome_data['precip_min']:
                precip_score = biome_data['precip_min'] - precipitation
            elif precipitation > biome_data['precip_max']:
                precip_score = precipitation - biome_data['precip_max']
                
            total_score = temp_score + precip_score * 0.001  # Weight temperature more heavily
            
            if total_score < best_score:
                best_score = total_score
                best_biome = biome_name
                
        return best_biome

class WorldgenVisualizer:
    """Main visualization class"""
    
    def __init__(self, seed=12345):
        self.world_seed = WorldSeed(seed)
        self.climate = ClimateSystem(self.world_seed)
        self.terrain = TerrainGenerator(self.world_seed)
        self.rivers = RiverGenerator(self.world_seed, self.terrain, self.climate)
        self.biomes = BiomeClassifier()
        
    def generate_heightmap(self, x_min, y_min, width, height, resolution=1.0):
        """Generate heightmap for given region"""
        x_coords = np.linspace(x_min, x_min + width, int(width * resolution))
        y_coords = np.linspace(y_min, y_min + height, int(height * resolution))
        
        heightmap = np.zeros((len(y_coords), len(x_coords)))
        
        for i, y in enumerate(y_coords):
            for j, x in enumerate(x_coords):
                heightmap[i, j] = self.terrain.get_elevation(x, y)
                
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
    parser.add_argument('--size', type=int, default=512, help='Output image size (square)')
    parser.add_argument('--region', type=str, default='-2500,-2500,5000,5000', 
                       help='Region to visualize as x_min,y_min,width,height')
    parser.add_argument('--output', type=str, default=None, help='Output filename')
    parser.add_argument('--resolution', type=float, default=0.5, help='Sampling resolution (samples per world unit)')
    
    args = parser.parse_args()
    
    # Parse region
    try:
        x_min, y_min, width, height = map(float, args.region.split(','))
    except:
        print("Error: Invalid region format. Use: x_min,y_min,width,height")
        return 1
    
    # Create visualizer
    print(f"Initializing world generator with seed {args.seed}...")
    visualizer = WorldgenVisualizer(args.seed)
    
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
        plt.imshow(data, cmap='terrain', extent=[x_min, x_min+width, y_min+height, y_min])
        plt.colorbar(label='Elevation (m)')
        plt.title(f'Heightmap - Seed {args.seed}')
        
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
