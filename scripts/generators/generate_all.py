#!/usr/bin/env python3
"""
Master Code Generation Script

Runs all code generators in the correct order:
1. ID Management (assign stable IDs)
2. C++ Code Generation (headers and source)
3. Python Code Generation (mappings and utilities)

This script ensures all generated code is consistent and up-to-date.
"""

import os
import sys
import subprocess
from pathlib import Path
from datetime import datetime

def run_generator(script_path: Path, description: str, project_root: Path) -> bool:
    """Run a generator script and return success status"""
    print(f"\n🔨 {description}")
    print(f"   Running: {script_path}")
    
    try:
        # Run the script
        result = subprocess.run([
            sys.executable, str(script_path)
        ], capture_output=True, text=True, cwd=project_root)
        
        if result.returncode == 0:
            print(f"✅ {description} completed successfully")
            if result.stdout.strip():
                # Indent output for better readability
                for line in result.stdout.strip().split('\n'):
                    print(f"   {line}")
            return True
        else:
            print(f"❌ {description} failed")
            print(f"   Exit code: {result.returncode}")
            if result.stdout.strip():
                print("   STDOUT:")
                for line in result.stdout.strip().split('\n'):
                    print(f"     {line}")
            if result.stderr.strip():
                print("   STDERR:")
                for line in result.stderr.strip().split('\n'):
                    print(f"     {line}")
            return False
            
    except Exception as e:
        print(f"❌ {description} failed with exception: {e}")
        return False

def main():
    """Main generation workflow"""
    print("=" * 60)
    print("🏗️  VOXEL ENGINE CODE GENERATION")
    print("=" * 60)
    print(f"Started at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    
    # Get project root (assume script is in scripts/generators/)
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    
    print(f"Project root: {project_root}")
    
    # Define generation steps
    generators = [
        (script_dir / "id_manager.py", "ID Registry Management"),
        (script_dir / "cpp_generator.py", "C++ Code Generation"),
        (script_dir / "python_generator.py", "Python Code Generation"),
    ]
    
    # Track results
    results = []
    total_start = datetime.now()
    
    # Run each generator
    for script_path, description in generators:
        if not script_path.exists():
            print(f"❌ Generator script not found: {script_path}")
            results.append(False)
            continue
            
        step_start = datetime.now()
        success = run_generator(script_path, description, project_root)
        step_duration = datetime.now() - step_start
        results.append(success)
        
        print(f"   Duration: {step_duration.total_seconds():.2f}s")
    
    # Summary
    total_duration = datetime.now() - total_start
    successful = sum(results)
    total = len(results)
    
    print("\n" + "=" * 60)
    print("📊 GENERATION SUMMARY")
    print("=" * 60)
    print(f"Total steps: {total}")
    print(f"Successful: {successful}")
    print(f"Failed: {total - successful}")
    print(f"Total duration: {total_duration.total_seconds():.2f}s")
    
    if all(results):
        print("\n🎉 ALL GENERATORS COMPLETED SUCCESSFULLY!")
        print("\nGenerated files should now be up-to-date:")
        print("  - data/id_registry.json")
        print("  - engine/include/generated/voxel_types_generated.h")
        print("  - engine/src/generated/voxel_types_generated.cpp")
        print("  - block_mapping_generated.py")
        print("\nNext steps:")
        print("  1. Build the project to compile new C++ code")
        print("  2. Update imports in existing Python scripts")
        print("  3. Gradually migrate legacy hardcoded IDs")
        return 0
    else:
        print("\n💥 SOME GENERATORS FAILED!")
        print("Please check the error messages above and fix any issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
