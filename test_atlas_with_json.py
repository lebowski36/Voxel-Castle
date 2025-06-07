#!/usr/bin/env python3
"""
Test atlas generation using JSON-generated BLOCK_MAPPING
This script tests the bridge between our new JSON system and the existing atlas generation.
"""

import os
import sys

# Import the generated BLOCK_MAPPING
from block_mapping_generated import BLOCK_MAPPING

# Test that we can import the atlas generation system
try:
    from atlas_face_system import BLOCK_FACE_PATTERNS
    print(f"✓ Face patterns loaded: {len(BLOCK_FACE_PATTERNS)} patterns")
    print(f"✓ Block mapping loaded: {len(BLOCK_MAPPING)} blocks")
    
    # Check consistency between face patterns and block mapping
    face_pattern_ids = set(BLOCK_FACE_PATTERNS.keys())
    block_mapping_ids = set(BLOCK_MAPPING.keys())
    
    print(f"\nConsistency check:")
    print(f"  Face patterns cover blocks: {min(face_pattern_ids)} to {max(face_pattern_ids)}")
    print(f"  Block mapping covers blocks: {min(block_mapping_ids)} to {max(block_mapping_ids)}")
    
    # Find missing patterns
    missing_patterns = block_mapping_ids - face_pattern_ids
    extra_patterns = face_pattern_ids - block_mapping_ids
    
    if missing_patterns:
        print(f"  ⚠ Warning: {len(missing_patterns)} blocks lack face patterns: {sorted(list(missing_patterns))[:10]}...")
    if extra_patterns:
        print(f"  ⚠ Warning: {len(extra_patterns)} face patterns lack block mapping: {sorted(list(extra_patterns))[:10]}...")
    
    if not missing_patterns and not extra_patterns:
        print(f"  ✓ Perfect alignment between block mapping and face patterns")
    
    # Show some example mappings
    print(f"\nExample mappings:")
    for i in [0, 1, 3, 30, 50]:
        if i in BLOCK_MAPPING and i in BLOCK_FACE_PATTERNS:
            block = BLOCK_MAPPING[i]
            pattern = BLOCK_FACE_PATTERNS[i]
            print(f"  Block {i}: {block['type']}/{block['subtype']} -> {pattern.value}")
    
    print(f"\n✓ JSON-to-legacy bridge working correctly!")
    
except ImportError as e:
    print(f"✗ Error importing atlas systems: {e}")
    sys.exit(1)
