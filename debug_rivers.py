#!/usr/bin/env python3

import sys
import os
sys.path.append('scripts/cpp_wrapper')

import numpy as np
import worldgen_cpp

def debug_river_system():
    """Debug the river system step by step"""
    print("🔍 DEBUGGING RIVER SYSTEM")
    print("=" * 50)
    
    seed = 12345
    
    # Test a single point
    test_x = 0.0
    test_z = 0.0
    
    print(f"🎯 Testing point ({test_x}, {test_z}) with seed {seed}")
    
    # Test elevation
    elevation = worldgen_cpp.generate_single_heightmap(test_x, test_z, seed)
    print(f"📏 Elevation: {elevation:.2f}m")
    
    # Test climate
    climate = worldgen_cpp.generate_climate_data(test_x, test_z, seed, elevation)
    print(f"🌡️ Climate:")
    print(f"  Temperature: {climate['temperature']:.1f}°C")
    print(f"  Humidity: {climate['humidity']:.3f}")
    print(f"  Precipitation: {climate['precipitation']:.1f}mm")
    
    # Test river flow directly
    print(f"\n🌊 Testing river flow...")
    try:
        # Create coordinate arrays for batch processing
        x_coords = np.array([test_x])
        z_coords = np.array([test_z])
        
        # Test comprehensive river flow
        river_flow = worldgen_cpp.generate_comprehensive_river_flow(x_coords, z_coords, seed)
        print(f"River flow strength: {river_flow[0]:.2f}")
        
        # Test comprehensive river data for single point
        river_data = worldgen_cpp.generate_comprehensive_river_data(test_x, test_z, seed)
        print(f"Comprehensive river data:")
        for key, value in river_data.items():
            print(f"  {key}: {value}")
            
    except Exception as e:
        print(f"❌ Error testing river functions: {e}")
    
    print(f"\n🗺️ Testing multiple points...")
    
    # Test a grid of points to see if ANY have rivers
    test_points = [
        (0, 0), (1000, 0), (0, 1000), (1000, 1000),
        (5000, 5000), (-5000, -5000), (10000, 10000)
    ]
    
    rivers_found = 0
    for x, z in test_points:
        try:
            x_arr = np.array([float(x)])
            z_arr = np.array([float(z)])
            flow = worldgen_cpp.generate_comprehensive_river_flow(x_arr, z_arr, seed)
            if flow[0] > 0.1:
                rivers_found += 1
                print(f"✅ River found at ({x}, {z}): strength {flow[0]:.2f}")
            else:
                print(f"❌ No river at ({x}, {z}): strength {flow[0]:.2f}")
        except Exception as e:
            print(f"❌ Error at ({x}, {z}): {e}")
    
    print(f"\n📊 Summary: {rivers_found} rivers found out of {len(test_points)} test points")

if __name__ == "__main__":
    debug_river_system()
