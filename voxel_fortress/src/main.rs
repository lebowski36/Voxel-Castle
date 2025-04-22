use bevy::core_pipeline::core_3d::{Camera3dDepthLoadOp, Camera3dDepthTextureUsage};
use bevy::input::keyboard::KeyCode;
use bevy::input::ButtonInput;
use bevy::prelude::*;
use bevy::render::mesh::{Indices, Mesh, PrimitiveTopology};
use bevy::render::render_asset::RenderAssetUsages;
use bevy::render::render_resource::TextureUsages;
use bevy::render::camera::ClearColor;
use bevy::ecs::system::Resource;
use bevy::window::PrimaryWindow;

mod terrain;
use terrain::{CHUNK_SIZE, VOXEL_SIZE_METERS, Chunk, Voxel, ChunkNode, ChunkSummary, WorldGen};
use std::collections::HashMap;

/// ECS resource: Manages loaded chunks and hierarchical worldgen
#[derive(Resource)]
pub struct ChunkManager {
    pub loaded_chunks: HashMap<(i64, i64, i64), Chunk>,
    pub high_level_nodes: HashMap<(i64, i64, i64), ChunkNode>,
    pub radius: i32, // Loaded chunk radius
}

impl ChunkManager {
    pub fn new(radius: i32) -> Self {
        Self {
            loaded_chunks: HashMap::new(),
            high_level_nodes: HashMap::new(),
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
                        self.loaded_chunks.insert(pos, chunk);
                    }
                }
            }
        }
    }
    // TODO: Add unload logic, LOD, async, etc.
}

fn main() {
    App::new()
        .add_plugins(DefaultPlugins)
        .insert_resource(ClearColor(Color::rgb(0.5, 0.7, 1.0)))
        .insert_resource(ChunkManager::new(5))
        .add_systems(Startup, setup)
        .add_systems(Update, camera_movement)
        .add_systems(Update, camera_mouse_look)
        .add_systems(Update, loading_progress_ui)
        .run();
}

fn camera_mouse_look(
    mut mouse_motion_events: EventReader<bevy::input::mouse::MouseMotion>,
    mouse_button_input: Res<ButtonInput<bevy::input::mouse::MouseButton>>,
    mut query: Query<&mut Transform, With<Camera3d>>,
) {
    if !mouse_button_input.pressed(bevy::input::mouse::MouseButton::Right) {
        return;
    }
    let mut delta = Vec2::ZERO;
    for event in mouse_motion_events.read() {
        delta += event.delta;
    }
    if delta == Vec2::ZERO {
        return;
    }
    for mut transform in query.iter_mut() {
        // Sensitivity can be adjusted as needed
        let sensitivity = 0.002;
        let yaw = -delta.x * sensitivity;
        let pitch = -delta.y * sensitivity;
        // Apply yaw (around global Y)
        transform.rotate(Quat::from_rotation_y(yaw));
        // Apply pitch (around local X)
        let right = transform.rotation * Vec3::X;
        transform.rotate(Quat::from_axis_angle(right, pitch));
    }
}

fn camera_movement(
    keyboard_input: Res<ButtonInput<KeyCode>>,
    mut query: Query<&mut Transform, With<Camera3d>>,
    time: Res<Time>,
) {
    for mut transform in query.iter_mut() {
        let mut direction = Vec3::ZERO;

        // Movement controls
        if keyboard_input.pressed(KeyCode::KeyW) {
            direction.z -= 1.0;
        }
        if keyboard_input.pressed(KeyCode::KeyS) {
            direction.z += 1.0;
        }
        if keyboard_input.pressed(KeyCode::KeyA) {
            direction.x -= 1.0;
        }
        if keyboard_input.pressed(KeyCode::KeyD) {
            direction.x += 1.0;
        }
        if keyboard_input.pressed(KeyCode::KeyE) {
            direction.y += 1.0;
        }
        if keyboard_input.pressed(KeyCode::KeyQ) {
            direction.y -= 1.0;
        }

        // Apply movement
        transform.translation += direction * time.delta_secs() * 5.0;

        // Rotation controls
        if keyboard_input.pressed(KeyCode::ArrowLeft) {
            transform.rotate(Quat::from_rotation_y(0.05));
        }
        if keyboard_input.pressed(KeyCode::ArrowRight) {
            transform.rotate(Quat::from_rotation_y(-0.05));
        }
        if keyboard_input.pressed(KeyCode::ArrowUp) {
            let right = transform.rotation * Vec3::X;
            transform.rotate(Quat::from_axis_angle(right, 0.05));
        }
        if keyboard_input.pressed(KeyCode::ArrowDown) {
            let right = transform.rotation * Vec3::X;
            transform.rotate(Quat::from_axis_angle(right, -0.05));
        }
    }
}

fn generate_plane_mesh(size: f32) -> Mesh {
    let half = size / 2.0;
    let vertices = vec![
        [-half, 0.0, -half],
        [half, 0.0, -half],
        [half, 0.0, half],
        [-half, 0.0, half],
    ];

    let indices = Indices::U32(vec![0, 1, 2, 2, 3, 0]);

    let normals = vec![[0.0, 1.0, 0.0]; 4];

    let mut mesh = Mesh::new(PrimitiveTopology::TriangleList, RenderAssetUsages::default());
    mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, vertices);
    mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
    mesh.insert_indices(indices);
    mesh
}

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut chunk_manager: ResMut<ChunkManager>,
) {
    // --- WorldGen instance ---
    let worldgen = WorldGen::new(3, CHUNK_SIZE, 42);
    chunk_manager.loaded_chunks.clear();
    chunk_manager.load_chunks_around((0, 0, 0), &worldgen);
    // Find the highest ground in the center chunk for player spawn
    let center_chunk = chunk_manager.loaded_chunks.get(&(0, 0, 0)).unwrap();
    let mut max_ground_y = 0.0;
    for x in 0..CHUNK_SIZE {
        for z in 0..CHUNK_SIZE {
            for y in (0..CHUNK_SIZE).rev() {
                if center_chunk.voxels[x][y][z] == Voxel::Solid {
                    let world_y = y as f32 * VOXEL_SIZE_METERS;
                    if world_y > max_ground_y {
                        max_ground_y = world_y;
                    }
                    break;
                }
            }
        }
    }
    // Add a camera above the ground
    commands.spawn((
        Camera3dBundle {
            transform: Transform::from_xyz(0.0, max_ground_y + 3.0, 0.0).looking_at(Vec3::new(0.0, max_ground_y, 0.0), Vec3::Y),
            ..default()
        },
    ));

    // --- Loading Progress UI ---
    commands.spawn((
        NodeBundle {
            style: Style {
                size: Size::width(Val::Percent(100.0)),
                position_type: PositionType::Absolute,
                position: UiRect { top: Val::Px(20.0), left: Val::Px(0.0), ..default() },
                justify_content: JustifyContent::Center,
                ..default()
            },
            background_color: Color::NONE.into(),
            ..default()
        },
        LoadingBar,
    ));

    let chunk_material = materials.add(StandardMaterial {
        base_color: Color::WHITE,
        ..default()
    });
    for (&(cx, cy, cz), chunk) in &chunk_manager.loaded_chunks {
        let chunk_mesh = meshes.add(chunk.to_mesh());
        commands.spawn((
            Mesh3d(chunk_mesh),
            MeshMaterial3d(chunk_material.clone()),
            Transform::from_xyz(
                cx as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS,
                cy as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS,
                cz as f32 * CHUNK_SIZE as f32 * VOXEL_SIZE_METERS,
            ),
        ));
    }

    // Add a light source
    commands.spawn((
        PointLight {
            intensity: 1500.0,
            shadows_enabled: true,
            ..default()
        },
        Transform::from_xyz(4.0, 8.0, 4.0),
    ));
}

#[derive(Component)]
struct LoadingBar;

fn loading_progress_ui(
    chunk_manager: Res<ChunkManager>,
    mut query: Query<&mut Style, With<LoadingBar>>,
    windows: Query<&Window, With<PrimaryWindow>>,
) {
    let total = ((chunk_manager.radius as i64 * 2 + 1).pow(3)) as usize;
    let loaded = chunk_manager.loaded_chunks.len();
    let percent = (loaded as f32 / total as f32).min(1.0);
    for mut style in query.iter_mut() {
        style.size.width = Val::Percent(percent * 100.0);
    }
}

fn generate_cube_mesh(size: f32) -> Mesh {
    let half = size / 2.0;
    let vertices = vec![
        // Front face
        [-half, -half, half],
        [half, -half, half],
        [half, half, half],
        [-half, half, half],
        // Back face
        [-half, -half, -half],
        [half, -half, -half],
        [half, half, -half],
        [-half, half, -half],
    ];

    let indices = Indices::U32(vec![
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        0, 3, 7, 7, 4, 0,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        0, 4, 5, 5, 1, 0,
    ]);

    let normals = vec![
        [0.0, 0.0, 1.0], // Front
        [0.0, 0.0, -1.0], // Back
        [-1.0, 0.0, 0.0], // Left
        [1.0, 0.0, 0.0], // Right
        [0.0, 1.0, 0.0], // Top
        [0.0, -1.0, 0.0], // Bottom
    ];

    let mut mesh = Mesh::new(PrimitiveTopology::TriangleList, RenderAssetUsages::default());
    mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, vertices);
    mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
    mesh.insert_indices(indices);
    mesh
}
