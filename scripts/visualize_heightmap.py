#!/usr/bin/env python3
"""
Simple heightmap visualization script for the seed system test.
Generates a visual representation of the terrain heightmap.
"""

import csv
import matplotlib.pyplot as plt
import numpy as np
import argparse
import os

def load_heightmap(filename):
    """Load heightmap data from CSV file."""
    x_coords = []
    z_coords = []
    heights = []
    terrain_values = []
    cave_values = []
    
    with open(filename, 'r') as file:
        reader = csv.DictReader(file)
        for row in reader:
            x_coords.append(int(row['x']))
            z_coords.append(int(row['z']))
            heights.append(int(row['height']))
            terrain_values.append(float(row['terrain']))
            cave_values.append(float(row['cave']))
    
    return x_coords, z_coords, heights, terrain_values, cave_values

def create_2d_grid(x_coords, z_coords, values):
    """Convert coordinate lists to 2D grid."""
    max_x = max(x_coords) + 1
    max_z = max(z_coords) + 1
    
    grid = np.zeros((max_z, max_x))
    for i, (x, z) in enumerate(zip(x_coords, z_coords)):
        grid[z, x] = values[i]
    
    return grid

def visualize_heightmap(filename):
    """Create visualizations of the heightmap data."""
    print(f"Loading heightmap from {filename}...")
    
    try:
        x_coords, z_coords, heights, terrain_values, cave_values = load_heightmap(filename)
    except FileNotFoundError:
        print(f"Error: {filename} not found. Please run the seed system test first.")
        return
    except Exception as e:
        print(f"Error loading heightmap: {e}")
        return
    
    # Create 2D grids
    height_grid = create_2d_grid(x_coords, z_coords, heights)
    terrain_grid = create_2d_grid(x_coords, z_coords, terrain_values)
    cave_grid = create_2d_grid(x_coords, z_coords, cave_values)
    
    # Create visualizations
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle('World Seed System - Terrain Generation Test', fontsize=16)
    
    # Height map
    im1 = axes[0, 0].imshow(height_grid, cmap='terrain', interpolation='bilinear')
    axes[0, 0].set_title('Height Map (0-100)')
    axes[0, 0].set_xlabel('X Coordinate')
    axes[0, 0].set_ylabel('Z Coordinate')
    plt.colorbar(im1, ax=axes[0, 0])
    
    # Terrain noise values
    im2 = axes[0, 1].imshow(terrain_grid, cmap='viridis', interpolation='bilinear')
    axes[0, 1].set_title('Terrain Noise (0.0-1.0)')
    axes[0, 1].set_xlabel('X Coordinate')
    axes[0, 1].set_ylabel('Z Coordinate')
    plt.colorbar(im2, ax=axes[0, 1])
    
    # Cave noise values
    im3 = axes[1, 0].imshow(cave_grid, cmap='plasma', interpolation='bilinear')
    axes[1, 0].set_title('Cave Noise (0.0-1.0)')
    axes[1, 0].set_xlabel('X Coordinate')
    axes[1, 0].set_ylabel('Z Coordinate')
    plt.colorbar(im3, ax=axes[1, 0])
    
    # 3D-style height visualization
    X, Z = np.meshgrid(range(height_grid.shape[1]), range(height_grid.shape[0]))
    contour = axes[1, 1].contour(X, Z, height_grid, levels=10, cmap='terrain')
    axes[1, 1].clabel(contour, inline=True, fontsize=8)
    axes[1, 1].set_title('Height Contours')
    axes[1, 1].set_xlabel('X Coordinate')
    axes[1, 1].set_ylabel('Z Coordinate')
    
    plt.tight_layout()
    
    # Save visualization
    output_file = 'heightmap_visualization.png'
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    print(f"Visualization saved to {output_file}")
    
    # Show statistics
    print(f"\nTerrain Statistics:")
    print(f"  Height range: {np.min(heights)} - {np.max(heights)}")
    print(f"  Height mean: {np.mean(heights):.2f}")
    print(f"  Height std: {np.std(heights):.2f}")
    print(f"  Terrain noise range: {np.min(terrain_values):.6f} - {np.max(terrain_values):.6f}")
    print(f"  Cave noise range: {np.min(cave_values):.6f} - {np.max(cave_values):.6f}")
    
    # Check for patterns that might indicate issues
    height_array = np.array(heights)
    if np.std(height_array) < 1.0:
        print("WARNING: Very low height variation - possible seed issue")
    
    if len(set(heights)) < len(heights) * 0.8:
        print("WARNING: Many repeated height values - possible pattern issue")
    
    # Display the plot
    plt.show()

def main():
    parser = argparse.ArgumentParser(description='Visualize heightmap data from seed system test')
    parser.add_argument('--file', '-f', default='heightmap_test.csv', 
                       help='CSV file containing heightmap data (default: heightmap_test.csv)')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.file):
        print(f"Heightmap file '{args.file}' not found.")
        print("Please run the seed system test first:")
        print("  cd build && make && ./bin/seed_system_test")
        return
    
    visualize_heightmap(args.file)

if __name__ == "__main__":
    main()
