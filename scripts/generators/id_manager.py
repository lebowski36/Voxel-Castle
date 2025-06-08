#!/usr/bin/env python3
"""
ID Manager - Block ID Assignment and Registry Management
Part of the Unified Block Resource System (08d.3a.3)

This module handles automatic ID assignment for blocks while maintaining
save game compatibility through a persistent registry system.
"""

import json
import os
from datetime import datetime
from typing import Dict, List, Tuple, Optional
from collections import OrderedDict

class BlockIDManager:
    """
    Manages automatic block ID assignment with stability guarantees.
    
    Key Features:
    - Auto-assigns IDs based on declaration order in JSON files
    - Maintains stability registry to preserve save game compatibility
    - Deterministic generation ensures consistent results
    - Category-based ordering for logical ID ranges
    """
    
    def __init__(self, data_dir: str = "data/blocks", registry_path: str = "data/id_registry.json"):
        self.data_dir = data_dir
        self.registry_path = registry_path
        self.category_order = [
            "terrain",
            "fluids", 
            "processed",
            "functional",
            "advanced",
            "placeholder"
        ]
        
        # Load existing registry for stability
        self.registry = self._load_registry()
        
    def _load_registry(self) -> Dict:
        """Load existing ID registry or create new one."""
        if os.path.exists(self.registry_path):
            try:
                with open(self.registry_path, 'r') as f:
                    return json.load(f)
            except (json.JSONDecodeError, FileNotFoundError) as e:
                print(f"Warning: Could not load registry {self.registry_path}: {e}")
                print("Creating new registry...")
        
        # Create new registry structure
        return {
            "version": "1.0",
            "last_generated": None,
            "assignments": {},
            "next_available_id": 0,
            "notes": {
                "generation_order": " → ".join(self.category_order),
                "AIR": "Reserved ID 0 - special air block"
            }
        }
    
    def _save_registry(self):
        """Save updated registry to file."""
        self.registry["last_generated"] = datetime.now().isoformat()
        
        # Ensure directory exists
        os.makedirs(os.path.dirname(self.registry_path), exist_ok=True)
        
        with open(self.registry_path, 'w') as f:
            json.dump(self.registry, f, indent=2, sort_keys=True)
        
        print(f"Updated ID registry saved: {self.registry_path}")
    
    def _load_category_blocks(self, category: str) -> OrderedDict:
        """Load blocks from a category JSON file."""
        category_file = os.path.join(self.data_dir, f"{category}.json")
        
        if not os.path.exists(category_file):
            print(f"Warning: Category file not found: {category_file}")
            return OrderedDict()
        
        try:
            with open(category_file, 'r') as f:
                data = json.load(f)
                
            # Return blocks in declaration order (OrderedDict preserves order)
            blocks = data.get("blocks", {})
            if isinstance(blocks, dict):
                return OrderedDict(blocks.items())
            else:
                print(f"Warning: 'blocks' section not found in {category_file}")
                return OrderedDict()
                
        except (json.JSONDecodeError, FileNotFoundError) as e:
            print(f"Error loading {category_file}: {e}")
            return OrderedDict()
    
    def generate_all_block_ids(self) -> Dict[str, int]:
        """
        Generate IDs for all blocks using deterministic algorithm.
        
        Algorithm:
        1. Process categories in defined order
        2. Within each category, use JSON declaration order
        3. Preserve existing ID assignments for stability
        4. Assign new IDs to new blocks
        
        Returns:
            Dictionary mapping block names to IDs
        """
        all_assignments = {}
        existing_assignments = self.registry.get("assignments", {})
        next_id = self.registry.get("next_available_id", 0)
        
        # Track which existing IDs are still valid
        used_ids = set()
        
        print("=== Block ID Generation ===")
        print(f"Processing categories: {' → '.join(self.category_order)}")
        
        for category in self.category_order:
            print(f"\nProcessing category: {category}")
            blocks = self._load_category_blocks(category)
            
            if not blocks:
                print(f"  No blocks found in {category}")
                continue
            
            category_assignments = []
            
            for block_name in blocks.keys():
                if block_name in existing_assignments:
                    # Use existing ID for stability
                    assigned_id = existing_assignments[block_name]
                    print(f"  {block_name}: {assigned_id} (existing)")
                    used_ids.add(assigned_id)
                else:
                    # Find next available ID (skip any that are already used)
                    while next_id in used_ids or next_id in existing_assignments.values():
                        next_id += 1
                    
                    assigned_id = next_id
                    print(f"  {block_name}: {assigned_id} (new)")
                    next_id += 1
                    used_ids.add(assigned_id)
                
                all_assignments[block_name] = assigned_id
                category_assignments.append((block_name, assigned_id))
            
            print(f"  Category {category}: {len(category_assignments)} blocks")
        
        # Update registry with new assignments
        self.registry["assignments"] = all_assignments
        self.registry["next_available_id"] = next_id
        
        print(f"\n=== Generation Summary ===")
        print(f"Total blocks: {len(all_assignments)}")
        print(f"Next available ID: {next_id}")
        
        return all_assignments
    
    def validate_assignments(self, assignments: Dict[str, int]) -> bool:
        """
        Validate ID assignments for conflicts and consistency.
        
        Args:
            assignments: Block name to ID mapping
            
        Returns:
            True if valid, False if conflicts found
        """
        print("\n=== Validating ID Assignments ===")
        
        # Check for duplicate IDs
        id_to_blocks = {}
        for block_name, block_id in assignments.items():
            if block_id in id_to_blocks:
                print(f"ERROR: Duplicate ID {block_id} assigned to both '{block_name}' and '{id_to_blocks[block_id]}'")
                return False
            id_to_blocks[block_id] = block_name
        
        # Check for negative IDs
        negative_ids = [(name, id_) for name, id_ in assignments.items() if id_ < 0]
        if negative_ids:
            print(f"ERROR: Negative IDs found: {negative_ids}")
            return False
        
        # Check for unreasonably high IDs (sanity check)
        max_reasonable_id = 10000  # Allow room for growth
        high_ids = [(name, id_) for name, id_ in assignments.items() if id_ > max_reasonable_id]
        if high_ids:
            print(f"WARNING: Unusually high IDs found: {high_ids}")
        
        # Check AIR block has ID 0 (special requirement)
        if "AIR" in assignments and assignments["AIR"] != 0:
            print(f"ERROR: AIR block must have ID 0, but has ID {assignments['AIR']}")
            return False
        
        print(f"✅ Validation passed: {len(assignments)} blocks, IDs 0-{max(assignments.values())}")
        return True
    
    def save_assignments(self, assignments: Dict[str, int]):
        """Save assignments to registry and update next available ID."""
        self.registry["assignments"] = assignments
        if assignments:
            self.registry["next_available_id"] = max(assignments.values()) + 1
        else:
            self.registry["next_available_id"] = 0
        
        self._save_registry()
    
    def get_block_id(self, block_name: str) -> Optional[int]:
        """Get ID for a specific block."""
        return self.registry.get("assignments", {}).get(block_name)
    
    def get_all_assignments(self) -> Dict[str, int]:
        """Get all current block ID assignments."""
        return self.registry.get("assignments", {}).copy()
    
    def print_summary(self):
        """Print a summary of current ID assignments."""
        assignments = self.get_all_assignments()
        
        if not assignments:
            print("No block ID assignments found.")
            return
        
        print(f"\n=== Block ID Registry Summary ===")
        print(f"Registry version: {self.registry.get('version', 'unknown')}")
        print(f"Last generated: {self.registry.get('last_generated', 'never')}")
        print(f"Total blocks: {len(assignments)}")
        print(f"ID range: {min(assignments.values())}-{max(assignments.values())}")
        print(f"Next available: {self.registry.get('next_available_id', 'unknown')}")
        
        # Group by category for display
        category_blocks = {category: [] for category in self.category_order}
        other_blocks = []
        
        for block_name, block_id in sorted(assignments.items(), key=lambda x: x[1]):
            # Try to determine category based on which file contains the block
            found_category = None
            for category in self.category_order:
                blocks = self._load_category_blocks(category)
                if block_name in blocks:
                    found_category = category
                    break
            
            if found_category:
                category_blocks[found_category].append((block_name, block_id))
            else:
                other_blocks.append((block_name, block_id))
        
        # Print by category
        for category in self.category_order:
            if category_blocks[category]:
                print(f"\n{category.upper()}:")
                for block_name, block_id in category_blocks[category]:
                    print(f"  {block_id:3d}: {block_name}")
        
        if other_blocks:
            print(f"\nOTHER:")
            for block_name, block_id in other_blocks:
                print(f"  {block_id:3d}: {block_name}")


def main():
    """Test the ID manager with current JSON data."""
    import sys
    
    # Initialize manager
    manager = BlockIDManager()
    
    if len(sys.argv) > 1 and sys.argv[1] == "--summary":
        # Just print current registry summary
        manager.print_summary()
        return
    
    # Generate all IDs
    assignments = manager.generate_all_block_ids()
    
    # Validate
    if not manager.validate_assignments(assignments):
        print("❌ Validation failed! Not saving assignments.")
        sys.exit(1)
    
    # Save
    manager.save_assignments(assignments)
    
    # Print summary
    manager.print_summary()
    
    print(f"\n✅ Block ID generation complete!")
    print(f"Registry saved to: {manager.registry_path}")


if __name__ == "__main__":
    main()
