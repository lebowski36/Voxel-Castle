// filepath: /home/system-x1/Projects/Voxel Castle/voxel_fortress/src/plugins/ui.rs
use bevy::prelude::*;

/// Game states to control application flow
#[derive(Debug, Clone, Copy, Default, Eq, PartialEq, Hash, States)]
pub enum GameState {
    #[default]
    Loading,
    Playing,
    Paused,
}

/// UI plugin for Voxel Fortress
pub struct UiPlugin;

impl Plugin for UiPlugin {
    fn build(&self, app: &mut App) {
        app.init_state::<GameState>()
           .insert_resource(DelayedStateTransition {
               timer: Timer::from_seconds(2.0, TimerMode::Once),
               target_state: GameState::Playing,
           })
           .add_systems(Update, check_delayed_transition);
    }
}

// Resource to hold the timer for state transition
#[derive(Resource)]
struct DelayedStateTransition {
    timer: Timer,
    target_state: GameState,
}

// System to check if the timer is finished and transition
fn check_delayed_transition(
    time: Res<Time>,
    mut transition: ResMut<DelayedStateTransition>,
    mut next_state: ResMut<NextState<GameState>>,
) {
    transition.timer.tick(time.delta());
    
    if transition.timer.finished() {
        next_state.set(transition.target_state);
    }
}
