use bevy::prelude::*;
use bevy::input::keyboard::KeyCode;
use bevy::input::ButtonInput;
use bevy::input::mouse::MouseMotion;

/// Marker component for the primary player camera
#[derive(Component)]
pub struct PlayerCamera;

/// Plugin handling camera systems and controls
pub struct CameraPlugin;

impl Plugin for CameraPlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(Startup, setup_camera)
           .add_systems(Update, (
               camera_movement,
               camera_mouse_look,
           ));
    }
}

/// Setup the camera system for the world
pub fn setup_camera(
    mut commands: Commands,
    // We don't have chunk data here since this is only initial setup
) {
    // For now just add a camera at a reasonable position
    // The camera height will be adjusted by the world plugin after terrain generation
    commands.spawn((
        Camera3d::default(),
        // Add the marker component
        PlayerCamera,
        Transform::from_xyz(0.0, 10.0, 0.0).looking_at(Vec3::new(5.0, 0.0, 5.0), Vec3::Y),
    ));
}

fn camera_mouse_look(
    mut mouse_motion_events: EventReader<MouseMotion>,
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
