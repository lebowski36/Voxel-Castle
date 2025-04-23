use bevy::prelude::*;
use bevy::render::camera::ClearColor;

// Import the plugins module and the main plugin group
mod plugins;
use plugins::VoxelFortressPlugins;

// Import additional modules
pub mod chunk;
pub mod chunk_loading;
pub mod terrain;
pub mod mesh_gen;
pub mod renderer;
pub mod systems;

fn main() {
    App::new()
        // Add DefaultPlugins first
        .add_plugins(DefaultPlugins)
        // Add our custom plugins
        .add_plugins(VoxelFortressPlugins)
        // Set clear color (background color when nothing else is rendered)
        .insert_resource(ClearColor(Color::srgb(0.5, 0.7, 1.0)))
        .run();
}