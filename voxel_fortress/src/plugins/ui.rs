use bevy::prelude::*;
use bevy::diagnostic::{DiagnosticsStore, FrameTimeDiagnosticsPlugin};
use crate::chunk::ChunkManager;

// Game states for game flow management
#[derive(States, Debug, Clone, PartialEq, Eq, Hash, Default)]
pub enum GameState {
    #[default]
    Loading,
    Playing,
}

// UI Components
#[derive(Component)]
pub struct LoadingScreen;

// Component to mark the loading text specifically
#[derive(Component)]
pub struct LoadingText;

#[derive(Component)]
pub struct DebugOverlay;

// Component to mark the debug text specifically
#[derive(Component)]
pub struct DebugText;


/// Plugin for managing UI elements
pub struct UiPlugin;

impl Plugin for UiPlugin {
    fn build(&self, app: &mut App) {
        app
            .init_state::<GameState>()
            .add_systems(Startup, setup_loading_ui)
            // Ensure update_loading_progress runs only during Loading state
            .add_systems(Update, update_loading_progress.run_if(in_state(GameState::Loading)))
            .add_systems(OnEnter(GameState::Playing), setup_debug_ui)
            .add_systems(Update, update_debug_info.run_if(in_state(GameState::Playing)));
    }
}

/// Set up the loading UI
fn setup_loading_ui(mut commands: Commands) {
    commands
        .spawn((
            NodeBundle { // Use NodeBundle directly
                style: Style { // Style is part of NodeBundle now
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    justify_content: JustifyContent::Center,
                    align_items: AlignItems::Center,
                    flex_direction: FlexDirection::Column,
                    ..default()
                },
                background_color: Color::BLACK.into(),
                ..default()
            },
            LoadingScreen,
        ))
        .with_children(|parent| {
            // Title
            parent.spawn(TextBundle::from_section( // Use TextBundle::from_section
                "Voxel Fortress",
                TextStyle { // TextStyle is used within TextBundle::from_section
                    font_size: 60.0,
                    color: Color::WHITE,
                    ..default()
                },
            ));

            // Loading text with percentage
            parent.spawn(( // Spawn tuple with marker component
                TextBundle::from_section(
                    "Loading World... 0%",
                    TextStyle {
                        font_size: 24.0,
                        color: Color::WHITE,
                        ..default()
                    },
                ),
                LoadingText, // Add marker component
            ));
        });
}

/// Update the loading progress
fn update_loading_progress(
    mut commands: Commands,
    chunk_manager: Res<ChunkManager>,
    loading_screen_query: Query<Entity, With<LoadingScreen>>, // Renamed for clarity
    // Query specifically for the LoadingText component
    mut loading_text_query: Query<&mut Text, With<LoadingText>>,
    mut next_state: ResMut<NextState<GameState>>,
) {
    let total_chunks = ((chunk_manager.radius as i64 * 2 + 1).pow(3)) as usize;
    // Ensure total_chunks is not zero to avoid division by zero
    if total_chunks == 0 { return; }

    let loaded_chunks = chunk_manager.loaded_chunks.len();
    let progress = (loaded_chunks as f32 / total_chunks as f32 * 100.0).min(100.0);

    // Update loading text - Text now has sections field directly
    for mut text in loading_text_query.iter_mut() {
        // Check if sections exist before accessing
        if !text.sections.is_empty() {
             text.sections[0].value = format!("Loading World... {:.0}%", progress);
        }
    }

    // Transition to playing state when loading is complete
    // Use >= 100.0 for safer comparison
    if progress >= 100.0 {
        // First remove loading screen
        if let Ok(entity) = loading_screen_query.get_single() {
             commands.entity(entity).despawn_recursive();
        }

        // Then transition to playing state
        next_state.set(GameState::Playing);
    }
}


/// Set up the debug UI overlay
fn setup_debug_ui(mut commands: Commands) {
    commands
        .spawn((
            NodeBundle {
                style: Style {
                    position_type: PositionType::Absolute, // Use Absolute for overlay
                    left: Val::Px(10.0),
                    top: Val::Px(10.0),
                    padding: UiRect::all(Val::Px(5.0)),
                    // Consider adding width/height or using AlignSelf::FlexStart
                    ..default()
                },
                // Use srgba for transparent colors
                background_color: Color::srgba(0.0, 0.0, 0.0, 0.5).into(),
                ..default()
            },
            DebugOverlay,
        ))
        .with_children(|parent| {
            parent.spawn(( // Spawn tuple with marker component
                TextBundle::from_section(
                    "FPS: --
Chunks: Active=0 LOD=0 Unloaded=0 Total=0",
                    TextStyle {
                        font_size: 16.0,
                        color: Color::WHITE,
                        ..default()
                    },
                ),
                DebugText, // Add marker component
            ));
        });
}

/// Update debug information display
fn update_debug_info(
    // Query specifically for the DebugText component
    mut debug_text_query: Query<&mut Text, With<DebugText>>,
    diagnostics: Res<DiagnosticsStore>,
    chunk_manager: Res<ChunkManager>,
) {
    // Update debug text - Text now has sections field directly
    for mut text in debug_text_query.iter_mut() {
         // Check if sections exist before accessing
        if !text.sections.is_empty() {
            // Count chunks by state
            let active = chunk_manager.loaded_chunks.values()
                .filter(|c| c.state == crate::chunk::ChunkState::Active)
                .count();

            let lod = chunk_manager.loaded_chunks.values()
                .filter(|c| c.state == crate::chunk::ChunkState::LOD)
                .count();

            let unloaded = chunk_manager.loaded_chunks.values()
                .filter(|c| c.state == crate::chunk::ChunkState::Unloaded)
                .count();

            let total = chunk_manager.loaded_chunks.len(); // Simpler total count

            // Get FPS
            let fps = diagnostics.get(&FrameTimeDiagnosticsPlugin::FPS)
                .and_then(|fps| fps.smoothed())
                .unwrap_or(0.0);

            // Update debug text
            text.sections[0].value = format!(
                "FPS: {:.1}
Chunks: Active={} LOD={} Unloaded={} Total={}",
                fps, active, lod, unloaded, total
            );
        }
    }
}
