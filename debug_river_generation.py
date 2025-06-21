#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import worldgen_cpp

def test_region_generation():
    """Test river network generation for a specific region"""
    print("=== Testing River Generation ===")
    
    seed = 12345
    region_size = 25000  # 25km regions
    
    # Test a few specific regions around the origin
    test_regions = [
        (0, 0),    # Origin region
        (-1, 0),   # West of origin  
        (0, -1),   # South of origin
        (1, 1),    # Northeast of origin
    ]
    
    for region_x, region_z in test_regions:
        print(f"\n--- Testing region ({region_x}, {region_z}) ---")
        
        # Generate river network for this region
        try:
            # Get terrain data for a few points in this region
            region_world_x = region_x * region_size
            region_world_z = region_z * region_size
            
            print(f"Region world coordinates: ({region_world_x}, {region_world_z}) to ({region_world_x + region_size}, {region_world_z + region_size})")
            
            # Test a few points within the region
            test_points = [
                (region_world_x + 1000, region_world_z + 1000),
                (region_world_x + 12500, region_world_z + 12500),  # Center
                (region_world_x + 20000, region_world_z + 20000),
            ]
            
            for x, z in test_points:
                elevation = worldgen_cpp.get_terrain_elevation(x, z, seed)
                river_data = worldgen_cpp.query_river_at_point(x, z, seed)
                print(f"  Point ({x}, {z}): elevation={elevation:.1f}m, has_river={river_data.has_river}")
                
                if river_data.has_river:
                    print(f"    ✓ River found! Width={river_data.river_width:.1f}m, depth={river_data.river_depth:.1f}m")
                    return x, z  # Found a river!
            
        except Exception as e:
            print(f"  ✗ Error testing region ({region_x}, {region_z}): {e}")
    
    print("\n✗ No rivers found in any test regions")
    return None, None

def test_river_sources():
    """Test if river sources are being detected at all"""
    print("\n=== Testing River Source Detection ===")
    
    seed = 12345
    
    # Test some points manually
    test_points = [
        (0, 0),
        (1000, 1000), 
        (5000, 5000),
        (-2000, 3000),
        (10000, -5000),
    ]
    
    for x, z in test_points:
        elevation = worldgen_cpp.get_terrain_elevation(x, z, seed)
        # We don't have direct access to IsRiverSource, but we can test through region generation
        print(f"Point ({x}, {z}): elevation={elevation:.1f}m")

if __name__ == "__main__":
    try:
        print("Testing river generation system...")
        river_x, river_z = test_region_generation()
        test_river_sources()
        
        if river_x is not None:
            print(f"\n✓ Found river at ({river_x}, {river_z})")
        else:
            print("\n✗ No rivers found in any test area")
            
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
