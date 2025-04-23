use bevy::prelude::*;
use bevy::diagnostic::{DiagnosticsStore, FrameTimeDiagnosticsPlugin};
use crate::chunk::ChunkManager;
use bevy::ui::{Style, JustifyContent, AlignItems, PositionType, FlexDirection, BackgroundColor};
use bevy::text::{Text, TextStyle, TextSection};
use bevy::ui::Val;

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
fn setup_loading_ui(mut commands: Commands, asset_server: Res<AssetServer>) {
    let root = commands
        .spawn((
            // Use NodeBundle for modern Bevy UI
            NodeBundle {
                style: Style {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    justify_content: JustifyContent::Center,
                    align_items: AlignItems::Center,
                    flex_direction: FlexDirection::Column,
                    ..default()
                },
                background_color: BackgroundColor(Color::BLACK),
                ..default()
            },
            LoadingScreen,
        ))
        .id();

    commands.entity(root).with_children(|parent| {
        // Title text
        parent.spawn((
            TextBundle {
                text: Text::from_section(
                    "Voxel Fortress",
                    TextStyle {
                        font: asset_server.load("fonts/FiraSans-Bold.ttf"),
                        font_size: 60.0,
                        color: Color::WHITE,
                    },
                ),
                ..default()
            },
        ));
        
        // Loading progress text
        parent.spawn((
            TextBundle {
                text: Text::from_section(
                    "Loading World... 0%",
                    TextStyle {
                        font: asset_server.load("fonts/FiraSans-Regular.ttf"),
                        font_size: 20.0,
                        color: Color::WHITE,
                    },
                ),
                ..default()
            },
            LoadingText,
        ));
    });
}

fn update_loading_progress(
    mut commands: Commands,
    chunk_manager: Res<ChunkManager>,
    loading_screen_query: Query<Entity, With<LoadingScreen>>,
    mut loading_text_query: Query<&mut Text, With<LoadingText>>,
    mut next_state: ResMut<NextState<GameState>>,
) {
    let total_chunks = ((chunk_manager.radius as i64 * 2 + 1).pow(3)) as usize;
    if total_chunks == 0 { return; }

    let loaded_chunks = chunk_manager.loaded_chunks.len();
    let progress = (loaded_chunks as f32 / total_chunks as f32 * 100.0).min(100.0);

    for mut text in loading_text_query.iter_mut() {
        // Update the text with the progress
        *text = Text::from_section(
            format!("Loading World... {:.0}%", progress),
            text.sections[0].style.clone()
        );
    }

    if progress >= 100.0 {
        if let Ok(entity) = loading_screen_query.get_single() {
            commands.entity(entity).despawn_recursive();
        }
        next_state.set(GameState::Playing);
    }
}

fn setup_debug_ui(mut commands: Commands, asset_server: Res<AssetServer>) {
    let root = commands
        .spawn((
            // Use NodeBundle for modern Bevy UI
            NodeBundle {
                style: Style {
                    position_type: PositionType::Absolute,
                    left: Val::Px(10.0),
                    top: Val::Px(10.0),
                    padding: UiRect::all(Val::Px(5.0)),
                    ..default()
                },
                background_color: BackgroundColor(Color::srgba(0.0, 0.0, 0.0, 0.5)),
                ..default()
            },
            DebugOverlay,
        ))
        .id();
    commands.entity(root).with_children(|parent| {
        parent
            .spawn((
                TextBundle {
                    text: Text::from_section(
                        "FPS: --\nChunks: Active=0 LOD=0 Unloaded=0 Total=0",
                        TextStyle {
                            font: asset_server.load("fonts/FiraSans-Regular.ttf"),
                            font_size: 16.0,
                            color: Color::WHITE,
                        },
                    ),
                    ..default()
                },
                DebugText,
            ));
    });
}

fn update_debug_info(
    mut debug_text_query: Query<&mut Text, With<DebugText>>,
    diagnostics: Res<DiagnosticsStore>,
    chunk_manager: Res<ChunkManager>,
) {
    for mut text in debug_text_query.iter_mut() {
        let active = chunk_manager.loaded_chunks.values()
            .filter(|c| c.state == crate::chunk::ChunkState::Active)
            .count();
        let lod = chunk_manager.loaded_chunks.values()
            .filter(|c| c.state == crate::chunk::ChunkState::LOD)
            .count();
        let unloaded = chunk_manager.loaded_chunks.values()
            .filter(|c| c.state == crate::chunk::ChunkState::Unloaded)
            .count();
        let total = chunk_manager.loaded_chunks.len();
        let fps = diagnostics.get(&FrameTimeDiagnosticsPlugin::FPS)
            .and_then(|fps| fps.smoothed())
            .unwrap_or(0.0);
        
        // Create a new Text component with the updated info
        // Preserve the existing style from the first section if available
        let style = if !text.sections.is_empty() {
            text.sections[0].style.clone()
        } else {
            TextStyle {
                font_size: 16.0,
                color: Color::WHITE,
                ..default()
            }
        };
        
        // Replace the entire Text component with updated content
        *text = Text::from_section(
            format!(
                "FPS: {:.1}\nChunks: Active={} LOD={} Unloaded={} Total={}",
                fps, active, lod, unloaded, total
            ),
            style
        );
    }
}
