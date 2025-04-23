use bevy::prelude::*;
use crate::chunk::*;
use crate::terrain::{CHUNK_SIZE, VOXEL_SIZE_METERS, Chunk, WorldGen};

// Helper function for chunk loading and unloading
pub fn manage_chunk_loading(
    mut chunk_manager: ResMut<ChunkManager>,
    camera_query: Query<&Transform, With<Camera3d>>,
) {
    let camera_transform = match camera_query.iter().next() {
        Some(transform) => transform,
        _ => return,
    };
    
    // Get camera position in chunk coordinates
    let camera_pos = camera_transform.translation;
    let camera_chunk_x = (camera_pos.x / (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS)).floor() as i64;
    let camera_chunk_y = (camera_pos.y / (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS)).floor() as i64;
    let camera_chunk_z = (camera_pos.z / (CHUNK_SIZE as f32 * VOXEL_SIZE_METERS)).floor() as i64;
    let camera_chunk_pos = (camera_chunk_x, camera_chunk_y, camera_chunk_z);
    
    // Get camera look direction for prioritized loading
    let look_dir = camera_transform.forward();
    let look_chunk_delta_x = if look_dir.x > 0.3 { 1 } else if look_dir.x < -0.3 { -1 } else { 0 };
    let look_chunk_delta_z = if look_dir.z > 0.3 { 1 } else if look_dir.z < -0.3 { -1 } else { 0 };
    
    // Create a temporary WorldGen to generate new chunks
    let worldgen = WorldGen::new(3, CHUNK_SIZE, 42);
    
    // Calculate chunk bounds to load/unload
    let r = chunk_manager.radius as i64;
    let extended_r = r + 3; // Add a buffer for smooth transitions
    
    // Find chunks to unload (too far away) and chunks to load (newly in range)
    let mut chunks_to_load = Vec::new();
    
    // Find chunks that are out of range and should be unloaded
    let mut chunks_to_remove = Vec::new();
    for (pos, _) in chunk_manager.loaded_chunks.iter() {
        let dx = pos.0 - camera_chunk_x;
        let dy = pos.1 - camera_chunk_y;
        let dz = pos.2 - camera_chunk_z;
        
        // Use distance in chunks for unloading decision
        let dist = ((dx*dx + dy*dy + dz*dz) as f32).sqrt();
        if dist > UNLOAD_RADIUS {
            chunks_to_remove.push(*pos);
        }
    }
    
    // Remove chunks that are too far away
    for pos in chunks_to_remove {
        chunk_manager.loaded_chunks.remove(&pos);
    }
    
    // Prioritize loading chunks in view direction
    let mut priority_chunks = Vec::new();
    
    // First, add chunks in the camera's look direction with higher priority
    for d in 1..=r {
        for dy in -r/2..=r/2 {
            for offset in 0..=2 {
                // Left/right of look direction
                let left_x = camera_chunk_x + look_chunk_delta_x * d - look_chunk_delta_z * offset;
                let left_z = camera_chunk_z + look_chunk_delta_z * d + look_chunk_delta_x * offset;
                let right_x = camera_chunk_x + look_chunk_delta_x * d + look_chunk_delta_z * offset;
                let right_z = camera_chunk_z + look_chunk_delta_z * d - look_chunk_delta_x * offset;
                
                let left_pos = (left_x, camera_chunk_y + dy, left_z);
                let right_pos = (right_x, camera_chunk_y + dy, right_z);
                
                if !chunk_manager.loaded_chunks.contains_key(&left_pos) {
                    priority_chunks.push(left_pos);
                }
                
                if !chunk_manager.loaded_chunks.contains_key(&right_pos) {
                    priority_chunks.push(right_pos);
                }
            }
        }
    }
    
    // Then load all other chunks in range
    for dx in -r..=r {
        for dy in -r/2..=r/2 { // Less vertical spread
            for dz in -r..=r {
                let pos = (camera_chunk_x + dx, camera_chunk_y + dy, camera_chunk_z + dz);
                
                // Skip if already in priority list or already loaded
                if priority_chunks.contains(&pos) || chunk_manager.loaded_chunks.contains_key(&pos) {
                    continue;
                }
                
                // Calculate distance for LOD decision
                let dist = ((dx*dx + dy*dy + dz*dz) as f32).sqrt();
                
                // Only add if within the radius
                if dist <= r as f32 {
                    chunks_to_load.push(pos);
                }
            }
        }
    }
    
    // Load priority chunks first (in look direction)
    let mut chunks_loaded = 0;
    let max_chunks_per_frame = 5; // Limit to prevent stutter
    
    for pos in priority_chunks {
        if chunks_loaded >= max_chunks_per_frame {
            break;
        }
        
        // Generate new chunk
        let chunk_data = Chunk::from_worldgen(&worldgen, pos);
        
        // Calculate distance for LOD decision
        let dx = pos.0 - camera_chunk_x;
        let dy = pos.1 - camera_chunk_y; 
        let dz = pos.2 - camera_chunk_z;
        let dist = ((dx*dx + dy*dy + dz*dz) as f32).sqrt();
        
        // Decide initial LOD state based on distance
        if dist <= ACTIVE_RADIUS {
            chunk_manager.loaded_chunks.insert(pos, ManagedChunk {
                pos,
                state: ChunkState::Active,
                chunk: Some(chunk_data),
                lod_mesh: None,
                entity: None,
            });
        } else if dist <= LOD_RADIUS {
            // For LOD chunks, we need to generate a low-detail mesh
            let lod_mesh = chunk_data.to_lod_mesh();
            chunk_manager.loaded_chunks.insert(pos, ManagedChunk {
                pos,
                state: ChunkState::LOD,
                chunk: None, // Don't store full chunk data for LOD
                lod_mesh: None, // Will be set in update_chunk_entities_system
                entity: None,
            });
        } else {
            chunk_manager.loaded_chunks.insert(pos, ManagedChunk {
                pos,
                state: ChunkState::Unloaded,
                chunk: None,
                lod_mesh: None, 
                entity: None,
            });
        }
        
        chunks_loaded += 1;
    }
    
    // Load remaining chunks with lower priority
    for pos in chunks_to_load {
        if chunks_loaded >= max_chunks_per_frame {
            break;
        }
        
        // Generate new chunk
        let chunk_data = Chunk::from_worldgen(&worldgen, pos);
        
        // Calculate distance for LOD decision
        let dx = pos.0 - camera_chunk_x;
        let dy = pos.1 - camera_chunk_y;
        let dz = pos.2 - camera_chunk_z;
        let dist = ((dx*dx + dy*dy + dz*dz) as f32).sqrt();
        
        // Decide initial LOD state based on distance
        if dist <= ACTIVE_RADIUS {
            chunk_manager.loaded_chunks.insert(pos, ManagedChunk {
                pos,
                state: ChunkState::Active,
                chunk: Some(chunk_data),
                lod_mesh: None,
                entity: None,
            });
        } else if dist <= LOD_RADIUS {
            // For LOD chunks, we need to generate a low-detail mesh
            let lod_mesh = chunk_data.to_lod_mesh();
            chunk_manager.loaded_chunks.insert(pos, ManagedChunk {
                pos,
                state: ChunkState::LOD,
                chunk: None, // Don't store full chunk data for LOD
                lod_mesh: None, // Will be set in update_chunk_entities_system
                entity: None,
            });
        } else {
            chunk_manager.loaded_chunks.insert(pos, ManagedChunk {
                pos,
                state: ChunkState::Unloaded,
                chunk: None,
                lod_mesh: None,
                entity: None,
            });
        }
        
        chunks_loaded += 1;
    }
}
