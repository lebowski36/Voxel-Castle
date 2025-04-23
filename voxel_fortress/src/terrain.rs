use bevy::prelude::*;
use bevy::render::mesh::{Mesh, Indices, PrimitiveTopology};
use noise::{NoiseFn, Perlin, RidgedMulti};

// --- Voxel Chunk Data Structure ---

pub const CHUNK_SIZE: usize = 32;
pub const VOXEL_SIZE_METERS: f32 = 0.25; // 25cm

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum Voxel {
    Air,
    Solid,
    Stone,
    Dirt,
    Grass,
    Sand,
    Forest,
    Plains,
    Taiga,
    Desert,
    Snow,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ChunkLodState {
    Active,   // High detail, full mesh
    LOD,      // Low detail, heightmap mesh
    Unloaded, // Not loaded/generated
}

pub struct Chunk {
    pub voxels: [[[Voxel; CHUNK_SIZE]; CHUNK_SIZE]; CHUNK_SIZE],
    pub lod_state: ChunkLodState, // Track LOD state for this chunk
}

impl Chunk {
    pub fn new_filled(voxel: Voxel) -> Self {
        Self {
            voxels: [[[voxel; CHUNK_SIZE]; CHUNK_SIZE]; CHUNK_SIZE],
            lod_state: ChunkLodState::Active,
        }
    }
    // Simple heightmap-based chunk generation
    pub fn from_heightmap(heightmap: &Vec<Vec<f64>>, y_offset: usize) -> Self {
        let mut chunk = Self::new_filled(Voxel::Air);
        for x in 0..CHUNK_SIZE {
            for z in 0..CHUNK_SIZE {
                let height = ((heightmap[z][x] + 1.0) * 0.5 * (CHUNK_SIZE as f64)) as usize;
                for y in 0..CHUNK_SIZE {
                    if y + y_offset < height {
                        chunk.voxels[x][y][z] = Voxel::Solid;
                    }
                }
            }
        }
        chunk
    }

    pub fn from_worldgen(worldgen: &WorldGen, chunk_pos: (i64, i64, i64)) -> Self {
        let mut chunk = Self::new_filled(Voxel::Air);
        let base_x = chunk_pos.0 * CHUNK_SIZE as i64;
        let base_y = chunk_pos.1 * CHUNK_SIZE as i64;
        let base_z = chunk_pos.2 * CHUNK_SIZE as i64;
        
        // Use 3D noise for terrain generation with proper features
        for x in 0..CHUNK_SIZE {
            for z in 0..CHUNK_SIZE {
                let wx = base_x + x as i64;
                let wz = base_z + z as i64;
                
                // Get surface height at this x,z coordinate
                let surface_height = worldgen.get_height(wx as f64, wz as f64);
                let biome = worldgen.get_biome(wx as f64, wz as f64);
                
                // Water level (sea level is at y=0)
                let water_level = 0.0;
                
                // Fill in this column of voxels using true 3D noise
                for y in 0..CHUNK_SIZE {
                    let wy = base_y + y as i64;
                    let world_y = wy as f64;
                    
                    // For underwater terrain, use a simpler system
                    if world_y < water_level {
                        // Simple underwater terrain
                        if world_y < surface_height {
                            // Underwater ground
                            chunk.voxels[x][y][z] = Voxel::Solid;
                        } else {
                            // Water will be handled separately in the future
                            chunk.voxels[x][y][z] = Voxel::Air;
                        }
                    } else {
                        // Above water, use full 3D density function for terrain
                        let density = worldgen.get_density(wx as f64, world_y, wz as f64, surface_height);
                        
                        // Positive density means solid terrain
                        if density > 0.0 {
                            // Set voxel type based on biome and height
                            chunk.voxels[x][y][z] = match biome {
                                0 => Voxel::Forest, // Forest
                                1 => Voxel::Plains, // Plains
                                2 => Voxel::Taiga,  // Taiga
                                3 => Voxel::Desert, // Desert
                                _ => Voxel::Stone,
                            };
                        }
                    }
                }
            }
        }
        chunk
    }

    /// Generate a mesh for all visible faces in this chunk (naive greedy meshing)
    pub fn to_mesh(&self) -> Mesh {
        let mut positions = Vec::new();
        let mut normals = Vec::new();
        let mut colors = Vec::new();
        let mut indices = Vec::new();
        let mut uvs = Vec::new();
        let mut i = 0u32;

        for x in 0..CHUNK_SIZE {
            for y in 0..CHUNK_SIZE {
                for z in 0..CHUNK_SIZE {
                    if self.voxels[x][y][z] == Voxel::Solid {
                        for (dx, dy, dz, normal, face_type) in [
                            (0, 0, -1, [0.0, 0.0, -1.0], "side"),
                            (0, 0, 1, [0.0, 0.0, 1.0], "side"),
                            (0, -1, 0, [0.0, -1.0, 0.0], "bottom"),
                            (0, 1, 0, [0.0, 1.0, 0.0], "top"),
                            (-1, 0, 0, [-1.0, 0.0, 0.0], "side"),
                            (1, 0, 0, [1.0, 0.0, 0.0], "side"),
                        ] {
                            let nx = x as isize + dx;
                            let ny = y as isize + dy;
                            let nz = z as isize + dz;
                            let neighbor = if nx < 0 || ny < 0 || nz < 0 ||
                                nx >= CHUNK_SIZE as isize || ny >= CHUNK_SIZE as isize || nz >= CHUNK_SIZE as isize {
                                Voxel::Air
                            } else {
                                self.voxels[nx as usize][ny as usize][nz as usize]
                            };
                            if neighbor == Voxel::Air {
                                let base = Vec3::new(x as f32, y as f32, z as f32) * VOXEL_SIZE_METERS;
                                let (face_verts, face_normals, face_indices) = face_mesh(base, normal, i);
                                positions.extend(face_verts.iter().cloned());
                                normals.extend(face_normals.iter().cloned());
                                uvs.extend(vec![[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]]);

                                // Get terrain colors based on voxel type and face
                                let above = if y + 1 < CHUNK_SIZE { self.voxels[x][y + 1][z] } else { Voxel::Air };
                                let t = y as f32 / (CHUNK_SIZE as f32 - 1.0);
                                
                                // Define colors for different voxel types
                                let stone_color = [0.5, 0.5, 0.5, 1.0];
                                let dirt_color = [0.45, 0.32, 0.18, 1.0];
                                let grass_side_color = [0.45, 0.32, 0.18, 1.0]; 
                                let grass_top_color = [0.2, 0.8, 0.2, 1.0];
                                let sand_color = [0.76, 0.7, 0.5, 1.0];
                                let forest_color = [0.15, 0.6, 0.15, 1.0];
                                let plains_color = [0.3, 0.65, 0.3, 1.0];
                                let taiga_color = [0.2, 0.5, 0.4, 1.0];
                                let desert_color = [0.85, 0.75, 0.5, 1.0];
                                let snow_color = [0.9, 0.9, 0.95, 1.0];
                                
                                // Select color based on voxel type and face
                                let face_color = match self.voxels[x][y][z] {
                                    Voxel::Stone => stone_color,
                                    Voxel::Dirt => dirt_color,
                                    Voxel::Grass => {
                                        // Grass has green top, dirt sides
                                        match face_type {
                                            "top" => grass_top_color,
                                            _ => grass_side_color,
                                        }
                                    },
                                    Voxel::Sand => sand_color,
                                    Voxel::Forest => {
                                        match face_type {
                                            "top" => forest_color,
                                            _ => dirt_color,
                                        }
                                    },
                                    Voxel::Plains => {
                                        match face_type {
                                            "top" => plains_color,
                                            _ => dirt_color,
                                        }
                                    },
                                    Voxel::Taiga => {
                                        match face_type {
                                            "top" => taiga_color,
                                            _ => dirt_color,
                                        }
                                    },
                                    Voxel::Desert => desert_color,
                                    Voxel::Snow => snow_color,
                                    // Default for Solid or any other voxel type
                                    _ => {
                                        let brown = [0.45, 0.32, 0.18, 1.0];
                                        let green = [0.2, 0.8, 0.2, 1.0];
                                        match face_type {
                                            "top" if above == Voxel::Air => green,
                                            "side" => [
                                                brown[0] * (1.0 - t) + green[0] * t,
                                                brown[1] * (1.0 - t) + green[1] * t,
                                                brown[2] * (1.0 - t) + green[2] * t,
                                                1.0
                                            ],
                                            _ => brown,
                                        }
                                    },
                                };
                                colors.extend([face_color; 4]);
                                indices.extend(face_indices.iter().cloned());
                                i += 4;
                            }
                        }
                    }
                }
            }
        }

        if positions.is_empty() {
            // Avoid creating an empty mesh, which can panic in Bevy
            let mut empty_mesh = Mesh::new(PrimitiveTopology::TriangleList, bevy::render::render_asset::RenderAssetUsages::default());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, Vec::<[f32; 3]>::new());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, Vec::<[f32; 3]>::new());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_COLOR, Vec::<[f32; 4]>::new());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, Vec::<[f32; 2]>::new());
            empty_mesh.insert_indices(Indices::U32(Vec::new()));
            return empty_mesh;
        }

        let mut mesh = Mesh::new(PrimitiveTopology::TriangleList, bevy::render::render_asset::RenderAssetUsages::default());
        mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, positions);
        mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
        mesh.insert_attribute(Mesh::ATTRIBUTE_COLOR, colors);
        mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, uvs); // Insert UVs
        mesh.insert_indices(Indices::U32(indices));
        mesh
    }

    /// Generate a simple heightmap mesh for LOD rendering (single quad per column)
    pub fn to_lod_mesh(&self) -> Mesh {
        let mut positions = Vec::new();
        let mut normals = Vec::new();
        let mut colors = Vec::new();
        let mut indices = Vec::new();
        let mut uvs = Vec::new();
        let mut i = 0u32;

        // For each (x, z) column, find the highest solid voxel (surface)
        for x in 0..CHUNK_SIZE {
            for z in 0..CHUNK_SIZE {
                let mut max_y = None;
                for y in (0..CHUNK_SIZE).rev() {
                    if self.voxels[x][y][z] == Voxel::Solid {
                        max_y = Some(y);
                        break;
                    }
                }
                if let Some(y) = max_y {
                    let fx = x as f32 * VOXEL_SIZE_METERS;
                    let fz = z as f32 * VOXEL_SIZE_METERS;
                    let fy = y as f32 * VOXEL_SIZE_METERS;
                    // Create a single quad (top face) for this column
                    let base = Vec3::new(fx, fy, fz);
                    let (face_verts, face_normals, face_indices) = face_mesh(base, [0.0, 1.0, 0.0], i);
                    positions.extend(face_verts.iter().cloned());
                    normals.extend(face_normals.iter().cloned());
                    uvs.extend(vec![[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]]);
                    let green = [0.2, 0.8, 0.2, 1.0];
                    colors.extend([green; 4]);
                    indices.extend(face_indices.iter().cloned());
                    i += 4;
                }
            }
        }

        if positions.is_empty() {
            let mut empty_mesh = Mesh::new(PrimitiveTopology::TriangleList, bevy::render::render_asset::RenderAssetUsages::default());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, Vec::<[f32; 3]>::new());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, Vec::<[f32; 3]>::new());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_COLOR, Vec::<[f32; 4]>::new());
            empty_mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, Vec::<[f32; 2]>::new());
            empty_mesh.insert_indices(Indices::U32(Vec::new()));
            return empty_mesh;
        }

        let mut mesh = Mesh::new(PrimitiveTopology::TriangleList, bevy::render::render_asset::RenderAssetUsages::default());
        mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, positions);
        mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
        mesh.insert_attribute(Mesh::ATTRIBUTE_COLOR, colors);
        mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, uvs);
        mesh.insert_indices(Indices::U32(indices));
        mesh
    }
}

fn face_mesh(base: Vec3, normal: [f32; 3], i: u32) -> (Vec<[f32; 3]>, Vec<[f32; 3]>, Vec<u32>) {
    // Returns the 4 vertices, normals, and 6 indices for a quad face at base with the given normal
    let (dx, dy, dz) = (normal[0], normal[1], normal[2]);
    let size = VOXEL_SIZE_METERS;
    let (v0, v1, v2, v3, inds) = match (dx, dy, dz) {
        // -Z (back)
        (0.0, 0.0, -1.0) => (
            [base.x, base.y, base.z],
            [base.x + size, base.y, base.z],
            [base.x + size, base.y + size, base.z],
            [base.x, base.y + size, base.z],
            vec![i, i + 1, i + 2, i, i + 2, i + 3],
        ),
        // +Z (front)
        (0.0, 0.0, 1.0) => (
            [base.x, base.y, base.z + size],
            [base.x + size, base.y, base.z + size],
            [base.x + size, base.y + size, base.z + size],
            [base.x, base.y + size, base.z + size],
            vec![i, i + 2, i + 1, i, i + 3, i + 2], // flip winding
        ),
        // -Y (bottom)
        (0.0, -1.0, 0.0) => (
            [base.x, base.y, base.z],
            [base.x + size, base.y, base.z],
            [base.x + size, base.y, base.z + size],
            [base.x, base.y, base.z + size],
            vec![i, i + 1, i + 2, i, i + 2, i + 3],
        ),
        // +Y (top)
        (0.0, 1.0, 0.0) => (
            [base.x, base.y + size, base.z],
            [base.x, base.y + size, base.z + size],
            [base.x + size, base.y + size, base.z + size],
            [base.x + size, base.y + size, base.z],
            vec![i, i + 1, i + 2, i, i + 2, i + 3],
        ),
        // -X (left)
        (-1.0, 0.0, 0.0) => (
            [base.x, base.y, base.z],
            [base.x, base.y + size, base.z],
            [base.x, base.y + size, base.z + size],
            [base.x, base.y, base.z + size],
            vec![i, i + 1, i + 2, i, i + 2, i + 3],
        ),
        // +X (right)
        (1.0, 0.0, 0.0) => (
            [base.x + size, base.y, base.z],
            [base.x + size, base.y, base.z + size],
            [base.x + size, base.y + size, base.z + size],
            [base.x + size, base.y + size, base.z],
            vec![i, i + 1, i + 2, i, i + 2, i + 3],
        ),
        _ => unreachable!(),
    };
    let verts = vec![v0, v1, v2, v3];
    let norms = vec![normal; 4];
    (verts, norms, inds)
}

/// Hierarchical chunk addressing for N levels
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct HierarchicalChunkAddr {
    pub level: u8, // 0 = block chunk, 1 = parent, ...
    pub x: i64,
    pub y: i64,
    pub z: i64,
}

/// High-level chunk node (for LOD, worldgen, and streaming)
pub struct ChunkNode {
    pub addr: HierarchicalChunkAddr,
    pub children: Option<Box<[ChunkNode]>>, // Only for non-leaf nodes, can be generated on demand
    pub summary: Option<ChunkSummary>, // Biome, height, etc.
}

/// Summary data for high-level chunks
#[derive(Clone, Debug)]
pub struct ChunkSummary {
    pub biome: u8, // For now, just an index
    pub avg_height: f32,
    // Add more as needed
}

/// WorldGen struct for hierarchical worldgen
#[derive(Resource, Clone)]
pub struct WorldGen {
    pub levels: u8, // Number of chunk levels
    pub chunk_size: usize, // Size per chunk (e.g., 32)
    pub seed: u64,
    pub base_noise: Perlin,
    pub ridged_noise: RidgedMulti<Perlin>,
    pub mountain_noise: Perlin,
    pub detail_noise: Perlin,
    pub cave_noise: Perlin,
    // Additional noise generators for enhanced terrain
    pub warp_x_noise: Perlin,
    pub warp_z_noise: Perlin,
}

impl WorldGen {
    pub fn new(levels: u8, chunk_size: usize, seed: u64) -> Self {
        Self {
            levels,
            chunk_size,
            seed,
            base_noise: Perlin::new(seed as u32),
            ridged_noise: RidgedMulti::new(seed as u32 + 1),
            mountain_noise: Perlin::new(seed as u32 + 2),
            detail_noise: Perlin::new(seed as u32 + 3),
            cave_noise: Perlin::new(seed as u32 + 4),
            warp_x_noise: Perlin::new(seed as u32 + 5),
            warp_z_noise: Perlin::new(seed as u32 + 6),
        }
    }
    
    /// Get the base height for a given world position (x, z) using multi-octave noise and domain warping
    pub fn get_height(&self, x: f64, z: f64) -> f64 {
        // Apply domain warping for more natural terrain
        let warp_scale = 0.001;
        let warp_strength = 150.0;
        
        // Create warped coordinates for more natural terrain flow
        let warp_x = self.warp_x_noise.get([x * warp_scale, z * warp_scale]) * warp_strength;
        let warp_z = self.warp_z_noise.get([x * warp_scale + 1000.0, z * warp_scale + 1000.0]) * warp_strength;
        
        let nx = x + warp_x;
        let nz = z + warp_z;
        
        // Base continent noise (large scale features)
        let continent_scale = 0.00025;
        let continent = self.base_noise.get([nx * continent_scale, nz * continent_scale]);
        let continent_height = (continent * 0.5 + 0.5) * 100.0; // 0-100 range
        
        // Mountain ranges
        let mountain_scale = 0.001;
        let mountains = self.mountain_noise.get([nx * mountain_scale, nz * mountain_scale]);
        let mountains_mask = (mountains + 0.1).max(0.0).min(1.0); // Threshold to create distinct mountain ranges
        let mountain_height = self.ridged_noise.get([nx * mountain_scale * 2.0, nz * mountain_scale * 2.0]) * 120.0;
        
        // Hills and details
        let hills_scale = 0.003;
        let hills = self.detail_noise.get([nx * hills_scale, nz * hills_scale]) * 25.0;
        
        // Combine all terrain features with proper weighting
        let base_terrain = continent_height;
        let mountains_contribution = mountain_height * mountains_mask.powf(1.5);
        
        // Final height calculation with smoothing between features
        let height = base_terrain + mountains_contribution + hills;
        
        height
    }
    
    /// Get 3D density value at a specific world position (true 3D noise)
    pub fn get_density(&self, x: f64, y: f64, z: f64, surface_height: f64) -> f64 {
        // Surface gradient - more solid as we go deeper
        let depth = surface_height - y;
        let surface_density = if depth > 0.0 { depth / 5.0 } else { -1.0 };
        
        // Cave systems
        let cave_scale = 0.03;
        let cave_threshold = 0.6;
        let cave_noise_val = self.cave_noise.get([
            x * cave_scale, 
            y * cave_scale, 
            z * cave_scale
        ]);
        
        // Cave systems only exist underground
        let cave_carver = if depth > 10.0 && depth < 80.0 {
            if cave_noise_val > cave_threshold {
                -1.0 // Carve cave
            } else {
                0.0 // No effect
            }
        } else {
            0.0 // No caves near surface or very deep
        };
        
        // Small 3D details for terrain texture
        let detail_scale = 0.04;
        let small_detail = self.detail_noise.get([
            x * detail_scale,
            y * detail_scale,
            z * detail_scale
        ]) * 0.1;
        
        // Combine all density factors
        surface_density + cave_carver + small_detail
    }
    
    /// Get a biome index for a given world position (x, z) using climate model
    pub fn get_biome(&self, x: f64, z: f64) -> u8 {
        // More varied climate model
        let temp_scale = 0.0007;
        let rain_scale = 0.0005;
        
        // Temperature decreases with distance from equator (use abs for symmetry)
        let equator_influence = 1.0 - (z * 0.0001).abs().min(1.0);
        
        // Base temperature with noise variation
        let temp = equator_influence * 0.7 + 
                   self.base_noise.get([x * temp_scale, 2000.0 + z * temp_scale]) * 0.3;
        
        // Rainfall with warping for more realistic weather patterns
        let rain_x = x + self.warp_x_noise.get([x * rain_scale * 0.5, z * rain_scale * 0.5]) * 200.0;
        let rain_z = z + self.warp_z_noise.get([x * rain_scale * 0.5 + 500.0, z * rain_scale * 0.5 + 500.0]) * 200.0;
        let rain = self.base_noise.get([rain_x * rain_scale + 5000.0, rain_z * rain_scale + 5000.0]);
        
        // Biome classification with more variants
        match (temp, rain) {
            // Hot biomes
            (t, r) if t > 0.7 && r > 0.7 => 0,  // Jungle
            (t, r) if t > 0.7 && r > 0.3 => 1,  // Savanna
            (t, r) if t > 0.7 => 3,             // Desert
            
            // Temperate biomes
            (t, r) if t > 0.4 && r > 0.6 => 0,  // Forest
            (t, r) if t > 0.4 && r > 0.3 => 1,  // Plains
            (t, r) if t > 0.4 => 3,             // Badlands
            
            // Cold biomes
            (t, r) if t > 0.2 && r > 0.5 => 2,  // Taiga
            (t, r) if t > 0.2 => 1,             // Tundra
            
            // Very cold
            _ => 2,                             // Snow fields
        }
    }
}
