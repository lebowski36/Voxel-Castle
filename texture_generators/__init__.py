"""
Texture Generators Package
=========================

Modular texture generation system for Voxel Castle's comprehensive block system.
Supports 256+ block types with reusable patterns and multi-face textures.
"""

__version__ = "1.0.0"
__author__ = "Voxel Castle Development Team"

# Import core modules for easy access
from .base_patterns import *
from .color_palettes import *

# Generator categories
from . import stone_textures
from . import ore_textures  
from . import wood_textures
from . import organic_textures
from . import fluid_textures
from . import metal_textures
from . import ceramic_textures
from . import crystal_textures
from . import special_textures
