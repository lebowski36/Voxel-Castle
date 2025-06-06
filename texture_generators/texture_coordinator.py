"""
Texture Coordinator
==================

Main interface for generating all block textures using the modular system.
This coordinates between different texture generators and handles per-face logic.
"""

from PIL import Image, ImageDraw
from typing import Dict, Tuple, Optional, List
import random

from texture_generators.base_patterns import *
from texture_generators.color_palettes import *
# Import enhanced generator functions
from texture_generators.stone_textures_enhanced import generate_stone_texture, generate_processed_stone_texture
from texture_generators.wood_textures_enhanced import generate_wood_texture, generate_processed_wood_texture

class TextureCoordinator:
    """
    Central coordinator for all texture generation.
    Handles per-face logic and delegates to appropriate generators.
    """
    
    def __init__(self, tile_size: int = 32, seed: Optional[int] = None):
        """
        Initialize the texture coordinator.
        
        Args:
            tile_size: Size of each texture tile (e.g., 32x32 pixels)
            seed: Random seed for reproducible texture generation
        """
        self.tile_size = tile_size
        if seed is not None:
            random.seed(seed)
        
        # Initialize all generators
        self.stone_gen = StoneTextureGenerator()
        self.wood_gen = WoodTextureGenerator()
        self.ore_gen = OreTextureGenerator()
        self.organic_gen = OrganicTextureGenerator()
        self.special_gen = SpecialTextureGenerator()
        self.crystal_gen = CrystalTextureGenerator()
        self.ceramic_gen = CeramicTextureGenerator()
        self.metal_gen = MetalTextureGenerator()
        self.fluid_gen = FluidTextureGenerator()
        
        # Map VoxelType enum values to generation functions
        self.texture_map = self._build_texture_map()
    
    def _build_texture_map(self) -> Dict[int, callable]:
        """Build mapping from voxel type ID to generation function."""
        return {
            # TERRAIN & NATURAL (0-49)
            # Basic Terrain (0-9)
            0: self._generate_air,           # AIR
            1: self._generate_stone_basic,   # STONE
            2: self._generate_dirt,          # DIRT
            3: self._generate_grass,         # GRASS
            4: self._generate_sand,          # SAND
            5: self._generate_gravel,        # GRAVEL
            6: self._generate_clay,          # CLAY
            7: self._generate_bedrock,       # BEDROCK
            8: self._generate_topsoil,       # TOPSOIL
            9: self._generate_subsoil,       # SUBSOIL
            
            # Stone Varieties (10-19)
            10: self._generate_granite,      # GRANITE
            11: self._generate_limestone,    # LIMESTONE
            12: self._generate_marble,       # MARBLE
            13: self._generate_sandstone,    # SANDSTONE
            14: self._generate_slate,        # SLATE
            15: self._generate_basalt,       # BASALT
            16: self._generate_quartzite,    # QUARTZITE
            17: self._generate_obsidian,     # OBSIDIAN
            18: self._generate_pumice,       # PUMICE
            19: self._generate_shale,        # SHALE
            
            # Ores & Minerals (20-29)
            20: self._generate_coal_ore,     # COAL_ORE
            21: self._generate_iron_ore,     # IRON_ORE
            22: self._generate_copper_ore,   # COPPER_ORE
            23: self._generate_tin_ore,      # TIN_ORE
            24: self._generate_silver_ore,   # SILVER_ORE
            25: self._generate_gold_ore,     # GOLD_ORE
            26: self._generate_gem_ruby,     # GEM_RUBY
            27: self._generate_gem_sapphire, # GEM_SAPPHIRE
            28: self._generate_gem_emerald,  # GEM_EMERALD
            29: self._generate_gem_diamond,  # GEM_DIAMOND
            
            # Organic Natural (30-39)
            30: self._generate_wood_oak,     # WOOD_OAK
            31: self._generate_wood_pine,    # WOOD_PINE
            32: self._generate_wood_birch,   # WOOD_BIRCH
            33: self._generate_wood_mahogany,# WOOD_MAHOGANY
            34: self._generate_leaves_oak,   # LEAVES_OAK
            35: self._generate_leaves_pine,  # LEAVES_PINE
            36: self._generate_leaves_birch, # LEAVES_BIRCH
            37: self._generate_leaves_palm,  # LEAVES_PALM
            38: self._generate_mushroom_brown, # MUSHROOM_BROWN
            39: self._generate_mushroom_red,   # MUSHROOM_RED
            
            # Biome Specific (40-49)
            40: self._generate_snow,         # SNOW
            41: self._generate_ice,          # ICE
            42: self._generate_packed_ice,   # PACKED_ICE
            43: self._generate_cactus,       # CACTUS
            44: self._generate_jungle_vine,  # JUNGLE_VINE
            45: self._generate_coral_pink,   # CORAL_PINK
            46: self._generate_coral_blue,   # CORAL_BLUE
            47: self._generate_seaweed,      # SEAWEED
            48: self._generate_tundra_moss,  # TUNDRA_MOSS
            49: self._generate_desert_rock,  # DESERT_ROCK
            
            # FLUIDS & GASES (50-59)
            50: self._generate_water,        # WATER
            51: self._generate_lava,         # LAVA
            52: self._generate_oil,          # OIL
            53: self._generate_acid,         # ACID
            54: self._generate_honey,        # HONEY
            55: self._generate_steam,        # STEAM
            56: self._generate_toxic_gas,    # TOXIC_GAS
            57: self._generate_natural_gas,  # NATURAL_GAS
            58: self._generate_magical_mist, # MAGICAL_MIST
            59: self._generate_smoke,        # SMOKE
        }
    
    def generate_texture(self, voxel_type_id: int, face: str = 'all') -> Image.Image:
        """
        Generate texture for a specific voxel type and face.
        
        Args:
            voxel_type_id: The VoxelType enum value as integer
            face: Which face to generate ('top', 'bottom', 'side', 'all')
        
        Returns:
            PIL Image with the generated texture
        """
        # Create image
        img = Image.new('RGBA', (self.tile_size, self.tile_size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Get generation function
        gen_func = self.texture_map.get(voxel_type_id, self._generate_placeholder)
        
        # Generate texture
        gen_func(draw, 0, 0, self.tile_size, face)
        
        return img
    
    def generate_all_textures(self) -> Dict[int, Dict[str, Image.Image]]:
        """
        Generate all textures for all voxel types.
        
        Returns:
            Dictionary mapping voxel_type_id -> face -> Image
        """
        all_textures = {}
        
        for voxel_type_id in self.texture_map.keys():
            all_textures[voxel_type_id] = {
                'top': self.generate_texture(voxel_type_id, 'top'),
                'bottom': self.generate_texture(voxel_type_id, 'bottom'),
                'side': self.generate_texture(voxel_type_id, 'side')
            }
        
        return all_textures
    
    # ========== TERRAIN & NATURAL GENERATORS ==========
    
    def _generate_air(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Air - transparent"""
        # Completely transparent, no drawing needed
        pass
    
    def _generate_stone_basic(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Basic stone"""
        self.stone_gen.generate_basic_stone(draw, x0, y0, size, face)
    
    def _generate_dirt(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Dirt with organic particles"""
        draw_speckled_pattern(draw, x0, y0, size, DIRT_BROWN, density=6, variation=25)
        # Add some organic flecks
        for _ in range(size // 4):
            fx = random.randint(x0, x0 + size - 1)
            fy = random.randint(y0, y0 + size - 1)
            fleck_color = random.choice([
                (80, 50, 30, 255),   # Dark organic
                (60, 40, 20, 255),   # Root fragment
                (100, 60, 40, 255)   # Clay particle
            ])
            draw.point((fx, fy), fill=fleck_color)
    
    def _generate_grass(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Grass with different faces"""
        if face == 'top':
            # Grass top - green with blade patterns
            draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], 
                         fill=GRASS_GREEN['base'])
            
            # Add grass blade details
            for _ in range(size // 2):
                bx = random.randint(x0, x0 + size - 1)
                by = random.randint(y0, y0 + size - 1)
                blade_color = vary_color(GRASS_GREEN['light'], 20)
                draw.point((bx, by), fill=blade_color)
        
        elif face == 'bottom':
            # Grass bottom - dirt
            self._generate_dirt(draw, x0, y0, size, face)
        
        else:  # side
            # Grass side - dirt bottom, grass top
            mid_y = y0 + (size * 3) // 4  # Grass takes top 1/4
            
            # Dirt portion (bottom)
            draw.rectangle([x0, mid_y, x0 + size - 1, y0 + size - 1], 
                         fill=DIRT_BROWN['base'])
            
            # Grass portion (top)
            draw.rectangle([x0, y0, x0 + size - 1, mid_y - 1], 
                         fill=GRASS_GREEN['base'])
            
            # Add transition detail
            for x in range(x0, x0 + size):
                if random.random() < 0.3:  # 30% chance of grass overhang
                    overhang_y = mid_y + random.randint(0, 2)
                    if overhang_y < y0 + size:
                        draw.point((x, overhang_y), fill=GRASS_GREEN['dark'])
    
    def _generate_sand(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Sandy texture"""
        draw_speckled_pattern(draw, x0, y0, size, SAND_YELLOW, density=4, variation=15)
    
    def _generate_gravel(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Gravel - mixed small stones"""
        base_color = (120, 110, 100, 255)
        draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], fill=base_color)
        
        # Add various colored pebbles
        pebble_colors = [
            (140, 130, 120, 255),  # Light gray
            (100, 90, 80, 255),    # Dark gray
            (130, 115, 95, 255),   # Brown
            (110, 105, 100, 255)   # Medium gray
        ]
        
        for _ in range(size // 2):
            px = random.randint(x0, x0 + size - 1)
            py = random.randint(y0, y0 + size - 1)
            pebble_color = random.choice(pebble_colors)
            
            # Draw small pebble (1-3 pixels)
            pebble_size = random.randint(1, 3)
            for dx in range(pebble_size):
                for dy in range(pebble_size):
                    if px + dx < x0 + size and py + dy < y0 + size:
                        draw.point((px + dx, py + dy), fill=pebble_color)
    
    def _generate_clay(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Clay texture"""
        self.ceramic_gen.generate_raw_clay(draw, x0, y0, size, face)
    
    def _generate_bedrock(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Indestructible bedrock"""
        draw_speckled_pattern(draw, x0, y0, size, BEDROCK_DARK, density=10, variation=10)
        
        # Add some crystalline flecks for magical appearance
        for _ in range(size // 8):
            fx = random.randint(x0, x0 + size - 1)
            fy = random.randint(y0, y0 + size - 1)
            fleck_color = (80, 80, 120, 255)  # Slightly blue tint
            draw.point((fx, fy), fill=fleck_color)
    
    def _generate_topsoil(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Rich topsoil"""
        draw_speckled_pattern(draw, x0, y0, size, TOPSOIL_RICH, density=6, variation=20)
        
        # Add organic matter
        for _ in range(size // 6):
            ox = random.randint(x0, x0 + size - 1)
            oy = random.randint(y0, y0 + size - 1)
            organic_color = (40, 25, 15, 255)  # Dark organic matter
            draw.point((ox, oy), fill=organic_color)
    
    def _generate_subsoil(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Poor subsoil"""
        draw_speckled_pattern(draw, x0, y0, size, SUBSOIL_PALE, density=8, variation=15)
    
    # Stone Varieties (10-19)
    def _generate_granite(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_granite(draw, x0, y0, size, face)
    
    def _generate_limestone(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_limestone(draw, x0, y0, size, face)
    
    def _generate_marble(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_marble(draw, x0, y0, size, face)
    
    def _generate_sandstone(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_sandstone(draw, x0, y0, size, face)
    
    def _generate_slate(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_slate(draw, x0, y0, size, face)
    
    def _generate_basalt(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_basalt(draw, x0, y0, size, face)
    
    def _generate_quartzite(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_quartzite(draw, x0, y0, size, face)
    
    def _generate_obsidian(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_obsidian(draw, x0, y0, size, face)
    
    def _generate_pumice(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_pumice(draw, x0, y0, size, face)
    
    def _generate_shale(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_shale(draw, x0, y0, size, face)
    
    # Ores & Minerals (20-29)
    def _generate_coal_ore(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.ore_gen.generate_coal_ore(draw, x0, y0, size, face)
    
    def _generate_iron_ore(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.ore_gen.generate_iron_ore(draw, x0, y0, size, face)
    
    def _generate_copper_ore(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.ore_gen.generate_copper_ore(draw, x0, y0, size, face)
    
    def _generate_tin_ore(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.ore_gen.generate_tin_ore(draw, x0, y0, size, face)
    
    def _generate_silver_ore(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.ore_gen.generate_silver_ore(draw, x0, y0, size, face)
    
    def _generate_gold_ore(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.ore_gen.generate_gold_ore(draw, x0, y0, size, face)
    
    def _generate_gem_ruby(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.crystal_gen.generate_ruby(draw, x0, y0, size, face)
    
    def _generate_gem_sapphire(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.crystal_gen.generate_sapphire(draw, x0, y0, size, face)
    
    def _generate_gem_emerald(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.crystal_gen.generate_emerald(draw, x0, y0, size, face)
    
    def _generate_gem_diamond(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.crystal_gen.generate_diamond(draw, x0, y0, size, face)
    
    # Wood types (30-33)
    def _generate_wood_oak(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.wood_gen.generate_oak(draw, x0, y0, size, face)
    
    def _generate_wood_pine(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.wood_gen.generate_pine(draw, x0, y0, size, face)
    
    def _generate_wood_birch(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.wood_gen.generate_birch(draw, x0, y0, size, face)
    
    def _generate_wood_mahogany(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.wood_gen.generate_mahogany(draw, x0, y0, size, face)
    
    # Leaves (34-37)
    def _generate_leaves_oak(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_oak_leaves(draw, x0, y0, size, face)
    
    def _generate_leaves_pine(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_pine_needles(draw, x0, y0, size, face)
    
    def _generate_leaves_birch(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_birch_leaves(draw, x0, y0, size, face)
    
    def _generate_leaves_palm(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_palm_fronds(draw, x0, y0, size, face)
    
    # Mushrooms (38-39)
    def _generate_mushroom_brown(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_brown_mushroom(draw, x0, y0, size, face)
    
    def _generate_mushroom_red(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_red_mushroom(draw, x0, y0, size, face)
    
    # Biome specific (40-49)
    def _generate_snow(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.special_gen.generate_snow(draw, x0, y0, size, face)
    
    def _generate_ice(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.special_gen.generate_ice(draw, x0, y0, size, face)
    
    def _generate_packed_ice(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.special_gen.generate_packed_ice(draw, x0, y0, size, face)
    
    def _generate_cactus(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_cactus(draw, x0, y0, size, face)
    
    def _generate_jungle_vine(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_jungle_vine(draw, x0, y0, size, face)
    
    def _generate_coral_pink(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_pink_coral(draw, x0, y0, size, face)
    
    def _generate_coral_blue(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_blue_coral(draw, x0, y0, size, face)
    
    def _generate_seaweed(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_seaweed(draw, x0, y0, size, face)
    
    def _generate_tundra_moss(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.organic_gen.generate_tundra_moss(draw, x0, y0, size, face)
    
    def _generate_desert_rock(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.stone_gen.generate_desert_rock(draw, x0, y0, size, face)
    
    # Fluids (50-59)
    def _generate_water(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_water(draw, x0, y0, size, face)
    
    def _generate_lava(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_lava(draw, x0, y0, size, face)
    
    def _generate_oil(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_oil(draw, x0, y0, size, face)
    
    def _generate_acid(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_acid(draw, x0, y0, size, face)
    
    def _generate_honey(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_honey(draw, x0, y0, size, face)
    
    def _generate_steam(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_steam(draw, x0, y0, size, face)
    
    def _generate_toxic_gas(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_toxic_gas(draw, x0, y0, size, face)
    
    def _generate_natural_gas(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_natural_gas(draw, x0, y0, size, face)
    
    def _generate_magical_mist(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_magical_mist(draw, x0, y0, size, face)
    
    def _generate_smoke(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        self.fluid_gen.generate_smoke(draw, x0, y0, size, face)
    
    def _generate_placeholder(self, draw: ImageDraw.Draw, x0: int, y0: int, size: int, face: str):
        """Placeholder for unmapped voxel types"""
        # Pink/magenta placeholder to easily identify missing textures
        draw.rectangle([x0, y0, x0 + size - 1, y0 + size - 1], 
                      fill=(255, 0, 255, 255))
        
        # Add diagonal lines to make it obvious
        for i in range(0, size, 4):
            draw.line([(x0 + i, y0), (x0, y0 + i)], fill=(200, 0, 200, 255))
            draw.line([(x0 + size - 1, y0 + i), (x0 + size - 1 - i, y0 + size - 1)], 
                     fill=(200, 0, 200, 255))
