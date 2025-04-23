use bevy::input::keyboard::KeyCode;
use bevy::input::ButtonInput;
use bevy::prelude::*;
use bevy::render::camera::ClearColor;

mod chunk;
mod systems;

use chunk::*;
use systems::*;

mod terrain;
use terrain::{CHUNK_SIZE, VOXEL_SIZE_METERS, Voxel, WorldGen};

#[derive(Resource, Clone)]
pub struct ChunkMaterialHandle(pub Handle<StandardMaterial>);

fn main() {
    App::new()
        .add_plugins(DefaultPlugins)
        .insert_resource(ClearColor(Color::srgb(0.5, 0.7, 1.0)))
        .insert_resource(ChunkManager::new(5))
        .add_systems(Startup, setup_chunk_material)
        .add_systems(Startup, setup)
        .add_systems(Update, camera_movement)
        .add_systems(Update, camera_mouse_look)
        .add_systems(Update, systems::loading_progress_ui)
        .add_systems(Update, chunk::update_chunk_lod_system)
        .add_systems(Update, systems::update_chunk_entities_system)
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

        // Apply movement relative to camera orientation
        if direction.length_squared() > 0.0 {
            // Only use X and Z for planar movement
            let forward = transform.forward();
            let right = transform.right();
            let up = Vec3::Y;
            let mut move_vec = Vec3::ZERO;
            move_vec += forward * -direction.z; // Invert Z so W is forward
            move_vec += right * direction.x;
            move_vec += up * direction.y;
            transform.translation += move_vec.normalize() * time.delta_secs() * 5.0;
        }

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

fn setup(
    mut commands: Commands,
    mut chunk_manager: ResMut<ChunkManager>,
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
    // Add a camera above the ground
    commands.spawn((
        Camera3d::default(),
        Transform::from_xyz(0.0, max_ground_y + 3.0, 0.0).looking_at(Vec3::new(0.0, max_ground_y, 0.0), Vec3::Y),
    ));

    // --- Loading Progress UI ---
    commands
        .spawn((
            Node {
                // Initialize required fields for the Node struct
                ..Default::default()
            },
            Transform::default(),
            GlobalTransform::default(),
            BackgroundColor(Color::srgb(0.5, 0.5, 0.5)),
            LoadingBar,
        ));
}

fn setup_chunk_material(
    mut commands: Commands,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    let chunk_material_handle = materials.add(StandardMaterial {
        base_color: Color::srgb(0.7, 0.7, 0.7),
        perceptual_roughness: 0.9,
        ..default()
    });
    commands.insert_resource(ChunkMaterialHandle(chunk_material_handle.clone()));
}