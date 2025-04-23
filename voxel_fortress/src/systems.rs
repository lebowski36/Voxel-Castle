use bevy::prelude::*;
use crate::chunk::ChunkManager;

#[derive(Component)]
pub struct LoadingBar;

pub fn loading_progress_ui(
    chunk_manager: Res<ChunkManager>,
    mut query: Query<&mut BackgroundColor, With<LoadingBar>>,
) {
    // Ensure total is not zero to prevent division by zero
    let total = ((chunk_manager.radius as i64 * 2 + 1).pow(3)) as usize;
    if total == 0 { return; }

    let loaded = chunk_manager.loaded_chunks.len();
    let percent = (loaded as f32 / total as f32).min(1.0);
    for mut background_color in query.iter_mut() {
        // Use srgb for color consistency
        background_color.0 = Color::srgb(percent, 0.5, 0.5);
    }
}