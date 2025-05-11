use bevy::prelude::*;

mod camera;
mod world;
mod ui;
mod terrain;

pub use camera::CameraPlugin;
pub use world::WorldPlugin;
// Re-export GameState and UiPlugin from ui module
pub use ui::{UiPlugin, GameState};
pub use terrain::TerrainPlugin;

/// Consolidated plugin collection for the Voxel Fortress game
pub struct VoxelFortressPlugins;

impl Plugin for VoxelFortressPlugins {
    fn build(&self, app: &mut App) {
        app.add_plugins((
            CameraPlugin,
            WorldPlugin,
            UiPlugin,
            TerrainPlugin,
        ));
    }
}
