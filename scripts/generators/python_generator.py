#!/usr/bin/env python3
"""
Python Code Generator for Voxel Block System

Generates Python mappings from JSON block definitions and ID registry.
Creates constants, enums, and utility functions for Python scripts.
"""

import json
import os
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any

class PythonGenerator:
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.id_registry_path = self.project_root / "data" / "id_registry.json"
        self.blocks_dir = self.project_root / "data" / "blocks"
        self.output_path = self.project_root / "block_mapping_generated.py"
        
    def load_data(self):
        """Load ID registry and block definitions"""
        print("Loading data files...")
        
        # Load ID registry
        with open(self.id_registry_path, 'r') as f:
            self.id_registry = json.load(f)
        
        # Load all block definition files
        self.block_definitions = {}
        block_files = [
            "terrain.json",
            "fluids.json", 
            "processed.json",
            "functional.json",
            "advanced.json",
            "placeholder.json"
        ]
        
        for file_name in block_files:
            file_path = self.blocks_dir / file_name
            if file_path.exists():
                with open(file_path, 'r') as f:
                    data = json.load(f)
                    if isinstance(data, dict) and "blocks" in data:
                        category = file_name.replace(".json", "")
                        self.block_definitions[category] = data["blocks"]
                        print(f"  Loaded {len(data['blocks'])} blocks from {file_name}")
        
        # Build categorized mapping from block definitions and ID registry
        self.categorized_blocks = {}
        for category, blocks in self.block_definitions.items():
            self.categorized_blocks[category] = {}
            for block_key in blocks.keys():
                if block_key in self.id_registry["assignments"]:
                    self.categorized_blocks[category][block_key] = self.id_registry["assignments"][block_key]
                else:
                    print(f"Warning: Block {block_key} not found in ID registry")
        
        print(f"  Built categorized mapping with {len(self.id_registry['assignments'])} total blocks")
    
    def generate_file_header(self) -> str:
        """Generate standard file header with timestamp and warning"""
        timestamp = datetime.now().isoformat()
        return f'''"""
Voxel Block Type Mappings

AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
Generated on: {timestamp}
Source: JSON block definitions + ID registry

This file is automatically generated by scripts/generators/python_generator.py
To update this file, modify the JSON data files and run the generator.
"""

from enum import IntEnum
from typing import Dict, List, Optional, Set

'''
    
    def format_enum_name(self, block_key: str) -> str:
        """Convert block key to Python enum name"""
        return block_key.upper()
    
    def format_constant_name(self, block_key: str) -> str:
        """Convert block key to Python constant name"""
        return f"BLOCK_{block_key.upper()}"
    
    def generate_voxel_enum(self) -> str:
        """Generate VoxelType enum class"""
        content = "class VoxelType(IntEnum):\n"
        content += '    """Enumeration of all voxel block types with their IDs"""\n'
        
        # Add enum values from categorized blocks
        for category_name, blocks in self.categorized_blocks.items():
            content += f"    # {category_name.upper()} BLOCKS\n"
            for block_key, block_id in blocks.items():
                enum_name = self.format_enum_name(block_key)
                content += f"    {enum_name} = {block_id}\n"
            content += "\n"
        
        content += f"    # Utility values\n"
        content += f"    COUNT = {self.id_registry['next_available_id']}\n"
        content += f"    INVALID = 65535\n\n"
        
        return content
    
    def generate_constants(self) -> str:
        """Generate block type constants"""
        content = "# Block type constants for easier usage\n"
        content += "class BlockTypes:\n"
        content += '    """Convenient constants for block types"""\n'
        
        for category_name, blocks in self.categorized_blocks.items():
            content += f"    # {category_name.upper()}\n"
            for block_key, block_id in blocks.items():
                const_name = self.format_constant_name(block_key)
                enum_name = self.format_enum_name(block_key)
                content += f"    {const_name} = VoxelType.{enum_name}\n"
            content += "\n"
        
        content += "\n"
        return content
    
    def generate_lookup_tables(self) -> str:
        """Generate lookup dictionaries"""
        content = "# Lookup tables\n"
        
        # Name to ID mapping
        content += "BLOCK_NAME_TO_ID: Dict[str, int] = {\n"
        for category_name, blocks in self.categorized_blocks.items():
            content += f"    # {category_name.upper()}\n"
            for block_key, block_id in blocks.items():
                content += f"    '{block_key}': {block_id},\n"
            content += "\n"
        content += "}\n\n"
        
        # ID to name mapping
        content += "BLOCK_ID_TO_NAME: Dict[int, str] = {\n"
        for category_name, blocks in self.categorized_blocks.items():
            content += f"    # {category_name.upper()}\n"
            for block_key, block_id in blocks.items():
                content += f"    {block_id}: '{block_key}',\n"
            content += "\n"
        content += "}\n\n"
        
        # Category mappings
        content += "BLOCK_CATEGORIES: Dict[str, Set[str]] = {\n"
        for category_name, blocks in self.categorized_blocks.items():
            content += f"    '{category_name}': {{\n"
            for block_key in blocks.keys():
                content += f"        '{block_key}',\n"
            content += f"    }},\n"
        content += "}\n\n"
        
        # Category by block name
        content += "BLOCK_TO_CATEGORY: Dict[str, str] = {\n"
        for category_name, blocks in self.categorized_blocks.items():
            content += f"    # {category_name.upper()}\n"
            for block_key in blocks.keys():
                content += f"    '{block_key}': '{category_name}',\n"
            content += "\n"
        content += "}\n\n"
        
        return content
    
    def generate_utility_functions(self) -> str:
        """Generate utility functions"""
        content = '''# Utility functions

def get_block_id(block_name: str) -> Optional[int]:
    """Get block ID by name"""
    return BLOCK_NAME_TO_ID.get(block_name)

def get_block_name(block_id: int) -> Optional[str]:
    """Get block name by ID"""
    return BLOCK_ID_TO_NAME.get(block_id)

def is_valid_block_id(block_id: int) -> bool:
    """Check if block ID is valid"""
    return block_id in BLOCK_ID_TO_NAME

def is_valid_block_name(block_name: str) -> bool:
    """Check if block name is valid"""
    return block_name in BLOCK_NAME_TO_ID

def get_block_category(block_name: str) -> Optional[str]:
    """Get category for a block"""
    return BLOCK_TO_CATEGORY.get(block_name)

def get_blocks_in_category(category: str) -> Set[str]:
    """Get all block names in a category"""
    return BLOCK_CATEGORIES.get(category, set())

def get_all_categories() -> List[str]:
    """Get list of all categories"""
    return list(BLOCK_CATEGORIES.keys())

def get_all_block_names() -> List[str]:
    """Get list of all block names"""
    return list(BLOCK_NAME_TO_ID.keys())

def get_all_block_ids() -> List[int]:
    """Get list of all block IDs"""
    return list(BLOCK_ID_TO_NAME.keys())

# Category checking functions
'''
        
        # Generate category checking functions
        for category_name in self.categorized_blocks.keys():
            func_name = f"is_{category_name}"
            content += f"def {func_name}(block_name: str) -> bool:\n"
            content += f'    """Check if block is in {category_name} category"""\n'
            content += f"    return get_block_category(block_name) == '{category_name}'\n\n"
        
        # Version info
        content += f'''# Generation metadata
GENERATION_INFO = {{
    "timestamp": "{datetime.now().isoformat()}",
    "total_blocks": {self.id_registry["next_available_id"]},
    "categories": {list(self.categorized_blocks.keys())},
    "blocks_per_category": {{
'''
        
        for category_name, blocks in self.categorized_blocks.items():
            content += f'        "{category_name}": {len(blocks)},\n'
        
        content += "    }\n"
        content += "}\n\n"
        
        return content
    
    def generate_all(self):
        """Generate the complete Python mapping file"""
        print("=== Python Code Generation ===")
        
        # Load data
        self.load_data()
        
        print("Generating block_mapping_generated.py...")
        
        # Generate complete file content
        content = self.generate_file_header()
        content += self.generate_voxel_enum()
        content += self.generate_constants()
        content += self.generate_lookup_tables()
        content += self.generate_utility_functions()
        
        # Write to file
        with open(self.output_path, 'w') as f:
            f.write(content)
        
        print(f"  Written: {self.output_path}")
        print("✅ Python code generation complete!")
        return True

def main():
    """Main entry point"""
    import sys
    
    # Get project root (assume script is in scripts/generators/)
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    
    generator = PythonGenerator(str(project_root))
    
    try:
        success = generator.generate_all()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"❌ Error during Python generation: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
