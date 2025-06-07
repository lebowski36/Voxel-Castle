#!/usr/bin/env python3
"""
Dynamic Atlas Calculator
Analyzes block data to determine optimal atlas sizes and layouts.
"""

import json
import os
import math
from typing import Dict, List, Tuple, Set
from enum import Enum

class FacePattern(Enum):
    """Face pattern types that match the C++ enum"""
    UNIFORM = "UNIFORM"
    TOP_BOTTOM_DIFFERENT = "TOP_BOTTOM_DIFFERENT"
    ALL_DIFFERENT = "ALL_DIFFERENT"
    DIRECTIONAL = "DIRECTIONAL"
    ALL_FACES_DIFFERENT = "ALL_FACES_DIFFERENT"

class AtlasRequirements:
    """Calculates atlas requirements from block data"""
    
    def __init__(self, data_dir: str = "data"):
        self.data_dir = data_dir
        self.blocks = {}
        self.load_all_blocks()
    
    def load_all_blocks(self):
        """Load all block data from JSON files"""
        blocks_dir = os.path.join(self.data_dir, "blocks")
        index_file = os.path.join(blocks_dir, "_index.json")
        
        if not os.path.exists(index_file):
            raise FileNotFoundError(f"Index file not found: {index_file}")
        
        with open(index_file, 'r') as f:
            index_data = json.load(f)
        
        self.blocks = {}
        for category_name, category_info in index_data["metadata"]["categories"].items():
            category_file = os.path.join(blocks_dir, category_info["file"])
            if os.path.exists(category_file):
                with open(category_file, 'r') as f:
                    category_data = json.load(f)
                    self.blocks.update(category_data["blocks"])
        
        print(f"✓ Loaded {len(self.blocks)} blocks from {len(index_data['metadata']['categories'])} categories")
    
    def analyze_face_patterns(self) -> Dict[str, List[int]]:
        """Analyze which blocks need which atlas types"""
        atlas_requirements = {
            "main": [],      # All blocks need main atlas
            "side": [],      # TOP_BOTTOM_DIFFERENT, ALL_DIFFERENT, ALL_FACES_DIFFERENT
            "bottom": []     # ALL_DIFFERENT, ALL_FACES_DIFFERENT
        }
        
        pattern_counts = {}
        
        for block_name, block_data in self.blocks.items():
            block_id = block_data["id"]
            face_pattern = block_data.get("face_pattern", "UNIFORM")
            
            # Count patterns
            pattern_counts[face_pattern] = pattern_counts.get(face_pattern, 0) + 1
            
            # All blocks need main atlas slot
            atlas_requirements["main"].append(block_id)
            
            # Determine additional atlas requirements
            if face_pattern in ["TOP_BOTTOM_DIFFERENT", "ALL_DIFFERENT", "ALL_FACES_DIFFERENT"]:
                atlas_requirements["side"].append(block_id)
            
            if face_pattern in ["ALL_DIFFERENT", "ALL_FACES_DIFFERENT"]:
                atlas_requirements["bottom"].append(block_id)
        
        print(f"\n📊 Face Pattern Analysis:")
        for pattern, count in sorted(pattern_counts.items()):
            percentage = (count / len(self.blocks)) * 100
            print(f"  {pattern:20s}: {count:3d} blocks ({percentage:5.1f}%)")
        
        return atlas_requirements
    
    def calculate_atlas_grid_size(self, num_slots: int) -> Tuple[int, int]:
        """Calculate optimal grid size for given number of slots"""
        if num_slots == 0:
            return (1, 1)  # Minimum size
        
        # Find the smallest square that fits all slots
        side_length = math.ceil(math.sqrt(num_slots))
        return (side_length, side_length)
    
    def calculate_atlas_files_needed(self, num_slots: int, max_grid_size: int = 16) -> Tuple[int, int, int]:
        """Calculate how many atlas files needed for given slots"""
        max_slots_per_file = max_grid_size * max_grid_size
        
        if num_slots <= max_slots_per_file:
            # Fits in one file
            grid_size = self.calculate_atlas_grid_size(num_slots)[0]
            return (1, grid_size, grid_size)
        else:
            # Need multiple files
            num_files = math.ceil(num_slots / max_slots_per_file)
            return (num_files, max_grid_size, max_grid_size)
    
    def generate_atlas_plan(self) -> Dict:
        """Generate comprehensive atlas generation plan"""
        requirements = self.analyze_face_patterns()
        
        plan = {
            "summary": {
                "total_blocks": len(self.blocks),
                "timestamp": "2025-06-07"
            },
            "atlases": {}
        }
        
        print(f"\n🎯 Atlas Requirements:")
        
        for atlas_type, block_ids in requirements.items():
            num_blocks = len(block_ids)
            if num_blocks == 0:
                continue
                
            # Calculate file requirements
            num_files, grid_width, grid_height = self.calculate_atlas_files_needed(num_blocks)
            total_slots = num_files * grid_width * grid_height
            efficiency = (num_blocks / total_slots) * 100 if total_slots > 0 else 0
            
            plan["atlases"][atlas_type] = {
                "blocks_needed": num_blocks,
                "files_needed": num_files,
                "grid_size": [grid_width, grid_height],
                "total_slots": total_slots,
                "efficiency": round(efficiency, 1),
                "block_ids": sorted(block_ids)
            }
            
            print(f"  {atlas_type.upper():8s}: {num_blocks:3d} blocks → {num_files} file(s) of {grid_width}x{grid_height} ({efficiency:5.1f}% efficient)")
        
        return plan
    
    def save_atlas_plan(self, plan: Dict, output_file: str = "atlas_plan.json"):
        """Save atlas plan to JSON file"""
        with open(output_file, 'w') as f:
            json.dump(plan, f, indent=2)
        print(f"\n✓ Atlas plan saved to: {output_file}")
    
    def compare_with_current_system(self, plan: Dict):
        """Compare new dynamic system with current fixed system"""
        current_system = {
            "main": {"files": 1, "grid": [16, 16], "slots": 256},
            "side": {"files": 1, "grid": [3, 3], "slots": 9},
            "bottom": {"files": 1, "grid": [1, 1], "slots": 1}
        }
        
        print(f"\n📈 System Comparison (Current vs Dynamic):")
        print(f"{'Atlas':<8} {'Current':<15} {'Dynamic':<15} {'Savings':<10}")
        print("-" * 50)
        
        total_current_slots = 0
        total_dynamic_slots = 0
        
        for atlas_type, dynamic_data in plan["atlases"].items():
            if atlas_type in current_system:
                current = current_system[atlas_type]
                current_slots = current["slots"]
                dynamic_slots = dynamic_data["total_slots"]
                
                savings = current_slots - dynamic_slots
                savings_pct = (savings / current_slots) * 100 if current_slots > 0 else 0
                
                print(f"{atlas_type:<8} {current_slots:<15} {dynamic_slots:<15} {savings:3d} ({savings_pct:4.1f}%)")
                
                total_current_slots += current_slots
                total_dynamic_slots += dynamic_slots
        
        total_savings = total_current_slots - total_dynamic_slots
        total_savings_pct = (total_savings / total_current_slots) * 100 if total_current_slots > 0 else 0
        
        print("-" * 50)
        print(f"{'TOTAL':<8} {total_current_slots:<15} {total_dynamic_slots:<15} {total_savings:3d} ({total_savings_pct:4.1f}%)")

def main():
    """Generate and analyze atlas requirements"""
    print("🧮 Dynamic Atlas Calculator")
    print("=" * 40)
    
    try:
        calculator = AtlasRequirements()
        plan = calculator.generate_atlas_plan()
        calculator.compare_with_current_system(plan)
        calculator.save_atlas_plan(plan)
        
        print(f"\n✅ Atlas analysis complete!")
        return True
        
    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    main()
