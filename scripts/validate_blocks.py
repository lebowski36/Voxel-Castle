#!/usr/bin/env python3
"""
Block Data Validation Script
Validates the unified block resource system data for consistency and integrity.
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Set, Any

class BlockDataValidator:
    def __init__(self, data_dir: str):
        self.data_dir = Path(data_dir)
        self.blocks_dir = self.data_dir / "blocks"
        self.schema_file = self.data_dir / "block_schema_proposal.json"
        self.index_file = self.blocks_dir / "_index.json"
        
        self.errors = []
        self.warnings = []
        self.all_blocks = {}
        self.used_ids = set()
        
    def validate_all(self) -> bool:
        """Run all validation checks"""
        print("🔍 Validating block data...")
        
        # Load and validate index
        if not self._validate_index():
            return False
            
        # Load and validate category files
        if not self._validate_category_files():
            return False
            
        # Cross-validation checks
        self._validate_id_consistency()
        self._validate_atlas_assignments()
        self._validate_face_patterns()
        
        # Report results
        self._report_results()
        
        return len(self.errors) == 0
        
    def _validate_index(self) -> bool:
        """Validate the master index file"""
        try:
            with open(self.index_file, 'r') as f:
                self.index_data = json.load(f)
        except FileNotFoundError:
            self.errors.append(f"Index file not found: {self.index_file}")
            return False
        except json.JSONDecodeError as e:
            self.errors.append(f"Invalid JSON in index file: {e}")
            return False
            
        # Validate required fields in index
        required_fields = ['metadata', 'validation']
        for field in required_fields:
            if field not in self.index_data:
                self.errors.append(f"Missing required field in index: {field}")
                
        return len(self.errors) == 0
        
    def _validate_category_files(self) -> bool:
        """Validate all category files"""
        categories = self.index_data.get('metadata', {}).get('categories', {})
        
        for category_name, category_info in categories.items():
            file_path = self.blocks_dir / category_info['file']
            
            if not file_path.exists():
                self.errors.append(f"Category file not found: {file_path}")
                continue
                
            try:
                with open(file_path, 'r') as f:
                    category_data = json.load(f)
                    
                self._validate_category_data(category_name, category_data, category_info)
                
            except json.JSONDecodeError as e:
                self.errors.append(f"Invalid JSON in {file_path}: {e}")
                
        return len(self.errors) == 0
        
    def _validate_category_data(self, category_name: str, data: Dict, info: Dict):
        """Validate a single category file"""
        id_range = info['id_range']
        min_id, max_id = id_range[0], id_range[1]
        
        blocks = data.get('blocks', {})
        
        for block_name, block_data in blocks.items():
            if block_name.startswith('_'):  # Skip placeholders
                continue
                
            # Validate block structure
            self._validate_block_data(block_name, block_data, min_id, max_id)
            
    def _validate_block_data(self, block_name: str, block_data: Dict, min_id: int, max_id: int):
        """Validate a single block definition"""
        # Check required fields
        required_fields = ['id', 'name', 'category', 'physical_properties', 'texture_info']
        
        for field in required_fields:
            if field not in block_data:
                self.errors.append(f"Block {block_name}: Missing required field '{field}'")
                return
                
        block_id = block_data.get('id')
        
        # Validate ID range
        if not (min_id <= block_id <= max_id):
            self.errors.append(f"Block {block_name}: ID {block_id} outside valid range [{min_id}, {max_id}]")
            
        # Check for duplicate IDs
        if block_id in self.used_ids:
            self.errors.append(f"Block {block_name}: Duplicate ID {block_id}")
        else:
            self.used_ids.add(block_id)
            
        # Store for cross-validation
        self.all_blocks[block_name] = block_data
        
        # Validate face pattern
        face_pattern = block_data.get('texture_info', {}).get('face_pattern')
        valid_patterns = ['UNIFORM', 'TOP_BOTTOM_DIFFERENT', 'ALL_DIFFERENT', 'DIRECTIONAL', 'ALL_FACES_DIFFERENT']
        if face_pattern not in valid_patterns:
            self.errors.append(f"Block {block_name}: Invalid face_pattern '{face_pattern}'")
            
    def _validate_id_consistency(self):
        """Check that all IDs are sequential and complete"""
        if not self.all_blocks:
            return
            
        all_ids = sorted([block['id'] for block in self.all_blocks.values()])
        
        # Check for gaps (except placeholders)
        for i in range(len(all_ids) - 1):
            if all_ids[i + 1] - all_ids[i] > 1:
                self.warnings.append(f"ID gap detected: {all_ids[i]} -> {all_ids[i + 1]}")
                
    def _validate_atlas_assignments(self):
        """Validate atlas slot assignments based on face patterns"""
        main_slots = set()
        side_slots = set()
        bottom_slots = set()
        
        for block_name, block_data in self.all_blocks.items():
            face_pattern = block_data.get('texture_info', {}).get('face_pattern')
            atlas_assignment = block_data.get('texture_info', {}).get('atlas_assignment', {})
            
            if face_pattern == 'UNIFORM':
                if 'main' not in atlas_assignment:
                    self.warnings.append(f"Block {block_name}: UNIFORM pattern should have 'main' atlas assignment")
                    
            elif face_pattern == 'TOP_BOTTOM_DIFFERENT':
                if 'main' not in atlas_assignment or 'side' not in atlas_assignment:
                    self.warnings.append(f"Block {block_name}: TOP_BOTTOM_DIFFERENT pattern should have 'main' and 'side' assignments")
                    
            elif face_pattern == 'ALL_DIFFERENT':
                if not all(key in atlas_assignment for key in ['main', 'side', 'bottom']):
                    self.warnings.append(f"Block {block_name}: ALL_DIFFERENT pattern should have 'main', 'side', and 'bottom' assignments")
                    
            elif face_pattern == 'ALL_FACES_DIFFERENT':
                self.warnings.append(f"Block {block_name}: ALL_FACES_DIFFERENT pattern is reserved for future multiblock implementation")
                    
    def _validate_face_patterns(self):
        """Validate face pattern usage for atlas efficiency"""
        pattern_counts = {}
        for block_data in self.all_blocks.values():
            pattern = block_data.get('texture_info', {}).get('face_pattern', 'UNIFORM')
            pattern_counts[pattern] = pattern_counts.get(pattern, 0) + 1
            
        total_blocks = len(self.all_blocks)
        uniform_ratio = pattern_counts.get('UNIFORM', 0) / total_blocks if total_blocks > 0 else 0
        
        if uniform_ratio < 0.7:
            self.warnings.append(f"Atlas efficiency warning: Only {uniform_ratio:.1%} blocks use UNIFORM pattern")
            
    def _report_results(self):
        """Report validation results"""
        print(f"\n📊 Validation Results:")
        print(f"   Blocks validated: {len(self.all_blocks)}")
        print(f"   Unique IDs: {len(self.used_ids)}")
        
        if self.errors:
            print(f"\n❌ Errors ({len(self.errors)}):")
            for error in self.errors:
                print(f"   • {error}")
                
        if self.warnings:
            print(f"\n⚠️  Warnings ({len(self.warnings)}):")
            for warning in self.warnings:
                print(f"   • {warning}")
                
        if not self.errors and not self.warnings:
            print("✅ All validations passed!")
        elif not self.errors:
            print("✅ No errors found (warnings can be addressed later)")
        else:
            print("❌ Validation failed - please fix errors before proceeding")

def main():
    if len(sys.argv) != 2:
        print("Usage: python validate_blocks.py <data_directory>")
        sys.exit(1)
        
    data_dir = sys.argv[1]
    
    if not os.path.exists(data_dir):
        print(f"Error: Data directory not found: {data_dir}")
        sys.exit(1)
        
    validator = BlockDataValidator(data_dir)
    success = validator.validate_all()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
