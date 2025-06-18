#!/usr/bin/env python3
"""
Debug script to analyze river values at different scales
"""

import sys
import os
sys.path.append('scripts')
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import matplotlib.pyplot as plt
import worldgen_cpp
from worldgen_visualizer import WorldVisualizer

def analyze_river_values(width, height, resolution, seed=12345):
    """Analyze river values at a given scale"""
    print(f"\n=== ANALYZING RIVERS: {width}x{height}m at resolution {resolution} ===")
    
    # Create visualizer
    visualizer = WorldVisualizer(seed)
    
    # Generate river data
    try:
        rivers = visualizer.generate_rivers_map(-width//2, -height//2, width, height, resolution)
        
        # Statistics
        total_points = rivers.size
        river_threshold = 30.0
        
        # Count rivers at different thresholds
        rivers_30 = np.sum(rivers > 30.0)
        rivers_50 = np.sum(rivers > 50.0) 
        rivers_100 = np.sum(rivers > 100.0)
        rivers_200 = np.sum(rivers > 200.0)
        rivers_500 = np.sum(rivers > 500.0)
        
        max_flow = np.max(rivers)
        min_flow = np.min(rivers)
        mean_flow = np.mean(rivers)
        
        print(f"Total sampling points: {total_points:,}")
        print(f"River flow range: {min_flow:.1f} to {max_flow:.1f}")
        print(f"Mean flow: {mean_flow:.1f}")
        print(f"Points with flow > 30: {rivers_30:,} ({rivers_30/total_points*100:.2f}%)")
        print(f"Points with flow > 50: {rivers_50:,} ({rivers_50/total_points*100:.2f}%)")
        print(f"Points with flow > 100: {rivers_100:,} ({rivers_100/total_points*100:.2f}%)")
        print(f"Points with flow > 200: {rivers_200:,} ({rivers_200/total_points*100:.2f}%)")
        print(f"Points with flow > 500: {rivers_500:,} ({rivers_500/total_points*100:.2f}%)")
        
        # Show histogram of values
        plt.figure(figsize=(12, 8))
        
        # Full range histogram
        plt.subplot(2, 2, 1)
        plt.hist(rivers.flatten(), bins=50, alpha=0.7, color='blue')
        plt.title(f'River Flow Distribution - Full Range\n{width}x{height}m, res={resolution}')
        plt.xlabel('Flow Value')
        plt.ylabel('Count')
        plt.yscale('log')
        
        # Focus on river values (>30)
        plt.subplot(2, 2, 2)
        river_values = rivers[rivers > 30]
        if len(river_values) > 0:
            plt.hist(river_values, bins=30, alpha=0.7, color='cyan')
            plt.title(f'River Flow Distribution - Rivers Only (>30)\nCount: {len(river_values)}')
            plt.xlabel('Flow Value')
            plt.ylabel('Count')
        else:
            plt.text(0.5, 0.5, 'No rivers found\n(flow > 30)', 
                    ha='center', va='center', transform=plt.gca().transAxes)
            plt.title('No Rivers Found')
        
        # River map visualization
        plt.subplot(2, 2, 3)
        river_vis = np.copy(rivers)
        river_vis[river_vis < 30] = 0  # Only show rivers
        plt.imshow(river_vis, cmap='Blues', origin='lower')
        plt.title('River Network (flow > 30)')
        plt.colorbar()
        
        # Log scale visualization 
        plt.subplot(2, 2, 4)
        river_vis_log = np.copy(rivers)
        river_vis_log[river_vis_log <= 0] = 0.1  # Avoid log(0)
        plt.imshow(np.log10(river_vis_log), cmap='viridis', origin='lower')
        plt.title('River Flow (log10 scale)')
        plt.colorbar()
        
        plt.tight_layout()
        filename = f'river_debug_{width//1000}km_res{resolution:.3f}.png'
        plt.savefig(filename, dpi=150, bbox_inches='tight')
        print(f"Saved analysis to: {filename}")
        plt.close()
        
        return {
            'total_points': total_points,
            'rivers_30': rivers_30,
            'rivers_50': rivers_50,
            'rivers_100': rivers_100,
            'rivers_200': rivers_200,
            'rivers_500': rivers_500,
            'max_flow': max_flow,
            'min_flow': min_flow,
            'mean_flow': mean_flow,
            'coverage_30': rivers_30/total_points*100
        }
    
    except Exception as e:
        print(f"ERROR: {e}")
        return None

if __name__ == '__main__':
    # Test at different scales
    scales = [
        (10000, 10000, 0.01),   # 10km x 10km
        (50000, 50000, 0.002),  # 50km x 50km  
        (100000, 100000, 0.001) # 100km x 100km
    ]
    
    results = []
    for width, height, resolution in scales:
        result = analyze_river_values(width, height, resolution)
        if result:
            results.append((width//1000, result))
    
    # Summary comparison
    print("\n=== SCALE COMPARISON SUMMARY ===")
    print("Scale(km) | Total Points | Rivers(>30) | Coverage(%) | Max Flow")
    print("-" * 65)
    for scale_km, result in results:
        print(f"{scale_km:8} | {result['total_points']:11,} | {result['rivers_30']:10,} | {result['coverage_30']:10.2f} | {result['max_flow']:8.1f}")
