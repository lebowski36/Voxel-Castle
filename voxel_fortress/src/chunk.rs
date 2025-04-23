use bevy::prelude::*;
use bevy::render::mesh::{Mesh, PrimitiveTopology};
use bevy::render::render_asset::RenderAssetUsages;
use crate::terrain::{CHUNK_SIZE, VOXEL_SIZE_METERS, Chunk, WorldGen};
use std::collections::HashMap;

// Reduced radii to improve performance
pub const ACTIVE_RADIUS: f32 = 2.5; // Full mesh within this radius
pub const LOD_RADIUS: f32 = 5.0;    // LOD mesh within this radius, else Unloaded
pub const UNLOAD_RADIUS: f32 = 8.0; // Beyond this, chunks are fully unloaded from memory

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ChunkState {
    Active,   // High detail, full mesh
    LOD,      // Low detail, heightmap mesh
    Unloaded, // Not loaded/generated
}

pub struct ManagedChunk {
    pub pos: (i64, i64, i64),
    pub state: ChunkState,
    pub chunk: Option<Chunk>, // Only present if Active
    pub lod_mesh: Option<Handle<Mesh>>, // Only present if LOD
    pub entity: Option<Entity>, // Entity for mesh (Active or LOD)
}

#[derive(Resource)]
pub struct ChunkManager {
    pub loaded_chunks: HashMap<(i64, i64, i64), ManagedChunk>,
    pub radius: i32, // Loaded chunk radius
}

impl ChunkManager {
    pub fn new(radius: i32) -> Self {
        Self {
            loaded_chunks: HashMap::new(),
            radius,
        }
    }
    pub fn load_chunks_around(&mut self, center: (i64, i64, i64), worldgen: &WorldGen) {
        let r = self.radius as i64;
        for dx in -r..=r {
            for dy in -r..=r {
                for dz in -r..=r {
                    let pos = (center.0 + dx, center.1 + dy, center.2 + dz);
                    if !self.loaded_chunks.contains_key(&pos) {
                        let chunk = Chunk::from_worldgen(worldgen, pos);
                        self.loaded_chunks.insert(pos, ManagedChunk {
                            pos,
                            state: ChunkState::Active,
                            chunk: Some(chunk),
                            lod_mesh: None,
                            entity: None,
                        });
                    }
                }
            }
        }
    }
}

pub fn update_chunk_lod_system(
    mut chunk_manager: ResMut<ChunkManager>,
    camera_query: Query<&Transform, With<Camera3d>>,
    mut meshes: ResMut<Assets<Mesh>>,
) {
    let camera_pos = if let Some(transform) = camera_query.iter().next() {
        transform.translation
    } else {
        return;
    };
    
    // Get camera chunk position for loading new chunks
    let camera_chunk_x = (camera_pos.x / (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS)).floor() as i64;
    let camera_chunk_y = (camera_pos.y / (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS)).floor() as i64;
    let camera_chunk_z = (camera_pos.z / (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS)).floor() as i64;
    
    for (pos, managed_chunk) in chunk_manager.loaded_chunks.iter_mut() {
        let chunk_center = Vec3::new(
            pos.0 as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS + (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS) / 2.0,
            pos.1 as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS + (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS) / 2.0,
            pos.2 as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS + (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS) / 2.0,
        );
        
        // Calculate normalized distance (in chunks)
        let dist = camera_pos.distance(chunk_center) / (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS);
        let prev_state = managed_chunk.state;
        
        // Update chunk state based on distance
        if dist <= ACTIVE_RADIUS {
            // Should be in ACTIVE state
            if managed_chunk.state != ChunkState::Active {
                // If transitioning from LOD to Active, we need to regenerate the full chunk
                if managed_chunk.state == ChunkState::LOD {
                    // Generate the full chunk data when transitioning from LOD to Active
                    let chunk_data = Chunk::from_worldgen(
                        &WorldGen::new(3, CHUNK_SIZE, 42), // Use the same seed
                        *pos
                    );
                    managed_chunk.chunk = Some(chunk_data);
                    managed_chunk.lod_mesh = None;
                }
                managed_chunk.state = ChunkState::Active;
            }
        } else if dist <= LOD_RADIUS {
            // Should be in LOD state
            if managed_chunk.state != ChunkState::LOD {
                if managed_chunk.state == ChunkState::Active {
                    // Transitioning from Active to LOD
                    if let Some(chunk) = &managed_chunk.chunk {
                        let lod_mesh = chunk.to_lod_mesh();
                        managed_chunk.lod_mesh = Some(meshes.add(lod_mesh));
                        // Free up memory by removing the full chunk data
                        managed_chunk.chunk = None;
                    }
                } else {
                    // Transitioning from Unloaded to LOD
                    // Generate a temporary chunk to create the LOD mesh
                    let temp_chunk = Chunk::from_worldgen(
                        &WorldGen::new(3, CHUNK_SIZE, 42),
                        *pos
                    );
                    let lod_mesh = temp_chunk.to_lod_mesh();
                    managed_chunk.lod_mesh = Some(meshes.add(lod_mesh));
                }
                managed_chunk.state = ChunkState::LOD;
            }
        } else if dist <= UNLOAD_RADIUS {
            // Keep in memory but no visuals
            if managed_chunk.state != ChunkState::Unloaded {
                managed_chunk.chunk = None;
                managed_chunk.lod_mesh = None;
                managed_chunk.state = ChunkState::Unloaded;
            }
        }
        // Beyond UNLOAD_RADIUS chunks will be removed from memory entirely
        // (This is handled elsewhere)
    }
}
