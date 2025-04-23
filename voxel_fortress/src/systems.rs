use bevy::prelude::*;
use crate::chunk::ChunkManager;
use crate::terrain::{CHUNK_SIZE, VOXEL_SIZE_METERS};

#[derive(Component)]
pub struct LoadingBar;

// Resource to store the chunk material handle
#[derive(Resource, Clone)]
pub struct ChunkMaterialHandle(pub Handle<StandardMaterial>);

pub fn loading_progress_ui(
    chunk_manager: Res<ChunkManager>,
    mut query: Query<&mut BackgroundColor, With<LoadingBar>>,
) {
    let total = ((chunk_manager.radius as i64 * 2 + 1).pow(3)) as usize;
    let loaded = chunk_manager.loaded_chunks.len();
    let percent = (loaded as f32 / total as f32).min(1.0);
    for mut background_color in query.iter_mut() {
        background_color.0 = Color::srgb(percent, 0.5, 0.5);
    }
}

// Setup function to create and store the material handle as a resource
pub fn setup_chunk_material(
    mut commands: Commands,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    let handle = materials.add(StandardMaterial::default());
    commands.insert_resource(ChunkMaterialHandle(handle));
}

pub fn update_chunk_entities_system(
    mut commands: Commands,
    mut chunk_manager: ResMut<crate::chunk::ChunkManager>,
    mut meshes: ResMut<Assets<Mesh>>,
    chunk_material: Res<ChunkMaterialHandle>,
) {
    for (pos, managed_chunk) in chunk_manager.loaded_chunks.iter_mut() {
        let should_have_entity = match managed_chunk.state {
            crate::chunk::ChunkState::Active => managed_chunk.chunk.is_some(),
            crate::chunk::ChunkState::LOD => managed_chunk.lod_mesh.is_some(),
            crate::chunk::ChunkState::Unloaded => false,
        };
        if should_have_entity && managed_chunk.entity.is_none() {
            let mesh_handle = match managed_chunk.state {
                crate::chunk::ChunkState::Active => managed_chunk.chunk.as_ref().map(|c| meshes.add(c.to_mesh())),
                crate::chunk::ChunkState::LOD => managed_chunk.lod_mesh.clone(),
                _ => None,
            };
            if let Some(mesh) = mesh_handle {
                let entity = commands.spawn((
                    Mesh3d(mesh),
                    MeshMaterial3d(chunk_material.0.clone()),
                    Transform::from_xyz(
                        pos.0 as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS,
                        pos.1 as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS,
                        pos.2 as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS,
                    ),
                )).id();
                managed_chunk.entity = Some(entity);
            }
        }
        if !should_have_entity {
            if let Some(entity) = managed_chunk.entity.take() {
                commands.entity(entity).despawn_recursive();
            }
        }
    }
}