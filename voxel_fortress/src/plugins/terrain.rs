use bevy::prelude::*;
use crate::chunk::update_chunk_lod_system;

/// Plugin for handling terrain generation and management
pub struct TerrainPlugin;

impl Plugin for TerrainPlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(Update, update_chunk_lod_system);
    }
}
