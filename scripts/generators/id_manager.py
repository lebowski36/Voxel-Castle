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
        
        # Migrate registry format if needed
        self._migrate_registry_if_needed()
        self._migrate_registry_if_needed()
        
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
            "version": "1.1",
            "last_generated": None,
            "assignments": {},
            "deprecated_blocks": {},
            "permanently_reserved_ids": [0],  # AIR block is permanently reserved
            "next_available_id": 0,
            "notes": {
                "generation_order": " → ".join(self.category_order),
                "AIR": "Reserved ID 0 - special air block",
                "deprecated_blocks": "Blocks removed from JSON but with IDs reserved for save compatibility",
                "permanently_reserved_ids": "IDs that can never be reused (includes deprecated blocks)"
            }
        }
    
    def _migrate_registry_if_needed(self):
        """Migrate registry from older versions to current format."""
        current_version = self.registry.get("version", "1.0")
        
        if current_version == "1.0":
            print("Migrating registry from v1.0 to v1.1...")
            self.registry["version"] = "1.1"
            self.registry["deprecated_blocks"] = {}
            self.registry["permanently_reserved_ids"] = [0]  # AIR is always reserved
            self.registry["notes"]["deprecated_blocks"] = "Blocks removed from JSON but with IDs reserved for save compatibility"
            self.registry["notes"]["permanently_reserved_ids"] = "IDs that can never be reused (includes deprecated blocks)"
            print("Registry migration complete.")
    
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
        reserved_ids = set(self.registry.get("permanently_reserved_ids", []))
        next_id = self.registry.get("next_available_id", 0)
        
        # Track which existing IDs are still valid
        used_ids = set(existing_assignments.values()) | reserved_ids
        
        print("=== Block ID Generation ===")
        print(f"Processing categories: {' → '.join(self.category_order)}")
        
        for category in self.category_order:
            print(f"\nProcessing category: {category}")
            blocks = self._load_category_blocks(category)
            
            if not blocks:
                print(f"  No blocks found in {category}")
                continue
            
            category_assignments = []
            existing_count = 0
            new_count = 0
            new_blocks = []
            
            for block_name in blocks.keys():
                if block_name in existing_assignments:
                    # Use existing ID for stability
                    assigned_id = existing_assignments[block_name]
                    existing_count += 1
                else:
                    # Find next available ID (skip reserved and used IDs)
                    while next_id in used_ids:
                        next_id += 1
                    
                    assigned_id = next_id
                    new_blocks.append(f"{block_name}: {assigned_id}")
                    new_count += 1
                    used_ids.add(assigned_id)
                    next_id += 1
                
                all_assignments[block_name] = assigned_id
                category_assignments.append((block_name, assigned_id))
            
            # Print summary instead of every block
            if existing_count > 0:
                print(f"  {existing_count} existing blocks")
            if new_count > 0:
                print(f"  {new_count} new blocks:")
                for new_block in new_blocks:
                    print(f"    {new_block}")
            if existing_count == 0 and new_count == 0:
                print(f"  No blocks found")
            
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
        
        # Check if we have any assignments at all
        if not assignments:
            print(f"❌ Validation failed: No blocks found in any category!")
            return False
        
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
        """Print a summary of current ID assignments and deprecated blocks."""
        assignments = self.get_all_assignments()
        deprecated = self.get_deprecated_blocks()
        reserved_ids = self.get_permanently_reserved_ids()
        
        if not assignments and not deprecated:
            print("No block ID assignments or deprecated blocks found.")
            return
        
        print(f"\n=== Block ID Registry Summary ===")
        print(f"Registry version: {self.registry.get('version', 'unknown')}")
        print(f"Last generated: {self.registry.get('last_generated', 'never')}")
        print(f"Active blocks: {len(assignments)}")
        print(f"Deprecated blocks: {len(deprecated)}")
        print(f"Reserved IDs: {len(reserved_ids)}")
        
        if assignments:
            print(f"Active ID range: {min(assignments.values())}-{max(assignments.values())}")
        print(f"Next available: {self.registry.get('next_available_id', 'unknown')}")
        
        # Show deprecated blocks if any
        if deprecated:
            print(f"\n=== Deprecated Blocks ===")
            for block_name, info in sorted(deprecated.items(), key=lambda x: x[1]['id']):
                fallback = info.get('fallback_block', 'AIR')
                reason = info.get('reason', 'No reason')
                print(f"  {block_name} (ID {info['id']}) → fallback: {fallback}")
                print(f"    Reason: {reason}")
        
        # Group active blocks by category for display
        if assignments:
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
            
            # Print category summary only (not detailed block list)
            print(f"\n=== Active Blocks by Category ===")
            for category in self.category_order:
                if category_blocks[category]:
                    block_count = len(category_blocks[category])
                    min_id = min(block_id for _, block_id in category_blocks[category])
                    max_id = max(block_id for _, block_id in category_blocks[category])
                    print(f"  {category.upper()}: {block_count} blocks (IDs {min_id}-{max_id})")
            
            if other_blocks:
                block_count = len(other_blocks)
                min_id = min(block_id for _, block_id in other_blocks)
                max_id = max(block_id for _, block_id in other_blocks)
                print(f"  OTHER: {block_count} blocks (IDs {min_id}-{max_id})")
    
    def deprecate_block(self, block_name: str, reason: str = "Block removed") -> bool:
        """
        Safely deprecate a block by moving it from active assignments to deprecated.
        
        Args:
            block_name: Name of the block to deprecate
            reason: Reason for deprecation (for documentation)
            
        Returns:
            True if successfully deprecated, False if block not found
        """
        assignments = self.registry.get("assignments", {})
        deprecated = self.registry.get("deprecated_blocks", {})
        reserved_ids = set(self.registry.get("permanently_reserved_ids", []))
        
        if block_name not in assignments:
            print(f"ERROR: Block '{block_name}' not found in active assignments")
            return False
        
        if block_name in deprecated:
            print(f"WARNING: Block '{block_name}' is already deprecated")
            return True
        
        # Move from assignments to deprecated
        block_id = assignments[block_name]
        del assignments[block_name]
        
        deprecated[block_name] = {
            "id": block_id,
            "deprecated_date": datetime.now().isoformat(),
            "reason": reason,
            "fallback_block": "AIR"  # Default fallback, can be customized
        }
        
        # Add ID to permanently reserved list
        reserved_ids.add(block_id)
        self.registry["permanently_reserved_ids"] = sorted(list(reserved_ids))
        
        print(f"✅ Block '{block_name}' (ID {block_id}) deprecated successfully")
        print(f"   Reason: {reason}")
        print(f"   ID {block_id} is now permanently reserved")
        
        return True
    
    def set_fallback_block(self, deprecated_block: str, fallback_block: str) -> bool:
        """
        Set the fallback block for a deprecated block.
        
        Args:
            deprecated_block: Name of the deprecated block
            fallback_block: Name of the block to use as fallback
            
        Returns:
            True if successful, False if deprecated block not found
        """
        deprecated = self.registry.get("deprecated_blocks", {})
        assignments = self.registry.get("assignments", {})
        
        if deprecated_block not in deprecated:
            print(f"ERROR: Block '{deprecated_block}' is not deprecated")
            return False
        
        if fallback_block not in assignments and fallback_block != "AIR":
            print(f"ERROR: Fallback block '{fallback_block}' does not exist")
            return False
        
        deprecated[deprecated_block]["fallback_block"] = fallback_block
        print(f"✅ Set fallback for '{deprecated_block}' to '{fallback_block}'")
        
        return True
    
    def get_deprecated_blocks(self) -> Dict[str, Dict]:
        """Get all deprecated blocks with their metadata."""
        return self.registry.get("deprecated_blocks", {}).copy()
    
    def get_permanently_reserved_ids(self) -> List[int]:
        """Get all permanently reserved IDs."""
        return self.registry.get("permanently_reserved_ids", []).copy()
    
    def is_id_available(self, block_id: int) -> bool:
        """Check if a specific ID is available for assignment."""
        assignments = self.registry.get("assignments", {})
        reserved_ids = set(self.registry.get("permanently_reserved_ids", []))
        
        return block_id not in assignments.values() and block_id not in reserved_ids


def main():
    """CLI interface for the ID manager with deprecation support."""
    import sys
    import argparse
    
    # Get project root (assume script is in scripts/generators/)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(os.path.dirname(script_dir))
    
    # Use absolute paths
    data_dir = os.path.join(project_root, "data", "blocks")
    registry_path = os.path.join(project_root, "data", "id_registry.json")
    
    # Initialize manager with absolute paths
    manager = BlockIDManager(data_dir=data_dir, registry_path=registry_path)
    
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Block ID Manager - Unified Block Resource System")
    parser.add_argument("--summary", action="store_true", help="Print current registry summary")
    parser.add_argument("--deprecate", type=str, help="Deprecate a block by name")
    parser.add_argument("--reason", type=str, default="Block removed", help="Reason for deprecation")
    parser.add_argument("--set-fallback", nargs=2, metavar=("DEPRECATED_BLOCK", "FALLBACK_BLOCK"),
                       help="Set fallback block for a deprecated block")
    parser.add_argument("--list-deprecated", action="store_true", help="List all deprecated blocks")
    
    args = parser.parse_args()
    
    # Handle specific commands
    if args.summary:
        manager.print_summary()
        return
    
    if args.deprecate:
        if manager.deprecate_block(args.deprecate, args.reason):
            manager._save_registry()
            print(f"✅ Successfully deprecated block '{args.deprecate}'")
        else:
            print(f"❌ Failed to deprecate block '{args.deprecate}'")
            sys.exit(1)
        return
    
    if args.set_fallback:
        deprecated_block, fallback_block = args.set_fallback
        if manager.set_fallback_block(deprecated_block, fallback_block):
            manager._save_registry()
            print(f"✅ Successfully set fallback for '{deprecated_block}' to '{fallback_block}'")
        else:
            print(f"❌ Failed to set fallback for '{deprecated_block}'")
            sys.exit(1)
        return
    
    if args.list_deprecated:
        deprecated = manager.get_deprecated_blocks()
        if not deprecated:
            print("No deprecated blocks found.")
        else:
            print("=== Deprecated Blocks ===")
            for block_name, info in sorted(deprecated.items(), key=lambda x: x[1]['id']):
                fallback = info.get('fallback_block', 'AIR')
                reason = info.get('reason', 'No reason')
                date = info.get('deprecated_date', 'Unknown date')
                print(f"  {block_name} (ID {info['id']})")
                print(f"    Fallback: {fallback}")
                print(f"    Reason: {reason}")
                print(f"    Date: {date}")
                print()
        return
    
    # Default behavior: generate all IDs
    print("Generating block IDs...")
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
    
    # Show usage hints
    print(f"\nUsage hints:")
    print(f"  --summary              Show current registry status")
    print(f"  --deprecate BLOCK      Safely remove a block")
    print(f"  --list-deprecated      Show all deprecated blocks")


if __name__ == "__main__":
    main()
