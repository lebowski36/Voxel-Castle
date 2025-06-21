#!/usr/bin/env python3
"""
Debug script to understand region mapping and find regions with rivers.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), 'scripts'))

# Import after adding path
try:
    import worldgen_cpp
except ImportError as e:
    print(f"Error importing worldgen_cpp: {e}")
    print("Make sure to build the C++ extension first with:")
    print("cd scripts && python setup.py build_ext --inplace")
    sys.exit(1)

def debug_region_mapping():
    """Debug the region mapping logic to understand river placement."""
    
    # Constants from C++ (these should match river_networks.h)
    REGIONAL_REGION_SIZE = 25000  # 25km regions
    
    seed = 12345
    
    print("=== Region Mapping Debug ===")
    print(f"REGIONAL_REGION_SIZE = {REGIONAL_REGION_SIZE} meters")
    print()
    
    # Test various world coordinates and their region mapping
    test_coords = [
        (0, 0),           # Origin
        (12500, 12500),   # Center of region (0,0) 
        (25000, 25000),   # Boundary - should be region (1,1)
        (24999, 24999),   # Just inside region (0,0)
        (25001, 25001),   # Just inside region (1,1)
        (-12500, -12500), # Negative coordinates
        (50000, 0),       # Far east
        (0, 50000),       # Far north
    ]
    
    print("Testing region mapping:")
    for world_x, world_z in test_coords:
        region_x = int(world_x // REGIONAL_REGION_SIZE)
        region_z = int(world_z // REGIONAL_REGION_SIZE)
        print(f"World ({world_x:6}, {world_z:6}) -> Region ({region_x:2}, {region_z:2})")
    
    print()
    print("=== Scanning for regions with rivers ===")
    
    # Scan a grid of regions to find ones with rivers
    found_river_regions = []
    
    for region_x in range(-2, 3):  # Check 5x5 grid of regions
        for region_z in range(-2, 3):
            # Calculate center of region
            center_x = region_x * REGIONAL_REGION_SIZE + REGIONAL_REGION_SIZE // 2
            center_z = region_z * REGIONAL_REGION_SIZE + REGIONAL_REGION_SIZE // 2
            
            # Query river at region center
            result = worldgen_cpp.query_river_at_point(center_x, center_z, seed)
            
            print(f"Region ({region_x:2}, {region_z:2}) center ({center_x:6}, {center_z:6}): "
                  f"rivers={result.get('has_river', False)}, "
                  f"lakes={result.get('has_lake', False)}")
            
            if result.get('has_river', False):
                found_river_regions.append((region_x, region_z, center_x, center_z))
    
    print()
    print("=== Regions with rivers found ===")
    if found_river_regions:
        for region_x, region_z, center_x, center_z in found_river_regions:
            print(f"Region ({region_x}, {region_z}) at center ({center_x}, {center_z}) HAS RIVERS!")
        
        # Use the first region with rivers for detailed analysis
        region_x, region_z, center_x, center_z = found_river_regions[0]
        print(f"\n=== Detailed analysis of region ({region_x}, {region_z}) ===")
        
        # Sample a grid within this region to find specific river points
        river_points = []
        region_start_x = region_x * REGIONAL_REGION_SIZE
        region_start_z = region_z * REGIONAL_REGION_SIZE
        
        step = 1000  # Sample every 1km
        for offset_x in range(0, REGIONAL_REGION_SIZE, step):
            for offset_z in range(0, REGIONAL_REGION_SIZE, step):
                test_x = region_start_x + offset_x
                test_z = region_start_z + offset_z
                
                result = worldgen_cpp.query_river_at_point(test_x, test_z, seed)
                if result.get('has_river', False):
                    river_points.append((test_x, test_z, result.get('river_width', 0)))
        
        print(f"Found {len(river_points)} river points in region ({region_x}, {region_z}):")
        for x, z, width in river_points[:10]:  # Show first 10
            print(f"  River at ({x}, {z}) width={width:.1f}m")
        
        if river_points:
            # Return the first river point for visualization
            return river_points[0][:2]  # (x, z)
    
    else:
        print("No regions with rivers found in the scanned area!")
        print("This suggests an issue with river generation or querying.")
    
    return None

if __name__ == "__main__":
    river_coords = debug_region_mapping()
    if river_coords:
        print(f"\n=== SUCCESS ===")
        print(f"Found guaranteed river at coordinates: {river_coords}")
        print("You can now use these coordinates to create a visualization!")
    else:
        print(f"\n=== NO RIVERS FOUND ===")
        print("Check the C++ debug output for river generation issues.")
