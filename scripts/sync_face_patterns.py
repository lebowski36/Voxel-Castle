#!/usr/bin/env python3
"""
Sync Face Patterns from atlas_face_system.py to JSON block data
This script updates the JSON files with the face patterns defined in atlas_face_system.py
"""

import json
import os
import sys

# Add parent directory to path to import atlas_face_system
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
sys.path.insert(0, parent_dir)

from atlas_face_system import BLOCK_FACE_PATTERNS

def update_json_with_face_patterns():
    """Update all JSON files with face patterns from atlas_face_system.py"""
    
    blocks_dir = "data/blocks"
    index_file = os.path.join(blocks_dir, "_index.json")
    
    # Load index to get category files
    with open(index_file, 'r') as f:
        index_data = json.load(f)
    
    total_updated = 0
    
    for category_name, category_info in index_data["metadata"]["categories"].items():
        category_file = os.path.join(blocks_dir, category_info["file"])
        
        if not os.path.exists(category_file):
            print(f"⚠ Warning: Category file not found: {category_file}")
            continue
            
        # Load category data
        with open(category_file, 'r') as f:
            category_data = json.load(f)
        
        updated_in_category = 0
        
        # Update each block with face pattern
        for block_name, block_data in category_data["blocks"].items():
            block_id = block_data["id"]
            
            if block_id in BLOCK_FACE_PATTERNS:
                face_pattern = BLOCK_FACE_PATTERNS[block_id].value
                block_data["face_pattern"] = face_pattern
                updated_in_category += 1
            else:
                print(f"⚠ Warning: No face pattern for block {block_id} ({block_name})")
        
        # Save updated category file
        with open(category_file, 'w') as f:
            json.dump(category_data, f, indent=2)
        
        print(f"✓ Updated {updated_in_category} blocks in {category_file}")
        total_updated += updated_in_category
    
    print(f"\n✅ Total blocks updated with face patterns: {total_updated}")

def main():
    print("🎭 Syncing Face Patterns: atlas_face_system.py → JSON files")
    print("=" * 60)
    
    try:
        update_json_with_face_patterns()
        print("\n🎯 Face pattern sync complete!")
        print("   Run the atlas calculator again to see the updated analysis.")
        return True
        
    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    main()
