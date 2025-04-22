use bevy::prelude::*;
use bevy::render::mesh::{Mesh, Indices, PrimitiveTopology};
use noise::{NoiseFn, Perlin, RidgedMulti, Seedable};

// --- Voxel Chunk Data Structure ---

pub const CHUNK_SIZE: usize = 32;
pub const VOXEL_SIZE_METERS: f32 = 0.25; // 25cm

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum Voxel {
    Air,
    Solid,
}

pub struct Chunk {
    pub voxels: [[[Voxel; CHUNK_SIZE]; CHUNK_SIZE]; CHUNK_SIZE],
}

impl Chunk {
    pub fn new_filled(voxel: Voxel) -> Self {
        Self {
            voxels: [[[voxel; CHUNK_SIZE]; CHUNK_SIZE]; CHUNK_SIZE],
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
        for x in 0..CHUNK_SIZE {
            for z in 0..CHUNK_SIZE {
                let wx = base_x + x as i64;
                let wz = base_z + z as i64;
                let height = worldgen.get_height(wx as f64, wz as f64);
                let biome = worldgen.get_biome(wx as f64, wz as f64);
                // World vertical range: y=0 is sea level, height is in meters
                let ground_y = (height as i64).max(0);
                for y in 0..CHUNK_SIZE {
                    let wy = base_y + y as i64;
                    if wy <= ground_y {
                        chunk.voxels[x][y][z] = match biome {
                            0 => Voxel::Solid, // Forest
                            1 => Voxel::Solid, // Plains
                            2 => Voxel::Solid, // Taiga
                            3 => Voxel::Solid, // Desert
                            _ => Voxel::Solid,
                        };
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
        let mut i = 0u32;
        for x in 0..CHUNK_SIZE {
            for y in 0..CHUNK_SIZE {
                for z in 0..CHUNK_SIZE {
                    if self.voxels[x][y][z] == Voxel::Solid {
                        // Color gradient: brown at bottom, green at top
                        let t = y as f32 / (CHUNK_SIZE as f32 - 1.0);
                        let brown = [0.45, 0.32, 0.18];
                        let green = [0.2, 0.8, 0.2];
                        // For top faces, always green
                        // For bottom faces, always brown
                        // For sides, gradient
                        for (dx, dy, dz, normal, face_type) in [
                            (0, 0, -1, [0.0, 0.0, -1.0], "side"), // -Z
                            (0, 0, 1, [0.0, 0.0, 1.0], "side"),  // +Z
                            (0, -1, 0, [0.0, -1.0, 0.0], "bottom"), // -Y
                            (0, 1, 0, [0.0, 1.0, 0.0], "top"),  // +Y
                            (-1, 0, 0, [-1.0, 0.0, 0.0], "side"), // -X
                            (1, 0, 0, [1.0, 0.0, 0.0], "side"),  // +X
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
                                // Color logic for each face
                                let face_color = match face_type {
                                    "top" => [green[0], green[1], green[2], 1.0],
                                    "bottom" => [brown[0], brown[1], brown[2], 1.0],
                                    _ => [brown[0] * (1.0 - t) + green[0] * t,
                                          brown[1] * (1.0 - t) + green[1] * t,
                                          brown[2] * (1.0 - t) + green[2] * t,
                                          1.0],
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
            return Mesh::new(PrimitiveTopology::TriangleList, bevy::render::render_asset::RenderAssetUsages::default());
        }
        let mut mesh = Mesh::new(PrimitiveTopology::TriangleList, bevy::render::render_asset::RenderAssetUsages::default());
        mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, positions.clone());
        mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals.clone());
        // Add dummy UVs (required by StandardMaterial)
        let uvs = vec![[0.0, 0.0]; positions.len()];
        mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, uvs);
        // Add per-vertex color
        mesh.insert_attribute(Mesh::ATTRIBUTE_COLOR, colors);
        mesh.insert_indices(Indices::U32(indices));
        mesh
    }
}

fn face_mesh(base: Vec3, normal: [f32; 3], i: u32) -> (Vec<[f32; 3]>, Vec<[f32; 3]>, Vec<u32>) {
    // Returns the 4 vertices, normals, and 6 indices for a quad face at base with the given normal
    let (dx, dy, dz) = (normal[0], normal[1], normal[2]);
    let size = VOXEL_SIZE_METERS;
    let (v0, v1, v2, v3) = match (dx, dy, dz) {
        (0.0, 0.0, -1.0) => (
            [base.x, base.y, base.z],
            [base.x + size, base.y, base.z],
            [base.x + size, base.y + size, base.z],
            [base.x, base.y + size, base.z],
        ),
        (0.0, 0.0, 1.0) => (
            [base.x, base.y, base.z + size],
            [base.x + size, base.y, base.z + size],
            [base.x + size, base.y + size, base.z + size],
            [base.x, base.y + size, base.z + size],
        ),
        (0.0, -1.0, 0.0) => (
            [base.x, base.y, base.z],
            [base.x + size, base.y, base.z],
            [base.x + size, base.y, base.z + size],
            [base.x, base.y, base.z + size],
        ),
        (0.0, 1.0, 0.0) => (
            [base.x, base.y + size, base.z],
            [base.x, base.y + size, base.z + size],
            [base.x + size, base.y + size, base.z + size],
            [base.x + size, base.y + size, base.z],
        ),
        (-1.0, 0.0, 0.0) => (
            [base.x, base.y, base.z],
            [base.x, base.y, base.z + size],
            [base.x, base.y + size, base.z + size],
            [base.x, base.y + size, base.z],
        ),
        (1.0, 0.0, 0.0) => (
            [base.x + size, base.y, base.z],
            [base.x + size, base.y, base.z + size],
            [base.x + size, base.y + size, base.z + size],
            [base.x + size, base.y + size, base.z],
        ),
        _ => unreachable!(),
    };
    let verts = vec![v0, v1, v2, v3];
    let norms = vec![normal; 4];
    let inds = vec![i, i + 1, i + 2, i, i + 2, i + 3];
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
pub struct WorldGen {
    pub levels: u8, // Number of chunk levels
    pub chunk_size: usize, // Size per chunk (e.g., 32)
    pub seed: u64,
    pub base_noise: Perlin,
    pub ridged_noise: RidgedMulti<Perlin>,
    // Add more worldgen parameters as needed
}

impl WorldGen {
    pub fn new(levels: u8, chunk_size: usize, seed: u64) -> Self {
        Self {
            levels,
            chunk_size,
            seed,
            base_noise: Perlin::new(seed as u32),
            ridged_noise: RidgedMulti::new(seed as u32),
        }
    }
    /// Get the base height for a given world position (x, z) using multi-octave noise and domain warping
    pub fn get_height(&self, x: f64, z: f64) -> f64 {
        let scale = 0.002;
        let warp = self.base_noise.get([x * scale * 0.5, z * scale * 0.5]) * 100.0;
        let nx = x * scale + warp;
        let nz = z * scale + warp;
        let base = self.base_noise.get([nx, nz]);
        let ridged = self.ridged_noise.get([nx * 0.5, nz * 0.5]);
        // Combine for mountains/valleys
        let height = base * 40.0 + ridged * 60.0;
        height
    }
    /// Get a biome index for a given world position (x, z) using climate simulation (stub for now)
    pub fn get_biome(&self, x: f64, z: f64) -> u8 {
        let temp = self.base_noise.get([x * 0.0005, 1000.0 + z * 0.0005]);
        let rain = self.base_noise.get([x * 0.0005 + 5000.0, z * 0.0005 + 5000.0]);
        match (temp, rain) {
            (t, r) if t > 0.3 && r > 0.3 => 0, // Forest
            (t, r) if t > 0.3 && r <= 0.3 => 1, // Plains
            (t, r) if t <= 0.3 && r > 0.3 => 2, // Taiga
            _ => 3, // Desert
        }
    }
    // TODO: Add erosion, river, cave, and feature generation
}
