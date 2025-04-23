use bevy::prelude::*;
use bevy::diagnostic::{DiagnosticsStore, FrameTimeDiagnosticsPlugin};
use crate::chunk::ChunkManager;
use crate::GameState;

/// Plugin for managing UI elements in Voxel Fortress
pub struct UiPlugin;

// Components for UI elements
#[derive(Component)]
pub struct LoadingScreen;

#[derive(Component)]
pub struct LoadingBar;

#[derive(Component)]
pub struct LoadingText;

#[derive(Component)]
pub struct DebugText;

#[derive(Component)]
pub struct DebugOverlay;

impl Plugin for UiPlugin {
    fn build(&self, app: &mut App) {
        app
            // Loading screen systems
            .add_systems(OnEnter(GameState::Loading), setup_loading_screen)
            .add_systems(Update, update_loading_screen.run_if(in_state(GameState::Loading)))
            .add_systems(OnExit(GameState::Loading), cleanup_loading_screen)
            
            // Debug overlay systems
            .add_systems(OnEnter(GameState::Playing), setup_debug_overlay)
            .add_systems(Update, update_debug_text.run_if(in_state(GameState::Playing)));
    }
}

/// Set up the loading screen UI
fn setup_loading_screen(mut commands: Commands, asset_server: Res<AssetServer>) {
    // Root node covering the whole screen
    commands
        .spawn((
            NodeBundle {
                style: Style {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    flex_direction: FlexDirection::Column,
                    align_items: AlignItems::Center,
                    justify_content: JustifyContent::Center,
                    ..default()
                },
                background_color: Color::BLACK.into(),
                ..default()
            },
            LoadingScreen,
        ))
        .with_children(|parent| {
            // Game title
            parent.spawn(
                TextBundle::from_section(
                    "Voxel Fortress",
                    TextStyle {
                        font_size: 60.0,
                        color: Color::WHITE,
                        ..default()
                    },
                )
                .with_style(Style {
                    margin: UiRect::all(Val::Px(20.0)),
                    ..default()
                }),
            );

            // Loading text
            parent.spawn((
                TextBundle::from_section(
                    "Loading world...",
                    TextStyle {
                        font_size: 30.0,
                        color: Color::WHITE,
                        ..default()
                    },
                )
                .with_style(Style {
                    margin: UiRect::all(Val::Px(20.0)),
                    ..default()
                }),
                LoadingText,
            ));

            // Progress bar container
            parent
                .spawn(NodeBundle {
                    style: Style {
                        width: Val::Px(400.0),
                        height: Val::Px(30.0),
                        border: UiRect::all(Val::Px(2.0)),
                        margin: UiRect::all(Val::Px(10.0)),
                        ..default()
                    },
                    border_color: BorderColor(Color::WHITE),
                    background_color: Color::rgba(0.1, 0.1, 0.1, 0.5).into(),
                    ..default()
                })
                .with_children(|parent| {
                    // Progress bar fill
                    parent.spawn((
                        NodeBundle {
                            style: Style {
                                width: Val::Percent(0.0),
                                height: Val::Percent(100.0),
                                ..default()
                            },
                            background_color: Color::rgb(0.2, 0.7, 0.2).into(),
                            ..default()
                        },
                        LoadingBar,
                    ));
                });
        });
}

/// Update the loading progress UI based on chunk loading status
fn update_loading_screen(
    chunk_manager: Res<ChunkManager>,
    mut loading_bar_query: Query<&mut Style, With<LoadingBar>>,
    mut loading_text_query: Query<&mut Text, With<LoadingText>>,
    mut next_state: ResMut<NextState<GameState>>,
) {
    let total = ((chunk_manager.radius as i64 * 2 + 1).pow(3)) as usize;
    let loaded = chunk_manager.loaded_chunks.len();
    let percent = (loaded as f32 / total as f32).min(1.0);
    
    // Update loading bar width
    if let Ok(mut style) = loading_bar_query.get_single_mut() {
        style.width = Val::Percent(percent * 100.0);
    }
    
    // Update loading text
    if let Ok(mut text) = loading_text_query.get_single_mut() {
        text.sections[0].value = format!("Loading world... {:.0}%", percent * 100.0);
    }
    
    // Transition to playing state when loading is complete 
    // (with a small buffer to ensure everything is loaded)
    if percent > 0.98 {
        next_state.set(GameState::Playing);
    }
}

/// Clean up the loading screen when transitioning to gameplay
fn cleanup_loading_screen(mut commands: Commands, query: Query<Entity, With<LoadingScreen>>) {
    for entity in &query {
        commands.entity(entity).despawn_recursive();
    }
}

/// Set up the debug overlay UI
fn setup_debug_overlay(mut commands: Commands) {
    commands
        .spawn((
            NodeBundle {
                style: Style {
                    position_type: PositionType::Absolute,
                    top: Val::Px(10.0),
                    left: Val::Px(10.0),
                    padding: UiRect::all(Val::Px(10.0)),
                    ..default()
                },
                background_color: Color::rgba(0.0, 0.0, 0.0, 0.5).into(),
                ..default()
            },
            DebugOverlay,
        ))
        .with_children(|parent| {
            parent.spawn((
                TextBundle::from_sections([
                    TextSection::new(
                        "FPS: --\nChunks: --/--/--\n",
                        TextStyle {
                            font_size: 16.0,
                            color: Color::WHITE,
                            ..default()
                        },
                    ),
                ]),
                DebugText,
            ));
        });
}

/// Update the debug text with current game stats
fn update_debug_text(
    mut query: Query<&mut Text, With<DebugText>>,
    diagnostics: Res<DiagnosticsStore>,
    chunk_manager: Res<ChunkManager>,
) {
    for mut text in &mut query {
        let active_chunks = chunk_manager.loaded_chunks.values()
            .filter(|c| c.state == crate::chunk::ChunkState::Active)
            .count();
            
        let lod_chunks = chunk_manager.loaded_chunks.values()
            .filter(|c| c.state == crate::chunk::ChunkState::LOD)
            .count();
            
        let unloaded_chunks = chunk_manager.loaded_chunks.values()
            .filter(|c| c.state == crate::chunk::ChunkState::Unloaded)
            .count();
            
        let total_chunks = active_chunks + lod_chunks + unloaded_chunks;
        
        // Get FPS value from diagnostics if available
        let mut fps = 0.0;
        if let Some(fps_diagnostic) = diagnostics.get(FrameTimeDiagnosticsPlugin::FPS) {
            if let Some(fps_value) = fps_diagnostic.smoothed() {
                fps = fps_value;
            }
        }
        
        // Update the text
        text.sections[0].value = format!(
            "FPS: {:.1}\nChunks: Active={}, LOD={}, Unloaded={}, Total={}\n",
            fps, active_chunks, lod_chunks, unloaded_chunks, total_chunks
        );
    }
}
