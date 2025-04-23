use bevy::prelude::*;
use crate::chunk::*;
use crate::terrain::{CHUNK_SIZE, VOXEL_SIZE_METERS, Voxel, WorldGen};
use crate::chunk_loading;

pub struct WorldPlugin;

/// Resource to store the chunk material
#[derive(Resource, Clone)]
pub struct ChunkMaterialHandle(pub Handle<StandardMaterial>);

impl Plugin for WorldPlugin {
    fn build(&self, app: &mut App) {
        app.insert_resource(ChunkManager::new(5))
           .add_systems(Startup, setup_chunk_material)
           // Initialize world during Loading state
           .add_systems(OnEnter(crate::GameState::Loading), setup_world)
           // Update chunks only in Playing state
           .add_systems(Update, 
                update_chunk_lod_system
                    .run_if(in_state(crate::GameState::Playing))
            )
           .add_systems(Update, 
                update_chunk_entities_system
                    .run_if(in_state(crate::GameState::Playing))
            )
           // Add the optimized chunk management system
           .add_systems(Update, 
                crate::chunk_loading::manage_chunk_loading
                    .run_if(in_state(crate::GameState::Playing))
            );
    }
}

/// Set up the chunk material that will be used for rendering the world
fn setup_chunk_material(
    mut commands: Commands,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    let chunk_material_handle = materials.add(StandardMaterial {
        base_color: Color::srgb(0.7, 0.7, 0.7),
        perceptual_roughness: 0.9,
        ..default()
    });
    commands.insert_resource(ChunkMaterialHandle(chunk_material_handle));
}

/// Set up the initial world generation
fn setup_world(
    mut commands: Commands,
    mut chunk_manager: ResMut<ChunkManager>,
    mut camera_query: Query<&mut Transform, With<Camera3d>>,
) {
    // --- WorldGen instance ---
    let worldgen = WorldGen::new(3, CHUNK_SIZE, 42);
    chunk_manager.loaded_chunks.clear();
    chunk_manager.load_chunks_around((0, 0, 0), &worldgen);

    // Find the highest ground in the center chunk for player spawn
    let center_chunk = chunk_manager.loaded_chunks.get(&(0, 0, 0)).unwrap();
    let mut max_ground_y = 0.0;
    if let Some(chunk) = &center_chunk.chunk {
        for x in 0..CHUNK_SIZE {
            for z in 0..CHUNK_SIZE {
                for y in (0..CHUNK_SIZE).rev() {
                    if chunk.voxels[x][y][z] == Voxel::Solid {
                        let world_y = y as f32 * VOXEL_SIZE_METERS;
                        if world_y > max_ground_y {
                            max_ground_y = world_y;
                        }
                        break;
                    }
                }
            }
        }
    }

    // Update camera position if available
    if let Ok(mut transform) = camera_query.get_single_mut() {
        transform.translation.y = max_ground_y + 3.0;
        // Look slightly forward
        transform.look_at(Vec3::new(5.0, max_ground_y, 5.0), Vec3::Y);
    }
}

/// Handle updating chunk entities based on their state
pub fn update_chunk_entities_system(
    mut commands: Commands,
    mut chunk_manager: ResMut<ChunkManager>,
    mut meshes: ResMut<Assets<Mesh>>,
    chunk_material: Res<ChunkMaterialHandle>,
    camera_query: Query<(&Transform, &Camera3d)>,
) {
    // Get camera for frustum culling
    let (camera_transform, camera) = match camera_query.iter().next() {
        Some((transform, camera)) => (transform, camera),
        _ => return,
    };
    
    let mut chunks_to_remove: Vec<(i64, i64, i64)> = Vec::new();
    
    // Track how many chunks are in each state for debugging
    let mut active_count = 0;
    let mut lod_count = 0;
    let mut unloaded_count = 0;
    
    // Update or create entities for chunks
    for (pos, managed_chunk) in chunk_manager.loaded_chunks.iter_mut() {
        // Track statistics
        match managed_chunk.state {
            ChunkState::Active => active_count += 1,
            ChunkState::LOD => lod_count += 1,
            ChunkState::Unloaded => unloaded_count += 1,
        }
        
        // Calculate world position and chunk bounds
        let chunk_size_meters = CHUNK_SIZE as f32 * crate::terrain::VOXEL_SIZE_METERS;
        let chunk_position = Vec3::new(
            pos.0 as f32 * chunk_size_meters,
            pos.1 as f32 * chunk_size_meters,
            pos.2 as f32 * chunk_size_meters,
        );
        
        // Check if the chunk should have a visual entity
        let should_have_entity = match managed_chunk.state {
            ChunkState::Active => managed_chunk.chunk.is_some(),
            ChunkState::LOD => managed_chunk.lod_mesh.is_some(),
            ChunkState::Unloaded => false,
        };
        
        // First, handle entity removal if needed
        if !should_have_entity && managed_chunk.entity.is_some() {
            if let Some(entity) = managed_chunk.entity.take() {
                commands.entity(entity).despawn_recursive();
            }
            continue;
        }
        
        // If it needs an entity, check visibility with basic frustum culling
        if should_have_entity {
            // Calculate distance to camera (more accurate than chunk-based)
            let chunk_center = chunk_position + Vec3::splat(chunk_size_meters / 2.0);
            let dist_to_camera = camera_transform.translation.distance(chunk_center);
            
            // Simple sphere-based frustum culling - if chunk is outside view frustum, don't create entity
            let chunk_radius = chunk_size_meters * 0.87; // Approximation for chunk diagonal
            let is_visible = is_sphere_in_frustum(camera_transform, chunk_center, chunk_radius);
            
            if is_visible {
                // Create or update entity as needed
                if managed_chunk.entity.is_none() {
                    // Need to create new entity
                    let mesh_handle = match managed_chunk.state {
                        ChunkState::Active => managed_chunk.chunk.as_ref().map(|c| meshes.add(c.to_mesh())),
                        ChunkState::LOD => managed_chunk.lod_mesh.clone(),
                        _ => None,
                    };
                    
                    if let Some(mesh) = mesh_handle {
                        let entity = commands.spawn((
                            Mesh3d(mesh),
                            MeshMaterial3d(chunk_material.0.clone()),
                            Transform::from_translation(chunk_position),
                            // Add component to mark visualization type
                            ChunkVisualization { state: managed_chunk.state },
                        )).id();
                        managed_chunk.entity = Some(entity);
                    }
                }
            } else {
                // Not visible - remove entity if it exists
                if let Some(entity) = managed_chunk.entity.take() {
                    commands.entity(entity).despawn_recursive();
                }
            }
        }
    }
    
    // Print debug info occasionally (every few seconds)
    // Use simpler approach to avoid Time initialization issues
    if active_count > 0 {
        println!("Chunks: Active={}, LOD={}, Unloaded={}, Total={}", 
            active_count, lod_count, unloaded_count, active_count + lod_count + unloaded_count);
    }
}

// Component to track chunk visualization type
#[derive(Component)]
struct ChunkVisualization {
    state: ChunkState,
}

// Simple frustum culling helper
fn is_sphere_in_frustum(camera_transform: &Transform, sphere_center: Vec3, sphere_radius: f32) -> bool {
    // Get camera forward direction
    let forward = camera_transform.forward();
    let camera_pos = camera_transform.translation;
    
    // Simple check - is the sphere behind the camera?
    let to_sphere = sphere_center - camera_pos;
    let distance_along_view = to_sphere.dot(forward.as_vec3());
    
    // If sphere is behind camera plane, it's not visible
    if distance_along_view < -sphere_radius {
        return false;
    }
    
    // Simplified frustum check - treat camera as having 90-degree FOV
    // and check if sphere center is too far from view axis
    let max_angle_cos = 0.5; // ~60 degree half-angle
    let view_dist = distance_along_view.max(0.1);
    let perpendicular_dist = (to_sphere - forward * distance_along_view).length();
    let angle_cos = view_dist / (view_dist.powi(2) + perpendicular_dist.powi(2)).sqrt();
    
    // If sphere is too far to the side, it's not visible
    if angle_cos < max_angle_cos && perpendicular_dist > sphere_radius {
        return false;
    }
    
    // Otherwise, it's potentially visible
    true
}
