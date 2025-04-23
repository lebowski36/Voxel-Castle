use bevy::prelude::*;
// Import Task and TaskPool
use bevy::tasks::{Task, TaskPool};
// Import HashMap for the task result
use std::collections::HashMap;
use futures_lite::future;
// Import necessary types from other modules
use crate::chunk::{ChunkManager, ChunkState, ManagedChunk};
use crate::terrain::{Voxel, WorldGen, VOXEL_SIZE_METERS, Chunk, CHUNK_SIZE};
use crate::plugins::camera::PlayerCamera;
use crate::plugins::ui::GameState;

pub struct WorldPlugin;

/// Resource to store the chunk material
#[derive(Resource, Clone)]
pub struct ChunkMaterialHandle(pub Handle<StandardMaterial>);

// Resource to hold the task that loads initial chunks
#[derive(Default, Resource)]
struct InitialChunkLoadTask(Option<Task<HashMap<(i64, i64, i64), ManagedChunk>>>);

impl Plugin for WorldPlugin {
    fn build(&self, app: &mut App) {
        app
            .init_resource::<InitialChunkLoadTask>()
            .insert_resource(WorldGen::new(3, CHUNK_SIZE, 42))
            // Fix system configuration by using proper Bevy system scheduling
            .add_systems(Startup, setup_chunk_material)
            .add_systems(Startup, setup_world)
            .add_systems(
                Update, 
                check_initial_chunk_load_task.run_if(in_state(GameState::Loading))
            )
            .add_systems(
                Update,
                (
                    update_chunk_entities_system,
                    crate::chunk::update_chunk_lod_system
                ).run_if(in_state(GameState::Playing))
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

/// Kicks off the initial world generation in a background task
fn setup_world(
    mut commands: Commands,
    worldgen: Res<WorldGen>,
    mut loading_task: ResMut<InitialChunkLoadTask>,
) {
    println!("Starting initial chunk generation task...");
    let radius = 5;
    // Clone the WorldGen resource so it can be moved into the async task
    let worldgen_clone = worldgen.clone();
    
    // Get the task pool from Bevy's global resources
    let thread_pool = TaskPool::new();
    
    let task = thread_pool.spawn(async move {
        println!("Background task: Generating initial chunks...");
        let mut initial_chunks = HashMap::new();
        let r = radius as i64;
        let center = (0, 0, 0);
        for dx in -r..=r {
            for dy in -r..=r {
                for dz in -r..=r {
                    let pos = (center.0 + dx, center.1 + dy, center.2 + dz);
                    let chunk_data = Chunk::from_worldgen(&worldgen_clone, pos);
                    let managed_chunk = ManagedChunk {
                        pos,
                        state: ChunkState::Active,
                        chunk: Some(chunk_data),
                        lod_mesh: None,
                        entity: None,
                    };
                    initial_chunks.insert(pos, managed_chunk);
                }
            }
        }
        initial_chunks
    });
    loading_task.0 = Some(task);
}

/// Checks if the initial chunk loading task is complete and populates the ChunkManager
fn check_initial_chunk_load_task(
    mut commands: Commands,
    mut task: ResMut<InitialChunkLoadTask>,
    mut camera_query: Query<&mut Transform, With<PlayerCamera>>, // Query for player camera
) {
    if let Some(mut inner_task) = task.0.take() {
        // Check if the task is finished using poll_once
        // We use future::poll_once here since we're in an update system
        if let Some(initial_chunks) = future::block_on(future::poll_once(&mut inner_task)) {
            println!("Initial chunk generation task completed. Populating ChunkManager.");

            // Task is complete, create and insert the ChunkManager resource
            let mut chunk_manager = ChunkManager::new(5); // Use the same radius as in setup_world
            chunk_manager.loaded_chunks = initial_chunks;

            // --- Adjust Camera Position ---
            // Find the highest ground in the center chunk for player spawn
            let mut max_ground_y = 0.0;
            if let Some(center_chunk) = chunk_manager.loaded_chunks.get(&(0, 0, 0)) {
                 if let Some(chunk) = &center_chunk.chunk {
                    for x in 0..CHUNK_SIZE {
                        for z in 0..CHUNK_SIZE {
                            for y in (0..CHUNK_SIZE).rev() {
                                // Ensure Voxel::Solid is the correct variant name
                                if chunk.voxels[x][y][z] == Voxel::Solid {
                                    let world_y = y as f32 * VOXEL_SIZE_METERS;
                                    if world_y > max_ground_y {
                                        max_ground_y = world_y;
                                    }
                                    break; // Found highest point for this x,z
                                }
                            }
                        }
                    }
                 }
            } else {
                 println!("Warning: Center chunk (0,0,0) not found after initial load.");
                 max_ground_y = 10.0; // Default height if center chunk missing
            }

            // Update camera position
            if let Ok(mut transform) = camera_query.get_single_mut() {
                transform.translation.y = max_ground_y + 3.0; // Position slightly above ground
                // Optionally adjust where the camera looks initially
                transform.look_at(Vec3::new(5.0, max_ground_y, 5.0), Vec3::Y);
                println!("Adjusted camera height to: {}", transform.translation.y);
            } else {
                println!("Warning: PlayerCamera not found, cannot adjust initial height.");
            }
            // --- End Adjust Camera Position ---


            commands.insert_resource(chunk_manager);
            println!("ChunkManager resource inserted.");

            // Task is done, no need to keep checking
            // The state transition to Playing is handled by the ui::update_loading_progress system
        } else {
            // Task not finished, put it back
            task.0 = Some(inner_task);
        }
    }
}

/// Handle updating chunk entities based on their state
pub fn update_chunk_entities_system(
    mut commands: Commands,
    // Use Option<ResMut<ChunkManager>> to handle the case where it might not exist yet
    chunk_manager_opt: Option<ResMut<ChunkManager>>,
    mut meshes: ResMut<Assets<Mesh>>,
    chunk_material: Res<ChunkMaterialHandle>,
    // Use Option<Query> for camera as well, though less critical here
    camera_query: Query<(&Transform, &Camera3d), With<PlayerCamera>>,
) {
    // Only run if ChunkManager exists
    let Some(mut chunk_manager) = chunk_manager_opt else {
        // println!("update_chunk_entities_system: ChunkManager not yet available.");
        return;
    };

    // Get camera for frustum culling
    // Use get_single for clarity if there should only be one PlayerCamera
    let Ok((camera_transform, _camera)) = camera_query.get_single() else {
        // println!("update_chunk_entities_system: PlayerCamera not found.");
        return;
    };

    // Removed tracking variables as they are unused
    // let mut active_count = 0;
    // let mut lod_count = 0;
    // let mut unloaded_count = 0;

    // Update or create entities for chunks
    for (pos, managed_chunk) in chunk_manager.loaded_chunks.iter_mut() {
        // Removed state tracking
        // match managed_chunk.state {
        //     ChunkState::Active => active_count += 1,
        //     ChunkState::LOD => lod_count += 1,
        //     ChunkState::Unloaded => unloaded_count += 1,
        // }

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
            continue; // Skip to next chunk if entity removed
        }

        // If it needs an entity, check visibility with basic frustum culling
        if should_have_entity {
            // Calculate distance to camera (more accurate than chunk-based)
            let chunk_center = chunk_position + Vec3::splat(chunk_size_meters / 2.0);
            // Removed unused distance calculation
            // let dist_to_camera = camera_transform.translation.distance(chunk_center);

            // Simple sphere-based frustum culling - if chunk is outside view frustum, don't create entity
            let chunk_radius = chunk_size_meters * 0.87; // Approximation for chunk diagonal
            let is_visible = is_sphere_in_frustum(camera_transform, chunk_center, chunk_radius);

            if is_visible {
                // Create or update entity as needed
                if managed_chunk.entity.is_none() {
                    // Need to create new entity
                    let mesh_handle = match managed_chunk.state {
                        // Call to_mesh() and add the resulting mesh to Assets<Mesh>
                        ChunkState::Active => {
                            if let Some(chunk) = &managed_chunk.chunk {
                                // Explicit error handling to catch any issues
                                let mesh = match std::panic::catch_unwind(|| chunk.to_mesh()) {
                                    Ok(mesh) => mesh,
                                    Err(_) => {
                                        println!("Error generating mesh for chunk at {:?}", pos);
                                        continue; // Skip this chunk and continue with the next one
                                    }
                                };
                                Some(meshes.add(mesh))
                            } else {
                                None
                            }
                        },
                        ChunkState::LOD => managed_chunk.lod_mesh.clone(),
                        _ => None,
                    };

                    if let Some(mesh_handle) = mesh_handle {
                        // Use the new component-based approach in Bevy 0.15.3
                        let entity = commands.spawn((
                            // Use Mesh3d and MeshMaterial3d components
                            Mesh3d::from(mesh_handle),
                            MeshMaterial3d::from(chunk_material.0.clone()),
                            TransformBundle::from_transform(Transform::from_translation(chunk_position)),
                            // Add component to mark visualization type
                            ChunkVisualization { state: managed_chunk.state },
                        )).id();
                        managed_chunk.entity = Some(entity);
                    }
                }
                // If entity exists but mesh needs update (e.g., state changed), handle here
                // else { ... }
            } else {
                // Not visible - remove entity if it exists
                if let Some(entity) = managed_chunk.entity.take() {
                    commands.entity(entity).despawn_recursive();
                }
            }
        }
    }

    // Removed debug printing
    // if active_count > 0 { ... }
}

// Component to track chunk visualization type
#[derive(Component)]
struct ChunkVisualization {
    state: ChunkState,
}

// Simple frustum culling helper
fn is_sphere_in_frustum(camera_transform: &Transform, sphere_center: Vec3, sphere_radius: f32) -> bool {
    // Get camera forward direction
    let forward = camera_transform.forward().as_vec3(); // Convert Dir3 to Vec3
    let camera_pos = camera_transform.translation;

    // Simple check - is the sphere behind the camera?
    let to_sphere = sphere_center - camera_pos;
    let distance_along_view = to_sphere.dot(forward);

    // If sphere is behind camera plane, it's not visible
    if distance_along_view < -sphere_radius {
        return false;
    }

    // Simplified frustum check - treat camera as having 90-degree FOV
    // and check if sphere center is too far from view axis
    // Use a slightly wider angle check
    let max_angle_cos = 0.4; // Cosine of ~66 degrees half-angle
    let view_dist = distance_along_view.max(0.1);
    let perpendicular_dist_sq = (to_sphere - forward * distance_along_view).length_squared();

    // Check if the sphere is outside the view cone
    // Compare squared distances to avoid sqrt
    let max_perp_dist_sq = (view_dist / max_angle_cos).powi(2) - view_dist.powi(2);
    if perpendicular_dist_sq > max_perp_dist_sq + sphere_radius.powi(2) + 2.0 * sphere_radius * max_perp_dist_sq.sqrt() {
         return false;
    }


    // Otherwise, it's potentially visible
    true
}
