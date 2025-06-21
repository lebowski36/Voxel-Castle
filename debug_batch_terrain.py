#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import worldgen_cpp

def debug_batch_terrain():
    """Debug the difference between single point and batch terrain generation"""
    print("=== Debugging Batch vs Single Point Terrain Generation ===")
    seed = 12345
    
    # Test coordinates from known river locations
    test_coords = [
        (-6250, -6250),  # Known river source at 156.429m elevation
        (-5750, -10250), # Known river source at 108.676m elevation  
        (-4250, -10250), # Known river source at 117.268m elevation
        (-2750, -10250), # Known river source at 104.488m elevation
    ]
    
    print("Testing individual points...")
    for x, z in test_coords:
        print(f"\nCoordinate: ({x}, {z})")
        
        # Single point queries
        try:
            regular = worldgen_cpp.generate_terrain_heightmap([x], [z], seed)
            carved = worldgen_cpp.generate_terrain_heightmap_with_rivers([x], [z], seed)
            river = worldgen_cpp.generate_comprehensive_river_flow([x], [z], seed)
            
            print(f"  Regular terrain: {regular[0]:.3f}m")
            print(f"  Carved terrain:  {carved[0]:.3f}m")
            print(f"  River strength:  {river[0]:.3f}")
            print(f"  Carving depth:   {regular[0] - carved[0]:.3f}m")
        except Exception as e:
            print(f"  ERROR: {e}")
    
    print("\n" + "="*60)
    print("Testing batch processing...")
    
    # Extract coordinates into arrays
    x_coords = [coord[0] for coord in test_coords]
    z_coords = [coord[1] for coord in test_coords]
    
    print(f"Batch coordinates: {len(x_coords)} points")
    print(f"X: {x_coords}")
    print(f"Z: {z_coords}")
    
    try:
        # Batch queries
        regular_batch = worldgen_cpp.generate_terrain_heightmap(x_coords, z_coords, seed)
        carved_batch = worldgen_cpp.generate_terrain_heightmap_with_rivers(x_coords, z_coords, seed)
        river_batch = worldgen_cpp.generate_comprehensive_river_flow(x_coords, z_coords, seed)
        
        print(f"\nBatch results:")
        for i, (x, z) in enumerate(test_coords):
            print(f"  Point {i+1} ({x}, {z}):")
            print(f"    Regular: {regular_batch[i]:.3f}m")
            print(f"    Carved:  {carved_batch[i]:.3f}m") 
            print(f"    River:   {river_batch[i]:.3f}")
            print(f"    Carving: {regular_batch[i] - carved_batch[i]:.3f}m")
            
    except Exception as e:
        print(f"Batch processing ERROR: {e}")
    
    print("\n" + "="*60)
    print("Testing array types and conversion...")
    
    # Test with different array types
    x_np = np.array(x_coords, dtype=np.float64)
    z_np = np.array(z_coords, dtype=np.float64)
    
    print(f"NumPy array types: X={x_np.dtype}, Z={z_np.dtype}")
    print(f"Array shapes: X={x_np.shape}, Z={z_np.shape}")
    print(f"Array contents: X={x_np}, Z={z_np}")
    
    try:
        regular_np = worldgen_cpp.generate_terrain_heightmap(x_np, z_np, seed)
        print(f"NumPy batch result: {regular_np}")
    except Exception as e:
        print(f"NumPy batch ERROR: {e}")

if __name__ == "__main__":
    debug_batch_terrain()
