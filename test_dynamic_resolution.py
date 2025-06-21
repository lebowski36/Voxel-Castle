#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import worldgen_cpp

def test_dynamic_resolution():
    """Test the dynamic resolution system with different area sizes"""
    print("=== Dynamic Resolution System Test ===")
    
    test_areas = [
        {"name": "Small", "size": 500},    # 500m
        {"name": "Medium", "size": 2000},  # 2km  
        {"name": "Large", "size": 10000},  # 10km
        {"name": "Huge", "size": 50000},   # 50km
    ]
    
    for area in test_areas:
        size = area["size"]
        
        # Test terrain resolution
        terrain_samples = worldgen_cpp.get_optimal_sample_count(size, "terrain")
        terrain_resolution = worldgen_cpp.get_optimal_sampling_resolution(size, "terrain")
        
        # Test river resolution
        river_samples = worldgen_cpp.get_optimal_sample_count(size, "rivers")
        river_resolution = worldgen_cpp.get_optimal_sampling_resolution(size, "rivers")
        
        print(f"\n{area['name']} Area ({size}m):")
        print(f"  Terrain: {terrain_samples}x{terrain_samples} samples ({terrain_resolution:.2f} samples/m, {size/terrain_samples:.1f}m/pixel)")
        print(f"  Rivers:  {river_samples}x{river_samples} samples ({river_resolution:.2f} samples/m, {size/river_samples:.1f}m/pixel)")
        print(f"  Total samples: Terrain={terrain_samples**2:,}, Rivers={river_samples**2:,}")

if __name__ == "__main__":
    test_dynamic_resolution()
