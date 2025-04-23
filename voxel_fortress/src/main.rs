use bevy::prelude::*;
use bevy::render::camera::ClearColor;
use bevy::diagnostic::{FrameTimeDiagnosticsPlugin, LogDiagnosticsPlugin};

mod chunk;
mod systems;
mod terrain;
mod plugins;
mod chunk_loading;

use plugins::VoxelFortressPlugins;

// Game states
#[derive(States, Debug, Clone, PartialEq, Eq, Hash, Default)]
pub enum GameState {
    #[default]
    Loading,
    Playing,
}

fn main() {
    App::new()
        .add_plugins(DefaultPlugins)
        // Add diagnostics for FPS monitoring
        .add_plugins(FrameTimeDiagnosticsPlugin::default())
        // Add our game plugins
        .add_plugins(VoxelFortressPlugins)
        // Add game states
        .init_state::<GameState>()
        // Set sky color
        .insert_resource(ClearColor(Color::srgb(0.5, 0.7, 1.0)))
        .run();
}
